#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "Game/Camera/CameraMan.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXCameraScrolledOverlayMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

int gCameraOverlayStageCount = 4;
bool gCameraOverlayShadowsEnabled = true;

nlMatrix4 sCameraOverlayViewMatrix;
int sCameraOverlayLightCount;
bool sCameraOverlayLightingActive;
bool sCameraOverlayShadowsActive;

extern "C" void glxConfigureCameraScrolledOverlay()
{
    gxSetNumChans(1);
    gxSetNumTevStages(gCameraOverlayStageCount);
    gxSetNumTexGens(3);

    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX1);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
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
void GXMaterialProgramImpl<GXCameraScrolledOverlayMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXCameraScrolledOverlayMaterialProgram*>(this)->ConfigureVertexFormat(true);
    glxConfigureCameraScrolledOverlay();
    view->m_Interface->GetViewMatrix(sCameraOverlayViewMatrix);
    sCameraOverlayLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sCameraOverlayLightCount, view, 0);
    sCameraOverlayLightingActive = false;
    SetGameObjectLightingEnabled(0, sCameraOverlayLightCount, 1);
    SetGameObjectAmbientLightingEnabled(0);
}

template <>
void GXMaterialProgramImpl<GXCameraScrolledOverlayMaterialProgram>::Deactivate()
{
    if (sCameraOverlayShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sCameraOverlayShadowsActive = false;
    }

    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);

    if (sCameraOverlayLightingActive)
    {
        SetGameObjectLightingEnabled(0, sCameraOverlayLightCount, 1);
        SetGameObjectAmbientLightingEnabled(0);
        sCameraOverlayLightingActive = false;
    }
}

template <>
void GXMaterialProgramImpl<GXCameraScrolledOverlayMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXCameraScrolledOverlayMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXCameraScrolledOverlayMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXCameraScrolledOverlayMaterialProgram*>(this)->BindParameters(packet);

    const nlVector3& cameraPosition = cCameraManager::PeekCamera()->GetCameraPosition();
    float overlayScale = static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->overlayScale;
    float cameraScroll = static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->cameraScroll;
    float inverseScale = overlayScale == 0.0f ? 1.0f : 1.0f / overlayScale;

    Mtx textureMatrix;
    textureMatrix[0][1] = 0.0f;
    textureMatrix[0][2] = 0.0f;
    textureMatrix[0][3] = 0.0f;
    textureMatrix[1][0] = 0.0f;
    textureMatrix[1][2] = 0.0f;
    textureMatrix[1][3] = 0.0f;
    textureMatrix[2][0] = 0.0f;
    textureMatrix[2][1] = 0.0f;
    textureMatrix[2][3] = 0.0f;
    textureMatrix[0][0] = inverseScale;
    textureMatrix[1][1] = inverseScale;
    textureMatrix[2][2] = 1.0f;
    textureMatrix[0][3] += 0.5f - cameraScroll * (cameraPosition.x * inverseScale);
    textureMatrix[1][3] += 0.5f - cameraScroll * (cameraPosition.y * inverseScale);
    GXLoadTexMtxImm(textureMatrix, GX_TEXMTX1, GX_MTX3x4);

    if (static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->lightingEnabled == 1)
    {
        if (!sCameraOverlayLightingActive)
        {
            SetGameObjectLightingEnabled(1, sCameraOverlayLightCount, 1);
            SetGameObjectAmbientLightingEnabled(1);
            sCameraOverlayLightingActive = true;
        }
    }
    else if (sCameraOverlayLightingActive)
    {
        SetGameObjectLightingEnabled(0, sCameraOverlayLightCount, 1);
        SetGameObjectAmbientLightingEnabled(0);
        sCameraOverlayLightingActive = false;
    }

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->shadowEnabled == 1
        && gCameraOverlayShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sCameraOverlayShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sCameraOverlayShadowsActive = true;
        }
    }
    else if (sCameraOverlayShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sCameraOverlayShadowsActive = false;
    }

    if (static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->diffuseWrapEnabled == 1)
    {
        glTextureBinding* texture = &static_cast<GXCameraScrolledOverlayParameters*>(packet->materialParameters)->diffuseTexture;
        texture->SetWrapS(true);
        texture->SetWrapT(true);
    }

    float overlayAmount = static_cast<const GXCameraScrolledOverlayParameters*>(packet->materialParameters)->overlayAmount;
    nlFloatColour overlayColour = { { overlayAmount, overlayAmount, overlayAmount, overlayAmount } };
    nlColour overlayColour8;
    ConvertColour(overlayColour8, overlayColour);
    GXColor overlayGXColour = { overlayColour8.c[0], overlayColour8.c[1], overlayColour8.c[2], overlayColour8.c[3] };
    GXSetTevKColor(GX_KCOLOR0, overlayGXColour);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXCameraScrolledOverlayMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXCameraScrolledOverlayMaterialProgram*>(this)->DrawDirect(packet);
}
