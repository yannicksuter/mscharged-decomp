#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioGlobals.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/debug.h"

#include "Game/Task/TextWindowTask.h"

#include "Game/Audio/AudioBankTable.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "revolution/sc.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Camera/CameraMan.h"
#include "Game/TweakRegistry.h"
#include "NL/nlAVLTree.h"
#include "NL/nlDebugString.h"
#include "NL/nlPrint.h"
#include "NL/nlTask.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/UnidentifiedStaticStorage.h"

void UpdateAudioSystem(AudioSystem*, float);
XSoundCueHandle* CreateAudioSoundHandle(
    AudioSystem*, int, XSoundOwner*, unsigned long,
    int, int, int, int, int);

extern void* gExclusiveAudioContext;
extern XSoundHandle* g_pLastAudioHandle;
extern unsigned long gResidentVoiceDropCount;
extern unsigned long gStreamVoiceDropCount;
extern bool s_AudioInInit__9ResetTask;

static char sNoAudio[] = "user/NoAudio";
static char sDisableControllerSpeaker[] = "user/DisableControllerSpeaker";
static char sResidentVoiceDrops[] = "Total resident voice drop count: %d\n";
static char sStreamVoiceDrops[] = "Total stream voice drop count: %d\n";
static char sMissingSlot[] = "SafePlay: No slot id %d";
static char sMissingCue[] = "SafePlay: No Cue \"%s\" (%d)";
static char sResumedCue[] = "Resumed cue";

bool gAudioEnabled = true;
unsigned long sAudioPauseDepth = 1;
char gAudioResourcePath[8] = "audio/";

typedef nlAVLTreeSlotPool<unsigned long, XSoundHandle*,
    DefaultKeyCompare<unsigned long> > AudioHandleMap;
typedef nlAVLTreeSlotPool<unsigned long, AudioHandleState,
    DefaultKeyCompare<unsigned long> > AudioHandleStateMap;
typedef nlAVLTreeSlotPool<unsigned long, bool,
    DefaultKeyCompare<unsigned long> > PausedAudioHandleMap;

static AudioHandleMap sAudioHandles(0x10, 0x10);
static AudioHandleStateMap sAudioHandleStates(0x10, 0x10);
static PausedAudioHandleMap sPausedAudioHandles(0x60, 0);

static inline unsigned long MakeAudioHandleKey(
    unsigned long cueId, const void* context)
{
    return cueId ^ (unsigned long)context;
}

static inline XSoundHandle** FindAudioHandleSlot(
    unsigned long cueId, void* context)
{
    XSoundHandle** slot = 0;
    unsigned long key = MakeAudioHandleKey(cueId, context);
    sAudioHandles.FindGet(key, &slot);
    return slot;
}

static inline AudioHandleState* FindAudioHandleState(
    unsigned long key)
{
    AudioHandleState* state = 0;
    sAudioHandleStates.FindGet(key, &state);
    return state;
}

static inline void AddAudioHandleState(int slotId, unsigned long cueId,
    void* context, bool restartable)
{
    AudioHandleState state;
    state.m_CueId = cueId;
    state.m_Context = context;
    state.m_FlagsHi16 = slotId;
    state.m_FlagsBit15 = restartable;
    state.m_FlagsBits12_14 = 0;
    unsigned long key = MakeAudioHandleKey(cueId, context);
    sAudioHandleStates.Add(key, state);
}

GameAudio::GameAudio()
    : AudioSystem()
    , m_PlayRequestCount(0)
{
}

bool GameAudio::Initialize()
{
    CurrentAllocator = &VirtualAllocator;
    unsigned int index = AllocatorStackDepth++;
    AllocatorStack[index] = CurrentAllocator;
    gAudioSourceListCount = 0x800000;

    s_AudioInInit__9ResetTask = true;
    SetResourcePath(gAudioResourcePath);
    s_AudioInInit__9ResetTask = false;

    if (SCGetSoundMode() == 0)
    {
        g_pAudioBackend->SetOutputMode(0);
    }

    gAudioEnabled = !GetTweakBool(sNoAudio, !gAudioEnabled);
    SetControllerSpeakerEnabled(!GetTweakBool(sDisableControllerSpeaker, false));
    m_Listener->SetEnabled(true);

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return true;
}

void GameAudio::Shutdown()
{
    AudioSystem::Shutdown();

    sAudioHandles.Clear();
    sAudioHandles.m_Allocator.FreeBlocks();
    sAudioHandleStates.Clear();
    sAudioHandleStates.m_Allocator.FreeBlocks();

    tDebugPrintManager::Print(DC_SOUND, sResidentVoiceDrops, gResidentVoiceDropCount);
    tDebugPrintManager::Print(DC_SOUND, sStreamVoiceDrops, gStreamVoiceDropCount);
    gResidentVoiceDropCount = 0;
    gStreamVoiceDropCount = 0;
}

void GameAudio::Update(float deltaTime)
{
    bool transformValid = false;

    if (nlTaskManager::m_pInstance->mCurrentState == 2
        && GetNextCamera() != 0)
    {
        nlVector3 listenerVector;
        cCameraManager::GetUpVector(listenerVector);
        m_Listener->SetUp(listenerVector);
        cCameraManager::GetViewVector(listenerVector);
        m_Listener->SetView(listenerVector);
        listenerVector = cCameraManager::m_cameraPosition;
        nlVec3ScaleAdd(listenerVector, 15.0f, m_Listener->m_View, listenerVector);
        m_Listener->SetPosition(listenerVector);

        transformValid = nlNear((float)(double)nlVec3Length(m_Listener->m_View), 1.0f)
            && nlNear((float)(double)nlVec3Length(m_Listener->m_Up), 1.0f);
    }

    m_Listener->SetTransformValid(transformValid);
    UpdateAudioSystem(this, deltaTime);
}

void LoadSoundBank(GameAudio* audio, int slotId,
    unsigned long cueId, AudioPlayCallback callback,
    void* context)
{
    if (!gAudioEnabled)
    {
        return;
    }

    if (gExclusiveAudioContext != 0 && context != gExclusiveAudioContext)
    {
        callback(0, context);
        return;
    }

    if (!audio->IsInitialized())
    {
        return;
    }

    ++audio->m_PlayRequestCount;
    audio->GetBundleManager()->GetSoundMap()->LoadBank(slotId,
        cueId, callback, context, 0);
}

void UnloadSoundBanks(GameAudio* audio)
{
    if (gAudioEnabled
        && audio->GetBundleManager()->GetSoundMap() != 0)
    {
        audio->GetBundleManager()->GetSoundMap()->UnloadAllBanks();
    }
}

bool PlaySound(int slotId, unsigned long cueId,
    const void* debugName, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return true;
    }

    XSoundHandle* handle = CreateSoundHandle(
        slotId, cueId, 0, debugName, context, false);
    if (handle != 0)
    {
        handle->Play(context == 0);
    }
    return handle != 0;
}

bool PlayOwnedSound(int slotId, unsigned long cueId,
    XSoundOwner* owner, const void* debugName, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return true;
    }

    XSoundHandle* handle = CreateSoundHandle(
        slotId, cueId, owner, debugName, context, false);
    if (handle != 0)
    {
        handle->Play(context == 0);
    }
    return handle != 0;
}

XSoundHandle* CreateSoundHandle(int slotId,
    unsigned long cueId, XSoundOwner* owner,
    const void* debugName, void* context, bool findExisting)
{
    if (gExclusiveAudioContext != 0)
    {
        return 0;
    }
    if (!gAudioEnabled)
    {
        return 0;
    }

    AudioSystem* audio = g_pAudioSystem;
    g_pLastAudioHandle = 0;
    AudioResourceLoadOwner* resource = audio->GetBundleManager()
        ->GetSoundMap()->records_0C[slotId].field_10;
    if (resource == 0)
    {
        PrintTextWindowMessage(sMissingSlot, slotId);
        return 0;
    }
    if (FindAudioResourceCue(resource, cueId, 0, 0, 0) == 0xFFFF)
    {
        PrintTextWindowMessage(sMissingCue,
            nlLookupDebugString(g_pDebugStringTable, cueId), cueId);
        return 0;
    }

    XSoundCueHandle* handle = CreateAudioSoundHandle(
        audio, slotId, owner, cueId, 0, 0, 0, 0, 0);
    g_pLastAudioHandle = handle;

    if (context != 0)
    {
        if (!findExisting)
        {
            XSoundHandle* soundHandle;
            unsigned long key;
            key = (unsigned long)context ^ cueId;
            soundHandle = handle;
            sAudioHandles.Add(key, soundHandle);
        }
        else
        {
            unsigned long key = (unsigned long)context ^ cueId;
            sAudioHandles.FindGet(key);
        }
    }
    return handle;
}

bool IsSoundTracked(unsigned long cueId, void* context)
{
    XSoundHandle** slot = 0;
    unsigned long key = MakeAudioHandleKey(cueId, context);
    return sAudioHandles.FindGet(key, &slot);
}

XSoundHandle* FindSoundHandle(
    unsigned long cueId, void* context)
{
    if (!gAudioEnabled)
    {
        return 0;
    }
    XSoundHandle** slot = 0;
    unsigned long key = MakeAudioHandleKey(cueId, context);
    sAudioHandles.FindGet(key, &slot);
    return slot != 0 ? *slot : 0;
}

bool PlayTrackedSound(int slotId, unsigned long cueId,
    const void* debugName, void* context, bool restartable)
{
    bool played;
    if (cueId == 0xFFFFFFFF)
    {
        played = true;
    }
    else
    {
        XSoundHandle* handle = CreateSoundHandle(
            slotId, cueId, 0, debugName, context, false);
        if (handle != 0)
        {
            handle->Play(context == 0);
        }
        played = handle != 0;
    }

    if (played)
    {
        AddAudioHandleState(slotId, cueId, context, restartable);
        return true;
    }
    return false;
}

bool PlayTrackedOwnedSound(int slotId, unsigned long cueId,
    XSoundOwner* owner, const void* debugName,
    void* context, bool restartable)
{
    if (PlayOwnedSound(slotId, cueId, owner, debugName, context))
    {
        AddAudioHandleState(slotId, cueId, context, restartable);
        return true;
    }
    return false;
}

void StopSound(unsigned long cueId, void* context)
{
    if (gExclusiveAudioContext == 0 && gAudioEnabled
        && cueId != 0xFFFFFFFF)
    {
        XSoundHandle** slot = 0;
        unsigned long key = MakeAudioHandleKey(cueId, context);
        if (sAudioHandles.FindGet(key, &slot))
        {
            sAudioHandleStates.Remove(key);
            XSoundHandle* handle = *slot;
            if (handle != 0)
            {
                switch (handle->m_State)
                {
                case 2:
                case 3:
                case 4:
                case 5:
                    handle->Stop(1, 0);
                    break;
                case 7:
                    handle->SetCallbackEnabled(1);
                    break;
                case 8:
                    handle->Release();
                    break;
                case 9:
                    break;
                }
            }
            sAudioHandles.Remove(key);
        }
    }
}

void PauseSound(unsigned long cueId, void* context)
{
    if (cueId != 0xFFFFFFFF)
    {
        unsigned long key = MakeAudioHandleKey(cueId, context);
        XSoundHandle** slot = 0;
        if (sAudioHandles.FindGet(key, &slot))
        {
            AudioHandleState* state;
            sAudioHandleStates.FindGet(key, &state);
            if (state->m_FlagsBit15 != 0 && *slot != 0)
            {
                if ((*slot)->m_State == 8)
                {
                    (*slot)->Release();
                    *slot = 0;
                }
                else if ((*slot)->m_State != 5)
                {
                    (*slot)->Pause();
                }
            }
            else if (*slot != 0)
            {
                (*slot)->Stop(1, 0);
                *slot = 0;
            }

            if (state->m_FlagsBits12_14 == 0)
            {
                state->m_FlagsBits12_14 = sAudioPauseDepth;
            }
        }
    }
}

void AudioSystem::ResumeTrackedSound(
    const unsigned long&, AudioHandleState* state)
{
    ResumeSound(state->m_CueId, state->m_Context);
}

void ResumeSound(unsigned long cueId, void* context)
{
    if (cueId != 0xFFFFFFFF)
    {
        unsigned long key = MakeAudioHandleKey(cueId, context);
        XSoundHandle** slot = 0;
        if (sAudioHandles.FindGet(key, &slot))
        {
            AudioHandleState* state;
            sAudioHandleStates.FindGet(key, &state);
            if (state->m_FlagsBits12_14 >= sAudioPauseDepth)
            {
                if (state->m_FlagsBit15 != 0)
                {
                    if (*slot != 0)
                    {
                        (*slot)->Resume();
                    }
                }
                else
                {
                    *slot = CreateSoundHandle(state->m_FlagsHi16,
                        state->m_CueId, 0, sResumedCue, context, true);
                    (*slot)->Play(false);
                }
                state->m_FlagsBits12_14 = 0;
            }
        }
    }
}

void SetLastSoundParameter(unsigned long parameter, float value)
{
    if (g_pLastAudioHandle == 0)
    {
        return;
    }

    AudioParameter* audioParameter
        = GetSoundParameter(g_pLastAudioHandle, parameter);
    if (value < audioParameter->m_Min)
    {
        audioParameter->m_Value = audioParameter->m_Min;
    }
    else if (value > audioParameter->m_Max)
    {
        audioParameter->m_Value = audioParameter->m_Max;
    }
    else
    {
        audioParameter->m_Value = value;
    }
    audioParameter->m_Time = 0.0f;
}

int GetSoundState(
    unsigned long cueId, void* context)
{
    XSoundHandle* handle = FindSoundHandle(cueId, context);
    return handle != 0 ? handle->m_State : 8;
}

void SetSoundCallbackEnabled(unsigned long cueId, void* context,
    unsigned char enabled)
{
    XSoundHandle* handle = FindSoundHandle(cueId, context);
    if (handle != 0)
    {
        handle->SetCallbackEnabled(enabled);
    }
}

bool PrepareTrackedSound(int slotId, unsigned long cueId,
    XSoundOwner* owner, const void* debugName,
    void* context, bool restartable)
{
    XSoundHandle* handle = CreateSoundHandle(
        slotId, cueId, owner, debugName, context, false);
    if (handle != 0)
    {
        AddAudioHandleState(slotId, cueId, context, restartable);
    }
    if (handle != 0)
    {
        handle->Prepare(false);
    }
    return handle != 0;
}

bool StartTrackedSound(unsigned long cueId, void* context)
{
    XSoundHandle* handle = FindSoundHandle(cueId, context);
    if (handle == 0)
    {
        return false;
    }
    handle->Play(false);
    return true;
}

void PauseAllAudio()
{
    ++sAudioPauseDepth;
    sAudioHandles.Walk(
        g_pAudioSystem, &AudioSystem::PauseTrackedSound);
    nlDLListIterator<XSoundHandle*> sounds = g_pAudioSystem->m_ActiveSoundList.Begin();
    while (sounds.hasNext())
    {
        unsigned long key = (unsigned long)*sounds;
        bool* paused;
        if (!sPausedAudioHandles.FindGet(key, &paused))
        {
            XSoundHandle* handle = (XSoundHandle*)key;
            switch (handle->m_State)
            {
            case 2:
            case 3:
            case 4:
            case 5:
                handle->Stop(1, 0);
                break;
            case 7:
                handle->SetCallbackEnabled(1);
                break;
            case 8:
                handle->Release();
                break;
            case 9:
                break;
            }
        }
        sounds.Step();
    }
    sPausedAudioHandles.Clear();
}

void ResumeAllAudio()
{
    sAudioHandleStates.Walk(
        g_pAudioSystem, &AudioSystem::ResumeTrackedSound);
    --sAudioPauseDepth;
}

int GetAudioPauseDepth()
{
    return sAudioPauseDepth;
}

void AudioSystem::PauseTrackedSound(
    const unsigned long& key, XSoundHandle** handle)
{
    AudioHandleState* state = FindAudioHandleState(key);
    if (state != 0)
    {
        PauseSound(state->m_CueId, state->m_Context);
    }
    else if (*handle != 0)
    {
        (*handle)->Stop(0, 0);
    }

    if (*handle != 0)
    {
        unsigned long handleKey = (unsigned long)*handle;
        sPausedAudioHandles.Add(handleKey, gAudioResourcePath[7]);
    }
}


void AudioListener::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}

void AudioListener::SetTransformValid(bool valid)
{
    m_TransformValid = valid;
}

bool AudioSystem::IsInitialized()
{
    return m_BundleManager != 0 && m_BundleManager->IsInitialized();
}

bool AudioSystem::IsAsyncLoading()
{
    return m_AsyncLoading;
}
