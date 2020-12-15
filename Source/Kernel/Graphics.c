#include "Kernel/Graphics.h"

void PutChar(TextOutput* this, char c, uint32_t color)
{
	if (c == '\n') { this->Cursor.Y++; return; }
	if (c == '\r') { this->Cursor.X = 0; return; }

	uint32_t* pixel = (uint32_t*)this->Buffer->BaseAddress;
	uint8_t* fontPtr = this->Font->Glyphs + c * this->Font->Header->CharSize;

	uint32_t yO = 16 * this->Cursor.Y;
	uint32_t xO = 8 * this->Cursor.X;
	for (uint32_t y = yO; y < yO + 16; y++)
	{
		for (uint32_t x = xO; x < xO + 8; x++)
		{
			if ((*fontPtr & (0b10000000 >> (x - xO))) > 0)
			{
				*(uint32_t*)(pixel + x + y * this->Buffer->Stride) = color;
			}
		}
		fontPtr++;
	}

	this->Cursor.X++;
	if (16 * this->Cursor.X > this->Buffer->Width)
	{
		this->Cursor.Y++;
		this->Cursor.X = 0;
	}
}

void Print(TextOutput* this, const char* str, uint32_t color)
{
	while (*str != '\0')
	{
		PutChar(this, *str, color);
		str++;
	}
}

TextOutput InitializeTextOutput(Framebuffer* framebuffer, PSF1Font* font)
{
	TextOutput out;
	out.Buffer = framebuffer;
	out.Font = font;
	out.Cursor.X = 0;
	out.Cursor.Y = 0;

	return out;
}
