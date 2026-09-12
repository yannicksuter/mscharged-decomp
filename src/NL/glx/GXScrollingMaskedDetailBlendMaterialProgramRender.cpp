#include <revolution/gx.h>
#include "Game/Render/StadiumLoading.h"
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXScrollingMaskedDetailBlendMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramTextureAnimation.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

enum ScrollingMaskedDetailBlendLightingMode
{
    LIGHTING_NONE,
    LIGHTING_VERTEX,
    LIGHTING_TEXTURE,
    LIGHTING_UNCONFIGURED
};

bool gScrollingMaskedDetailBlendShadowsEnabled = true;

nlMatrix4 sScrollingMaskedDetailBlendViewMatrix;
bool sScrollingMaskedDetailBlendShadowsActive;
int sScrollingMaskedDetailBlendLightCount;
int sScrollingMaskedDetailBlendLightingMode;

extern "C" void glxConfigureScrollingMaskedDetailBlendLighting(
    int mode, GXScrollingMaskedDetailBlendMaterialProgram*)
{
    sScrollingMaskedDetailBlendLightingMode = mode;

    if (mode == LIGHTING_VERTEX)
        SetGameObjectAmbientLightingEnabled(1);

    if (mode != LIGHTING_NONE)
        SetGameObjectLightingEnabled(1, sScrollingMaskedDetailBlendLightCount, 1);

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

    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX1, GX_TEXMTX1);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_TEX2, GX_TEXMTX2);
}

template <>
void GXMaterialProgramImpl<GXScrollingMaskedDetailBlendMaterialProgram>::Activate(GLView* view)
{
    sScrollingMaskedDetailBlendLightingMode = LIGHTING_UNCONFIGURED;
    static_cast<GXScrollingMaskedDetailBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sScrollingMaskedDetailBlendViewMatrix);
    sScrollingMaskedDetailBlendLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sScrollingMaskedDetailBlendLightCount, view, 0);
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
void GXMaterialProgramImpl<GXScrollingMaskedDetailBlendMaterialProgram>::Deactivate()
{
    if (sScrollingMaskedDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingMaskedDetailBlendShadowsActive = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    SetGameObjectLightingEnabled(0, sScrollingMaskedDetailBlendLightCount, 1);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_TEX2, GX_IDENTITY);

    if (sScrollingMaskedDetailBlendLightingMode == LIGHTING_TEXTURE)
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
void GXMaterialProgramImpl<GXScrollingMaskedDetailBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXScrollingMaskedDetailBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    float detailWeight = 1.0f - static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->blendAmount;

    int mode;
    if (static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->lightingEnabled == 0 || IsGameObjectLightingEnabled() == 0)
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

    if (sScrollingMaskedDetailBlendLightingMode != mode)
    {
        Deactivate();
        glxConfigureScrollingMaskedDetailBlendLighting(mode, static_cast<GXScrollingMaskedDetailBlendMaterialProgram*>(this));
    }

    nlFloatColour detailColour = { { detailWeight, detailWeight, detailWeight, detailWeight } };
    nlColour detailColour8;
    ConvertColour(detailColour8, detailColour);
    GXColor detailGXColour = { detailColour8.c[0], detailColour8.c[1], detailColour8.c[2], detailColour8.c[3] };
    GXSetTevKColor(GX_KCOLOR0, detailGXColour);

    static_cast<GXScrollingMaskedDetailBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXScrollingMaskedDetailBlendMaterialProgram*>(this)->BindParameters(packet);

    nlVector2 diffuseScrollSpeed;
    diffuseScrollSpeed.x = static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->diffuseScrollSpeedX;
    diffuseScrollSpeed.y = static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->diffuseScrollSpeedY;
    nlVector2 detailScrollSpeed;
    detailScrollSpeed.x = static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->detailScrollSpeedX;
    detailScrollSpeed.y = static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->detailScrollSpeedY;
    nlVector2 blendMaskScrollSpeed;
    blendMaskScrollSpeed.x = static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->blendMaskScrollSpeedX;
    blendMaskScrollSpeed.y = static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->blendMaskScrollSpeedY;

    glxLoadScrollingTextureMatrix(GX_TEXMTX0, diffuseScrollSpeed);
    glxLoadScrollingTextureMatrix(GX_TEXMTX1, detailScrollSpeed);
    glxLoadScrollingTextureMatrix(GX_TEXMTX2, blendMaskScrollSpeed);

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXScrollingMaskedDetailBlendParameters*>(packet->materialParameters)->shadowEnabled == 1 && gScrollingMaskedDetailBlendShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sScrollingMaskedDetailBlendShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sScrollingMaskedDetailBlendShadowsActive = true;
        }
    }
    else if (sScrollingMaskedDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingMaskedDetailBlendShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXScrollingMaskedDetailBlendMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXScrollingMaskedDetailBlendMaterialProgram*>(this)->DrawDirect(packet);
}
