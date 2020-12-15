#pragma once

#include "Core/Output.h"
#include "Core/Math.h"

struct Cursor
{
    Point Position;
};

class TextRenderer
{
public:
    TextRenderer(Framebuffer* framebuffer, PSF1Font* font);

    void PutChar(char c, uint32_t color);
    void Print(const char* str);

    void SetCursorPosition(Point position) { m_Cursor.Position = position; }
    Point GetCursorPosition() { return m_Cursor.Position; }
    void SetColor(uint32_t color) { m_Color = color; }

private:
    Framebuffer *m_Buffer;
    PSF1Font *m_Font;
    Cursor m_Cursor;
    uint32_t m_Color;
};
