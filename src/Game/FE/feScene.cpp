#include "Game/FE/feScene.h"

#include "Game/FE/fePackage.h"
#include "Game/FE/feSceneManager.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glMatrix.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlRing.h"

#include <string.h>

struct FE_FILE_HEADER
{
    char Thumbprint[4];
    unsigned int Version;
    unsigned int DataLength;
    unsigned int PointerTableLength;
};

class QueueResourceLoadCallback
{
public:
    void Callback(FEResourceHandle* handle);

    FEResourceManager* m_resourceManager;
    MemoryAllocator* m_pAllocator;
};

class UnloadResourceCallback
{
public:
    void Callback(FEResourceHandle* handle);

    FEResourceManager* m_resourceManager;
};

class Callback_802FEA20
{
public:
    void Callback(FEResourceHandle* handle);

    FEResourceManager* m_resourceManager;
};

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

extern "C" void fn_802FF644(FESceneManager* pSceneManager, FEScene* pFEScene);

static inline void PushAllocator(MemoryAllocator* pAllocator)
{
    CurrentAllocator = pAllocator;
    AllocatorStack[AllocatorStackDepth++] = pAllocator;
}

static inline void PopAllocator()
{
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

static inline void RelocatePointer(unsigned long* pPointer, void* pData)
{
    unsigned long value = *pPointer;
    if (value == 0xFFFFFFFF)
    {
        value = 0;
    }
    else
    {
        value += (unsigned long)pData;
    }
    *pPointer = value;
}

FEScene::FEScene()
    : m_pFEPackage(0)
    , m_uHashID(0)
    , m_uRenderView(0)
    , field_0x4C(0)
    , mState(1)
    , field_0x78(0)
    , m_pAllocator(0)
{
    nlVector3 FROM;
    nlVec3Set(FROM, 0.0f, 0.0f, 600.0f);
    nlVector3 TO;
    nlVec3Set(TO, 0.0f, 0.0f, 0.0f);
    nlVector3 UP;
    nlVec3Set(UP, 0.0f, 1.0f, 0.0f);
    glMatrixLookAt(m_matView, FROM, TO, UP);

    field_0x4C = (FE_FILE_HEADER*)nlMalloc(sizeof(FE_FILE_HEADER), 0x20, false);
}

FEScene::~FEScene()
{
    ::operator delete(field_0x4C);

    if (m_pFEPackage != 0)
    {
        ::operator delete[](m_pFEPackage);
        m_pFEPackage = 0;
        m_uHashID = 0;
    }
}

extern "C" void fn_802FE6EC(void* pData, unsigned long uSize, void* pUserData);
extern "C" void fn_802FE730(FEScene* pFEScene, void* pData, unsigned long uSize);

bool FEScene::LoadPackage(const char* szPackageFileName, MemoryAllocator* pAllocator)
{
    nlLoadEntireFileAsync(szPackageFileName, fn_802FE6EC, this, 0x20, AllocateStart, 0, 0, pAllocator);
    return true;
}

extern "C" void fn_802FE6EC(void* pData, unsigned long uSize, void* pUserData)
{
    FEScene* pFEScene = (FEScene*)pUserData;
    fn_802FE730(pFEScene, pData, uSize);
    ::operator delete[](pData);
}

extern "C" void fn_802FE730(FEScene* pFEScene, void* pData, unsigned long)
{
    nlFile* file;
    unsigned char* pFileData;
    unsigned long* pCurrentPointer;
    unsigned long* pLastPointer;
    unsigned long* pPointer;
    void* pPackageData;

    if (pFEScene->m_pAllocator != 0)
    {
        PushAllocator(pFEScene->m_pAllocator);
    }

    memcpy(pFEScene->field_0x4C, pData, sizeof(FE_FILE_HEADER));

    pFEScene->m_pFEPackage = (FEPackage*)nlMalloc(pFEScene->field_0x4C->DataLength, 0x20, false);
    pFileData = (unsigned char*)pData + sizeof(FE_FILE_HEADER);
    memcpy(pFEScene->m_pFEPackage, pFileData, pFEScene->field_0x4C->DataLength);

    pFEScene->field_0x50 = (unsigned long*)nlMalloc(pFEScene->field_0x4C->PointerTableLength, 0x20, true);
    memcpy(
        pFEScene->field_0x50,
        pFileData + pFEScene->field_0x4C->DataLength,
        pFEScene->field_0x4C->PointerTableLength);

    pCurrentPointer = pFEScene->field_0x50;
    pLastPointer = (unsigned long*)((unsigned char*)pCurrentPointer + (pFEScene->field_0x4C->PointerTableLength & ~3));
    pPackageData = pFEScene->m_pFEPackage;
    for (; pCurrentPointer < pLastPointer; ++pCurrentPointer)
    {
        pPointer = (unsigned long*)((unsigned char*)pPackageData + *pCurrentPointer);
        RelocatePointer(pPointer, pPackageData);
    }

    nlFree(pFEScene->field_0x50);
    pFEScene->field_0x50 = 0;
    pFEScene->mState = 5;

    QueueResourceLoadCallback cb;
    file = (nlFile*)pFEScene->m_pFEPackage;
    cb.m_pAllocator = pFEScene->m_pAllocator;
    cb.m_resourceManager = FEResourceManager::Instance();

    pFEScene->m_feSceneResourceHandle.m_pFESceneContext = pFEScene;
    pFEScene->m_feSceneResourceHandle.m_hashID = pFEScene->m_uHashID;
    pFEScene->m_feSceneResourceHandle.m_next = 0;
    pFEScene->m_feSceneResourceHandle.m_prev = 0;
    pFEScene->m_feSceneResourceHandle.m_type = FERT_SCENE;
    FEResourceManager::Instance()->QueueResourceLoad(&pFEScene->m_feSceneResourceHandle, 0);

    nlWalkRing<FEResourceHandle, QueueResourceLoadCallback>(
        ((FEPackage*)file)->m_pResourceList, &cb, &QueueResourceLoadCallback::Callback);

    fn_802FF644(FESceneManager::Instance(), pFEScene);

    if (pFEScene->m_pAllocator != 0)
    {
        PopAllocator();
    }
}

void FEScene::UnloadPackage()
{
    UnloadResourceCallback unloadResourceCallback;
    unloadResourceCallback.m_resourceManager = FEResourceManager::Instance();
    nlWalkRing<FEResourceHandle, UnloadResourceCallback>(
        m_pFEPackage->m_pResourceList,
        &unloadResourceCallback,
        &UnloadResourceCallback::Callback);
    FEResourceManager::Instance()->UnloadResource(&m_feSceneResourceHandle);
}

void UnloadResourceCallback::Callback(FEResourceHandle* handle)
{
    m_resourceManager->UnloadResource(handle);
}

void QueueResourceLoadCallback::Callback(FEResourceHandle* handle)
{
    m_resourceManager->QueueResourceLoad(handle, m_pAllocator);
}

void FEScene::AllResourcesLoadedCallback()
{
}

extern "C" void fn_802FEA20(FEScene* pFEScene)
{
    Callback_802FEA20 callback;
    callback.m_resourceManager = FEResourceManager::Instance();
    nlWalkRing<FEResourceHandle, Callback_802FEA20>(
        pFEScene->field_0x78, &callback, &Callback_802FEA20::Callback);
    pFEScene->field_0x78 = 0;
}

void Callback_802FEA20::Callback(FEResourceHandle* handle)
{
    m_resourceManager->UnloadResource(handle);
    ::operator delete(handle);
}

void FEScene::Update(float dt)
{
    m_pFEPackage->Update(dt);
}
