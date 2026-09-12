#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glView.h"
#include "NL/glx/GXMaskedDetailBlendMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

enum MaskedDetailBlendLightingMode
{
    LIGHTING_NONE,
    LIGHTING_VERTEX,
    LIGHTING_TEXTURE,
    LIGHTING_UNCONFIGURED
};

bool gMaskedDetailBlendEnabled = true;
bool gMaskedDetailBlendShadowsEnabled = true;

nlMatrix4 sMaskedDetailBlendViewMatrix;
bool sMaskedDetailBlendShadowsActive;
int sMaskedDetailBlendLightCount;
int sMaskedDetailBlendLightingMode;

extern "C" void glxConfigureMaskedDetailBlendLighting(
    int mode, GXMaskedDetailBlendMaterialProgram*)
{
    sMaskedDetailBlendLightingMode = mode;

    if (mode == LIGHTING_VERTEX)
        SetGameObjectAmbientLightingEnabled(1);

    if (mode != LIGHTING_NONE)
        SetGameObjectLightingEnabled(1, sMaskedDetailBlendLightCount, 1);

    if (mode == LIGHTING_TEXTURE)
    {
        gxSetNumTevStages(5);
        gxSetNumTexGens(4);
        GXTevScale colourScale = ShouldDoubleGameObjectLighting() ? GX_CS_SCALE_2 : GX_CS_SCALE_1;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD3, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);

        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);

        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG1);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);

        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_TEXC, GX_CC_ZERO, GX_CC_C1, GX_CC_CPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

        gxSetTevColourOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, colourScale, true, GX_TEVPREV);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        gxSetNumTevStages(4);
        gxSetNumTexGens(3);

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);

        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);

        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_TEXC, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
}

template <>
void GXMaterialProgramImpl<GXMaskedDetailBlendMaterialProgram>::Activate(GLView* view)
{
    sMaskedDetailBlendLightingMode = LIGHTING_UNCONFIGURED;
    static_cast<GXMaskedDetailBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sMaskedDetailBlendViewMatrix);
    sMaskedDetailBlendLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sMaskedDetailBlendLightCount, view, 0);
    gxSetNumChans(1);

    glTextureBinding texture;
    texture.texture = 0;
    texture.textureIndex = 0xFFFF;
    texture.flags = 0;
    texture.unknown07 = 0;
    texture.texture = GetGameObjectLightTexture();
    texture.textureIndex = 0xFFFF;
    texture.SetWrapS(true);
    texture.SetWrapT(true);
    texture.unknown07 = 0;
    glx_BindTexture(3, &texture);
}

template <>
void GXMaterialProgramImpl<GXMaskedDetailBlendMaterialProgram>::Deactivate()
{
    if (sMaskedDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sMaskedDetailBlendShadowsActive = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    SetGameObjectLightingEnabled(0, sMaskedDetailBlendLightCount, 1);

    if (sMaskedDetailBlendLightingMode == LIGHTING_TEXTURE)
    {
        gxSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
    else
    {
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
}

template <>
void GXMaterialProgramImpl<GXMaskedDetailBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMaskedDetailBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXMaskedDetailBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gMaskedDetailBlendEnabled)
        return;

    float detailWeight = 1.0f - static_cast<const GXMaskedDetailBlendParameters*>(packet->materialParameters)->blendAmount;

    int mode;
    if (static_cast<const GXMaskedDetailBlendParameters*>(packet->materialParameters)->lightingEnabled == 0 || IsGameObjectLightingEnabled() == 0)
    {
        mode = LIGHTING_NONE;
    }
    else
    {
        int useLightTexture = ShouldUseGameObjectLightTexture(0);
        mode = LIGHTING_VERTEX;
        if (useLightTexture != 0)
            mode = LIGHTING_TEXTURE;
    }

    if (sMaskedDetailBlendLightingMode != mode)
    {
        Deactivate();
        glxConfigureMaskedDetailBlendLighting(mode, static_cast<GXMaskedDetailBlendMaterialProgram*>(this));
    }

    nlFloatColour detailColour = { { detailWeight, detailWeight, detailWeight, detailWeight } };
    nlColour detailColour8;
    ConvertColour(detailColour8, detailColour);
    GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&detailColour8);

    static_cast<GXMaskedDetailBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXMaskedDetailBlendMaterialProgram*>(this)->BindParameters(packet);
    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXMaskedDetailBlendParameters*>(packet->materialParameters)->receiveShadows == 1 && gMaskedDetailBlendShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sMaskedDetailBlendShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sMaskedDetailBlendShadowsActive = true;
        }
    }
    else if (sMaskedDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sMaskedDetailBlendShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaskedDetailBlendMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaskedDetailBlendMaterialProgram*>(this)->DrawDirect(packet);
}
