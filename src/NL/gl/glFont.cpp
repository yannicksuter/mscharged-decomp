#include "NL/gl/glDraw2.h"
#include "NL/gl/glFont.h"
#include "NL/glx/glxFont.h"
#include "NL/gl/gl.h"
#include "NL/gl/glState.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <stdarg.h>

extern "C" void fn_802C8280(const char* name);
extern "C" void fn_802C8288();
extern "C" bool fn_802C8874(void* renderView, eGLView view, unsigned long numPolys, glPoly2* polys, const void* userData);
extern "C" bool fn_802CC094();
extern "C" void fn_802CDF14(unsigned long texture, unsigned long platformTexture, bool permanent);
extern "C" float fn_802CE7B0(void* renderView);

enum eGLFont
{
    GLFONT_Small,
    GLFONT_Medium,
    GLFONT_Large,
    GLFONT_Count,
};

extern "C"
{
    char sLargeFontName[] __attribute__((aligned(4))) = "font/fixedWidthLarge";
    char sMediumFontName[] __attribute__((aligned(4))) = "font/fixedWidthMedium";
    char sSmallFontName[] __attribute__((aligned(4))) = "font/fixedWidthSmall";

    const char* lbl_806DF3D0 = sLargeFontName;
    const char* lbl_806DF3D4 = sMediumFontName;
    const char* lbl_806DF3D8 = sSmallFontName;
}

#include "NL/gl/font_data.h"

extern "C"
{
    int lbl_8052E1C8[3] = { 8, 9, 11 };
    int lbl_8052E1D4[3] = { 12, 15, 18 };
    int lbl_8052E1E0[3] = { 9, 10, 12 };
    int lbl_8052E1EC[3] = { 13, 16, 20 };
    int lbl_8052E1F8[3] = { 128, 128, 128 };
    int lbl_8052E204[3] = { 256, 256, 256 };
    int lbl_8052E210[4] = { 14, 12, 10, 0 };

    const char* lbl_8057C798[4] = { lbl_806DF3D8, lbl_806DF3D4, lbl_806DF3D0 };
    glPoly2 lbl_8057C7A8[128];

    float lbl_806DF3DC = 0.5f;
    float lbl_806DF3E0 = 0.5f;
    char lbl_806DF3E4[] = "RLFont";

    int lbl_806E1EB8;
    float lbl_806E1EBC;
    float lbl_806E1EC0;
    int lbl_806E1EC4;
    float lbl_806E1EC8;
    bool lbl_806E1ECC;
    bool lbl_806E1ECD;
    bool lbl_806E1ECE;
    bool lbl_806E1ECF;
}

// The original type identity of this common weak static is not yet known.
struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

extern "C" void fn_802C9A0C(int x, int y, char character, unsigned short* image, int imageWidth, int font)
{
    unsigned short* characterData;

    switch (font)
    {
    case 0:
        characterData = (unsigned short*)((unsigned char*)sSmallFontData + (character - 0x20) * 0x18);
        break;
    case 1:
        characterData = (unsigned short*)((unsigned char*)sMediumFontData + (character - 0x20) * 0x1E);
        break;
    case 2:
        characterData = (unsigned short*)((unsigned char*)sLargeFontData + (character - 0x20) * 0x24);
        break;
    default:
        characterData = 0;
        break;
    }

    const int endX = x + lbl_8052E1C8[font];
    const int endY = y + lbl_8052E1D4[font];

    for (int imageY = y; imageY < endY; ++imageY)
    {
        unsigned char* row = (unsigned char*)(characterData + (imageY - y));

        for (int imageX = x; imageX < endX; ++imageX)
        {
            const int bit = imageX - x;
            bool set;
            if (bit < 8)
            {
                set = ((row[0] << bit) >> 7) & 1;
            }
            else
            {
                set = ((row[1] << (bit - 8)) >> 7) & 1;
            }

            if (set)
            {
                image[imageY * imageWidth + imageX] = 0xFFFF;
            }
        }
    }
}

void gl_FontStartup()
{
    fn_802C8280(lbl_806DF3E4);

    for (eGLFont font = GLFONT_Small; font < GLFONT_Count; font = (eGLFont)(font + 1))
    {
        unsigned long texture = glGetTexture(lbl_8057C798[font]);
        int width = lbl_8052E1F8[font];
        int height = lbl_8052E204[font];
        unsigned long imageSize = width * height * sizeof(unsigned short);
        unsigned short* image = (unsigned short*)nlMalloc(imageSize, 8, false);
        nlZeroMemory(image, imageSize);

        int x = 0;
        int y = 0;
        for (int character = 0; character < 0x5E; ++character)
        {
            fn_802C9A0C(x, y, character + 0x20, image, width, font);
            x += lbl_8052E1E0[font];
            if (x + lbl_8052E1E0[font] >= width)
            {
                x = 0;
                y += lbl_8052E1EC[font];
            }
        }

        unsigned long platformTexture = glplatCreateFont(width, height, image, texture, fn_802CC094());
        fn_802CDF14(texture, platformTexture, fn_802CC094());
        delete[] image;
    }

    lbl_806E1ECC = false;
    lbl_806E1ECE = true;
    lbl_806E1ECD = false;
    lbl_806E1ECF = true;
    fn_802C8288();
}

extern "C" int fn_802C9CAC(int font)
{
    int previous = lbl_806E1EC4;
    if (font == 4)
    {
        font = lbl_806E1EB8;
    }
    lbl_806E1EC4 = font;
    return previous;
}

extern "C" int fn_802C9CC8(void* renderView)
{
    return ((int)fn_802CE7B0(renderView) - 100) / lbl_8052E1EC[lbl_806E1EC4];
}

void glFontVirtualPosToScreenCoordPos(float x, float y, float& outX, float& outY)
{
    int font = lbl_806E1EC4;
    outX = x * lbl_8052E1E0[font] + 45.0f;
    outY = y * lbl_8052E1EC[font] + 50.0f;
}

void glFontBegin(bool drop)
{
    if (lbl_806E1ECE)
    {
        unsigned long texture = glGetTexture(lbl_8057C798[lbl_806E1EC4]);
        glSetDefaultState(false);
        glSetCurrentTexture(texture, GLTT_Diffuse);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetCurrentRasterState(glHandleizeRasterState());
        lbl_806E1ECD = drop;
        lbl_806E1ECC = true;
    }
}

void glFontEnd()
{
    if (lbl_806E1ECE)
    {
        lbl_806E1ECC = false;
    }
}

static void _Putchar(glPoly2& poly, void*, float sx, float sy, int characterIndex, const nlColour& colour, int font)
{
    const int charactersPerRow = lbl_8052E210[font];
    float s = (float)((characterIndex % charactersPerRow) * lbl_8052E1E0[font]);
    float t = (float)((characterIndex / charactersPerRow) * lbl_8052E1EC[font]);
    float inverseTextureWidth = 1.0f / (float)lbl_8052E1F8[font];
    float inverseTextureHeight = 1.0f / (float)lbl_8052E204[font];
    float characterWidth = (float)lbl_8052E1C8[font];
    float characterHeight = (float)lbl_8052E1D4[font];

    poly.m_uv[0].x = s * inverseTextureWidth;
    poly.m_uv[0].y = t * inverseTextureHeight;
    poly.m_uv[1].x = s * inverseTextureWidth;
    poly.m_uv[1].y = (t + characterHeight) * inverseTextureHeight;
    poly.m_uv[2].x = (s + characterWidth) * inverseTextureWidth;
    poly.m_uv[2].y = (t + characterHeight) * inverseTextureHeight;
    poly.m_uv[3].x = (s + characterWidth) * inverseTextureWidth;
    poly.m_uv[3].y = t * inverseTextureHeight;

    float y;
    float x = sx + lbl_806DF3DC;
    y = sy + lbl_806DF3E0;
    nlVec2Set(poly.m_pos[0], x, y);
    nlVec2Set(poly.m_pos[1], x, y + (float)lbl_8052E1D4[font] + lbl_806E1EC0);
    nlVec2Set(poly.m_pos[2], x + (float)lbl_8052E1C8[font] + lbl_806E1EBC, y + (float)lbl_8052E1D4[font] + lbl_806E1EC0);
    nlVec2Set(poly.m_pos[3], x + (float)lbl_8052E1C8[font] + lbl_806E1EBC, y);

    poly.depth = lbl_806E1EC8;
    poly.m_colour[0] = colour;
    poly.m_colour[1] = colour;
    poly.m_colour[2] = colour;
    poly.m_colour[3] = colour;
}

int glFontPrint(void* renderView, eGLView view, int virtualX, int virtualY, const nlColour& colour, const char* str)
{
    int font = lbl_806E1EC4;
    if (nlStrLen(str) == 0)
    {
        return 0;
    }
    if (!lbl_806E1ECE)
    {
        return 0;
    }

    int screenX;
    int screenY;
    if (lbl_806E1ECF)
    {
        screenX = virtualX * lbl_8052E1E0[font] + 45;
        screenY = virtualY * lbl_8052E1EC[font] + 50;
    }
    else
    {
        screenX = virtualX;
        screenY = virtualY;
    }

    nlStrLen(str);
    int numChars = 0;
    glPoly2* poly = lbl_8057C7A8;
    const char* current = str;
    while (*current != '\0')
    {
        if (*current >= 0x20 && *current <= 0x7E)
        {
            _Putchar(*poly, renderView, (float)screenX, (float)screenY, *current - 0x20, colour, lbl_806E1EC4);
            ++poly;
            ++numChars;
        }
        else if (*current == '\n')
        {
            screenY += lbl_8052E1EC[font];
            screenX = 45 - lbl_8052E1E0[font];
        }
        screenX += lbl_8052E1E0[font];
        ++current;
    }

    if (lbl_806E1ECD)
    {
        poly = lbl_8057C7A8;
        for (int i = 0; i < numChars; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                poly[i].m_colour[j].c[0] = 0;
                poly[i].m_colour[j].c[1] = 0;
                poly[i].m_colour[j].c[2] = 0;
                poly[i].m_colour[j].c[3] = 0xFF;
                poly[i].m_pos[j].x += 3.0f;
                poly[i].m_pos[j].y += 3.0f;
            }
            poly[i].depth += -0.001f;
        }

        fn_802C8874(renderView, view, numChars, lbl_8057C7A8, 0);

        poly = lbl_8057C7A8;
        for (int i = 0; i < numChars; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                poly[i].m_colour[j] = colour;
                poly[i].m_pos[j].x -= 3.0f;
                poly[i].m_pos[j].y -= 3.0f;
            }
            poly[i].depth = lbl_806E1EC8;
        }
    }

    fn_802C8874(renderView, view, numChars, lbl_8057C7A8, 0);
    return numChars;
}

int glFontPrint(void* renderView, eGLView view, int x, int y, const char* str)
{
    nlColour colour;
    colour.c[0] = 0xFF;
    colour.c[1] = 0xFF;
    colour.c[2] = 0xFF;
    colour.c[3] = 0xFF;
    return glFontPrint(renderView, view, x, y, colour, str);
}

struct FontStringBuffer
{
    char text[0x80];
};

int glFontPrintf(void* renderView, int x, int y, const char* format, ...)
{
    FontStringBuffer string;
    va_list args;

    if (!lbl_806E1ECE)
    {
        return 0;
    }

    va_start(args, format);
    nlVSNPrintf(string.text, sizeof(string.text), format, args);
    va_end(args);

    nlColour colour;
    colour.c[0] = 0xFF;
    colour.c[1] = 0xFF;
    colour.c[2] = 0xFF;
    colour.c[3] = 0xFF;
    return glFontPrint(renderView, (eGLView)0, x, y, colour, string.text);
}

int glFontPrintf(void* renderView, int x, int y, const nlColour& colour, const char* format, ...)
{
    va_list args;
    char string[0x84];

    if (!lbl_806E1ECE)
    {
        return 0;
    }

    va_start(args, format);
    nlVSNPrintf(string, 0x80, format, args);
    va_end(args);

    return glFontPrint(renderView, (eGLView)0, x, y, colour, string);
}

int glFontPrintf(void* renderView, eGLView view, int x, int y, const nlColour& colour, const char* format, ...)
{
    va_list args;
    char string[0x84];

    if (!lbl_806E1ECE)
    {
        return 0;
    }

    va_start(args, format);
    nlVSNPrintf(string, 0x80, format, args);
    va_end(args);

    return glFontPrint(renderView, view, x, y, colour, string);
}

bool glFontVirtualCoordinates(bool virtualCoordinates)
{
    bool previous = lbl_806E1ECF;
    lbl_806E1ECF = virtualCoordinates;
    return previous;
}

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
