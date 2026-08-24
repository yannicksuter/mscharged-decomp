#ifndef NL_FONT_H
#define NL_FONT_H

#include "NL/gl/gl.h"
#include "NL/nlMath.h"

struct nlColour;
class FontCharString;

struct TextMetrics
{
    /* 0x00 */ unsigned long FontName;
    /* 0x04 */ unsigned short Height;
    /* 0x06 */ unsigned short Ascent;
    /* 0x08 */ unsigned short InternalLeading;
    /* 0x0C */ float Spacing;
    /* 0x10 */ float LineHeight;
};

class nlFont
{
public:
    enum TextPass
    {
        PASS_None = 0,
        PASS_Text = 1,
        PASS_Effect = 2,
        PASS_TextAndEffect = 3,
    };

    unsigned long GetCharWidth(unsigned short FontChar, unsigned short PrevFontChar) const;
    void DrawString(eGLView View, const FontCharString& Text, const nlVector2& Position, const nlColour& Colour, const nlColour& EffectColour, int Length, nlFont::TextPass Passes, bool FlipY, unsigned long* pMatrix, nlColour* pOverrideColour) const;

    /* 0x000 */ unsigned char _pad[0xC0];
    /* 0x0C0 */ TextMetrics m_Metrics;
};

class FontCharString
{
public:
    FontCharString() { }
    ~FontCharString()
    {
        if (m_InternalBuffer != 0)
        {
            delete[] m_pString;
        }
    }

    /* 0x0 */ unsigned short* m_pString;
    /* 0x4 */ unsigned char m_InternalBuffer;
};

#endif // NL_FONT_H
