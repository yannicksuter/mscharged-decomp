#include "Game/Sys/audio.h"

#include "Game/Audio/AudioBundleManager_802EDA7C.h"
#include "Game/Camera/CameraMan.h"
#include "Game/TweakRegistry.h"
#include "NL/nlAVLTree.h"
#include "NL/nlPrint.h"
#include "NL/nlTask.h"

extern "C" unsigned char SCGetSoundMode();

extern "C" void fn_8035C818(void*, int);
extern "C" void fn_803622F0(bool);
extern "C" void fn_802EC1F4(AudioLoadMode_806E201C*, float);
extern "C" void fn_802EBBF0(void*, int, unsigned long,
    AudioPlayCallback_800EBB04, void*, int);
extern "C" void fn_802EBCCC(void*, unsigned long);
extern "C" XSoundHandle_802ED74C* fn_802EC030(
    AudioLoadMode_806E201C*, int, XSoundOwner_802ED74C*, unsigned long,
    int, int, int, int, int);
extern "C" void fn_802BD820(const char*, ...);
extern "C" void fn_8004F594(int, const char*, ...);
extern "C" void* fn_800F1C14();

struct AudioParameter_802F1A70
{
    u8 m_Unknown00[0xC];
    float m_Value;
    float m_Time;
    u8 m_Unknown14[0x4];
    float m_Min;
    float m_Max;
};

extern "C" AudioParameter_802F1A70* fn_802F1A70(
    XSoundHandle_802ED74C*, unsigned long);

extern void* lbl_806E0E80;
extern XSoundHandle_802ED74C* lbl_806E0E84;
extern void* lbl_806E2020;
extern unsigned long lbl_806E2210;
extern unsigned long lbl_806E2214;
extern bool s_AudioInInit__9ResetTask;

static char sNoAudio[] = "user/NoAudio";
static char sDisableControllerSpeaker[] = "user/DisableControllerSpeaker";
static char sResidentVoiceDrops[] = "Total resident voice drop count: %d\n";
static char sStreamVoiceDrops[] = "Total stream voice drop count: %d\n";
static char sMissingSlot[] = "SafePlay: No slot id %d";
static char sMissingCue[] = "SafePlay: No Cue \"%s\" (%d)";
static char sResumedCue[] = "Resumed cue";

bool lbl_806DC450 = true;
unsigned long lbl_806DC454 = 1;
char lbl_806DC458[8] = "audio/";

typedef nlAVLTreeSlotPool<unsigned long, XSoundHandle_802ED74C*,
    DefaultKeyCompare<unsigned long> > AudioHandleMap;
typedef nlAVLTreeSlotPool<unsigned long, AudioHandleState_800EBF78,
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

static inline XSoundHandle_802ED74C** FindAudioHandleSlot(
    unsigned long cueId, void* context)
{
    XSoundHandle_802ED74C** slot = 0;
    unsigned long key = MakeAudioHandleKey(cueId, context);
    sAudioHandles.FindGet(key, &slot);
    return slot;
}

static inline AudioHandleState_800EBF78* FindAudioHandleState(
    unsigned long key)
{
    AudioHandleState_800EBF78* state = 0;
    sAudioHandleStates.FindGet(key, &state);
    return state;
}

static inline void AddAudioHandleState(int slotId, unsigned long cueId,
    void* context, bool restartable)
{
    AudioHandleState_800EBF78 state;
    state.m_CueId = cueId;
    state.m_Context = context;
    state.m_Flags = ((unsigned long)slotId << 16)
        | (restartable ? 0x8000 : 0);
    unsigned long key = MakeAudioHandleKey(cueId, context);
    sAudioHandleStates.Add(key, state);
}

GameAudio_800EB6AC::GameAudio_800EB6AC()
    : AudioLoadMode_806E201C()
    , m_PlayRequestCount(0)
{
}

bool GameAudio_800EB6AC::Initialize()
{
    s_AudioInInit__9ResetTask = true;
    SetResourcePath(lbl_806DC458);
    s_AudioInInit__9ResetTask = false;

    if (SCGetSoundMode() == 0)
    {
        fn_8035C818(lbl_806E2020, 0);
    }

    lbl_806DC450 = !fn_802C2C84(sNoAudio, !lbl_806DC450);
    fn_803622F0(!fn_802C2C84(sDisableControllerSpeaker, false));
    m_Listener->SetEnabled(true);
    return true;
}

void GameAudio_800EB6AC::Shutdown()
{
    AudioLoadMode_806E201C::Shutdown();

    sAudioHandles.Clear();
    sAudioHandles.m_Allocator.FreeBlocks();
    sAudioHandleStates.Clear();
    sAudioHandleStates.m_Allocator.FreeBlocks();

    fn_8004F594(10, sResidentVoiceDrops, lbl_806E2210);
    fn_8004F594(10, sStreamVoiceDrops, lbl_806E2214);
    lbl_806E2210 = 0;
    lbl_806E2214 = 0;
}

void GameAudio_800EB6AC::Update(float deltaTime)
{
    bool transformValid = false;

    if (nlTaskManager::m_pInstance->mCurrentState == 2
        && fn_800F1C14() != 0)
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
    fn_802EC1F4(this, deltaTime);
}

extern "C" void fn_800EBB04(GameAudio_800EB6AC* audio, int slotId,
    unsigned long cueId, AudioPlayCallback_800EBB04 callback,
    void* context)
{
    if (!lbl_806DC450)
    {
        return;
    }

    if (lbl_806E0E80 != 0 && context != lbl_806E0E80)
    {
        callback(0, context);
        return;
    }

    if (!audio->fn_800ED85C())
    {
        return;
    }

    ++audio->m_PlayRequestCount;
    fn_802EBBF0(audio->GetBundleManager()->GetSoundMap(), slotId,
        cueId, callback, context, 0);
}

extern "C" void fn_800EBBD8(
    GameAudio_800EB6AC* audio, unsigned long value)
{
    if (lbl_806DC450
        && audio->GetBundleManager()->GetSoundMap() != 0)
    {
        fn_802EBCCC(audio->GetBundleManager()->GetSoundMap(), value);
    }
}

extern "C" bool fn_800EBBFC(int slotId, unsigned long cueId,
    const void* debugName, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return true;
    }

    XSoundHandle_802ED74C* handle = fn_800EBD00(
        slotId, cueId, 0, debugName, context, false);
    if (handle != 0)
    {
        handle->fn_802ED74C_3(context == 0);
    }
    return handle != 0;
}

extern "C" bool fn_800EBC84(int slotId, unsigned long cueId,
    XSoundOwner_802ED74C* owner, const void* debugName, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return true;
    }

    XSoundHandle_802ED74C* handle = fn_800EBD00(
        slotId, cueId, owner, debugName, context, false);
    if (handle != 0)
    {
        handle->fn_802ED74C_3(context == 0);
    }
    return handle != 0;
}

extern "C" XSoundHandle_802ED74C* fn_800EBD00(int slotId,
    unsigned long cueId, XSoundOwner_802ED74C* owner,
    const void* debugName, void* context, bool findExisting)
{
    if (lbl_806E0E80 != 0 || !lbl_806DC450)
    {
        return 0;
    }

    lbl_806E0E84 = 0;
    if (slotId < 0)
    {
        fn_802BD820(sMissingSlot, slotId);
        return 0;
    }
    if (cueId == 0xFFFFFFFF)
    {
        fn_802BD820(sMissingCue, (const char*)debugName, cueId);
        return 0;
    }

    XSoundHandle_802ED74C* handle = fn_802EC030(lbl_806E201C,
        slotId, owner, cueId, 0, 0, 0, 0, 0);
    lbl_806E0E84 = handle;

    if (context != 0 && handle != 0)
    {
        unsigned long key = MakeAudioHandleKey(cueId, context);
        if (!findExisting)
        {
            sAudioHandles.Add(key, handle);
        }
        else
        {
            XSoundHandle_802ED74C** existing = 0;
            sAudioHandles.FindGet(key, &existing);
        }
    }
    return handle;
}

extern "C" bool fn_800EBE90(unsigned long cueId, void* context)
{
    return FindAudioHandleSlot(cueId, context) != 0;
}

extern "C" XSoundHandle_802ED74C* fn_800EBEF4(
    unsigned long cueId, void* context)
{
    if (!lbl_806DC450)
    {
        return 0;
    }
    XSoundHandle_802ED74C** slot = FindAudioHandleSlot(cueId, context);
    return slot != 0 ? *slot : 0;
}

extern "C" bool fn_800EBF78(int slotId, unsigned long cueId,
    const void* debugName, void* context, bool restartable)
{
    bool played;
    if (cueId == 0xFFFFFFFF)
    {
        played = true;
    }
    else
    {
        XSoundHandle_802ED74C* handle = fn_800EBD00(
            slotId, cueId, 0, debugName, context, false);
        if (handle != 0)
        {
            handle->fn_802ED74C_3(context == 0);
        }
        played = handle != 0;
    }

    if (played)
    {
        AddAudioHandleState(slotId, cueId, context, restartable);
    }
    return played;
}

extern "C" bool fn_800EC058(int slotId, unsigned long cueId,
    XSoundOwner_802ED74C* owner, const void* debugName,
    void* context, bool restartable)
{
    bool played;
    if (cueId == 0xFFFFFFFF)
    {
        played = true;
    }
    else
    {
        XSoundHandle_802ED74C* handle = fn_800EBD00(
            slotId, cueId, owner, debugName, context, false);
        if (handle != 0)
        {
            handle->fn_802ED74C_3(context == 0);
        }
        played = handle != 0;
    }

    if (played)
    {
        AddAudioHandleState(slotId, cueId, context, restartable);
    }
    return played;
}

extern "C" void fn_800EC12C(unsigned long cueId, void* context)
{
    if (lbl_806E0E80 != 0 || !lbl_806DC450
        || cueId == 0xFFFFFFFF)
    {
        return;
    }

    unsigned long key = MakeAudioHandleKey(cueId, context);
    XSoundHandle_802ED74C** slot = FindAudioHandleSlot(cueId, context);
    if (slot == 0)
    {
        return;
    }

    sAudioHandleStates.Remove(key);
    XSoundHandle_802ED74C* handle = *slot;
    if (handle != 0)
    {
        switch (handle->m_State)
        {
        case 2:
        case 3:
        case 4:
        case 5:
            handle->fn_802ED74C_5(1, 0);
            break;
        case 7:
            handle->fn_800ED8C0(1);
            break;
        case 8:
            handle->fn_802ED74C_10();
            break;
        }
    }
    sAudioHandles.Remove(key);
}

extern "C" void fn_800EC2A4(unsigned long cueId, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return;
    }

    unsigned long key = MakeAudioHandleKey(cueId, context);
    XSoundHandle_802ED74C** slot = FindAudioHandleSlot(cueId, context);
    AudioHandleState_800EBF78* state = FindAudioHandleState(key);
    if (slot == 0 || state == 0)
    {
        return;
    }

    XSoundHandle_802ED74C* handle = *slot;
    if ((state->m_Flags & 0x8000) != 0)
    {
        if (handle != 0 && handle->m_State == 8)
        {
            handle->fn_802ED74C_10();
            *slot = 0;
        }
        else if (handle != 0 && handle->m_State != 5)
        {
            handle->fn_802ED74C_6();
        }
    }
    else if (handle != 0)
    {
        handle->fn_802ED74C_5(1, 0);
        *slot = 0;
    }

    if ((state->m_Flags & 0x7000) == 0)
    {
        state->m_Flags |= (lbl_806DC454 & 7) << 12;
    }
}

extern "C" void fn_800EC400(unsigned long cueId, void* context)
{
    if (cueId == 0xFFFFFFFF)
    {
        return;
    }

    unsigned long key = MakeAudioHandleKey(cueId, context);
    XSoundHandle_802ED74C** slot = FindAudioHandleSlot(cueId, context);
    AudioHandleState_800EBF78* state = FindAudioHandleState(key);
    if (slot == 0 || state == 0
        || ((state->m_Flags >> 12) & 7) < lbl_806DC454)
    {
        return;
    }

    if ((state->m_Flags & 0x8000) != 0)
    {
        if (*slot != 0)
        {
            (*slot)->fn_802ED74C_7();
        }
    }
    else
    {
        *slot = fn_800EBD00((int)(state->m_Flags >> 16),
            state->m_CueId, 0, sResumedCue, context, true);
        if (*slot != 0)
        {
            (*slot)->fn_802ED74C_3(false);
        }
    }
    state->m_Flags &= ~0x7000;
}

extern "C" void fn_800EC548(unsigned long parameter, float value)
{
    if (lbl_806E0E84 == 0)
    {
        return;
    }

    AudioParameter_802F1A70* audioParameter
        = fn_802F1A70(lbl_806E0E84, parameter);
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

extern "C" unsigned long fn_800EC5C4(
    unsigned long cueId, void* context)
{
    XSoundHandle_802ED74C* handle = fn_800EBEF4(cueId, context);
    return handle != 0 ? handle->m_State : 8;
}

extern "C" void fn_800EC65C(unsigned long cueId, void* context,
    unsigned char enabled)
{
    XSoundHandle_802ED74C* handle = fn_800EBEF4(cueId, context);
    if (handle != 0)
    {
        handle->fn_800ED8C0(enabled);
    }
}

extern "C" bool fn_800EC708(int slotId, unsigned long cueId,
    XSoundOwner_802ED74C* owner, const void* debugName,
    void* context, bool restartable)
{
    XSoundHandle_802ED74C* handle = fn_800EBD00(
        slotId, cueId, owner, debugName, context, false);
    if (handle != 0)
    {
        AddAudioHandleState(slotId, cueId, context, restartable);
        handle->fn_802ED74C_4(false);
    }
    return handle != 0;
}

extern "C" bool fn_800EC7BC(unsigned long cueId, void* context)
{
    XSoundHandle_802ED74C* handle = fn_800EBEF4(cueId, context);
    if (handle == 0)
    {
        return false;
    }
    handle->fn_802ED74C_3(false);
    return true;
}

extern "C" void fn_800EC868()
{
    ++lbl_806DC454;
    sAudioHandles.Walk(
        lbl_806E201C, &AudioLoadMode_806E201C::PauseTrackedSound);
    sPausedAudioHandles.Clear();
}

extern "C" void fn_800ECB50()
{
    sAudioHandleStates.Walk(
        lbl_806E201C, &AudioLoadMode_806E201C::ResumeTrackedSound);
    --lbl_806DC454;
}

extern "C" unsigned int fn_800ECCCC()
{
    return lbl_806DC454;
}

void AudioLoadMode_806E201C::PauseTrackedSound(
    const unsigned long& key, XSoundHandle_802ED74C** handle)
{
    AudioHandleState_800EBF78* state = FindAudioHandleState(key);
    if (state != 0)
    {
        fn_800EC2A4(state->m_CueId, state->m_Context);
    }
    else if (*handle != 0)
    {
        (*handle)->fn_802ED74C_5(0, 0);
    }

    if (*handle != 0)
    {
        unsigned long handleKey = (unsigned long)*handle;
        sPausedAudioHandles.Add(handleKey, lbl_806DC458[7]);
    }
}

void AudioLoadMode_806E201C::ResumeTrackedSound(
    const unsigned long&, AudioHandleState_800EBF78* state)
{
    fn_800EC400(state->m_CueId, state->m_Context);
}

void AudioListener_802EBD54::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}

void AudioListener_802EBD54::SetTransformValid(bool valid)
{
    m_TransformValid = valid;
}

bool AudioLoadMode_806E201C::fn_800ED85C()
{
    return m_BundleManager != 0 && m_BundleManager->fn_800ED8B8();
}

bool AudioLoadMode_806E201C::fn_806E201C_0()
{
    return m_AsyncLoading;
}
