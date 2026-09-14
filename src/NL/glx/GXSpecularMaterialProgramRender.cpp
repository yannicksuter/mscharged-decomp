#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXSpecularMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

static bool sSpecularRenderingEnabled = true;
static bool sSpecularDefaultLightingEnabled = true;
static float sSpecularBlendOverride = 1.0f;
static float sSpecularAlphaOverride = 1.0f;
static bool sSpecularDepthPrepassEnabled = true;

static nlMatrix4 sSpecularViewMatrix;
static int sSpecularLightCount;
static float sSpecularExponent;
static bool sSpecularLightingEnabled;

extern "C" void glxConfigureSpecularLighting(bool enabled)
{
    bool subtract;
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    SetGameObjectSpecularLightingEnabled(1, sSpecularLightCount);
    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(enabled, sSpecularLightCount, 0);
        subtract = ShouldDoubleGameObjectLighting() != 0;
        numChans = 2;
        numTexGens = 3;
        numTevStages = 5;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        gxSetTevOrder(3, 2, 2, 255);
        gxSetTevOrder(4, 255, 255, 5);
        gxSetTevColourOp(2, 0, 0, subtract, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevColourIn(3, 15, 14, 8, 15);
        gxSetTevColourIn(4, 15, 0, 10, 2);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
    }
    else
    {
        numChans = 2;
        numTexGens = 3;
        numTevStages = 5;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        gxSetTevOrder(3, 2, 2, 255);
        gxSetTevOrder(4, 255, 255, 5);
        gxSetTevColourOp(2, 0, 0, 0, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevColourIn(3, 15, 14, 8, 15);
        gxSetTevColourIn(4, 15, 0, 10, 2);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXSpecularMaterialProgram>::Activate(GLView* view)
{
    sSpecularExponent = 0.0f;
    static_cast<GXSpecularMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sSpecularViewMatrix);
    SetGameObjectShadowViewMatrix(&sSpecularViewMatrix);
    SetGameObjectShadowModelMatrix(-1);
    sSpecularLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sSpecularLightCount, view, 0);
    sSpecularLightingEnabled = sSpecularDefaultLightingEnabled;
    glxConfigureSpecularLighting(sSpecularDefaultLightingEnabled);
}

template <>
void GXMaterialProgramImpl<GXSpecularMaterialProgram>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sSpecularLightCount, 1);
    SetGameObjectSpecularLightingEnabled(0, sSpecularLightCount);
}

template <>
void GXMaterialProgramImpl<GXSpecularMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXSpecularParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXSpecularMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!sSpecularRenderingEnabled)
        return;

    float alphaValue = 1.0f != sSpecularAlphaOverride ? sSpecularAlphaOverride : static_cast<const GXSpecularParameters*>(packet->materialParameters)->alphaValue;
    float blendAmount = 1.0f != sSpecularBlendOverride ? sSpecularBlendOverride : static_cast<const GXSpecularParameters*>(packet->materialParameters)->blendAmount;
    float specularExponent = static_cast<const GXSpecularParameters*>(packet->materialParameters)->specularExponent;
    float specularLevel = static_cast<const GXSpecularParameters*>(packet->materialParameters)->specularLevel;
    nlFloatColour specularColour;
    nlFloatColourSet(specularColour,
        static_cast<const GXSpecularParameters*>(packet->materialParameters)->specularColour);

    if (alphaValue == 0.0f)
        return;

    nlFloatColour blendColour;
    nlFloatColourSet(blendColour, blendAmount, blendAmount, blendAmount, blendAmount);
    gxSetTevKColour(GX_KCOLOR0, blendColour);
    nlFloatColour alphaColour;
    nlFloatColourSet(alphaColour, alphaValue, alphaValue, alphaValue, alphaValue);
    gxSetTevKColour(GX_KCOLOR1, alphaColour);

    specularColour.c[0] *= specularLevel;
    specularColour.c[1] *= specularLevel;
    specularColour.c[2] *= specularLevel;
    specularColour.c[3] *= specularLevel;
    gxSetTevKColour(GX_KCOLOR2, specularColour);

    if (sSpecularExponent != specularExponent)
    {
        sSpecularExponent = specularExponent;
        for (int i = 0; i < sSpecularLightCount; ++i)
        {
            GameObjectLight* light = GetGameObjectLight(i, 0);
            LoadGameObjectSpecularLight(i, light, specularExponent, sSpecularViewMatrix);
        }
    }

    static_cast<GXSpecularMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXSpecularMaterialProgram*>(this)->BindParameters(packet);

    bool enabled = static_cast<const GXSpecularParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (enabled != sSpecularLightingEnabled)
    {
        sSpecularLightingEnabled = enabled;
        Deactivate();
        glxConfigureSpecularLighting(enabled);
    }

    if (sSpecularDefaultLightingEnabled)
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
        glx_BindTexture(3, &texture);
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sSpecularViewMatrix);
    SetGameObjectShadowModelMatrix(packet->matrix);

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(static_cast<const GXSpecularParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXSpecularParameters*>(packet->materialParameters)->skinMatricesSize / 48, &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    ApplyGameObjectShadowLighting(1, static_cast<const GXSpecularParameters*>(packet->materialParameters)->shadowLevel);

    if (sSpecularDefaultLightingEnabled && alphaValue != 1.0f)
    {
        gxSaveZMode();
        if (sSpecularDepthPrepassEnabled)
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
