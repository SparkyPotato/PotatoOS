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

class Memory
{
public:
    Memory(uint8_t* descriptor, uint64_t size, uint64_t descSize);

    uint64_t GetTotalMemory() { return m_TotalMemory; }
    uint64_t GetAvailableMemory() { return m_AvailableMemory; }

private:
    uint64_t m_TotalMemory = 0, m_AvailableMemory = 0;
};
