#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <map>
#include <cstdint> // For int32_t, int8_t

// Structure to hold information about each instruction mnemonic
struct OpcodeInfo {
    int8_t opcode;        // The 8-bit numeric opcode
    bool expectsOperand;  // True if the instruction takes an operand
};

// This map will be used in Pass 1 and Pass 2
static std::map<std::string, OpcodeInfo> opcodeTable = {
    // Instructions with operands
    {"data", {-1, true}},  // Special case for assembler, not a real opcode
    {"ldc",  {0,  true}},
    {"adc",  {1,  true}},
    {"ldl",  {2,  true}},
    {"stl",  {3,  true}},
    {"ldnl", {4,  true}},
    {"stnl", {5,  true}},
    {"adj",  {10, true}},
    {"call", {13, true}},
    {"brz",  {15, true}},
    {"brlz", {16, true}},
    {"br",   {17, true}},
    {"SET",  {-2, true}},  // Special case for assembler pseudo-instruction

    // Instructions without operands
    {"add",  {6,  false}},
    {"sub",  {7,  false}},
    {"shl",  {8,  false}},
    {"shr",  {9,  false}},
    {"a2sp", {11, false}},
    {"sp2a", {12, false}},
    {"return",{14, false}},
    {"HALT", {18, false}}
};

#endif // COMMON_H