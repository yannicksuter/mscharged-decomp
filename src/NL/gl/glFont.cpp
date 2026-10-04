#include "NL/gl/glDraw2.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glView.h"
#include "NL/gl/glMemory.h"
#include "NL/glx/glxFont.h"
#include "NL/gl/gl.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <stdarg.h>
#include "NL/nlstring_tmpl.h"
#include "Game/UnidentifiedStaticStorage.h"

enum eGLFont
{
    GLFONT_Small,
    GLFONT_Medium,
    GLFONT_Large,
    GLFONT_Count,
};

char sLargeFontName[] __attribute__((aligned(4))) = "font/fixedWidthLarge";
char sMediumFontName[] __attribute__((aligned(4))) = "font/fixedWidthMedium";
char sSmallFontName[] __attribute__((aligned(4))) = "font/fixedWidthSmall";

const char* sLargeFontTextureName = sLargeFontName;
const char* sMediumFontTextureName = sMediumFontName;
const char* sSmallFontTextureName = sSmallFontName;

#include "NL/gl/font_data.h"

int sFontCharacterWidth[3] = { 8, 9, 11 };
int sFontCharacterHeight[3] = { 12, 15, 18 };
int sFontCellWidth[3] = { 9, 10, 12 };
int sFontCellHeight[3] = { 13, 16, 20 };
int sFontTextureWidth[3] = { 128, 128, 128 };
int sFontTextureHeight[3] = { 256, 256, 256 };
int sFontCharactersPerRow[4] = { 14, 12, 10, 0 };

const char* sFontTextureNames[4] = { sSmallFontTextureName, sMediumFontTextureName, sLargeFontTextureName };
glPoly2 sFontPolys[128];

float sFontOffsetX = 0.5f;
float sFontOffsetY = 0.5f;
char sFontResourceName[] = "RLFont";

int sDefaultFont;
float sFontExtraWidth;
float sFontExtraHeight;
int sCurrentFont;
float sFontZ;
bool sFontInsideBegin;
bool sFontDropShadow;
bool sFontEnabled;
bool sFontVirtualCoords;

void glFontBlitCharacter(int x, int y, char character, unsigned short* image, int imageWidth, int font)
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

    const int endX = x + sFontCharacterWidth[font];
    const int endY = y + sFontCharacterHeight[font];

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
    glBeginResource(sFontResourceName);

    for (eGLFont font = GLFONT_Small; font < GLFONT_Count; font = (eGLFont)(font + 1))
    {
        unsigned long texture = glGetTexture(sFontTextureNames[font]);
        int width = sFontTextureWidth[font];
        int height = sFontTextureHeight[font];
        unsigned long imageSize = width * height * sizeof(unsigned short);
        unsigned short* image = (unsigned short*)nlMalloc(imageSize, 8, false);
        nlZeroMemory(image, imageSize);

        int x = 0;
        int y = 0;
        for (int character = 0; character < 0x5E; ++character)
        {
            glFontBlitCharacter(x, y, character + 0x20, image, width, font);
            x += sFontCellWidth[font];
            if (x + sFontCellWidth[font] >= width)
            {
                x = 0;
                y += sFontCellHeight[font];
            }
        }

        unsigned long platformTexture = glplatCreateFont(width, height, image,
            texture, (MemoryAllocator*)glGetCurrentResourcePool());
        glRegisterTexture(texture, (PlatTexture*)platformTexture,
            (MemoryAllocator*)glGetCurrentResourcePool());
        delete[] image;
    }

    sFontInsideBegin = false;
    sFontEnabled = true;
    sFontDropShadow = false;
    sFontVirtualCoords = true;
    glEndResource();
}

int glFontSetFont(int font)
{
    int previous = sCurrentFont;
    if (font == 4)
    {
        font = sDefaultFont;
    }
    sCurrentFont = font;
    return previous;
}

int glFontGetScreenHeight(GLView* renderView)
{
    return ((int)glViewGetOrthographicHeight(renderView) - 100) / sFontCellHeight[sCurrentFont];
}

void glFontVirtualPosToScreenCoordPos(float x, float y, float& outX, float& outY)
{
    int font = sCurrentFont;
    outX = x * sFontCellWidth[font] + 45.0f;
    outY = y * sFontCellHeight[font] + 50.0f;
}

void glFontBegin(bool drop)
{
    if (sFontEnabled)
    {
        unsigned long texture = glGetTexture(sFontTextureNames[sCurrentFont]);
        glSetDefaultState(false);
        glSetCurrentTexture(texture, GLTT_Diffuse);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetCurrentRasterState(glHandleizeRasterState());
        sFontDropShadow = drop;
        sFontInsideBegin = true;
    }
}

void glFontEnd()
{
    if (sFontEnabled)
    {
        sFontInsideBegin = false;
    }
}

static void _Putchar(glPoly2& poly, void*, float sx, float sy, int characterIndex, const nlColour& colour, int font)
{
    const int charactersPerRow = sFontCharactersPerRow[font];
    float s = (float)((characterIndex % charactersPerRow) * sFontCellWidth[font]);
    float t = (float)((characterIndex / charactersPerRow) * sFontCellHeight[font]);
    float inverseTextureWidth = 1.0f / (float)sFontTextureWidth[font];
    float inverseTextureHeight = 1.0f / (float)sFontTextureHeight[font];
    float characterWidth = (float)sFontCharacterWidth[font];
    float characterHeight = (float)sFontCharacterHeight[font];

    poly.m_uv[0].x = s * inverseTextureWidth;
    poly.m_uv[0].y = t * inverseTextureHeight;
    poly.m_uv[1].x = s * inverseTextureWidth;
    poly.m_uv[1].y = (t + characterHeight) * inverseTextureHeight;
    poly.m_uv[2].x = (s + characterWidth) * inverseTextureWidth;
    poly.m_uv[2].y = (t + characterHeight) * inverseTextureHeight;
    poly.m_uv[3].x = (s + characterWidth) * inverseTextureWidth;
    poly.m_uv[3].y = t * inverseTextureHeight;

    float y;
    float x = sx + sFontOffsetX;
    y = sy + sFontOffsetY;
    nlVec2Set(poly.m_pos[0], x, y);
    nlVec2Set(poly.m_pos[1], x, y + (float)sFontCharacterHeight[font] + sFontExtraHeight);
    nlVec2Set(poly.m_pos[2], x + (float)sFontCharacterWidth[font] + sFontExtraWidth, y + (float)sFontCharacterHeight[font] + sFontExtraHeight);
    nlVec2Set(poly.m_pos[3], x + (float)sFontCharacterWidth[font] + sFontExtraWidth, y);

    poly.depth = sFontZ;
    poly.m_colour[0] = colour;
    poly.m_colour[1] = colour;
    poly.m_colour[2] = colour;
    poly.m_colour[3] = colour;
}

int glFontPrint(void* renderView, eGLView view, int virtualX, int virtualY, const nlColour& colour, const char* str)
{
    int font = sCurrentFont;
    if (nlStrLen(str) == 0)
    {
        return 0;
    }
    if (!sFontEnabled)
    {
        return 0;
    }

    int screenX;
    int screenY;
    if (sFontVirtualCoords)
    {
        screenX = virtualX * sFontCellWidth[font] + 45;
        screenY = virtualY * sFontCellHeight[font] + 50;
    }
    else
    {
        screenX = virtualX;
        screenY = virtualY;
    }

    nlStrLen(str);
    int numChars = 0;
    glPoly2* poly = sFontPolys;
    const char* current = str;
    while (*current != '\0')
    {
        if (*current >= 0x20 && *current <= 0x7E)
        {
            _Putchar(*poly, renderView, (float)screenX, (float)screenY, *current - 0x20, colour, sCurrentFont);
            ++poly;
            ++numChars;
        }
        else if (*current == '\n')
        {
            screenY += sFontCellHeight[font];
            screenX = 45 - sFontCellWidth[font];
        }
        screenX += sFontCellWidth[font];
        ++current;
    }

    if (sFontDropShadow)
    {
        poly = sFontPolys;
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

        glAttachPoly2((GLView*)renderView, view, numChars, sFontPolys, 0);

        poly = sFontPolys;
        for (int i = 0; i < numChars; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                poly[i].m_colour[j] = colour;
                poly[i].m_pos[j].x -= 3.0f;
                poly[i].m_pos[j].y -= 3.0f;
            }
            poly[i].depth = sFontZ;
        }
    }

    glAttachPoly2((GLView*)renderView, view, numChars, sFontPolys, 0);
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

    if (!sFontEnabled)
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

    if (!sFontEnabled)
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

    if (!sFontEnabled)
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
    bool previous = sFontVirtualCoords;
    sFontVirtualCoords = virtualCoordinates;
    return previous;
}

