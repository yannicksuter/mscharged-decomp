#include <revolution/gx.h>
#include "Game/Render/StadiumLoading.h"
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXScrollingDiffuseMaterialProgram.h"
#include "NL/glx/GXMaterialProgramTextureAnimation.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

enum ScrollingDiffuseLightingMode
{
    LIGHTING_NONE,
    LIGHTING_VERTEX,
    LIGHTING_VERTEX_DOUBLE,
    LIGHTING_TEXTURE,
    LIGHTING_UNCONFIGURED
};

bool gScrollingDiffuseEnabled = true;
bool gScrollingDiffuseScrollingEnabled = true;
bool gScrollingDiffuseShadowsEnabled = true;

nlMatrix4 sScrollingDiffuseViewMatrix;
bool sScrollingDiffuseShadowsActive;
int sScrollingDiffuseLightCount;
int sScrollingDiffuseLightingMode;
float sScrollingDiffuseActivationValues[2];

extern "C" void glxConfigureScrollingDiffuseLighting(int mode)
{
    sScrollingDiffuseLightingMode = mode;
    gxSetNumChans(1);

    if (mode != LIGHTING_NONE)
        SetGameObjectLightingEnabled(1, sScrollingDiffuseLightCount, 1);

    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);

    if (mode == LIGHTING_TEXTURE)
    {
        gxSetNumTexGens(2);
        gxSetNumTevStages(2);
        GXTevScale colourScale = ShouldDoubleGameObjectLighting() ? GX_CS_SCALE_2 : GX_CS_SCALE_1;

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, colourScale, true, GX_TEVPREV);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    }
    else
    {
        if (mode != LIGHTING_NONE)
        {
            SetGameObjectAmbientLightingEnabled(1);
            if (mode == LIGHTING_VERTEX_DOUBLE)
                gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
        }

        gxSetNumTexGens(1);
        gxSetNumTevStages(1);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    }
}

template <>
void GXMaterialProgramImpl<GXScrollingDiffuseMaterialProgram>::Activate(GLView* view)
{
    sScrollingDiffuseLightingMode = LIGHTING_UNCONFIGURED;
    sScrollingDiffuseActivationValues[0] = 10000000000.0f;
    sScrollingDiffuseActivationValues[1] = 10000000000.0f;
    static_cast<GXScrollingDiffuseMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sScrollingDiffuseViewMatrix);
    sScrollingDiffuseLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sScrollingDiffuseLightCount, view, 0);
}

template <>
void GXMaterialProgramImpl<GXScrollingDiffuseMaterialProgram>::Deactivate()
{
    if (sScrollingDiffuseShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingDiffuseShadowsActive = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    if (sScrollingDiffuseLightingMode == LIGHTING_TEXTURE)
    {
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    }
    else if (sScrollingDiffuseLightingMode != LIGHTING_NONE)
    {
        SetGameObjectAmbientLightingEnabled(0);
        if (sScrollingDiffuseLightingMode == LIGHTING_VERTEX_DOUBLE)
            gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }

    SetGameObjectLightingEnabled(0, sScrollingDiffuseLightCount, 1);
}

template <>
void GXMaterialProgramImpl<GXScrollingDiffuseMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(
        this, packet, static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->diffuseTexture.texture);

    if (static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->disableCulling != 0)
        glSetRasterState(packet->rasterState, GLS_Culling, 0);
    if (static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->forceDepthWrite != 0)
        glSetRasterState(packet->rasterState, GLS_DepthWrite, 1);
}

template <>
void GXMaterialProgramImpl<GXScrollingDiffuseMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gScrollingDiffuseEnabled)
        return;

    GXScrollingDiffuseMaterialProgram* program = static_cast<GXScrollingDiffuseMaterialProgram*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    if (static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->textureWrapEnabled == 1)
    {
        glTextureBinding* texture = &static_cast<GXScrollingDiffuseParameters*>(packet->materialParameters)->diffuseTexture;
        texture->SetWrapS(true);
        texture->SetWrapT(true);
    }

    int mode;
    if (static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->lightingEnabled == 0 || IsGameObjectLightingEnabled() == 0)
    {
        mode = LIGHTING_NONE;
    }
    else if (ShouldUseGameObjectLightTexture(0) != 0)
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

    if (sScrollingDiffuseLightingMode != mode)
    {
        Deactivate();
        glxConfigureScrollingDiffuseLighting(mode);
    }

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->shadowEnabled == 1 && gScrollingDiffuseShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sScrollingDiffuseShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sScrollingDiffuseShadowsActive = true;
        }
    }
    else if (sScrollingDiffuseShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingDiffuseShadowsActive = false;
    }

    if (sScrollingDiffuseLightingMode == LIGHTING_TEXTURE)
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
        glx_BindTexture(1, &texture);
    }

    nlVector2 scrollSpeed;
    scrollSpeed.x = gScrollingDiffuseScrollingEnabled ? static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->scrollSpeedX : 0.0f;
    scrollSpeed.y = gScrollingDiffuseScrollingEnabled ? static_cast<const GXScrollingDiffuseParameters*>(packet->materialParameters)->scrollSpeedY : 0.0f;
    glxLoadScrollingTextureMatrix(GX_TEXMTX0, scrollSpeed);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        program->DrawIndexed(packet);
    else
        program->DrawDirect(packet);
}
