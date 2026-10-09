#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"
#include "Game/GameObjectLighting.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/gl/glDrawSyncLog.h"
#include "NL/glx/GXCharacterSkinCustomMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/nlMath.h"
#include "Game/SharedStaticStorage.h"

static bool sRenderCharacterSkin = true;
static bool sEnableCharacterSkinLighting = true;
static bool sDoubleLightRampColour = true;
static float sBlendAmountOverride = 1.0f;
static float sAlphaValueOverride = 1.0f;
static bool sDrawAlphaDepthPass = true;

static nlMatrix4 sCharacterSkinViewMatrix;
static bool sUseCharacterLightRamp;
static int sCharacterLightCount;
static bool sCharacterSkinLightingEnabled;

void glxConfigureCharacterSkinCustomLighting(bool enabled)
{
    bool doubleLightRampColour;
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    if (enabled)
        SetGameObjectLightingEnabled(1, sCharacterLightCount, 0);

    if (enabled && sUseCharacterLightRamp)
    {
        numChans = 1;
        numTexGens = 3;
        numTevStages = 3;
        doubleLightRampColour = sDoubleLightRampColour != 0;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
        gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, doubleLightRampColour, true, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        numChans = 1;
        numTexGens = 2;
        numTevStages = 3;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        if (ShouldDoubleGameObjectLighting() != 0)
            gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        numChans = 0;
        numTexGens = 2;
        numTevStages = 3;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ONE, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXCharacterSkinCustomMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXCharacterSkinCustomMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sCharacterSkinViewMatrix);
    SetGameObjectShadowViewMatrix(&sCharacterSkinViewMatrix);
    SetGameObjectShadowModelMatrix(-1);
    sCharacterLightCount = GetGameObjectLightCount(1, 0);
    LoadGameObjectLights(sCharacterLightCount, view, 1);
    sCharacterSkinLightingEnabled = sEnableCharacterSkinLighting;
    glxConfigureCharacterSkinCustomLighting(sEnableCharacterSkinLighting);
}

template <>
void GXMaterialProgramImpl<GXCharacterSkinCustomMaterialProgram>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sCharacterLightCount, 1);
    gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
}

template <>
void GXMaterialProgramImpl<GXCharacterSkinCustomMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    const GXCharacterSkinCustomParameters* parameters = (const GXCharacterSkinCustomParameters*)packet->materialParameters;
    glSetMaterialTextureAlphaState(this, packet, parameters->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXCharacterSkinCustomMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!sRenderCharacterSkin)
        return;

    float blendAmount = glGetMaterialFloatParameterWithOverride(packet,
        &GXCharacterSkinCustomParameters::blendAmount, sBlendAmountOverride);
    float alphaValue = glGetMaterialFloatParameterWithOverride(packet,
        &GXCharacterSkinCustomParameters::alphaValue, sAlphaValueOverride);
    if (alphaValue == 0.0f)
        return;

    nlFloatColour blendColour;
    nlFloatColourSet(blendColour, blendAmount, blendAmount, blendAmount, blendAmount);
    gxSetTevKColour(GX_KCOLOR0, blendColour);
    nlFloatColour alphaColour;
    nlFloatColourSet(alphaColour, alphaValue, alphaValue, alphaValue, alphaValue);
    gxSetTevKColour(GX_KCOLOR1, alphaColour);

    static_cast<GXCharacterSkinCustomMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXCharacterSkinCustomMaterialProgram*>(this)->BindParameters(packet);

    bool enabled = static_cast<const GXCharacterSkinCustomParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (enabled != sCharacterSkinLightingEnabled)
    {
        sCharacterSkinLightingEnabled = enabled;
        Deactivate();
        glxConfigureCharacterSkinCustomLighting(enabled);
    }

    if (sEnableCharacterSkinLighting && sUseCharacterLightRamp)
    {
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
        glx_BindTexture(2, &texture);
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sCharacterSkinViewMatrix);
    SetGameObjectShadowModelMatrix(packet->matrix);

    if (packet->skinnedVertices == 0)
    {
        unsigned int numSkinMatrices;
        const GXCharacterSkinCustomParameters* parameters = static_cast<const GXCharacterSkinCustomParameters*>(packet->materialParameters);
        numSkinMatrices = parameters->skinMatrixBytes / sizeof(*parameters->skinMatrices);
        glGetDrawSyncLog()->AddMarker("CharacterSkinCustom - RLXLoadSkinMatrices");
        glx_LoadSkinMatrices(parameters->skinMatrices, numSkinMatrices, &modelview, 1);
        glGetDrawSyncLog()->AddMarker("CharacterSkinCustom - After RLXLoadSkinMatrices");
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    ApplyGameObjectShadowLighting(1, static_cast<const GXCharacterSkinCustomParameters*>(packet->materialParameters)->shadowLevel);

    if (sEnableCharacterSkinLighting && alphaValue != 1.0f)
    {
        gxSaveZMode();
        if (sDrawAlphaDepthPass)
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
