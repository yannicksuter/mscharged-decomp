#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMegaDiffuseMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

static nlMatrix4 sMegaDiffuseViewMatrix;
static int sMegaDiffuseLightCount;
static bool sMegaDiffuseLightingEnabled;

extern "C" void glxConfigureMegaDiffuseLighting(bool enabled)
{
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    if (enabled)
    {
        SetGameObjectLightingEnabled(1, sMegaDiffuseLightCount, 0);
        SetGameObjectAmbientLightingEnabled(1);
        numChans = 1;
        numTexGens = 3;
        numTevStages = 4;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        gxSetTevOrder(3, 2, 2, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        if (ShouldDoubleGameObjectLighting() != 0)
            gxSetTevColourOp(2, 0, 0, 1, true, 0);

        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevColourIn(3, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }
    else
    {
        numChans = 0;
        numTexGens = 3;
        numTevStages = 4;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        gxSetTevOrder(3, 2, 2, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevColourIn(3, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMegaDiffuseMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXMegaDiffuseMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sMegaDiffuseViewMatrix);
    SetGameObjectShadowViewMatrix(&sMegaDiffuseViewMatrix);
    SetGameObjectShadowModelMatrix(-1);
    sMegaDiffuseLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sMegaDiffuseLightCount, view, 0);
    sMegaDiffuseLightingEnabled = true;
    glxConfigureMegaDiffuseLighting(true);
}

template <>
void GXMaterialProgramImpl<GXMegaDiffuseMaterialProgram>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sMegaDiffuseLightCount, 1);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
}

template <>
void GXMaterialProgramImpl<GXMegaDiffuseMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXMegaDiffuseMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    float blendAmount = static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->blendAmount;
    float alphaValue = static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->alphaValue;
    float megaBlend = static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->megaBlend;

    if (alphaValue == 0.0f)
        return;

    nlFloatColour blendColour;
    nlFloatColourSet(blendColour, blendAmount, blendAmount, blendAmount, blendAmount);
    gxSetTevKColour(GX_KCOLOR0, blendColour);

    nlFloatColour alphaColour;
    nlFloatColourSet(alphaColour, alphaValue, alphaValue, alphaValue, alphaValue);
    gxSetTevKColour(GX_KCOLOR1, alphaColour);

    nlFloatColour megaColour;
    nlFloatColourSet(megaColour, megaBlend, megaBlend, megaBlend, megaBlend);
    gxSetTevKColour(GX_KCOLOR2, megaColour);

    static_cast<GXMegaDiffuseMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXMegaDiffuseMaterialProgram*>(this)->BindParameters(packet);

    bool enabled = static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (enabled != sMegaDiffuseLightingEnabled)
    {
        sMegaDiffuseLightingEnabled = enabled;
        Deactivate();
        glxConfigureMegaDiffuseLighting(enabled);
    }

    gxSetNumTevStages(megaBlend == 0.0f ? 3 : 4);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sMegaDiffuseViewMatrix);
    SetGameObjectShadowModelMatrix(packet->matrix);

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->skinMatricesSize / 48, &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    ApplyGameObjectShadowLighting(1, static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->shadowLevel);

    if (enabled && alphaValue != 1.0f)
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
