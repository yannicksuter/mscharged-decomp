#ifndef GAME_RENDER_IMPOSTOR_MODEL_H
#define GAME_RENDER_IMPOSTOR_MODEL_H

#include "Game/SAnim.h"
#include "NL/nlMath.h"
#include "types.h"

class AnimRetarget;
class cPN_SAnimController;
class cPoseAccumulator;
class cPoseNode;
class cSHierarchy;
template <typename T>
class cInventory;
class GLView;
class GLSkinMesh;
class GLResourcePool;
struct glModel;

class ImpostorModel
{
public:
    ImpostorModel(cSHierarchy& hierarchy, unsigned long modelID,
        cInventory<cSAnim>* animations,
        GLResourcePool* resource);
    ImpostorModel(cSHierarchy& hierarchy, unsigned long modelID,
        GLResourcePool* resource);
    virtual ~ImpostorModel();
    virtual void Render(
        GLView* opaqueView, GLView* translucentView);
    virtual void Update(float dt);
    virtual void RenderPose(GLView* opaqueView,
        GLView* translucentView, const cPoseAccumulator& poseAccumulator,
        const nlMatrix4* worldMatrix);

    ImpostorModel* Clone(GLResourcePool* resource) const;
    void UpdateAnimation(float dt);
    void EvaluatePose();
    void SetAnimationTime(float time);
    void PlayAnimation(cSAnim& anim, ePlayMode playMode, const AnimRetarget* retarget);
    void ResolveTextures();

    void SetReplacementTexture(unsigned long texture);
    void PlayAnimation(const char* name, float blendTime, ePlayMode playMode);

    /* 0x04 */ nlMatrix4 mWorldMatrix;
    /* 0x44 */ cPN_SAnimController* mAnimController;
    /* 0x48 */ cPoseAccumulator* mPoseAccumulator;
    /* 0x4C */ GLSkinMesh* mSkinMesh;
    /* 0x50 */ cPoseNode* mPoseTree;
    /* 0x54 */ unsigned long mModelID;
    /* 0x58 */ cSHierarchy* mHierarchy;
    /* 0x5C */ bool mVisible;
    /* 0x5D */ bool mTexturesResolved;
    /* 0x5E */ u8 mPadding05E[2];
    /* 0x60 */ glModel* mLastModel;
    /* 0x64 */ cInventory<cSAnim>* mAnimations;
    /* 0x68 */ unsigned long mOriginalTexture;
    /* 0x6C */ unsigned long mReplacementTexture;
    /* 0x70 */ unsigned long mResolvedTexture;
    /* 0x74 */ void (*mModelCallback)(
        ImpostorModel*, glModel*);
}; // size: 0x78

#endif // GAME_RENDER_IMPOSTOR_MODEL_H
