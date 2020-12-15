#pragma once

#include <stdint.h>

typedef struct
{
	void *BaseAddress;
	uint64_t Size;
	uint32_t Width, Height;
	uint32_t Stride;
} Framebuffer;

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

typedef struct
{
	uint32_t X;
	uint32_t Y;
} CursorPosition;

typedef struct
{
	Framebuffer* Buffer;
	PSF1Font* Font;

	CursorPosition Cursor;
} TextOutput;

TextOutput InitializeTextOutput(Framebuffer* framebuffer, PSF1Font* font);
void PutChar(TextOutput* this, char c, uint32_t color);
void Print(TextOutput* this, const char* str, uint32_t color);
