#include "revolution/os/OSTime.h"
#include "Game/Sys/audio.h"
#include "Game/Audio/GameStreams.h"
#include "Game/Audio/AudioGlobals.h"

#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioGlobals.h"
#include "Game/Player.h"
#include "Game/TweakRegistry.h"
#include "NL/globalpad.h"
#include "NL/nlString.h"
#include "Game/NetworkInput.h"


struct AudioControllerOwner
{
    u8 m_Unknown00[4];
    int m_PadIndex;
};

struct RestrictedStreamSlot
{
    unsigned long m_CueId;
    unsigned long m_LastPlayedMilliseconds;
};

void* gExclusiveAudioContext;
XSoundHandle* g_pLastAudioHandle;

static char sRestrictStreamsKey[] = "user/RestrictStreams";
static char sCrowdReactionName[] = "CrowdReaction";
static char sCaptainChantName[] = "CaptainChant";
static char sCaptainPowerupStreamName[] = "CapSigPUActivate";
static char sSuddenDeathMusicName[] = "Sudden Death Music";
static char sControllerSpeakerName[] = "ControllerSpeaker";

static RestrictedStreamSlot sRestrictedStreams[2];

static bool sSuddenDeathMusicStarted;
static bool sSuddenDeathMusicRequested;
static bool sSuddenDeathMusicPaused;
static bool sCaptainStreamActive;

void XSoundHandle::SetCallbackEnabled(unsigned char enabled)
{
    m_CallbackEnabled = enabled;
}

void InvalidateSoundHandle(XSoundHandle* handle)
{
    handle->m_State = 9;
}

void InitializeGameStreams()
{
}

static inline unsigned long GetAudioMilliseconds()
{
    return (unsigned long)(OSGetTime()
        / ((*(unsigned long*)0x800000F8 >> 2) / 1000));
}

void StopCrowdReactions()
{
    int i = 0;
    RestrictedStreamSlot* slot = sRestrictedStreams;
    for (; i < 2; ++i, ++slot)
    {
        StopSound(slot->m_CueId, slot);
    }
}

void PlayCrowdReaction(unsigned long cueId)
{
    if (GetTweakBool(sRestrictStreamsKey, false))
    {
        return;
    }
    if (sCaptainStreamActive)
    {
        return;
    }

    RestrictedStreamSlot* slot = 0;
    for (int i = 0; i < 2; ++i)
    {
        if (sRestrictedStreams[i].m_CueId == cueId)
        {
            slot = &sRestrictedStreams[i];
            break;
        }
    }

    bool play = true;
    unsigned long now = GetAudioMilliseconds();
    if (slot != 0)
    {
        play = now - slot->m_LastPlayedMilliseconds > 1200;
    }

    if (!play)
    {
        return;
    }

    if (slot == 0)
    {
        slot = &sRestrictedStreams[0];
        RestrictedStreamSlot* other = &sRestrictedStreams[1];
        if (other->m_LastPlayedMilliseconds
            < slot->m_LastPlayedMilliseconds)
        {
            slot = other;
        }
    }

    StopSound(slot->m_CueId, slot);
    slot->m_LastPlayedMilliseconds = now;
    slot->m_CueId = cueId;
    PlayOwnedSound(14, cueId, 0, sCrowdReactionName, slot);
}

void PlayCaptainChant(
    int slotId, unsigned long cueId, void* context)
{
    sCaptainStreamActive = true;

    int i = 0;
    RestrictedStreamSlot* slot = sRestrictedStreams;
    for (; i < 2; ++i, ++slot)
    {
        StopSound(slot->m_CueId, slot);
    }

    PlayTrackedOwnedSound(slotId, cueId, 0, sCaptainChantName, context, false);
}

void StopCaptainChant(unsigned long cueId, void* context)
{
    sCaptainStreamActive = false;
    StopSound(cueId, context);
}

void PlayCaptainPowerupStream(
    int slotId, unsigned long cueId, void* context)
{
    sCaptainStreamActive = true;

    int i = 0;
    RestrictedStreamSlot* slot = sRestrictedStreams;
    for (; i < 2; ++i, ++slot)
    {
        StopSound(slot->m_CueId, slot);
    }

    PlayTrackedOwnedSound(slotId, cueId, 0, sCaptainPowerupStreamName, context, false);
}

void StopCaptainPowerupStream(unsigned long cueId, void* context)
{
    sCaptainStreamActive = false;
    StopSound(cueId, context);
}

void PlaySuddenDeathMusic()
{
    if (!sSuddenDeathMusicPaused)
    {
        PlayTrackedOwnedSound(18, 0x1326CB0C, 0, sSuddenDeathMusicName,
            (void*)-1, true);
        sSuddenDeathMusicStarted = true;
    }
    sSuddenDeathMusicRequested = true;
}

void PauseSuddenDeathMusic()
{
    PauseSound(0x1326CB0C, (void*)-1);
    sSuddenDeathMusicPaused = true;
}

void ResumeSuddenDeathMusic()
{
    sSuddenDeathMusicPaused = false;
    if (sSuddenDeathMusicStarted)
    {
        ResumeSound(0x1326CB0C, (void*)-1);
    }
    else if (sSuddenDeathMusicRequested)
    {
        PlayTrackedOwnedSound(18, 0x1326CB0C, 0, sSuddenDeathMusicName,
            (void*)-1, true);
        sSuddenDeathMusicStarted = true;
        sSuddenDeathMusicRequested = true;
    }
}

void StopSuddenDeathMusic()
{
    StopSound(0x1326CB0C, (void*)-1);
    sSuddenDeathMusicPaused = false;
    sSuddenDeathMusicStarted = false;
    sSuddenDeathMusicRequested = false;
}

void SetPlayerAudioController(cPlayer* player)
{
    DetInput* globalPad = 0;
    if (player != 0)
    {
        globalPad = player->GetGlobalPad();
    }

    AudioControllerOwner* owner = 0;
    if (globalPad != 0)
    {
        owner = (AudioControllerOwner*)((NetworkPeerChannel*)globalPad->m_pMyUser)->GetLocalChannelPad();
    }

    if (owner != 0)
    {
        int padIndex = owner->m_PadIndex;
        unsigned long hash = nlStringLowerHash(sControllerSpeakerName);
        SetAudioEffectContext(&hash, padIndex + 1);
    }
    else
    {
        unsigned long hash = nlStringLowerHash(sControllerSpeakerName);
        SetAudioEffectContext(&hash, -1);
    }
}
