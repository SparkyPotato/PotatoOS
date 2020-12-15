#include "Text/TextRenderer.h"

TextRenderer::TextRenderer(Framebuffer* framebuffer, PSF1Font* font)
	: m_Buffer(framebuffer), m_Font(font), m_Color(0xffffffff)
{}

void TextRenderer::Clear()
{
	uint32_t* pixel = (uint32_t*)m_Buffer->BaseAddress;
	for (uint32_t y = 0; y < m_Buffer->Height; y++)
	{
		for (uint32_t x = 0; x < m_Buffer->Width; x++)
		{
			*(uint32_t*)(pixel + x + y * m_Buffer->Stride) = 0xff000000;
		}
	}
}

void TextRenderer::PutChar(char c, uint32_t color)
{
	if (m_Cursor.Position.Y * 16 + 16 > m_Buffer->Height) { return; }
	if (m_Cursor.Position.X * 8 + 8 > m_Buffer->Width) { return; }

	if (c == '\n') { m_Cursor.Position.Y++; return; }
	if (c == '\r') { m_Cursor.Position.X = 0; return; }

	uint32_t* pixel = (uint32_t*)m_Buffer->BaseAddress;
	uint8_t* fontPtr = m_Font->Glyphs + c * m_Font->Header->CharSize;

	uint32_t yO = 16 * m_Cursor.Position.Y;
	uint32_t xO = 8 * m_Cursor.Position.X;
	for (uint32_t y = yO; y < yO + 16; y++)
	{
		for (uint32_t x = xO; x < xO + 8; x++)
		{
			if ((*fontPtr & (0b10000000 >> (x - xO))) > 0)
			{
				*(uint32_t*)(pixel + x + y * m_Buffer->Stride) = color;
			}
		}
		fontPtr++;
	}

	m_Cursor.Position.X++;
	if (16 * m_Cursor.Position.X > m_Buffer->Width)
	{
		m_Cursor.Position.Y++;
		m_Cursor.Position.X = 0;
	}
}

void TextRenderer::Print(const char *str)
{
	while (*str != '\0')
	{
		PutChar(*str, m_Color);
		str++;
	}
}
