#pragma once

#include <cstdint>

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
    static const char *Format(int64_t value);
};

template <>
class Formatter<int32_t>
{
public:
    static const char *Format(int32_t value);
};

template <>
class Formatter<int16_t>
{
public:
    static const char *Format(int16_t value);
};

template <>
class Formatter<int8_t>
{
public:
    static const char *Format(int8_t value);
};
