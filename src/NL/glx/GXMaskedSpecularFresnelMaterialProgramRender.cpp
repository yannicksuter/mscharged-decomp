#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include <revolution/mtx.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlColour.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaskedSpecularFresnelMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"

enum MaskedSpecularFresnelLightingMode
{
    LIGHTING_NONE,
    LIGHTING_VERTEX,
    LIGHTING_VERTEX_DOUBLE,
    LIGHTING_TEXTURE,
    LIGHTING_UNCONFIGURED
};

Mtx sMaskedSpecularFresnelLookupMatrix = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

bool gMaskedSpecularFresnelEnabled = true;
float gMaskedSpecularFresnelRampOverride = -0.1f;
bool gMaskedSpecularFresnelShadowsEnabled = true;

nlMatrix4 sMaskedSpecularFresnelViewMatrix;
unsigned long sMaskedSpecularFresnelTextures[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};

bool sMaskedSpecularFresnelRampDisabled;
bool sMaskedSpecularFresnelShadowsActive;
unsigned long sMaskedSpecularFresnelModelMatrix;
nlVector2 sMaskedSpecularLookupScale;
int sMaskedSpecularFresnelLightCount;
int sMaskedSpecularFresnelLightingMode;
unsigned long sMaskedSpecularFresnelBoundTexture;

extern "C" void glxConfigureMaskedSpecularFresnelLighting(int mode)
{
    sMaskedSpecularFresnelLightingMode = mode;
    gxSetNumChans(1);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);

    if (mode != LIGHTING_NONE)
        SetGameObjectLightingEnabled(1, sMaskedSpecularFresnelLightCount, 1);
    if (mode == LIGHTING_VERTEX || mode == LIGHTING_VERTEX_DOUBLE)
        SetGameObjectAmbientLightingEnabled(1);

    if (mode == LIGHTING_VERTEX_DOUBLE)
    {
        gxSetNumTexGens(4);
        gxSetNumTevStages(5);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_ONE, GX_CC_CPREV, GX_CC_C0);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else if (mode == LIGHTING_NONE || mode == LIGHTING_VERTEX)
    {
        gxSetNumTexGens(4);
        gxSetNumTevStages(4);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    }
    else if (mode == LIGHTING_TEXTURE)
    {
        GXTevScale colourScale = ShouldDoubleGameObjectLighting() ? GX_CS_SCALE_2 : GX_CS_SCALE_1;
        gxSetNumTexGens(5);
        gxSetNumTevStages(6);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD4, GX_TEXMAP4, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD4, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG1);
        gxSetTevColourOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, colourScale, true, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_ONE, GX_CC_CPREV, GX_CC_C1);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
}

template <>
void GXMaterialProgramImpl<GXMaskedSpecularFresnelMaterialProgram>::Activate(GLView* view)
{
    sMaskedSpecularFresnelLightingMode = LIGHTING_UNCONFIGURED;
    static_cast<GXMaskedSpecularFresnelMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sMaskedSpecularFresnelViewMatrix);
    sMaskedSpecularFresnelModelMatrix = -1;
    sMaskedSpecularLookupScale.x = -1.0f;
    sMaskedSpecularLookupScale.y = -1.0f;
    sMaskedSpecularFresnelBoundTexture = -1;
    sMaskedSpecularFresnelLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sMaskedSpecularFresnelLightCount, view, 0);

    glTextureBinding texture(GetGameObjectLightTexture());
    texture.textureIndex = 0xFFFF;
    texture.flags = 0;
    texture.SetWrapS(true);
    texture.SetWrapT(true);
    texture.unknown07 = 0;
    glx_BindTexture(4, &texture);
    GXLoadTexMtxImm(sMaskedSpecularFresnelLookupMatrix, GX_PTTEXMTX1, GX_MTX3x4);
}

template <>
void GXMaterialProgramImpl<GXMaskedSpecularFresnelMaterialProgram>::Deactivate()
{
    if (sMaskedSpecularFresnelShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sMaskedSpecularFresnelShadowsActive = false;
    }

    SetGameObjectAmbientLightingEnabled(0);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY);

    if (sMaskedSpecularFresnelLightingMode == LIGHTING_TEXTURE)
    {
        gxSetTexCoordGen(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
    else if (sMaskedSpecularFresnelLightingMode == LIGHTING_VERTEX_DOUBLE)
    {
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }

    SetGameObjectLightingEnabled(0, sMaskedSpecularFresnelLightCount, 1);
}

template <>
void GXMaterialProgramImpl<GXMaskedSpecularFresnelMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMaskedSpecularFresnelParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXMaskedSpecularFresnelMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gMaskedSpecularFresnelEnabled)
        return;

    GXMaskedSpecularFresnelMaterialProgram* program = static_cast<GXMaskedSpecularFresnelMaterialProgram*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    const GXMaskedSpecularFresnelParameters* parameters = static_cast<const GXMaskedSpecularFresnelParameters*>(packet->materialParameters);
    float specularAmount = parameters->specularAmount;
    float specularScaleX = parameters->specularScaleX;
    float specularScaleY = parameters->specularScaleY;
    float fresnelRamp = parameters->fresnelRamp;
    if (gMaskedSpecularFresnelRampOverride >= 0.0f)
        fresnelRamp = gMaskedSpecularFresnelRampOverride;

    nlFloatColour specularColour = {
        { specularAmount, specularAmount, specularAmount, specularAmount }
    };
    nlColour specularColour8;
    ConvertColour(specularColour8, specularColour);
    GXColor specularGXColour = { specularColour8.c[0], specularColour8.c[1], specularColour8.c[2], specularColour8.c[3] };
    GXSetTevKColor(GX_KCOLOR0, specularGXColour);

    if (sMaskedSpecularLookupScale.x != specularScaleX || sMaskedSpecularLookupScale.y != specularScaleY)
    {
        Mtx textureMatrix = {
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 0.5f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        textureMatrix[0][0] = 0.5f * specularScaleX;
        textureMatrix[1][1] = -0.5f * specularScaleY;
        GXLoadTexMtxImm(textureMatrix, GX_PTTEXMTX0, GX_MTX3x4);
        sMaskedSpecularLookupScale.x = specularScaleX;
        sMaskedSpecularLookupScale.y = specularScaleY;
    }

    int mode;
    if (static_cast<const GXMaskedSpecularFresnelParameters*>(packet->materialParameters)->lightingEnabled == 0 || IsGameObjectLightingEnabled() == 0)
    {
        mode = LIGHTING_NONE;
    }
    else if (ShouldUseGameObjectLightTexture(1) != 0)
    {
        mode = LIGHTING_TEXTURE;
    }
    else
    {
        int doubleLighting = ShouldDoubleGameObjectLighting();
        mode = LIGHTING_VERTEX;
        if (doubleLighting != 0)
            mode = LIGHTING_VERTEX_DOUBLE;
    }

    if (mode != sMaskedSpecularFresnelLightingMode)
    {
        Deactivate();
        glxConfigureMaskedSpecularFresnelLighting(mode);
    }

    unsigned long texture;
    if (sMaskedSpecularFresnelRampDisabled || fresnelRamp < 0.2f)
        texture = sMaskedSpecularFresnelTextures[0];
    else if (fresnelRamp < 0.95f)
        texture = sMaskedSpecularFresnelTextures[1];
    else if (fresnelRamp < 1.666f)
        texture = sMaskedSpecularFresnelTextures[2];
    else if (fresnelRamp < 3.25f)
        texture = sMaskedSpecularFresnelTextures[3];
    else
        texture = sMaskedSpecularFresnelTextures[4];

    if (sMaskedSpecularFresnelBoundTexture != texture)
    {
        glTextureBinding textureState;
        textureState.texture = texture;
        textureState.textureIndex = 0xFFFF;
        textureState.flags = 0;
        textureState.SetWrapS(true);
        textureState.SetWrapT(true);
        textureState.unknown07 = 0;
        glx_BindTexture(3, &textureState);
        sMaskedSpecularFresnelBoundTexture = texture;
    }

    if (packet->matrix != sMaskedSpecularFresnelModelMatrix)
    {
        sMaskedSpecularFresnelModelMatrix = packet->matrix;
        nlMatrix4 model;
        nlMatrix4 modelview;
        Mtx modelViewMatrix;
        Mtx normalMatrix;
        glGetMatrix(packet->matrix, model);
        nlMultMatrices(modelview, model, sMaskedSpecularFresnelViewMatrix);
        glxCopyMatrix(modelViewMatrix, modelview);
        PSMTXInvXpose(modelViewMatrix, normalMatrix);
        GXLoadTexMtxImm(normalMatrix, GX_TEXMTX0, GX_MTX3x4);
    }

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXMaskedSpecularFresnelParameters*>(packet->materialParameters)->shadowEnabled == 1 && gMaskedSpecularFresnelShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sMaskedSpecularFresnelShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sMaskedSpecularFresnelShadowsActive = true;
        }
    }
    else if (sMaskedSpecularFresnelShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sMaskedSpecularFresnelShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        program->DrawIndexed(packet);
    else
        program->DrawDirect(packet);
}
