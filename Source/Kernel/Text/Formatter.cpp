#include "Text/Formatter.h"

const char* Format(uint64_t value)
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

const char* Format(int64_t value)
{
	static char buffer[64];

	bool isNeg = 0;
	if (value < 0) { isNeg = 1; value *= -1; }
	buffer[0] = '-';

	uint8_t size = isNeg;
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

const char* Format(double value, uint8_t precision)
{
	static char buffer[64];
	const char* integer = Format(int64_t(value));
	char* insert = buffer;
	
	if (value < 0) { value *= -1; }

	while (*integer != '\0') { *insert = *integer; integer++; insert++; }
	*insert = '.';
	insert++;

	value -= int64_t(value);
	
	for (uint8_t i = 0; i < precision; i++)
	{
		value *= 10;
		*insert = int64_t(value) + '0';
		insert++;
		value -= int64_t(value);
	}
	*insert = '\0';

	return buffer;
}

const char* Formatter<uint64_t>::Format(uint64_t value)
{
	return ::Format(value);
}

const char* Formatter<uint32_t>::Format(uint32_t value)
{
	return ::Format(uint64_t(value));
}

const char* Formatter<uint16_t>::Format(uint16_t value)
{
	return ::Format(uint64_t(value));
}

const char* Formatter<uint8_t>::Format(uint8_t value)
{
	return ::Format(uint64_t(value));
}

const char* Formatter<int64_t>::Format(int64_t value)
{
	return ::Format(value);
}

const char* Formatter<int32_t>::Format(int32_t value)
{
	return ::Format(int64_t(value));
}

const char* Formatter<int16_t>::Format(int16_t value)
{
	return ::Format(int64_t(value));
}

const char* Formatter<int8_t>::Format(int8_t value)
{
	return ::Format(int64_t(value));
}

const char* Formatter<double>::Format(double value)
{
	return ::Format(value, 5);
}

const char* Formatter<float>::Format(float value)
{
	return ::Format(double(value), 5);
}
