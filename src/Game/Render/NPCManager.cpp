#include "Game/Render/NPCManager.h"

#include "Game/Drawable/Drawable_8017FFCC.h"
#include "Game/GameTweaks.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "unclassified/tu_801A0E64.h"
#include "unclassified/tu_80199880.h"
#include "unclassified/tu_801B298C.h"

#include <string.h>

struct UnidentifiedNPCConfig_801B532C
{
    u8 mUnidentified000[4];
    const char* mName;
    float mUnidentified008;
    void* mUnidentified00C;
    void* mUnidentified010;
};

extern "C"
{
    void* fn_8019AE7C(void* pObject, cSHierarchy* pHierarchy, int nModel,
        void* pPhysics, cInventory<cSAnim>* pInventory, void* pResource);
    void fn_8019BF40(
        PhysicsObject*, PhysicsObject*, const nlVector3&);

    UnidentifiedObject_801B535C* fn_801B535C(
        UnidentifiedObject_801B535C* pObject, void* pDrawable);
    UnidentifiedObject_801B535C* fn_801B543C(
        UnidentifiedObject_801B535C* pObject, int bDelete);
    void fn_801B5544(UnidentifiedObject_801B535C* pObject, float fDeltaT);
    void fn_801B5D14(UnidentifiedObject_801B535C* pObject);

    State_80199E84* fn_80199E84(State_80199E84* pObject, void* pDrawable);
    State_80199E84* fn_80199F6C(State_80199E84* pObject, int bDelete);
    void fn_80199FDC(State_80199E84* pObject, float fDeltaT);
    void fn_8019A584(State_80199E84* pObject);

    KoopaShellObject* fn_801A5F30(
        KoopaShellObject* pObject, void* pDrawable);
    KoopaShellObject* fn_801A6004(
        KoopaShellObject* pObject, int bDelete);
    void fn_801A6074(KoopaShellObject* pObject, float fDeltaT);
    void fn_801A65F8(KoopaShellObject* pObject);

    State_8019A710* fn_8019A710(State_8019A710* pObject,
        void* pDrawable, unsigned int nIndex, float fRadius, float fParam);
    State_8019A710* fn_8019A7E4(State_8019A710* pObject, int bDelete);
    void fn_8019A854(State_8019A710* pObject, float fDeltaT);
    void fn_8019AD18(State_8019A710* pObject);

    UnidentifiedNPCConfig_801B532C* fn_801B532C(int* pIndex);
    SkinAnimatedNPC* fn_801B43F8(SkinAnimatedNPC* pObject,
        cSHierarchy* pHierarchy, int nModel, void* pParam1, void* pParam2,
        void* pPhysics, cInventory<cSAnim>* pInventory, void* pResource);
    void fn_801B4830(
        PhysicsObject*, PhysicsObject*, const nlVector3&);
    void fn_801B4B9C(SkinAnimatedNPC* pObject);

    void* fn_80276360(int nType, int nIndex);
    void fn_801A01F8();
    void fn_801A0208(float fDeltaT);

    void* fn_80118A74(NPCTemplate* pTemplate);
    void* fn_8011B850(void* pObject);
    bool fn_802B3E94(const char* pPath, LoadAsyncCallback pCallback,
        void* pUserData, unsigned int nAlignment, int nAllocType,
        unsigned int nUncompressedSize, void* pBuffer,
        unsigned long nBufferSize, void* pParam, unsigned long nParam,
        MemoryAllocator* pAllocator);
    bool fn_802C8200(const char* pPath, LoadAsyncCallback pCallback,
        void* pUserData, void* pContext);
    bool fn_802C8204(const char* pPath, LoadAsyncCallback pCallback,
        void* pUserData, void* pContext);
    MemoryAllocator* fn_802CC094();
    bool fn_802CDD78(void* pData, unsigned long nSize,
        MemoryAllocator* pAllocator, int nParam);
    unsigned int* fn_802C81FC(void* pData, unsigned long nSize,
        unsigned long* pNumModels, void* pContext);
}

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;
int nlSNPrintf(char* pBuffer, unsigned long nSize, const char* pFormat, ...);

static char lbl_805142BC[] = "ChainChomp";
static char lbl_805142C8[] = "DiddyBanana";
static char lbl_805142D4[] = "art/animation/%s.sanim.zlib";
static char lbl_805142F0[] = "art/animation/%s.shier";
static char lbl_80514308[] = "art/characters/npcs/%s/%s.rlt";
static char lbl_80514328[] = "art/characters/npcs/%s/%s.rlg";

float lbl_806DD000 = 0.48f;
const float lbl_806E5210 = 0.45f;
const float lbl_806E5214 = 1.0f;

NPCManager* lbl_806E1608;
NPCManager* lbl_806E160C;

NPCManager::NPCManager()
    : mUnidentified004(0)
    , mUnidentified008(0)
    , mUnidentified01C(0)
    , mpChainChomp(0)
    , mUnidentified024(0)
    , mUnidentified028(0)
    , mUnidentified02C(0)
    , mUnidentified030(0)
    , mUnidentified054(0)
    , mUnidentified0D8(0)
{
    lbl_806E160C = this;
    mUnidentified004 = new (nlMalloc(
        sizeof(cInventory<cSHierarchy>), 8, false)) cInventory<cSHierarchy>();
    mUnidentified008 = new (nlMalloc(
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
        mUnidentified034[i] = 0;
    }
    for (i = 0; i < 3; ++i)
    {
        mUnidentified0CC[i] = 0;
    }
    for (i = 0; i < 8; ++i)
    {
        mUnidentified0AC[i] = 0;
    }
}

void NPCManager::CreateNPCTemplate(
    const char* pName, bool bType)
{
    NPCTemplate* pTemplate
        = new (nlMalloc(sizeof(NPCTemplate), 8, false))
            NPCTemplate(pName, bType);

    if (bType)
    {
        mUnidentified00C.AddEnd(pTemplate);
    }
    else
    {
        mUnidentified014.AddEnd(pTemplate);
    }
}

bool NPCManager::fn_801A977C()
{
    mUnidentified01C = 0;
    for (int i = 0; i < 2; ++i)
    {
        nlDLListIterator<NPCTemplate*> iterator
            = i == 0 ? mUnidentified00C.Begin()
                     : mUnidentified014.Begin();
        while (iterator.hasNext())
        {
            if (!(*iterator)->loaded)
            {
                mUnidentified01C = *iterator;
                return true;
            }
            iterator.next();
        }
    }
    return false;
}

void NPCManager::fn_801A9874()
{
    NPCTemplate* pTemplate
        = fn_801ABBDC_inline(lbl_805142BC);

    PhysicsNPC* chainPhysics = new (nlMalloc(
        sizeof(PhysicsNPC), 8, false)) PhysicsNPC(
        lbl_8056CF08.m_pGameTweaks->fChainChompRadius);

    void* chainChomp = nlMalloc(0xB4, 8, false);
    if (chainChomp != 0)
    {
        chainChomp = fn_8019AE7C(chainChomp,
            pTemplate->hierarchy,
            pTemplate->modelID,
            chainPhysics,
            &pTemplate->mUnidentified014,
            pTemplate->mUnidentified010);
    }
    mpChainChomp = (ChainChomp*)chainChomp;
    chainPhysics->SetCallbackFunction(fn_8019BF40);
}

void NPCManager::fn_801A9AF8()
{
    UnidentifiedObject_801B535C* pObject
        = (UnidentifiedObject_801B535C*)nlMalloc(0x4C, 8, false);
    if (pObject != 0)
    {
        pObject = fn_801B535C(pObject, fn_80276360(3, 0));
    }
    mUnidentified024 = pObject;
}

void NPCManager::fn_801A9B64()
{
    State_80199E84* pObject = (State_80199E84*)nlMalloc(0x4C, 8, false);
    if (pObject != 0)
    {
        pObject = fn_80199E84(pObject, fn_80276360(4, 0));
    }
    mUnidentified028 = pObject;
}

void NPCManager::fn_801A9BD0()
{
    KoopaShellObject* pObject
        = (KoopaShellObject*)nlMalloc(0x3C, 8, false);
    if (pObject != 0)
    {
        pObject = fn_801A5F30(pObject, fn_80276360(5, 0));
    }
    mUnidentified02C = pObject;
}

void NPCManager::fn_801A9C3C()
{
    for (unsigned int i = 0; i < 8; ++i)
    {
        Object_8017FFF4* pObject
            = (Object_8017FFF4*)nlMalloc(sizeof(Object_8017FFF4), 8, false);
        pObject = new (pObject) Object_8017FFF4(i);
        mUnidentified034[i] = pObject;
    }
}

Object_8017FFF4* NPCManager::fn_801A9CA4(int nIndex)
{
    if (nIndex >= 0)
    {
        return mUnidentified034[nIndex];
    }

    for (unsigned int i = 0; i < 8; ++i)
    {
        Object_8017FFF4* pObject = mUnidentified034[i];
        if (pObject != 0 && !pObject->visible)
        {
            mUnidentified030 = 8;
            return mUnidentified034[i];
        }
    }
    return 0;
}

State_8019A710* NPCManager::fn_801A9D10(int nIndex)
{
    return mUnidentified058[nIndex];
}

State_8019A710* NPCManager::fn_801A9D20()
{
    State_8019A710* pObject = 0;
    for (unsigned int i = 0; i < 6; ++i)
    {
        if (mUnidentified058[i] == 0)
        {
            pObject = (State_8019A710*)nlMalloc(0x48, 8, false);
            if (pObject != 0)
            {
                pObject = fn_8019A710(pObject, fn_80276360(1, i), i, lbl_806E5210, lbl_806E5214);
            }
            mUnidentified058[i] = pObject;
            mUnidentified054 = i + 1;
            break;
        }
    }
    return pObject;
}

SkinAnimatedNPC* NPCManager::fn_801A9DE0(int nIndex)
{
    return mUnidentified0CC[nIndex];
}

void NPCManager::fn_801A9DF0()
{
    for (int i = 0; i < 3; ++i)
    {
        UnidentifiedNPCConfig_801B532C* pConfig = fn_801B532C(&i);
        NPCTemplate* pTemplate
            = fn_801ABBDC_inline(pConfig->mName);

        PhysicsNPC* pPhysics
            = (PhysicsNPC*)nlMalloc(sizeof(PhysicsNPC), 8, false);
        if (pPhysics != 0)
        {
            pPhysics = new (pPhysics) PhysicsNPC(
                pConfig->mUnidentified008);
        }

        SkinAnimatedNPC* pObject
            = (SkinAnimatedNPC*)nlMalloc(0x98, 8, false);
        if (pObject != 0)
        {
            pObject = fn_801B43F8(pObject, pTemplate->hierarchy, pTemplate->modelID, pConfig->mUnidentified00C, pConfig->mUnidentified010, pPhysics, &pTemplate->mUnidentified014, pTemplate->mUnidentified010);
        }
        mUnidentified0CC[i] = pObject;
        pPhysics->SetCallbackFunction(fn_801B4830);
    }
}

void NPCManager::fn_801AA088()
{
    NPCTemplate* pTemplate
        = fn_801ABBDC_inline(lbl_805142C8);
    UnidentifiedSkinAnimatedNPC_80199880* pObject
        = (UnidentifiedSkinAnimatedNPC_80199880*)nlMalloc(0x84, 8, false);
    pObject = new (pObject) UnidentifiedSkinAnimatedNPC_80199880(
        *pTemplate->hierarchy, pTemplate->modelID, pTemplate->mUnidentified014, pTemplate->mUnidentified010);
    mUnidentified0D8 = pObject;
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
        HammerObject* pObject = mUnidentified070[i];
        if (pObject != 0 && !pObject->_024)
        {
            return mUnidentified070[i];
        }
    }
    return 0;
}

void NPCManager::fn_801AA4C0()
{
    for (unsigned int i = 0; i < 8; ++i)
    {
        ThwompObject* pObject
            = (ThwompObject*)nlMalloc(sizeof(ThwompObject), 8, false);
        if (pObject != 0)
        {
            pObject = fn_801B298C(pObject, i);
        }
        mUnidentified0AC[i] = pObject;
    }
}

ThwompObject* NPCManager::fn_801AA528(
    int nIndex)
{
    if (nIndex >= 0)
    {
        return mUnidentified0AC[nIndex];
    }

    for (int i = 0; i < 8; ++i)
    {
        ThwompObject* pObject = mUnidentified0AC[i];
        if (pObject != 0 && !pObject->mVisible)
        {
            return pObject;
        }
    }
    return 0;
}

extern "C" void fn_801AA648(
    void* pData, unsigned long nSize, void* pUserData)
{
    lbl_806E1608->mUnidentified01C->mUnidentified001 = true;
    ((cInventory<cSAnim>*)pUserData)->AddFile((char*)pData, nSize);
}

extern "C" void fn_801AA794(
    void* pData, unsigned long nSize, void* pUserData)
{
    lbl_806E1608->mUnidentified01C->mUnidentified002 = true;
    ((cInventory<cSHierarchy>*)pUserData)->AddFile((char*)pData, nSize);
}

extern "C" void fn_801AA8E0(
    void* pData, unsigned long nSize, void* pUserData)
{
    NPCManager* pManager = lbl_806E1608;
    NPCTemplate* pTemplate
        = (NPCTemplate*)pUserData;
    pTemplate->mUnidentified010 = fn_802CC094();
    pManager->mUnidentified01C->mUnidentified003 = true;
    fn_802CDD78(pData, nSize, fn_802CC094(), 0);
    nlFree(pData);
}

extern "C" void fn_801AA960(
    void* pData, unsigned long nSize, void* pUserData)
{
    NPCTemplate* pTemplate
        = (NPCTemplate*)pUserData;
    pTemplate->mUnidentified010 = fn_802CC094();
    unsigned long nNumModels = 0;
    unsigned int* pModel = fn_802C81FC(
        pData, nSize, &nNumModels, fn_802CC094());
    pTemplate->modelID = *pModel;
    nlFree(pData);
}

void NPCManager::fn_801AA9D8()
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    void* pContext;
    if (mUnidentified01C->mUnidentified005)
    {
        pContext = fn_8011B850(fn_80118A74(mUnidentified01C));
    }
    else
    {
        pContext = fn_802CC094();
    }

    char path[256];
    nlSNPrintf(path, sizeof(path), lbl_805142D4, mUnidentified01C->mName, mUnidentified01C->mName);
    if (fn_802B3E94(path, fn_801AA648, &mUnidentified01C->mUnidentified014, 0x20, 0, 0x40000, 0, 0, 0, 0, &StandardAllocator))
    {
        mUnidentified01C->mUnidentified000 = true;
    }

    nlSNPrintf(path, sizeof(path), lbl_805142F0, mUnidentified01C->mName, mUnidentified01C->mName);
    cInventory<cSHierarchy>* pInventory = mUnidentified01C->mUnidentified005
                                            ? mUnidentified004
                                            : mUnidentified008;
    nlLoadEntireFileAsync(path, fn_801AA794, pInventory, 0x20, AllocateStart, 0, 0, &StandardAllocator);

    nlSNPrintf(path, sizeof(path), lbl_80514308, mUnidentified01C->mName, mUnidentified01C->mName);
    fn_802C8204(path, fn_801AA8E0, mUnidentified01C, pContext);

    nlSNPrintf(path, sizeof(path), lbl_80514328, mUnidentified01C->mName, mUnidentified01C->mName);
    fn_802C8200(path, fn_801AA960, mUnidentified01C, pContext);
}

bool NPCManager::fn_801AABB0()
{
    if (mUnidentified01C->mUnidentified000
        && !mUnidentified01C->mUnidentified001)
    {
        return false;
    }
    if (!mUnidentified01C->mUnidentified002)
    {
        return false;
    }
    if (!mUnidentified01C->mUnidentified003)
    {
        return false;
    }
    if (mUnidentified01C->modelID == -1)
    {
        return false;
    }

    if (mUnidentified01C->mUnidentified005)
    {
        mUnidentified01C->hierarchy = mUnidentified004->Find(
            nlStringLowerHash(mUnidentified01C->mName));
    }
    else
    {
        mUnidentified01C->hierarchy = mUnidentified008->Find(
            nlStringLowerHash(mUnidentified01C->mName));
    }
    mUnidentified01C->loaded = true;

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return true;
}

void NPCManager::fn_801AAD0C()
{
    nlDLListIterator<NPCTemplate*> iterator
        = mUnidentified014.Begin();
    while (iterator.hasNext())
    {
        delete *iterator;
        iterator.next();
    }
    mUnidentified014.Clear();
    mUnidentified008->Clear();
}

NPCManager::~NPCManager()
{
    nlDLListContainer<NPCTemplate*>* pLists[2]
        = { &mUnidentified00C, &mUnidentified014 };
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
    delete mUnidentified0D8;
    mUnidentified0D8 = 0;

    if (mUnidentified024 != 0)
    {
        fn_801B543C(mUnidentified024, 1);
        mUnidentified024 = 0;
    }
    if (mUnidentified028 != 0)
    {
        fn_80199F6C(mUnidentified028, 1);
        mUnidentified028 = 0;
    }
    if (mUnidentified02C != 0)
    {
        fn_801A6004(mUnidentified02C, 1);
        mUnidentified02C = 0;
    }

    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        delete mUnidentified034[i];
        mUnidentified034[i] = 0;
    }
    for (i = 0; i < 6; ++i)
    {
        if (mUnidentified058[i] != 0)
        {
            fn_8019A7E4(mUnidentified058[i], 1);
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
        if (mUnidentified0AC[i] != 0)
        {
            fn_801B2B60(mUnidentified0AC[i], 1);
            mUnidentified0AC[i] = 0;
        }
    }

    fn_801A01F8();
    delete mUnidentified004;
    delete mUnidentified008;
    lbl_806E160C = 0;
}

void NPCManager::fn_801AB9D4()
{
    delete mpChainChomp;
    mpChainChomp = 0;
    delete mUnidentified0D8;
    mUnidentified0D8 = 0;

    if (mUnidentified024 != 0)
    {
        fn_801B543C(mUnidentified024, 1);
        mUnidentified024 = 0;
    }
    if (mUnidentified028 != 0)
    {
        fn_80199F6C(mUnidentified028, 1);
        mUnidentified028 = 0;
    }
    if (mUnidentified02C != 0)
    {
        fn_801A6004(mUnidentified02C, 1);
        mUnidentified02C = 0;
    }

    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        delete mUnidentified034[i];
        mUnidentified034[i] = 0;
    }
    for (i = 0; i < 6; ++i)
    {
        if (mUnidentified058[i] != 0)
        {
            fn_8019A7E4(mUnidentified058[i], 1);
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
        if (mUnidentified0AC[i] != 0)
        {
            fn_801B2B60(mUnidentified0AC[i], 1);
            mUnidentified0AC[i] = 0;
        }
    }
    fn_801A01F8();
}

NPCTemplate* NPCManager::fn_801ABBDC(const char* pName)
{
    return fn_801ABBDC_inline(pName);
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
        fn_801B5544(mUnidentified024, dt);
    }
    if (mUnidentified028 != 0)
    {
        fn_80199FDC(mUnidentified028, dt);
    }
    if (mUnidentified02C != 0)
    {
        fn_801A6074(mUnidentified02C, dt);
    }
    if (mUnidentified0D8 != 0)
    {
        mUnidentified0D8->Update(dt);
    }

    mUnidentified030 = 0;
    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        Object_8017FFF4* pObject = mUnidentified034[i];
        if (pObject != 0)
        {
            pObject->fn_8019D778(dt);
            if (pObject->visible)
            {
                ++mUnidentified030;
            }
        }
    }
    for (i = 0; i < mUnidentified054; ++i)
    {
        fn_8019A854(mUnidentified058[i], dt);
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
        if (mUnidentified0AC[i] != 0)
        {
            fn_801B2C00(mUnidentified0AC[i], dt);
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
        fn_801B5D14(mUnidentified024);
    }
    if (mUnidentified028 != 0)
    {
        fn_8019A584(mUnidentified028);
    }
    if (mUnidentified02C != 0)
    {
        fn_801A65F8(mUnidentified02C);
    }
    if (mUnidentified0D8 != 0)
    {
        mUnidentified0D8->fn_80199A88();
    }

    mUnidentified030 = 0;
    unsigned int i;
    for (i = 0; i < 8; ++i)
    {
        if (mUnidentified034[i] != 0)
        {
            mUnidentified034[i]->fn_8019DA14();
        }
        mUnidentified030 = 0;
    }
    for (i = 0; i < mUnidentified054; ++i)
    {
        fn_8019AD18(mUnidentified058[i]);
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
            fn_801B4B9C(mUnidentified0CC[i]);
        }
    }
    for (i = 0; i < 8; ++i)
    {
        if (mUnidentified0AC[i] != 0)
        {
            fn_801B2E64(mUnidentified0AC[i], 1);
        }
    }
}
