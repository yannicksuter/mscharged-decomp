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
    QueueResourceLoadCallback(FEResourceManager* resourceManager, MemoryAllocator* pAllocator)
        : m_resourceManager(resourceManager)
        , m_pAllocator(pAllocator)
    {
    }

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

class ReleaseResourceCallback
{
public:
    void Callback(FEResourceHandle* handle);

    FEResourceManager* m_resourceManager;
};


extern "C" void InitializeScene(FESceneManager* pSceneManager, FEScene* pFEScene);

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
    nlVector3 FROM;
    nlVec3Set(FROM, 0.0f, 0.0f, 600.0f);
    nlVector3 TO;
    nlVec3Set(TO, 0.0f, 0.0f, 0.0f);
    nlVector3 UP;
    nlVec3Set(UP, 0.0f, 1.0f, 0.0f);
    glMatrixLookAt(m_matView, FROM, TO, UP);

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

void FEScene::LoadPackage(void* pData, unsigned long)
{
    nlFile* file;
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

    file = (nlFile*)m_pFEPackage;
    QueueResourceLoadCallback cb(FEResourceManager::Instance(), m_pAllocator);

    m_feSceneResourceHandle.m_pFESceneContext = this;
    m_feSceneResourceHandle.m_hashID = m_uHashID;
    m_feSceneResourceHandle.m_next = 0;
    m_feSceneResourceHandle.m_prev = 0;
    m_feSceneResourceHandle.m_type = FERT_SCENE;
    FEResourceManager::Instance()->QueueResourceLoad(&m_feSceneResourceHandle, 0);

    nlWalkRing<FEResourceHandle, QueueResourceLoadCallback>(
        ((FEPackage*)file)->m_pResourceList, &cb, &QueueResourceLoadCallback::Callback);

    FESceneManager::Instance()->InitializeScene(this);

    if (m_pAllocator != 0)
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

void FEScene::ReleaseResourceHandles()
{
    ReleaseResourceCallback callback;
    callback.m_resourceManager = FEResourceManager::Instance();
    nlWalkRing<FEResourceHandle, ReleaseResourceCallback>(
        m_pResourceHandles, &callback, &ReleaseResourceCallback::Callback);
    m_pResourceHandles = 0;
}

void ReleaseResourceCallback::Callback(FEResourceHandle* handle)
{
    m_resourceManager->UnloadResource(handle);
    ::operator delete(handle);
}

void FEScene::Update(float dt)
{
    m_pFEPackage->Update(dt);
}
