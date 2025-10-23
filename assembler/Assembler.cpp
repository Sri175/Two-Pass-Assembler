#include "Assembler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>   // For formatting the listing file
#include <algorithm> // For std::find
#include <cctype>    // For isspace
#include <cstdlib>   // For strtol

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

Assembler::Assembler() {
    // Constructor. The opcodeTable is already initialized in Common.h.
}

bool Assembler::assemble(const std::string& inputFilename, 
                         const std::string& outputObjectFilename, 
                         const std::string& outputListFilename) {
    
    std::cout << "Starting Pass 1..." << std::endl;
    if (!performPass1(inputFilename)) {
        logError("Pass 1 failed.");
        return false;
    }
    std::cout << "Pass 1 complete. Symbol table built." << std::endl;

    std::cout << "Starting Pass 2..." << std::endl;
    if (!performPass2(outputObjectFilename, outputListFilename)) {
        logError("Pass 2 failed.");
        return false;
    }
    std::cout << "Pass 2 complete. Object and listing files generated." << std::endl;
    
    return true;
}

bool Assembler::performPass1(const std::string& inputFilename) {
    std::ifstream inFile(inputFilename);
    if (!inFile) {
        logError("Could not open input file: " + inputFilename);
        return false;
    }

    std::string line;
    int32_t locationCounter = 0; // Code starts at address zero
    int lineNumber = 0;

    programLines.clear(); // Clear any previous assembly
    symbolTable.clear();

    while (std::getline(inFile, line)) {
        lineNumber++;
        
        ParsedLine pLine = parseLine(line);
        pLine.originalLine = line; // Store original for listing

        // Ignore empty lines (comments/whitespace only)
        if (pLine.mnemonic.empty() && pLine.label.empty()) {
            continue;
        }

        pLine.address = locationCounter;
        
        // If there's a label, add it to the symbol table
        if (!pLine.label.empty()) {
            if (symbolTable.count(pLine.label)) {
                logError("Duplicate label definition: " + pLine.label, lineNumber);
                return false;
            }
            symbolTable[pLine.label] = locationCounter;
        }

        // If it's a 'SET' pseudo-instruction, handle it
        if (pLine.mnemonic == "SET") {
            if (pLine.label.empty()) {
                logError("SET instruction requires a label", lineNumber);
                return false;
            }
            // Resolve its operand *now* (it can't be a forward ref).
            bool isValid = true;
            // SET operands cannot be labels, so isBranch=false, currentPC=0 are fine
            int32_t value = resolveOperand(pLine.operandStr, 0, false, isValid);
            
            if (!isValid || symbolTable.count(pLine.operandStr)) {
                 logError("Invalid operand for SET. Must be a number.", lineNumber);
                 return false;
            }
            // Update the symbol table with the SET value
            symbolTable[pLine.label] = value;
            // Do NOT increment locationCounter and do NOT store in programLines.
            // A 'SET' instruction does not generate code.
        }
        // If there's an instruction or 'data', it takes up one 32-bit word.
        else if (!pLine.mnemonic.empty()) {
            programLines.push_back(pLine);
            locationCounter++; // Each instruction/data takes one 32-bit location
        } else {
            // This case is just a label on its own line.
            // The label is already processed, but we store the line for the listing file.
            programLines.push_back(pLine);
        }
    }

    inFile.close();
    return true;
}

bool Assembler::performPass2(const std::string& outputObjectFilename, 
                            const std::string& outputListFilename) {
    
    std::ofstream objFile(outputObjectFilename, std::ios::binary);
    if (!objFile) {
        logError("Could not open object file for writing: " + outputObjectFilename);
        return false;
    }

    std::ofstream lstFile(outputListFilename);
    if (!lstFile) {
        logError("Could not open listing file for writing: " + outputListFilename);
        return false;
    }

    // Set up formatting for the listing file
    lstFile << std::hex << std::setfill('0');

    for (const auto& pLine : programLines) {
        // Handle lines that are just labels
        if (pLine.mnemonic.empty()) {
            if (!pLine.label.empty()) {
                // Format: "start:"
                lstFile << "\n" << pLine.label << ":" << std::endl;
            }
            continue; // Nothing to write to object file
        }

        // Find the instruction in the opcode table
        auto it = opcodeTable.find(pLine.mnemonic);
        if (it == opcodeTable.end()) {
            logError("Unknown instruction: " + pLine.mnemonic);
            return false;
        }

        const OpcodeInfo& opInfo = it->second;
        int32_t operandValue = 0;
        bool operandValid = true;

        // Handle operand
        if (opInfo.expectsOperand) {
            if (pLine.operandStr.empty()) {
                logError("Missing operand for: " + pLine.mnemonic);
                return false;
            }
            
            bool isBranch = (pLine.mnemonic == "br" || pLine.mnemonic == "brz" || pLine.mnemonic == "brlz" || pLine.mnemonic == "call");
            operandValue = resolveOperand(pLine.operandStr, pLine.address, isBranch, operandValid);

            if (!operandValid) {
                logError("No such label or invalid operand: " + pLine.operandStr);
                return false;
            }
        } else {
            if (!pLine.operandStr.empty()) {
                logError("Unexpected operand for: " + pLine.mnemonic);
                return false;
            }
        }

        // Build the 32-bit machine word
        int32_t machineWord = 0;
        if (pLine.mnemonic == "data") {
            machineWord = operandValue;
        } else {
            // [operand] is upper 24 bits, [opcode] is bottom 8 bits
            machineWord = (operandValue << 8) | (opInfo.opcode & 0xFF);
        }

        // Write to object file (binary)
        objFile.write(reinterpret_cast<const char*>(&machineWord), sizeof(machineWord));

        // Write to listing file (text)
        // Format: 00000002 00006500 ldc 0x65
        lstFile << std::setw(8) << pLine.address << " "
                << std::setw(8) << machineWord << "    "
                << pLine.mnemonic << " " << pLine.operandStr << std::endl;
    }

    objFile.close();
    lstFile.close();
    return true;
}

Assembler::ParsedLine Assembler::parseLine(const std::string& line) {
    ParsedLine pLine = {0, "", "", "", ""};
    std::string processedLine = line;

    // 1. Find and strip comments (anything after ';')
    size_t commentPos = processedLine.find(';');
    if (commentPos != std::string::npos) {
        processedLine = processedLine.substr(0, commentPos);
    }

    // 2. Trim leading/trailing whitespace
    processedLine = trim(processedLine);
    if (processedLine.empty()) {
        return pLine; // Line was just whitespace or a comment
    }

    // 3. Check for a label (ends with ':')
    size_t labelPos = processedLine.find(':');
    if (labelPos != std::string::npos) {
        pLine.label = trim(processedLine.substr(0, labelPos));
        processedLine = trim(processedLine.substr(labelPos + 1));
    }

    // 4. Use a stringstream to read mnemonic and operand
    std::stringstream ss(processedLine);
    std::string mnemonic;
    
    ss >> mnemonic;
    pLine.mnemonic = mnemonic;

    // 5. Read the rest of the line. This is the operand.
    std::string operand;
    std::getline(ss, operand); // Read the rest of the stringstream
    pLine.operandStr = trim(operand);
    
    return pLine;
}

int32_t Assembler::resolveOperand(const std::string& operandStr, int32_t currentPC, bool isBranch, bool& operandValid) {
    operandValid = true;
    
    // 1. Check if operandStr is a label (check if it's in symbolTable)
    if (symbolTable.count(operandStr)) {
        int32_t labelAddress = symbolTable.at(operandStr);
        
        if (isBranch) {
            // Branch instructions use a PC-relative offset
            // Offset = targetAddress - (address of *next* instruction)
            return labelAddress - (currentPC + 1);
        } else {
            // Non-branch instructions use the label's value (address) directly
            return labelAddress;
        }
    }

    // 2. If not a label, try to parse it as a number
    // Use strtol, which handles decimal, hex (0x..), and octal (0..)
    try {
        char* end = nullptr;
        // 0 base auto-detects hex/octal/decimal
        long value = std::strtol(operandStr.c_str(), &end, 0); 
        
        // Check if strtol parsed the whole string. If not, error.
        if (end == operandStr.c_str() || (*end != '\0' && !isspace(*end))) {
             operandValid = false;
             return 0; // Not a valid number
        }

        // Check if the value fits in our 24-bit signed operand
        const int32_t min_op = -(1 << 23); // -8388608
        const int32_t max_op = (1 << 23) - 1;  // 8388607
        
        if (!isBranch && (value < min_op || value > max_op)) {
             // For non-branch, warn if it's out of 24-bit range
             logError("Warning: Operand " + std::to_string(value) + " out of 24-bit range.", -1);
        }
        // For 'data', it's a 32-bit value, so this check is not needed,
        // but truncation to 24-bits for other instructions is handled by the shift.

        return static_cast<int32_t>(value);

    } catch (...) {
        operandValid = false;
        return 0;
    }
}

void Assembler::logError(const std::string& message, int lineNumber) {
    std::cerr << "Error";
    if (lineNumber != -1) {
        std::cerr << " (line " << lineNumber << ")";
    }
    std::cerr << ": " << message << std::endl;
}