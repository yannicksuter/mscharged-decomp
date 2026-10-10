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
#include "Game/SharedStaticStorage.h"

static nlMatrix4 sMegaDiffuseViewMatrix;
static int sMegaDiffuseLightCount;
static bool sMegaDiffuseLightingEnabled;

void glxConfigureMegaDiffuseLighting(bool enabled)
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

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        if (ShouldDoubleGameObjectLighting() != 0)
            gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);

        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        numChans = 0;
        numTexGens = 3;
        numTevStages = 4;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ONE, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
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
    gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
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
        glx_LoadSkinMatrices(static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXMegaDiffuseParameters*>(packet->materialParameters)->skinMatrixBytes / 48, &modelview, 0);
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
