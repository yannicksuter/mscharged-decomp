#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "Game/UnidentifiedStaticStorage.h"
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

extern "C" void glxConfigureMegaSpecularFresnelLighting(bool enabled)
{
    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(1, sMegaSpecularFresnelLightCount, 0);
    }

    gxSetNumChans(1);
    gxSetNumTexGens(6);
    gxSetNumTevStages(7);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 0, 255);
    gxSetTevOrder(2, 255, 255, enabled ? 4 : 255);
    gxSetTevOrder(3, 2, 2, 255);
    gxSetTevOrder(4, 3, 3, 255);
    gxSetTevOrder(5, 5, 5, 255);
    gxSetTevOrder(6, 4, 4, 255);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);
    gxSetTevColourOp(2, 0, 0, 0, true, 1);
    if (enabled && ShouldDoubleGameObjectLighting() != 0)
        gxSetTevColourOp(1, 0, 0, 1, true, 0);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
    GXSetTevKColorSel(GX_TEVSTAGE6, GX_TEV_KCSEL_K3);
    gxSetTevColourIn(0, 15, 12, 8, 14);
    gxSetTevColourIn(1, 15, 0, 8, 15);
    gxSetTevColourIn(2, 15, 0, enabled ? 10 : 12, 15);
    gxSetTevColourIn(3, 15, 14, 8, 15);
    gxSetTevColourIn(4, 15, 0, 8, 15);
    gxSetTevColourIn(5, 15, 0, 8, 2);
    gxSetTevColourIn(6, 0, 8, 14, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 4);
    gxSetTevAlphaIn(2, 7, 7, 7, 0);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
    gxSetTevAlphaIn(5, 7, 7, 7, 0);
    gxSetTevAlphaIn(6, 7, 7, 7, 0);
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
    GXLoadTexMtxImm(sMegaFresnelLookupMatrix, 67, GX_MTX3x4);
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
    gxSetTevColourOp(1, 0, 0, 0, true, 0);
    gxSetTexCoordGen(2, 1, 6, 60);
    gxSetTexCoordGen(5, 1, 9, 60);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
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
        GXLoadTexMtxImm(specularTextureMatrix, 64, GX_MTX3x4);
        sMegaSpecularLookupScale[0] = specularScaleX;
        sMegaSpecularLookupScale[1] = specularScaleY;
    }

    unsigned long fresnelTexture = sMegaSpecularFresnelTextures[fresnelRamp];
    if (sMegaSpecularFresnelBoundTexture != fresnelTexture)
    {
        if (!sMegaSpecularFresnelIndicesInitialized)
        {
            sMegaSpecularFresnelTextureIndices[0] = 0xFFFF;
            sMegaSpecularFresnelTextureIndices[1] = 0xFFFF;
            sMegaSpecularFresnelTextureIndices[2] = 0xFFFF;
            sMegaSpecularFresnelTextureIndices[3] = 0xFFFF;
            sMegaSpecularFresnelTextureIndices[4] = 0xFFFF;
            sMegaSpecularFresnelIndicesInitialized = true;
        }
        if (sMegaSpecularFresnelTextureIndices[fresnelRamp] == 0xFFFF
            || sMegaSpecularFresnelTextureIndices[fresnelRamp] == 0)
        {
            sMegaSpecularFresnelTextureIndices[fresnelRamp] = glGetTextureManager()->GetTextureIndex(fresnelTexture);
        }
        glTextureBinding fresnelBinding;
        fresnelBinding.texture = fresnelTexture;
        fresnelBinding.flags = 0;
        fresnelBinding.SetWrapS(true);
        fresnelBinding.SetWrapT(true);
        fresnelBinding.unknown07 = 0;
        fresnelBinding.textureIndex = sMegaSpecularFresnelTextureIndices[fresnelRamp];
        glx_BindTexture(5, &fresnelBinding);
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
        GXLoadTexMtxImm(normalMatrix, 30, GX_MTX3x4);
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(
            static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->skinMatrices,
            static_cast<const GXMegaSpecularFresnelParameters*>(packet->materialParameters)->skinMatricesSize / 48,
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
