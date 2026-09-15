#ifndef GAME_AUDIO_AUDIOSEQUENCEEVENT_H
#define GAME_AUDIO_AUDIOSEQUENCEEVENT_H

#include "Game/Audio/AudioSequenceInstance.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class AudioSource;
class XSoundHandle;
struct AudioSourceInfo;

struct AudioSequenceEvent;

struct SoundChoice
{
    u32 index;
    u32 weight;
};

struct SoundEventDefinition
{
    u32 field_00;
    u32 soundId;
    u32 choiceCount;
    u32 field_0C;
    SoundChoice* choices;
    u8 randomPitch;
    u8 randomVolume;
    u8 pad_16[2];
    float pitchMinimum;
    float pitchMaximum;
    float volumeMinimum;
    float volumeMaximum;
    float delayMinimum;
    float delayRange;
};

struct HitMarkerEventDefinition
{
    u32 field_00;
    void* marker;
    float delayMinimum;
    float delayRange;
};

struct ParameterChangeEventDefinition
{
    u32 field_00;
    u32 field_04;
    float value;
    u8 parameter;
    u8 pad_0D[3];
    float delayMinimum;
    float delayRange;
};

struct AudioSequenceEventDefinition
{
    s32 type;
    union
    {
        SoundEventDefinition* sound;
        HitMarkerEventDefinition* hitMarker;
        ParameterChangeEventDefinition* parameter;
    };
};

struct AudioSequenceEvent
{
    AudioSequenceEvent(AudioSequenceInstance* owner)
        : next(0)
        , owner(owner)
        , state(0)
        , startTime(0.0f)
    {
    }

    virtual ~AudioSequenceEvent() { }
    virtual void Play() = 0;
    virtual void Prepare() = 0;
    virtual void Stop() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual int Update(float time) = 0;
    virtual void UnidentifiedVirtual24() { }
    virtual void UnidentifiedVirtual28() { }
    virtual u32 GetSources(AudioSource** results) { return 0; }

    static AudioSequenceEvent* Create(AudioSequenceInstance* owner,
        AudioSequenceEventDefinition* definition);

    /* 0x04 */ AudioSequenceEvent* next;
    AudioSequenceInstance* owner;
    s32 state;
    float startTime;
};

struct SoundPlaybackEvent : AudioSequenceEvent
{
    SoundPlaybackEvent(AudioSequenceInstance* owner,
        SoundEventDefinition* definition);
    virtual ~SoundPlaybackEvent();
    virtual void Play()
    {
        flags = 1;
        Update(0.0f);
    }
    virtual void Prepare();
    virtual void Stop();
    virtual void Pause();
    virtual void Resume();
    virtual int Update(float time);
    virtual u32 GetSources(AudioSource** results);
    virtual void StartPlayback();

    AudioSourceInfo* SelectSourceInfo();
    void UpdatePlaybackParameters(bool force);
    void AccumulateRpcModifiers(
        bool* hasVolume, float* volume, bool* hasPitch, float* pitch);

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    SoundEventDefinition* definition;
    AudioSourceInfo* sourceInfo;
    float volumeModifier;
    float pitchModifier;
    float currentVolume;
    float currentPitch;
    AudioSource* source;
    u32 flags : 16;
    u32 savedState : 16;
};

struct HitMarkerEvent : AudioSequenceEvent
{
    HitMarkerEvent(AudioSequenceInstance* owner,
        HitMarkerEventDefinition* definition);
    virtual ~HitMarkerEvent() { }
    virtual void Play() { state = 4; }
    virtual void Prepare() { state = 2; }
    virtual void Stop() { state = 8; }
    virtual void Pause() { state = 5; }
    virtual void Resume() { state = 4; }
    virtual int Update(float time);

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    HitMarkerEventDefinition* definition;
};

struct ParameterChangeEvent : AudioSequenceEvent
{
    ParameterChangeEvent(AudioSequenceInstance* owner,
        ParameterChangeEventDefinition* definition);
    virtual ~ParameterChangeEvent() { }
    virtual void Play() { state = 4; }
    virtual void Prepare() { state = 2; }
    virtual void Stop() { state = 8; }
    virtual void Pause() { state = 5; }
    virtual void Resume() { state = 4; }
    virtual int Update(float time);

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    ParameterChangeEventDefinition* definition;
};

extern SlotPool<SoundPlaybackEvent> sSoundPlaybackEventPool;
extern SlotPool<HitMarkerEvent> sHitMarkerEventPool;
extern SlotPool<ParameterChangeEvent> sParameterChangeEventPool;

inline void* SoundPlaybackEvent::operator new(unsigned long size)
{
    return sSoundPlaybackEventPool.Allocate();
}

inline void* HitMarkerEvent::operator new(unsigned long size)
{
    return sHitMarkerEventPool.Allocate();
}

inline void* ParameterChangeEvent::operator new(unsigned long size)
{
    return sParameterChangeEventPool.Allocate();
}

inline void SoundPlaybackEvent::operator delete(void* object)
{
    sSoundPlaybackEventPool.Free((SoundPlaybackEvent*)object);
}

inline void HitMarkerEvent::operator delete(void* object)
{
    sHitMarkerEventPool.Free((HitMarkerEvent*)object);
}

inline void ParameterChangeEvent::operator delete(void* object)
{
    sParameterChangeEventPool.Free((ParameterChangeEvent*)object);
}

#endif // GAME_AUDIO_AUDIOSEQUENCEEVENT_H
