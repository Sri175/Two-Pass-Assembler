#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <string>
#include <vector>
#include <cstdint>

class VirtualMachine {
public:
    VirtualMachine(int memorySize = 65536); // Default 64k words (256KB)
    
    // Loads the binary object file into memory
    bool loadProgram(const std::string& objectFilename);

    // Runs the loaded program
    void run();

    // Dumps the state of the machine (registers, memory)
    void dumpState();
    int32_t readMemory(int32_t address);

private:
    // Registers
    int32_t A, B;   // Two registers, arranged as a stack
    int32_t PC;   // Program Counter
    int32_t SP;   // Stack Pointer

    // Machine state
    bool halted;

    // Main memory
    std::vector<int32_t> memory;

    // The fetch-decode-execute cycle
    void executeInstruction();
};

#endif // VIRTUAL_MACHINE_H