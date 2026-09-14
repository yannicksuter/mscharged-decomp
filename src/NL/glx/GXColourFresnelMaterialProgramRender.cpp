#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glMaterialParameters.h"
#include <revolution/mtx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXColourFresnelMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

static bool sRenderColourFresnel = true;
static bool sColourFresnelLightingEnabled = true;
static bool sColourFresnelDoubleLighting = true;
static float sColourFresnelBlendOverride = 1.0f;
static float sColourFresnelAlphaOverride = 1.0f;
static bool sColourFresnelAlphaDepthPass = true;
static int sColourFresnelRampOverride = -1;

static Mtx sColourFresnelLookupMatrix = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 sColourFresnelViewMatrix;
static unsigned long sColourFresnelTextures[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/cfresnel1"),
    glGetTexture("global/cfresnel2"),
    glGetTexture("global/cfresnel3"),
    glGetTexture("global/cfresnel4"),
};

static bool sColourFresnelLightRampEnabled;
static unsigned long sColourFresnelModelMatrix;
static int sColourFresnelLightCount;
static unsigned long sColourFresnelBoundTexture;

template <>
void GXMaterialProgramImpl<GXColourFresnelMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXColourFresnelMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sColourFresnelViewMatrix);
    sColourFresnelModelMatrix = -1;
    sColourFresnelBoundTexture = -1;
    GXLoadTexMtxImm(sColourFresnelLookupMatrix, 67, GX_MTX3x4);
    gxSetTexCoordGen(3, 0, 1, 30, true, 67);

    if (sColourFresnelLightingEnabled)
    {
        sColourFresnelLightCount = GetGameObjectLightCount(0, 1);
        LoadGameObjectLights(sColourFresnelLightCount, view, 0);
        SetGameObjectLightingEnabled(1, sColourFresnelLightCount, 0);
    }

    GXTevScale lightingScale;
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;
    if (sColourFresnelLightingEnabled && sColourFresnelLightRampEnabled)
    {
        glTextureBinding lightBinding;
        lightBinding.texture = GetGameObjectLightTexture();
        lightBinding.textureIndex = 0xFFFF;
        lightBinding.flags = 0;
        lightBinding.SetWrapS(true);
        lightBinding.SetWrapT(true);
        lightBinding.unknown07 = 0;
        glx_BindTexture(4, &lightBinding);

        numChans = 1;
        numTexGens = 5;
        numTevStages = 6;
        lightingScale = sColourFresnelDoubleLighting ? GX_CS_SCALE_2 : GX_CS_SCALE_1;
        gxSetTevOrder(0, 0, 0, 255);
        gxSetTevOrder(1, 3, 3, 255);
        gxSetTevOrder(2, 2, 2, 255);
        gxSetTevOrder(3, 1, 1, 255);
        gxSetTevOrder(4, 255, 255, 255);
        gxSetTevOrder(5, 4, 4, 4);
        gxSetTexCoordGen(4, 10, 19, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourOp(2, 0, 0, 0, true, 2);
        gxSetTevColourOp(5, 0, 0, lightingScale, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 15, 15, 8);
        gxSetTevColourIn(1, 15, 15, 15, 8);
        gxSetTevColourIn(2, 2, 8, 0, 15);
        gxSetTevColourIn(3, 15, 12, 8, 14);
        gxSetTevColourIn(4, 15, 0, 4, 15);
        gxSetTevColourIn(5, 15, 0, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 4);
        gxSetTevAlphaIn(1, 7, 7, 7, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
    else if (sColourFresnelLightingEnabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        numChans = 1;
        numTexGens = 4;
        numTevStages = 6;
        gxSetTevOrder(0, 0, 0, 255);
        gxSetTevOrder(1, 3, 3, 255);
        gxSetTevOrder(2, 2, 2, 255);
        gxSetTevOrder(3, 1, 1, 255);
        gxSetTevOrder(4, 255, 255, 255);
        gxSetTevOrder(5, 255, 255, 4);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourOp(2, 0, 0, 0, true, 2);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 15, 15, 8);
        gxSetTevColourIn(1, 15, 15, 15, 8);
        gxSetTevColourIn(2, 2, 8, 0, 15);
        gxSetTevColourIn(3, 15, 12, 8, 14);
        gxSetTevColourIn(4, 15, 0, 4, 15);
        gxSetTevColourIn(5, 15, 0, 10, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 4);
        gxSetTevAlphaIn(1, 7, 7, 7, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
    else
    {
        numChans = 0;
        numTexGens = 1;
        sColourFresnelLightCount = 0;
        numTevStages = 1;
        gxSetTevOrder(0, 0, 0, 255);
        gxSetTevColourIn(0, 15, 12, 8, 15);
        gxSetTevAlphaIn(0, 7, 6, 4, 7);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXColourFresnelMaterialProgram>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    if (sColourFresnelLightingEnabled)
    {
        SetGameObjectAmbientLightingEnabled(0);
        SetGameObjectLightingEnabled(0, sColourFresnelLightCount, 1);
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
        gxSetTevColourOp(2, 0, 0, 0, true, 0);
        if (sColourFresnelLightRampEnabled)
        {
            gxSetTevColourOp(5, 0, 0, 0, true, 0);
            gxSetTexCoordGen(4, 1, 8, 60);
        }
    }
    gxSetTexCoordGen(3, 1, 7, 60);
}

template <>
void GXMaterialProgramImpl<GXColourFresnelMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXColourFresnelMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!sRenderColourFresnel)
        return;

    float alphaValue = 1.0f != sColourFresnelAlphaOverride
        ? sColourFresnelAlphaOverride
        : static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->alphaValue;
    float blendAmount = 1.0f != sColourFresnelBlendOverride
        ? sColourFresnelBlendOverride
        : static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->blendAmount;
    int colourFresnelRamp = sColourFresnelRampOverride >= 0
        ? sColourFresnelRampOverride
        : static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->colourFresnelRamp;
    if (alphaValue == 0.0f)
        return;

    nlFloatColour blendColour = { { blendAmount, blendAmount, blendAmount, blendAmount } };
    gxSetTevKColour(GX_KCOLOR0, blendColour);
    nlFloatColour alphaColour = { { alphaValue, alphaValue, alphaValue, alphaValue } };
    gxSetTevKColour(GX_KCOLOR1, alphaColour);

    static_cast<GXColourFresnelMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXColourFresnelMaterialProgram*>(this)->BindParameters(packet);

    unsigned long fresnelTexture = sColourFresnelTextures[colourFresnelRamp];
    if (sColourFresnelBoundTexture != fresnelTexture)
    {
        glTextureBinding fresnelBinding;
        fresnelBinding.texture = fresnelTexture;
        fresnelBinding.textureIndex = 0xFFFF;
        fresnelBinding.flags = 0;
        fresnelBinding.SetWrapS(true);
        fresnelBinding.SetWrapT(true);
        fresnelBinding.unknown07 = 0;
        glx_BindTexture(3, &fresnelBinding);
        sColourFresnelBoundTexture = fresnelTexture;
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    unsigned long modelMatrix = packet->matrix;
    glGetMatrix(modelMatrix, model);
    nlMultMatrices(modelview, model, sColourFresnelViewMatrix);

    if (modelMatrix != sColourFresnelModelMatrix)
    {
        Mtx modelViewTransform;
        Mtx normalMatrix;
        glxCopyMatrix(modelViewTransform, modelview);
        PSMTXInvXpose(modelViewTransform, normalMatrix);
        GXLoadTexMtxImm(normalMatrix, 30, GX_MTX3x4);
        sColourFresnelModelMatrix = modelMatrix;
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(
            static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->skinMatrices,
            static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->skinMatricesSize / 48,
            &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    if (sColourFresnelLightingEnabled && alphaValue != 1.0f)
    {
        gxSaveZMode();
        if (sColourFresnelAlphaDepthPass)
        {
            bool colourUpdate = gxSetColourUpdate(false);
            GXCallDisplayList(
                packet->displayList->list, packet->displayList->size);
            gxSetColourUpdate(colourUpdate);
            gxSetZMode(true, GX_EQUAL, true);
        }
        gxSaveBlendMode();
        gxSetBlendMode(true, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, false);
        GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K1_A);
        gxSetTevAlphaIn(0, 7, 7, 7, 6);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxRestoreBlendMode();
        gxRestoreZMode();
    }
    else
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }

    gxSetCurrentMtx(0, true);
}
