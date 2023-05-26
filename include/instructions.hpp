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

#define A_I 1

extern uint8_t instructionMCycles[0x100];
extern uint8_t instructionSizes[0x100];
extern int instructionAddressing[0x100];

#endif