#pragma once

#include <stdint.h>

class Bitmap
{
public:
	Bitmap() = default;
	Bitmap(uint8_t *buffer, uint64_t bufSize);

	void Set(uint8_t *buffer, uint64_t bufSize);

	bool Get(uint64_t index) const;
	void Set(bool value, uint64_t index);
	void Toggle(uint64_t index);

	uint64_t GetSize() const { return m_Size; }

private:
	uint8_t* m_Buffer = nullptr;
	uint64_t m_Size = 0;
};
