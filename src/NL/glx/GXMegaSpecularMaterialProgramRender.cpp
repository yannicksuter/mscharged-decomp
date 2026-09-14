#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMegaSpecularMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

static nlMatrix4 sMegaSpecularViewMatrix;
static int sMegaSpecularLightCount;
static float sMegaSpecularExponent;
static bool sMegaSpecularLightingEnabled;

extern "C" void glxConfigureMegaSpecularLighting(bool enabled)
{
    bool subtract;
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    SetGameObjectSpecularLightingEnabled(1, sMegaSpecularLightCount);
    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(enabled, sMegaSpecularLightCount, 0);
        subtract = ShouldDoubleGameObjectLighting() != 0;
        numChans = 2;
        numTexGens = 4;
        numTevStages = 6;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        gxSetTevOrder(3, 2, 2, 255);
        gxSetTevOrder(4, 255, 255, 5);
        gxSetTevOrder(5, 3, 3, 255);
        gxSetTevColourOp(2, 0, 0, subtract, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        GXSetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K3);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevColourIn(3, 15, 14, 8, 15);
        gxSetTevColourIn(4, 15, 0, 10, 2);
        gxSetTevColourIn(5, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
    else
    {
        numChans = 2;
        numTexGens = 4;
        numTevStages = 6;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        gxSetTevOrder(3, 2, 2, 255);
        gxSetTevOrder(4, 255, 255, 5);
        gxSetTevOrder(5, 3, 3, 255);
        gxSetTevColourOp(2, 0, 0, 0, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        GXSetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K3);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevColourIn(3, 15, 14, 8, 15);
        gxSetTevColourIn(4, 15, 0, 10, 2);
        gxSetTevColourIn(5, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularMaterialProgram>::Activate(GLView* view)
{
    sMegaSpecularExponent = 0.0f;
    static_cast<GXMegaSpecularMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sMegaSpecularViewMatrix);
    SetGameObjectShadowViewMatrix(&sMegaSpecularViewMatrix);
    SetGameObjectShadowModelMatrix(-1);
    sMegaSpecularLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sMegaSpecularLightCount, view, 0);
    sMegaSpecularLightingEnabled = true;
    glxConfigureMegaSpecularLighting(true);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularMaterialProgram>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sMegaSpecularLightCount, 1);
    SetGameObjectSpecularLightingEnabled(0, sMegaSpecularLightCount);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXMegaSpecularMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    float alphaValue = static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->alphaValue;
    nlFloatColour specularColour;
    nlFloatColourSet(specularColour,
        static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->specularColour);
    float blendAmount = static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->blendAmount;
    float megaBlend = static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->megaBlend;
    float specularExponent = static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->specularExponent;
    float specularLevel = static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->specularLevel;

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

    nlFloatColour megaColour;
    nlFloatColourSet(megaColour, megaBlend, megaBlend, megaBlend, megaBlend);
    gxSetTevKColour(GX_KCOLOR3, megaColour);

    if (sMegaSpecularExponent != specularExponent)
    {
        sMegaSpecularExponent = specularExponent;
        for (int i = 0; i < sMegaSpecularLightCount; ++i)
        {
            GameObjectLight* light = GetGameObjectLight(i, 0);
            LoadGameObjectSpecularLight(i, light, specularExponent, sMegaSpecularViewMatrix);
        }
    }

    static_cast<GXMegaSpecularMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXMegaSpecularMaterialProgram*>(this)->BindParameters(packet);

    bool enabled = static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (enabled != sMegaSpecularLightingEnabled)
    {
        sMegaSpecularLightingEnabled = enabled;
        Deactivate();
        glxConfigureMegaSpecularLighting(enabled);
    }

    gxSetNumTevStages(megaBlend == 0.0f ? 5 : 6);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sMegaSpecularViewMatrix);
    SetGameObjectShadowModelMatrix(packet->matrix);

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->skinMatricesSize / 48, &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    ApplyGameObjectShadowLighting(1, static_cast<const GXMegaSpecularParameters*>(packet->materialParameters)->shadowLevel);

    if (alphaValue != 1.0f)
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
