#pragma once

#include <stdint.h>

const char* Format(uint64_t value);
const char* Format(int64_t value);
const char* Format(double value, uint8_t precision);

template <typename T>
const char* FormatHex(T value)
{
	static char buffer[64];

	T* ptr = &value;
	uint8_t size = sizeof(T) * 2 - 1;

	for (uint8_t i = 0; i < size; i++)
	{
		uint8_t* p = ((uint8_t*)ptr + i);
		uint8_t temp = ((*p & 0xF0) >> 4);
		buffer[size - (i * 2 + 1)] = temp > 9 ? temp - 10 + 'A' : temp + '0';
		temp = (*p & 0x0F);
		buffer[size - i * 2] = temp > 9 ? temp - 10 + 'A' : temp + '0';
	}

	buffer[size + 1] = '\0';
	return buffer;
}

template <typename T>
class Formatter
{
public:
    static const char* Format(const T& value)
    {
        return nullptr;
    }
};

template <>
class Formatter<uint64_t>
{
public:
    static const char* Format(uint64_t value);
};

template <>
class Formatter<uint32_t>
{
public:
    static const char* Format(uint32_t value);
};

template <>
class Formatter<uint16_t>
{
public:
    static const char* Format(uint16_t value);
};

template <>
class Formatter<uint8_t>
{
public:
    static const char* Format(uint8_t value);
};

template <>
class Formatter<int64_t>
{
public:
    static const char* Format(int64_t value);
};

template <>
class Formatter<int32_t>
{
public:
    static const char* Format(int32_t value);
};

template <>
class Formatter<int16_t>
{
public:
    static const char* Format(int16_t value);
};

template <>
class Formatter<int8_t>
{
public:
    static const char* Format(int8_t value);
};

template <>
class Formatter<double>
{
public:
    static const char* Format(double value);
};

template <>
class Formatter<float>
{
public:
    static const char* Format(float value);
};
