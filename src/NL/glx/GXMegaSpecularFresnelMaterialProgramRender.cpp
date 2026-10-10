#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "Game/SharedStaticStorage.h"
#include "NL/gl/glMaterialParameters.h"
#include <revolution/mtx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMegaSpecularFresnelMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxFresnelTexture.h"

static Mtx sMegaFresnelLookupMatrix = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 sMegaSpecularFresnelViewMatrix;
static unsigned long sMegaSpecularFresnelTextures[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};
static unsigned long sMegaSpecularFresnelTextureIndices[5];

static unsigned long sMegaSpecularFresnelModelMatrix;
static float sMegaSpecularLookupScale[2];
static int sMegaSpecularFresnelLightCount;
static bool sMegaSpecularFresnelLightingEnabled;
static bool sMegaSpecularFresnelIndicesInitialized;
static unsigned long sMegaSpecularFresnelBoundTexture;

void glxConfigureMegaSpecularFresnelLighting(bool enabled)
{
    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(1, sMegaSpecularFresnelLightCount, 0);
    }

    gxSetNumChans(1);
    gxSetNumTexGens(6);
    gxSetNumTevStages(7);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, enabled ? GX_COLOR0A0 : GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD5, GX_TEXMAP5, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD4, GX_TEXMAP4, GX_COLOR_NULL);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);
    gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
    if (enabled && ShouldDoubleGameObjectLighting() != 0)
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
    GXSetTevKColorSel(GX_TEVSTAGE6, GX_TEV_KCSEL_K3);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, enabled ? GX_CC_RASC : GX_CC_ONE, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_C0);
    gxSetTevColourIn(GX_TEVSTAGE6, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE6, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularFresnelMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXMegaSpecularFresnelMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sMegaSpecularFresnelViewMatrix);
    SetGameObjectShadowViewMatrix(&sMegaSpecularFresnelViewMatrix);
    SetGameObjectShadowModelMatrix(-1);
    sMegaSpecularFresnelModelMatrix = -1;
    sMegaSpecularLookupScale[0] = -1.0f;
    sMegaSpecularLookupScale[1] = -1.0f;
    sMegaSpecularFresnelBoundTexture = -1;
    GXLoadTexMtxImm(sMegaFresnelLookupMatrix, GX_PTTEXMTX1, GX_MTX3x4);
    sMegaSpecularFresnelLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sMegaSpecularFresnelLightCount, view, 0);
    sMegaSpecularFresnelLightingEnabled = true;
    glxConfigureMegaSpecularFresnelLighting(true);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularFresnelMaterialProgram>::Deactivate()
{
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sMegaSpecularFresnelLightCount, 1);
    gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY);
    gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularFresnelMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularFresnelMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    GXMegaSpecularFresnelMaterialProgram* program = static_cast<GXMegaSpecularFresnelMaterialProgram*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    float megaBlend = static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->megaBlend;
    gxSetNumTevStages(megaBlend == 0.0f ? 6 : 7);
    const GXMegaSpecularFresnelParameters* parameters = static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters);
    float blendAmount = parameters->blendAmount;
    float alphaValue = parameters->alphaValue;
    if (alphaValue == 0.0f)
        return;
    float specularAmount = parameters->specularAmount;
    float specularScaleX = parameters->specularScaleX;
    float specularScaleY = parameters->specularScaleY;
    int fresnelRamp = parameters->fresnelRamp;

    nlFloatColour blendColour;
    nlFloatColourSet(blendColour, blendAmount, blendAmount, blendAmount, blendAmount);
    gxSetTevKColour(GX_KCOLOR0, blendColour);
    nlFloatColour alphaColour;
    nlFloatColourSet(alphaColour, alphaValue, alphaValue, alphaValue, alphaValue);
    gxSetTevKColour(GX_KCOLOR1, alphaColour);
    nlFloatColour specularColour;
    nlFloatColourSet(specularColour, specularAmount, specularAmount, specularAmount, specularAmount);
    gxSetTevKColour(GX_KCOLOR2, specularColour);
    nlFloatColour megaColour;
    nlFloatColourSet(megaColour, megaBlend, megaBlend, megaBlend, megaBlend);
    gxSetTevKColour(GX_KCOLOR3, megaColour);

    bool enabled = static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (sMegaSpecularFresnelLightingEnabled != enabled)
    {
        sMegaSpecularFresnelLightingEnabled = enabled;
        Deactivate();
        glxConfigureMegaSpecularFresnelLighting(enabled);
    }

    if (sMegaSpecularLookupScale[0] != specularScaleX || sMegaSpecularLookupScale[1] != specularScaleY)
    {
        Mtx specularTextureMatrix = {
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        specularTextureMatrix[0][0] = 0.5f * specularScaleX;
        specularTextureMatrix[1][1] = -0.5f * specularScaleY;
        GXLoadTexMtxImm(specularTextureMatrix, GX_PTTEXMTX0, GX_MTX3x4);
        sMegaSpecularLookupScale[0] = specularScaleX;
        sMegaSpecularLookupScale[1] = specularScaleY;
    }

    unsigned long fresnelTexture = sMegaSpecularFresnelTextures[fresnelRamp];
    if (sMegaSpecularFresnelBoundTexture != fresnelTexture)
    {
        glxBindFresnelTexture(5, fresnelTexture, fresnelRamp,
            sMegaSpecularFresnelTextureIndices, sMegaSpecularFresnelIndicesInitialized);
        sMegaSpecularFresnelBoundTexture = fresnelTexture;
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    unsigned long modelMatrix = packet->matrix;
    glGetMatrix(modelMatrix, model);
    nlMultMatrices(modelview, model, sMegaSpecularFresnelViewMatrix);
    SetGameObjectShadowModelMatrix(modelMatrix);

    if (modelMatrix != sMegaSpecularFresnelModelMatrix)
    {
        Mtx modelViewTransform;
        Mtx normalMatrix;
        sMegaSpecularFresnelModelMatrix = modelMatrix;
        glxCopyMatrix(modelViewTransform, modelview);
        PSMTXInvXpose(modelViewTransform, normalMatrix);
        GXLoadTexMtxImm(normalMatrix, GX_TEXMTX0, GX_MTX3x4);
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(
            static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->skinMatrices,
            static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->skinMatrixBytes / 48,
            &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    ApplyGameObjectShadowLighting(
        1,
        static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->shadowLevel);
    if (sMegaSpecularFresnelLightingEnabled && alphaValue != 1.0f)
    {
        gxSaveZMode();
        bool colourUpdate = gxSetColourUpdate(false);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxSetColourUpdate(colourUpdate);
        gxSetZMode(true, GX_EQUAL, true);
        gxSaveBlendMode();
        gxSetBlendMode(true, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, false);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxRestoreBlendMode();
        gxRestoreZMode();
    }
    else
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    RestoreGameObjectShadowLighting();
}
