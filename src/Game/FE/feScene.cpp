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
    QueueResourceLoadCallback(FEResourceManager* pResourceManager, MemoryAllocator* pAllocator)
        : m_pResourceManager(pResourceManager)
        , m_pAllocator(pAllocator)
    {
    }

    void Callback(FEResourceHandle* pFeResourceHandle);

    FEResourceManager* m_pResourceManager;
    MemoryAllocator* m_pAllocator;
};

class UnloadResourceCallback
{
public:
    void Callback(FEResourceHandle* pFeResourceHandle);

    FEResourceManager* m_pResourceManager;
};

class ReleaseResourceCallback
{
public:
    void Callback(FEResourceHandle* pFeResourceHandle);

    FEResourceManager* m_pResourceManager;
};

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
    , m_pFileHeader(0)
    , mState(1)
    , m_pResourceHandles(0)
    , m_pAllocator(0)
{
    nlVector3 v3Eye;
    nlVec3Set(v3Eye, 0.0f, 0.0f, 600.0f);
    nlVector3 v3At;
    nlVec3Set(v3At, 0.0f, 0.0f, 0.0f);
    nlVector3 v3Up;
    nlVec3Set(v3Up, 0.0f, 1.0f, 0.0f);
    glMatrixLookAt(m_matView, v3Eye, v3At, v3Up);

    m_pFileHeader = (FE_FILE_HEADER*)nlMalloc(sizeof(FE_FILE_HEADER), 0x20, false);
}

FEScene::~FEScene()
{
    ::operator delete(m_pFileHeader);

    if (m_pFEPackage != 0)
    {
        ::operator delete[](m_pFEPackage);
        m_pFEPackage = 0;
        m_uHashID = 0;
    }
}

bool FEScene::LoadPackage(const char* szPackageFileName, MemoryAllocator* pAllocator)
{
    nlLoadEntireFileAsync(szPackageFileName, FEScene::LoadPackageCallback, this, 0x20, AllocateStart, 0, 0, pAllocator);
    return true;
}

void FEScene::LoadPackageCallback(void* pData, unsigned long uSize, void* pUserData)
{
    FEScene* pFEScene = (FEScene*)pUserData;
    pFEScene->LoadPackage(pData, uSize);
    ::operator delete[](pData);
}

inline void FEScene::LoadPackageResources(FEPackage* pFEPackage)
{
    QueueResourceLoadCallback cb(FEResourceManager::Instance(), m_pAllocator);

    m_feSceneResourceHandle.m_pFESceneContext = this;
    m_feSceneResourceHandle.m_hashID = m_uHashID;
    m_feSceneResourceHandle.m_next = 0;
    m_feSceneResourceHandle.m_prev = 0;
    m_feSceneResourceHandle.m_type = FERT_SCENE;
    FEResourceManager::Instance()->QueueResourceLoad(&m_feSceneResourceHandle, 0);

    nlWalkRing<FEResourceHandle, QueueResourceLoadCallback>(
        pFEPackage->m_pResourceList, &cb, &QueueResourceLoadCallback::Callback);
}

void FEScene::LoadPackage(void* pData, unsigned long)
{
    unsigned char* pFileData;
    unsigned long* pCurrentPointer;
    unsigned long* pLastPointer;
    unsigned long* pPointer;
    void* pPackageData;

    if (m_pAllocator != 0)
    {
        PushAllocator(m_pAllocator);
    }

    memcpy(m_pFileHeader, pData, sizeof(FE_FILE_HEADER));

    m_pFEPackage = (FEPackage*)nlMalloc(m_pFileHeader->DataLength, 0x20, false);
    pFileData = (unsigned char*)pData + sizeof(FE_FILE_HEADER);
    memcpy(m_pFEPackage, pFileData, m_pFileHeader->DataLength);

    m_pPointerTable = (unsigned long*)nlMalloc(m_pFileHeader->PointerTableLength, 0x20, true);
    memcpy(
        m_pPointerTable,
        pFileData + m_pFileHeader->DataLength,
        m_pFileHeader->PointerTableLength);

    pCurrentPointer = m_pPointerTable;
    pLastPointer = (unsigned long*)((unsigned char*)pCurrentPointer + (m_pFileHeader->PointerTableLength & ~3));
    pPackageData = m_pFEPackage;
    for (; pCurrentPointer < pLastPointer; ++pCurrentPointer)
    {
        pPointer = (unsigned long*)((unsigned char*)pPackageData + *pCurrentPointer);
        RelocatePointer(pPointer, pPackageData);
    }

    nlFree(m_pPointerTable);
    m_pPointerTable = 0;
    mState = 5;

    LoadPackageResources(m_pFEPackage);

    FESceneManager::Instance()->InitializeScene(this);

    if (m_pAllocator != 0)
    {
        PopAllocator();
    }
}

void FEScene::UnloadPackage()
{
    UnloadResourceCallback cb;
    cb.m_pResourceManager = FEResourceManager::Instance();
    nlWalkRing<FEResourceHandle, UnloadResourceCallback>(
        m_pFEPackage->m_pResourceList,
        &cb,
        &UnloadResourceCallback::Callback);
    FEResourceManager::Instance()->UnloadResource(&m_feSceneResourceHandle);
}

void UnloadResourceCallback::Callback(FEResourceHandle* pFeResourceHandle)
{
    m_pResourceManager->UnloadResource(pFeResourceHandle);
}

void QueueResourceLoadCallback::Callback(FEResourceHandle* pFeResourceHandle)
{
    m_pResourceManager->QueueResourceLoad(pFeResourceHandle, m_pAllocator);
}

void FEScene::AllResourcesLoadedCallback()
{
}

void FEScene::ReleaseResourceHandles()
{
    ReleaseResourceCallback callback;
    callback.m_pResourceManager = FEResourceManager::Instance();
    nlWalkRing<FEResourceHandle, ReleaseResourceCallback>(
        m_pResourceHandles, &callback, &ReleaseResourceCallback::Callback);
    m_pResourceHandles = 0;
}

void ReleaseResourceCallback::Callback(FEResourceHandle* pFeResourceHandle)
{
    m_pResourceManager->UnloadResource(pFeResourceHandle);
    ::operator delete(pFeResourceHandle);
}

void FEScene::Update(float fDeltaT)
{
    m_pFEPackage->Update(fDeltaT);
}
