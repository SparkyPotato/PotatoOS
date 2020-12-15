#include "LibC/string.h"

void memset(void *dest, uint8_t value, uint64_t size)
{
    uint64_t val = value;
    uint64_t packed = val | (val << 8) | (val << 16) | (val << 24) |
        (val << 32) | (val << 40) | (val << 48) | (val << 56);

    uint64_t loops = size / 8;
    uint64_t* d = reinterpret_cast<uint64_t*>(dest);

    for (uint64_t i = 0; i < loops; i++) { d[i] = packed; }
    loops--;
    uint8_t* dS = reinterpret_cast<uint8_t*>(d + loops);
    for (uint64_t i = 0; i < size - loops; i++) { dS[i] = value; }
}
