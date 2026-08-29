#ifndef GAME_RENDER_RL_VIEW_LAYERS_H
#define GAME_RENDER_RL_VIEW_LAYERS_H

#include "Game/Render/RLView.h"
#include "Game/TweakValue.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "types.h"

class RLViewCamera : public GLViewInterface
{
public:
    RLViewCamera();

    virtual void GetViewMatrix(nlMatrix4&);
    virtual void GetProjectionMatrix(nlMatrix4&);
    virtual void GetInverseViewMatrix(nlMatrix4&);
    virtual void GetViewProjectionMatrix(nlMatrix4&);
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;
    virtual const nlMatrix4* GetShadowMatrix();

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
    /* 0x104 */ nlMatrix4 mShadowMatrix;
    /* 0x144 */ u8 mUnidentified144[0x20];
    /* 0x164 */ bool mShadowDirty;
}; // total size: 0x168

class RLViewOrthoCamera : public GLViewInterface
{
public:
    virtual void GetViewMatrix(nlMatrix4&);
    virtual void GetProjectionMatrix(nlMatrix4&);
    virtual void GetInverseViewMatrix(nlMatrix4&);
    virtual void GetViewProjectionMatrix(nlMatrix4&);
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

extern TweakValueBool_804F4578 eCLV_ImpostorTextureEnabled;
extern TweakValueBool_804F4578 eCLV_ShadowTextureEnabled;
extern TweakValueBool_804F4578 eCLV_GrabTextureEnabled;
extern TweakValueBool_804F4578 eCLV_PictureInPictureEnabled;
extern TweakValueBool_804F4578 eCLV_PictureInPictureAlphaEnabled;
extern TweakValueBool_804F4578 eCLV_NoFogEnabled;
extern TweakValueBool_804F4578 eCLV_ShadowedEnabled;
extern TweakValueBool_804F4578 eCLV_WorldShadowedEnabled;
extern TweakValueBool_804F4578 eCLV_UnshadowedEnabled;
extern TweakValueBool_804F4578 eCLV_MegastrikeBackgroundEnabled;
extern TweakValueBool_804F4578 eCLV_ImpostorOutEnabled;
extern TweakValueBool_804F4578 eCLV_CharactersEnabled;
extern TweakValueBool_804F4578 eCLV_PeachPhoto3DEnabled;
extern TweakValueBool_804F4578 eCLV_MoreCharactersEnabled;
extern TweakValueBool_804F4578 eCLV_WorldAlphaBlendedEnabled;
extern TweakValueBool_804F4578 eCLV_HighRange3DEnabled;
extern TweakValueBool_804F4578 eCLV_HighRange3DNoFogEnabled;
extern TweakValueBool_804F4578 eCLV_HighRangeChainEnabled;
extern TweakValueBool_804F4578 eCLV_HighRange2DEnabled;
extern TweakValueBool_804F4578 eCLV_BigBlackPolygonEnabled;
extern TweakValueBool_804F4578 eCLV_ShadowVolumeEnabled;
extern TweakValueBool_804F4578 eCLV_ShadowVolumeBlendEnabled;
extern TweakValueBool_804F4578 eCLV_UnsortedPerspectiveEnabled;
extern TweakValueBool_804F4578 eCLV_DepthOfFieldEnabled;
extern TweakValueBool_804F4578 eCLV_LingeringParticlesEnabled;
extern TweakValueBool_804F4578 eCLV_ParticlesEnabled;
extern TweakValueBool_804F4578 eCLV_BallChargeAlphaBlendedEnabled;
extern TweakValueBool_804F4578 eCLV_CoPlanarEnabled;
extern TweakValueBool_804F4578 eCLV_InvisiblePlaneEnabled;
extern TweakValueBool_804F4578 eCLV_ElectricFenceEnabled;
extern TweakValueBool_804F4578 eCLV_PreWarbleEnabled;
extern TweakValueBool_804F4578 eCLV_WarbleEnabled;
extern TweakValueBool_804F4578 eCLV_WarbleBlendEnabled;
extern TweakValueBool_804F4578 eCLV_CameraSpaceEnabled;
extern TweakValueBool_804F4578 eCLV_ScreenBlurEnabled;
extern TweakValueBool_804F4578 eCLV_ScreenBlur2Enabled;
extern TweakValueBool_804F4578 eCLV_ScreenGrabEnabled;
extern TweakValueBool_804F4578 eCLV_FrontEndEnabled;
extern TweakValueBool_804F4578 eCLV_UnsortedOrtho640Enabled;
extern TweakValueBool_804F4578 eCLV_UnsortedSquareOrthoEnabled;
extern TweakValueBool_804F4578 eCLV_Transitions3DEnabled;
extern TweakValueBool_804F4578 eCLV_TransitionsEnabled;
extern TweakValueBool_804F4578 eCLV_Anark3D_BGEnabled;
extern TweakValueBool_804F4578 eCLV_AnarkEnabled;
extern TweakValueBool_804F4578 eCLV_Anark3D_FGEnabled;
extern TweakValueBool_804F4578 eCLV_HomeButtonFadeOutEnabled;
extern TweakValueBool_804F4578 eCLV_DebugEnabled;
extern TweakValueBool_804F4578 eCLV_DebugSquareEnabled;
extern TweakValueBool_804F4578 eCLV_NumEnabled;

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

extern const nlMatrix4 sIdentityMatrix;

#endif // GAME_RENDER_RL_VIEW_LAYERS_H
