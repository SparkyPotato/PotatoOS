#pragma once

#include <stdint.h>

struct MemoryDescriptor
{
    uint32_t Type;
    void* PhysicalAddress;
    void* VirtualAddress;
    uint64_t PageCount;
    uint64_t Attributes;
};

extern const char* MemoryMapTypes[];
