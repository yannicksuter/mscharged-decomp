#ifndef GAME_AUDIO_AUDIOSEQUENCEINSTANCE_H
#define GAME_AUDIO_AUDIOSEQUENCEINSTANCE_H

#include "NL/nlSlotPool.h"
#include "types.h"

class AudioSource;
struct SoundInstance_802F2110;
struct AudioSequenceEvent;
struct AudioSequenceEventDefinition;

struct AudioSequenceDefinition
{
    float volumeOffset;
    u32 eventCount;
    AudioSequenceEventDefinition* eventDefinitions;
};

class AudioSequenceInstance
{
public:
    AudioSequenceInstance(
        SoundInstance_802F2110* soundInstance, AudioSequenceDefinition* definition);
    ~AudioSequenceInstance();

    void Play();
    void Prepare();
    void Pause();
    void Resume();
    int Update(float time);
    void Stop();
    void SetPitch(float value);
    void SetVolume(float value);
    void GetSources(AudioSource** results, u32* count);

    static void* operator new(unsigned long size);
    static void operator delete(void* instance);

    /* 0x00 */ AudioSequenceInstance* next;
    /* 0x04 */ SoundInstance_802F2110* soundInstance;
    /* 0x08 */ AudioSequenceDefinition* definition;
    /* 0x0C */ AudioSequenceEvent* events;
    /* 0x10 */ float volumeOffset;
    /* 0x14 */ float pitchOffset;
    /* 0x18 */ bool stopped;
}; // size: 0x1C

extern SlotPool<AudioSequenceInstance> sAudioSequenceInstancePool;

inline void* AudioSequenceInstance::operator new(unsigned long size)
{
    return sAudioSequenceInstancePool.Allocate();
}

inline void AudioSequenceInstance::operator delete(void* instance)
{
    sAudioSequenceInstancePool.Free((AudioSequenceInstance*)instance);
}

#endif // GAME_AUDIO_AUDIOSEQUENCEINSTANCE_H
