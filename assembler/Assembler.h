#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include "../Common.h"

class Assembler {
public:
    // Constructor
    Assembler();

    // Main function to assemble a file
    // Returns true on success, false on error
    bool assemble(const std::string& inputFilename, 
                  const std::string& outputObjectFilename, 
                  const std::string& outputListFilename);

private:
    // The Symbol Table: maps label strings to their 32-bit address
    std::map<std::string, int32_t> symbolTable;

    // Helper struct to store a parsed line from the source
    // This makes Pass 2 much easier
    struct ParsedLine {
        int32_t address;
        std::string label;
        std::string mnemonic;
        std::string operandStr;
        std::string originalLine; // For the listing file
    };

    // This vector will hold the entire program, parsed
    std::vector<ParsedLine> programLines;

    // --- Pass 1 ---
    // Reads the file, parses lines, and builds the symbol table
    // Returns true on success, false on error
    bool performPass1(const std::string& inputFilename);

    // --- Pass 2 ---
    // Generates the object and listing files using the symbol table
    // Returns true on success, false on error
    bool performPass2(const std::string& outputObjectFilename, 
                      const std::string& outputListFilename);

    // --- Helper Functions ---

    // Parses a single line of assembly code
    ParsedLine parseLine(const std::string& line);

    // Converts an operand string (like "5", "0x10", or "myLabel")
    // into its 32-bit integer value. Uses the symbolTable for labels.
    // 'operandValid' is set to false if a label isn't found.
    int32_t resolveOperand(const std::string& operandStr, int32_t currentPC, bool isBranch, bool& operandValid);

    // Helper to log errors
    void logError(const std::string& message, int lineNumber = -1);
};

#endif // ASSEMBLER_H