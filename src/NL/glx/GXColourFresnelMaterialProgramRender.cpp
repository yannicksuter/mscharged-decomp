#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "Game/SharedStaticStorage.h"
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
    GXLoadTexMtxImm(sColourFresnelLookupMatrix, GX_PTTEXMTX1, GX_MTX3x4);
    gxSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, true, GX_PTTEXMTX1);

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
        glTextureBinding lightBinding(GetGameObjectLightTexture());
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
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD4, GX_TEXMAP4, GX_COLOR0A0);
        gxSetTexCoordGen(GX_TEXCOORD4, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG1);
        gxSetTevColourOp(GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, lightingScale, true, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C1, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else if (sColourFresnelLightingEnabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        numChans = 1;
        numTexGens = 4;
        numTevStages = 6;
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG1);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C1, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        numChans = 0;
        numTexGens = 1;
        sColourFresnelLightCount = 0;
        numTevStages = 1;
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
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
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        if (sColourFresnelLightRampEnabled)
        {
            gxSetTevColourOp(GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
            gxSetTexCoordGen(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY);
        }
    }
    gxSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY);
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

    float alphaValue = glGetMaterialFloatParameterWithOverride(packet,
        &GXColourFresnelParameters::alphaValue, sColourFresnelAlphaOverride);
    float blendAmount = glGetMaterialFloatParameterWithOverride(packet,
        &GXColourFresnelParameters::blendAmount, sColourFresnelBlendOverride);
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
        GXLoadTexMtxImm(normalMatrix, GX_TEXMTX0, GX_MTX3x4);
        sColourFresnelModelMatrix = modelMatrix;
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(
            static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->skinMatrices,
            static_cast<const GXColourFresnelParameters*>(packet->materialParameters)->skinMatrixBytes / 48,
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
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
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
