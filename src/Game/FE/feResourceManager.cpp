#include "Game/FE/feResourceManager.h"

#include "Game/FE/feFontResource.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feSceneResource.h"
#include "Game/FE/feTextureResource.h"
#include "NL/MemAlloc.h"
#include "NL/nlAVLTree.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/gl/glTexture.h"

struct PendingResourceLoad
{
    FEResourceHandle* pHandle;
    MemoryAllocator* pAllocator;
};

class ResourceInterface_802CC094
{
public:
    virtual void fn_08();
    virtual unsigned long MarkResource();
    virtual void ReleaseResource(unsigned long marker);
};

struct PermanentBundleLoadState
{
    /* 0x00 */ bool bComplete;
    /* 0x04 */ BundleFile* pBundle;
    /* 0x08 */ unsigned long uFileHashID;
    /* 0x0C */ unsigned long uFileIndex;
    /* 0x10 */ void (*callback)();
    /* 0x14 */ unsigned long field_0x14;
};

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;
extern void* lbl_806E2090;
extern void nlPrintf(const char* format, ...);

extern "C" ResourceInterface_802CC094* fn_802CC094();
extern "C" void fn_802C8284(unsigned long texture);
extern "C" void fn_802C8288(void* texture);
extern "C" unsigned int fn_802A95C4(AVLTreeNode* node, unsigned int count);
extern "C" void* fn_80307260(void* manager, unsigned long hashID);

static nlAVLTreeSlotPool<unsigned long, FEResourceHandle*, DefaultKeyCompare<unsigned long> > s_loadedResourceList(0x200, 0);
static unsigned char* s_pResourceLoadBuffer;
static nlDLListSlotPool<PendingResourceLoad> pendingResourceQueue(0x100, 0);
static FEResourceHandle* s_pCurrentResourceBeingLoaded;
FESceneResource* s_pCurrentFESceneResourceContext;
static ResourceInterface_802CC094* s_pResourceInterface;
static BundleFile* s_pPermanentBundle;
static BundleFile* s_pOnDemandBundle;
static FESceneResource* s_pPermanentBundleSceneResource;
static PermanentBundleLoadState s_permanentBundleLoadState;

template <>
FEResourceManager* nlSingleton<FEResourceManager>::s_pInstance = 0;

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

void FEResourceManager::AddResourceToResourceList(FEResourceHandle* pFEResourceHandle)
{
    s_loadedResourceList.Add(pFEResourceHandle->GetHashID(), pFEResourceHandle);
}

void FEResourceManager::RemoveResourceFromResourceList(FEResourceHandle* pFEResourceHandle)
{
    FEResourceHandle** pLoadedResourceHandle;
    if (!s_loadedResourceList.FindGet(pFEResourceHandle->GetHashID(), &pLoadedResourceHandle))
    {
        return;
    }
    if (*pLoadedResourceHandle != pFEResourceHandle)
    {
        return;
    }
    s_loadedResourceList.Remove(pFEResourceHandle->GetHashID());
}

FEResourceHandle* FEResourceManager::FindExistingResourceInResourceList(FEResourceHandle* pFEResourceHandle)
{
    FEResourceHandle** pPreExistingResourceHandle;
    if (!s_loadedResourceList.FindGet(pFEResourceHandle->GetHashID(), &pPreExistingResourceHandle))
    {
        return 0;
    }
    return *pPreExistingResourceHandle;
}

ResourceResult FEResourceManager::IssueSceneContextSwitch(FESceneResource* pFeSceneResource)
{
    if (s_pCurrentFESceneResourceContext != 0
        && s_pCurrentFESceneResourceContext != pFeSceneResource
        && s_pPermanentBundleSceneResource != s_pCurrentFESceneResourceContext)
    {
        s_pCurrentFESceneResourceContext->m_pFESceneContext->mState = 6;
        s_pCurrentFESceneResourceContext->m_pFESceneContext->AllResourcesLoadedCallback();
    }

    pFeSceneResource->m_glResourceMarker = s_pResourceInterface->MarkResource();
    pFeSceneResource->m_bValid = true;
    s_pCurrentFESceneResourceContext = pFeSceneResource;
    return FERR_WaitingForResource;
}

FEResourceManager::FEResourceManager()
    : m_bPermanentBundleLoadInProgress(false)
{
    s_pResourceInterface = fn_802CC094();
}

void FEResourceManager::fn_802FC9C4(void* buffer, unsigned long uReadSize, unsigned long uParam)
{
    PermanentBundleLoadState* state = (PermanentBundleLoadState*)uParam;
    FETextureResource* pTextureResource = new (8, false) FETextureResource();
    pTextureResource->m_hashID = state->uFileHashID;
    s_pResourceLoadBuffer = (unsigned char*)buffer;
    ResourceInterface_802CC094* resourceInterface = s_pResourceInterface;
    fn_802C8284(pTextureResource->m_hashID);
    void* texture = glTextureAdd(pTextureResource->m_hashID, s_pResourceLoadBuffer, uReadSize, resourceInterface);
    fn_802C8288(texture);
    delete[] s_pResourceLoadBuffer;
    s_pResourceLoadBuffer = 0;
    unsigned long textureHandle = pTextureResource->m_hashID;
    pTextureResource->fn_8030009C(textureHandle);
    FEResourceManager::Instance()->AddResourceToResourceList(pTextureResource);
    pTextureResource->m_bValid = true;
    fn_802FC858(state);
}

int FEResourceManager::fn_802FC858(PermanentBundleLoadState* state)
{
    BundleFileDirectoryEntry fileDirectoryEntry;
    FEResourceHandle** pPreExistingResourceHandle;

    while (state->uFileIndex < state->pBundle->nNumFiles)
    {
        if (!state->pBundle->GetFileInfoByIndex(state->uFileIndex, &fileDirectoryEntry))
        {
            nlPrintf("FEResourceManager Error: Failed to get file information in permanent bundle (async)\n");
            return false;
        }

        if (s_loadedResourceList.FindGet(fileDirectoryEntry.m_hash, &pPreExistingResourceHandle))
        {
            nlPrintf("FEResourceManager: Skipping over existing resource.\n");
            ++state->uFileIndex;
            continue;
        }

        state->uFileHashID = fileDirectoryEntry.m_hash;
        unsigned char* buffer = (unsigned char*)nlMalloc(fileDirectoryEntry.m_length, 0x20, true);
        unsigned long fileIndex = state->uFileIndex++;
        state->pBundle->ReadFileAsyncByIndex(fileIndex, buffer, fileDirectoryEntry.m_length, fn_802FC9C4, (unsigned long)state);
        return true;
    }

    state->pBundle->Close();
    delete state->pBundle;
    state->bComplete = true;
    state->pBundle = 0;
    FEResourceManager::Instance()->m_bPermanentBundleLoadInProgress = false;
    if (state->callback != 0)
    {
        state->callback();
    }
    return false;
}

void FEResourceManager::fn_802FC850(void*, unsigned long, unsigned long uParam)
{
    fn_802FC858((PermanentBundleLoadState*)uParam);
}

void FEResourceManager::Cleanup()
{
    if (s_pOnDemandBundle != 0)
    {
        s_pOnDemandBundle->Close();
        delete s_pOnDemandBundle;
        s_pOnDemandBundle = 0;
    }

    if (fn_802A95C4((AVLTreeNode*)s_loadedResourceList.m_Root, 0) != 0)
    {
        nlPrintf("FEResourceManager: Warning! Manager being destroyed while resources are still loaded!\n");
        nlPrintf("                   Did all the scenes get popped before destroying the FEResourceManager?\n");

        typedef nlAVLTreeIterator<unsigned long, FEResourceHandle*, DefaultKeyCompare<unsigned long> > ResourceIterator;
        ResourceIterator* iterator = s_loadedResourceList.GetIterator();
        while (iterator->IsValid())
        {
            FEResourceHandle* pFeResourceHandle = iterator->Current()->value;
            nlPrintf(
                "                   Outstanding resource 0x%08x ( type = 0x%08x ) for load\n",
                pFeResourceHandle->m_hashID,
                pFeResourceHandle->m_type);
            iterator->Next();
        }
        delete iterator;
    }

    s_loadedResourceList.Clear();
}

void FEResourceManager::LoadPermanentResourceBundle(const char* szBundleFileName)
{
    nlStrNCpy(m_szPermanentBundleFileName, szBundleFileName, 0x20);

    s_pPermanentBundleSceneResource = new (8, false) FESceneResource();
    s_pPermanentBundleSceneResource->m_pFESceneContext = 0;
    s_pPermanentBundleSceneResource->m_hashID = nlStringLowerHash("PermanentContext");
    s_pPermanentBundleSceneResource->m_next = 0;
    s_pPermanentBundleSceneResource->m_prev = 0;
    s_pPermanentBundleSceneResource->m_type = FERT_SCENE;
    IssueSceneContextSwitch(s_pPermanentBundleSceneResource);
    LoadPermanentTextures();
}

void FEResourceManager::LoadPermanentResourceBundle(const char* szBundleFileName, void (*callback)())
{
    nlStrNCpy(m_szPermanentBundleFileName, szBundleFileName, 0x20);

    s_pPermanentBundleSceneResource = new (8, false) FESceneResource();
    s_pPermanentBundleSceneResource->m_pFESceneContext = 0;
    s_pPermanentBundleSceneResource->m_hashID = nlStringLowerHash("PermanentContext");
    s_pPermanentBundleSceneResource->m_next = 0;
    s_pPermanentBundleSceneResource->m_prev = 0;
    s_pPermanentBundleSceneResource->m_type = FERT_SCENE;
    IssueSceneContextSwitch(s_pPermanentBundleSceneResource);

    s_pPermanentBundle = new (0x20, true) BundleFile();
    s_permanentBundleLoadState.pBundle = 0;
    s_permanentBundleLoadState.uFileHashID = -1;
    s_permanentBundleLoadState.uFileIndex = 0;
    s_permanentBundleLoadState.bComplete = false;
    s_permanentBundleLoadState.callback = callback;
    s_permanentBundleLoadState.pBundle = s_pPermanentBundle;
    s_pPermanentBundle->OpenAsync(szBundleFileName, fn_802FC850, (unsigned long)&s_permanentBundleLoadState, false);
}

void FEResourceManager::LoadPermanentTextures()
{
    s_pPermanentBundle = new (8, false) BundleFile();
    s_pPermanentBundle->Open(m_szPermanentBundleFileName, false);

    unsigned long uFileLength;
    unsigned long uFileHashID;
    BundleFileDirectoryEntry fileDirectoryEntry;
    unsigned long i;
    FEResourceHandle** pPreExistingResourceHandle;
    unsigned long fileCount = s_pPermanentBundle->nNumFiles;

    for (i = 0; i < fileCount; ++i)
    {
        if (!s_pPermanentBundle->GetFileInfoByIndex(i, &fileDirectoryEntry))
        {
            nlPrintf("FEResourceManager Error: Failed to get file information in permanent bundle!\n");
        }
        else
        {
            uFileLength = fileDirectoryEntry.m_length;
            uFileHashID = fileDirectoryEntry.m_hash;
            if (s_loadedResourceList.FindGet(uFileHashID, &pPreExistingResourceHandle))
            {
                nlPrintf("FEResourceManager: Skipping over existing resource.\n");
            }
            else
            {
                FETextureResource* pTextureResource = new (8, false) FETextureResource();
                pTextureResource->m_hashID = uFileHashID;

                s_pResourceLoadBuffer = (unsigned char*)nlMalloc(uFileLength, 0x20, true);
                s_pPermanentBundle->ReadFileByIndex(i, s_pResourceLoadBuffer, uFileLength);
                ResourceInterface_802CC094* resourceInterface = s_pResourceInterface;
                fn_802C8284(pTextureResource->m_hashID);
                void* texture = glTextureAdd(pTextureResource->m_hashID, s_pResourceLoadBuffer, uFileLength, resourceInterface);
                fn_802C8288(texture);
                delete[] s_pResourceLoadBuffer;
                s_pResourceLoadBuffer = 0;
                unsigned long textureHandle = pTextureResource->m_hashID;
                pTextureResource->fn_8030009C(textureHandle);
                AddResourceToResourceList(pTextureResource);
                pTextureResource->m_bValid = true;
                delete[] s_pResourceLoadBuffer;
                s_pResourceLoadBuffer = 0;
                pTextureResource->m_bValid = true;
            }
        }
    }

    s_pPermanentBundle->Close();
    delete s_pPermanentBundle;
    s_pPermanentBundle = 0;
}

bool FEResourceManager::OpenOnDemandResourceBundle(const char* szBundleFileName, FileOpenAsyncCallback callback)
{
    nlStrNCpy(m_szOnDemandBundleFileName, szBundleFileName, 0x20);
    s_pOnDemandBundle = new (0x20, false) BundleFile();
    if (!s_pOnDemandBundle->OpenAsync(szBundleFileName, callback, 0, false))
    {
        return false;
    }
    return true;
}

void FEResourceManager::fn_802FD26C(void* resourceInterface)
{
    s_pResourceInterface = (ResourceInterface_802CC094*)resourceInterface;
}

void FEResourceManager::QueueResourceLoad(FEResourceHandle* pHandle, MemoryAllocator* pAllocator)
{
    if (pAllocator == 0)
    {
        pAllocator = CurrentAllocator;
    }

    PendingResourceLoad pendingResource = { pHandle, pAllocator };
    if (s_pOnDemandBundle != 0 && pHandle->m_type != FERT_SCENE && pendingResourceQueue.m_Head != 0)
    {
        BundleFileDirectoryEntry fileDirectoryEntry;
        if (s_pOnDemandBundle->GetFileInfo(pHandle->m_hashID, &fileDirectoryEntry, false))
        {
            pHandle->field_0x14 = fileDirectoryEntry.m_blockNumber;
            nlDLListIterator<PendingResourceLoad> insertAfter(
                pendingResourceQueue.m_Head, nlDLRingGetEnd(pendingResourceQueue.m_Head));
            if (pHandle->field_0x14 > (*insertAfter).pHandle->field_0x14)
            {
                pendingResourceQueue.AddEnd(pendingResource);
                return;
            }
            else
            {
                while ((*insertAfter).pHandle->m_type != FERT_SCENE
                       && pHandle->field_0x14 < (*insertAfter).pHandle->field_0x14)
                {
                    if (nlDLRingIsStart(insertAfter.m_Head, insertAfter.m_Curr))
                    {
                        insertAfter.m_Curr = 0;
                        break;
                    }
                    insertAfter.m_Curr = insertAfter.m_Curr->m_prev;
                }

                DLListEntry<PendingResourceLoad>* entry = pendingResourceQueue.Allocate(pendingResource);
                nlDLRingInsert(&pendingResourceQueue.m_Head, insertAfter.CurrentEntry(), entry);
                return;
            }
        }
        else
        {
            pHandle->field_0x14 = -1;
            pendingResourceQueue.AddEnd(pendingResource);
            return;
        }
    }
    pendingResourceQueue.AddEnd(pendingResource);
}

void FEResourceManager::UnloadResource(FEResourceHandle* pFeResourceHandle)
{
    switch (pFeResourceHandle->m_type)
    {
    case FERT_TEXTURE:
        RemoveResourceFromResourceList(pFeResourceHandle);
        break;
    case FERT_SCENE:
        s_pResourceInterface->ReleaseResource(((FESceneResource*)pFeResourceHandle)->m_glResourceMarker);
        break;
    default:
        break;
    }
}

void FEResourceManager::UnloadPermanentResourceBundle()
{
    s_pPermanentBundle = new (8, false) BundleFile();
    s_pPermanentBundle->Open(m_szPermanentBundleFileName, false);

    unsigned long fileCount = s_pPermanentBundle->nNumFiles;
    FEResourceHandle** pLoadedResourceHandle;
    BundleFileDirectoryEntry fileDirectoryEntry;

    UnloadResource(s_pPermanentBundleSceneResource);
    for (unsigned long fileIndex = 0; fileIndex < fileCount; ++fileIndex)
    {
        s_pPermanentBundle->GetFileInfoByIndex(fileIndex, &fileDirectoryEntry);
        if (s_loadedResourceList.FindGet(fileDirectoryEntry.m_hash, &pLoadedResourceHandle))
        {
            unsigned long hashToDelete = (*pLoadedResourceHandle)->GetHashID();
            delete *pLoadedResourceHandle;
            s_loadedResourceList.Remove(hashToDelete);
        }
    }

    s_pPermanentBundle->Close();
    delete s_pPermanentBundle;
    s_pPermanentBundle = 0;
    delete s_pPermanentBundleSceneResource;
    s_pPermanentBundleSceneResource = 0;
}

void FEResourceManager::TextureResourceLoadComplete(void*, unsigned long uReadSize, unsigned long uParam)
{
    FETextureResource* pHandle = (FETextureResource*)uParam;
    ResourceInterface_802CC094* resourceInterface = s_pResourceInterface;
    fn_802C8284(pHandle->m_hashID);
    void* texture = glTextureAdd(pHandle->m_hashID, s_pResourceLoadBuffer, uReadSize, resourceInterface);
    fn_802C8288(texture);
    delete[] s_pResourceLoadBuffer;
    s_pResourceLoadBuffer = 0;
    unsigned long textureHandle = pHandle->m_hashID;
    pHandle->fn_8030009C(textureHandle);
    FEResourceManager::Instance()->AddResourceToResourceList(pHandle);
    pHandle->m_bValid = true;
}

void FEResourceManager::Update(float)
{
    ResourceResult result;
    bool bQueueNextResource = true;
    while (bQueueNextResource)
    {
        if (s_pCurrentResourceBeingLoaded != 0 && !s_pCurrentResourceBeingLoaded->IsValid())
        {
            return;
        }

        if (s_pCurrentResourceBeingLoaded != 0)
        {
            pendingResourceQueue.RemoveStart(0);
            s_pCurrentResourceBeingLoaded = 0;
            if (pendingResourceQueue.m_Head == 0 && s_pCurrentFESceneResourceContext != 0)
            {
                s_pCurrentFESceneResourceContext->m_pFESceneContext->mState = 6;
                s_pCurrentFESceneResourceContext->m_pFESceneContext->AllResourcesLoadedCallback();
                s_pCurrentFESceneResourceContext = 0;
            }
        }

        if (pendingResourceQueue.m_Head == 0)
        {
            return;
        }

        PendingResourceLoad pendingResource = pendingResourceQueue.Begin().m_Curr->entry;
        s_pCurrentResourceBeingLoaded = pendingResource.pHandle;
        result = FERR_WaitingForResource;
        switch (s_pCurrentResourceBeingLoaded->m_type)
        {
        case FERT_TEXTURE:
            result = IssueTextureLoadRequest((FETextureResource*)s_pCurrentResourceBeingLoaded, pendingResource.pAllocator);
            break;
        case FERT_SCENE:
            result = IssueSceneContextSwitch((FESceneResource*)s_pCurrentResourceBeingLoaded);
            break;
        case FERT_FONT:
            ((FEFontResource*)pendingResource.pHandle)
                ->SetFontReference((nlFont*)fn_80307260(lbl_806E2090, pendingResource.pHandle->m_hashID));
            pendingResource.pHandle->m_bValid = true;
            result = FERR_AlreadyLoaded;
            break;
        default:
            break;
        }
        bQueueNextResource = result == FERR_AlreadyLoaded;
    }
}

ResourceResult FEResourceManager::IssueTextureLoadRequest(FETextureResource* pFeTextureResource, MemoryAllocator* pAllocator)
{
    FETextureResource* pFeExistingTextureResource = (FETextureResource*)FindExistingResourceInResourceList(pFeTextureResource);
    if (pFeExistingTextureResource != 0
        && pFeExistingTextureResource->GetResourceType() == pFeTextureResource->GetResourceType())
    {
        unsigned long textureHandle = pFeExistingTextureResource->GetTextureHandle();
        pFeTextureResource->fn_8030009C(textureHandle);
        pFeTextureResource->m_bValid = pFeExistingTextureResource->m_bValid;
        return FERR_AlreadyLoaded;
    }

    BundleFileDirectoryEntry fileDirectoryEntry;
    if (s_pOnDemandBundle->GetFileInfo(pFeTextureResource->m_hashID, &fileDirectoryEntry, true))
    {
        if (pAllocator != 0)
        {
            PushAllocator(pAllocator);
        }
        s_pResourceLoadBuffer = (unsigned char*)nlMalloc(fileDirectoryEntry.m_length, 0x20, true);
        s_pOnDemandBundle->ReadFileAsync(
            pFeTextureResource->m_hashID,
            s_pResourceLoadBuffer,
            fileDirectoryEntry.m_length,
            FEResourceManager::TextureResourceLoadComplete,
            (unsigned long)pFeTextureResource);
        if (pAllocator != 0)
        {
            PopAllocator();
        }
    }
    return FERR_WaitingForResource;
}

void* FEResourceManager::fn_802FDD84()
{
    return s_pResourceInterface;
}

FEMiniBundle* FEResourceManager::fn_802FDD8C(const char* szBundleFileName)
{
    FEMiniBundle* miniBundle = (FEMiniBundle*)nlMalloc(sizeof(FEMiniBundle), 8, false);
    nlStrNCpy(miniBundle->m_szBundleFileName, szBundleFileName, 0x20);
    miniBundle->m_pSceneResource = new (8, false) FESceneResource();
    miniBundle->m_pSceneResource->m_pFESceneContext = 0;
    miniBundle->m_pSceneResource->m_hashID = nlStringLowerHash("MiniBundleContext");
    miniBundle->m_pSceneResource->m_next = 0;
    miniBundle->m_pSceneResource->m_prev = 0;
    miniBundle->m_pSceneResource->m_type = FERT_SCENE;
    nlStrNCpy(m_szPermanentBundleFileName, szBundleFileName, 0x20);
    IssueSceneContextSwitch(miniBundle->m_pSceneResource);
    LoadPermanentTextures();
    s_pCurrentFESceneResourceContext = 0;
    return miniBundle;
}

bool FEResourceManager::fn_802FDF3C(FEMiniBundle* miniBundle)
{
    BundleFile* bundle = new (8, false) BundleFile();
    bundle->Open(miniBundle->m_szBundleFileName, false);
    unsigned long fileCount = bundle->nNumFiles;
    FEResourceHandle** pLoadedResourceHandle;
    BundleFileDirectoryEntry fileDirectoryEntry;

    UnloadResource(miniBundle->m_pSceneResource);
    for (unsigned long fileIndex = 0; fileIndex < fileCount; ++fileIndex)
    {
        bundle->GetFileInfoByIndex(fileIndex, &fileDirectoryEntry);
        if (s_loadedResourceList.FindGet(fileDirectoryEntry.m_hash, &pLoadedResourceHandle))
        {
            unsigned long hashToDelete = (*pLoadedResourceHandle)->GetHashID();
            delete *pLoadedResourceHandle;
            s_loadedResourceList.Remove(hashToDelete);
        }
    }

    bundle->Close();
    delete bundle;
    delete miniBundle->m_pSceneResource;
    delete miniBundle;
    return true;
}
