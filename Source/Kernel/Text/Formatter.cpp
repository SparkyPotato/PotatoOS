#include "Text/Formatter.h"

const char* Formatter<uint64_t>::Format(uint64_t value)
{
	static char buffer[64];

	uint8_t size = 0;
	uint64_t sizeTest = value;
	while (sizeTest / 10 > 0)
	{
		sizeTest /= 10;
		size++;
	}

	uint8_t index = 0;
	while (value / 10 > 0)
	{
		uint8_t modulo = value % 10;
		value /= 10;
		buffer[size - index] = modulo + '0';
		index++;
	}
	uint8_t modulo = value % 10;
	buffer[size - index] = modulo + '0';
	buffer[size + 1] = '\0';

	return buffer;
}

const char* Formatter<uint32_t>::Format(uint32_t value)
{
	return Formatter<uint64_t>::Format(uint64_t(value));
}

const char *Formatter<uint16_t>::Format(uint16_t value)
{
	return Formatter<uint64_t>::Format(uint64_t(value));
}

const char *Formatter<uint8_t>::Format(uint8_t value)
{
	return Formatter<uint64_t>::Format(uint64_t(value));
}

const char *Formatter<int64_t>::Format(int64_t value)
{
	static char buffer[64];

	bool isNeg = false;
	if (value < 0) { isNeg = true; value *= -1; }
	buffer[0] = '-';

	uint8_t size = isNeg;
	uint64_t sizeTest = value;
	while (sizeTest / 10 > 0)
	{
		sizeTest /= 10;
		size++;
	}

	uint8_t index = isNeg;
	while (value / 10 > 0)
	{
		uint8_t modulo = value % 10;
		value /= 10;
		buffer[size - index] = modulo + '0';
		index++;
	}
	uint8_t modulo = value % 10;
	buffer[size - index] = modulo + '0';
	buffer[size + 1] = '\0';

	return buffer;
}

const char *Formatter<int32_t>::Format(int32_t value)
{
	return Formatter<int64_t>::Format(int64_t(value));
}

const char *Formatter<int16_t>::Format(int16_t value)
{
	return Formatter<int64_t>::Format(int64_t(value));
}

const char *Formatter<int8_t>::Format(int8_t value)
{
	return Formatter<int64_t>::Format(int64_t(value));
}
