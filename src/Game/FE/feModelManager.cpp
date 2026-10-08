#include "NL/nlDLListContainer.inl"
#include "Game/FE/feImpostorCharacter.h"
#include "Game/FE/feModelManager.h"
#include <assert.h>
#include "Game/CharacterTemplate.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/GameInfo.h"
#include "Game/GL/GLSkinMesh.h"
#include "Game/MathHelpers.h"

#include "Game/Render/CrowdImpostors.h"

#include "Game/Render/RLView.h"

#include "Game/Render/ImpostorCharacter.h"
#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorManager.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/gl.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlFileGC.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/Render/StadiumPhysicsObject.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/TweakValue.h"
#include "NL/gl/glState.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakValue.inl"

template <>
FEModelManager* nlSingleton<FEModelManager>::s_pInstance = 0;

struct FEImpostorDimensions
{
    float mOffsetZ;
    float mWidth;
    float mHeight;
    int mTextureWidth;
    int mTextureHeight;
};

static FEImpostorDimensions sFEImpostorDimensions[12] = {
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 384, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 256, 256 },
    { -0.5f, 3.0f, 3.0f, 384, 256 },
};

static float sFEImpostorTintRed = 0.35f;
static float sFEImpostorTintGreen = 0.4f;
static float sFEImpostorTintBlue = 0.45f;
static float sFEImpostorEnabledShade = 0.9f;
static float sFEImpostorDimChance = 0.2f;
static float sFEImpostorOrbitRadius = 0.03f;
static float sFEImpostorOrbitSpeed = 1.0f;
static float sFEImpostorOrbitRadiusRate = 0.5f;
static int sDiddyKongAlphaPacketSecond = 8;
static int sDiddyKongAlphaPacketFirst;
static char sDefaultAnimation[] = "fe_idle";

FEModel::FEModel(tCharacterTemplateInfo* modelData)
    : mModelID(-1)
    , mHierarchy(0)
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

    GLMemoryRequirement requirements[2] = {
        { GLM_Header, 0x8000 },
        { GLM_VertexData, 0x100000 },
    };
    mLoader = glCreateResourcePool(requirements, 2, "FEModelManager");
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
        mTextureFileData = 0;
    }
    if (mModelFileData != 0)
    {
        ::operator delete(mModelFileData);
        mModelFileData = 0;
    }
    mLoader->ReleaseResource(mLoaderHandle);
    glDestroyResourcePool(mLoader);
    if (mAnimations != 0)
    {
        delete mAnimations;
    }
    if (mHierarchies != 0)
    {
        delete mHierarchies;
    }
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
        nlLoadEntireFileAsync(model->mModelData->szFEAnimFilename, OnAnimationsLoaded, model, 32, AllocateEnd, 0, 0, 0);
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
    tCharacterTemplateInfo* modelData, bool mirror, void* loadedCallback,
    void* loadedCallbackData, bool alternate)
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

    mLoadedCallback = loadedCallback;
    mLoadedCallbackData = loadedCallbackData;
    mEnabled = true;
    mMirror = mirror;
    mAnimationCompleteCallback = 0;
    nlVec3Set(mPosition, 0.0f, 0.0f, 0.0f);
    mUseAlternateTextures = alternate;
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
    switch (mModel->mType)
    {
    case FE_MODEL_SKINNED:
    {
        FESkinnedModel* model
            = (FESkinnedModel*)mModel;
        model->mModel->mWorldMatrix = transform;
        break;
    }
    case FE_MODEL_IMPOSTOR:
    {
        FEImpostorModel* model
            = (FEImpostorModel*)mModel;
        model->mPosition = transform.GetTranslation();
        model->mTime = 0.0f;
        for (int i = 0; i < 6; ++i)
        {
            Impostor* impostor = model->mImpostors[i];
            impostor->mPosition.x = transform.m41;
            impostor->mPosition.y = transform.m42;
            impostor->mPosition.z = transform.m43;
        }
        break;
    }
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
            ((FESkinnedModel*)mModel)->mModel->mpAnimController->SetMirror(
                mMirror);
            break;
        }
        case FE_MODEL_IMPOSTOR:
        {
            ((FEImpostorModel*)mModel)->mModel->PlayAnimation(
                name, blendTime, playMode);
            ((FEImpostorModel*)mModel)->mModel->mAnimController->SetMirror(
                mMirror);
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

void FESkinnedModel::Initialize()
{
    if (mTextureFileData != 0)
    {
        ::operator delete(mTextureFileData);
        mTextureFileData = 0;
    }
    if (mModelFileData != 0)
    {
        ::operator delete(mModelFileData);
        mModelFileData = 0;
    }
    if (mModel != 0)
    {
        delete mModel;
        mModel = 0;
    }
    if (mAnimations != 0)
    {
        delete mAnimations;
        mAnimations = 0;
    }
    mAnimations = new (8, false) cInventory<cSAnim>;
    if (mHierarchies != 0)
    {
        delete mHierarchies;
        mHierarchies = 0;
    }
    mHierarchies = new (8, false) cInventory<cSHierarchy>;
    mLoader->ReleaseResource(mLoaderHandle);
    mLoaderHandle = mLoader->MarkResource();
    mModelData = 0;
    mPendingLoads = 0;
    mLoaded = false;
    mSynchronousLoad = false;
    mLoadQueued = false;
    mModelID = -1;
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

void FEImpostorModel::Update(float dt)
{
    for (int i = 0; i < 6; ++i)
    {
        nlVector3 position;
        if (i > 0)
        {
            mTime += dt;
            float angle = mTime * sFEImpostorOrbitSpeed;
            float radius = sFEImpostorOrbitRadius * nlSin(RadToAng16(sFEImpostorOrbitRadiusRate * mTime) + 0x4000);
            angle += DegreesToRadians(72.0f * i);
            float x = radius * nlSin(RadToAng16(angle) + 0x4000);
            float z = radius * nlSin(RadToAng16(angle));
            x = mPosition.x + x;
            nlVec3Set(position, x, mPosition.y,
                mPosition.z + z + sFEImpostorDimensions[mModelData->cc].mOffsetZ);
        }
        else
        {
            position = mPosition;
            position.z += sFEImpostorDimensions[mModelData->cc].mOffsetZ;
        }
        Impostor* impostor = mImpostors[i];
        nlVec3Set(impostor->mPosition, position.x, position.y, position.z);
    }
    if (mModel != 0)
    {
        mModel->Update(dt);
    }
}

void FEImpostorModel::Render()
{
}

FEModelManager::FEModelManager()
    : mResource(0)
{
}

void FEModelManager::DestroyDanglingModels()
{
    nlDLListIterator<FEModelHandle*> danglingModels;
    danglingModels = mDanglingModels.Begin();
    while (danglingModels.hasNext())
    {
        FEModelHandle* handle = *danglingModels;
        if (!handle->mModel->mLoadQueued)
        {
            delete handle;
            mDanglingModels.RemoveEntry(danglingModels.next());
        }
        else
        {
            danglingModels.Step();
        }
    }
}

void FEModelManager::DestroyReleasedModels()
{
    nlDLListIterator<FEModelHandle*> pendingModels;
    pendingModels = mReleasedModels.Begin();
    while (pendingModels.hasNext())
    {
        FEModelHandle* handle = *pendingModels;
        if (handle->CanDestroy())
        {
            delete *pendingModels;
            mReleasedModels.RemoveEntry(pendingModels.next());
        }
        else
        {
            pendingModels.Step();
        }
    }
}

FEModelManager::~FEModelManager()
{
    if (mResource != 0)
    {
        glGetCurrentResourcePool()->ReleaseResource(mResource);
        mResource = 0;
    }

    while (mActiveModels.GetHead() != 0)
    {
        DestroyModel(*mActiveModels.GetHead());
    }

    nlDLListIterator<FEModelHandle*> loadedModels;
    loadedModels = mQueuedModels.Begin();
    while (loadedModels.hasNext())
    {
        delete *loadedModels;
        mQueuedModels.Remove(&loadedModels);
    }

    while (mReleasedModels.CountElements() != 0)
    {
        nlServiceFileSystem();
        DestroyReleasedModels();
        DestroyDanglingModels();
    }
}

void FEModelManager::Update(float dt)
{
    DestroyDanglingModels();

    BeginLoadModels();

    DestroyReleasedModels();

    ListEntry<FEModelHandle*>* entry = mActiveModels.m_Head;
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

void FEModelManager::FinishLoadModel(FEModelHandle* handle)
{
    FEModel* model = handle->mModel;
    if (model->mSynchronousLoad)
    {
        return;
    }

    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
    CurrentAllocator = &VirtualAllocator;
    glEndLoadTextureBundle(model->mTextureFileData,
        model->mTextureFileDataSize, model->mLoader, 0);
    ::operator delete(model->mTextureFileData);
    model->mTextureFileData = 0;
    if (handle->mUseAlternateTextures)
    {
        glEndLoadTextureBundle(model->mAlternateTextureFileData,
            model->mAlternateTextureFileDataSize, model->mLoader, 0);
        ::operator delete(model->mAlternateTextureFileData);
        model->mAlternateTextureFileData = 0;
    }

    unsigned long numModels = 0;
    model->mModelID = glEndLoadModel(model->mModelFileData,
        model->mModelFileDataSize, &numModels, model->mLoader)->id;
    ::operator delete(model->mModelFileData);
    model->mModelFileData = 0;

    model->mHierarchies->AddFile((char*)model->mHierarchyFileData,
        model->mHierarchyFileDataSize);
    cInventory<cSHierarchy>* hierarchies = model->mHierarchies;
    model->mHierarchy = hierarchies->Find(nlStringHash(model->mModelData->szHierarchy));
    model->mAnimations->AddFile((char*)model->mAnimationFileData,
        model->mAnimationFileDataSize);

    switch (model->mType)
    {
    case FE_MODEL_SKINNED:
    {
        FESkinnedModel* skinnedModel = (FESkinnedModel*)handle->mModel;
        skinnedModel->mModel = new (8, false) SkinAnimatedNPC(
            *handle->mModel->mHierarchy,
            handle->mModel->mModelID, handle->mModel->mLoader);
        break;
    }
    case FE_MODEL_IMPOSTOR:
    {
        FEModelManager* manager = FEModelManager::Instance();
        if (manager->mResource == 0)
        {
            manager->mResource = glGetCurrentResourcePool()->MarkResource();
            // R4QE01 queries the pool's free memory here and discards the
            // result: r3 from the virtual call is overwritten by the next load
            // without being read. No print or assertion survives around it,
            // and what enclosed the query is not recoverable from a stripped
            // executable.
            glGetCurrentResourcePool()->GetFreeMemory();
        }
        const CharacterInfo& characterInfo = GetCharacterInfo(model->mModelData->cc);
        FEImpostorModel* impostorModel = (FEImpostorModel*)handle->mModel;
        impostorModel->mModel = new (8, false) ImpostorModel(
            *model->mHierarchy, model->mModelID,
            model->mAnimations, model->mLoader);
        impostorModel->mModel->mSkinMesh->m_Unknown0C = 0;

        ImpostorCharacterParams params;
        int character = handle->mModel->mModelData->cc;
        params.mWidth = sFEImpostorDimensions[character].mTextureWidth;
        params.mHeight = sFEImpostorDimensions[character].mTextureHeight;
        params.mUseAdditiveBlend = 1;
        params.mUseIntensityAlpha = 0;
        params.mBaseAngle = 0xc000;
        enum ModelType { Default, InitialCup, Cup };
        ModelType modelType = Default;
        if (GameInfoManager::Instance()->IsInMode3())
        {
            modelType = CupManager::s_pInstance->GetCurrentMode() == -1 ? InitialCup : Cup;
        }
        impostorModel->mCharacter = new (8, false) FEImpostorCharacter(
            characterInfo.mName, impostorModel->mModel,
            (void*)handle->mDefaultAnimation, 30, handle->mMirror,
            handle->mUseAlternateTextures, &params, modelType);
        for (int i = 0; i < 6; ++i)
        {
            int slot;
            impostorModel->mImpostors[i] = ImpostorManager::GetInstance()->AllocImpostor(&slot);
            int width = sFEImpostorDimensions[impostorModel->mModelData->cc].mWidth;
            int height = sFEImpostorDimensions[impostorModel->mModelData->cc].mHeight;
            impostorModel->mImpostors[i]->Set(impostorModel->mCharacter,
                handle->mPosition, width, height, 0xc000);
        }
        impostorModel->mPosition = handle->mPosition;
        break;
    }
    }

    if (handle->mLoadedCallback != 0)
    {
        ((void (*)(FEModelHandle*, void*))handle->mLoadedCallback)(handle, handle->mLoadedCallbackData);
    }
    handle->mModel->mLoaded = true;
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void FEModelManager::Render()
{
    nlListIterator<FEModelHandle*> iterator = mActiveModels.Begin();
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

void FEModelManager::RegisterObject(StadiumFEModelMarker* object)
{
    mObjects.AddStart(object);
}

StadiumFEModelMarker* FEModelManager::GetObject(int id)
{
    nlListIterator<StadiumFEModelMarker*> iterator = mObjects.Begin();
    while (iterator.IsValid())
    {
        StadiumFEModelMarker* object = iterator.Current();
        if (id == object->mMarkerID)
        {
            return object;
        }
        iterator.Next();
    }
    return 0;
}

FEModelHandle* FEModelManager::CreateModel(FEModelType type,
    const char* name, int captain, bool mirror,
    void* loadedCallback, void* loadedCallbackData, bool alternate)
{
    int characterIndex = GetCharacterIndexFromCaptain(captain);
    if (characterIndex != -1)
    {
        return CreateModel(type, name,
            GetCharacterTemplateInfo((eCharacterClass)characterIndex), mirror,
            loadedCallback, loadedCallbackData, alternate);
    }
    return 0;
}

FEModelHandle* FEModelManager::CreateModel(FEModelType type,
    const char* name, tCharacterTemplateInfo* modelData, bool mirror,
    void* loadedCallback, void* loadedCallbackData, bool alternate)
{
    {
        unsigned int nameHash = nlStringLowerHash(name);
        nlDLListIterator<FEModelHandle*> queued;
        queued = mQueuedModels.Begin();
        while (queued.hasNext())
        {
            assert((*queued)->mNameHash != nameHash);
            queued.Step();
        }
    }
    {
        unsigned int nameHash = nlStringLowerHash(name);
        nlListIterator<FEModelHandle*> active = mActiveModels.Begin();
        while (active.IsValid())
        {
            assert(active.Current()->mNameHash != nameHash);
            active.Next();
        }
    }

    FEModelHandle* handle = 0;
    unsigned int nameHash = nlStringLowerHash(name);
    nlDLListIterator<FEModelHandle*> pending;
    pending = mReleasedModels.Begin();
    while (pending.hasNext())
    {
        FEModelHandle* current = *pending;
        if (nameHash == current->mNameHash)
        {
            handle = current;
            if (handle->mModel->mLoadQueued
                || handle->mModel->mType == FE_MODEL_IMPOSTOR)
            {
                mDanglingModels.AddEnd(handle);
                mReleasedModels.Remove(&pending);
                handle = 0;
                nlPrintf("Can't reuse this model handle as there is still a pending load happening.  Added to dangling model list.\n");
            }
            else
            {
                mReleasedModels.Remove(&pending);
            }
            break;
        }
        pending.Step();
    }

    if (handle == 0)
    {
        handle = new (8, false) FEModelHandle(type, name, modelData,
            mirror, loadedCallback, loadedCallbackData, alternate);
    }
    else
    {
        handle->mNameHash = nlStringLowerHash(name);
        handle->mModel->Initialize();
        handle->mModel->mModelData = modelData;
        handle->mLoadedCallback = loadedCallback;
        handle->mLoadedCallbackData = loadedCallbackData;
        handle->mEnabled = true;
        handle->mMirror = mirror;
        handle->mAnimationCompleteCallback = 0;
        handle->mUseAlternateTextures = alternate;
    }
    handle->mModel->mType = type;
    mQueuedModels.AddEnd(handle);
    return handle;
}

bool FEModelManager::DestroyModel(FEModelHandle* handle)
{
    nlDLListIterator<FEModelHandle*> iterator;
    iterator = mQueuedModels.Begin();
    while (iterator.hasNext())
    {
        if (*iterator == handle)
        {
            delete *iterator;
            mQueuedModels.Remove(&iterator);
            return true;
        }
        iterator.Step();
    }

    mActiveModels.RemoveEntry(handle);
    mReleasedModels.AddEnd(handle);
    return true;
}

void FEModelManager::BeginLoadModels()
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    nlDLListIterator<FEModelHandle*> iterator;
    iterator = mQueuedModels.Begin();
    while (iterator.hasNext())
    {
        FEModelHandle* handle = *iterator;
        FEModel* model = handle->mModel;
        tCharacterTemplateInfo* modelData = model->mModelData;
        model->mPendingLoads = 0xf;
        if (handle->mUseAlternateTextures)
        {
            handle->mUseAlternateTextures = glBeginLoadTextureBundle(
                modelData->szAlternateTextureFilename, FEModel::OnAlternateTexturesLoaded,
                model, model->mLoader);
        }
        glBeginLoadTextureBundle(modelData->szTextureFilename,
            FEModel::OnTexturesLoaded, model, model->mLoader);
        mActiveModels.AddEnd(handle);
        mQueuedModels.Remove(&iterator);
        break;
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

FEModelHandle* FEModelManager::GetModel(const char* name)
{
    u32 hash = nlStringLowerHash(name);
    ListEntry<FEModelHandle*>* entry = mActiveModels.m_Head;
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

void FEModelManager::ReleaseImpostors()
{
    ImpostorManager::GetInstance()->ResetImpostors();
    nlDLListSlotPool<ImpostorCharacter*>& characters = ImpostorManager::GetInstance()->mCharacters;
    characters.Free();
    BasicSlotPool<DLListEntry<ImpostorCharacter*> >* pool =
        &ImpostorManager::GetInstance()->mCharacters.m_Allocator;
    pool->FreeBlocks();
    unsigned long resource = mResource;
    if (resource != 0)
    {
        glGetCurrentResourcePool()->ReleaseResource(resource);
        mResource = 0;
    }
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

void FEImpostorCharacter::Render(GLView* target, int texture)
{
    nlDLListIterator<ImpostorSprite*> iterator;
    iterator = mSprites.Begin();
    while (iterator.hasNext())
    {
        ImpostorSprite* sprite = *iterator;
        int numSlots = sprite->mNumImpostorSlots;
        int* slots = sprite->mImpostorSlots;
        bool dim = nlRandomf(0.0f, 1.0f, &nlDefaultSeed) < sFEImpostorDimChance;
        float shade = dim ? 196 : 255;
        for (int i = 0; i < numSlots; ++i)
        {
            int slot = slots[i];
            Impostor* impostor = &ImpostorManager::GetInstance()->mImpostors[slot];
            if (impostor->mpCharacter == this)
            {
                shade *= mEnabled ? sFEImpostorEnabledShade : 1.0f - sFEImpostorEnabledShade;
                nlColourSet(impostor->mColour,
                    sFEImpostorTintRed * shade, sFEImpostorTintGreen * shade, sFEImpostorTintBlue * shade, 255);
            }
        }
        iterator.Step();
    }

    mModels[texture]->Render(target, 0);
    if (nlStrNCmp(mName, "diddykong", 16) == 0)
    {
        int numPackets = mModels[texture]->mLastModel->numPackets;
        glModel* model = mModels[texture]->mLastModel;
        if (sDiddyKongAlphaPacketFirst < numPackets)
        {
            static unsigned long alphaValue = nlStringLowerHash("alphaValue");
            glSetMaterialFloatParameter(&model->packets[sDiddyKongAlphaPacketFirst], alphaValue, 0.0f);
        }
        if (sDiddyKongAlphaPacketSecond < numPackets)
        {
            static unsigned long alphaValue = nlStringLowerHash("alphaValue");
            glSetMaterialFloatParameter(&model->packets[sDiddyKongAlphaPacketSecond], alphaValue, 0.0f);
        }
    }
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
