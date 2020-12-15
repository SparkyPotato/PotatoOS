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

namespace Memory
{
	uint64_t m_TotalMemory = 0, m_ReservedMemory = 0, m_UsedMemory = 0;;
Bitmap m_PageMap;
uint64_t m_Page = 0;

bool m_Initialized = false;
void Initialize(uint8_t* descriptor, uint64_t size, uint64_t descSize)
{
	if (m_Initialized) { return; }
	m_Initialized = true;

	for (uint64_t i = 0; i < size / descSize; i++)
	{
		MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(descriptor + i * descSize);
		m_TotalMemory += desc->PageCount;
		if (desc->Type != 7) { m_ReservedMemory += desc->PageCount; }
	}

	uint64_t pageBytes = m_TotalMemory / 8 + 1;
	m_TotalMemory *= 4096;
	m_ReservedMemory *= 4096;

	for (uint64_t i = 0; i < size / descSize; i++)
	{
		MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(descriptor + i * descSize);

		if (desc->Type == 7 && desc->PageCount * 4096 >= pageBytes)
		{
			m_PageMap.Set(reinterpret_cast<uint8_t*>(desc->PhysicalAddress), pageBytes);
			uint64_t startPage = uint64_t(desc->PhysicalAddress) / 4096;
			for (int64_t pageB = pageBytes; pageB > 0; pageB -= 4096)
			{
				m_UsedMemory += 4096;
				m_PageMap.Set(true, startPage);
				startPage++;
			}
			break;
		}
	}

	for (uint64_t i = 0; i < size / descSize; i++)
	{
		MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(descriptor + i * descSize);

		if (desc->Type != 7)
		{
			uint64_t startPage = uint64_t(desc->PhysicalAddress) / 4096;
			while (desc->PageCount > 0)
			{
				m_PageMap.Set(true, startPage);
				startPage++;
				desc->PageCount--;
			}
		}
	}
}

uint64_t GetTotalMemory()
{
	return m_TotalMemory;
}

uint64_t GetUsedMemory()
{
	return m_UsedMemory;
}

uint64_t GetReservedMemory()
{
	return m_ReservedMemory;
}

void* GetPage()
{
	uint64_t size = m_PageMap.GetSize();
	for (; m_Page < size; m_Page++)
	{
		if (!m_PageMap.Get(m_Page))
		{
			m_UsedMemory += 4096;
			m_PageMap.Set(true, m_Page);
			return reinterpret_cast<void*>(m_Page * 4096);
		}
	}

	return nullptr;
}

void FreePage(void* page)
{
	m_Page = uint64_t(page) / 4096;
	if (m_PageMap.Get(m_Page))
	{
		m_UsedMemory -= 4096;
		m_PageMap.Set(false, m_Page);
	}
}

void LockPages(void* address, uint64_t pages)
{
	uint64_t page = uint64_t(address) / 4096;
	while (pages > 0)
	{
		if (!m_PageMap.Get(page))
		{
			m_UsedMemory += 4096;
			m_PageMap.Set(true, page);
		}

		pages--;
		page++;
	}
}

void FreePages(void* address, uint64_t pages)
{
	uint64_t page = m_Page = uint64_t(address) / 4096;
	while (pages > 0)
	{
		if (m_PageMap.Get(page))
		{
			m_UsedMemory -= 4096;
			m_PageMap.Set(false, page);
		}

		pages--;
		page++;
	}
}
}
