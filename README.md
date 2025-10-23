# Two-Pass Assembler and Stack-Based Virtual Machine

This project is a complete, custom-built computing ecosystem that demonstrates the foundational principles of computer architecture and compiler design. It consists of two main components:

1.  **A Two-Pass Assembler:** A C++ program that translates a custom, human-readable assembly language into executable 32-bit machine code.
2.  **A Stack-Based Virtual Machine (VM):** A C++ program that simulates a custom-designed CPU. It loads the generated machine code and executes it, instruction by instruction, just like a real processor.

To prove the system's functionality, a Bubble Sort algorithm is implemented in the custom assembly language, compiled by the assembler, and executed on the virtual machine to sort an array of integers.

## Features

### Assembler
* **Two-Pass Design:** Correctly handles forward references (using labels before they are defined) by building a Symbol Table in Pass 1 and generating code in Pass 2.
* **Symbol Table:** Manages labels for both code (`main:`, `loop:`) and data (`n:`, `array:`).
* **Binary Output:** Generates a raw binary object file (`.obj`) containing the 32-bit machine code instructions.
* **Listing File:** Generates a human-readable listing file (`.lst`) that shows the memory address, the machine code (hex), and the original assembly line for easy debugging.

### Virtual Machine
* **Stack-Based Architecture:** The CPU is designed around a 2-level register stack (`A`, `B`) and a main memory stack (`SP`), simplifying arithmetic and function calls.
* **Custom Instruction Set Architecture (ISA):** Features 19 custom opcodes for memory, arithmetic, stack, and control flow operations.
* **Fetch-Decode-Execute Cycle:** The core of the VM, which faithfully simulates how a real CPU operates.
* **Memory Model:** A simple, linear 64k-word (256KB) RAM, implemented as a `std::vector`.

## VM Architecture Deep Dive

Understanding this architecture is key to understanding the assembly language.

* **Registers:**
    * `A`: Top of the register stack.
    * `B`: Bottom of the register stack.
    * `PC`: Program Counter (points to the *next* instruction).
    * `SP`: Stack Pointer (points to the top of the data stack in RAM).

* **Instruction Format:**
    All instructions are 32-bit words.
    * **Operand (Bits 31-8):** A 24-bit signed value.
    * **Opcode (Bits 7-0):** An 8-bit instruction identifier.

* **Example Instructions:**
    * `ldc <value>` (Load Constant): `B = A; A = <value>`
    * `add`: `A = B + A`
    * `ldl <offset>` (Load Local): `B = A; A = memory[SP + <offset>]` (Read from data stack)
    * `stl <offset>` (Store Local): `memory[SP + <offset>] = A; A = B` (Write to data stack)
    * `brz <offset>` (Branch if Zero): `if (A == 0) PC = PC + <offset>` (PC-relative jump)
    * `stnl <offset>` (Store Non-Local): `memory[A + <offset>] = B` (Used for array writes: `array[i] = val`)
    * `ldnl <offset>` (Load Non-Local): `A = memory[A + <offset>]` (Used for array reads: `val = array[i]`)

## How to Build and Run

This project uses a `Makefile` to simplify compilation.

### 1. (Recommended) Create a `Makefile`

If you don't have one, create a file named `Makefile` in the project root:

```Makefile
# Compiler
CXX = g++
# Flags: C++11 standard, all warnings, extra warnings, debug symbols
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# Target executables
ASSEMBLER = asm
EMULATOR = emu

# Source files
ASM_SRCS = assembler/main.cpp assembler/Assembler.cpp
ASM_OBJS = $(ASM_SRCS:.cpp=.o)

EMU_SRCS = emulator/main.cpp emulator/VirtualMachine.cpp
EMU_OBJS = $(EMU_SRCS:.cpp=.o)

# Default rule: build everything
all: $(ASSEMBLER) $(EMULATOR)

# Rule to build the assembler
$(ASSEMBLER): $(ASM_OBJS)
	$(CXX) $(CXXFLAGS) -o $(ASSEMBLER) $(ASM_OBJS)

# Rule to build the emulator
$(EMULATOR): $(EMU_OBJS)
	$(CXX) $(CXXFLAGS) -o $(EMULATOR) $(EMU_OBJS)

# Generic rule to compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(ASM_OBJS) $(EMU_OBJS) $(ASSEMBLER) $(EMULATOR) *.o *.obj *.lst *.exe
```

### 2. Build the Tools

Open your terminal and run `make`:

```sh
make
```

This will create two executables: `asm` (the assembler) and `emu` (the emulator).

### 3. Run the Full Pipeline

1.  **Assemble:** Use `asm` to convert `bubble_sort.asm` into machine code.

    ```sh
    ./asm bubble_sort.asm bubble_sort.obj bubble_sort.lst
    ```

2.  **Execute:** Use `emu` to load and run the generated `bubble_sort.obj` file.

    ```sh
    ./emu bubble_sort.obj
    ```

### 4. Check the Output

The emulator will run the bubble sort and then print the state of the sorted array from its own memory, verifying the result.

```
Loaded 78 words into memory.
--- Running Program ---
--- Program Halted ---
Registers:
  A:  0x00000000 (0)
  B:  0x00000004 (4)
  PC: 0x00000048 (72)
  SP: 0x00001000 (4096)
--- Verifying Sorted Array in Memory ---
Memory at addresses 73-77:
  addr[73]: 2
  addr[74]: 5
  addr[75]: 10
  addr[76]: 30
  addr[77]: 50
```

## Project Structure

```
.
├── assembler/
│   ├── Assembler.cpp       # Pass 1 & Pass 2 logic
│   ├── Assembler.h         # Assembler class assembler
├── emulator/
│   ├── VirtualMachine.cpp  # VM (CPU) implementation
│   ├── VirtualMachine.h    # VM class definition
│   └── main.cpp            # Driver for the VM (includes verifier)
├── Common.h                # Shared definitions (opcode table)
├── bubble_sort.asm         # Example program to be assembled
├── Makefile                # Build script
└── README.md               # This file
```