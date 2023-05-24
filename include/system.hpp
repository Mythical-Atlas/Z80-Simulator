#ifndef SYSTEM_H
#define SYSTEM_H

#include <cstdint>
#include <string>

#include "instructions.hpp"

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

    uint8_t romInit[0x120] = { // address begins at 0x0000
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x30, 0x08, 0x06, 0x00, 0x00, 0x00, 0xC9, 0xB8, 0xFE,
        0x27, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00,
        0x02, 0x24, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xED, 0x9C, 0xD1, 0x72, 0xC4, 0x20, 0x08, 0x45,
        0x43, 0x27, 0x1F, 0xEE, 0x7E, 0xB9, 0x7D, 0x68, 0xD2, 0xDA, 0xAD, 0x7A, 0x03, 0x4A, 0x0C, 0xD3,
        0x7B, 0x5E, 0x3A, 0x53, 0x8A, 0x50, 0x51, 0x16, 0x89, 0x59, 0xC9, 0x39, 0xE7, 0xAD, 0x83, 0x88,
        0x48, 0x4F, 0x5E, 0x72, 0x8E, 0xA5, 0xD1, 0xD1, 0x80, 0xC6, 0xF7, 0x96, 0x3F, 0xD1, 0xBF, 0x0F,
        0x8B, 0xA3, 0xE4, 0x39, 0x30, 0x80, 0xC1, 0x61, 0x00, 0x83, 0xB3, 0xAF, 0x76, 0xE0, 0x3F, 0xF1,
        0x7A, 0xBD, 0xA6, 0xCB, 0xB9, 0x03, 0x83, 0x23, 0xA8, 0x0A, 0x85, 0x03, 0x14, 0x15, 0xD1, 0xD5,
        0x2A, 0x4A, 0x33, 0xE6, 0x6A, 0xFD, 0xD1, 0xF9, 0xB9, 0x60, 0xAC, 0x2F, 0x06, 0xD6, 0x61, 0x0A,
        0xF5, 0x38, 0x12, 0x58, 0x27, 0x78, 0x96, 0x2F, 0x1A, 0xFB, 0x9A, 0x05, 0x8A, 0x48, 0x29, 0xE5,
        0xE3, 0x67, 0x55, 0xBF, 0x2A, 0x97, 0xBE, 0x9C, 0x29, 0x34, 0x38, 0x30, 0x85, 0xCE, 0x3C, 0xC8,
        0x7B, 0x1F, 0xA4, 0x9F, 0xEE, 0x1F, 0x0F, 0xF2, 0xE4, 0x0F, 0x0C, 0x60, 0x70, 0x76, 0xD9, 0xE6,
        0x65, 0x2B, 0x34, 0x16, 0x4A, 0x3D, 0x30, 0x35, 0xE5, 0x2D, 0x77, 0x0D, 0x01, 0x39, 0xF4, 0x6F,
        0xD0, 0x7F, 0x77, 0xFB, 0x15, 0x39, 0x77, 0x60, 0x70, 0x18, 0xC0, 0xE0, 0x30, 0x80, 0xC1, 0x61,
        0x2F, 0xF4, 0x4E, 0xFA, 0xAD, 0x4E, 0x93, 0x9C, 0x3B, 0x30, 0x38, 0xF2, 0x5D, 0x39, 0xD9, 0x47,
        0xF8, 0x29, 0x8D, 0xAE, 0x56, 0x89, 0x9A, 0x31, 0x57, 0xEB, 0x0F, 0xCE, 0x0F, 0x54, 0xF7, 0xEE,
        0x85, 0xC2, 0xDA, 0xD6, 0x82, 0x75, 0x82, 0x67, 0xF9, 0xA2, 0xB1, 0xAF, 0x59, 0xA0, 0xC8, 0x54,
    };
    uint8_t ramInit[0x150] { // address begins at 0x1000
        0x3A, 0xF4, 0x53, 0x43, 0xBF, 0x26, 0x97, 0xBE, 0x9C, 0x29, 0x34, 0x38, 0x38, 0x85, 0x6A, 0x56,
        0xFD, 0xE0, 0x41, 0xDB, 0xB2, 0xC2, 0x55, 0xAC, 0xF6, 0xCF, 0xC1, 0x3E, 0x77, 0x60, 0x70, 0x18,
        0xC0, 0xE0, 0xEC, 0x19, 0x24, 0x03, 0x4D, 0xAE, 0x38, 0xC7, 0x6A, 0xEA, 0x0C, 0x36, 0x03, 0xF3,
        0xF6, 0x95, 0x42, 0xA4, 0x61, 0x02, 0xCA, 0x81, 0x7F, 0xA3, 0xFE, 0x7B, 0xDB, 0xAF, 0xC9, 0xB9,
        0x03, 0x83, 0xC3, 0x00, 0x06, 0x87, 0x01, 0x0C, 0x0E, 0x7B, 0xA1, 0x37, 0xE2, 0xD0, 0x0A, 0xE5,
        0x0E, 0x8C, 0x8E, 0x9C, 0x95, 0x93, 0x79, 0x80, 0xA2, 0x28, 0xBA, 0x5A, 0x25, 0x6A, 0xC6, 0x5C,
        0xAD, 0x3F, 0x3A, 0x3F, 0x1B, 0xBA, 0x56, 0xEA, 0x7E, 0x2F, 0x54, 0x77, 0x92, 0xB8, 0x84, 0x75,
        0x82, 0x67, 0xF9, 0xA2, 0xB1, 0xAF, 0x59, 0xA0, 0x75, 0x63, 0x3F, 0x7F, 0x9A, 0x0E, 0xFD, 0xD4,
        0xD0, 0xAF, 0xCA, 0xA5, 0x2F, 0x67, 0x0A, 0x0D, 0x0E, 0x4C, 0xA1, 0x9A, 0xD5, 0x36, 0x7C, 0xD0,
        0xB6, 0xAC, 0x70, 0x05, 0xAB, 0xFD, 0xF3, 0xB0, 0xCF, 0x1D, 0x18, 0x1C, 0x06, 0x30, 0x38, 0x3B,
        0xAA, 0x92, 0x60, 0x8A, 0x2D, 0xEF, 0x4A, 0x9E, 0x63, 0xB5, 0x2A, 0xAB, 0x51, 0x39, 0x02, 0x3E,
        0x8E, 0x19, 0xB3, 0x8F, 0x52, 0x27, 0x4A, 0x81, 0x48, 0xDF, 0x22, 0xDF, 0x9F, 0xF4, 0x25, 0x06,
        0x44, 0x0F, 0x53, 0x68, 0x70, 0x18, 0xC0, 0xE0, 0x30, 0x80, 0xC1, 0x19, 0x6E, 0x66, 0xD7, 0xDE,
        0x2F, 0x2C, 0x7F, 0xC7, 0xCF, 0x4B, 0x5F, 0x86, 0x03, 0x68, 0x0A, 0x90, 0xE2, 0xAE, 0xA5, 0xE9,
        0x1D, 0xF5, 0x62, 0xFC, 0x9A, 0x7F, 0x9A, 0xFE, 0xA6, 0xC5, 0x7E, 0xB5, 0x25, 0xF7, 0xE6, 0xC7,
        0xAC, 0x82, 0x70, 0xCA, 0xE3, 0xA4, 0xD6, 0x3F, 0xD9, 0x74, 0x4E, 0x71, 0x2F, 0xF3, 0xF7, 0x31,
        0x65, 0xCE, 0xAD, 0x35, 0x55, 0x2F, 0xB4, 0x7C, 0x47, 0xDE, 0xD0, 0x89, 0xF1, 0xFE, 0x92, 0x84,
        0x29, 0x01, 0x64, 0x9A, 0x5C, 0xC7, 0xB4, 0x07, 0xBA, 0xEF, 0x2B, 0x8D, 0x41, 0xBD, 0x87, 0x69,
        0x01, 0x64, 0xC0, 0xEA, 0x88, 0x88, 0xE4, 0x83, 0x73, 0x8E, 0x66, 0x36, 0x44, 0x78, 0xA5, 0xE2,
        0x06, 0x3C, 0x02, 0x77, 0xF2, 0x09, 0xBF, 0xC3, 0x50, 0x6E, 0x29, 0xA2, 0x71, 0x21, 0x00, 0x00,
        0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    uint8_t netValues[40];

    //     0 = clock
    //     1 = read
    //     2 = write
    //  3-16 = address bytes 0-13
    // 17-24 = data
    //    25 = memory request
    //    26 = refresh
    //    27 = machine cycle 1

    int oldClock;

    int currentTCycleHalf;
    int currentTCycle;
    int currentMCycle;

    int mType;
    int currentInstruction;
    int pc;

    uint16_t internalAddressBus;
    uint8_t internalDataBus;

    void initSystem() {
        memset(netValues, 3, 27 * sizeof(uint8_t));
        memset(&(netValues[1]), 2, 2 * sizeof(uint8_t));
        memset(&(netValues[25]), 2, 3 * sizeof(uint8_t));

        oldClock = netValues[0];

        currentTCycleHalf = -1;
        currentTCycle = 0;
        currentMCycle = 0;

        mType = 0;
        currentInstruction = -1;
        pc = 0;

        internalAddressBus = 0;
        internalDataBus = 0;
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

    void updateSystem() {
        if(netValues[0] != oldClock) {
            oldClock = netValues[0];

            currentTCycleHalf++;
            if(currentTCycleHalf == 2) {
                currentTCycleHalf = 0;
                currentTCycle++;
            }

            if(mType == 0) {
                if(currentTCycle == 2 && currentTCycleHalf == 0) {currentInstruction = getDataBus();}
                if(currentTCycle == 4) {
                    currentTCycle = 0;
                    pc++;

                    if(instructionMCycles[currentInstruction] ==  M_F) {
                        mType = 0;
                        currentMCycle = 0;
                        currentInstruction = -1;
                    }
                    else if(
                        instructionMCycles[currentInstruction] ==  M_FR ||
                        instructionMCycles[currentInstruction] ==  M_FRR ||
                        instructionMCycles[currentInstruction] ==  M_FRW ||
                        instructionMCycles[currentInstruction] ==  M_FTR ||
                        instructionMCycles[currentInstruction] ==  M_FQR
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
                if(currentTCycle == 3) {
                    currentTCycle = 0;
                    pc++;

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
                        instructionMCycles[currentInstruction] ==  M_FQR && currentMCycle < 4
                    ) {
                        mType = 1;
                        currentMCycle++;
                    }
                    else if(instructionMCycles[currentInstruction] ==  M_FRW) {
                        mType = 2;
                        currentMCycle++;
                    }
                }
            }
            if(mType == 2) {
                if(currentTCycle == 3) {
                    currentTCycle = 0;
                    pc++;
                    mType = 0;
                    currentMCycle = 0;
                    currentInstruction = -1;
                }
            }

            setAddressBus(0);
            setDataBus(0);

            if(mType == 0) {
                setControlValues(instructionFetchControlValues, (currentTCycleHalf + currentTCycle * 2) * 5);
                if(currentTCycle == 0 || currentTCycle == 1) {setAddressBus(pc);}
                //if(currentTCycle == 2 || currentTCycle == 3) {setAddressBus(0);}
            }
            if(mType == 1) {
                setControlValues(memoryReadControlValues, (currentTCycleHalf + currentTCycle * 2) * 5);
                setAddressBus(internalAddressBus);
            }
            if(mType == 2) {
                setControlValues(memoryWriteControlValues, (currentTCycleHalf + currentTCycle * 2) * 5);
                setAddressBus(internalAddressBus + 0x1000);
            }
        }

        if(netValues[1] == 3) { // read
            uint16_t addressValue = getAddressBus() & 0xFFF;
            
            if(netValues[16] == 3) { // ROM
                uint8_t dataValue = 0;
                if(addressValue < 0x120) {dataValue = romInit[addressValue];}

                setDataBus(dataValue);
            }
            else { // RAM
                uint8_t dataValue = 0;
                if(addressValue < 0x150) {dataValue = ramInit[addressValue];}

                setDataBus(dataValue);
            }
        }
    }
};

#endif
