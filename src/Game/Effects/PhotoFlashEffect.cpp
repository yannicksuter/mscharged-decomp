#include "Game/Effects/PhotoFlashEffect.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/nlColour.h"

extern "C" GLView* fn_8027267C(int index);

static s32 sNumFlashFadeFrames = 8;

s32 PhotoFlash::sNumFramesSinceFlash = 999;

void PhotoFlash::Render(float dt)
{
    glPoly2 poly;
    u8 value;
    if (sNumFramesSinceFlash < sNumFlashFadeFrames)
    {
        glSetDefaultState(false);
        glSetRasterState(GLS_AlphaBlend, 2);
        glSetCurrentRasterState(glHandleizeRasterState());

        value = 255 - ((0xFF / sNumFlashFadeFrames) * sNumFramesSinceFlash);

        nlColour c;
        c.c[0] = value;
        c.c[1] = value;
        c.c[2] = value;
        c.c[3] = 255;

        poly.SetColour(c);
        poly.SetupRectangle(0.0f, 0.0f, 640.0f, 480.0f, 0.0f);
        glAttachPoly2(fn_8027267C(0x25), 1, &poly, 0);
    }

    ++sNumFramesSinceFlash;
}
