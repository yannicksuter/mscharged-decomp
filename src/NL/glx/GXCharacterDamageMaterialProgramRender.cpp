#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glMaterialParameters.h"
#include <revolution/mtx.h>
#include "NL/glx/GXCharacterDamageMaterialProgram.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxCharacterDamage.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxFresnelTexture.h"

static int sCharacterDamageStageCount = 5;
static int sCharacterDamageTexGenCount = 4;
static bool sCharacterDamageEnabled = true;
static int sCharacterDamageFresnelRampOverride = -1;
static bool sCharacterDamageShadowLightingEnabled = true;
static int sCharacterDamageFresnelTextureSlot = 255;
static int sCharacterDamageFresnelTexCoord = 255;

static Mtx sCharacterDamageFresnelLookupMatrix = {
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 sCharacterDamageViewMatrix;
static unsigned long sCharacterDamageFresnelTextures[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};
static unsigned long sCharacterDamageFresnelTextureIndices[5];

static bool sCharacterDamageHideDiffuse;
static float sCharacterDamageMegaBlendOverride;
static float sCharacterDamageSpecularAmountOverride;
static bool sCharacterDamageFresnelOnly;
static bool sCharacterDamageSpecularOnly;
static bool sCharacterDamageLightingOnly;
static bool sCharacterDamageForceDamage1;
static bool sCharacterDamageForceDamage2;
static unsigned long sCharacterDamageModelMatrix;
static float sCharacterDamageSpecularLookupScale[2];
static int sCharacterDamageLightCount;
static bool sCharacterDamageLightingEnabled;
static bool sCharacterDamageFresnelIndicesInitialized;
static unsigned long sCharacterDamageUnidentifiedCache;

extern "C" void glxConfigureCharacterDamageLighting(bool enabled, GXCharacterDamageMaterialProgram*)
{
    if (!sCharacterDamageEnabled)
        return;

    if (enabled)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(1, sCharacterDamageLightCount, 0);
    }

    sCharacterDamageFresnelTextureSlot = 3;
    sCharacterDamageFresnelTexCoord = 3;
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    if (sCharacterDamageLightingOnly)
        sCharacterDamageStageCount = 2;

    gxSetNumChans(1);
    gxSetNumTexGens(sCharacterDamageTexGenCount);
    gxSetNumTevStages(sCharacterDamageStageCount);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    if (sCharacterDamageHideDiffuse)
        gxSetTevColourIn(0, 15, 15, 15, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 4);
    if (enabled && ShouldDoubleGameObjectLighting() != 0)
        gxSetTevColourOp(0, 0, 0, 1, true, 0);

    gxSetTevOrder(1, 255, 255, enabled ? 4 : 255);
    gxSetTevColourIn(
        1, GX_CC_ZERO, sCharacterDamageLightingOnly ? GX_CC_ONE : GX_CC_CPREV, enabled ? GX_CC_RASC : GX_CC_ONE, GX_CC_ZERO);
    gxSetTevAlphaIn(1, 7, 7, 7, 0);
    gxSetTevColourOp(1, 0, 0, 0, true, !sCharacterDamageLightingOnly);

    gxSetTevOrder(2, 1, 1, 255);
    gxSetTevColourIn(2, 15, 14, 8, 15);
    gxSetTevAlphaIn(2, 7, 7, 7, 0);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K2);

    gxSetTevOrder(3, 2, 2, 255);
    gxSetTevColourIn(3, 15, 0, 8, 15);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);

    gxSetTevOrder(4, sCharacterDamageFresnelTexCoord, sCharacterDamageFresnelTextureSlot, 255);
    GXSetTexCoordGen2((GXTexCoordID)sCharacterDamageFresnelTexCoord, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
    gxSetTevColourIn(4, 15, 0, 8, 2);
    if (sCharacterDamageFresnelOnly)
        gxSetTevColourIn(4, 15, 12, 8, 15);
    if (sCharacterDamageSpecularOnly)
        gxSetTevColourIn(4, 15, 0, 8, 15);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
}

template <>
void GXMaterialProgramImpl<GXCharacterDamageMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXCharacterDamageMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sCharacterDamageViewMatrix);
    if (sCharacterDamageShadowLightingEnabled)
    {
        SetGameObjectShadowViewMatrix(&sCharacterDamageViewMatrix);
        SetGameObjectShadowModelMatrix(-1);
    }
    sCharacterDamageModelMatrix = -1;
    sCharacterDamageSpecularLookupScale[0] = -1.0f;
    sCharacterDamageSpecularLookupScale[1] = -1.0f;
    sCharacterDamageUnidentifiedCache = -1;
    GXLoadTexMtxImm(sCharacterDamageFresnelLookupMatrix, 67, GX_MTX3x4);
    sCharacterDamageLightCount = GetGameObjectLightCount(1, 1);
    LoadGameObjectLights(sCharacterDamageLightCount, view, true);
    sCharacterDamageLightingEnabled = true;
    glxConfigureCharacterDamageLighting(true, static_cast<GXCharacterDamageMaterialProgram*>(this));
}

template <>
void GXMaterialProgramImpl<GXCharacterDamageMaterialProgram>::Deactivate()
{
    sCharacterDamageFresnelTexCoord = 3;
    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sCharacterDamageLightCount, 1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevColourOp(1, 0, 0, 0, true, 0);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTexCoordGen(sCharacterDamageFresnelTexCoord, 1, sCharacterDamageFresnelTexCoord + 4, 60);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXCharacterDamageMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXCharacterDamageMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!sCharacterDamageEnabled)
        return;

    GXCharacterDamageMaterialProgram* program = static_cast<GXCharacterDamageMaterialProgram*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    float megaBlend = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->megaBlend;
    if (sCharacterDamageMegaBlendOverride > 0.0f)
        megaBlend = sCharacterDamageMegaBlendOverride;

    int stageCount = sCharacterDamageStageCount;
    if (static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->blackOnly != 0)
    {
        gxSetNumTevStages(1);
        gxSetTevColourIn(0, 15, 15, 15, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 6);
    }
    else
    {
        gxSetNumTevStages(stageCount);
        gxSetTevColourIn(0, 15, 12, 8, 15);
        bool damageEnabled[2];
        damageEnabled[0] = sCharacterDamageForceDamage1 || static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->damage1Enabled != 0;
        damageEnabled[1] = sCharacterDamageForceDamage2 || static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->damage2Enabled != 0;
        glxConfigureCharacterDamage(megaBlend, damageEnabled, stageCount, sCharacterDamageTexGenCount, 4, 5, 3, 4, 5, 3);
    }

    float alphaValue = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->alphaValue;
    if (alphaValue == 0.0f)
        return;
    float specularScaleX = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->specularScaleX;
    float specularScaleY = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->specularScaleY;
    int fresnelRamp = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->fresnelRamp;
    float specularAmount = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->specularAmount;
    if (sCharacterDamageSpecularAmountOverride != 0.0f)
        specularAmount = sCharacterDamageSpecularAmountOverride;
    if (sCharacterDamageFresnelRampOverride != -1)
        fresnelRamp = sCharacterDamageFresnelRampOverride;

    nlFloatColour whiteColour;
    nlFloatColourSet(whiteColour, 1.0f, 1.0f, 1.0f, 1.0f);
    gxSetTevKColour(GX_KCOLOR0, whiteColour);
    nlFloatColour alphaColour;
    nlFloatColourSet(alphaColour, alphaValue, alphaValue, alphaValue, alphaValue);
    gxSetTevKColour(GX_KCOLOR1, alphaColour);
    nlFloatColour specularColour;
    nlFloatColourSet(specularColour, specularAmount, specularAmount, specularAmount, specularAmount);
    gxSetTevKColour(GX_KCOLOR2, specularColour);

    bool enabled = static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->lightingEnabled == 1;
    if (sCharacterDamageLightingEnabled != enabled)
    {
        sCharacterDamageLightingEnabled = enabled;
        Deactivate();
        glxConfigureCharacterDamageLighting(enabled, program);
    }

    if (sCharacterDamageSpecularLookupScale[0] != specularScaleX || sCharacterDamageSpecularLookupScale[1] != specularScaleY)
    {
        Mtx specularTextureMatrix = {
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        specularTextureMatrix[0][0] = 0.5f * specularScaleX;
        specularTextureMatrix[1][1] = -0.5f * specularScaleY;
        GXLoadTexMtxImm(specularTextureMatrix, 64, GX_MTX3x4);
        sCharacterDamageSpecularLookupScale[0] = specularScaleX;
        sCharacterDamageSpecularLookupScale[1] = specularScaleY;
    }

    if (megaBlend == 0.0f)
    {
        unsigned long fresnelTexture = sCharacterDamageFresnelTextures[fresnelRamp];
        glxBindFresnelTexture(sCharacterDamageFresnelTextureSlot, fresnelTexture, fresnelRamp,
            sCharacterDamageFresnelTextureIndices, sCharacterDamageFresnelIndicesInitialized);
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    unsigned long modelMatrix = packet->matrix;
    glGetMatrix(modelMatrix, model);
    nlMultMatrices(modelview, model, sCharacterDamageViewMatrix);
    if (sCharacterDamageShadowLightingEnabled)
        SetGameObjectShadowModelMatrix(modelMatrix);

    if (modelMatrix != sCharacterDamageModelMatrix)
    {
        Mtx modelViewTransform;
        Mtx normalMatrix;
        sCharacterDamageModelMatrix = modelMatrix;
        glxCopyMatrix(modelViewTransform, modelview);
        PSMTXInvXpose(modelViewTransform, normalMatrix);
        GXLoadTexMtxImm(normalMatrix, 30, GX_MTX3x4);

        Mtx fresnelTextureMatrix;
        for (int i = 0; i < 4; i++)
        {
            fresnelTextureMatrix[0][i] = normalMatrix[2][i];
            fresnelTextureMatrix[1][i] = 0.0f;
        }
        GXLoadTexMtxImm(fresnelTextureMatrix, 33, GX_MTX2x4);
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->skinMatricesSize / 48, &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    if (sCharacterDamageShadowLightingEnabled)
        ApplyGameObjectShadowLighting(1, static_cast<const GXCharacterDamageParameters*>(packet->materialParameters)->shadowLevel);
    if (sCharacterDamageLightingEnabled && alphaValue != 1.0f)
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
