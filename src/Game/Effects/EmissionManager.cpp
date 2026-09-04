#include "Game/Effects/EmissionManager.h"

#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/TweakValue.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

static EffectsLight g_EffectsLights[3];
static int g_nNumLights;
static unsigned long fx_sTerrain;
static unsigned int sResourceIdCounter;
static const char* sDefaultResourceNames[2] = { "Default", "World" };

extern u8 lbl_8057F6D4[0x0C];
extern void* lbl_806E1FE8;
extern void* lbl_806E1FEC;
extern void* lbl_806E1FF0;
extern void* lbl_806E1FF4;
extern void* lbl_806E1FF8;
extern void* lbl_806E1FFC;

extern "C" void* fn_802CC094();
extern "C" void fn_802E6444(
    void* data, unsigned long size, void* userData);
extern "C" bool fn_802CDD78(
    void* data, unsigned long size, void* allocator, int unknown);
extern "C" bool fn_802E3D4C(EffectsGroup* group);
extern "C" void fn_802E4358(EmissionController* controller);
extern "C" bool fn_802E5348(EmissionController* controller, float dt);
extern "C" int fn_802E57B8(EmissionController* controller);
extern "C" void fn_802E5BA0(void* parser, void* data, void* allocated,
    unsigned long size, void* context);
extern "C" void fxParticleStartup(int numParticles);
extern "C" void fn_802E3AC0(int parameter);
extern "C" void fn_802E99C4(EmissionResourceStats* stats);
extern "C" bool fn_802B3E94(const char* path, LoadAsyncCallback callback,
    void* userData, unsigned int alignment, int allocType,
    unsigned int chunkSize, void* readBuffer0, void* readBuffer1,
    void* param, unsigned long value, MemoryAllocator* allocator);

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
extern "C" void fn_802E643C(
    void* data, unsigned long size, void* userData)
{
    *(void**)userData = data;
}

/**
 * Offset/Address/Size: 0x710 | 0x802E65CC | size: 0x54
 */
extern "C" void fn_802E65CC(void* data, unsigned long size)
{
    fn_802CDD78(data, size, fn_802CC094(), 0);
    nlFree(data);
}

/**
 * Offset/Address/Size: 0x6BC | 0x802E6620 | size: 0x154
 */
extern "C" void fn_802E6620(
    bool first, bool second, bool third, bool fourth)
{
    lbl_806E1FE8 = 0;
    lbl_806E1FEC = 0;
    lbl_806E1FF0 = 0;
    lbl_806E1FF4 = 0;
    lbl_806E1FF8 = 0;

    nlLoadEntireFileAsync("art/effects/effects.bun", fn_802E643C,
        &lbl_806E1FE8, 0x20,
        first ? AllocateStart : AllocateEnd, 0, 0, 0);

    if (fourth)
    {
        fn_802B3E94("art/effects/effectsNonRes.bun.zlib", fn_802E643C,
            &lbl_806E1FEC, 0x20,
            second ? AllocateStart : AllocateEnd, 0x40000,
            0, 0, 0, 0, 0);
    }
    else
    {
        nlLoadEntireFileAsync("art/effects/effectsNonRes.bun", fn_802E643C,
            &lbl_806E1FEC, 0x20,
            second ? AllocateStart : AllocateEnd, 0, 0, 0);
    }

    nlLoadEntireFileAsync("art/objects/effectsgeometry.bun", fn_802E6444,
        &lbl_806E1FF4, 0x20,
        first ? AllocateStart : AllocateEnd, 0, 0, 0);
    nlLoadEntireFileAsync("art/objects/effectsgeometrytextures.rlt",
        (LoadAsyncCallback)fn_802E65CC, &lbl_806E1FF8, 0x20,
        first ? AllocateStart : AllocateEnd, 0, 0, 0);

    lbl_806E1FFC = *(void**)((u8*)fn_802CC094() + 0x0C);
}

/**
 * Offset/Address/Size: 0x568 | 0x802E6774 | size: 0x6C
 */
extern "C" bool fn_802E6774(void* context)
{
    if (lbl_806E1FE8 == 0)
    {
        return false;
    }
    if (lbl_806E1FEC == 0)
    {
        return false;
    }

    fn_802E5BA0(lbl_8057F6D4, lbl_806E1FE8, lbl_806E1FEC,
        (unsigned long)context, 0);
    nlFree(lbl_806E1FEC);
    lbl_806E1FEC = 0;
    return true;
}

/**
 * Offset/Address/Size: 0x504 | 0x802E67E0 | size: 0x64
 */
extern "C" void fn_802E67E0(void* data, void* allocated,
    unsigned long size, void* context)
{
    if (data != 0 || allocated != 0)
    {
        fn_802E5BA0(
            lbl_8057F6D4, data, allocated, size, context);
    }
    ::operator delete(allocated);
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
extern "C" void fn_802E6C20(EmissionManager* manager, void* context,
    int numParticles, int parameter)
{
    manager->mContext = context;
    manager->mNumParticles = numParticles;
    fxParticleStartup(numParticles);
    fn_802E3AC0(parameter);

    for (int i = 0; i < 8; ++i)
    {
        fn_802E99C4(&manager->mResourceStats[i]);
    }
    manager->mUpdateEnabled = true;
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
        if (fn_802E5348(p, dt))
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
            || fn_802E3D4C(current->m_pGroup))
        {
            fn_802E57B8(current);
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
extern "C" EmissionController* fn_802E81A0(EmissionManager* manager,
    unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    nlDLListIterator<EmissionController*> iterator
        = manager->mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if ((pEffectsGroup == 0 || current->m_pGroup == pEffectsGroup)
            && userData == current->m_uUserData)
        {
            return current;
        }
        iterator.Step();
    }
    return 0;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E824C | size: 0x98
 */
bool EmissionManager::IsStillAlive(EmissionController* controller)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        if (*iterator == controller)
        {
            return true;
        }
        iterator.Step();
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
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if ((pEffectsGroup == 0 || current->m_pGroup == pEffectsGroup)
            && userData == current->m_uUserData)
        {
            current->Die();
        }
        iterator.Step();
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E83C4 | size: 0xC8
 */
extern "C" void fn_802E83C4(
    EmissionManager* manager, const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup == 0)
    {
        return;
    }

    nlDLListIterator<EmissionController*> iterator
        = manager->mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (current->m_pGroup == pEffectsGroup)
        {
            current->Die();
        }
        iterator.Step();
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
        while (iterator.hasNext())
        {
            EmissionController* current = *iterator;
            if (current->m_pGroup == pEffectsGroup
                && (userData == 0 || userData == current->m_uUserData))
            {
                return true;
            }
            iterator.Step();
        }
    }
    return false;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8544 | size: 0xB8
 */
extern "C" bool fn_802E8544(EmissionManager* manager,
    unsigned long userData, const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup != 0)
    {
        nlDLListIterator<EmissionController*> iterator
            = manager->mControllers.Begin();
        while (iterator.hasNext())
        {
            EmissionController* current = *iterator;
            if (current->m_pGroup == pEffectsGroup
                && (userData == 0 || userData == current->m_uUserData))
            {
                return current->m_bPlaying;
            }
            iterator.Step();
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
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (current->m_pContext == mContext
            && current->m_View == view
            && (!exceptPersistent
                || !fn_802E3D4C(current->m_pGroup)))
        {
            mControllers.Remove(&iterator);
            fn_802E4358(current);
            delete current;
        }
        else
        {
            iterator.Step();
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8770 | size: 0x15C
 */
void EmissionManager::DestroyAll(bool exceptPersistent)
{
    nlDLListIterator<EmissionController*> iterator = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (current->m_pContext == mContext
            && (!exceptPersistent
                || !fn_802E3D4C(current->m_pGroup)))
        {
            mControllers.Remove(&iterator);
            delete current;
        }
        else
        {
            iterator.Step();
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
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if ((pEffectsGroup == 0 || current->m_pGroup == pEffectsGroup)
            && userData == current->m_uUserData)
        {
            mControllers.Remove(&iterator);
            fn_802E4358(current);
            delete current;
        }
        else
        {
            iterator.Step();
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8A2C | size: 0x14C
 */
extern "C" void fn_802E8A2C(
    EmissionManager* manager, const EffectsGroup* pEffectsGroup)
{
    if (pEffectsGroup == 0)
    {
        return;
    }

    nlDLListIterator<EmissionController*> iterator
        = manager->mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        if (current->m_pGroup == pEffectsGroup)
        {
            manager->mControllers.Remove(&iterator);
            delete current;
        }
        else
        {
            iterator.Step();
        }
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E8B78 | size: 0xE4
 */
extern "C" void fn_802E8B78(
    EmissionManager* manager, Function<void*>* callback)
{
    nlDLListIterator<EmissionController*> iterator
        = manager->mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        (*callback)(current);
        iterator.Step();
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
    float prevBestAge = 0.0f;
    float currentBestAge = 0.0f;

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
                float age = current->m_Age;
                if (bestAge < age
                    && (prevBestAge == currentBestAge
                        || age < currentBestAge))
                {
                    bestAge = age;
                    bestController = current;
                    currentBestAge = age;
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
extern "C" void fn_802E97C0(EmissionManager* manager)
{
    nlDLListIterator<EmissionController*> iterator
        = manager->mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        fn_802E4358(current);
        current->Die();
        iterator.Step();
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E987C | size: 0xBC
 */
extern "C" void fn_802E987C(EmissionManager* manager)
{
    while (manager->mReplayControllers.m_Head != 0)
    {
        nlDLListIterator<EmissionController*> iterator
            = manager->mReplayControllers.Begin();
        EmissionController* current = *iterator;
        manager->mReplayControllers.Remove(&iterator);
        delete current;
    }

    manager->mReplayControllers.m_Head = manager->mControllers.m_Head;
    manager->mControllers.m_Head = 0;
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9938 | size: 0x8C
 */
void EmissionManager::SetContext(void* context)
{
    mContext = context;
    nlDLListIterator<EmissionController*> iterator
        = mControllers.Begin();
    while (iterator.hasNext())
    {
        EmissionController* current = *iterator;
        current->m_pContext = context;
        iterator.Step();
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x802E9E0C | size: 0x90
 */
extern "C" void fn_802E9E0C(int resource, int budget)
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
extern "C" void fn_802E9E9C(
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
extern "C" void fn_802E9F94(
    unsigned long resource, int numParticles)
{
    if (resource != (unsigned long)-1)
    {
        EmissionResourceStats* stats
            = EmissionManager::Instance()->mResourceStats;
        EmissionResourceStats& resourceStats = stats[resource];
        TweakValueIntImpl_804FD898* count = resourceStats.mCount;
        TweakValueIntImpl_804FD898* highWaterMark
            = resourceStats.mHighWaterMark;
        *count->m_pValue += numParticles;
        if (*count->m_pValue >= *highWaterMark->m_pValue)
        {
            *highWaterMark->m_pValue = *count->m_pValue;
        }
    }
}
