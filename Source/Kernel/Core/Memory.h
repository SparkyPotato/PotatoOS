#pragma once

#include <stdint.h>

#include "Core/Bitmap.h"

struct MemoryDescriptor
{
	uint32_t Type;
	void* PhysicalAddress;
	void* VirtualAddress;
	uint64_t PageCount;
	uint64_t Attributes;
};

extern const char* MemoryMapTypes[];

namespace Memory
{
void Initialize(uint8_t* descriptor, uint64_t size, uint64_t descSize);

uint64_t GetTotalMemory();
uint64_t GetUsedMemory();
uint64_t GetReservedMemory();

void* GetPage();
void FreePage(void* page);

void LockPages(void* address, uint64_t pages = 1);
void FreePages(void* address, uint64_t pages = 1);
};
