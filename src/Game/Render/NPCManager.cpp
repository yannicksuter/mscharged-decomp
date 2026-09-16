#include "Game/Render/KoopaShellObject.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/tu_801B43F8.h"
#include "Game/Render/tu_801B532C.h"
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
#include "unclassified/tu_801A0E64.h"
#include "Game/Render/DiddyBanana.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/BulletBill.h"
#include "Game/Render/ThwompObject.h"
#include "Game/Render/YoshiEggObject.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C"
{
    void* fn_8019AE7C(void* pObject, cSHierarchy* pHierarchy, int nModel,
        void* pPhysics, cInventory<cSAnim>* pInventory, void* pResource);
    void fn_8019BF40(
        PhysicsObject*, PhysicsObject*, const nlVector3&);

    BirdoEggObject* __ct(BirdoEggObject* pObject, RenderObject* pDrawable);
    BirdoEggObject* __dt(BirdoEggObject* pObject, int bDelete);
    void Update(BirdoEggObject* pObject, float fDeltaT);
    void Reset(BirdoEggObject* pObject);

    void fn_801A01F8();
    void fn_801A0208(float fDeltaT);
}

int nlSNPrintf(char* pBuffer, unsigned long nSize, const char* pFormat, ...);

static char sChainChompTemplateName[] = "ChainChomp";
static char sDiddyBananaTemplateName[] = "DiddyBanana";
static char sNPCAnimationPath[] = "art/animation/%s.sanim.zlib";
static char sNPCHierarchyPath[] = "art/animation/%s.shier";
static char sNPCTexturePath[] = "art/characters/npcs/%s/%s.rlt";
static char sNPCModelPath[] = "art/characters/npcs/%s/%s.rlg";

float lbl_806DD000 = 0.48f;
const float lbl_806E5210 = 0.45f;
const float lbl_806E5214 = 1.0f;

NPCManager* gNPCManager;
NPCManager* gNPCManagerInstance;

NPCManager::NPCManager()
    : mPersistentHierarchies(0)
    , mTransientHierarchies(0)
    , mPendingTemplate(0)
    , mpChainChomp(0)
    , mUnidentified024(0)
    , mpBirdoEgg(0)
    , mUnidentified02C(0)
    , mUnidentified030(0)
    , mUnidentified054(0)
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
        mUnidentified070[i] = 0;
    }
    for (i = 0; i < 6; ++i)
    {
        mUnidentified058[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        mDaisyFists[i] = 0;
    }
    for (i = 0; i < 3; ++i)
    {
        mUnidentified0CC[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        mThwomps[i] = 0;
    }
}

void NPCManager::CreateNPCTemplate(
    const char* pName, bool bPersistent)
{
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
        nlDLListIterator<NPCTemplate*> iterator
            = i == 0 ? mPersistentTemplates.Begin()
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
        = fn_801ABBDC_inline(sChainChompTemplateName);

    PhysicsNPC* chainPhysics = new (nlMalloc(
        sizeof(PhysicsNPC), 8, false)) PhysicsNPC(
        gGameTweaks.m_pGameTweaks->fChainChompRadius);

    ChainChomp* chainChomp = new (nlMalloc(sizeof(ChainChomp), 8, false))
        ChainChomp(*pTemplate->hierarchy, pTemplate->modelID,
            *chainPhysics, &pTemplate->mInventorySAnim, pTemplate->mResourcePool);
    mpChainChomp = chainChomp;
    chainPhysics->SetCallbackFunction(&ChainChomp::CollisionCallback);
}

void NPCManager::fn_801A9AF8()
{
    mUnidentified024 = new (8, false) YoshiEggObject(GetRenderObject(3, 0));
}

void NPCManager::CreateBirdoEgg()
{
    BirdoEggObject* pObject = new (nlMalloc(sizeof(BirdoEggObject), 8, false))
        BirdoEggObject(GetRenderObject(4, 0));
    mpBirdoEgg = pObject;
}

void NPCManager::fn_801A9BD0()
{
    mUnidentified02C = new (8, false) KoopaShellObject(GetRenderObject(5, 0));
}

void NPCManager::fn_801A9C3C()
{
    for (unsigned int i = 0; i < 8; ++i)
    {
        DaisyFistObject* pObject
            = (DaisyFistObject*)nlMalloc(sizeof(DaisyFistObject), 8, false);
        pObject = new (pObject) DaisyFistObject(i);
        mDaisyFists[i] = pObject;
    }
}

DaisyFistObject* NPCManager::fn_801A9CA4(int nIndex)
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
            mUnidentified030 = 8;
            return mDaisyFists[i];
        }
    }
    return 0;
}

BulletBillObject* NPCManager::fn_801A9D10(int nIndex)
{
    return mUnidentified058[nIndex];
}

BulletBillObject* NPCManager::fn_801A9D20()
{
    BulletBillObject* pObject = 0;
    for (unsigned int i = 0; i < 6; ++i)
    {
        if (mUnidentified058[i] == 0)
        {
            pObject = new (8, false) BulletBillObject(
                GetRenderObject(1, i), i, lbl_806E5210, lbl_806E5214);
            mUnidentified058[i] = pObject;
            mUnidentified054 = i + 1;
            break;
        }
    }
    return pObject;
}

UnidentifiedNPC_801B43F8* NPCManager::fn_801A9DE0(int nIndex)
{
    return mUnidentified0CC[nIndex];
}

void NPCManager::fn_801A9DF0()
{
    for (int i = 0; i < 3; ++i)
    {
        UnidentifiedNPCConfig_801B532C* pConfig = fn_801B532C(i);
        NPCTemplate* pTemplate
            = fn_801ABBDC_inline(pConfig->mName);

        PhysicsNPC* pPhysics = new (8, false) PhysicsNPC(
            pConfig->mUnidentified008);
        UnidentifiedNPC_801B43F8* pObject = new (8, false) UnidentifiedNPC_801B43F8(
            *pTemplate->hierarchy, pTemplate->modelID,
            pConfig->mUnidentified00C, pConfig->mUnidentified010,
            *pPhysics, &pTemplate->mInventorySAnim,
            pTemplate->mResourcePool);
        mUnidentified0CC[i] = pObject;
        pPhysics->SetCallbackFunction(UnidentifiedNPC_801B43F8::fn_801B4830);
    }
}

void NPCManager::CreateDiddyBanana()
{
    NPCTemplate* pTemplate
        = fn_801ABBDC_inline(sDiddyBananaTemplateName);
    DiddyBanana* pObject
        = (DiddyBanana*)nlMalloc(0x84, 8, false);
    pObject = new (pObject) DiddyBanana(
        *pTemplate->hierarchy, pTemplate->modelID, pTemplate->mInventorySAnim, pTemplate->mResourcePool);
    mpDiddyBanana = pObject;
}

void NPCManager::fn_801AA2C0()
{
    for (unsigned int i = 0; i < 15; ++i)
    {
        HammerObject* pObject
            = (HammerObject*)nlMalloc(sizeof(HammerObject), 8, false);
        if (pObject != 0)
        {
            pObject = fn_801A0E64(pObject, i, lbl_806DD000);
        }
        mUnidentified070[i] = pObject;
    }
}

int NPCManager::fn_801AA32C()
{
    return mUnidentified070[0] == 0 ? 0 : 15;
}

void NPCManager::fn_801AA348()
{
    for (int i = 0; i < 15; ++i)
    {
        HammerObject* pObject = mUnidentified070[i];
        if (pObject != 0 && pObject->_024)
        {
            fn_801A1CFC(pObject, 1);
        }
    }
}

HammerObject* NPCManager::fn_801AA3AC(int nIndex)
{
    if (nIndex >= 0)
    {
        return mUnidentified070[nIndex];
    }

    for (int i = 0; i < 15; ++i)
    {
        if (mUnidentified070[i] != 0 && !mUnidentified070[i]->_024)
        {
            return mUnidentified070[i];
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
    ((cInventory<cSAnim>*)pUserData)->AddFile((char*)pData, nSize);
}

void OnNPCHierarchyLoaded(
    void* pData, unsigned long nSize, void* pUserData)
{
    gNPCManager->mPendingTemplate->mHierarchyLoaded = true;
    ((cInventory<cSHierarchy>*)pUserData)->AddFile((char*)pData, nSize);
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
    nlSNPrintf(path, sizeof(path), sNPCAnimationPath, mPendingTemplate->mName, mPendingTemplate->mName);
    if (nlLoadCompressedFileAsync(path, OnNPCAnimationsLoaded, &mPendingTemplate->mInventorySAnim, 0x20, AllocateStart, 0x40000, 0, 0, 0, 0, &StandardAllocator))
    {
        mPendingTemplate->mAnimationLoadStarted = true;
    }

    nlSNPrintf(path, sizeof(path), sNPCHierarchyPath, mPendingTemplate->mName, mPendingTemplate->mName);
    cInventory<cSHierarchy>* pInventory = mPendingTemplate->mPersistent
                                            ? mPersistentHierarchies
                                            : mTransientHierarchies;
    nlLoadEntireFileAsync(path, OnNPCHierarchyLoaded, pInventory, 0x20, AllocateStart, 0, 0, &StandardAllocator);

    nlSNPrintf(path, sizeof(path), sNPCTexturePath, mPendingTemplate->mName, mPendingTemplate->mName);
    glBeginLoadTextureBundle(path, OnNPCTexturesLoaded, mPendingTemplate, pContext);

    nlSNPrintf(path, sizeof(path), sNPCModelPath, mPendingTemplate->mName, mPendingTemplate->mName);
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
    nlDLListIterator<NPCTemplate*> iterator
        = mTransientTemplates.Begin();
    while (iterator.hasNext())
    {
        delete *iterator;
        iterator.next();
    }
    mTransientTemplates.Clear();
    mTransientHierarchies->Clear();
}

NPCManager::~NPCManager()
{
    nlDLListContainer<NPCTemplate*>* pLists[2]
        = { &mPersistentTemplates, &mTransientTemplates };
    for (int i = 0; i < 2; ++i)
    {
        nlDLListIterator<NPCTemplate*> iterator
            = pLists[i]->Begin();
        while (iterator.hasNext())
        {
            delete *iterator;
            iterator.next();
        }
        pLists[i]->Clear();
    }

    delete mpChainChomp;
    mpChainChomp = 0;
    delete mpDiddyBanana;
    mpDiddyBanana = 0;

    if (mUnidentified024 != 0)
    {
        delete mUnidentified024;
        mUnidentified024 = 0;
    }
    if (mpBirdoEgg != 0)
    {
        delete mpBirdoEgg;
        mpBirdoEgg = 0;
    }
    if (mUnidentified02C != 0)
    {
        delete mUnidentified02C;
        mUnidentified02C = 0;
    }

    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        delete mDaisyFists[i];
        mDaisyFists[i] = 0;
    }
    for (i = 0; i < 6; ++i)
    {
        if (mUnidentified058[i] != 0)
        {
            delete mUnidentified058[i];
            mUnidentified058[i] = 0;
        }
    }
    mUnidentified054 = 0;
    for (i = 0; i < 15; ++i)
    {
        if (mUnidentified070[i] != 0)
        {
            fn_801A10D0(mUnidentified070[i], 1);
            mUnidentified070[i] = 0;
        }
    }
    for (i = 0; i < 3; ++i)
    {
        delete mUnidentified0CC[i];
        mUnidentified0CC[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        if (mThwomps[i] != 0)
        {
            delete mThwomps[i];
            mThwomps[i] = 0;
        }
    }

    fn_801A01F8();
    delete mPersistentHierarchies;
    delete mTransientHierarchies;
    gNPCManagerInstance = 0;
}

void NPCManager::DestroyNPCs()
{
    delete mpChainChomp;
    mpChainChomp = 0;
    delete mpDiddyBanana;
    mpDiddyBanana = 0;

    if (mUnidentified024 != 0)
    {
        delete mUnidentified024;
        mUnidentified024 = 0;
    }
    if (mpBirdoEgg != 0)
    {
        delete mpBirdoEgg;
        mpBirdoEgg = 0;
    }
    if (mUnidentified02C != 0)
    {
        delete mUnidentified02C;
        mUnidentified02C = 0;
    }

    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        delete mDaisyFists[i];
        mDaisyFists[i] = 0;
    }
    for (i = 0; i < 6; ++i)
    {
        if (mUnidentified058[i] != 0)
        {
            delete mUnidentified058[i];
            mUnidentified058[i] = 0;
        }
    }
    mUnidentified054 = 0;
    for (i = 0; i < 15; ++i)
    {
        if (mUnidentified070[i] != 0)
        {
            fn_801A10D0(mUnidentified070[i], 1);
            mUnidentified070[i] = 0;
        }
    }
    for (i = 0; i < 3; ++i)
    {
        delete mUnidentified0CC[i];
        mUnidentified0CC[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        if (mThwomps[i] != 0)
        {
            delete mThwomps[i];
            mThwomps[i] = 0;
        }
    }
    fn_801A01F8();
}

NPCTemplate* NPCManager::fn_801ABBDC(const char* pName)
{
    for (int i = 0; i < 2; ++i)
    {
        nlDLListIterator<NPCTemplate*> iterator
            = i == 0 ? mPersistentTemplates.Begin()
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
    if (mUnidentified024 != 0)
    {
        mUnidentified024->Update(dt);
    }
    if (mpBirdoEgg != 0)
    {
        mpBirdoEgg->Update(dt);
    }
    if (mUnidentified02C != 0)
    {
        mUnidentified02C->Update(dt);
    }
    if (mpDiddyBanana != 0)
    {
        mpDiddyBanana->Update(dt);
    }

    mUnidentified030 = 0;
    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        DaisyFistObject* pObject = mDaisyFists[i];
        if (pObject != 0)
        {
            pObject->Update(dt);
            if (pObject->mVisible)
            {
                ++mUnidentified030;
            }
        }
    }
    for (i = 0; i < mUnidentified054; ++i)
    {
        mUnidentified058[i]->Update(dt);
    }
    for (i = 0; i < 15; ++i)
    {
        if (mUnidentified070[i] != 0)
        {
            fn_801A16A4(mUnidentified070[i], dt);
        }
    }
    for (i = 0; i < 3; ++i)
    {
        if (mUnidentified0CC[i] != 0)
        {
            mUnidentified0CC[i]->Update(dt);
        }
    }
    for (i = 0; i < 8; ++i)
    {
        if (mThwomps[i] != 0)
        {
            mThwomps[i]->Update(dt);
        }
    }
    fn_801A0208(dt);
}

void NPCManager::fn_801ABF8C()
{
    if (mpChainChomp != 0)
    {
        mpChainChomp->Hide();
    }
    if (mUnidentified024 != 0)
    {
        mUnidentified024->Reset();
    }
    if (mpBirdoEgg != 0)
    {
        mpBirdoEgg->Reset();
    }
    if (mUnidentified02C != 0)
    {
        mUnidentified02C->Reset();
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
        mUnidentified030 = 0;
    }
    for (i = 0; i < mUnidentified054; ++i)
    {
        mUnidentified058[i]->Reset();
    }
    for (i = 0; i < 15; ++i)
    {
        if (mUnidentified070[i] != 0)
        {
            fn_801A1CFC(mUnidentified070[i], 0);
        }
    }
    for (i = 0; i < 3; ++i)
    {
        if (mUnidentified0CC[i] != 0)
        {
            mUnidentified0CC[i]->fn_801B4B9C();
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
