#ifndef GAME_RENDER_RL_VIEW_LAYERS_H
#define GAME_RENDER_RL_VIEW_LAYERS_H

#include "Game/Render/RLView.h"
#include "Game/TweakValue.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "types.h"

bool IsWidescreen();

class RLViewCamera : public GLViewInterface
{
public:
    RLViewCamera();

    virtual void GetViewMatrix(nlMatrix4&) const;
    virtual void GetProjectionMatrix(nlMatrix4&) const;
    virtual void GetInverseViewMatrix(nlMatrix4&) const;
    virtual void GetViewProjectionMatrix(nlMatrix4&) const;
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;
    virtual const nlVector4* GetShadowMatrix() const;

    void Set(const nlMatrix4& view, const nlMatrix4& projection)
    {
        mView = view;
        mProjection = projection;
        nlInvertMatrix(mViewInverse, mView);
        nlMultMatrices(mViewProjection, mView, mProjection);
        mShadowDirty = true;
    }

    /* 0x004 */ nlMatrix4 mView;
    /* 0x044 */ nlMatrix4 mProjection;
    /* 0x084 */ nlMatrix4 mViewInverse;
    /* 0x0C4 */ nlMatrix4 mViewProjection;
    /* 0x104 */ mutable nlVector4 mShadowPlanes[6];
    /* 0x164 */ mutable bool mShadowDirty;
}; // total size: 0x168

class RLViewOrthoCamera : public GLViewInterface
{
public:
    virtual void GetViewMatrix(nlMatrix4&) const;
    virtual void GetProjectionMatrix(nlMatrix4&) const;
    virtual void GetInverseViewMatrix(nlMatrix4&) const;
    virtual void GetViewProjectionMatrix(nlMatrix4&) const;
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;

    /* 0x04 */ nlMatrix4 mMatrix;
}; // total size: 0x44

struct RLViewRect
{
    /* 0x00 */ u32 x;
    /* 0x04 */ u32 y;
    /* 0x08 */ u32 width;
    /* 0x0C */ u32 height;
};

extern TweakValueBool eCLV_ImpostorTextureEnabled;
extern TweakValueBool eCLV_ShadowTextureEnabled;
extern TweakValueBool eCLV_GrabTextureEnabled;
extern TweakValueBool eCLV_PictureInPictureEnabled;
extern TweakValueBool eCLV_PictureInPictureAlphaEnabled;
extern TweakValueBool eCLV_NoFogEnabled;
extern TweakValueBool eCLV_ShadowedEnabled;
extern TweakValueBool eCLV_WorldShadowedEnabled;
extern TweakValueBool eCLV_UnshadowedEnabled;
extern TweakValueBool eCLV_MegastrikeBackgroundEnabled;
extern TweakValueBool eCLV_ImpostorOutEnabled;
extern TweakValueBool eCLV_CharactersEnabled;
extern TweakValueBool eCLV_PeachPhoto3DEnabled;
extern TweakValueBool eCLV_MoreCharactersEnabled;
extern TweakValueBool eCLV_WorldAlphaBlendedEnabled;
extern TweakValueBool eCLV_HighRange3DEnabled;
extern TweakValueBool eCLV_HighRange3DNoFogEnabled;
extern TweakValueBool eCLV_HighRangeChainEnabled;
extern TweakValueBool eCLV_HighRange2DEnabled;
extern TweakValueBool eCLV_BigBlackPolygonEnabled;
extern TweakValueBool eCLV_ShadowVolumeEnabled;
extern TweakValueBool eCLV_ShadowVolumeBlendEnabled;
extern TweakValueBool eCLV_UnsortedPerspectiveEnabled;
extern TweakValueBool eCLV_DepthOfFieldEnabled;
extern TweakValueBool eCLV_LingeringParticlesEnabled;
extern TweakValueBool eCLV_ParticlesEnabled;
extern TweakValueBool eCLV_BallChargeAlphaBlendedEnabled;
extern TweakValueBool eCLV_CoPlanarEnabled;
extern TweakValueBool eCLV_InvisiblePlaneEnabled;
extern TweakValueBool eCLV_ElectricFenceEnabled;
extern TweakValueBool eCLV_PreWarbleEnabled;
extern TweakValueBool eCLV_WarbleEnabled;
extern TweakValueBool eCLV_WarbleBlendEnabled;
extern TweakValueBool eCLV_CameraSpaceEnabled;
extern TweakValueBool eCLV_ScreenBlurEnabled;
extern TweakValueBool eCLV_ScreenBlur2Enabled;
extern TweakValueBool eCLV_ScreenGrabEnabled;
extern TweakValueBool eCLV_FrontEndEnabled;
extern TweakValueBool eCLV_UnsortedOrtho640Enabled;
extern TweakValueBool eCLV_UnsortedSquareOrthoEnabled;
extern TweakValueBool eCLV_Transitions3DEnabled;
extern TweakValueBool eCLV_TransitionsEnabled;
extern TweakValueBool eCLV_Anark3D_BGEnabled;
extern TweakValueBool eCLV_AnarkEnabled;
extern TweakValueBool eCLV_Anark3D_FGEnabled;
extern TweakValueBool eCLV_HomeButtonFadeOutEnabled;
extern TweakValueBool eCLV_DebugEnabled;
extern TweakValueBool eCLV_DebugSquareEnabled;
extern TweakValueBool eCLV_NumEnabled;

extern GLView* sViews[11];
extern RLViewCamera sShadowDebugCameras[11];
extern RLViewRect sShadowDebugRects[11];
extern char sShadowDebugNames[11][16];
extern u32 sShadowDebugTargets[11];
extern GLRenderPair sShadowDebugPairs[11];
extern RLView* sLayerViews[eCLV_Num];
extern RLViewCamera sPerspectiveCamera;
extern RLViewCamera sPipCamera;
extern RLViewCamera sNoTranslationCamera;
extern RLViewCamera sCameraSpaceCamera;
extern RLViewCamera sTransitions3DCamera;
extern RLViewCamera sAnark3DCamera;
extern RLViewOrthoCamera sOrthoCamera;
extern RLViewOrthoCamera sOrthoCenteredCamera;
extern RLViewOrthoCamera sOrtho640Camera;

extern GLRenderPair sWarbleColourTarget;
extern GLRenderPair sWarbleOffsetTarget;
extern GLRenderPair sWarbleTextureTarget;
extern GLRenderPair sDofTarget;
extern GLRenderPair sScreenGrabTarget;
extern GLRenderPair sTarget_806E1950;
extern bool sWidescreen;

void rlSetWidescreen(bool widescreen);
bool IsWidescreen();

extern const nlMatrix4 sIdentityMatrix;

void fn_80273144(const nlMatrix4& view, const nlMatrix4& pipView, float aspect,
    float fov, float pipAspect, float pipFov);
void fn_80273A30(eCLV layer);

void rlSetWidescreen(bool widescreen);
bool IsWidescreen();

#endif // GAME_RENDER_RL_VIEW_LAYERS_H
