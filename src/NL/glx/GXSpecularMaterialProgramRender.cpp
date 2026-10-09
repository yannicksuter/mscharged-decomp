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
#include "Game/SharedStaticStorage.h"

static bool sSpecularRenderingEnabled = true;
static bool sSpecularDefaultLightingEnabled = true;
static float sSpecularBlendOverride = 1.0f;
static float sSpecularAlphaOverride = 1.0f;
static bool sSpecularDepthPrepassEnabled = true;

static nlMatrix4 sSpecularViewMatrix;
static int sSpecularLightCount;
static float sSpecularExponent;
static bool sSpecularLightingEnabled;

void glxConfigureSpecularLighting(bool enabled)
{
    bool doubleLighting;
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    SetGameObjectSpecularLightingEnabled(1, sSpecularLightCount);
    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(enabled, sSpecularLightCount, 0);
        doubleLighting = ShouldDoubleGameObjectLighting() != 0;
        numChans = 2;
        numTexGens = 3;
        numTevStages = 5;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR1A1);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, doubleLighting, true, GX_TEVREG0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_C0);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        numChans = 2;
        numTexGens = 3;
        numTevStages = 5;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR1A1);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ONE, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_C0);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
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
    gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
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
        glx_LoadSkinMatrices(static_cast<const GXSpecularParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXSpecularParameters*>(packet->materialParameters)->skinMatrixBytes / 48, &modelview, 0);
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
