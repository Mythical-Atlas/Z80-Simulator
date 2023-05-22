#ifndef SYSTEM_H
#define SYSTEM_H

#include <cstdint>
#include <string>

uint8_t netValues[40];

uint8_t futureValues[40 * 10] = {
	0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // clock on
	0, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // clock off
	0, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	0, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // off
	0, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	0, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // off
	0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // off
	0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // off
};

int oldClock;
int htcCount;
int tcCount;
int mcCount;
int icCount;

void initSystem() {
	memset(netValues, 0, 4 * sizeof(uint8_t));

	netValues[0] = 3;
    netValues[1] = 3;
    netValues[2] = 3;
    netValues[3] = 3;
    netValues[4] = 3;
    netValues[5] = 3;
    netValues[6] = 3;
    netValues[7] = 3;
    netValues[8] = 3;
    netValues[9] = 3;
    netValues[10] = 3;
    netValues[11] = 3;
    netValues[12] = 3;
    netValues[13] = 3;
    netValues[14] = 3;
    netValues[15] = 3;
    netValues[16] = 3;
    netValues[17] = 3;
    netValues[18] = 3;
    netValues[19] = 3;
    netValues[20] = 3;
    netValues[21] = 3;
    netValues[22] = 3;
    netValues[23] = 3;
    netValues[24] = 3;

	oldClock = netValues[0];
	htcCount = 0;
}

void updateSystem() {
	if(netValues[0] != oldClock) {
		oldClock = netValues[0];

		if(htcCount < 10) {for(int i = 1; i < 40; i++) {netValues[i] = futureValues[i + htcCount * 40];}}

		htcCount++;
	}
}

#endif

/*

instruction fetch (M1 cycle)

clock rising edge begins T1 and M1
- M1 goes low, PC appears on address bus

clock falling edge
- RD and MREQ go low

clock rising edge (T2)
- nothing changes

clock falling edge
- nothing changes

clock rising edge (T3)
- MREQ, RD, and M1 go high
- RFSH goes low
- refresh address (?) appears on address bus

clock falling edge
- MREQ goes low

clock rising edge (T4)
- nothing

clock falling edge
- MREQ goes high

clock rising edge (T_ -> starting next M cycle)
- RFSH goes high

memory read

clock rising edge (T1)
-

clock falling edge
-

clock rising edge (T2)
-

clock falling edge
-

clock rising edge (T3)
-

clock falling edge
-

clock rising edge (T_ -> starting next M cycle)
-

*/