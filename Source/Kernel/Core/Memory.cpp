#include "Core/Memory.h"

const char* MemoryMapTypes[] =
{
    "Reserved Memory",
    "Loader Code",
    "Loader Data",
    "Boot Services Code",
    "Boot Services Data",
    "Runtime Services Code",
    "Runtime Services Data",
    "Conventional Memory",
    "Unusable Memory",
    "ACPI Reclaimable Memory",
    "ACPI Memory NVS",
    "Memory Mapped IO",
    "Memory Mapped IO Port Space",
    "PAL Code"
};

Memory::Memory(uint8_t* descriptor, uint64_t size, uint64_t descSize)
{
    for (uint64_t i = 0; i < size / descSize; i++)
    {
        MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(descriptor + i * descSize);
        m_TotalMemory += desc->PageCount;
        if (desc->Type == 7) { m_AvailableMemory += desc->PageCount; }
    }

    m_AvailableMemory *= 4;
    m_TotalMemory *= 4;
}
