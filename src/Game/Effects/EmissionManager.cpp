#include "Game/Effects/EmissionManager.h"

#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EffectsBundleData.h"
#include "Game/Inventory.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/Effects/ParticleSystem.h"
#include "Game/TweakValue.h"
#include "Game/TweakIntBindingInline.h"
#include "Game/Replay.h"
#include "Game/Sys/debug.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glResourceLoader.h"
#include "NL/gl/glTexture.h"
#include "NL/nlFile.h"
#include "NL/nlChunk.h"
#include "NL/nlCompressedFile.h"
#include "NL/nlMemory.h"
#include "NL/MemAlloc.h"
#include "NL/nlAVLTree.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebugViews.h"

class EffectsBundle
{
public:
    EffectsBundle()
        : m_next(0)
        , m_prev(0)
        , mData(0)
        , mInventory()
        , mExternallyOwnedData(false)
    {
    }

    ~EffectsBundle()
    {
        mInventory.Clear();
        if (mData != 0 && !mExternallyOwnedData)
        {
            ::operator delete(mData);
        }
    }

    inline void Load(nlChunk* bundle);

    /* 0x00 */ EffectsBundle* m_next;
    /* 0x04 */ EffectsBundle* m_prev;
    /* 0x08 */ void* mData;
    /* 0x0C */ cInventory<EffectsBundleData> mInventory;
    /* 0x28 */ bool mExternallyOwnedData;
};

struct LingerMessage
{
    char szMessage[256];
    int nLingers;
    int nParticles;
};

typedef nlAVLTree<unsigned long, LingerMessage*,
    DefaultKeyCompare<unsigned long> > LingerTree;
typedef nlAVLTreeIterator<unsigned long, LingerMessage*,
    DefaultKeyCompare<unsigned long> > LingerTreeIterator;

static int lbl_806E1FD8;
static int lbl_806E1FDC;
static LingerTree* lingerers;
static unsigned int sResourceIdCounter;
static const char* sDefaultResourceNames[2] = { "Default", "World" };

static nlAVLTree<unsigned long, EffectsGroup*,
    DefaultKeyCompare<unsigned long> > sEffectsGroups;

class EffectsBundleManager
{
public:
    EffectsBundleManager()
        : mDefaultBundles(0)
        , mAdditionalBundles(0)
        , mResourcePool(0)
    {
    }

    ~EffectsBundleManager();

    void Load(void* data, void* nonResidentData, GLResourcePool* context, int bundleType);
    inline void ClearAdditional();

    EffectsBundle* mDefaultBundles;
    EffectsBundle* mAdditionalBundles;
    GLResourcePool* mResourcePool;
};

EffectsBundleManager gEffectsBundleManager;
void* gEffectsData;
void* gEffectsNonResidentData;
void* lbl_806E1FF0;
void* gEffectsGeometryData;
void* gEffectsTextureData;
GLInventory* gEffectsModelInventory;

EffectsBundleManager::~EffectsBundleManager()
{
}

inline void EffectsBundleManager::ClearAdditional()
{
    while (mAdditionalBundles != 0)
    {
        EffectsBundle* bundle = mAdditionalBundles;
        nlDLRingRemove(&mAdditionalBundles, bundle);

        for (nlListIterator<EffectsBundleData*> iterator
                 = bundle->mInventory.Begin();
             iterator.IsValid(); iterator.Next())
        {
            EffectsBundleData* data = iterator.Current();
            for (int i = 0; i < data->mNumGroups; ++i)
            {
                unsigned long hash = data->GetGroup(i)->GetHashID();
                sEffectsGroups.Remove(hash);
            }
        }

        delete bundle;
    }
}

void OnEffectsGeometryLoaded(
    void* data, unsigned long size, void* userData);

static inline void RegisterBundleGroups(EffectsBundleData* data)
{
    for (int i = 0; i < data->mNumGroups; ++i)
    {
        EffectsGroup* group = data->GetGroup(i);
        if (sEffectsGroups.Add(group->GetHashID(), group) != 0)
        {
            sEffectsGroups.Remove(group->GetHashID());
            sEffectsGroups.Add(group->GetHashID(), group);
        }
    }
}

inline void EffectsBundle::Load(nlChunk* bundle)
{
    mInventory.ParseChunks(bundle, bundle->GetNextChunk());
    RegisterBundleGroups(mInventory.Find(0));
}

class EffectsBundleChunkLoader : public GLResourceChunkLoader
{
public:
    EffectsBundleChunkLoader(GLResourcePool* resourcePool, EffectsBundle* bundle)
        : GLResourceChunkLoader(resourcePool, 1)
        , mBundle(bundle)
    {
    }

    void Load(nlChunk* bundle)
    {
        nlChunk* end = bundle->GetLastChunk();
        nlChunk* chunk = bundle->GetFirstChunk();
        while (chunk != end)
        {
            LoadChunk(chunk);
            chunk = chunk->GetNextChunk();
        }
    }

    bool LoadChunk(nlChunk* chunk)
    {
        if (EffectsBundleData::IsValidChunkID(chunk->GetID()))
        {
            mBundle->Load(chunk);
            return true;
        }
        return GLResourceChunkLoader::LoadChunk(chunk);
    }

private:
    /* 0x0C */ EffectsBundle* mBundle;
};

/**
 * Offset/Address/Size: 0x40 | 0x802E5BA0 | size: 0x89C
 */
void EffectsBundleManager::Load(void* data, void* nonResidentData,
    GLResourcePool* context, int bundleType)
{
    EffectsBundle* bundle
        = new (nlMalloc(sizeof(EffectsBundle), 8, false)) EffectsBundle;
    EffectsBundleChunkLoader loader(context, bundle);

    if (bundleType == 2)
    {
        loader.LoadChunk((nlChunk*)data);
    }
    else
    {
        if (data != 0)
        {
            loader.Load((nlChunk*)data);
        }
        if (nonResidentData != 0)
        {
            loader.Load((nlChunk*)nonResidentData);
        }
    }

    bundle->mData = data;
    if (bundleType == 0)
    {
        nlDLRingAddEnd(&mDefaultBundles, bundle);
    }
    else
    {
        nlDLRingAddEnd(&mAdditionalBundles, bundle);
    }

    if (mResourcePool == 0)
    {
        mResourcePool = context;
    }
}


inline void EmissionResourceStats::Configure(const char* name, int budget)
{
    nlStrNCpy(mName, name, sizeof(mName));
    mBudget = budget;
    unknown_0x32_bit15 = mBudget != 0;
}

inline EmissionResourceStats::EmissionResourceStats()
    : mId(sResourceIdCounter++)
{
    unknown_0x32_bit15 = 0;
    unknown_0x32_bit14 = 0;
    mBudgetTweak = 0;
    mHighWaterMark = 0;
    mCount = 0;
    if (mId < 2)
    {
        Configure(sDefaultResourceNames[mId], 0xFFFF);
    }
}

/**
 * Offset/Address/Size: 0x8A0 | 0x802E643C | size: 0x8
 */
void OnEffectsDataLoaded(
    void* data, unsigned long size, void* userData)
{
    *(void**)userData = data;
}

/**
 * Offset/Address/Size: 0x8E4 | 0x802E6444 | size: 0x188
 */
void OnEffectsGeometryLoaded(void* data, unsigned long, void*)
{
    nlChunk* bundle = (nlChunk*)data;
    nlChunk* chunk = bundle->GetFirstChunk();
    while (chunk != bundle->GetLastChunk())
    {
        unsigned long numModels = 0;
        glEndLoadModel(chunk, chunk->GetDataSize(), &numModels,
            glGetCurrentResourcePool());
        chunk = chunk->GetNextChunk();
    }
}

/**
 * Offset/Address/Size: 0x710 | 0x802E65CC | size: 0x54
 */
void OnEffectsTexturesLoaded(void* data, unsigned long size, void* userData)
{
    glEndLoadTextureBundle(data, size, glGetCurrentResourcePool(), 0);
    nlFree(data);
}

/**
 * Offset/Address/Size: 0x6BC | 0x802E6620 | size: 0x154
 */
void EmissionManager::StartLoading(
    bool first, bool second, bool third, bool fourth)
{
    gEffectsData = 0;
    gEffectsNonResidentData = 0;
    lbl_806E1FF0 = 0;
    gEffectsGeometryData = 0;
    gEffectsTextureData = 0;

    nlLoadEntireFileAsync("art/effects/effects.bun", OnEffectsDataLoaded,
        &gEffectsData, 0x20,
        first ? AllocateStart : AllocateEnd, 0, 0, 0);

    if (fourth)
    {
        nlLoadCompressedFileAsync("art/effects/effectsNonRes.bun.zlib", OnEffectsDataLoaded,
            &gEffectsNonResidentData, 0x20,
            second ? AllocateStart : AllocateEnd, 0x40000,
            0, 0, 0, 0, 0);
    }
    else
    {
        nlLoadEntireFileAsync("art/effects/effectsNonRes.bun", OnEffectsDataLoaded,
            &gEffectsNonResidentData, 0x20,
            second ? AllocateStart : AllocateEnd, 0, 0, 0);
    }

    nlLoadEntireFileAsync("art/objects/effectsgeometry.bun", OnEffectsGeometryLoaded,
        &gEffectsGeometryData, 0x20,
        first ? AllocateStart : AllocateEnd, 0, 0, 0);
    nlLoadEntireFileAsync("art/objects/effectsgeometrytextures.rlt",
        OnEffectsTexturesLoaded, &gEffectsTextureData, 0x20,
        first ? AllocateStart : AllocateEnd, 0, 0, 0);

    gEffectsModelInventory = glGetCurrentResourcePool()->m_inventory;
}

/**
 * Offset/Address/Size: 0x568 | 0x802E6774 | size: 0x6C
 */
bool EmissionManager::FinishLoading(GLResourcePool* context)
{
    if (gEffectsData == 0)
    {
        return false;
    }
    if (gEffectsNonResidentData == 0)
    {
        return false;
    }

    gEffectsBundleManager.Load(gEffectsData, gEffectsNonResidentData,
        context, 0);
    nlFree(gEffectsNonResidentData);
    gEffectsNonResidentData = 0;
    return true;
}

/**
 * Offset/Address/Size: 0x504 | 0x802E67E0 | size: 0x64
 */
void EmissionManager::LoadBundle(void* data, void* nonResidentData,
    GLResourcePool* context, int bundleType)
{
    if (data != 0 || nonResidentData != 0)
    {
        gEffectsBundleManager.Load(
            data, nonResidentData, context, bundleType);
    }
    ::operator delete(nonResidentData);
}

/**
 * Offset/Address/Size: 0x3AC | 0x802E6844 | size: 0x158
 */
EmissionManager::EmissionManager()
    : mNextControllerId(1)
    , m_bRecording(true)
    , mContext(0)
    , unknown_0x1B0(false)
    , mReplayControllers()
    , mControllers()
    , mUnidentifiedControllers()
    , mParticleMemory(0)
    , mParticles()
    , mUpdateEnabled(false)
    , mRenderPersistentOnly(false)
    , mShadowHeight(0.0f)
    , unknown_0x1F8(true)
{
}

/**
 * Offset/Address/Size: 0x128 | 0x802E699C | size: 0x284
 */
EmissionManager::~EmissionManager()
{
    Shutdown();
}

static void AllocateParticles(EmissionManager* manager)
{
    int i;
    const int count = manager->mNumParticles;
    manager->mParticleMemory = (Particle*)nlMalloc(count * sizeof(Particle), 8, false);
    tDebugPrintManager::Print(DC_RENDER, "%dKB used by Particle pool\n",
        (manager->mNumParticles * sizeof(Particle)) >> 10);

    for (i = 0; i < manager->mNumParticles; ++i)
    {
        manager->mParticles.AddStart(&manager->mParticleMemory[i]);
    }
}

static void DeallocateParticles(EmissionManager* manager)
{
    manager->mParticles.Clear();
    if (manager->mParticleMemory != 0)
    {
        delete[] manager->mParticleMemory;
        manager->mParticleMemory = 0;
    }
}

static void InitializeResourceStats()
{
    EmissionResourceStats* stats = EmissionManager::Instance()->mResourceStats;
    for (unsigned int i = 0; i < 8; ++i)
    {
        stats[i].Initialize();
    }
}

static void DeinitializeResourceStats()
{
    EmissionResourceStats* stats = EmissionManager::Instance()->mResourceStats;
    for (unsigned int i = 0; i < 8; ++i)
    {
        delete stats[i].mCount;
        delete stats[i].mHighWaterMark;
        delete stats[i].mBudgetTweak;
        stats[i].mBudgetTweak = 0;
        stats[i].mHighWaterMark = 0;
        stats[i].mCount = 0;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E6C20 | size: 0x1D8
 */
void EmissionManager::Startup(void* context,
    int numParticles, int maxRenderedParticles)
{
    mContext = context;
    mNumParticles = numParticles;
    mMemoryContext = &VirtualAllocator;

    lingerers = new (nlMalloc(sizeof(LingerTree), 8, false)) LingerTree();

    AllocateParticles(this);

    fxParticleStartup(numParticles);
    fxSetMaxNumParticles(maxRenderedParticles);

    InitializeResourceStats();
    mUpdateEnabled = true;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E6DF8 | size: 0x7FC
 */
void EmissionManager::Shutdown()
{
    if (!mUpdateEnabled)
    {
        return;
    }

    if (mControllers.m_Head != 0)
    {
        tDebugPrintManager::Print(DC_RENDER,
            "EmissionManager being deleted non-empty\n");
    }

    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        delete current;
        iterator.Step();
    }
    mControllers.Clear();

    nlDLListIterator<char*> errorIterator
        = mUnidentifiedControllers.Begin();
    while (errorIterator.hasNext())
    {
        delete *errorIterator;
        errorIterator.Step();
    }
    mUnidentifiedControllers.Clear();

    if (lingerers != 0)
    {
        lingerers->DeleteValues();
    }
    delete lingerers;
    lingerers = 0;

    while (mReplayControllers.m_Head != 0)
    {
        EmissionController* current;
        mReplayControllers.RemoveStart(&current);
        delete current;
    }

    fxParticleShutdown();
    DeallocateParticles(this);

    MemoryAllocator* allocator = mMemoryContext;
    AllocatorStack[AllocatorStackDepth++] = allocator;
    CurrentAllocator = allocator;

    gEffectsBundleManager.ClearAdditional();

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    DeinitializeResourceStats();

    BasicSlotPool<DLListEntry<Particle*> >* particlePool = &mParticles.m_Allocator;
    mUpdateEnabled = false;
    m_bRecording = true;
    particlePool->FreeBlocks();
}

/**
 * Offset/Address/Size: 0x0 | 0x802E75F4 | size: 0x64
 */
EmissionManager* GetEmissionManager()
{
    return EmissionManager::Instance();
}

/**
 * Offset/Address/Size: 0x0 | 0x802E7658 | size: 0x64
 */
EmissionManager* EmissionManager::Instance()
{
    static EmissionManager instance;
    return &instance;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E76BC | size: 0x64
 */
EmissionManager& EmissionManager::InstanceForReplayOnly()
{
    return *Instance();
}

/**
 * Offset/Address/Size: 0x6C | 0x802E7720 | size: 0x370
 */
void EmissionManager::Update(float dt)
{
    if (!mUpdateEnabled)
    {
        return;
    }

    MemoryAllocator* allocator = mMemoryContext;
    AllocatorStack[AllocatorStackDepth++] = allocator;
    CurrentAllocator = allocator;
    lbl_806E1FD8 = 0;

    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* p = *iterator;
        if (p->Update(dt))
        {
            iterator.Step();
        }
        else
        {
            mControllers.RemoveEntry(iterator.next());
            delete p;
        }
    }

    if (lingerers != 0 && lingerers->m_Root != 0)
    {
        nlColour colour = { 0xFF, 0xFF, 0x40, 0xFF };
        LingerTreeIterator* iter;
        int y = 3;
        glFontBegin(false);

        iter = lingerers->GetIterator();
        while (iter->IsValid())
        {
            LingerTree::Entry* entry = iter->Current();
            LingerMessage* l = entry->value;
            glFontPrintf(GetDebugFontView(), 0, y, colour,
                "%s lingers (%d .. %d)",
                l->szMessage, l->nLingers, l->nParticles);
            iter->Next();
            ++y;
        }

        if (iter != 0)
        {
            delete iter;
        }
        glFontEnd();
        if (lingerers != 0)
        {
            lingerers->DeleteValues();
        }
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

static EffectsLight g_EffectsLights[3];
static int g_nNumLights;

/**
 * Offset/Address/Size: 0x0 | 0x802E7A90 | size: 0x8
 */
int EmissionManager::GetNumLights()
{
    return g_nNumLights;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E7A98 | size: 0x30
 */
EffectsLight* EmissionManager::GetLight(int index)
{
    if (index < 0 || index >= g_nNumLights)
    {
        return 0;
    }
    return &g_EffectsLights[index];
}

/**
 * Offset/Address/Size: 0x0 | 0x802E7AC8 | size: 0x68
 */
void EmissionManager::AddEffectsLight(const EffectsLight& light)
{
    if (g_nNumLights >= 3)
    {
        return;
    }
    g_EffectsLights[g_nNumLights++] = light;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E7B30 | size: 0x1A4
 */
void EmissionManager::Render()
{
    g_nNumLights = 0;
    gNumRenderedParticles = 0;

    int i;
    EmissionResourceStats* stats = Instance()->mResourceStats;
    for (i = 0; i < 8; ++stats, ++i)
    {
        stats->ResetCount();
    }

    int renderedParticles = 0;
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (!mRenderPersistentOnly
            || !current->m_pGroup->IsPersistent())
        {
            renderedParticles += current->Render();
        }
        iterator.Step();
    }
    lbl_806E1FDC = renderedParticles;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E7CD4 | size: 0x8
 */
nlDLListContainer<EmissionController*>* EmissionManager::GetContainer()
{
    return &mControllers;
}

EffectsGroup* EmissionManager::GetEffectsGroup(const char* name)
{
    unsigned long hash = nlStringLowerHash(name);
    EffectsGroup** group;
    if (sEffectsGroups.FindGet(hash, &group))
    {
        return *group;
    }
    return 0;
}

EffectsGroup* fxGetGroup(EmissionManager*, unsigned long hashID)
{
    EffectsGroup** group;
    if (!sEffectsGroups.FindGet(hashID, &group))
    {
        return 0;
    }
    return *group;
}

/**
 * Offset/Address/Size: 0x2264 | 0x802E7DC4 | size: 0x220
 */
EmissionController* EmissionManager::Create(
    const char* name, int view, bool addToEnd, int id)
{
    EffectsGroup* group = GetEffectsGroup(name);
    EmissionController* controller = 0;
    if (group != 0)
    {
        MemoryAllocator* allocator = mMemoryContext;
        AllocatorStack[AllocatorStackDepth++] = allocator;
        CurrentAllocator = allocator;

        if (id == 0)
        {
            id = (unsigned short)mNextControllerId++;
        }
        if (mNextControllerId > 0x7E16)
        {
            mNextControllerId = 1;
        }

        controller = new (nlMalloc(sizeof(EmissionController), 8, false))
            EmissionController(group, this, id, mContext, view);
        if (addToEnd)
        {
            mControllers.AddEnd(controller);
        }
        else
        {
            mControllers.AddStart(controller);
        }

        --AllocatorStackDepth;
        AllocatorStack[AllocatorStackDepth] = 0;
        CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    }
    return controller;
}

EmissionController* EmissionManager::Create(EffectsGroup* group, int view, bool addToEnd, unsigned short id)
{
    MemoryAllocator* allocator = mMemoryContext;
    AllocatorStack[AllocatorStackDepth++] = allocator;
    CurrentAllocator = allocator;

    if (id == 0)
    {
        id = mNextControllerId++;
    }
    if (mNextControllerId > 0x7E16)
    {
        mNextControllerId = 1;
    }

    EmissionController* controller = new (nlMalloc(sizeof(EmissionController), 8, false))
        EmissionController(group, this, id, mContext, view);
    if (addToEnd)
    {
        mControllers.AddEnd(controller);
    }
    else
    {
        mControllers.AddStart(controller);
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return controller;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E81A0 | size: 0xAC
 */
EmissionController* EmissionManager::FindController(unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        if ((pEffectsGroup == 0 || controller->m_pGroup == pEffectsGroup)
            && userData == controller->m_uUserData)
        {
            return controller;
        }
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
    return 0;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E824C | size: 0x98
 */
bool EmissionManager::IsStillAlive(EmissionController* controller)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        if (current->entry == controller)
        {
            return true;
        }
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
    return false;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E82E4 | size: 0xE0
 */
void EmissionManager::Kill(
    unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        if ((pEffectsGroup == 0 || controller->m_pGroup == pEffectsGroup)
            && userData == controller->m_uUserData)
        {
            controller->Die();
        }
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E83C4 | size: 0xC8
 */
void EmissionManager::Kill(const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup == 0)
    {
        return;
    }

    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (current->m_pGroup == pEffectsGroup)
        {
            current->Die();
        }
        iterator.next();
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E848C | size: 0xB8
 */
bool EmissionManager::IsPlaying(
    unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup != 0)
    {
        nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
        DLListEntry<EmissionController*>* head = iterator.m_Head;
        DLListEntry<EmissionController*>* current = iterator.m_Curr;
        while (current != 0)
        {
            EmissionController* controller = current->entry;
            if (controller->m_pGroup == pEffectsGroup
                && (userData == 0 || userData == controller->m_uUserData))
            {
                return true;
            }
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }
    }
    return false;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8544 | size: 0xB8
 */
bool EmissionManager::IsDying(unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup != 0)
    {
        nlDLListIterator<EmissionController*> iterator
            = mControllers.Begin();
        DLListEntry<EmissionController*>* head = iterator.m_Head;
        DLListEntry<EmissionController*>* current = iterator.m_Curr;
        while (current != 0)
        {
            EmissionController* controller = current->entry;
            if (controller->m_pGroup == pEffectsGroup
                && (userData == 0 || userData == controller->m_uUserData))
            {
                return controller->m_bDying;
            }
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }
    }
    return false;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E85FC | size: 0x174
 */
void EmissionManager::DestroyAll(int view, bool exceptPersistent)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        if (controller->m_pContext == mContext
            && controller->m_View == view
            && (!exceptPersistent
                || !controller->m_pGroup->IsPersistent()))
        {
            DLListEntry<EmissionController*>* entry = current;
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
            nlDLRingRemove(&mControllers.m_Head, entry);
            delete entry;
            controller->ClearParticles();
            delete controller;
        }
        else
        {
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8770 | size: 0x15C
 */
void EmissionManager::DestroyAll(bool exceptPersistent)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        if (controller->m_pContext == mContext
            && (!exceptPersistent
                || !controller->m_pGroup->IsPersistent()))
        {
            DLListEntry<EmissionController*>* entry = current;
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
            nlDLRingRemove(&mControllers.m_Head, entry);
            delete entry;
            delete controller;
        }
        else
        {
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E88CC | size: 0x160
 */
void EmissionManager::Destroy(
    unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        if ((pEffectsGroup == 0 || controller->m_pGroup == pEffectsGroup)
            && userData == controller->m_uUserData)
        {
            DLListEntry<EmissionController*>* entry = current;
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
            nlDLRingRemove(&mControllers.m_Head, entry);
            delete entry;
            controller->ClearParticles();
            delete controller;
        }
        else
        {
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8A2C | size: 0x14C
 */
void EmissionManager::Destroy(const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup == 0)
    {
        return;
    }

    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (current->m_pGroup == pEffectsGroup)
        {
            mControllers.Remove(&iterator);
            delete current;
        }
        else
        {
            iterator.next();
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8B78 | size: 0xE4
 */
void EmissionManager::ForEachController(
    const Function1<void, EmissionController&>& callback)
{
    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        callback(*controller);
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8C5C | size: 0x50
 */
void EmissionManager::AddError(const char* format, ...)
{
}

// Expanded only through Replayable<0>(LoadFrame&, EmissionController&) in this
// unit; retail keeps no standalone copy.
inline void EmissionController::Replay(LoadFrame& frame)
{
    ::Replayable<0>(frame, (unsigned int&)m_pPose);
    ::Replayable<0>(frame, (unsigned int&)m_pAnimController);
    frame.Replayable<0>(m_uUserData);
    ::Replayable<0>(frame, m_fGround);
    ::Replayable<0>(frame, m_aFacing);
    ::Replayable<0>(frame, m_View);
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 8>(m_vPosition.x));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 8>(m_vPosition.y));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 8>(m_vPosition.z));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vDirection.x));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vDirection.y));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vDirection.z));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vVelocity.x));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vVelocity.y));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vVelocity.z));

    m_Replaying = true;

    float age = 0.0f;
    ::Replayable<0>(frame, age);
    age += frame.fn_801948B0();
    m_ReplayDeltaTime = age - m_Age;
    m_Age = age;

    bool dying = false;
    ::Replayable<0>(frame, dying);
    if (dying == true)
    {
        Die();
    }

    unsigned int updateCallback = 0;
    ::Replayable<0>(frame, updateCallback);
    mUpdateCallback.Clear();
    if (updateCallback != 0)
    {
        mUpdateCallback = (void (*)(EmissionController&))updateCallback;
    }

    // Retail clears the position callback while restoring the finished one,
    // and the finished callback while restoring the position one.
    unsigned int finishedCallback = 0;
    ::Replayable<0>(frame, finishedCallback);
    mPositionCallback.Clear();
    if (finishedCallback != 0)
    {
        mFinishedCallback
            = (void (*)(EmissionController&, int))finishedCallback;
    }

    unsigned int positionCallback
        = (unsigned int)mPositionCallback.GetFreeFunction();
    ::Replayable<0>(frame, positionCallback);
    mFinishedCallback.Clear();
    if (positionCallback != 0)
    {
        mPositionCallback
            = (nlVector3 (*)(EmissionController&, EffectsSpec&))positionCallback;
    }
}

inline void EmissionController::Replay(SaveFrame& frame)
{
    ::Replayable<0>(frame, (unsigned int&)m_pPose);
    ::Replayable<0>(frame, (unsigned int&)m_pAnimController);
    frame.Replayable<0>(m_uUserData);
    ::Replayable<0>(frame, m_fGround);
    ::Replayable<0>(frame, m_aFacing);
    ::Replayable<0>(frame, m_View);
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 8>(m_vPosition.x));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 8>(m_vPosition.y));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 8>(m_vPosition.z));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vDirection.x));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vDirection.y));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vDirection.z));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vVelocity.x));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vVelocity.y));
    ::Replayable<0>(frame, FloatCompressor<-1024, 1024, 6>(m_vVelocity.z));

    m_Replaying = false;
    m_ReplayDeltaTime = 0.0f;
    ::Replayable<0>(frame, m_Age);
    ::Replayable<0>(frame, m_bDying);

    unsigned int updateCallback = (unsigned int)mUpdateCallback.GetFreeFunction();
    ::Replayable<0>(frame, updateCallback);

    unsigned int finishedCallback
        = (unsigned int)mFinishedCallback.GetFreeFunction();
    ::Replayable<0>(frame, finishedCallback);

    unsigned int positionCallback
        = (unsigned int)mPositionCallback.GetFreeFunction();
    ::Replayable<0>(frame, positionCallback);
}

/**
 * Offset/Address/Size: 0x314C | 0x802E8CAC | size: 0x210
 */
void EmissionManager::Replay(LoadFrame& frame)
{
    if (m_bRecording)
    {
        if (unknown_0x1B0)
        {
            DestroyAll(true);
        }
        else
        {
            PrepareForReplay();
        }
        m_bRecording = false;
    }

    int numEffects = 0;
    Replayable<0>(frame, numEffects);

    nlDLListContainer<EmissionController*> oldControllers(0);
    oldControllers.Copy(mControllers);
    mControllers.Clear();

    for (int i = 0; i < numEffects; ++i)
    {
        unsigned short id;
        unsigned int view;
        EffectsGroup* group = 0;
        Replayable<0>(frame, id);
        Replayable<0>(frame, view);
        Replayable<0>(frame, (unsigned int&)group);

        bool found = false;
        nlDLListIterator<EmissionController*> iterator
            = oldControllers.Begin();
        while (!iterator.IsDone())
        {
            EmissionController* controller = *iterator;
            if (id == controller->GetId())
            {
                Replayable<0>(frame, *controller);
                oldControllers.Remove(&iterator);
                mControllers.AddStart(controller);
                found = true;
                break;
            }
            iterator.Step();
        }

        if (!found)
        {
            EmissionController* controller
                = Create(group, view, true, id);
            Replayable<0>(frame, *controller);
        }
    }

    nlDLListIterator<EmissionController*> iterator
        = oldControllers.Begin();
    while (!iterator.IsDone())
    {
        EmissionController* controller = *iterator;
        if (controller->GetContext() != mContext)
        {
            mControllers.AddStart(controller);
        }
        else
        {
            delete controller;
        }
        iterator.Step();
    }
    oldControllers.Clear();
}

// Replay drops the controllers created while recording so the stashed ones
// can be restored.
static inline void DestroyReplayedControllers(EmissionManager* manager)
{
    nlDLListIterator<EmissionController*> iterator = manager->mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        if (controller->m_pContext == manager->mContext
            && !controller->m_pGroup->IsPersistent())
        {
            DLListEntry<EmissionController*>* entry = current;
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
            nlDLRingRemove(&manager->mControllers.m_Head, entry);
            delete entry;
            delete controller;
        }
        else
        {
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }
    }
}

// Replay restores the controllers stashed by PrepareForReplay and discards
// the ones created while replaying.
static void RestoreRecordedControllers(EmissionManager* manager)
{
    DLListEntry<EmissionController*>* head = manager->mReplayControllers.m_Head;
    manager->mReplayControllers.m_Head = manager->mControllers.m_Head;
    manager->mControllers.m_Head = head;

    while (manager->mReplayControllers.m_Head != 0)
    {
        EmissionController* controller;
        manager->mReplayControllers.RemoveStart(&controller);
        delete controller;
    }
}

/**
 * Offset/Address/Size: 0x335C | 0x802E8EBC | size: 0x794
 */
void EmissionManager::Replay(SaveFrame& frame)
{
    if (!m_bRecording)
    {
        if (unknown_0x1B0)
        {
            DestroyReplayedControllers(this);
        }
        else
        {
            RestoreRecordedControllers(this);
        }
        m_bRecording = true;
    }

    int numEffects = nlDLRingCountElements(mControllers.m_Head);
    Replayable<0>(frame, numEffects);

    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    while (!iterator.IsDone())
    {
        EmissionController* controller = *iterator;
        unsigned short id = controller->m_Id;
        unsigned int group = (unsigned int)controller->m_pGroup;
        Replayable<0>(frame, id);
        Replayable<0>(frame, controller->m_View);
        Replayable<0>(frame, group);
        Replayable<0>(frame, *controller);
        iterator.Step();
    }
}

static unsigned long fx_sTerrain;

/**
 * Offset/Address/Size: 0x0 | 0x802E9650 | size: 0x8
 */
u32 fxGetTerrain()
{
    return fx_sTerrain;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9658 | size: 0x8
 */
void fxSetTerrain(unsigned long terrainID)
{
    fx_sTerrain = terrainID;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9660 | size: 0x160
 */
void EmissionManager::KillOldest(int num, bool lingeringOnly)
{
    float prevBestAge;
    float currentBestAge = 0.0f;
    prevBestAge = currentBestAge;

    while (num > 0)
    {
        nlDLListIterator<EmissionController*> iterator;
        EmissionController* bestController;
        float bestAge;
        bestController = 0;
        bestAge = 0.0f;
        iterator = mControllers.Begin();

        while (iterator.hasNext())
        {
            EmissionController* current = *iterator;
            if ((!lingeringOnly || current->IsLingering())
                && (current->m_uUserData + 0x21530000 != 0x0000BEEF))
            {
                if (bestAge < current->m_Age
                    && (prevBestAge == currentBestAge
                        || current->m_Age < currentBestAge))
                {
                    bestAge = current->m_Age;
                    bestController = current;
                    currentBestAge = bestAge;
                }
            }
            iterator.Step();
        }

        if (bestController == 0)
        {
            break;
        }
        bestController->Die();
        --num;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E97C0 | size: 0xBC
 */
void EmissionManager::KillAll()
{
    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* controller = current->entry;
        controller->ClearParticles();
        controller->Die();
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E987C | size: 0xBC
 */
void EmissionManager::PrepareForReplay()
{
    while (mReplayControllers.m_Head != 0)
    {
        EmissionController* current;
        mReplayControllers.RemoveStart(&current);
        delete current;
    }

    DLListEntry<EmissionController*>* replayHead = mReplayControllers.m_Head;
    mReplayControllers.m_Head = mControllers.m_Head;
    mControllers.m_Head = replayHead;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9938 | size: 0x8C
 */
void EmissionManager::SetContext(void* context)
{
    mContext = context;
    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        current->entry->m_pContext = context;
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
}

/**
 * Offset/Address/Size: 0x3E64 | 0x802E99C4 | size: 0x448
 */
void EmissionResourceStats::Initialize()
{
    if (!unknown_0x32_bit15)
    {
        return;
    }

    mCount = new (gTweakBindingAllocator->Allocate(sizeof(TweakIntBinding)))
        TweakIntBinding;
    mHighWaterMark
        = new (gTweakBindingAllocator->Allocate(sizeof(TweakIntBinding)))
            TweakIntBinding;
    mBudgetTweak
        = new (gTweakBindingAllocator->Allocate(sizeof(TweakIntBinding)))
            TweakIntBinding;

    char category[32];
    char name[32];
    nlSNPrintf(category, sizeof(category), "Effects/Stats/%s", mName);
    char* suffix = name + nlSNPrintf(name, sizeof(name), "%s", mName);
    int suffixLength = sizeof(name) - (suffix - name);

    nlStrNCpy(suffix, " Count", suffixLength);
    mCount->BindWithDefault(name, 0, category, false, 0.0f, 0.0f, 0.0f);

    nlStrNCpy(suffix, " HWM", suffixLength);
    mHighWaterMark->BindWithDefault(
        name, 0, category, false, 0.0f, 0.0f, 0.0f);

    nlStrNCpy(suffix, " Budget", suffixLength);
    mBudgetTweak->BindWithDefault(
        name, 0, category, false, 0.0f, 0.0f, 0.0f);
    *mBudgetTweak = mBudget;
    unknown_0x32_bit14 = 1;
}

inline void EmissionResourceStats::ResetCount()
{
    if (unknown_0x32_bit14)
    {
        *mCount = 0;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9E0C | size: 0x90
 */
void EmissionManager::SetResourceBudget(int resource, int budget)
{
    if (resource != -1)
    {
        EmissionResourceStats* stats
            = EmissionManager::Instance()->mResourceStats;
        stats[resource].mBudget = budget;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9E9C | size: 0xF8
 */
void EmissionManager::ConfigureResource(
    int resource, const char* name, int budget)
{
    if (resource != -1)
    {
        EmissionResourceStats* const stats
            = EmissionManager::Instance()->mResourceStats;
        stats[resource].Configure(name, budget);
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9F94 | size: 0xC4
 */
void EmissionManager::RecordRenderedParticles(
    unsigned long resource, int numParticles)
{
    if ((int)resource != -1)
    {
        EmissionResourceStats* stats
            = EmissionManager::Instance()->mResourceStats;
        TweakIntBinding* count = stats[resource].mCount;
        unsigned int rendered = (int)*count + numParticles;
        *count = rendered;
        unsigned int maximum;
        TweakIntBinding* highWaterMark = stats[resource].mHighWaterMark;
        maximum = (int)*highWaterMark;
        if (rendered >= maximum)
        {
            maximum = rendered;
        }
        *highWaterMark = maximum;
    }
}
