#include "Game/Render/RLViewLayers.h"

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

TweakValueBool_804F4578 eCLV_ImpostorTextureEnabled("eCLV_ImpostorTextureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ShadowTextureEnabled("eCLV_ShadowTextureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_GrabTextureEnabled("eCLV_GrabTextureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_PictureInPictureEnabled("eCLV_PictureInPictureEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_PictureInPictureAlphaEnabled("eCLV_PictureInPictureAlphaEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_NoFogEnabled("eCLV_NoFogEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ShadowedEnabled("eCLV_ShadowedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_WorldShadowedEnabled("eCLV_WorldShadowedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_UnshadowedEnabled("eCLV_UnshadowedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_MegastrikeBackgroundEnabled("eCLV_MegastrikeBackgroundEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ImpostorOutEnabled("eCLV_ImpostorOutEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_CharactersEnabled("eCLV_CharactersEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_PeachPhoto3DEnabled("eCLV_PeachPhoto3DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_MoreCharactersEnabled("eCLV_MoreCharactersEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_WorldAlphaBlendedEnabled("eCLV_WorldAlphaBlendedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_HighRange3DEnabled("eCLV_HighRange3DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_HighRange3DNoFogEnabled("eCLV_HighRange3DNoFogEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_HighRangeChainEnabled("eCLV_HighRangeChainEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_HighRange2DEnabled("eCLV_HighRange2DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_BigBlackPolygonEnabled("eCLV_BigBlackPolygonEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ShadowVolumeEnabled("eCLV_ShadowVolumeEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ShadowVolumeBlendEnabled("eCLV_ShadowVolumeBlendEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_UnsortedPerspectiveEnabled("eCLV_UnsortedPerspectiveEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_DepthOfFieldEnabled("eCLV_DepthOfFieldEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_LingeringParticlesEnabled("eCLV_LingeringParticlesEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ParticlesEnabled("eCLV_ParticlesEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_BallChargeAlphaBlendedEnabled("eCLV_BallChargeAlphaBlendedEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_CoPlanarEnabled("eCLV_CoPlanarEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_InvisiblePlaneEnabled("eCLV_InvisiblePlaneEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ElectricFenceEnabled("eCLV_ElectricFenceEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_PreWarbleEnabled("eCLV_PreWarbleEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_WarbleEnabled("eCLV_WarbleEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_WarbleBlendEnabled("eCLV_WarbleBlendEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_CameraSpaceEnabled("eCLV_CameraSpaceEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ScreenBlurEnabled("eCLV_ScreenBlurEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ScreenBlur2Enabled("eCLV_ScreenBlur2Enabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_ScreenGrabEnabled("eCLV_ScreenGrabEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_FrontEndEnabled("eCLV_FrontEndEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_UnsortedOrtho640Enabled("eCLV_UnsortedOrtho640Enabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_UnsortedSquareOrthoEnabled("eCLV_UnsortedSquareOrthoEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_Transitions3DEnabled("eCLV_Transitions3DEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_TransitionsEnabled("eCLV_TransitionsEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_Anark3D_BGEnabled("eCLV_Anark3D_BGEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_AnarkEnabled("eCLV_AnarkEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_Anark3D_FGEnabled("eCLV_Anark3D_FGEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_HomeButtonFadeOutEnabled("eCLV_HomeButtonFadeOutEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_DebugEnabled("eCLV_DebugEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_DebugSquareEnabled("eCLV_DebugSquareEnabled", "/Rendering/RLView Toggles", true);
TweakValueBool_804F4578 eCLV_NumEnabled("eCLV_NumEnabled", "/Rendering/RLView Toggles", true);

GLView* sViews[11];
RLViewCamera sShadowDebugCameras[11];
RLViewRect sShadowDebugRects[11];
char sShadowDebugNames[11][16];
u32 sShadowDebugTargets[11];
GLRenderPair sShadowDebugPairs[11];
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

RLViewCamera::RLViewCamera()
{
    mView.SetIdentity();
    mShadowDirty = true;
}

void RLViewOrthoCamera::GetViewProjectionMatrix(nlMatrix4& matrix)
{
    matrix = mMatrix;
}

void RLViewOrthoCamera::GetInverseViewMatrix(nlMatrix4& matrix)
{
    matrix.SetIdentity();
}

void RLViewOrthoCamera::GetProjectionMatrix(nlMatrix4& matrix)
{
    matrix = mMatrix;
}

const nlMatrix4* RLViewOrthoCamera::GetProjectionMatrix() const
{
    return &mMatrix;
}

void RLViewOrthoCamera::GetViewMatrix(nlMatrix4& matrix)
{
    matrix.SetIdentity();
}

const nlMatrix4* RLViewOrthoCamera::GetViewMatrix() const
{
    return &sIdentityMatrix;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
