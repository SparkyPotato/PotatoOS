#pragma once

#include <stdint.h>

typedef struct
{
    void *BaseAddress;
    uint64_t Size;
    uint32_t Width, Height;
    uint32_t Stride;
} Framebuffer;
