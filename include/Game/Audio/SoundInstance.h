#ifndef GAME_AUDIO_SOUND_INSTANCE_H
#define GAME_AUDIO_SOUND_INSTANCE_H

#include "Game/Audio/AudioResourceBundle.h"
#include "Game/Audio/AudioRpc.h"
#include "Game/Audio/Transition.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class AudioSource;
class AudioSequenceInstance;
class XSoundHandle;

enum SoundInstanceState
{
    SOUND_INSTANCE_STATE_INITIAL = 0,
    SOUND_INSTANCE_STATE_PENDING = 1,
    SOUND_INSTANCE_STATE_PREPARING = 2,
    SOUND_INSTANCE_STATE_PREPARED = 3,
    SOUND_INSTANCE_STATE_PLAYING = 4,
    SOUND_INSTANCE_STATE_PAUSED = 5,
    SOUND_INSTANCE_STATE_FAILED = 6,
    SOUND_INSTANCE_STATE_STOPPING = 7,
    SOUND_INSTANCE_STATE_STOPPED = 8
};

struct SoundInstance;
extern SlotPool<SoundInstance> sSoundInstancePool;

struct SoundInstance
{
    SoundInstance(XSoundHandle* owner, AudioVoiceDefinition* definition);
    ~SoundInstance() { Destroy(); }

    void Play(float time);
    void Prepare();
    void SetVolume(bool releaseAfterTransition, float target, float duration);
    void Stop(void* value);
    void Pause();
    void Resume();
    void GetSources(AudioSource** sources, unsigned int* count);
    void Update(float dt);
    float GetVolume();
    float GetPitch();
    void Destroy();

    static void operator delete(void* instance)
    {
        sSoundInstancePool.Free((SoundInstance*)instance);
    }

    /* 0x00 */ XSoundHandle* owner;
    /* 0x04 */ AudioVoiceDefinition* definition;
    /* 0x08 */ AudioSequenceInstance* voices;
    /* 0x0C */ AudioRpcList rpcEntries;
    /* 0x14 */ SoundInstanceState state;
    /* 0x18 */ float previousTime;
    /* 0x1C */ float currentTime;
    /* 0x20 */ AudioRpcRuntimeNode* activeRpc;
    /* 0x24 */ float transitionTime;
    /* 0x28 */ Transition volume;
    /* 0x4C */ Transition pitch;
    /* 0x70 */ float volumeOffset;
    /* 0x74 */ float releaseTime;
    /* 0x78 */ SoundInstance* nextInstance;
}; // size 0x7C

#endif // GAME_AUDIO_SOUND_INSTANCE_H
