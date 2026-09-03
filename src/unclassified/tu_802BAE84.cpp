#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glState.h"
#include "NL/nlColour.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802BAE84.h"

#include <stdarg.h>
#include <string.h>

extern "C" GLView* fn_802B77A0();
extern "C" int fn_802C9CAC(int font);
extern "C" int fn_802C9CC8(GLView* view);

static nlColour lbl_806DF2C8 = { 0xFF, 0xFF, 0xFF, 0xFF };
static bool lbl_806DF2CC = true;
static nlColour lbl_806DF2D0 = { 0, 0, 0, 150 };
static float lbl_806DF2D4 = 4.0f;

static eGLView lbl_806E1DE0;

extern "C" int fn_802BAE84(eGLView view, int x, int y, bool flipY,
    int font, const nlColour& backgroundColour, const nlColour& textColour,
    bool drawBackground, const char* format, va_list args)
{
    char string[128];
    glStateBundle state;

    fn_802C9CAC(font);
    if (!flipY)
    {
        y = fn_802C9CC8(fn_802B77A0()) - y;
    }

    nlVSNPrintf(string, sizeof(string), format, args);
    glStateSave(state);

    if (drawBackground)
    {
        int length = nlStrLen(string);
        float x0 = (float)x;
        float y0 = (float)y;
        float x1 = (float)(x + length);
        float y1 = (float)(y + 1);
        GLView* renderView = fn_802B77A0();
        fn_802BB11C(renderView, backgroundColour,
            x0, y0, x1, y1, 0.0f,
            view - 1, true, true);
    }

    glFontBegin(false);
    bool oldVirtualCoordinates = glFontVirtualCoordinates(true);
    int result = glFontPrintf(
        fn_802B77A0(), view, x, y, textColour, string);
    glFontEnd();
    glFontVirtualCoordinates(oldVirtualCoordinates);
    glStateRestore(state);
    return result;
}

extern "C" int fn_802BB048(
    int x, int y, bool flipY, int font, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = fn_802BAE84(lbl_806E1DE0, x, y, flipY, font,
        lbl_806DF2D0, lbl_806DF2C8, lbl_806DF2CC, format, args);
    va_end(args);
    return result;
}

extern "C" void fn_802BB11C(GLView* view, const nlColour& colour,
    float x0, float y0, float x1, float y1, float depth, int layer,
    bool expand, bool convertCoordinates)
{
    float screenX0 = 0.0f;
    float screenY0 = 0.0f;
    float screenX1 = 0.0f;
    float screenY1 = 0.0f;

    if (convertCoordinates)
    {
        glFontVirtualPosToScreenCoordPos(x0, y0, screenX0, screenY0);
        glFontVirtualPosToScreenCoordPos(x1, y1, screenX1, screenY1);
    }
    else
    {
        screenX0 = x0;
        screenY0 = y0;
        screenX1 = x1;
        screenY1 = y1;
    }

    if (expand)
    {
        screenX0 -= lbl_806DF2D4;
        screenY0 -= lbl_806DF2D4;
        screenX1 += lbl_806DF2D4;
        screenY1 += lbl_806DF2D4;
    }

    float width = screenX1 - screenX0;
    float height = screenY1 - screenY0;
    glPoly2 poly;
    glSetDefaultState(false);
    if (colour.c[3] != 0xFF)
    {
        glSetRasterState(GLS_AlphaBlend, 1);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetRasterState(GLS_AlphaTestRef, 0);
        glSetCurrentRasterState(glHandleizeRasterState());
    }

    nlVec2Set(poly.m_pos[0], screenX0, screenY0);
    nlVec2Set(poly.m_pos[1], screenX0, screenY0 + height);
    nlVec2Set(poly.m_pos[2], screenX0 + width, screenY0 + height);
    nlVec2Set(poly.m_pos[3], screenX0 + width, screenY0);
    poly.m_colour[3] = colour;
    poly.m_colour[2] = poly.m_colour[3];
    poly.m_colour[1] = poly.m_colour[3];
    poly.m_colour[0] = poly.m_colour[3];
    poly.depth = depth;
    poly.Attach(view, layer, 0);
}

static u32 lbl_806E1DE4 = glGetTexture("global/white");

struct UnidentifiedDebugPolyStorage_802BB300;

extern SlotPool<glPoly2> lbl_8057C1E8;

struct UnidentifiedDebugPolyStorage_802BB300
{
    UnidentifiedDebugPolyStorage_802BB300()
        : mCurrent(0)
    {
    }

    ~UnidentifiedDebugPolyStorage_802BB300()
    {
        glPoly2* poly = 0;
        while (mPolys.m_Head != 0)
        {
            DLListEntry<glPoly2*>* entry = mPolys.m_Head;
            nlDLRingRemove(&mPolys.m_Head, entry);
            mPolys.Deallocate(entry, &poly);
            lbl_8057C1E8.DeleteEntry(poly);
        }
        mPolys.m_Allocator.FreeBlocks();
        lbl_8057C1E8.FreeBlocks();
    }

    glPoly2* mCurrent;
    nlDLListSlotPool<glPoly2*> mPolys;
};

UnidentifiedDebugPolyStorage_802BB300 lbl_8057C1B8;
SlotPool<glPoly2> lbl_8057C1E8(16, 16);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
