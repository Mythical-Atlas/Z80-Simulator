#include "instructions.hpp"
#include "ops.hpp"

uint8_t instructionMCycles[0x100] = {
//  0x_0   0x_1   0x_2   0x_3   0x_4   0x_5   0x_6   0x_7   0x_8   0x_9   0x_A   0x_B   0x_C   0x_D   0x_E   0x_F
	M_F  , M_FRR, M_FW , M_X  , M_F  , M_F  , M_FR , M_F  , M_F  , M_X  , M_FR , M_X  , M_F  , M_F  , M_FR , M_F  , // 0x0_
	M_X  , M_FRR, M_FW , M_X  , M_F  , M_F  , M_FR , M_F  , M_X  , M_X  , M_FR , M_X  , M_F  , M_F  , M_FR , M_F  , // 0x1_
	M_X  , M_FRR, M_TRW, M_X  , M_F  , M_F  , M_FR , M_F  , M_X  , M_X  , M_FQR, M_X  , M_F  , M_F  , M_FR , M_F  , // 0x2_
	M_X  , M_FRR, M_DRW, M_X  , M_X  , M_X  , M_FRW, M_F  , M_X  , M_X  , M_FTR, M_X  , M_F  , M_F  , M_FR , M_F  , // 0x3_
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
	1,     3,     1,     1,     1,     1,     2,     1,     1,     1,     1,     1,     1,     1,     2,     1,     // 0x0_
	2,     3,     1,     1,     1,     1,     2,     1,     2,     1,     1,     1,     1,     1,     2,     1,     // 0x1_
	2,     3,     3,     1,     1,     1,     2,     1,     2,     1,     3,     1,     1,     1,     2,     1,     // 0x2_
	2,     3,     3,     1,     1,     1,     2,     1,     2,     1,     3,     1,     1,     1,     2,     1,     // 0x3_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0x4_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0x5_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0x6_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0x7_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0x8_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0x9_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0xA_
	1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     // 0xB_
	1,     1,     3,     3,     3,     1,     2,     1,     1,     1,     3,     0,     3,     3,     2,     1,     // 0xC_
	1,     1,     3,     2,     3,     1,     2,     1,     1,     1,     3,     2,     3,     0,     2,     1,     // 0xD_
	1,     1,     3,     1,     3,     1,     2,     1,     1,     1,     3,     1,     3,     0,     2,     1,     // 0xE_
	1,     1,     3,     1,     3,     1,     2,     1,     1,     1,     3,     1,     3,     0,     2,     1,     // 0xF_
};

int instructionInputs[0x100] = {
//  0x_0   0x_1   0x_2   0x_3   0x_4   0x_5   0x_6   0x_7   0x_8   0x_9   0x_A   0x_B   0x_C   0x_D   0x_E   0x_F
	0,     A_IMS, A_A,   0,     0,     0,     A_IMB, 0,     0,     0,     A_IBC, 0,     0,     0,     A_IMB, 0,     // 0x0_
	0,     A_IMS, A_A,   0,     0,     0,     A_IMB, 0,     0,     0,     A_IDE, 0,     0,     0,     A_IMB, 0,     // 0x1_
	0,     A_IMS, A_HL,  0,     0,     0,     A_IMB, 0,     0,     0,     A_IIM, 0,     0,     0,     A_IMB, 0,     // 0x2_
	0,     A_IMS, A_A,   0,     0,     0,     A_IMB, 0,     0,     0,     A_IIM, 0,     0,     0,     A_IMB, 0,     // 0x3_
	A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   // 0x4_
	A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   // 0x5_
	A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   // 0x6_
	A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   0,     A_A,   A_B,   A_C,   A_D,   A_E,   A_H,   A_L,   A_IHL, A_A,   // 0x7_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x8_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x9_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xA_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xB_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xC_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xD_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xE_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     A_HL,  0,     0,     0,     0,     0,     0,     // 0xF_
};
int instructionOutputs[0x100] = {
//  0x_0   0x_1   0x_2   0x_3   0x_4   0x_5   0x_6   0x_7   0x_8   0x_9   0x_A   0x_B   0x_C   0x_D   0x_E   0x_F
	0,     A_BC,  A_IBC, 0,     0,     0,     A_B,   0,     0,     0,     A_A,   0,     0,     0,     A_C,   0,     // 0x0_
	0,     A_DE,  A_IDE, 0,     0,     0,     A_D,   0,     0,     0,     A_A,   0,     0,     0,     A_E,   0,     // 0x1_
	0,     A_HL,  A_IIM, 0,     0,     0,     A_H,   0,     0,     0,     A_HL,  0,     0,     0,     A_L,   0,     // 0x2_
	0,     A_SP,  A_IIM, 0,     0,     0,     A_IHL, 0,     0,     0,     A_A,   0,     0,     0,     A_A,   0,     // 0x3_
	A_B,   A_B,   A_B,   A_B,   A_B,   A_B,   A_B,   A_B,   A_C,   A_C,   A_C,   A_C,   A_C,   A_C,   A_C,   A_C,   // 0x4_
	A_D,   A_D,   A_D,   A_D,   A_D,   A_D,   A_D,   A_D,   A_E,   A_E,   A_E,   A_E,   A_E,   A_E,   A_E,   A_E,   // 0x5_
	A_H,   A_H,   A_H,   A_H,   A_H,   A_H,   A_H,   A_H,   A_L,   A_L,   A_L,   A_L,   A_L,   A_L,   A_L,   A_L,   // 0x6_
	A_IHL, A_IHL, A_IHL, A_IHL, A_IHL, A_IHL, 0,     A_IHL, A_A,   A_A,   A_A,   A_A,   A_A,   A_A,   A_A,   A_A,   // 0x7_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x8_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0x9_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xA_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xB_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xC_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xD_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     // 0xE_
	0,     0,     0,     0,     0,     0,     0,     0,     0,     A_SP,  0,     0,     0,     0,     0,     0,     // 0xF_
};