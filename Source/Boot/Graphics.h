#pragma once

#include <stdint.h>

#include "efi.h"
#include "efilib.h"

typedef struct
{
    void *BaseAddress;
    uint64_t Size;
    uint32_t Width, Height;
    uint32_t Stride;
} Framebuffer;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct
{
	uint8_t Magic[2];
	uint8_t Mode;
	uint8_t CharSize;
} PSF1Header;

typedef struct
{
	PSF1Header* Header;
	void* Glyphs;
} PSF1Font;

Framebuffer InitializeGraphics();

PSF1Font* LoadPSF1Font(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable);
