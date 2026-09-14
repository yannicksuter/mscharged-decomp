#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "Game/Camera/CameraMan.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXScrollingCameraOverlayMaterialProgram.h"
#include "NL/glx/GXMaterialProgramTextureAnimation.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

int gScrollingCameraOverlayStageCount = 4;
bool gScrollingCameraOverlayShadowsEnabled = true;

nlMatrix4 sScrollingCameraOverlayViewMatrix;
int sScrollingCameraOverlayLightCount;
bool sScrollingCameraOverlayLightingActive;
bool sScrollingCameraOverlayShadowsActive;

extern "C" void glxConfigureScrollingCameraOverlay()
{
    gxSetNumChans(1);
    gxSetNumTevStages(gScrollingCameraOverlayStageCount);
    gxSetNumTexGens(3);

    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX1, GX_TEXMTX0);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX1);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_TEXMTX2);
    gxSetTevKColourSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);

    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
}

template <>
void GXMaterialProgramImpl<GXScrollingCameraOverlayMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXScrollingCameraOverlayMaterialProgram*>(this)->ConfigureVertexFormat(true);
    glxConfigureScrollingCameraOverlay();
    view->m_Interface->GetViewMatrix(sScrollingCameraOverlayViewMatrix);
    sScrollingCameraOverlayLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sScrollingCameraOverlayLightCount, view, 0);
    sScrollingCameraOverlayLightingActive = false;
    SetGameObjectLightingEnabled(0, sScrollingCameraOverlayLightCount, 1);
    SetGameObjectAmbientLightingEnabled(0);
}

template <>
void GXMaterialProgramImpl<GXScrollingCameraOverlayMaterialProgram>::Deactivate()
{
    if (sScrollingCameraOverlayShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingCameraOverlayShadowsActive = false;
    }

    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);

    if (sScrollingCameraOverlayLightingActive)
    {
        SetGameObjectLightingEnabled(0, sScrollingCameraOverlayLightCount, 1);
        SetGameObjectAmbientLightingEnabled(0);
        sScrollingCameraOverlayLightingActive = false;
    }
}

template <>
void GXMaterialProgramImpl<GXScrollingCameraOverlayMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXScrollingCameraOverlayMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXScrollingCameraOverlayMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXScrollingCameraOverlayMaterialProgram*>(this)->BindParameters(packet);

    const nlVector3& cameraPosition = cCameraManager::PeekCamera()->GetCameraPosition();
    float cameraScroll = static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->cameraScroll;
    float inverseScale = 1.0f / static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->overlayScale;
    Mtx cameraTextureMatrix;
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 4; ++column)
            cameraTextureMatrix[row][column] = 0.0f;
    }
    cameraTextureMatrix[0][0] = inverseScale;
    cameraTextureMatrix[1][1] = inverseScale;
    cameraTextureMatrix[2][2] = 1.0f;
    cameraTextureMatrix[0][3] +=
        0.5f - cameraScroll * (cameraPosition.x * inverseScale);
    cameraTextureMatrix[1][3] +=
        0.5f - cameraScroll * (cameraPosition.y * inverseScale);
    GXLoadTexMtxImm(cameraTextureMatrix, GX_TEXMTX1, GX_MTX3x4);

    nlVector2 diffuseScrollSpeed;
    diffuseScrollSpeed.x = static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->diffuseScrollSpeedX;
    diffuseScrollSpeed.y = static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->diffuseScrollSpeedY;
    nlVector2 maskScrollSpeed = { 0.0f, 0.0f };
    // This parameter is tested by address in the renderer.
    if (&static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->maskScrollEnabled != 0)
        maskScrollSpeed = diffuseScrollSpeed;

    glxLoadScrollingTextureMatrix(GX_TEXMTX0, diffuseScrollSpeed);
    glxLoadScrollingTextureMatrix(GX_TEXMTX2, maskScrollSpeed);

    if (static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->lightingEnabled == 1)
    {
        if (!sScrollingCameraOverlayLightingActive)
        {
            SetGameObjectLightingEnabled(1, sScrollingCameraOverlayLightCount, 1);
            SetGameObjectAmbientLightingEnabled(1);
            sScrollingCameraOverlayLightingActive = true;
        }
    }
    else if (sScrollingCameraOverlayLightingActive)
    {
        SetGameObjectLightingEnabled(0, sScrollingCameraOverlayLightCount, 1);
        SetGameObjectAmbientLightingEnabled(0);
        sScrollingCameraOverlayLightingActive = false;
    }

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->shadowEnabled == 1
        && gScrollingCameraOverlayShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sScrollingCameraOverlayShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sScrollingCameraOverlayShadowsActive = true;
        }
    }
    else if (sScrollingCameraOverlayShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingCameraOverlayShadowsActive = false;
    }

    if (static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->diffuseWrapEnabled == 1)
    {
        glTextureBinding* texture = &static_cast<GXScrollingCameraOverlayParameters*>(packet->materialParameters)->diffuseTexture;
        texture->SetWrapS(true);
        texture->SetWrapT(true);
    }

    float overlayAmount = static_cast<const GXScrollingCameraOverlayParameters*>(packet->materialParameters)->overlayAmount;
    nlFloatColour overlayColour = { { overlayAmount, overlayAmount, overlayAmount, overlayAmount } };
    nlColour overlayColour8;
    ConvertColour(overlayColour8, overlayColour);
    GXColor overlayGXColour = { overlayColour8.c[0], overlayColour8.c[1], overlayColour8.c[2], overlayColour8.c[3] };
    GXSetTevKColor(GX_KCOLOR0, overlayGXColour);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXScrollingCameraOverlayMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXScrollingCameraOverlayMaterialProgram*>(this)->DrawDirect(packet);
}
