#ifndef HASH_H
#define HASH_H
#include <Arduino.h>
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
uint32_t SuperFastHash(uint8_t *data, int len);
#endif