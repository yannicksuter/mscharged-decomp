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
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

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

extern "C" void glxConfigureCharacterSkinCustomLighting(bool enabled)
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

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 2, 2, 4);
        gxSetTexCoordGen(2, 10, 19, 60);
        gxSetTevColourOp(2, 0, 0, doubleLightRampColour, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
    }
    else if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        numChans = 1;
        numTexGens = 2;
        numTevStages = 3;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        if (ShouldDoubleGameObjectLighting() != 0)
            gxSetTevColourOp(2, 0, 0, 1, true, 0);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
    }
    else
    {
        numChans = 0;
        numTexGens = 2;
        numTevStages = 3;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
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
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetTexCoordGen(2, 1, 6, 60);
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

    float blendAmount;
    if (sBlendAmountOverride != 1.0f)
        blendAmount = sBlendAmountOverride;
    else
        blendAmount = static_cast<const GXCharacterSkinCustomParameters*>(packet->materialParameters)->blendAmount;
    float alphaValue;
    if (sAlphaValueOverride != 1.0f)
        alphaValue = sAlphaValueOverride;
    else
        alphaValue = static_cast<const GXCharacterSkinCustomParameters*>(packet->materialParameters)->alphaValue;
    if (alphaValue == 0.0f)
        return;

    nlFloatColour blendColour;
    nlFloatColourSet(blendColour, blendAmount, blendAmount, blendAmount, blendAmount);
    nlColour blendColour8;
    ConvertColour(blendColour8, blendColour);
    GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&blendColour8);
    nlFloatColour alphaColour;
    nlFloatColourSet(alphaColour, alphaValue, alphaValue, alphaValue, alphaValue);
    nlColour alphaColour8;
    ConvertColour(alphaColour8, alphaColour);
    GXSetTevKColor(GX_KCOLOR1, *(GXColor*)&alphaColour8);

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
        numSkinMatrices = parameters->skinMatricesSize / sizeof(*parameters->skinMatrices);
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
