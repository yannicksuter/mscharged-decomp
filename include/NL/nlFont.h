#ifndef NL_FONT_H
#define NL_FONT_H

#include "NL/gl/gl.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTextEscape.h"

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

enum TextureType
{
    InvalidTextureType = 0,
    Colour = 1,
    Greyscale = 2,
    SplitFX = 3,
};

enum Distribution
{
    InvalidDistribution = 0,
    English = 1,
    InOrder = 2,
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

    struct ScissorBox
    {
        /* 0x0 */ unsigned short X;
        /* 0x2 */ unsigned short Y;
        /* 0x4 */ unsigned short Width;
        /* 0x6 */ unsigned short Height;
    };

    struct GlyphInfo
    {
        GlyphInfo() { nlVec2Set(uv, -1.0f, -1.0f); }

        /* 0x00 */ nlVector2 uv;
        /* 0x08 */ unsigned char _pad_08[8];
        /* 0x10 */ unsigned char Advance;
        /* 0x11 */ unsigned char RenderWidth;
        /* 0x12 */ unsigned char _pad_12[2];
        /* 0x14 */ signed char Offset;
        /* 0x15 */ unsigned char Page : 4;
        /* 0x15 */ unsigned char HasKernPairs : 1;
        union
        {
            /* 0x16 */ unsigned short UnicodeChar;
            /* 0x16 */ unsigned short hash;
        };

        operator unsigned long() const { return UnicodeChar; }
    };

    struct KernPair
    {
        union
        {
            struct
            {
                /* 0x0 */ unsigned short A;
                /* 0x2 */ unsigned short B;
            } s;
            /* 0x0 */ unsigned long hash;
        };
        /* 0x4 */ int Kern;

        operator unsigned long() const { return hash; }
    };

    unsigned long GetCharWidth(unsigned short FontChar, unsigned short PrevFontChar) const;
    void DisableScissorBox() const;
    void SetScissorBox(const ScissorBox& other) const;
    void DrawString(eGLView View, const FontCharString& Text, const nlVector2& Position, const nlColour& Colour, const nlColour& EffectColour, int Length, nlFont::TextPass Passes, bool FlipY, unsigned long* pMatrix, nlColour* pOverrideColour) const;
    unsigned char Load(const char* szFontName, char* pFontDescData, unsigned long HashId);

    ~nlFont();
    nlFont();

    /* 0x000 */ unsigned long m_PageCount;
    /* 0x004 */ unsigned long m_TextureHandles[16];
    /* 0x044 */ unsigned long m_EffectTextureHandles[16];
    /* 0x084 */ TextureType m_TextureType;
    /* 0x088 */ mutable unsigned char m_bScissorBox;
    /* 0x089 */ unsigned char _pad_089;
    /* 0x08A */ mutable ScissorBox m_scissorBox;
    /* 0x092 */ unsigned char _pad_092[2];
    /* 0x094 */ Distribution m_Distribution;
    /* 0x098 */ unsigned long m_CharacterSet;
    /* 0x09C */ unsigned long m_PageSize;
    /* 0x0A0 */ char m_FontName[32];
    /* 0x0C0 */ TextMetrics m_Metrics;
    /* 0x0D4 */ float m_InvTexSize;
    /* 0x0D8 */ GlyphInfo m_GlyphLookup[95];
    /* 0x9C0 */ GlyphInfo* m_pExtendedGlyphs;
    /* 0x9C4 */ unsigned long m_ExtendedGlyphCount;
    /* 0x9C8 */ KernPair* m_pKernTable;
    /* 0x9CC */ unsigned long m_KernTableSize;
};

class FontCharString
{
public:
    FontCharString() { }
    template <typename T>
    FontCharString(const T*, const nlFont*, T*);
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

template <typename T>
inline FontCharString::FontCharString(const T* Source, const nlFont* pFont, T* pBuffer)
{
    m_InternalBuffer = 0;
    if (pBuffer == 0)
    {
        m_pString = (unsigned short*)nlMalloc((nlStrLen<T>(Source) + 1) * sizeof(T), 8, false);
        m_InternalBuffer = 1;
    }
    else
    {
        m_pString = pBuffer;
    }

    unsigned short* dest = m_pString;
    const T* src = Source;
    unsigned short escBegin = nlEscapeSequence::ESCAPE_BEGIN;
    unsigned short ch;

    while ((ch = *src) != 0)
    {
        if (ch == escBegin)
        {
            nlEscapeSequence EscSeq(src);
            const T* end = (const T*)EscSeq.m_pEnd;
            while (src < end)
            {
                *dest++ = *src++;
            }
        }
        else
        {
            if (ch <= 0x7F)
            {
                ch &= 0xFFFF;
            }
            else
            {
                nlFont::GlyphInfo key;
                key.UnicodeChar = ch;
                nlFont::GlyphInfo* result;
                if (pFont->m_pExtendedGlyphs != 0 && pFont->m_ExtendedGlyphCount != 0 && (result = nlBSearch<nlFont::GlyphInfo, nlFont::GlyphInfo>(key, pFont->m_pExtendedGlyphs, pFont->m_ExtendedGlyphCount)) != 0)
                {
                    ch = ((result - pFont->m_pExtendedGlyphs) + 0x80) & 0xFFFF;
                }
                else
                {
                    ch = 0x3F;
                }
            }
            *dest++ = ch;
            src++;
        }
    }

    *dest = 0;
}

#endif // NL_FONT_H
