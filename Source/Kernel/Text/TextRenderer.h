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
    void Print(uint32_t color, const char* str);
    void Print(const char* str);

    void SetCursorPosition(Point position) { m_Cursor.Position = position; }
    Point GetCursorPosition() { return m_Cursor.Position; }

private:
    Framebuffer *m_Buffer;
    PSF1Font *m_Font;
    Cursor m_Cursor;
};
