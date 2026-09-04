#include "Game/FE/feModelManager.h"

#include "unclassified/tu_801A4188.h"

#include "Game/Render/RLView.h"

#include "Game/Render/ImpostorCharacter.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/ResourceInterface_802CC094.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/TweakValue.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

class FEModelType0_801C0960 : public FEModel
{
public:
    FEModelType0_801C0960(void* modelData)
        : FEModel(modelData)
        , mModel(0)
    {
    }

    virtual void Update(float dt);
    virtual void Render();
    virtual void Initialize();
    virtual FEAnimation_801C0704* GetCurrentAnimation();
    virtual bool IsAnimationFinished();
    virtual ~FEModelType0_801C0960();

    /* 0x54 */ SkinAnimatedNPC* mModel;
}; // size: 0x58

struct FEImpostorModelState_801C10DC
{
    /* 0x00 */ u8 mUnidentified00[0x44];
    /* 0x44 */ cPN_SAnimController* mAnimationState;
};

class FEModelType1_801C0F58 : public FEModel
{
public:
    FEModelType1_801C0F58(void* modelData)
        : FEModel(modelData)
        , mModel(0)
        , mCharacter(0)
    {
    }

    virtual void Update(float dt);
    virtual void Render();
    virtual void Initialize();
    virtual FEAnimation_801C0704* GetCurrentAnimation();
    virtual bool IsAnimationFinished();
    virtual ~FEModelType1_801C0F58();

    /* 0x54 */ FEImpostorModelState_801C10DC* mModel;
    /* 0x58 */ u32 mUnidentified58;
    /* 0x5C */ ImpostorCharacter* mCharacter;
    /* 0x60 */ nlVector3 mPosition;
    /* 0x6C */ float mTime;
    /* 0x70 */ void* mModels[6];
}; // size: 0x88

class FEImpostorCharacter_801C3100
    : public ImpostorCharacterImpl_8052E9B8
{
public:
    FEImpostorCharacter_801C3100(const char* name,
        ImpostorModel_802DAEE0* model, void* animations, int budget,
        bool animationFlag, bool alternate,
        const ImpostorCharacterParams* params, int modelType);
    virtual ~FEImpostorCharacter_801C3100();

    virtual void SetScale(float scale);
    virtual float GetScale();
    virtual float GetCameraDistance();
    virtual float GetCameraLookatZ();
    virtual void UnidentifiedVirtual20(void* target, int texture);
    virtual void UnidentifiedVirtual24(float dt);

    /* 0x74 */ bool mEnabled;
    /* 0x75 */ u8 mPadding75[3];
    /* 0x78 */ int mModelType;
    /* 0x7C */ TweakValueImpl_804F4DC8 mfScaleInitialCup;
    /* 0x8C */ TweakValueImpl_804F4DC8 mfScaleCup;
    /* 0x9C */ TweakValueImpl_804F4DC8 mfCameraLookatZInitialCup;
    /* 0xAC */ TweakValueImpl_804F4DC8 mfCameraLookatZCup;
    /* 0xBC */ TweakValueImpl_804F4DC8 mfCameraDistanceInitialCup;
    /* 0xCC */ TweakValueImpl_804F4DC8 mfCameraDistanceCup;
}; // size: 0xDC

extern "C"
{
    ResourceInterface_802CC094* fn_802CBFD8(
        const void* configuration, int count, const char* name);
    void fn_802CC02C(ResourceInterface_802CC094* loader);
    void* fn_8002600C(int characterIndex);
    int GetCharacterIndexFromCaptain__Fi(int captain);
}

static const u32 sLoaderConfiguration[4] = { 0, 0x8000, 3, 0x100000 };
static char sDefaultAnimation[] = "fe_idle";

FEModel::FEModel(void* modelData)
    : mType((FEModelType)-1)
    , mAnimations(0)
    , mHierarchies(0)
    , mLoader(0)
    , mLoaderHandle(0)
    , mState(0)
    , mUnidentified1C(0)
    , mModelData(modelData)
    , mAnimationData(0)
    , mAnimationDataSize(0)
    , mHierarchyData(0)
    , mHierarchyDataSize(0)
    , mModelFileData(0)
    , mModelFileDataSize(0)
    , mTextureFileData(0)
    , mTextureFileDataSize(0)
    , mUnidentified44(0)
    , mUnidentified48(0)
    , mPendingLoads(0)
    , mLoaded(false)
    , mSynchronousLoad(false)
    , mLoadQueued(false)
{
    mAnimations = new (8, false) FEAnimationInventory_801C0704;
    if (mAnimations != 0)
    {
        mAnimations->mHead = 0;
        mAnimations->mUnidentified08 = 0;
        mAnimations->mUnidentified10 = 0;
        mAnimations->mUnidentified14 = 0;
        mAnimations->mUnidentified18 = 0;
    }

    mHierarchies = new (8, false) FEAnimationInventory_801C0704;
    if (mHierarchies != 0)
    {
        mHierarchies->mHead = 0;
        mHierarchies->mUnidentified08 = 0;
        mHierarchies->mUnidentified10 = 0;
        mHierarchies->mUnidentified14 = 0;
        mHierarchies->mUnidentified18 = 0;
    }

    mLoader = fn_802CBFD8(sLoaderConfiguration, 2, "FEModelManager");
    mLoaderHandle = mLoader->MarkResource();
}

FEModel::~FEModel()
{
    if (mAnimationData != 0)
    {
        ::operator delete(mAnimationData);
        mAnimationData = 0;
    }
    if (mHierarchyData != 0)
    {
        ::operator delete(mHierarchyData);
        mHierarchyData = 0;
    }
    if (mModelFileData != 0)
    {
        ::operator delete(mModelFileData);
        mModelFileData = 0;
    }
    if (mLoader != 0)
    {
        mLoader->ReleaseResource(mLoaderHandle);
        fn_802CC02C(mLoader);
    }
    delete mAnimations;
    delete mHierarchies;
}

void fn_801C0170(void* data, u32 size, FEModel* model)
{
    model->mAnimationData = data;
    model->mAnimationDataSize = size;
    model->mPendingLoads &= ~8;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
        model->mLoadQueued = false;
    }
}

void fn_801C0244(void* data, u32 size, FEModel* model)
{
    model->mHierarchyData = data;
    model->mHierarchyDataSize = size;
}

void fn_801C0250(void* data, u32 size, FEModel* model)
{
    model->mModelFileData = data;
    model->mModelFileDataSize = size;
    model->mPendingLoads &= ~0x10;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
        model->mLoadQueued = false;
    }
}

void fn_801C0334(void* data, u32 size, FEModel* model)
{
    model->mTextureFileData = data;
    model->mTextureFileDataSize = size;
    model->mPendingLoads &= ~4;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
        model->mLoadQueued = false;
    }
}

void fn_801C0418(void* data, u32 size, FEModel* model)
{
    u32 pendingLoads = model->mPendingLoads & ~1;
    model->mUnidentified44 = data;
    model->mPendingLoads = pendingLoads;
    model->mUnidentified48 = size;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
    }
    model->mLoadQueued = false;
}

FEModelHandle::FEModelHandle(FEModelType type, const char* name,
    void* modelData, bool unidentified59, void* unidentified4C,
    void* unidentified50, bool unidentified5A)
{
    nlStrNCpy(mName, name, 64);
    mNameHash = nlStringLowerHash(name);
    mDefaultAnimation = sDefaultAnimation;

    switch (type)
    {
    case FE_MODEL_SKINNED:
        mModel = new (8, false) FEModelType0_801C0960(modelData);
        break;
    case FE_MODEL_IMPOSTOR:
        mModel = new (8, false) FEModelType1_801C0F58(modelData);
        break;
    }

    mUnidentified4C = unidentified4C;
    mUnidentified50 = unidentified50;
    mEnabled = true;
    mUnidentified59 = unidentified59;
    mAnimationCompleteCallback = 0;
    nlVec3Set(mPosition, 0.0f, 0.0f, 0.0f);
    mUnidentified5A = unidentified5A;
}

bool FEModelHandle::IsAnimationFinished()
{
    return mModel->IsAnimationFinished();
}

bool FEModelHandle::IsLoaded() const
{
    return mModel->mLoaded;
}

void FEModelHandle::SetTransform(const nlMatrix4& transform)
{
    if (mModel->mType == FE_MODEL_SKINNED)
    {
        FEModelType0_801C0960* model
            = (FEModelType0_801C0960*)mModel;
        model->mModel->mWorldMatrix = transform;
    }
    else if (mModel->mType == FE_MODEL_IMPOSTOR)
    {
        FEModelType1_801C0F58* model
            = (FEModelType1_801C0F58*)mModel;
        model->mPosition.x = transform.m41;
        model->mPosition.y = transform.m42;
        model->mPosition.z = transform.m43;
        model->mTime = 0.0f;
    }
}

void FEModelHandle::PlayAnimation(const char* name, ePlayMode playMode,
    float blendTime, float speed, bool force)
{
    FEAnimation_801C0704* animation = 0;
    u32 hash = nlStringHash(name);
    FEAnimationListEntry_801C0704* entry = mModel->mAnimations->mHead;
    while (entry != 0)
    {
        if (entry->mAnimation->mHash == hash)
        {
            animation = entry->mAnimation;
            break;
        }
        entry = entry->mNext;
    }

    if (animation == 0)
    {
        return;
    }

    bool changeAnimation = force || animation != mModel->GetCurrentAnimation();
    if (changeAnimation && mModel->mType == FE_MODEL_SKINNED)
    {
        FEModelType0_801C0960* model
            = (FEModelType0_801C0960*)mModel;
        model->mModel->SetAnimState(
            *(cSAnim*)animation, blendTime, playMode);
    }
    mAnimationCompleteCallback = 0;
}

void FEModelHandle::SetAnimationCompleteCallback(
    void (*callback)(FEModelHandle*))
{
    mAnimationCompleteCallback = callback;
}

void FEModelHandle::SetPosition(const nlVector3& position)
{
    mPosition = position;
}

void FEModelHandle::SetDefaultAnimation(const char* animation)
{
    mDefaultAnimation = animation;
}

bool FEModelHandle::IsPlayingAnimation(const char* name) const
{
    FEAnimation_801C0704* animation = 0;
    u32 hash = nlStringHash(name);
    FEAnimationListEntry_801C0704* entry = mModel->mAnimations->mHead;
    while (entry != 0)
    {
        if (entry->mAnimation->mHash == hash)
        {
            animation = entry->mAnimation;
            break;
        }
        entry = entry->mNext;
    }
    return animation != 0 && animation == mModel->GetCurrentAnimation();
}

FEModelType0_801C0960::~FEModelType0_801C0960()
{
    delete mModel;
}

FEAnimation_801C0704* FEModelType0_801C0960::GetCurrentAnimation()
{
    if (mModel != 0 && mModel->mpAnimController != 0)
    {
        return (FEAnimation_801C0704*)mModel->mpAnimController->m_pSAnim;
    }
    return 0;
}

bool FEModelType0_801C0960::IsAnimationFinished()
{
    if (mModel != 0 && mModel->mpAnimController != 0)
    {
        cPN_SAnimController* state = mModel->mpAnimController;
        return (state->m_ePlayMode == PM_HOLD && state->m_fTime == 1.0f)
            || state->m_bLooped;
    }
    return false;
}

void FEModelType0_801C0960::Update(float dt)
{
    if (mModel != 0)
    {
        mModel->Update(dt);
    }
}

void FEModelType0_801C0960::Render()
{
    if (mModel != 0)
    {
        mModel->Render();
    }
}

FEModelType1_801C0F58::~FEModelType1_801C0F58()
{
    delete mCharacter;
    delete mModel;
}

FEImpostorCharacter_801C3100::~FEImpostorCharacter_801C3100()
{
}

void FEModelType1_801C0F58::Initialize()
{
}

FEAnimation_801C0704* FEModelType1_801C0F58::GetCurrentAnimation()
{
    if (mModel != 0 && mModel->mAnimationState != 0)
    {
        return (FEAnimation_801C0704*)mModel->mAnimationState->m_pSAnim;
    }
    return 0;
}

bool FEModelType1_801C0F58::IsAnimationFinished()
{
    if (mModel != 0 && mModel->mAnimationState != 0)
    {
        cPN_SAnimController* state = mModel->mAnimationState;
        return (state->m_ePlayMode == PM_HOLD && state->m_fTime == 1.0f)
            || state->m_bLooped;
    }
    return false;
}

void FEModelType1_801C0F58::Render()
{
}

FEModelManager::FEModelManager()
    : mHandlesHead(0)
    , mHandlesTail(0)
    , mModelsHead(0)
    , mModelsTail(0)
    , mPendingModels(0)
    , mLoadedModels(0)
    , mDanglingModels(0)
    , mResource(0)
{
}

void FEModelManager::Update(float dt)
{
    FEModelHandleListEntry_801C2FB4* entry = mHandlesHead;
    while (entry != 0)
    {
        FEModelHandle* handle = entry->mHandle;
        if (handle->mModel->mLoaded)
        {
            handle->mModel->Update(dt);
        }
        entry = entry->mNext;
    }
    fn_801C2E10(this);
}

void FEModelManager::Render()
{
    FEModelHandleListEntry_801C2FB4* entry = mHandlesHead;
    while (entry != 0)
    {
        FEModelHandle* handle = entry->mHandle;
        FEModel* model = handle->mModel;
        if (model->mLoaded && handle->mEnabled
            && model->mType == FE_MODEL_SKINNED)
        {
            model->Render();
        }
        entry = entry->mNext;
    }

    UpdateImpostorPositions();
    ImpostorManager* impostorManager = ImpostorManager::GetInstance();
    if (impostorManager->mInitialized)
    {
        ImpostorManager::GetInstance()->SetSpritesInvalid();
        ImpostorManager::GetInstance()->ResetSpriteSlots();
        ImpostorManager::GetInstance()->UpdateSprites();
        ImpostorManager::GetInstance()->Render(GetLayerView(eCLV_ImpostorOut), false);
    }
}

struct FEModelData_801C2798
{
    /* 0x00 */ u8 mUnidentified00[0x60];
    /* 0x60 */ int mUnidentified60;
};

struct FEModelListEntry_801C271C
{
    FEModelListEntry_801C271C* mNext;
    FEModelData_801C2798* mModel;
};

void fn_801C271C(FEModelManager* manager, void* model)
{
    FEModelListEntry_801C271C* entry
        = new (8, false) FEModelListEntry_801C271C;
    if (entry != 0)
    {
        entry->mNext = 0;
        entry->mModel = (FEModelData_801C2798*)model;
    }

    if (&manager->mModelsTail != 0 && manager->mModelsHead == 0)
    {
        manager->mModelsTail = entry;
    }
    entry->mNext = (FEModelListEntry_801C271C*)manager->mModelsHead;
    manager->mModelsHead = entry;
}

void* fn_801C2798(FEModelManager* manager, int id)
{
    FEModelListEntry_801C271C* entry
        = (FEModelListEntry_801C271C*)manager->mModelsHead;
    while (entry != 0)
    {
        FEModelData_801C2798* model = entry->mModel;
        if (id == model->mUnidentified60)
        {
            return model;
        }
        entry = entry->mNext;
    }
    return 0;
}

FEModelHandle* fn_801C27C4(FEModelManager* manager, FEModelType type,
    const char* name, int captain, bool unidentified59,
    void* unidentified4C, void* unidentified50, bool alternate)
{
    int characterIndex = GetCharacterIndexFromCaptain__Fi(captain);
    if (characterIndex != -1)
    {
        return fn_801C2844(manager, type, name,
            fn_8002600C(characterIndex), unidentified59,
            unidentified4C, unidentified50, alternate);
    }
    return 0;
}

FEModelHandle* fn_801C2844(FEModelManager* manager, FEModelType type,
    const char* name, void* modelData, bool unidentified59,
    void* unidentified4C, void* unidentified50, bool alternate)
{
    FEModelHandle* handle = fn_801C2FB4(manager, name);
    if (handle == 0)
    {
        handle = new (8, false) FEModelHandle(type, name, modelData,
            unidentified59, unidentified4C, unidentified50, alternate);
        FEModelHandleListEntry_801C2FB4* entry
            = new (8, false) FEModelHandleListEntry_801C2FB4;
        if (entry != 0)
        {
            entry->mNext = manager->mHandlesHead;
            entry->mHandle = handle;
            manager->mHandlesHead = entry;
            if (manager->mHandlesTail == 0)
            {
                manager->mHandlesTail = entry;
            }
        }
    }
    return handle;
}

FEModelHandle* fn_801C2FB4(FEModelManager* manager, const char* name)
{
    u32 hash = nlStringLowerHash(name);
    FEModelHandleListEntry_801C2FB4* entry = manager->mHandlesHead;
    while (entry != 0)
    {
        if (hash == entry->mHandle->mNameHash)
        {
            return entry->mHandle;
        }
        entry = entry->mNext;
    }
    return 0;
}

FEImpostorCharacter_801C3100::FEImpostorCharacter_801C3100(
    const char* name, ImpostorModel_802DAEE0* model, void* animations,
    int budget, bool animationFlag, bool alternate,
    const ImpostorCharacterParams* params, int modelType)
    : ImpostorCharacterImpl_8052E9B8(
        name, model, animations, budget, 1, 1, params)
    , mEnabled(true)
    , mModelType(modelType)
{
    (void)animationFlag;
    (void)alternate;
    char category[128];
    nlSNPrintf(category, 128,
        "/Render/Impostor/CharacterTweaks/%s", name);

    mfScaleInitialCup.fn_802C4FEC(
        "mfScaleInitialCup", 0.0f, category, true, 0.0f, 3.0f);
    mfCameraLookatZInitialCup.fn_802C4FEC(
        "mfCameraLookatZInitialCup", 0.0f, category, true, -0.5f, 3.0f);
    mfCameraDistanceInitialCup.fn_802C4FEC(
        "mfCameraDistanceInitialCup", 0.0f, category, true, 0.0f, 3.0f);
    mfScaleCup.fn_802C4FEC(
        "mfScaleCup", 0.0f, category, true, 0.0f, 3.0f);
    mfCameraLookatZCup.fn_802C4FEC(
        "mfCameraLookatZCup", 0.0f, category, true, -0.5f, 3.0f);
    mfCameraDistanceCup.fn_802C4FEC(
        "mfCameraDistanceCup", 0.0f, category, true, 0.0f, 3.0f);
}

void FEImpostorCharacter_801C3100::SetScale(float scale)
{
    switch (mModelType)
    {
    case 1:
        mfScaleInitialCup = scale;
        break;
    case 2:
        mfScaleCup = scale;
        break;
    default:
        ImpostorCharacter::SetScale(scale);
        break;
    }
}

float FEImpostorCharacter_801C3100::GetScale()
{
    switch (mModelType)
    {
    case 1:
        return mfScaleInitialCup;
    case 2:
        return mfScaleCup;
    default:
        return ImpostorCharacter::GetScale();
    }
}

float FEImpostorCharacter_801C3100::GetCameraDistance()
{
    switch (mModelType)
    {
    case 1:
        return mfCameraDistanceInitialCup;
    case 2:
        return mfCameraDistanceCup;
    default:
        return ImpostorCharacter::GetCameraDistance();
    }
}

float FEImpostorCharacter_801C3100::GetCameraLookatZ()
{
    switch (mModelType)
    {
    case 1:
        return mfCameraLookatZInitialCup;
    case 2:
        return mfCameraLookatZCup;
    default:
        return ImpostorCharacter::GetCameraLookatZ();
    }
}

extern "C" void fn_801C38D4(void*, void* entry)
{
    ::operator delete(entry);
}

extern "C" void fn_801C38DC(void*, void* entry)
{
    ::operator delete(entry);
}

extern "C" void fn_801C38F4(void*, void* entry)
{
    ::operator delete(entry);
}

void FEImpostorCharacter_801C3100::UnidentifiedVirtual24(float)
{
}

FEModelManager* nlSingleton<FEModelManager>::s_pInstance = 0;
