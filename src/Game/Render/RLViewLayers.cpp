#include <revolution/gx.h>

#include "NL/gl/glPlat.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/Frustum.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxSend.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/Render/HighRange.h"

#include "Game/Render/ShadowVolume.h"
#include "Game/UnidentifiedStaticStorage.h"
void CopyShadowVolumeColour(const GXColor* colour);

struct RLViewLayerDesc
{
    /* 0x00 */ eCLV layer;
    /* 0x04 */ const char* name;
    /* 0x08 */ bool useDisplayTarget;
    /* 0x0C */ GLViewSortMode sortMode;
};

const nlMatrix4 sIdentityMatrix = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static const GXColor sShadowVolumeColour = { 0, 0, 0, 0x9B };

static char sDofTargetName[] = "dof";
static char sPipTargetName[] = "pip";

TweakValueBool eCLV_ImpostorTextureEnabled("eCLV_ImpostorTextureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ShadowTextureEnabled("eCLV_ShadowTextureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_GrabTextureEnabled("eCLV_GrabTextureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_PictureInPictureEnabled("eCLV_PictureInPictureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_PictureInPictureAlphaEnabled("eCLV_PictureInPictureAlphaEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_NoFogEnabled("eCLV_NoFogEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ShadowedEnabled("eCLV_ShadowedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_WorldShadowedEnabled("eCLV_WorldShadowedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_UnshadowedEnabled("eCLV_UnshadowedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_MegastrikeBackgroundEnabled("eCLV_MegastrikeBackgroundEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ImpostorOutEnabled("eCLV_ImpostorOutEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_CharactersEnabled("eCLV_CharactersEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_PeachPhoto3DEnabled("eCLV_PeachPhoto3DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_MoreCharactersEnabled("eCLV_MoreCharactersEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_WorldAlphaBlendedEnabled("eCLV_WorldAlphaBlendedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_HighRange3DEnabled("eCLV_HighRange3DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_HighRange3DNoFogEnabled("eCLV_HighRange3DNoFogEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_HighRangeChainEnabled("eCLV_HighRangeChainEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_HighRange2DEnabled("eCLV_HighRange2DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_BigBlackPolygonEnabled("eCLV_BigBlackPolygonEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ShadowVolumeEnabled("eCLV_ShadowVolumeEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ShadowVolumeBlendEnabled("eCLV_ShadowVolumeBlendEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_UnsortedPerspectiveEnabled("eCLV_UnsortedPerspectiveEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_DepthOfFieldEnabled("eCLV_DepthOfFieldEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_LingeringParticlesEnabled("eCLV_LingeringParticlesEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ParticlesEnabled("eCLV_ParticlesEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_BallChargeAlphaBlendedEnabled("eCLV_BallChargeAlphaBlendedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_CoPlanarEnabled("eCLV_CoPlanarEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_InvisiblePlaneEnabled("eCLV_InvisiblePlaneEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ElectricFenceEnabled("eCLV_ElectricFenceEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_PreWarbleEnabled("eCLV_PreWarbleEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_WarbleEnabled("eCLV_WarbleEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_WarbleBlendEnabled("eCLV_WarbleBlendEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_CameraSpaceEnabled("eCLV_CameraSpaceEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ScreenBlurEnabled("eCLV_ScreenBlurEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ScreenBlur2Enabled("eCLV_ScreenBlur2Enabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_ScreenGrabEnabled("eCLV_ScreenGrabEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_FrontEndEnabled("eCLV_FrontEndEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_UnsortedOrtho640Enabled("eCLV_UnsortedOrtho640Enabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_UnsortedSquareOrthoEnabled("eCLV_UnsortedSquareOrthoEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_Transitions3DEnabled("eCLV_Transitions3DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_TransitionsEnabled("eCLV_TransitionsEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_Anark3D_BGEnabled("eCLV_Anark3D_BGEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_AnarkEnabled("eCLV_AnarkEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_Anark3D_FGEnabled("eCLV_Anark3D_FGEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_HomeButtonFadeOutEnabled("eCLV_HomeButtonFadeOutEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_DebugEnabled("eCLV_DebugEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_DebugSquareEnabled("eCLV_DebugSquareEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool eCLV_NumEnabled("eCLV_NumEnabled", "/Rendering/RLView Toggles", true);

GLView* sShadowPartitionViews[11];
RLViewCamera sShadowPartitionCameras[11];
RLViewRect sShadowPartitionRects[11];
char sShadowPartitionNames[11][16];
u32 sShadowPartitionTextures[11];
GLRenderPair sShadowPartitionPairs[11];
RLView* sLayerViews[eCLV_Num];
RLViewCamera sPerspectiveCamera;
RLViewCamera sPipCamera;
RLViewCamera sNoTranslationCamera;
RLViewCamera sCameraSpaceCamera;
RLViewCamera sTransitions3DCamera;
RLViewCamera sAnark3DCamera;
RLViewOrthoCamera sOrthoCamera;
RLViewOrthoCamera sOrthoCenteredCamera;
RLViewOrthoCamera sOrtho640Camera;

GLRenderPair sWarbleColourTarget;
GLRenderPair sWarbleOffsetTarget;
GLRenderPair sWarbleTextureTarget;
GLRenderPair sDofTarget;
GLRenderPair sScreenGrabTarget;
GLRenderPair sTarget_806E1950;
bool sWidescreen;

void fn_80271DE0()
{
    sLayerViews[eCLV_ImpostorTexture]->m_Visible = eCLV_ImpostorTextureEnabled.GetValue();
    sLayerViews[eCLV_ShadowTexture]->m_Visible = eCLV_ShadowTextureEnabled.GetValue();
    sLayerViews[eCLV_GrabTexture]->m_Visible = eCLV_GrabTextureEnabled.GetValue();
    sLayerViews[eCLV_PictureInPicture]->m_Visible = eCLV_PictureInPictureEnabled.GetValue();
    sLayerViews[eCLV_PictureInPictureAlpha]->m_Visible = eCLV_PictureInPictureAlphaEnabled.GetValue();
    sLayerViews[eCLV_NoFog]->m_Visible = eCLV_NoFogEnabled.GetValue();
    sLayerViews[eCLV_Shadowed]->m_Visible = eCLV_ShadowedEnabled.GetValue();
    sLayerViews[eCLV_WorldShadowed]->m_Visible = eCLV_WorldShadowedEnabled.GetValue();
    sLayerViews[eCLV_Unshadowed]->m_Visible = eCLV_UnshadowedEnabled.GetValue();
    sLayerViews[eCLV_MegastrikeBackground]->m_Visible = eCLV_MegastrikeBackgroundEnabled.GetValue();
    sLayerViews[eCLV_ImpostorOut]->m_Visible = eCLV_ImpostorOutEnabled.GetValue();
    sLayerViews[eCLV_Characters]->m_Visible = eCLV_CharactersEnabled.GetValue();
    sLayerViews[eCLV_PeachPhoto3D]->m_Visible = eCLV_PeachPhoto3DEnabled.GetValue();
    sLayerViews[eCLV_MoreCharacters]->m_Visible = eCLV_MoreCharactersEnabled.GetValue();
    sLayerViews[eCLV_WorldAlphaBlended]->m_Visible = eCLV_WorldAlphaBlendedEnabled.GetValue();
    sLayerViews[eCLV_HighRange3D]->m_Visible = eCLV_HighRange3DEnabled.GetValue();
    sLayerViews[eCLV_HighRange3DNoFog]->m_Visible = eCLV_HighRange3DNoFogEnabled.GetValue();
    sLayerViews[eCLV_HighRangeChain]->m_Visible = eCLV_HighRangeChainEnabled.GetValue();
    sLayerViews[eCLV_HighRange2D]->m_Visible = eCLV_HighRange2DEnabled.GetValue();
    sLayerViews[eCLV_BigBlackPolygon]->m_Visible = eCLV_BigBlackPolygonEnabled.GetValue();
    sLayerViews[eCLV_ShadowVolume]->m_Visible = eCLV_ShadowVolumeEnabled.GetValue();
    sLayerViews[eCLV_ShadowVolumeBlend]->m_Visible = eCLV_ShadowVolumeBlendEnabled.GetValue();
    sLayerViews[eCLV_UnsortedPerspective]->m_Visible = eCLV_UnsortedPerspectiveEnabled.GetValue();
    sLayerViews[eCLV_DepthOfField]->m_Visible = eCLV_DepthOfFieldEnabled.GetValue();
    sLayerViews[eCLV_LingeringParticles]->m_Visible = eCLV_LingeringParticlesEnabled.GetValue();
    sLayerViews[eCLV_Particles]->m_Visible = eCLV_ParticlesEnabled.GetValue();
    sLayerViews[eCLV_BallChargeAlphaBlended]->m_Visible = eCLV_BallChargeAlphaBlendedEnabled.GetValue();
    sLayerViews[eCLV_CoPlanar]->m_Visible = eCLV_CoPlanarEnabled.GetValue();
    sLayerViews[eCLV_InvisiblePlane]->m_Visible = eCLV_InvisiblePlaneEnabled.GetValue();
    sLayerViews[eCLV_ElectricFence]->m_Visible = eCLV_ElectricFenceEnabled.GetValue();
    sLayerViews[eCLV_PreWarble]->m_Visible = eCLV_PreWarbleEnabled.GetValue();
    sLayerViews[eCLV_Warble]->m_Visible = eCLV_WarbleEnabled.GetValue();
    sLayerViews[eCLV_WarbleBlend]->m_Visible = eCLV_WarbleBlendEnabled.GetValue();
    sLayerViews[eCLV_CameraSpace]->m_Visible = eCLV_CameraSpaceEnabled.GetValue();
    sLayerViews[eCLV_ScreenBlur]->m_Visible = eCLV_ScreenBlurEnabled.GetValue();
    sLayerViews[eCLV_ScreenBlur2]->m_Visible = eCLV_ScreenBlur2Enabled.GetValue();
    sLayerViews[eCLV_ScreenGrab]->m_Visible = eCLV_ScreenGrabEnabled.GetValue();
    sLayerViews[eCLV_FrontEnd]->m_Visible = eCLV_FrontEndEnabled.GetValue();
    sLayerViews[eCLV_UnsortedOrtho640]->m_Visible = eCLV_UnsortedOrtho640Enabled.GetValue();
    sLayerViews[eCLV_UnsortedSquareOrtho]->m_Visible = eCLV_UnsortedSquareOrthoEnabled.GetValue();
    sLayerViews[eCLV_Transitions3D]->m_Visible = eCLV_Transitions3DEnabled.GetValue();
    sLayerViews[eCLV_Transitions]->m_Visible = eCLV_TransitionsEnabled.GetValue();
    sLayerViews[eCLV_Anark3D_BG]->m_Visible = eCLV_Anark3D_BGEnabled.GetValue();
    sLayerViews[eCLV_Anark]->m_Visible = eCLV_AnarkEnabled.GetValue();
    sLayerViews[eCLV_Anark3D_FG]->m_Visible = eCLV_Anark3D_FGEnabled.GetValue();
    sLayerViews[eCLV_HomeButtonFadeOut]->m_Visible = eCLV_HomeButtonFadeOutEnabled.GetValue();
    sLayerViews[eCLV_Debug]->m_Visible = eCLV_DebugEnabled.GetValue();
    sLayerViews[eCLV_DebugSquare]->m_Visible = eCLV_DebugSquareEnabled.GetValue();
}

int GetShadowPartitionCount()
{
    return 11;
}

void SetShadowPartitionEnabled(int partition, bool enabled)
{
    GLView* v = sShadowPartitionViews[partition];
    v->m_Target = enabled ? GLViewTarget_Mode8 : GLViewTarget_None;
}

GLView* GetShadowPartitionView(int partition)
{
    return sShadowPartitionViews[partition];
}

u32 GetShadowPartitionTexture(int partition)
{
    return sShadowPartitionTextures[partition];
}

void SetShadowPartitionCamera(int partition, const nlMatrix4& viewMatrix, const nlMatrix4& projectionMatrix)
{
    sShadowPartitionCameras[partition].Set(viewMatrix, projectionMatrix);
}

void CreateShadowPartitionViews()
{
    GLTargetInfo info;
    int i;

    for (i = 0; i < 11; i++)
    {
        nlZeroMemory(&sShadowPartitionRects[i], sizeof(RLViewRect));
        sShadowPartitionRects[i].width = 0x90;
        sShadowPartitionRects[i].height = 0x90;
        sShadowPartitionRects[i].x = (i % 4) * 0x90;
        sShadowPartitionRects[i].y = (i / 4) * 0x90;
    }

    for (i = 0; i < 11; i++)
    {
        nlSNPrintf(sShadowPartitionNames[i], sizeof(sShadowPartitionNames[i]), "shadow_%02d", i);
    }

    for (i = 0; i < 11; i++)
    {
        nlZeroMemory(&info, sizeof(GLTargetInfo));
        info.width = 0x48;
        info.height = 0x48;
        info.format = GLTargetFormat_6;
        info.unknown18 = 0;
        info.unknown1C = 0;
        GLRenderPair pair = glCreateTarget(sShadowPartitionNames[i], &info);
        sShadowPartitionPairs[i] = pair;
        sShadowPartitionTextures[i] = pair.hash;
    }

    for (i = 0; i < 11; i++)
    {
        RLView* newView = new (8, false) RLView(&sShadowPartitionCameras[i], sShadowPartitionPairs[i], GLViewSort_None);
        newView->m_Name = sShadowPartitionNames[i];
        newView->m_ViewportX = sShadowPartitionRects[i].x;
        newView->m_ViewportY = sShadowPartitionRects[i].y;
        newView->m_ViewportWidth = sShadowPartitionRects[i].width;
        newView->m_ViewportHeight = sShadowPartitionRects[i].height;
        newView->m_ClearColour = false;
        newView->m_ClearDepth = false;
        newView->m_Target = 0;
        sShadowPartitionViews[i] = newView;
    }

    for (i = 0; i < 11; i++)
    {
        GetShadowPartitionView(i)->m_Enabled = true;
        GetShadowPartitionView(i)->SetParent(sLayerViews[eCLV_ShadowTexture]);
    }

    sLayerViews[eCLV_ShadowTexture]->m_ClearColour = true;
    sLayerViews[eCLV_ShadowTexture]->m_ClearDepth = true;
}

static RLViewLayerDesc sPerspectiveLayers[] = {
    { eCLV_Shadowed, "eCLV_Shadowed", true, GLViewSort_Texture },
    { eCLV_NoFog, "eCLV_NoFog", true, GLViewSort_Texture },
    { eCLV_ShadowVolume, "eCLV_ShadowVolume", true, GLViewSort_None },
    { eCLV_WorldShadowed, "eCLV_WorldShadowed", true, GLViewSort_Texture },
    { eCLV_Characters, "eCLV_Characters", true, GLViewSort_Texture },
    { eCLV_PeachPhoto3D, "eCLV_PeachPhoto3D", true, GLViewSort_None },
    { eCLV_MoreCharacters, "eCLV_MoreCharacters", true, GLViewSort_Texture },
    { eCLV_ImpostorOut, "eCLV_ImpostorOut", true, GLViewSort_Texture },
    { eCLV_Unshadowed, "eCLV_Unshadowed", true, GLViewSort_Texture },
    { eCLV_WorldAlphaBlended, "eCLV_WorldAlphaBlended", true, GLViewSort_Texture },
    { eCLV_HighRange3DNoFog, "eCLV_HighRange3DNoFog", true, GLViewSort_Texture },
    { eCLV_HighRange3D, "eCLV_HighRange3D", true, GLViewSort_Texture },
    { eCLV_Warble, "eCLV_Warble", true, GLViewSort_Texture },
    { eCLV_LingeringParticles, "eCLV_LingeringParticles", true, GLViewSort_Texture },
    { eCLV_Particles, "eCLV_Particles", true, GLViewSort_Texture },
    { eCLV_BallChargeAlphaBlended, "eCLV_BallChargeAlphaBlended", true, GLViewSort_Texture },
    { eCLV_UnsortedPerspective, "eCLV_UnsortedPerspective", true, GLViewSort_None },
    { eCLV_InvisiblePlane, "eCLV_InvisiblePlane", true, GLViewSort_Texture },
    { eCLV_ElectricFence, "eCLV_ElectricFence", true, GLViewSort_None },
    { eCLV_CoPlanar, "eCLV_CoPlanar", true, GLViewSort_None },
    { eCLV_PreWarble, "eCLV_PreWarble", true, GLViewSort_None },
    { eCLV_ImpostorTexture, "eCLV_ImpostorTexture", true, GLViewSort_Texture },
    { eCLV_ShadowTexture, "eCLV_ShadowTexture", true, GLViewSort_Texture },
    { eCLV_GrabTexture, "eCLV_GrabTexture", true, GLViewSort_Texture },
    { eCLV_ScreenBlur, "eCLV_ScreenBlur", true, GLViewSort_Texture },
    { eCLV_ScreenBlur2, "eCLV_ScreenBlur2", true, GLViewSort_Texture },
    { eCLV_ScreenGrab, "eCLV_ScreenGrab", true, GLViewSort_Texture },
};

static RLViewLayerDesc sPipLayers[] = {
    { eCLV_PictureInPicture, "eCLV_PictureInPicture", true, GLViewSort_Texture },
    { eCLV_PictureInPictureAlpha, "eCLV_PictureInPictureAlpha", true, GLViewSort_Texture },
};

static RLViewLayerDesc sOrthoLayers[] = {
    { eCLV_ShadowVolumeBlend, "eCLV_ShadowVolumeBlend", true, GLViewSort_None },
    { eCLV_FrontEnd, "eCLV_FrontEnd", true, GLViewSort_TransformedMatrixDepth },
    { eCLV_Transitions, "eCLV_Transitions", true, GLViewSort_None },
    { eCLV_WarbleBlend, "eCLV_WarbleBlend", true, GLViewSort_None },
    { eCLV_DepthOfField, "eCLV_DepthOfField", true, GLViewSort_Texture },
    { eCLV_MegastrikeBackground, "eCLV_MegastrikeBackground", true, GLViewSort_TransformedMatrixDepth },
    { eCLV_BigBlackPolygon, "eCLV_BigBlackPolygon", true, GLViewSort_TransformedMatrixDepth },
    { eCLV_HighRangeChain, "eCLV_HighRangeChain", true, GLViewSort_None },
    { eCLV_HighRange2D, "eCLV_HighRange2D", true, GLViewSort_None },
    { eCLV_UnsortedOrtho640, "eCLV_UnsortedOrtho640", true, GLViewSort_None },
    { eCLV_HomeButtonFadeOut, "eCLV_HomeButtonFadeOut", true, GLViewSort_None },
    { eCLV_Debug, "eCLV_Debug", true, GLViewSort_None },
};

static RLViewLayerDesc sSquareOrthoLayers[] = {
    { eCLV_UnsortedSquareOrtho, "eCLV_UnsortedSquareOrtho", true, GLViewSort_None },
    { eCLV_DebugSquare, "eCLV_DebugSquare", true, GLViewSort_None },
};

static RLViewLayerDesc sAnarkLayers[] = {
    { eCLV_Anark, "eCLV_Anark", true, GLViewSort_Reverse },
};

static RLViewLayerDesc sCameraSpaceLayers[] = {
    { eCLV_CameraSpace, "eCLV_CameraSpace", true, GLViewSort_Texture },
};

static RLViewLayerDesc sAnark3DLayers[] = {
    { eCLV_Anark3D_BG, "eCLV_Anark3D_BG", true, GLViewSort_Texture },
    { eCLV_Anark3D_FG, "eCLV_Anark3D_FG", true, GLViewSort_Texture },
};

static RLViewLayerDesc sTransitions3DLayers[] = {
    { eCLV_Transitions3D, "eCLV_Transitions3D", true, GLViewSort_Texture },
};

extern "C" RLView* fn_8027261C()
{
    return sLayerViews[eCLV_Shadowed];
}

RLView* GetUnshadowedView()
{
    return sLayerViews[eCLV_Unshadowed];
}

glModelStream* glModelPacketGetStream(const glModelPacket* packet, int id)
{
    for (unsigned int i = 0; i < packet->numStreams; i++)
    {
        glModelStream* stream = packet->streams + i;
        if (id == stream->id)
        {
            return stream;
        }
    }
    return 0;
}

RLView* GetLayerView(eCLV layer)
{
    if (layer < eCLV_Num)
    {
        return sLayerViews[layer];
    }
    return 0;
}

GLViewInterface* GetOrthoCamera()
{
    return &sOrthoCamera;
}

void CreateWarbleTargets()
{
    GLTargetInfo info;
    info.width = 0x40;
    info.height = 0x40;
    info.format = GLTargetFormat_7;
    info.unknown18 = 0;
    info.unknown1C = 0;
    info.colour[0] = 0x7C;
    info.colour[1] = 0x7C;
    info.colour[2] = 0x7C;
    info.colour[3] = 0x7C;
    GLRenderPair texturePair = glCreateTarget("warbletexture", &info);
    sWarbleTextureTarget = texturePair;

    info.width = glplatGetDefaultTargetWidth();
    info.height = glplatGetDefaultTargetHeight();
    info.format = GLTargetFormat_1;
    info.unknown18 = 0;
    info.unknown1C = 0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    GLRenderPair colourPair = glCreateTarget("warblecolour", &info);
    sWarbleColourTarget = colourPair;
    sLayerViews[eCLV_PreWarble]->SetRenderPair(sWarbleColourTarget);
    sLayerViews[eCLV_PreWarble]->m_Target = 8;

    unsigned long warbleWidth = glplatGetDefaultTargetWidth() >> 1;
    unsigned long warbleHeight = glplatGetDefaultTargetHeight() >> 1;
    sLayerViews[eCLV_Warble]->SetViewport(0, 0, warbleWidth, warbleHeight);

    info.width = glplatGetDefaultTargetWidth() >> 2;
    info.height = glplatGetDefaultTargetHeight() >> 2;
    info.format = GLTargetFormat_5;
    info.unknown18 = 1;
    info.colour[0] = 0x80;
    info.colour[1] = 0x80;
    info.colour[2] = 0x80;
    info.colour[3] = 0x80;
    GLRenderPair offsetPair = glCreateTarget("warbleoffset", &info);
    sWarbleOffsetTarget = offsetPair;
    sLayerViews[eCLV_Warble]->SetRenderPair(sWarbleOffsetTarget);
    sLayerViews[eCLV_Warble]->m_ClearColour = true;
    sLayerViews[eCLV_Warble]->m_Target = 8;
}

void CreateRenderTargets()
{
    GLTargetInfo info;
    nlZeroMemory(&info, sizeof(GLTargetInfo));
    info.width = glplatGetDefaultTargetWidth() >> 1;
    info.height = glplatGetDefaultTargetHeight() >> 1;
    info.format = GLTargetFormat_1;
    info.unknown18 = 1;
    info.unknown1C = 0;
    info.colour[0] = 0xFF;
    info.colour[1] = 0xFF;
    info.colour[2] = 0;
    info.colour[3] = 0;
    GLRenderPair dofPair = glCreateTarget(sDofTargetName, &info);
    sDofTarget = dofPair;
    sLayerViews[eCLV_UnsortedPerspective]->SetRenderPair(sDofTarget);
    sLayerViews[eCLV_UnsortedPerspective]->m_ClearColour = false;
    sLayerViews[eCLV_UnsortedPerspective]->m_ClearDepth = false;

    info.unknown18 = 0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    GLRenderPair grabPair = glCreateTarget("screengrab", &info);
    sScreenGrabTarget = grabPair;
    sLayerViews[eCLV_ScreenGrab]->SetRenderPair(sScreenGrabTarget);
    sLayerViews[eCLV_ScreenGrab]->m_ClearColour = false;
    sLayerViews[eCLV_ScreenGrab]->m_ClearDepth = false;

    nlZeroMemory(&info, sizeof(GLTargetInfo));
    info.width = 0x100;
    info.height = 0x80;
    info.format = GLTargetFormat_1;
    info.unknown18 = 1;
    info.unknown1C = 0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0xFF;
    sLayerViews[eCLV_PictureInPicture]->m_Target = 0;
    sLayerViews[eCLV_PictureInPicture]->m_ClearDepth = true;
    sLayerViews[eCLV_PictureInPicture]->m_ClearColour = true;
    sLayerViews[eCLV_PictureInPicture]->SetViewport(0, 0, 0x200, 0x100);
    GLRenderPair pipPair = glCreateTarget(sPipTargetName, &info);
    sLayerViews[eCLV_PictureInPictureAlpha]->SetRenderPair(pipPair);
    sLayerViews[eCLV_PictureInPictureAlpha]->m_Target = 9;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ClearDepth = false;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ClearColour = false;
    sLayerViews[eCLV_PictureInPictureAlpha]->SetViewport(0, 0, 0x200, 0x100);

    info.width = glplatGetDefaultTargetWidth() >> 1;
    info.height = glplatGetDefaultTargetHeight() >> 1;
    info.format = GLTargetFormat_7;
    info.unknown18 = 0;
    GLRenderPair greyPair = glCreateTarget("grayscale", &info);
    sLayerViews[eCLV_Characters]->SetRenderPair(greyPair);
    sLayerViews[eCLV_Characters]->m_ClearDepth = false;
    sLayerViews[eCLV_Characters]->m_ClearColour = false;

    CreateWarbleTargets();
}

static void CreateLayerViews(
    GLViewInterface* camera, const RLViewLayerDesc* layers, int count)
{
    GLRenderPair display = glGetBackBufferTarget();
    GLRenderPair blank;
    int i;

    for (i = 0; i < count; i++)
    {
        RLView* view = new (8, false) RLView(camera, layers[i].useDisplayTarget ? display : blank, layers[i].sortMode);
        sLayerViews[layers[i].layer] = view;
        sLayerViews[layers[i].layer]->m_Name = layers[i].name;
        sLayerViews[layers[i].layer]->m_Unknown48 = layers[i].layer;
    }
}

void SetupViews()
{
    int i;

    CreateLayerViews(&sPerspectiveCamera, sPerspectiveLayers, 27);

    CreateLayerViews(&sPipCamera, sPipLayers, 2);

    CreateLayerViews(&sOrthoCamera, sOrthoLayers, 12);

    CreateLayerViews(&sOrtho640Camera, sSquareOrthoLayers, 2);

    CreateLayerViews(&sOrthoCenteredCamera, sAnarkLayers, 1);

    CreateLayerViews(&sCameraSpaceCamera, sCameraSpaceLayers, 1);

    CreateLayerViews(&sTransitions3DCamera, sTransitions3DLayers, 1);

    CreateLayerViews(&sAnark3DCamera, sAnark3DLayers, 2);

    sLayerViews[eCLV_NoFog]->mFogEnabled = false;
    sLayerViews[eCLV_HighRange3DNoFog]->mFogEnabled = false;
    sLayerViews[eCLV_DepthOfField]->mFogEnabled = false;
    sLayerViews[eCLV_BigBlackPolygon]->mFogEnabled = false;
    sLayerViews[eCLV_MegastrikeBackground]->mFogEnabled = false;
    sLayerViews[eCLV_CoPlanar]->mCoPlanarEnabled = true;

    CreateRenderTargets();

    for (i = 0; i < eCLV_Num; i++)
    {
        sLayerViews[i]->m_Enabled = false;
        sLayerViews[i]->SetParent(&gRootView);
    }

    sLayerViews[eCLV_Warble]->m_Enabled = true;

    CreateShadowPartitionViews();
    CreateShadowVolumeTarget();
    SetShadowVolumeTarget(sLayerViews[eCLV_ShadowVolume], sLayerViews[eCLV_ShadowVolume]);

    GXColor colour = sShadowVolumeColour;
    CopyShadowVolumeColour(&colour);

    sLayerViews[eCLV_ImpostorTexture]->m_ClearColour = true;
    sLayerViews[eCLV_ImpostorTexture]->m_ClearDepth = true;
    sLayerViews[eCLV_ImpostorTexture]->m_Enabled = true;
    sLayerViews[eCLV_HighRange3D]->m_Enabled = true;
    sLayerViews[eCLV_HighRange3DNoFog]->m_Enabled = true;

    InitializeHighRange(&gHighRange);
}

float fn_8027313C()
{
    return 0.25f;
}

void fn_80273144(const nlMatrix4& view, const nlMatrix4& pipView, float aspect, float fov, float pipAspect, float pipFov)
{
    nlMatrix4 projection;
    glMatrixPerspective(projection, fov, aspect, 0.25f, 4096.0f);

    nlMatrix4 rotation = view;
    rotation.m41 = 0.0f;
    rotation.m42 = 0.0f;
    rotation.m43 = 0.0f;
    rotation.m44 = 1.0f;
    sNoTranslationCamera.Set(rotation, projection);
    sPerspectiveCamera.Set(view, projection);

    nlMatrix4 pipProjection;
    glMatrixPerspective(pipProjection, pipFov, pipAspect, 0.25f, 4096.0f);
    sPipCamera.Set(pipView, pipProjection);

    sCameraSpaceCamera.mProjection = projection;
    sCameraSpaceCamera.mView.SetIdentity();

    glMatrixOrthographic(sOrthoCamera.mMatrix, 640.0f, 480.0f);

    glMatrixOrthographic(sOrtho640Camera.mMatrix, (float)(sWidescreen ? 854 : 640), 480.0f);
    glMatrixOrthographicCentered(sOrthoCenteredCamera.mMatrix, (float)(sWidescreen ? 854 : 640), 480.0f, 0.0f, 5000.0f);

    glMatrixPerspective(sAnark3DCamera.mProjection, 0.4712389f, aspect, 0.25f, 4096.0f);
    sTransitions3DCamera.mProjection = sAnark3DCamera.mProjection;

    nlMatrix4 lookAt;
    nlVector3 eye = { 0.0f, 0.0f, 0.0f };
    nlVector3 at = { 0.0f, 0.0f, -1.0f };
    nlVector3 up = { 0.0f, 1.0f, 0.0f };
    glMatrixLookAt(lookAt, eye, at, up);
    sTransitions3DCamera.mView = lookAt;

    eye.x = 0.0f;
    eye.y = 12.0f;
    eye.z = 0.0f;
    up.x = 0.0f;
    up.y = 0.0f;
    up.z = 1.0f;
    glMatrixLookAt(lookAt, eye, at, up);
    sAnark3DCamera.mView = lookAt;

    glx_SetFogClipPlanes(0.25f, 4096.0f);
    fn_80271DE0();
}

void fn_80273A14(eCLV layer)
{
    sLayerViews[layer]->m_Visible = true;
}

void fn_80273A30(eCLV layer)
{
    sLayerViews[layer]->m_Visible = false;
}

extern "C" void fn_80273A4C(eCLV layer, const glModel* model, unsigned long key)
{
    RLView* view;
    if (layer < eCLV_Num)
    {
        view = sLayerViews[layer];
    }
    else
    {
        view = 0;
    }

    for (unsigned long i = 0; i < model->numPackets; i++)
    {
        glModelPacket* packet = &model->packets[i];
        if (glGetRasterState(packet->rasterState, GLS_AlphaBlend) == 0)
        {
            view->AttachPacket(packet, key);
        }
        else
        {
            view->AttachPacket(packet, key + 1);
        }
    }
}

void rlSetWidescreen(bool widescreen)
{
    sWidescreen = widescreen;
}

bool IsWidescreen()
{
    return sWidescreen;
}

const nlVector4* RLViewCamera::GetShadowMatrix() const
{
    if (mShadowDirty)
    {
        ExtractFrustumPlanes(mShadowPlanes, mProjection, mView);
        mShadowDirty = false;
    }
    return mShadowPlanes;
}

void RLViewCamera::GetViewProjectionMatrix(nlMatrix4& matrix) const
{
    matrix = mViewProjection;
}

void RLViewCamera::GetInverseViewMatrix(nlMatrix4& matrix) const
{
    matrix = mViewInverse;
}

void RLViewCamera::GetProjectionMatrix(nlMatrix4& matrix) const
{
    matrix = mProjection;
}

const nlMatrix4* RLViewCamera::GetProjectionMatrix() const
{
    return &mProjection;
}

void RLViewCamera::GetViewMatrix(nlMatrix4& matrix) const
{
    matrix = mView;
}

const nlMatrix4* RLViewCamera::GetViewMatrix() const
{
    return &mView;
}

RLViewCamera::RLViewCamera()
{
    mView.SetIdentity();
    mShadowDirty = true;
}

inline const nlMatrix4* RLViewOrthoCamera::GetViewMatrix() const
{
    return &sIdentityMatrix;
}

inline void RLViewOrthoCamera::GetViewMatrix(nlMatrix4& matrix) const
{
    matrix.SetIdentity();
}

inline const nlMatrix4* RLViewOrthoCamera::GetProjectionMatrix() const
{
    return &mMatrix;
}

inline void RLViewOrthoCamera::GetProjectionMatrix(nlMatrix4& matrix) const
{
    matrix = mMatrix;
}

inline void RLViewOrthoCamera::GetInverseViewMatrix(nlMatrix4& matrix) const
{
    matrix.SetIdentity();
}

inline void RLViewOrthoCamera::GetViewProjectionMatrix(nlMatrix4& matrix) const
{
    matrix = mMatrix;
}
