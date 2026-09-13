#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glState.h"
#include "NL/nlColour.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/Sys/DebugPolyList.h"
#include "unclassified/tu_802B7798.h"
#include "Game/Sys/tweak.h"

#include <stdarg.h>
#include <string.h>
#include "NL/nlstring_tmpl.h"

static nlColour DefaultScreenPrintfTextColour = { 0xFF, 0xFF, 0xFF, 0xFF };
static bool DefaultScreenPrintfHaveBackground = true;
static nlColour DefaultScreenPrintfBackgroundColour = { 0, 0, 0, 150 };
static float sfTextRectangleBorder = 4.0f;

static eGLView DefaultScreenPrintfGLView;

int nlScreenPrint(eGLView view, int x, int y, bool bFromTop,
    int font, const nlColour& backgroundColour, const nlColour& textColour,
    bool bHaveBackground, const char* format, va_list args)
{
    char printbuf[128];
    glStateBundle state;

    glFontSetFont(font);
    if (!bFromTop)
    {
        y = fn_802C9CC8(GetDebugFontView()) - y;
    }

    nlVSNPrintf(printbuf, sizeof(printbuf), format, args);
    glStateSave(state);

    if (bHaveBackground)
    {
        int length = nlStrLen(printbuf);
        float left = (float)x;
        float top = (float)y;
        float right = (float)(x + length);
        float bottom = (float)(y + 1);
        GLView* renderView = GetDebugFontView();
        DrawTextRectangle(renderView, backgroundColour, left, top, right, bottom, 0.0f, view - 1, true, true);
    }

    glFontBegin(false);
    bool bPrevVirtualCoords = glFontVirtualCoordinates(true);
    int result = glFontPrintf(
        GetDebugFontView(), view, x, y, textColour, printbuf);
    glFontEnd();
    glFontVirtualCoordinates(bPrevVirtualCoords);
    glStateRestore(state);
    return result;
}

int nlScreenPrintf(
    int x, int y, bool bFromTop, int font, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    int result = nlScreenPrint(DefaultScreenPrintfGLView, x, y, bFromTop, font, DefaultScreenPrintfBackgroundColour, DefaultScreenPrintfTextColour, DefaultScreenPrintfHaveBackground, format, va);
    va_end(va);
    return result;
}

void DrawTextRectangle(GLView* view, const nlColour& c,
    float left, float top, float right, float bottom, float z, int layer,
    bool bSnapToGrid, bool bVirtualCoords)
{
    float x = 0.0f;
    float y = 0.0f;
    float x2 = 0.0f;
    float y2 = 0.0f;

    if (bVirtualCoords)
    {
        glFontVirtualPosToScreenCoordPos(left, top, x, y);
        glFontVirtualPosToScreenCoordPos(right, bottom, x2, y2);
    }
    else
    {
        x = left;
        y = top;
        x2 = right;
        y2 = bottom;
    }

    if (bSnapToGrid)
    {
        x -= sfTextRectangleBorder;
        y -= sfTextRectangleBorder;
        x2 += sfTextRectangleBorder;
        y2 += sfTextRectangleBorder;
    }

    float w = x2 - x;
    float h = y2 - y;
    glPoly2 poly;
    glSetDefaultState(false);
    if (c.c[3] != 0xFF)
    {
        glSetRasterState(GLS_AlphaBlend, 1);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetRasterState(GLS_AlphaTestRef, 0);
        glSetCurrentRasterState(glHandleizeRasterState());
    }

    nlVec2Set(poly.m_pos[0], x, y);
    nlVec2Set(poly.m_pos[1], x, y + h);
    nlVec2Set(poly.m_pos[2], x + w, y + h);
    nlVec2Set(poly.m_pos[3], x + w, y);
    poly.m_colour[3] = c;
    poly.m_colour[2] = poly.m_colour[3];
    poly.m_colour[1] = poly.m_colour[3];
    poly.m_colour[0] = poly.m_colour[3];
    poly.depth = z;
    poly.Attach(view, layer, 0);
}

static char sWhiteTextureName[] = "global/white";
static u32 WhiteTexture = glGetTexture(sWhiteTextureName);

DebugPolyList g_DebugPolyList;
SlotPool<glPoly2> g_DebugPolySlotPool(16, 16);
