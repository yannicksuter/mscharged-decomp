#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glView.h"
#include "NL/glx/GXShadowedDetailBlendMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

enum ShadowedDetailBlendLightingMode
{
    LIGHTING_NONE,
    LIGHTING_VERTEX,
    LIGHTING_TEXTURE,
    LIGHTING_UNCONFIGURED
};

bool gShadowedDetailBlendEnabled = true;
bool gShadowedDetailBlendShadowsEnabled = true;

nlMatrix4 sShadowedDetailBlendViewMatrix;
bool sShadowedDetailBlendShadowsActive;
int sShadowedDetailBlendLightCount;
int sShadowedDetailBlendLightingMode;

extern "C" void glxConfigureShadowedDetailBlendLighting(
    int mode, GXShadowedDetailBlendMaterialProgram*)
{
    sShadowedDetailBlendLightingMode = mode;

    if (mode == LIGHTING_VERTEX)
        SetGameObjectAmbientLightingEnabled(1);

    if (mode != LIGHTING_NONE)
        SetGameObjectLightingEnabled(1, sShadowedDetailBlendLightCount, 1);

    if (mode == LIGHTING_TEXTURE)
    {
        gxSetNumTevStages(7);
        gxSetNumTexGens(5);
        ShouldDoubleGameObjectLighting();

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
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
        gxSetTevColourOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K1);
        gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevColourIn(GX_TEVSTAGE6, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE6, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        gxSetNumTevStages(6);
        gxSetNumTexGens(4);

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_TEXC, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K1);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
}

template <>
void GXMaterialProgramImpl<GXShadowedDetailBlendMaterialProgram>::Activate(GLView* view)
{
    sShadowedDetailBlendLightingMode = LIGHTING_UNCONFIGURED;
    static_cast<GXShadowedDetailBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sShadowedDetailBlendViewMatrix);
    sShadowedDetailBlendLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sShadowedDetailBlendLightCount, view, 0);
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
void GXMaterialProgramImpl<GXShadowedDetailBlendMaterialProgram>::Deactivate()
{
    if (sShadowedDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sShadowedDetailBlendShadowsActive = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    SetGameObjectLightingEnabled(0, sShadowedDetailBlendLightCount, 1);

    if (sShadowedDetailBlendLightingMode == LIGHTING_TEXTURE)
    {
        gxSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
    else
    {
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
}

template <>
void GXMaterialProgramImpl<GXShadowedDetailBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXShadowedDetailBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXShadowedDetailBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gShadowedDetailBlendEnabled)
        return;

    float detailWeight =
        1.0f - static_cast<const GXShadowedDetailBlendParameters*>(packet->materialParameters)->blendAmount;

    int mode;
    if (static_cast<const GXShadowedDetailBlendParameters*>(packet->materialParameters)->lightingEnabled == 0
        || IsGameObjectLightingEnabled() == 0)
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

    if (sShadowedDetailBlendLightingMode != mode)
    {
        Deactivate();
        glxConfigureShadowedDetailBlendLighting(mode, static_cast<GXShadowedDetailBlendMaterialProgram*>(this));
    }

    nlFloatColour detailColour = { { detailWeight, detailWeight, detailWeight, detailWeight } };
    gxSetTevKColour(GX_KCOLOR0, detailColour);

    float shadowLevel = static_cast<const GXShadowedDetailBlendParameters*>(packet->materialParameters)->shadowLevel;
    nlFloatColour shadowColour = { { shadowLevel, shadowLevel, shadowLevel, shadowLevel } };
    gxSetTevKColour(GX_KCOLOR1, shadowColour);

    static_cast<GXShadowedDetailBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXShadowedDetailBlendMaterialProgram*>(this)->BindParameters(packet);
    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXShadowedDetailBlendParameters*>(packet->materialParameters)->shadowEnabled == 1
        && gShadowedDetailBlendShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sShadowedDetailBlendShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sShadowedDetailBlendShadowsActive = true;
        }
    }
    else if (sShadowedDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sShadowedDetailBlendShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXShadowedDetailBlendMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXShadowedDetailBlendMaterialProgram*>(this)->DrawDirect(packet);
}
