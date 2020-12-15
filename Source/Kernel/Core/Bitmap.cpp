#include "Core/Bitmap.h"

#include "LibC/string.h"

Bitmap::Bitmap(uint8_t *buffer, uint64_t size)
	: m_Buffer(buffer), m_Size(size * 8)
{
	memset(buffer, 0, size);
}

void Bitmap::Set(uint8_t* buffer, uint64_t bufSize)
{
	m_Buffer = buffer;
	m_Size = bufSize;
}

bool Bitmap::Get(uint64_t index) const
{
	if (index > m_Size) { return false; }

	uint64_t byte = index / 8;
	uint64_t extra = index % 8;
	return m_Buffer[byte] & (1 << extra);
}

void Bitmap::Set(bool value, uint64_t index)
{
	if (index > m_Size) { return; }
	uint64_t byte = index / 8;
	uint64_t extra = index % 8;

	if (value) { m_Buffer[byte] = m_Buffer[byte] | (1 << extra); }
	else { m_Buffer[byte] = m_Buffer[byte] & ~(1 << extra); }
}

void Bitmap::Toggle(uint64_t index)
{
	if (index > m_Size) { return; }

	uint64_t byte = index / 8;
	uint64_t extra = index % 8;
	m_Buffer[byte] = m_Buffer[byte] ^ (1 << extra);
}
