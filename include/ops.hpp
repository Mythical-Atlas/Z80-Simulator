#ifndef OPS_H
#define OPS_H

#include <cstdint>

uint8_t getImmediateByte(class Circuit* circuit);

uint8_t getA(class Circuit* circuit);
void setA(class Circuit* circuit, uint8_t value);

void load8(uint8_t* input, uint8_t* output);
void load16(uint16_t* input, uint16_t* output);

#endif