#include "VirtualMachine.h"
#include <iostream>
#include <fstream>
#include <iomanip>

VirtualMachine::VirtualMachine(int memorySize) {
    memory.resize(memorySize, 0); // Initialize memory to all zeros
    A = B = PC = SP = 0;
    halted = false;
}

bool VirtualMachine::loadProgram(const std::string& objectFilename) {
    // Open the binary object file
    std::ifstream objFile(objectFilename, std::ios::binary);
    if (!objFile) {
        std::cerr << "Error: Could not open object file " << objectFilename << std::endl;
        return false;
    }

    // Read the file word by word into memory
    int32_t instruction;
    int address = 0;
    while (objFile.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) {
        if (address >= (int)memory.size()) {
            std::cerr << "Error: Program is too large for memory. Resizing..." << std::endl;
            memory.resize(memory.size() * 2);
        }
        memory[address] = instruction;
        address++;
    }

    objFile.close();
    std::cout << "Loaded " << address << " words into memory." << std::endl;
    return true;
}

void VirtualMachine::run() {
    halted = false;
    PC = 0; // Execution starts at address 0

    while (!halted) {
        if (PC < 0 || PC >= (int)memory.size()) {
            std::cerr << "Error: PC out of bounds (" << PC << ")" << std::endl;
            halted = true;
            break;
        }
        executeInstruction();
    }
    
    std::cout << "--- Program Halted ---" << std::endl;
    dumpState();
}

void VirtualMachine::dumpState() {
    std::cout << "Registers:" << std::endl;
    std::cout << "  A:  0x" << std::hex << std::setw(8) << std::setfill('0') << A 
              << " (" << std::dec << A << ")" << std::endl;
    std::cout << "  B:  0x" << std::hex << std::setw(8) << std::setfill('0') << B 
              << " (" << std::dec << B << ")" << std::endl;
    std::cout << "  PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << PC 
              << " (" << std::dec << PC << ")" << std::endl;
    std::cout << "  SP: 0x" << std::hex << std::setw(8) << std::setfill('0') << SP 
              << " (" << std::dec << SP << ")" << std::endl;
}

void VirtualMachine::executeInstruction() {
    // 1. Fetch
    int32_t instructionWord = memory[PC];

    // 2. Increment PC (happens *before* execution)
    int32_t old_PC = PC;
    PC++;

    // 3. Decode
    // Opcode is the bottom 8 bits
    int8_t opcode = instructionWord & 0xFF;
    // Operand is the upper 24 bits (sign-extended)
    int32_t operand = instructionWord >> 8; 

    // 4. Execute
    switch (opcode) {
        case 0:  // ldc
            B = A;
            A = operand;
            break;
            
        case 1:  // adc
            A = A + operand;
            break;

        case 2:  // ldl
            B = A;
            A = memory[SP + operand];
            break;
            
        case 3:  // stl
            memory[SP + operand] = A;
            A = B;
            break;
        
        case 4: // ldnl
            A = memory[A + operand];
            break;

        case 5: // stnl
            memory[A + operand] = B;
            break;

        case 6:  // add
            A = B + A;
            break;

        case 7: // sub
            A = B - A;
            break;
        
        case 8: // shl
            A = B << A;
            break;
            
        case 9: // shr
            A = B >> A; // This is an arithmetic shift because B is signed
            break;

        case 10: // adj
            SP = SP + operand;
            break;
            
        case 11: // a2sp
            SP = A;
            A = B;
            break;
            
        case 12: // sp2a
            B = A;
            A = SP;
            break;
            
        case 13: // call
            B = A;
            A = PC; // Store return address (PC of *next* instruction) in A
            PC = PC + operand; // Branch to new location
            break;

        case 14: // return
            PC = A;
            A = B;
            break;
        
        case 15: // brz
            if (A == 0) {
                PC = PC + operand;
            }
            break;
            
        case 16: // brlz
            if (A < 0) {
                PC = PC + operand;
            }
            break;

        case 17: // br
            PC = PC + operand;
            break;

        case 18: // HALT
            halted = true;
            break;
            
        default:
            std::cerr << "Error: Unknown opcode " << (int)opcode 
                      << " at address " << old_PC << std::endl;
            halted = true;
            break;
    }
}
int32_t VirtualMachine::readMemory(int32_t address) {
    if (address >= 0 && address < (int)memory.size()) {
        return memory[address];
    }
    return 0; // Return 0 for an invalid address
}