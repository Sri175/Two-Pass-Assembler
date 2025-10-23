# Use g++ for C++ compilation
CXX = g++
# Flags: C++11 standard, all warnings, debugging symbols
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# Phony targets don't represent files
.PHONY: all clean

# Default target: build both assembler and emulator
all: asm emu

# Target for the assembler
asm: assembler/main.o assembler/Assembler.o
	$(CXX) $(CXXFLAGS) -o asm assembler/main.o assembler/Assembler.o

# Target for the emulator
emu: emulator/main.o emulator/VirtualMachine.o
	$(CXX) $(CXXFLAGS) -o emu emulator/main.o emulator/VirtualMachine.o

# Object file dependencies
assembler/main.o: assembler/main.cpp assembler/Assembler.h Common.h
	$(CXX) $(CXXFLAGS) -c assembler/main.cpp -o assembler/main.o

assembler/Assembler.o: assembler/Assembler.cpp assembler/Assembler.h Common.h
	$(CXX) $(CXXFLAGS) -c assembler/Assembler.cpp -o assembler/Assembler.o

emulator/main.o: emulator/main.cpp emulator/VirtualMachine.h
	$(CXX) $(CXXFLAGS) -c emulator/main.cpp -o emulator/main.o

emulator/VirtualMachine.o: emulator/VirtualMachine.cpp emulator/VirtualMachine.h
	$(CXX) $(CXXFLAGS) -c emulator/VirtualMachine.cpp -o emulator/VirtualMachine.o

# Clean up build files
clean:
	rm -f asm emu assembler/*.o emulator/*.o