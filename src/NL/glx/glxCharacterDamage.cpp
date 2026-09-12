#include <revolution/gx.h>

#include "NL/glx/glxCharacterDamage.h"
#include "NL/glx/glxGX.h"
#include "NL/nlColour.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" void glxConfigureCharacterDamage(float megaBlend, const bool* damageEnabled, int stageCount, int texGenCount,
    int damageTexCoord1, int damageTexCoord2, int megaTexCoord, int damageTexture1,
    int damageTexture2, int megaTexture)
{
    int extraStages = 0;
    int extraTexGens = 0;
    bool useMegaTexture = megaBlend != 0.0f;
    if (useMegaTexture)
        extraStages = 1;

    bool useDamageTextures = false;
    if (damageEnabled[0])
    {
        useDamageTextures = true;
        extraStages = 2;
        extraTexGens = 2;
    }
    if (damageEnabled[1])
    {
        useDamageTextures = true;
        extraStages = 2;
        extraTexGens = 2;
    }

    gxSetNumTevStages(stageCount + extraStages);
    gxSetNumTexGens(texGenCount + extraTexGens);
    if (useMegaTexture)
    {
        gxSetTevOrder(4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevColourIn(4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ONE, GX_CC_C0);
        gxSetTevAlphaIn(4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }

    int damageTexCoords[2] = { damageTexCoord1, damageTexCoord2 };
    int damageTextures[2] = { damageTexture1, damageTexture2 };
    for (int i = 0; i < 2; i++)
    {
        if (useDamageTextures)
        {
            gxSetTevOrder(stageCount, damageTexCoords[i], damageTextures[i], GX_COLOR_NULL);
            gxSetTevColourIn(stageCount, GX_CC_CPREV, GX_CC_TEXC,
                damageEnabled[i] ? GX_CC_TEXA : GX_CC_ZERO, GX_CC_ZERO);
            gxSetTevAlphaIn(stageCount, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            stageCount++;
        }
    }

    if (useMegaTexture)
    {
        nlFloatColour megaColour;
        nlFloatColourSet(megaColour, megaBlend, megaBlend, megaBlend, megaBlend);
        nlColour megaColour8;
        ConvertColour(megaColour8, megaColour);
        GXSetTevKColor(GX_KCOLOR3, *(GXColor*)&megaColour8);
        gxSetTexCoordGen(megaTexCoord, GX_TG_MTX2x4, megaTexCoord + GX_TG_TEX0, GX_IDENTITY);
        gxSetTevOrder(stageCount, megaTexCoord, megaTexture, GX_COLOR_NULL);
        gxSetTevColourIn(stageCount, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevAlphaIn(stageCount, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevKColourSel(stageCount, GX_TEV_KCSEL_K3);
    }
}
