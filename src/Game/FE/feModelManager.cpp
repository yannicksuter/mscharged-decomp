#include "Game/FE/feModelManager.h"
#include "Game/CharacterTemplate.h"
#include "Game/DB/CharacterInfo.h"

#include "Game/Render/CrowdImpostors.h"

#include "Game/Render/RLView.h"

#include "Game/Render/ImpostorCharacter.h"
#include "Game/Render/ImpostorManager.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/gl.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/TweakValue.h"
#include "NL/gl/glState.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include "Game/UnidentifiedStaticStorage.h"

class FESkinnedModel : public FEModel
{
public:
    FESkinnedModel(tCharacterTemplateInfo* modelData)
        : FEModel(modelData)
        , mModel(0)
    {
    }

    virtual void Update(float dt);
    virtual void Render();
    virtual void Initialize();
    virtual cSAnim* GetCurrentAnimation();
    virtual bool IsAnimationFinished();
    virtual ~FESkinnedModel();

    /* 0x54 */ SkinAnimatedNPC* mModel;
}; // size: 0x58

class FEImpostorCharacter;

class FEImpostorModel : public FEModel
{
public:
    FEImpostorModel(tCharacterTemplateInfo* modelData)
        : FEModel(modelData)
        , mModel(0)
        , mCharacter(0)
    {
    }

    virtual void Update(float dt);
    virtual void Render();
    virtual void Initialize();
    virtual cSAnim* GetCurrentAnimation();
    virtual bool IsAnimationFinished();
    virtual ~FEImpostorModel();

    /* 0x54 */ ImpostorModel* mModel;
    /* 0x58 */ u32 mUnidentified58;
    /* 0x5C */ FEImpostorCharacter* mCharacter;
    /* 0x60 */ nlVector3 mPosition;
    /* 0x6C */ float mTime;
    /* 0x70 */ void* mModels[6];
}; // size: 0x88

class FEImpostorCharacter
    : public AnimatedImpostorCharacter
{
public:
    FEImpostorCharacter(const char* name,
        ImpostorModel* model, void* animations, int budget,
        bool animationFlag, bool alternate,
        const ImpostorCharacterParams* params, int modelType);
    virtual ~FEImpostorCharacter();

    virtual void SetScale(float scale);
    virtual float GetScale();
    virtual float GetCameraDistance();
    virtual float GetCameraLookatZ();
    virtual void Render(GLView* target, int texture);
    virtual void UpdateAnimation(float dt);

    /* 0x74 */ bool mEnabled;
    /* 0x75 */ u8 mPadding75[3];
    /* 0x78 */ int mModelType;
    /* 0x7C */ TweakFloatBinding mfScaleInitialCup;
    /* 0x8C */ TweakFloatBinding mfScaleCup;
    /* 0x9C */ TweakFloatBinding mfCameraLookatZInitialCup;
    /* 0xAC */ TweakFloatBinding mfCameraLookatZCup;
    /* 0xBC */ TweakFloatBinding mfCameraDistanceInitialCup;
    /* 0xCC */ TweakFloatBinding mfCameraDistanceCup;
}; // size: 0xDC


static const GLMemoryRequirement sLoaderConfiguration[2] = {
    { GLM_Header, 0x8000 },
    { GLM_VertexData, 0x100000 },
};
static char sDefaultAnimation[] = "fe_idle";

FEModel::FEModel(tCharacterTemplateInfo* modelData)
    : mType((FEModelType)-1)
    , mAnimations(0)
    , mHierarchies(0)
    , mLoader(0)
    , mLoaderHandle(0)
    , mModelID(0)
    , mUnidentified1C(0)
    , mModelData(modelData)
    , mTextureFileData(0)
    , mTextureFileDataSize(0)
    , mAlternateTextureFileData(0)
    , mAlternateTextureFileDataSize(0)
    , mModelFileData(0)
    , mModelFileDataSize(0)
    , mHierarchyFileData(0)
    , mHierarchyFileDataSize(0)
    , mAnimationFileData(0)
    , mAnimationFileDataSize(0)
    , mPendingLoads(0)
    , mLoaded(false)
    , mSynchronousLoad(false)
    , mLoadQueued(false)
{
    mAnimations = new (8, false) cInventory<cSAnim>;

    mHierarchies = new (8, false) cInventory<cSHierarchy>;

    mLoader = glCreateResourcePool(sLoaderConfiguration, 2, "FEModelManager");
    mLoaderHandle = mLoader->MarkResource();
}

FEModel::~FEModel()
{
    if (mTextureFileData != 0)
    {
        ::operator delete(mTextureFileData);
        mTextureFileData = 0;
    }
    if (mAlternateTextureFileData != 0)
    {
        ::operator delete(mAlternateTextureFileData);
        mAlternateTextureFileData = 0;
    }
    if (mModelFileData != 0)
    {
        ::operator delete(mModelFileData);
        mModelFileData = 0;
    }
    if (mLoader != 0)
    {
        mLoader->ReleaseResource(mLoaderHandle);
        glDestroyResourcePool(mLoader);
    }
    delete mAnimations;
    delete mHierarchies;
}

void FEModel::OnTexturesLoaded(void* data, unsigned long size, void* userData)
{
    FEModel* model = (FEModel*)userData;
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    model->mTextureFileData = data;
    model->mPendingLoads &= ~4;
    model->mTextureFileDataSize = size;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
        model->mLoadQueued = false;
    }
    else
    {
        glBeginLoadModel(model->mModelData->szModelFilename, OnModelLoaded, model, model->mLoader);
        model->mLoadQueued = true;
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void FEModel::OnAlternateTexturesLoaded(void* data, unsigned long size, void* userData)
{
    FEModel* model = (FEModel*)userData;
    model->mAlternateTextureFileData = data;
    model->mAlternateTextureFileDataSize = size;
}

void FEModel::OnModelLoaded(void* data, unsigned long size, void* userData)
{
    FEModel* model = (FEModel*)userData;
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    model->mModelFileData = data;
    model->mPendingLoads &= ~8;
    model->mModelFileDataSize = size;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
        model->mLoadQueued = false;
    }
    else
    {
        nlLoadEntireFileAsync(model->mModelData->szHierarchyFilename, OnHierarchyLoaded, model, 32, AllocateEnd, 0, 0, 0);
        model->mLoadQueued = true;
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void FEModel::OnHierarchyLoaded(void* data, unsigned long size, void* userData)
{
    FEModel* model = (FEModel*)userData;
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    model->mHierarchyFileData = data;
    model->mPendingLoads &= ~2;
    model->mHierarchyFileDataSize = size;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
        model->mLoadQueued = false;
    }
    else
    {
        nlLoadEntireFileAsync(model->mModelData->pUnidentified38, OnAnimationsLoaded, model, 32, AllocateEnd, 0, 0, 0);
        model->mLoadQueued = true;
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void FEModel::OnAnimationsLoaded(void* data, unsigned long size, void* userData)
{
    FEModel* model = (FEModel*)userData;
    u32 pendingLoads = model->mPendingLoads & ~1;
    model->mAnimationFileData = data;
    model->mPendingLoads = pendingLoads;
    model->mAnimationFileDataSize = size;
    if (model->mSynchronousLoad)
    {
        model->mPendingLoads = 0x10;
    }
    model->mLoadQueued = false;
}

FEModelHandle::FEModelHandle(FEModelType type, const char* name,
    tCharacterTemplateInfo* modelData, bool unidentified59, void* unidentified4C,
    void* unidentified50, bool unidentified5A)
{
    nlStrNCpy(mName, name, 64);
    mNameHash = nlStringLowerHash(name);
    mDefaultAnimation = sDefaultAnimation;

    switch (type)
    {
    case FE_MODEL_SKINNED:
        mModel = new (8, false) FESkinnedModel(modelData);
        break;
    case FE_MODEL_IMPOSTOR:
        mModel = new (8, false) FEImpostorModel(modelData);
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

void FEModelManager::Update(float dt)
{
    nlDLListIterator<FEModelHandle*> danglingModels = mDanglingModels.Begin();
    while (danglingModels.hasNext())
    {
        FEModelHandle* handle = *danglingModels;
        if (!handle->mModel->mLoadQueued)
        {
            delete handle;
            mDanglingModels.Remove(&danglingModels);
        }
        else
        {
            danglingModels.Step();
        }
    }

    BeginLoadModels();

    nlDLListIterator<FEModelHandle*> pendingModels = mPendingModels.Begin();
    while (pendingModels.hasNext())
    {
        FEModelHandle* handle = *pendingModels;
        if (handle->CanDestroy())
        {
            delete handle;
            mPendingModels.Remove(&pendingModels);
        }
        else
        {
            pendingModels.Step();
        }
    }

    ListEntry<FEModelHandle*>* entry = mHandles.m_Head;
    while (entry != 0)
    {
        if (!entry->entry->mModel->mLoaded)
        {
            if (entry->entry->mModel->mPendingLoads == 0)
            {
                FinishLoadModel(entry->entry);
                if (entry->entry->mModel->GetCurrentAnimation() == 0)
                {
                    entry->entry->PlayAnimation(entry->entry->mDefaultAnimation,
                        PM_CYCLIC, 0.2f, 0.0f, false);
                }
            }
            else
            {
                entry = entry->next;
                continue;
            }
        }

        entry->entry->mModel->Update(dt);
        if (entry->entry->mModel->IsAnimationFinished())
        {
            FEModelHandle* handle = entry->entry;
            if (handle->mAnimationCompleteCallback != 0)
            {
                handle->mAnimationCompleteCallback(handle);
            }
            handle->mAnimationCompleteCallback = 0;
        }
        entry = entry->next;
    }

    ImpostorManager::GetInstance()->UpdateAnimations(dt);
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
        FESkinnedModel* model
            = (FESkinnedModel*)mModel;
        model->mModel->mWorldMatrix = transform;
    }
    else if (mModel->mType == FE_MODEL_IMPOSTOR)
    {
        FEImpostorModel* model
            = (FEImpostorModel*)mModel;
        model->mPosition.x = transform.m41;
        model->mPosition.y = transform.m42;
        model->mPosition.z = transform.m43;
        model->mTime = 0.0f;
    }
}

void FEModelHandle::PlayAnimation(const char* name, ePlayMode playMode,
    float blendTime, float speed, bool force)
{
    cInventory<cSAnim>* animations = mModel->mAnimations;
    cSAnim* animation = animations->Find(nlStringHash(name));

    if (animation == 0)
    {
        return;
    }

    bool changeAnimation = false;
    if (force)
    {
        changeAnimation = true;
    }
    else if (animation != mModel->GetCurrentAnimation())
    {
        changeAnimation = true;
    }

    if (changeAnimation)
    {
        switch (mModel->mType)
        {
        case FE_MODEL_SKINNED:
        {
            ((FESkinnedModel*)mModel)->mModel->SetAnimState(
                *animation, blendTime, playMode);
            ((FESkinnedModel*)mModel)->mModel->mpAnimController->m_bMirror
                = mUnidentified59;
            break;
        }
        case FE_MODEL_IMPOSTOR:
        {
            ((FEImpostorModel*)mModel)->mModel->PlayAnimation(
                name, blendTime, playMode);
            ((FEImpostorModel*)mModel)->mModel->mAnimController->m_bMirror
                = mUnidentified59;
            ((FEImpostorModel*)mModel)->mModel->mAnimController->SetTime(speed);
            break;
        }
        }
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
    cInventory<cSAnim>* animations = mModel->mAnimations;
    cSAnim* animation = animations->Find(nlStringHash(name));
    if (animation != 0 && animation == mModel->GetCurrentAnimation())
    {
        return true;
    }
    return false;
}

FESkinnedModel::~FESkinnedModel()
{
    delete mModel;
}

cSAnim* FESkinnedModel::GetCurrentAnimation()
{
    if (mModel != 0 && mModel->mpAnimController != 0)
    {
        return mModel->mpAnimController->m_pSAnim;
    }
    return 0;
}

bool FESkinnedModel::IsAnimationFinished()
{
    if (mModel != 0 && mModel->mpAnimController != 0)
    {
        cPN_SAnimController* state = mModel->mpAnimController;
        return (state->m_ePlayMode == PM_HOLD && state->m_fTime == 1.0f)
            || state->m_bLooped;
    }
    return false;
}

void FESkinnedModel::Update(float dt)
{
    if (mModel != 0)
    {
        mModel->Update(dt);
    }
}

void FESkinnedModel::Render()
{
    if (mModel != 0)
    {
        mModel->Render();
    }
}

FEImpostorModel::~FEImpostorModel()
{
    delete mCharacter;
    delete mModel;
}

FEImpostorCharacter::~FEImpostorCharacter()
{
}

void FEImpostorModel::Initialize()
{
}

cSAnim* FEImpostorModel::GetCurrentAnimation()
{
    if (mModel != 0 && mModel->mAnimController != 0)
    {
        return mModel->mAnimController->m_pSAnim;
    }
    return 0;
}

bool FEImpostorModel::IsAnimationFinished()
{
    if (mModel != 0 && mModel->mAnimController != 0)
    {
        cPN_SAnimController* state = mModel->mAnimController;
        return (state->m_ePlayMode == PM_HOLD && state->m_fTime == 1.0f)
            || state->m_bLooped;
    }
    return false;
}

void FEImpostorModel::Render()
{
}

FEModelManager::FEModelManager()
    : mResource(0)
{
}

void FEModelManager::Render()
{
    nlListIterator<FEModelHandle*> iterator = mHandles.Begin();
    while (iterator.IsValid())
    {
        FEModelHandle* handle = iterator.Current();
        FEModel* model = handle->mModel;
        if (model->mLoaded && handle->mEnabled
            && model->mType == FE_MODEL_SKINNED)
        {
            model->Render();
        }
        iterator.Next();
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

struct FEModelObject
{
    /* 0x00 */ u8 mUnidentified00[0x60];
    /* 0x60 */ int mUnidentified60;
};

void FEModelManager::RegisterObject(void* model)
{
    mModels.AddStart(model);
}

void* FEModelManager::GetObject(int id)
{
    nlListIterator<void*> iterator = mModels.Begin();
    while (iterator.IsValid())
    {
        FEModelObject* model = (FEModelObject*)iterator.Current();
        if (id == model->mUnidentified60)
        {
            return model;
        }
        iterator.Next();
    }
    return 0;
}

FEModelHandle* FEModelManager::CreateModel(FEModelType type,
    const char* name, int captain, bool unidentified59,
    void* unidentified4C, void* unidentified50, bool alternate)
{
    int characterIndex = GetCharacterIndexFromCaptain(captain);
    if (characterIndex != -1)
    {
        return CreateModel(type, name,
            GetCharacterTemplateInfo((eCharacterClass)characterIndex), unidentified59,
            unidentified4C, unidentified50, alternate);
    }
    return 0;
}

FEModelHandle* FEModelManager::CreateModel(FEModelType type,
    const char* name, tCharacterTemplateInfo* modelData, bool unidentified59,
    void* unidentified4C, void* unidentified50, bool alternate)
{
    FEModelHandle* handle = GetModel(name);
    if (handle == 0)
    {
        handle = new (8, false) FEModelHandle(type, name, modelData,
            unidentified59, unidentified4C, unidentified50, alternate);
        mHandles.AddStart(handle);
    }
    return handle;
}

FEModelHandle* FEModelManager::GetModel(const char* name)
{
    u32 hash = nlStringLowerHash(name);
    ListEntry<FEModelHandle*>* entry = mHandles.m_Head;
    while (entry != 0)
    {
        if (hash == entry->entry->mNameHash)
        {
            return entry->entry;
        }
        entry = entry->next;
    }
    return 0;
}

FEImpostorCharacter::FEImpostorCharacter(
    const char* name, ImpostorModel* model, void* animations,
    int budget, bool mirror, bool alternate,
    const ImpostorCharacterParams* params, int modelType)
    : AnimatedImpostorCharacter(
        name, model, animations, budget, 1, 1, params)
    , mModelType(modelType)
{
    model->PlayAnimation((const char*)animations, 0.0f, PM_HOLD);
    model->mAnimController->m_bMirror = mirror;
    mEnabled = true;
    if (alternate)
    {
        char alternateTexture[64];
        char originalTexture[64];
        nlSNPrintf(originalTexture, sizeof(originalTexture), "%s/%s", name, name);
        nlSNPrintf(alternateTexture, sizeof(alternateTexture), "%s_alt/%s_alt", name, name);
        unsigned long original = glGetTexture(originalTexture);
        unsigned long replacement = glGetTexture(alternateTexture);
        model->mOriginalTexture = original;
        model->SetReplacementTexture(replacement);
    }

    char category[128];
    nlSNPrintf(category, sizeof(category),
        "/Render/Impostor/CharacterTweaks/%s", name);
    mfScaleInitialCup.BindWithDefault(
        "mfScaleInitialCup", 1.0f, category, true, 0.0f, 3.0f, 0.001f);
    mfCameraLookatZInitialCup.BindWithDefault(
        "mfCameraLookatZInitialCup", 1.2f, category, true, 0.0f, 10.0f, 0.01f);
    mfCameraDistanceInitialCup.BindWithDefault(
        "mfCameraDistanceInitialCup", 2.3f, category, true, 0.0f, 40.0f, 0.01f);
    mfScaleCup.BindWithDefault(
        "mfScaleCup", 1.0f, category, true, 0.0f, 3.0f, 0.001f);
    mfCameraLookatZCup.BindWithDefault(
        "mfCameraLookatZCup", 1.2f, category, true, 0.0f, 10.0f, 0.01f);
    mfCameraDistanceCup.BindWithDefault(
        "mfCameraDistanceCup", 2.3f, category, true, 0.0f, 40.0f, 0.01f);
}

void FEImpostorCharacter::SetScale(float scale)
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

float FEImpostorCharacter::GetScale()
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

float FEImpostorCharacter::GetCameraDistance()
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

float FEImpostorCharacter::GetCameraLookatZ()
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

void FreeFEModelHandleListEntry(void*, void* entry)
{
    ::operator delete(entry);
}

void FreeFEModelDataListEntry(void*, void* entry)
{
    ::operator delete(entry);
}

void FreeFEModelHandleRingEntry(void*, void* entry)
{
    ::operator delete(entry);
}

void FEImpostorCharacter::UpdateAnimation(float)
{
}

template <>
FEModelManager* nlSingleton<FEModelManager>::s_pInstance = 0;
