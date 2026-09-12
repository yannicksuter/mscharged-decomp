#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glMaterialParameters.h"
#include <revolution/mtx.h>

#include <string.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXSpecularFresnelMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

static bool sRenderSpecularFresnel = true;
static bool sEnableSpecularFresnelLighting = true;
static int sFresnelRampOverride = -1;
static float sSpecularFresnelBlendOverride = 1.0f;
static float sSpecularFresnelAlphaOverride = 1.0f;
static bool sSpecularFresnelAlphaDepthPass = true;

static Mtx sFresnelLookupMatrix = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 sSpecularFresnelViewMatrix;
static unsigned long sSpecularFresnelTextures[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};
static unsigned long sSpecularFresnelTextureIndices[5];

static unsigned long sSpecularFresnelModelMatrix;
static float sSpecularLookupScale[2];
static int sSpecularFresnelLightCount;
static bool sSpecularFresnelLightingEnabled;
static bool sSpecularFresnelIndicesInitialized;
static unsigned long sSpecularFresnelBoundTexture;

extern "C" void glxConfigureSpecularFresnelLighting(bool enabled)
{
    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(1, sSpecularFresnelLightCount, 0);
    }

    gxSetNumChans(1);
    gxSetNumTexGens(5);
    gxSetNumTevStages(6);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 0, 255);
    gxSetTevOrder(2, 255, 255, enabled ? 4 : 255);
    gxSetTevOrder(3, 2, 2, 255);
    gxSetTevOrder(4, 3, 3, 255);
    gxSetTevOrder(5, 4, 4, 255);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD4, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);
    gxSetTevColourOp(2, 0, 0, 0, true, 1);
    if (enabled && ShouldDoubleGameObjectLighting() != 0)
        gxSetTevColourOp(1, 0, 0, 1, true, 0);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
    gxSetTevColourIn(0, 15, 12, 8, 14);
    gxSetTevColourIn(1, 15, 0, 8, 15);
    gxSetTevColourIn(2, 15, 0, enabled ? 10 : 12, 15);
    gxSetTevColourIn(3, 15, 14, 8, 15);
    gxSetTevColourIn(4, 15, 0, 8, 15);
    gxSetTevColourIn(5, 15, 0, 8, 2);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 4);
    gxSetTevAlphaIn(2, 7, 7, 7, 0);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
    gxSetTevAlphaIn(5, 7, 7, 7, 0);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXSpecularFresnelMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXSpecularFresnelMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sSpecularFresnelViewMatrix);
    SetGameObjectShadowViewMatrix(&sSpecularFresnelViewMatrix);
    SetGameObjectShadowModelMatrix(-1);
    sSpecularFresnelModelMatrix = -1;
    sSpecularLookupScale[0] = 255.0f;
    sSpecularLookupScale[1] = 255.0f;
    sSpecularFresnelBoundTexture = -1;
    GXLoadTexMtxImm(sFresnelLookupMatrix, 67, GX_MTX3x4);
    sSpecularFresnelLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sSpecularFresnelLightCount, view, 0);
    sSpecularFresnelLightingEnabled = sEnableSpecularFresnelLighting;
    glxConfigureSpecularFresnelLighting(sEnableSpecularFresnelLighting);
}

template <>
void GXMaterialProgramImpl<GXSpecularFresnelMaterialProgram>::Deactivate()
{
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sSpecularFresnelLightCount, 1);
    gxSetTevColourOp(1, 0, 0, 0, true, 0);
    gxSetTexCoordGen(2, 1, 6, 60);
    gxSetTexCoordGen(4, 1, 8, 60);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXSpecularFresnelMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXSpecularFresnelMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!sRenderSpecularFresnel)
        return;

    GXSpecularFresnelMaterialProgram* program = static_cast<GXSpecularFresnelMaterialProgram*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    const float one = 1.0f;
    float blendAmount = one != sSpecularFresnelBlendOverride
        ? sSpecularFresnelBlendOverride
        : static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->blendAmount;
    float alphaValue = one != sSpecularFresnelAlphaOverride
        ? sSpecularFresnelAlphaOverride
        : static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->alphaValue;
    if (alphaValue == 0.0f)
        return;

    float specularAmount = static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->specularAmount;
    float specularScaleX = static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->specularScaleX;
    float specularScaleY = static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->specularScaleY;
    int fresnelRamp = sFresnelRampOverride >= 0
        ? sFresnelRampOverride
        : static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->fresnelRamp;

    nlFloatColour blendColour = { { blendAmount, blendAmount, blendAmount, blendAmount } };
    nlColour blendColour8;
    ConvertColour(blendColour8, blendColour);
    GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&blendColour8);
    nlFloatColour alphaColour = { { alphaValue, alphaValue, alphaValue, alphaValue } };
    nlColour alphaColour8;
    ConvertColour(alphaColour8, alphaColour);
    GXSetTevKColor(GX_KCOLOR1, *(GXColor*)&alphaColour8);
    nlFloatColour specularColour = { { specularAmount, specularAmount, specularAmount, specularAmount } };
    nlColour specularColour8;
    ConvertColour(specularColour8, specularColour);
    GXSetTevKColor(GX_KCOLOR2, *(GXColor*)&specularColour8);

    bool enabled = static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (sSpecularFresnelLightingEnabled != enabled)
    {
        sSpecularFresnelLightingEnabled = enabled;
        Deactivate();
        glxConfigureSpecularFresnelLighting(enabled);
    }

    if (sSpecularLookupScale[0] != specularScaleX || sSpecularLookupScale[1] != specularScaleY)
    {
        Mtx specularTextureMatrix = {
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        specularTextureMatrix[0][0] = 0.5f * specularScaleX;
        specularTextureMatrix[1][1] = -0.5f * specularScaleY;
        GXLoadTexMtxImm(specularTextureMatrix, 64, GX_MTX3x4);
        sSpecularLookupScale[0] = specularScaleX;
        sSpecularLookupScale[1] = specularScaleY;
    }

    unsigned long fresnelTexture = sSpecularFresnelTextures[fresnelRamp];
    if (sSpecularFresnelBoundTexture != fresnelTexture)
    {
        if (!sSpecularFresnelIndicesInitialized)
        {
            sSpecularFresnelTextureIndices[0] = 0xFFFF;
            sSpecularFresnelTextureIndices[1] = 0xFFFF;
            sSpecularFresnelTextureIndices[2] = 0xFFFF;
            sSpecularFresnelTextureIndices[3] = 0xFFFF;
            sSpecularFresnelTextureIndices[4] = 0xFFFF;
            sSpecularFresnelIndicesInitialized = true;
        }
        if (sSpecularFresnelTextureIndices[fresnelRamp] == 0xFFFF
            || sSpecularFresnelTextureIndices[fresnelRamp] == 0)
        {
            sSpecularFresnelTextureIndices[fresnelRamp] = glGetTextureManager()->GetTextureIndex(fresnelTexture);
        }
        glTextureBinding fresnelBinding;
        fresnelBinding.texture = fresnelTexture;
        fresnelBinding.flags = 0;
        fresnelBinding.unknown07 = 0;
        fresnelBinding.SetWrapS(true);
        fresnelBinding.SetWrapT(true);
        fresnelBinding.textureIndex = sSpecularFresnelTextureIndices[fresnelRamp];
        glx_BindTexture(4, &fresnelBinding);
        sSpecularFresnelBoundTexture = fresnelTexture;
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    unsigned long modelMatrix = packet->matrix;
    glGetMatrix(modelMatrix, model);
    nlMultMatrices(modelview, model, sSpecularFresnelViewMatrix);
    SetGameObjectShadowModelMatrix(modelMatrix);

    if (modelMatrix != sSpecularFresnelModelMatrix)
    {
        Mtx modelViewTransform;
        Mtx normalMatrix;
        sSpecularFresnelModelMatrix = modelMatrix;
        glxCopyMatrix(modelViewTransform, modelview);
        PSMTXInvXpose(modelViewTransform, normalMatrix);
        GXLoadTexMtxImm(normalMatrix, 30, GX_MTX3x4);
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(
            static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->skinMatrices,
            static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->skinMatricesSize / 48,
            &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    ApplyGameObjectShadowLighting(
        1, static_cast<const GXSpecularFresnelParameters*>(packet->materialParameters)->shadowLevel);
    if (sEnableSpecularFresnelLighting && alphaValue != 1.0f)
    {
        gxSaveZMode();
        if (sSpecularFresnelAlphaDepthPass)
        {
            bool colourUpdate = gxSetColourUpdate(false);
            GXCallDisplayList(
                packet->displayList->list, packet->displayList->size);
            gxSetColourUpdate(colourUpdate);
            gxSetZMode(true, GX_EQUAL, true);
        }
        gxSaveBlendMode();
        gxSetBlendMode(true, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, false);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
        gxSetTevAlphaIn(1, 7, 7, 7, 6);
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
