#include <revolution/gx.h>

#include "NL/glx/glxGX.h"
#include "NL/nlColour.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" void fn_801B5EE8(float value, const bool* flags, int stageCount, int texGenCount,
    int texture4, int texture5, int texCoord3, int texCoord4,
    int texCoord5, int finalTexture)
{
    int extraStages = 0;
    int extraTexGens = 0;
    bool useValue = value != 0.0f;
    if (useValue)
        extraStages = 1;

    bool useTextures = false;
    if (flags[0])
    {
        useTextures = true;
        extraStages = 2;
        extraTexGens = 2;
    }
    if (flags[1])
    {
        useTextures = true;
        extraStages = 2;
        extraTexGens = 2;
    }

    gxSetNumTevStages(stageCount + extraStages);
    gxSetNumTexGens(texGenCount + extraTexGens);
    if (useValue)
    {
        gxSetTevOrder(4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevColourIn(4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ONE, GX_CC_C0);
        gxSetTevAlphaIn(4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }

    int coords[2] = { texture4, texture5 };
    int textures[2] = { texCoord4, texCoord5 };
    for (int i = 0; i < 2; i++)
    {
        if (useTextures)
        {
            gxSetTevOrder(stageCount, coords[i], textures[i], GX_COLOR_NULL);
            gxSetTevColourIn(stageCount, GX_CC_CPREV, GX_CC_TEXC,
                flags[i] ? GX_CC_TEXA : GX_CC_ZERO, GX_CC_ZERO);
            gxSetTevAlphaIn(stageCount, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            stageCount++;
        }
    }

    if (useValue)
    {
        nlFloatColour source;
        nlFloatColourSet(source, value, value, value, value);
        nlColour colour;
        ConvertColour(colour, source);
        GXSetTevKColor(GX_KCOLOR3, *(GXColor*)&colour);
        gxSetTexCoordGen(texCoord3, GX_TG_MTX2x4, texCoord3 + GX_TG_TEX0, GX_IDENTITY);
        gxSetTevOrder(stageCount, texCoord3, finalTexture, GX_COLOR_NULL);
        gxSetTevColourIn(stageCount, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevAlphaIn(stageCount, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevKColourSel(stageCount, GX_TEV_KCSEL_K3);
    }
}
