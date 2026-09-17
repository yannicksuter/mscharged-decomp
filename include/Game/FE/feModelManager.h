#ifndef GAME_FE_FE_MODEL_MANAGER_H
#define GAME_FE_FE_MODEL_MANAGER_H

#include "Game/SAnim.h"
#include "Game/Inventory.h"
#include "Game/SHierarchy.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "NL/nlSingleton.h"
#include "types.h"

class FEModelManager;
struct tCharacterTemplateInfo;
class GLResourcePool;
class SkinAnimatedNPC;

enum FEModelType
{
    FE_MODEL_SKINNED = 0,
    FE_MODEL_IMPOSTOR = 1,
};

class FEModel
{
public:
    FEModel(tCharacterTemplateInfo* modelData);
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Initialize() = 0;
    virtual cSAnim* GetCurrentAnimation() = 0;
    virtual bool IsAnimationFinished() = 0;
    virtual ~FEModel();

    static void OnTexturesLoaded(void* data, unsigned long size, void* userData);
    static void OnAlternateTexturesLoaded(void* data, unsigned long size, void* userData);
    static void OnModelLoaded(void* data, unsigned long size, void* userData);
    static void OnHierarchyLoaded(void* data, unsigned long size, void* userData);
    static void OnAnimationsLoaded(void* data, unsigned long size, void* userData);

    bool IsLoadFinished() const
    {
        return (mPendingLoads == 0x10 || mPendingLoads == 0) && !mLoadQueued;
    }

    /* 0x04 */ FEModelType mType;
    /* 0x08 */ cInventory<cSAnim>* mAnimations;
    /* 0x0C */ cInventory<cSHierarchy>* mHierarchies;
    /* 0x10 */ GLResourcePool* mLoader;
    /* 0x14 */ unsigned long mLoaderHandle;
    /* 0x18 */ int mModelID;
    /* 0x1C */ void* mUnidentified1C;
    /* 0x20 */ tCharacterTemplateInfo* mModelData;
    /* 0x24 */ void* mTextureFileData;
    /* 0x28 */ u32 mTextureFileDataSize;
    /* 0x2C */ void* mAlternateTextureFileData;
    /* 0x30 */ u32 mAlternateTextureFileDataSize;
    /* 0x34 */ void* mModelFileData;
    /* 0x38 */ u32 mModelFileDataSize;
    /* 0x3C */ void* mHierarchyFileData;
    /* 0x40 */ u32 mHierarchyFileDataSize;
    /* 0x44 */ void* mAnimationFileData;
    /* 0x48 */ u32 mAnimationFileDataSize;
    /* 0x4C */ int mPendingLoads;
    /* 0x50 */ bool mLoaded;
    /* 0x51 */ bool mSynchronousLoad;
    /* 0x52 */ bool mLoadQueued;
    /* 0x53 */ u8 mPadding53;
}; // size: 0x54

class FEModelHandle
{
public:
    FEModelHandle(FEModelType type, const char* name, tCharacterTemplateInfo* modelData,
        bool unidentified59, void* unidentified4C, void* unidentified50,
        bool unidentified5A);
    ~FEModelHandle() { delete mModel; }

    bool IsAnimationFinished();
    bool IsLoaded() const;
    bool CanDestroy() const
    {
        return mModel->mLoaded || mModel->IsLoadFinished();
    }
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

class FEModelManager : public nlSingleton<FEModelManager>
{
public:
    FEModelManager();
    virtual ~FEModelManager();

    void Update(float dt);
    void Render();
    void FinishLoadModel(FEModelHandle* handle);
    void RegisterObject(void* object);
    void* GetObject(int id);
    FEModelHandle* CreateModel(FEModelType type, const char* name,
        int captain, bool unidentified59, void* unidentified4C,
        void* unidentified50, bool alternate);
    FEModelHandle* CreateModel(FEModelType type, const char* name,
        tCharacterTemplateInfo* modelData, bool unidentified59, void* unidentified4C,
        void* unidentified50, bool alternate);
    void DestroyModel(FEModelHandle* handle);
    void BeginLoadModels();
    FEModelHandle* GetModel(const char* name);
    void ReleaseImpostors();

    /* 0x04 */ nlListContainer<FEModelHandle*> mHandles;
    /* 0x10 */ nlListContainer<void*> mModels;
    /* 0x1C */ nlDLListContainer<FEModelHandle*> mPendingModels;
    /* 0x24 */ nlDLListContainer<FEModelHandle*> mLoadedModels;
    /* 0x2C */ nlDLListContainer<FEModelHandle*> mDanglingModels;
    /* 0x34 */ void* mResource;
}; // size: 0x38

#endif // GAME_FE_FE_MODEL_MANAGER_H
