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
#define M_X   255 // has extra t-cycles or is unique in some way (conditional), handled separately

// minimum t-cycles for 1 byte instruction: 4
// minimum t-cycles for 2 byte instruction: 7
// minimum t-cycles for 3 byte instruction: 10

uint8_t instructionMCycles[0x100] = {
//  0x_0   0x_1   0x_2   0x_3   0x_4   0x_5   0x_6   0x_7   0x_8   0x_9   0x_A   0x_B   0x_C   0x_D   0x_E   0x_F
	M_F  , M_FRR, M_FW , M_X  , M_F  , M_F  , M_FR , M_F  , M_F  , M_X  , M_FR , M_X  , M_F  , M_F  , M_FR , M_F  , // 0x0_
	M_X  , M_FRR, M_FW , M_X  , M_F  , M_F  , M_FR , M_F  , M_X  , M_X  , M_FR , M_X  , M_F  , M_F  , M_FR , M_F  , // 0x1_
	M_X  , M_FRR, M_FW , M_X  , M_F  , M_F  , M_FR , M_F  , M_X  , M_X  , M_FQR, M_X  , M_F  , M_F  , M_FR , M_F  , // 0x2_
	M_X  , M_FRR, M_FW , M_X  , M_X  , M_X  , M_FRW, M_F  , M_X  , M_X  , M_FTR, M_X  , M_F  , M_F  , M_FR , M_F  , // 0x3_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0x4_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0x5_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0x6_
	M_FW , M_FW , M_FW , M_FW , M_FW , M_FW , M_F  , M_FW , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0x7_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0x8_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0x9_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0xA_
	M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_F  , M_FR , M_F  , // 0xB_
	M_X  , M_FRR, M_FRR, M_FRR, M_X  , M_X  , M_FR , M_X  , M_X  , M_FRR, M_FRR, M_X  , M_X  , M_X  , M_FR , M_X  , // 0xC_
	M_X  , M_FRR, M_FRR, M_X  , M_X  , M_X  , M_FR , M_X  , M_X  , M_F  , M_FRR, M_X  , M_X  , M_X  , M_FR , M_X  , // 0xD_
	M_X  , M_FRR, M_FRR, M_X  , M_X  , M_X  , M_FR , M_X  , M_X  , M_F  , M_FRR, M_F  , M_X  , M_X  , M_FR , M_X  , // 0xE_
	M_X  , M_FRR, M_FRR, M_F  , M_X  , M_X  , M_FR , M_X  , M_X  , M_X  , M_FRR, M_F  , M_X  , M_X  , M_FR , M_X  , // 0xF_
};
uint8_t instructionSizes[0x100] = {
//  0x_0   0x_1   0x_2   0x_3   0x_4   0x_5   0x_6   0x_7   0x_8   0x_9   0x_A   0x_B   0x_C   0x_D   0x_E   0x_F
	1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x0_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x1_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x2_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x3_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x4_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x5_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x6_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x7_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x8_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x9_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xA_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xB_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xC_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xD_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xE_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xF_
};

#endif