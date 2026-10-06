#include "NL/nlDLListContainer.inl"
#include "Game/Render/KoopaShellObject.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/FlyingCamera.h"
#include "Game/Render/tu_801B43F8.h"
#include "Game/Render/WindDebrisConfig.h"
#include "Game/AsyncLoading.h"
#include "Game/Drawable/RenderObject.h"

#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "Game/Render/DaisyFist.h"
#include "Game/GameTweaks.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlCompressedFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "Game/Render/HammerObject.h"
#include "Game/Render/DiddyBanana.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/BulletBill.h"
#include "Game/Render/ThwompObject.h"
#include "Game/Render/YoshiEggObject.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"

#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlPrint.h"

float gHammerRadius = 0.48f;
const float sBulletBillRadius = 0.45f;
const float lbl_806E5214 = 1.0f;

NPCManager* gNPCManager;
NPCManager* gNPCManagerInstance;

NPCManager::NPCManager()
    : mPersistentHierarchies(0)
    , mTransientHierarchies(0)
    , mPendingTemplate(0)
    , mpChainChomp(0)
    , mpYoshiEgg(0)
    , mpBirdoEgg(0)
    , mpKoopaShell(0)
    , mNumVisibleDaisyFists(0)
    , mNumBulletBills(0)
    , mpDiddyBanana(0)
{
    gNPCManagerInstance = this;
    mPersistentHierarchies = new (nlMalloc(
        sizeof(cInventory<cSHierarchy>), 8, false)) cInventory<cSHierarchy>();
    mTransientHierarchies = new (nlMalloc(
        sizeof(cInventory<cSHierarchy>), 8, false)) cInventory<cSHierarchy>();

    unsigned int i;
    for (i = 0; i < 15; ++i)
    {
        mHammers[i] = 0;
    }
    for (i = 0; i < 6; ++i)
    {
        mBulletBills[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        mDaisyFists[i] = 0;
    }
    for (i = 0; i < 3; ++i)
    {
        mWindDebris[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        mThwomps[i] = 0;
    }
}

void NPCManager::CreateNPCTemplate(
    const char* pName, bool bPersistent)
{
    // R4QE01 measures the name here and discards the result: strlen runs on
    // pName before the allocation and r3 is overwritten by the nlMalloc size
    // without the length ever being read. The GameCube predecessor's engine
    // does the same thing in two places, and its debug metadata shows no local
    // holding the length at either site, so the evaluation was not assigned in
    // the original source. What enclosed it is not recoverable from a stripped
    // executable; the copy below is the plain strcpy the image shows.
    strlen(pName);
    NPCTemplate* pTemplate
        = new (nlMalloc(sizeof(NPCTemplate), 8, false))
            NPCTemplate();
    strcpy(pTemplate->mName, pName);
    pTemplate->mPersistent = bPersistent;

    if (bPersistent)
    {
        mPersistentTemplates.AddEnd(pTemplate);
    }
    else
    {
        mTransientTemplates.AddEnd(pTemplate);
    }
}

bool NPCManager::SelectNextNPCTemplate()
{
    mPendingTemplate = 0;
    for (int i = 0; i < 2; ++i)
    {
        nlDLListIterator<NPCTemplate*> iterator;
        iterator = i == 0 ? mPersistentTemplates.Begin()
                     : mTransientTemplates.Begin();
        while (iterator.hasNext())
        {
            if (!(*iterator)->loaded)
            {
                mPendingTemplate = *iterator;
                return true;
            }
            iterator.next();
        }
    }
    return false;
}

void NPCManager::CreateChainChomp()
{
    NPCTemplate* pTemplate
        = fn_801ABBDC_inline("ChainChomp");

    PhysicsNPC* chainPhysics = new (nlMalloc(
        sizeof(PhysicsNPC), 8, false)) PhysicsNPC(
        gGameTweaks.m_pGameTweaks->fChainChompRadius);

    ChainChomp* chainChomp = new (nlMalloc(sizeof(ChainChomp), 8, false))
        ChainChomp(*pTemplate->hierarchy, pTemplate->modelID,
            *chainPhysics, &pTemplate->mInventorySAnim, pTemplate->mResourcePool);
    mpChainChomp = chainChomp;
    chainPhysics->SetCallbackFunction(&ChainChomp::CollisionCallback);
}

void NPCManager::CreateYoshiEgg()
{
    mpYoshiEgg = new (8, false) YoshiEggObject(GetRenderObject(3, 0));
}

void NPCManager::CreateBirdoEgg()
{
    BirdoEggObject* pObject = new (nlMalloc(sizeof(BirdoEggObject), 8, false))
        BirdoEggObject(GetRenderObject(4, 0));
    mpBirdoEgg = pObject;
}

void NPCManager::CreateKoopaShell()
{
    mpKoopaShell = new (8, false) KoopaShellObject(GetRenderObject(5, 0));
}

void NPCManager::CreateDaisyFists()
{
    for (unsigned int i = 0; i < 8; ++i)
    {
        DaisyFistObject* pObject
            = (DaisyFistObject*)nlMalloc(sizeof(DaisyFistObject), 8, false);
        pObject = new (pObject) DaisyFistObject(i);
        mDaisyFists[i] = pObject;
    }
}

DaisyFistObject* NPCManager::GetDaisyFist(int nIndex)
{
    if (nIndex >= 0)
    {
        return mDaisyFists[nIndex];
    }

    DaisyFistObject* pObject;
    for (unsigned int i = 0; i < 8; ++i)
    {
        pObject = mDaisyFists[i];
        if (pObject != 0 && !pObject->mVisible)
        {
            mNumVisibleDaisyFists = 8;
            return mDaisyFists[i];
        }
    }
    return 0;
}

BulletBillObject* NPCManager::GetBulletBill(int nIndex)
{
    return mBulletBills[nIndex];
}

BulletBillObject* NPCManager::fn_801A9D20()
{
    BulletBillObject* pObject = 0;
    for (unsigned int i = 0; i < 6; ++i)
    {
        if (mBulletBills[i] == 0)
        {
            pObject = new (8, false) BulletBillObject(
                GetRenderObject(1, i), i, sBulletBillRadius, lbl_806E5214);
            mBulletBills[i] = pObject;
            mNumBulletBills = i + 1;
            break;
        }
    }
    return pObject;
}

UnidentifiedNPC_801B43F8* NPCManager::fn_801A9DE0(int nIndex)
{
    return mWindDebris[nIndex];
}

void NPCManager::CreateWindDebris()
{
    for (long i = 0; i < 3; ++i)
    {
        WindDebrisConfig* pConfig = GetWindDebrisConfig(i);
        NPCTemplate* pTemplate
            = fn_801ABBDC_inline(pConfig->mName);

        PhysicsNPC* pPhysics = new (8, false) PhysicsNPC(
            pConfig->mRadius);
        UnidentifiedNPC_801B43F8* pObject = new (8, false) UnidentifiedNPC_801B43F8(
            *pTemplate->hierarchy, pTemplate->modelID,
            pConfig->mCueId, pConfig->mUnidentified010,
            *pPhysics, &pTemplate->mInventorySAnim,
            pTemplate->mResourcePool);
        mWindDebris[i] = pObject;
        pPhysics->SetCallbackFunction(UnidentifiedNPC_801B43F8::fn_801B4830);
    }
}

void NPCManager::CreateDiddyBanana()
{
    NPCTemplate* pTemplate
        = fn_801ABBDC_inline("DiddyBanana");
    DiddyBanana* pObject
        = (DiddyBanana*)nlMalloc(0x84, 8, false);
    pObject = new (pObject) DiddyBanana(
        *pTemplate->hierarchy, pTemplate->modelID, pTemplate->mInventorySAnim, pTemplate->mResourcePool);
    mpDiddyBanana = pObject;
}

void NPCManager::CreateHammers()
{
    for (unsigned int i = 0; i < 15; ++i)
    {
        HammerObject* pObject
            = (HammerObject*)nlMalloc(sizeof(HammerObject), 8, false);
        pObject = new (pObject) HammerObject(i, gHammerRadius);
        mHammers[i] = pObject;
    }
}

int NPCManager::GetNumHammers()
{
    return mHammers[0] == 0 ? 0 : 15;
}

void NPCManager::ResetActiveHammers()
{
    for (int i = 0; i < 15; ++i)
    {
        HammerObject* pObject = mHammers[i];
        if (pObject != 0 && pObject->mActive)
        {
            pObject->Reset(true);
        }
    }
}

HammerObject* NPCManager::GetHammer(int nIndex)
{
    if (nIndex >= 0)
    {
        return mHammers[nIndex];
    }

    for (int i = 0; i < 15; ++i)
    {
        if (mHammers[i] != 0 && !mHammers[i]->mActive)
        {
            return mHammers[i];
        }
    }
    return 0;
}

void NPCManager::CreateThwomps()
{
    for (unsigned int i = 0; i < 8; ++i)
    {
        ThwompObject* pObject
            = (ThwompObject*)nlMalloc(sizeof(ThwompObject), 8, false);
        pObject = new (pObject) ThwompObject(i);
        mThwomps[i] = pObject;
    }
}

ThwompObject* NPCManager::GetThwomp(
    int nIndex)
{
    if (nIndex >= 0)
    {
        return mThwomps[nIndex];
    }

    for (int i = 0; i < 8; ++i)
    {
        if (mThwomps[i] != 0 && !mThwomps[i]->mVisible)
        {
            return mThwomps[i];
        }
    }
    return 0;
}

void OnNPCAnimationsLoaded(
    void* pData, unsigned long nSize, void* pUserData)
{
    gNPCManager->mPendingTemplate->mAnimationsLoaded = true;
    ((cInventory<cSAnim>*)pUserData)->AddFile(pData, nSize);
}

void OnNPCHierarchyLoaded(
    void* pData, unsigned long nSize, void* pUserData)
{
    gNPCManager->mPendingTemplate->mHierarchyLoaded = true;
    ((cInventory<cSHierarchy>*)pUserData)->AddFile(pData, nSize);
}

void OnNPCTexturesLoaded(
    void* pData, unsigned long nSize, void* pUserData)
{
    NPCManager* pManager = gNPCManager;
    NPCTemplate* pTemplate
        = (NPCTemplate*)pUserData;
    pTemplate->mResourcePool = glGetCurrentResourcePool();
    pManager->mPendingTemplate->mTexturesLoaded = true;
    glEndLoadTextureBundle(pData, nSize, glGetCurrentResourcePool(), 0);
    nlFree(pData);
}

void OnNPCModelLoaded(
    void* pData, unsigned long nSize, void* pUserData)
{
    NPCTemplate* pTemplate
        = (NPCTemplate*)pUserData;
    pTemplate->mResourcePool = glGetCurrentResourcePool();
    unsigned long nNumModels = 0;
    unsigned int* pModel = (unsigned int*)glEndLoadModel(
        pData, nSize, &nNumModels, glGetCurrentResourcePool());
    pTemplate->modelID = *pModel;
    nlFree(pData);
}

void NPCManager::BeginLoadNPCTemplate()
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    GLResourcePool* pContext;
    if (mPendingTemplate->mPersistent)
    {
        pContext = AsyncLoadingManager::Instance()->GetPersistentResourcePool();
    }
    else
    {
        pContext = glGetCurrentResourcePool();
    }

    char path[256];
    nlSNPrintf(path, sizeof(path), "art/animation/%s.sanim.zlib", mPendingTemplate->mName, mPendingTemplate->mName);
    if (nlLoadCompressedFileAsync(path, OnNPCAnimationsLoaded, &mPendingTemplate->mInventorySAnim, 0x20, AllocateStart, 0x40000, 0, 0, 0, 0, &StandardAllocator))
    {
        mPendingTemplate->mAnimationLoadStarted = true;
    }

    nlSNPrintf(path, sizeof(path), "art/animation/%s.shier", mPendingTemplate->mName, mPendingTemplate->mName);
    cInventory<cSHierarchy>* pInventory = mPendingTemplate->mPersistent
                                            ? mPersistentHierarchies
                                            : mTransientHierarchies;
    nlLoadEntireFileAsync(path, OnNPCHierarchyLoaded, pInventory, 0x20, AllocateStart, 0, 0, &StandardAllocator);

    nlSNPrintf(path, sizeof(path), "art/characters/npcs/%s/%s.rlt", mPendingTemplate->mName, mPendingTemplate->mName);
    glBeginLoadTextureBundle(path, OnNPCTexturesLoaded, mPendingTemplate, pContext);

    nlSNPrintf(path, sizeof(path), "art/characters/npcs/%s/%s.rlg", mPendingTemplate->mName, mPendingTemplate->mName);
    glBeginLoadModel(path, OnNPCModelLoaded, mPendingTemplate, pContext);
}

bool NPCManager::FinishLoadNPCTemplate()
{
    if (mPendingTemplate->mAnimationLoadStarted
        && !mPendingTemplate->mAnimationsLoaded)
    {
        return false;
    }
    if (!mPendingTemplate->mHierarchyLoaded)
    {
        return false;
    }
    if (!mPendingTemplate->mTexturesLoaded)
    {
        return false;
    }
    if (mPendingTemplate->modelID == -1)
    {
        return false;
    }

    if (mPendingTemplate->mPersistent)
    {
        mPendingTemplate->hierarchy = mPersistentHierarchies->Find(
            nlStringLowerHash(mPendingTemplate->mName));
    }
    else
    {
        mPendingTemplate->hierarchy = mTransientHierarchies->Find(
            nlStringLowerHash(mPendingTemplate->mName));
    }
    mPendingTemplate->loaded = true;

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return true;
}

void NPCManager::UnloadTransientNPCTemplates()
{
    nlDLListIterator<NPCTemplate*> iterator;
    iterator = mTransientTemplates.Begin();
    while (iterator.hasNext())
    {
        delete *iterator;
        iterator.next();
    }
    mTransientTemplates.Clear();
    mTransientHierarchies->Clear();
}

static inline void DestroyNPCs(NPCManager* pManager)
{
    if (pManager->mpChainChomp != 0)
    {
        delete pManager->mpChainChomp;
        pManager->mpChainChomp = 0;
    }
    if (pManager->mpDiddyBanana != 0)
    {
        delete pManager->mpDiddyBanana;
        pManager->mpDiddyBanana = 0;
    }

    if (pManager->mpYoshiEgg != 0)
    {
        delete pManager->mpYoshiEgg;
        pManager->mpYoshiEgg = 0;
    }
    if (pManager->mpBirdoEgg != 0)
    {
        delete pManager->mpBirdoEgg;
        pManager->mpBirdoEgg = 0;
    }
    if (pManager->mpKoopaShell != 0)
    {
        delete pManager->mpKoopaShell;
        pManager->mpKoopaShell = 0;
    }

    for (unsigned int i = 0; i < 8; ++i)
    {
        if (pManager->mDaisyFists[i] != 0)
        {
            delete pManager->mDaisyFists[i];
            pManager->mDaisyFists[i] = 0;
        }
    }
    for (unsigned int i = 0; i < 6; ++i)
    {
        if (pManager->mBulletBills[i] != 0)
        {
            delete pManager->mBulletBills[i];
            pManager->mBulletBills[i] = 0;
        }
    }
    pManager->mNumBulletBills = 0;
    for (unsigned int i = 0; i < 15; ++i)
    {
        if (pManager->mHammers[i] != 0)
        {
            delete pManager->mHammers[i];
            pManager->mHammers[i] = 0;
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        if (pManager->mWindDebris[i] != 0)
        {
            delete pManager->mWindDebris[i];
            pManager->mWindDebris[i] = 0;
        }
    }
    for (unsigned int i = 0; i < 8; ++i)
    {
        if (pManager->mThwomps[i] != 0)
        {
            delete pManager->mThwomps[i];
            pManager->mThwomps[i] = 0;
        }
    }
    ResetFlyingCameras();
}

NPCManager::~NPCManager()
{
    for (int i = 0; i < 2; ++i)
    {
        nlDLListIterator<NPCTemplate*> iterator;
        iterator = i == 0 ? mPersistentTemplates.Begin()
                     : mTransientTemplates.Begin();
        while (iterator.hasNext())
        {
            delete *iterator;
            iterator.next();
        }
    }
    mPersistentTemplates.Clear();
    mTransientTemplates.Clear();

    ::DestroyNPCs(this);
    delete mPersistentHierarchies;
    delete mTransientHierarchies;
    gNPCManagerInstance = 0;
}

void NPCManager::DestroyNPCs()
{
    ::DestroyNPCs(this);
}

NPCTemplate* NPCManager::FindNPCTemplate(const char* pName)
{
    for (int i = 0; i < 2; ++i)
    {
        nlDLListIterator<NPCTemplate*> iterator;
        iterator = i == 0 ? mPersistentTemplates.Begin()
                     : mTransientTemplates.Begin();
        while (iterator.hasNext())
        {
            char name[40];
            unsigned long length = nlStrLen((*iterator)->mName) + 1;
            unsigned long copyLength = sizeof(name);
            if (length <= sizeof(name))
            {
                copyLength = length;
            }
            nlStrNCpy(name, pName, copyLength);
            if (nlStrICmp(name, (*iterator)->mName) == 0)
            {
                return *iterator;
            }
            iterator.next();
        }
    }
    return 0;
}

void NPCManager::UpdateNPCs(float dt)
{
}

void NPCManager::RenderNPCs()
{
}

void NPCManager::UpdateAINPCs(float dt)
{
    mpChainChomp->Update(dt);
    if (mpYoshiEgg != 0)
    {
        mpYoshiEgg->Update(dt);
    }
    if (mpBirdoEgg != 0)
    {
        mpBirdoEgg->Update(dt);
    }
    if (mpKoopaShell != 0)
    {
        mpKoopaShell->Update(dt);
    }
    if (mpDiddyBanana != 0)
    {
        mpDiddyBanana->Update(dt);
    }

    mNumVisibleDaisyFists = 0;
    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        if (mDaisyFists[i] != 0)
        {
            mDaisyFists[i]->Update(dt);
            if (mDaisyFists[i]->mVisible)
            {
                ++mNumVisibleDaisyFists;
            }
        }
    }
    for (i = 0; i < mNumBulletBills; ++i)
    {
        mBulletBills[i]->Update(dt);
    }
    for (i = 0; i < 15; ++i)
    {
        if (mHammers[i] != 0)
        {
            mHammers[i]->Update(dt);
        }
    }
    for (i = 0; i < 3; ++i)
    {
        if (mWindDebris[i] != 0)
        {
            mWindDebris[i]->Update(dt);
        }
    }
    for (i = 0; i < 8; ++i)
    {
        if (mThwomps[i] != 0)
        {
            mThwomps[i]->Update(dt);
        }
    }
    UpdateFlyingCameras(dt);
}

void NPCManager::ResetNPCs()
{
    if (mpChainChomp != 0)
    {
        mpChainChomp->Hide();
    }
    if (mpYoshiEgg != 0)
    {
        mpYoshiEgg->Reset();
    }
    if (mpBirdoEgg != 0)
    {
        mpBirdoEgg->Reset();
    }
    if (mpKoopaShell != 0)
    {
        mpKoopaShell->Reset();
    }
    if (mpDiddyBanana != 0)
    {
        mpDiddyBanana->Hide();
    }

    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        if (mDaisyFists[i] != 0)
        {
            mDaisyFists[i]->Reset();
        }
        mNumVisibleDaisyFists = 0;
    }
    for (i = 0; i < mNumBulletBills; ++i)
    {
        mBulletBills[i]->Reset();
    }
    for (i = 0; i < 15; ++i)
    {
        if (mHammers[i] != 0)
        {
            mHammers[i]->Reset(false);
        }
    }
    for (i = 0; i < 3; ++i)
    {
        if (mWindDebris[i] != 0)
        {
            mWindDebris[i]->fn_801B4B9C();
        }
    }
    for (i = 0; i < 8; ++i)
    {
        if (mThwomps[i] != 0)
        {
            mThwomps[i]->Stop(true);
        }
    }
}
