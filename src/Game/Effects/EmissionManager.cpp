#include "Game/Effects/EmissionManager.h"

#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/Effects/ParticleSystem.h"
#include "Game/TweakValue.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/nlFile.h"
#include "NL/nlCompressedFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

static EffectsLight g_EffectsLights[3];
static int g_nNumLights;
static unsigned long fx_sTerrain;
static unsigned int sResourceIdCounter;
static const char* sDefaultResourceNames[2] = { "Default", "World" };

class EffectsBundle;

class EffectsBundleManager
{
public:
    void Load(void* data, void* nonResidentData, GLResourcePool* context, int bundleType);

    EffectsBundle* mDefaultBundles;
    EffectsBundle* mAdditionalBundles;
    GLResourcePool* mResourcePool;
};

extern EffectsBundleManager gEffectsBundleManager;
extern void* gEffectsData;
extern void* gEffectsNonResidentData;
extern void* lbl_806E1FF0;
extern void* gEffectsGeometryData;
extern void* gEffectsTextureData;

void OnEffectsGeometryLoaded(
    void* data, unsigned long size, void* userData);

inline EmissionResourceStats::EmissionResourceStats()
    : mCount(0)
    , mHighWaterMark(0)
    , mBudgetTweak(0)
    , mId(sResourceIdCounter++)
{
    mFlags &= 0x3FFF;
    if (mId < 2)
    {
        nlStrNCpy(mName, sDefaultResourceNames[mId], sizeof(mName));
        mBudget = 0xFFFF;
        mFlags = (mFlags & 0x7FFF) | 0x8000;
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
    , mTimeScale(0.0f)
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

/**
 * Offset/Address/Size: 0x0 | 0x802E6C20 | size: 0x1D8
 */
void EmissionManager::Startup(void* context,
    int numParticles, int maxRenderedParticles)
{
    mContext = context;
    mNumParticles = numParticles;
    fxParticleStartup(numParticles);
    fxSetMaxNumParticles(maxRenderedParticles);

    for (int i = 0; i < 8; ++i)
    {
        mResourceStats[i].Initialize();
    }
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

    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        iterator.Step();
        delete current;
    }
    mControllers.Clear();

    while (mReplayControllers.m_Head != 0)
    {
        nlDLListIterator<EmissionController*> replayIterator
            = mReplayControllers.Begin();
        EmissionController* current = *replayIterator;
        mReplayControllers.Remove(&replayIterator);
        delete current;
    }

    mUpdateEnabled = false;
    m_bRecording = true;
    mParticles.m_Allocator.FreeBlocks();
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
 * Offset/Address/Size: 0x0 | 0x802E75F4 | size: 0x64
 */
EmissionManager* GetEmissionManager()
{
    return EmissionManager::Instance();
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
            mControllers.Remove(&iterator);
            delete p;
        }
    }
}

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

    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        iterator.Step();
        if (!mRenderPersistentOnly
            || current->m_pGroup->IsPersistent())
        {
            current->Render();
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E7CD4 | size: 0x8
 */
nlDLListContainer<EmissionController*>* EmissionManager::GetContainer()
{
    return &mControllers;
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
        EmissionController* bestController = 0;
        float bestAge = 0.0f;
        nlDLListIterator<EmissionController*> iterator = mControllers.Begin();

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
        nlDLListIterator<EmissionController*> iterator
            = mReplayControllers.Begin();
        EmissionController* current = *iterator;
        mReplayControllers.Remove(&iterator);
        delete current;
    }

    mReplayControllers.m_Head = mControllers.m_Head;
    mControllers.m_Head = 0;
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
        EmissionResourceStats* stats
            = EmissionManager::Instance()->mResourceStats;
        EmissionResourceStats& resourceStats = stats[resource];
        nlStrNCpy(resourceStats.mName, name, sizeof(resourceStats.mName));
        resourceStats.mBudget = budget;
        resourceStats.mFlags
            = (resourceStats.mFlags & 0x7FFF) | ((budget != 0) << 15);
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9F94 | size: 0xC4
 */
void EmissionManager::RecordRenderedParticles(
    unsigned long resource, int numParticles)
{
    if (resource != (unsigned long)-1)
    {
        EmissionResourceStats* stats
            = EmissionManager::Instance()->mResourceStats;
        EmissionResourceStats& resourceStats = stats[resource];
        TweakIntBinding* count = resourceStats.mCount;
        TweakIntBinding* highWaterMark
            = resourceStats.mHighWaterMark;
        *count->m_pValue += numParticles;
        if (*count->m_pValue >= *highWaterMark->m_pValue)
        {
            *highWaterMark->m_pValue = *count->m_pValue;
        }
    }
}
