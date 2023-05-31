#ifndef SYSTEM_H
#define SYSTEM_H

#include <cstdint>
#include <string>

#include "instructions.hpp"
#include "ops.hpp"

/*
chips:
cpu = Zilog Z84C
ram = Atmel AT28C64B
rom = Alliance Memory AS6C6264
uart = Western Design Center W65C51N (68c51? more modern uart?)
*/

class Circuit {
public:
    uint8_t instructionFetchControlValues[5 * 8] = {
    //  MREQ RD WR M1 RFSH
    //  v    v  v  v  v
        1,   1, 1, 0, 1, // clock rises
        0,   0, 1, 0, 1, // clock falls
        0,   0, 1, 0, 1,
        0,   0, 1, 0, 1,
        1,   1, 1, 1, 0,
        0,   1, 1, 1, 0,
        0,   1, 1, 1, 0,
        1,   1, 1, 1, 0,
    };
    uint8_t memoryReadControlValues[5 * 6] = {
    //  MREQ RD WR M1 RFSH
    //  v    v  v  v  v
        1,   1, 1, 1, 1, // clock rises
        0,   0, 1, 1, 1, // clock falls
        0,   0, 1, 1, 1,
        0,   0, 1, 1, 1,
        0,   0, 1, 1, 1,
        1,   1, 1, 1, 1,
    };
    uint8_t memoryWriteControlValues[5 * 6] = {
    //  MREQ RD WR M1 RFSH
    //  v    v  v  v  v
        1,   1, 1, 1, 1, // clock rises
        0,   1, 1, 1, 1, // clock falls
        0,   1, 1, 1, 1,
        0,   1, 0, 1, 1,
        0,   1, 0, 1, 1,
        1,   1, 1, 1, 1,
    };

    uint8_t rom[0x1000] = { // address begins at 0x0000
        0x3E, 0xFF,
        0x32, 0x00, 0x10
    };
    uint8_t ram[0x1000]; // address begins at 0x1000

    uint8_t netValues[40];

    //     0 = clock
    //     1 = read
    //     2 = write
    //  3-16 = address bytes 0-13
    // 17-24 = data
    //    25 = memory request
    //    26 = refresh
    //    27 = machine cycle 1
    //    28 = vcc
    //    29 = ground

    int oldClock;

    int currentTCycleHalf;
    int currentTCycle;
    int currentMCycle;

    int mType;
    int currentInstruction;

    uint16_t pcReg;
    uint8_t aReg;

    uint16_t addressLatch;
    uint8_t dataLatchLow;
    uint8_t dataLatchHigh;

    bool shouldOperate;
    int appearingOnAddress;

    void initSystem() {
        memset(netValues, 3, 27 * sizeof(uint8_t));
        memset(&(netValues[1]), 2, 2 * sizeof(uint8_t));
        memset(&(netValues[25]), 2, 3 * sizeof(uint8_t));
        memset(&(netValues[28]), 1, 2 * sizeof(uint8_t));

        oldClock = netValues[0];

        currentTCycleHalf = -1;
        currentTCycle = 0;
        currentMCycle = 0;

        mType = 0;
        currentInstruction = -1;
        pcReg = 0;

        addressLatch = 0;
        dataLatchLow = 0;
        dataLatchHigh = 0;

        shouldOperate = false;
        appearingOnAddress = 0;
    }

    void setControlValues(uint8_t* values, int offset) {
        netValues[25] = 3 - values[offset + 0];
        netValues[1] = 3 - values[offset + 1];
        netValues[2] = 3 - values[offset + 2];
        netValues[27] = 3 - values[offset + 3];
        netValues[26] = 3 - values[offset + 4];
    }

    void setAddressBus(uint16_t value) {
        netValues[ 3] = 3 - ((value & (1 <<  0)) >>  0);
        netValues[ 4] = 3 - ((value & (1 <<  1)) >>  1);
        netValues[ 5] = 3 - ((value & (1 <<  2)) >>  2);
        netValues[ 6] = 3 - ((value & (1 <<  3)) >>  3);
        netValues[ 7] = 3 - ((value & (1 <<  4)) >>  4);
        netValues[ 8] = 3 - ((value & (1 <<  5)) >>  5);
        netValues[ 9] = 3 - ((value & (1 <<  6)) >>  6);
        netValues[10] = 3 - ((value & (1 <<  7)) >>  7);
        netValues[11] = 3 - ((value & (1 <<  8)) >>  8);
        netValues[12] = 3 - ((value & (1 <<  9)) >>  9);
        netValues[13] = 3 - ((value & (1 << 10)) >> 10);
        netValues[14] = 3 - ((value & (1 << 11)) >> 11);
        netValues[15] = 3 - ((value & (1 << 12)) >> 12);
        netValues[16] = 3 - ((value & (1 << 13)) >> 13);
    }
    uint16_t getAddressBus() {
        return
            ((netValues[3] == 2) << 0) +
            ((netValues[4] == 2) << 1) +
            ((netValues[5] == 2) << 2) +
            ((netValues[6] == 2) << 3) + 
            ((netValues[7] == 2) << 4) + 
            ((netValues[8] == 2) << 5) + 
            ((netValues[9] == 2) << 6) + 
            ((netValues[10] == 2) << 7) + 
            ((netValues[11] == 2) << 8) + 
            ((netValues[12] == 2) << 9) + 
            ((netValues[13] == 2) << 10) + 
            ((netValues[14] == 2) << 11) + 
            ((netValues[15] == 2) << 12) + 
            ((netValues[16] == 2) << 13);
    }
    void setDataBus(uint8_t value) {
        netValues[17] = 3 - ((value & (1 << 0)) >> 0);
        netValues[18] = 3 - ((value & (1 << 1)) >> 1);
        netValues[19] = 3 - ((value & (1 << 2)) >> 2);
        netValues[20] = 3 - ((value & (1 << 3)) >> 3);
        netValues[21] = 3 - ((value & (1 << 4)) >> 4);
        netValues[22] = 3 - ((value & (1 << 5)) >> 5);
        netValues[23] = 3 - ((value & (1 << 6)) >> 6);
        netValues[24] = 3 - ((value & (1 << 7)) >> 7);
    }
    uint8_t getDataBus() {
        return
            ((netValues[17] == 2) << 0) +
            ((netValues[18] == 2) << 1) +
            ((netValues[19] == 2) << 2) +
            ((netValues[20] == 2) << 3) + 
            ((netValues[21] == 2) << 4) + 
            ((netValues[22] == 2) << 5) + 
            ((netValues[23] == 2) << 6) + 
            ((netValues[24] == 2) << 7);
    }

    string hex16(uint16_t value) {
        char output[4];
        uint8_t temp;
        
        for(int i = 0; i < 4; i++) {
            temp = (value >> (i * 4)) & 0xF;

            if(temp < 0xA) {output[3 - i] = temp + '0';}
            else {output[3 - i] = temp - 0xA + 'A';}
        }

        return string(output);
    }
    string hex8(uint8_t value) {
        char output[2];
        uint8_t temp;
        
        for(int i = 0; i < 2; i++) {
            temp = (value >> (i * 4)) & 0xF;

            if(temp < 0xA) {output[1 - i] = temp + '0';}
            else {output[1 - i] = temp - 0xA + 'A';}
        }

        return string(output);
    }

    uint8_t getByte(uint16_t address) {
        if((address % 0x2000) < 0x1000) {
            return rom[address];
        }
        else {
            return ram[address - 0x1000];
        }
    }
    void setByte(uint16_t address, uint8_t value) {
        if((address % 0x2000) < 0x1000) {
            //rom[address] = value;
        }
        else {
            ram[address - 0x1000] = value;
        }
    }

    void updateCPUOperation() {
        if(mType == 0) {
            if(currentTCycle == 2 && currentTCycleHalf == 0) {currentInstruction = getDataBus();}

            if(currentTCycle == 4) {
                currentTCycle = 0;
                pcReg++;

                if(instructionMCycles[currentInstruction] ==  M_F) {
                    mType = 0;
                    currentMCycle = 0;
                    currentInstruction = -1;
                    shouldOperate = true;
                }
                else if(
                    instructionMCycles[currentInstruction] ==  M_FR ||
                    instructionMCycles[currentInstruction] ==  M_FRR ||
                    instructionMCycles[currentInstruction] ==  M_FRW ||
                    instructionMCycles[currentInstruction] ==  M_FTR ||
                    instructionMCycles[currentInstruction] ==  M_FQR ||
                    instructionMCycles[currentInstruction] ==  M_DRW ||
                    instructionMCycles[currentInstruction] ==  M_TRW
                ) {
                    mType = 1;
                    currentMCycle++;
                }
                else if(instructionMCycles[currentInstruction] ==  M_FW) {
                    mType = 2;
                    currentMCycle++;
                }
            }
        }
        if(mType == 1) {
            if(currentTCycle == 2 && currentTCycleHalf == 1) {
                if(currentMCycle == 1) {dataLatchLow = getDataBus();}
                if(currentMCycle == 2) {dataLatchHigh = getDataBus() << 8;}
            }

            if(currentTCycle == 3) {
                currentTCycle = 0;
                pcReg++;

                if(currentMCycle == instructionSizes[currentInstruction] - 1) {shouldOperate = true;}

                if(
                    instructionMCycles[currentInstruction] ==  M_FR ||
                    instructionMCycles[currentInstruction] ==  M_FRR && currentMCycle == 2 ||
                    instructionMCycles[currentInstruction] ==  M_FTR && currentMCycle == 3 ||
                    instructionMCycles[currentInstruction] ==  M_FQR && currentMCycle == 4
                ) {
                    mType = 0;
                    currentMCycle = 0;
                    currentInstruction = -1;
                }
                else if(
                    instructionMCycles[currentInstruction] ==  M_FRR && currentMCycle < 2 ||
                    instructionMCycles[currentInstruction] ==  M_FTR && currentMCycle < 3 ||
                    instructionMCycles[currentInstruction] ==  M_FQR && currentMCycle < 4 ||
                    instructionMCycles[currentInstruction] ==  M_DRW && currentMCycle < 2 ||
                    instructionMCycles[currentInstruction] ==  M_TRW && currentMCycle < 3
                ) {
                    mType = 1;
                    currentMCycle++;
                }
                else if(
                    instructionMCycles[currentInstruction] ==  M_FRW ||
                    instructionMCycles[currentInstruction] ==  M_DRW && currentMCycle == 2 ||
                    instructionMCycles[currentInstruction] ==  M_TRW && currentMCycle == 3
                ) {
                    mType = 2;
                    currentMCycle++;
                }
            }
        }
        if(mType == 2) {
            if(currentTCycle == 2 && currentTCycleHalf == 1) {setByte(getAddressBus(), getDataBus());}
            
            if(currentTCycle == 3) {
                currentTCycle = 0;
                pcReg++;
                mType = 0;
                currentMCycle = 0;
                currentInstruction = -1;
            }
        }
    }

    void updateCPUBusses() {
        setAddressBus(0);
        setDataBus(0);

        if(mType == 0) {
            setControlValues(instructionFetchControlValues, (currentTCycleHalf + currentTCycle * 2) * 5);
            if(currentTCycle == 0 || currentTCycle == 1) {setAddressBus(pcReg);}

            // refresh
            //if(currentTCycle == 2 || currentTCycle == 3) {setAddressBus(0);}
        }
        if(mType == 1) {
            setControlValues(memoryReadControlValues, (currentTCycleHalf + currentTCycle * 2) * 5);

            if(!shouldOperate) {
                setAddressBus(pcReg);
                appearingOnAddress = 0;
            }
            else {
                setAddressBus(addressLatch);
                appearingOnAddress = 1;
            }
        }
        if(mType == 2) {
            setControlValues(memoryWriteControlValues, (currentTCycleHalf + currentTCycle * 2) * 5);
            
            setAddressBus(addressLatch);
            if(currentTCycle == 0 && currentTCycleHalf == 1 || currentTCycle >= 1) {
                setDataBus(dataLatchLow); // check fo rhigh
            }
        }
    }

    void updateMemory() {
        if(netValues[1] == 3) { // read
            uint16_t addressValue = getAddressBus() & 0xFFF;
            
            if(netValues[16] == 3) { // ROM
                uint8_t dataValue = 0;
                if(addressValue < 0x120) {dataValue = rom[addressValue];}

                setDataBus(dataValue);
            }
            else { // RAM
                uint8_t dataValue = 0;
                if(addressValue < 0x150) {dataValue = ram[addressValue];}

                setDataBus(dataValue);
            }
        }
    }

    void updateSystem() {
        updateCPUOperation();
        updateCPUBusses();
        updateMemory();

        if(shouldOperate) {
            shouldOperate = false;

            
        }
    }

    void clockTick() {
        if(netValues[0] != oldClock) {
            oldClock = netValues[0];

            currentTCycleHalf++;
            if(currentTCycleHalf == 2) {
                currentTCycleHalf = 0;
                currentTCycle++;
            }

            updateSystem();
        }
    }
};

#endif
