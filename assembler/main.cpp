#include <iostream>
#include <string>
#include "Assembler.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.asm> <output.obj> <output.lst>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string objectFile = argv[2];
    std::string listFile = argv[3];

    Assembler asmInstance;
    
    if (asmInstance.assemble(inputFile, objectFile, listFile)) {
        std::cout << "Assembly successful. Output files: " 
                  << objectFile << ", " << listFile << std::endl;
    } else {
        std::cerr << "Assembly failed." << std::endl;
        return 1;
    }

    return 0;
}