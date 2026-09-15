#ifndef GAME_AUDIO_AUDIOSEQUENCEINSTANCE_H
#define GAME_AUDIO_AUDIOSEQUENCEINSTANCE_H

#include "NL/nlSlotPool.h"
#include "types.h"

class AudioSource;
struct SoundInstance_802F2110;
struct PlaybackObject_802F2C3C;
struct PlaybackRequest_802F2C3C;

struct AudioSequenceDefinition
{
    float volume;
    u32 count;
    PlaybackRequest_802F2C3C* requests;
};

class AudioSequenceInstance
{
public:
    AudioSequenceInstance(SoundInstance_802F2110* instance, AudioSequenceDefinition* sequence);
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
    /* 0x04 */ SoundInstance_802F2110* instance;
    /* 0x08 */ AudioSequenceDefinition* sequence;
    /* 0x0C */ PlaybackObject_802F2C3C* objects;
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
