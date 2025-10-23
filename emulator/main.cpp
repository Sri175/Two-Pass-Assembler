#include <iostream>
#include <string>
#include "VirtualMachine.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.obj>" << std::endl;
        return 1;
    }

    std::string objectFile = argv[1];

    VirtualMachine vm;
    
    if (!vm.loadProgram(objectFile)) {
        std::cerr << "Failed to load program." << std::endl;
        return 1;
    }

    std::cout << "--- Running Program ---" << std::endl;
    vm.run(); // <-- The program runs and sorts the memory

    std::cout << "--- Program Halted ---" << std::endl;
    vm.dumpState(); // <-- The registers are printed

    // --- VERIFICATION CODE ---
    
    std::cout << "--- Verifying Sorted Array in Memory ---" << std::endl;
    
    //
    // !! CRITICAL !!
    // You MUST update this address in the next step
    //
    int32_t array_address = 73; // <-- We will fix this
    int32_t n = 7;

    std::cout << "Memory at addresses " << array_address 
              << "-" << (array_address + n - 1) << ":" << std::endl;
              
    for (int i = 0; i < n; i++) {
        // We read the *final* value from memory
        int32_t value = vm.readMemory(array_address + i); 
        std::cout << "  addr[" << (array_address + i) << "]: " 
                  << value << std::endl;
    }

    return 0;
}