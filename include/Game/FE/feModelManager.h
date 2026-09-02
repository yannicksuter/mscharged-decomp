#ifndef GAME_FE_FE_MODEL_MANAGER_H
#define GAME_FE_FE_MODEL_MANAGER_H

#include "Game/SAnim.h"
#include "NL/nlMath.h"
#include "NL/nlSingleton.h"
#include "types.h"

class FEModelManager;
class ResourceInterface_802CC094;
class SkinAnimatedNPC;

enum FEModelType
{
    FE_MODEL_SKINNED = 0,
    FE_MODEL_IMPOSTOR = 1,
};

struct FEAnimation_801C0704
{
    /* 0x00 */ u32 mUnidentified00;
    /* 0x04 */ u32 mHash;
};

struct FEAnimationListEntry_801C0704
{
    /* 0x00 */ FEAnimationListEntry_801C0704* mNext;
    /* 0x04 */ FEAnimation_801C0704* mAnimation;
};

struct FEAnimationInventory_801C0704
{
    /* 0x00 */ u32 mUnidentified00;
    /* 0x04 */ FEAnimationListEntry_801C0704* mHead;
    /* 0x08 */ u32 mUnidentified08;
    /* 0x0C */ u32 mUnidentified0C;
    /* 0x10 */ u32 mUnidentified10;
    /* 0x14 */ u32 mUnidentified14;
    /* 0x18 */ u32 mUnidentified18;
}; // size: 0x1C

class FEModel
{
public:
    FEModel(void* modelData);
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Initialize() = 0;
    virtual FEAnimation_801C0704* GetCurrentAnimation() = 0;
    virtual bool IsAnimationFinished() = 0;
    virtual ~FEModel();

    /* 0x04 */ FEModelType mType;
    /* 0x08 */ FEAnimationInventory_801C0704* mAnimations;
    /* 0x0C */ FEAnimationInventory_801C0704* mHierarchies;
    /* 0x10 */ ResourceInterface_802CC094* mLoader;
    /* 0x14 */ unsigned long mLoaderHandle;
    /* 0x18 */ int mState;
    /* 0x1C */ void* mUnidentified1C;
    /* 0x20 */ void* mModelData;
    /* 0x24 */ void* mAnimationData;
    /* 0x28 */ u32 mAnimationDataSize;
    /* 0x2C */ void* mHierarchyData;
    /* 0x30 */ u32 mHierarchyDataSize;
    /* 0x34 */ void* mModelFileData;
    /* 0x38 */ u32 mModelFileDataSize;
    /* 0x3C */ void* mTextureFileData;
    /* 0x40 */ u32 mTextureFileDataSize;
    /* 0x44 */ void* mUnidentified44;
    /* 0x48 */ u32 mUnidentified48;
    /* 0x4C */ u32 mPendingLoads;
    /* 0x50 */ bool mLoaded;
    /* 0x51 */ bool mSynchronousLoad;
    /* 0x52 */ bool mLoadQueued;
    /* 0x53 */ u8 mPadding53;
}; // size: 0x54

class FEModelHandle
{
public:
    FEModelHandle(FEModelType type, const char* name, void* modelData,
        bool unidentified59, void* unidentified4C, void* unidentified50,
        bool unidentified5A);

    bool IsAnimationFinished();
    bool IsLoaded() const;
    void SetTransform(const nlMatrix4& transform);
    void PlayAnimation(const char* name, ePlayMode playMode,
        float blendTime, float speed, bool force);
    void SetAnimationCompleteCallback(void (*callback)(FEModelHandle*));
    void SetPosition(const nlVector3& position);
    void SetDefaultAnimation(const char* animation);
    bool IsPlayingAnimation(const char* animation) const;

    /* 0x00 */ u32 mNameHash;
    /* 0x04 */ FEModel* mModel;
    /* 0x08 */ char mName[64];
    /* 0x48 */ const char* mDefaultAnimation;
    /* 0x4C */ void* mUnidentified4C;
    /* 0x50 */ void* mUnidentified50;
    /* 0x54 */ void (*mAnimationCompleteCallback)(FEModelHandle*);
    /* 0x58 */ bool mEnabled;
    /* 0x59 */ bool mUnidentified59;
    /* 0x5A */ bool mUnidentified5A;
    /* 0x5B */ u8 mPadding5B;
    /* 0x5C */ nlVector3 mPosition;
}; // size: 0x68

struct FEModelHandleListEntry_801C2FB4
{
    /* 0x00 */ FEModelHandleListEntry_801C2FB4* mNext;
    /* 0x04 */ FEModelHandle* mHandle;
};

class FEModelManager : public nlSingleton<FEModelManager>
{
public:
    FEModelManager();
    virtual ~FEModelManager();

    void Update(float dt);
    void Render();

    /* 0x04 */ void* mUnidentified04;
    /* 0x08 */ FEModelHandleListEntry_801C2FB4* mHandlesHead;
    /* 0x0C */ FEModelHandleListEntry_801C2FB4* mHandlesTail;
    /* 0x10 */ void* mUnidentified10;
    /* 0x14 */ void* mModelsHead;
    /* 0x18 */ void* mModelsTail;
    /* 0x1C */ void* mUnidentified1C;
    /* 0x20 */ void* mPendingModels;
    /* 0x24 */ void* mUnidentified24;
    /* 0x28 */ void* mLoadedModels;
    /* 0x2C */ void* mUnidentified2C;
    /* 0x30 */ void* mDanglingModels;
    /* 0x34 */ void* mResource;
}; // size: 0x38

extern "C"
{
    void fn_801C0170(void* data, u32 size, FEModel* model);
    void fn_801C0244(void* data, u32 size, FEModel* model);
    void fn_801C0250(void* data, u32 size, FEModel* model);
    void fn_801C0334(void* data, u32 size, FEModel* model);
    void fn_801C0418(void* data, u32 size, FEModel* model);

    void fn_801C204C(FEModelManager* manager);
    void fn_801C271C(FEModelManager* manager, void* model);
    void* fn_801C2798(FEModelManager* manager, int id);
    FEModelHandle* fn_801C27C4(FEModelManager* manager, FEModelType type,
        const char* name, int captain, bool unidentified59,
        void* unidentified4C, void* unidentified50, bool alternate);
    FEModelHandle* fn_801C2844(FEModelManager* manager, FEModelType type,
        const char* name, void* modelData, bool unidentified59,
        void* unidentified4C, void* unidentified50, bool alternate);
    void fn_801C2BD8(FEModelManager* manager, FEModelHandle* handle);
    void fn_801C2E10(FEModelManager* manager);
    FEModelHandle* fn_801C2FB4(
        FEModelManager* manager, const char* name);
    void fn_801C3014(FEModelManager* manager);
}

#endif // GAME_FE_FE_MODEL_MANAGER_H
