#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioGlobals.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/debug.h"

#include "Game/Task/TextWindowTask.h"

#include "Game/Audio/AudioBankTable.h"
#include "revolution/sc.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Camera/CameraMan.h"
#include "Game/TweakRegistry.h"
#include "NL/nlAVLTree.h"
#include "NL/nlPrint.h"
#include "NL/nlTask.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/UnidentifiedStaticStorage.h"

void UpdateAudioSystem(AudioSystem*, float);
XSoundHandle* CreateAudioSoundHandle(
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
        cCameraManager::GetUpVector(m_Listener->m_Up);
        cCameraManager::GetViewVector(m_Listener->m_View);
        m_Listener->m_Position.x = cCameraManager::m_cameraPosition.x
            + 15.0f * m_Listener->m_View.x;
        m_Listener->m_Position.y = cCameraManager::m_cameraPosition.y
            + 15.0f * m_Listener->m_View.y;
        m_Listener->m_Position.z = cCameraManager::m_cameraPosition.z
            + 15.0f * m_Listener->m_View.z;
        m_Listener->m_HasTransform = true;

        float viewLength = nlSqrt(
            m_Listener->m_View.x * m_Listener->m_View.x
                + m_Listener->m_View.y * m_Listener->m_View.y
                + m_Listener->m_View.z * m_Listener->m_View.z,
            true);
        float upLength = nlSqrt(
            m_Listener->m_Up.x * m_Listener->m_Up.x
                + m_Listener->m_Up.y * m_Listener->m_Up.y
                + m_Listener->m_Up.z * m_Listener->m_Up.z,
            true);
        transformValid = nlNear(viewLength, 1.0f)
            && nlNear(upLength, 1.0f);
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
    if (gExclusiveAudioContext != 0 || !gAudioEnabled)
    {
        return 0;
    }

    g_pLastAudioHandle = 0;
    if (slotId < 0)
    {
        PrintTextWindowMessage(sMissingSlot, slotId);
        return 0;
    }
    if (cueId == 0xFFFFFFFF)
    {
        PrintTextWindowMessage(sMissingCue, (const char*)debugName, cueId);
        return 0;
    }

    XSoundHandle* handle = CreateAudioSoundHandle(g_pAudioSystem,
        slotId, owner, cueId, 0, 0, 0, 0, 0);
    g_pLastAudioHandle = handle;

    if (context != 0 && handle != 0)
    {
        unsigned long key = MakeAudioHandleKey(cueId, context);
        if (!findExisting)
        {
            sAudioHandles.Add(key, handle);
        }
        else
        {
            XSoundHandle** existing = 0;
            sAudioHandles.FindGet(key, &existing);
        }
    }
    return handle;
}

bool IsSoundTracked(unsigned long cueId, void* context)
{
    return FindAudioHandleSlot(cueId, context) != 0;
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
    if (gExclusiveAudioContext != 0 || !gAudioEnabled
        || cueId == 0xFFFFFFFF)
    {
        return;
    }

    unsigned long key = MakeAudioHandleKey(cueId, context);
    XSoundHandle** slot = FindAudioHandleSlot(cueId, context);
    if (slot == 0)
    {
        return;
    }

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
        }
    }
    sAudioHandles.Remove(key);
}

void PauseSound(unsigned long cueId, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return;
    }

    unsigned long key = MakeAudioHandleKey(cueId, context);
    XSoundHandle** slot = FindAudioHandleSlot(cueId, context);
    AudioHandleState* state = FindAudioHandleState(key);
    if (slot == 0 || state == 0)
    {
        return;
    }

    XSoundHandle* handle = *slot;
    if (state->m_FlagsBit15 != 0)
    {
        if (handle != 0 && handle->m_State == 8)
        {
            handle->Release();
            *slot = 0;
        }
        else if (handle != 0 && handle->m_State != 5)
        {
            handle->Pause();
        }
    }
    else if (handle != 0)
    {
        handle->Stop(1, 0);
        *slot = 0;
    }

    if (state->m_FlagsBits12_14 == 0)
    {
        state->m_FlagsBits12_14 = sAudioPauseDepth;
    }
}

void ResumeSound(unsigned long cueId, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return;
    }

    unsigned long key = MakeAudioHandleKey(cueId, context);
    XSoundHandle** slot = FindAudioHandleSlot(cueId, context);
    AudioHandleState* state = FindAudioHandleState(key);
    if (slot == 0 || state == 0
        || state->m_FlagsBits12_14 < sAudioPauseDepth)
    {
        return;
    }

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
        if (*slot != 0)
        {
            (*slot)->Play(false);
        }
    }
    state->m_FlagsBits12_14 = 0;
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

void AudioSystem::ResumeTrackedSound(
    const unsigned long&, AudioHandleState* state)
{
    ResumeSound(state->m_CueId, state->m_Context);
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
