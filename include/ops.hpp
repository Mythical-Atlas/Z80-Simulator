#ifndef OPS_H
#define OPS_H

#include <cstdint>

uint8_t getImmediateByte(class Circuit* circuit);

uint8_t getA(class Circuit* circuit);
void setA(class Circuit* circuit, uint8_t value);

#endif