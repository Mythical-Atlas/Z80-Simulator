#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>

#define M_F   0 // fetch/m1 only (4 t-cycles)
#define M_FR  1 // fetch, read (4 + 3 = 7)
#define M_FW  2 // fetch, write (4 + 3)
#define M_FRR 3 // fetch, read, read (4 + 3 + 3)
#define M_FRW 4 // fetch, read, write (4 + 3 + 3)
#define M_FTR 5 // fetch, read, read, read (4 + 3 + 3 + 3)
#define M_FQR 6 // fetch, read, read, read, read (4 + 3 + 3 + 3 + 3)
#define M_DRW 7 // fetch, read, read, write (4 + 3 + 3 + 3)
#define M_TRW 8 // fetch, read, read, read, write (4 + 3 + 3 + 3 + 3)
#define M_X   255 // has extra t-cycles or is unique in some way (conditional), handled separately

// minimum t-cycles for 1 byte instruction: 4
// minimum t-cycles for 2 byte instruction: 7
// minimum t-cycles for 3 byte instruction: 10

#define A_IM  1 // immediate
#define A_A   2 // reg a
#define A_B   3 // reg b
#define A_C   4 // reg c
#define A_D   5 // reg d
#define A_E   6 // reg e
#define A_H   7 // reg h
#define A_L   8 // reg l
#define A_SP  9 // reg stack pointer
#define A_PC  10 // reg program counter
#define A_IBC 11 // indirect using bc
#define A_IDE 12 // indirect using de
#define A_IHL 13 // indirect using hl
#define A_IIM 14 // indirect using immediate
#define A_BC  3 // regs bc
#define A_DE  4 // regs de
#define A_HL  5 // regs hl
extern uint8_t instructionMCycles[0x100];
extern uint8_t instructionSizes[0x100];
extern int instructionAddressing[0x100];

#endif