#pragma once

#include <cstddef>
#include <cstdint>

struct Framebuffer
{
	void *BaseAddress;
	uint64_t Size;
	uint32_t Width, Height;
	uint32_t Stride;
};

struct PSF1Header
{
	uint8_t Magic[2];
	uint8_t Mode;
	uint8_t CharSize;
};

struct PSF1Font
{
	PSF1Header* Header;
	uint8_t* Glyphs;
};
