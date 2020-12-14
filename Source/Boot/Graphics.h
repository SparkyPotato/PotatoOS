#pragma once

#include <stdint.h>

typedef struct
{
    void *BaseAddress;
    size_t Size;
    uint32_t Width, Height;
    uint32_t Stride;
} Framebuffer;

Framebuffer InitializeGraphics();
