#ifndef UNCLASSIFIED_TU_802F2C3C_H
#define UNCLASSIFIED_TU_802F2C3C_H

#include "Game/Audio/AudioSequenceInstance.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class AudioSource;
class XSoundHandle;

struct PlaybackObject_802F2C3C;

// Sound playback object family of the audio script player.

struct WeightedEntry_802F2C3C
{
    u32 index;
    u32 weight;
};

struct PlaybackDefinition_802F2C3C
{
    u32 field_00;
    u32 soundId;
    union
    {
        u32 choiceCount;
        float value;
    };
    u8 mode;
    u8 pad_0D[3];
    WeightedEntry_802F2C3C* choices;
    u8 randomPitch;
    u8 randomVolume;
    u8 pad_16[2];
    float pitchMinimum;
    float pitchMaximum;
    float volumeMinimum;
    float volumeMaximum;
};

struct PlaybackRequest_802F2C3C
{
    u32 kind;
    PlaybackDefinition_802F2C3C* definition;
};

struct PlaybackObject_802F2C3C
{
    PlaybackObject_802F2C3C(AudioSequenceInstance* owner)
        : next(0), owner(owner), state(0), startTime(0.0f)
    {
    }

    virtual ~PlaybackObject_802F2C3C();
    virtual void Play() = 0;
    virtual void Prepare() = 0;
    virtual void Stop() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual int Update(float time) = 0;
    virtual void UnidentifiedVirtual24() { }
    virtual void UnidentifiedVirtual28() { }
    virtual u32 GetSources(AudioSource** results) { return 0; }

    /* 0x04 */ PlaybackObject_802F2C3C* next;
    AudioSequenceInstance* owner;
    s32 state;
    float startTime;
};

struct PlaybackObject_8052F7B0 : PlaybackObject_802F2C3C
{
    PlaybackObject_8052F7B0(AudioSequenceInstance* owner,
        PlaybackRequest_802F2C3C* request);
    virtual ~PlaybackObject_8052F7B0();
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
    virtual void UnidentifiedVirtual30();

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    PlaybackDefinition_802F2C3C* definition;
    u32 selection;
    float volumeModifier;
    float pitchModifier;
    float currentVolume;
    float currentPitch;
    AudioSource* backend;
    u32 flags : 16;
    u32 savedState : 16;
};

struct PlaybackObject_8052F780 : PlaybackObject_802F2C3C
{
    PlaybackObject_8052F780(AudioSequenceInstance* owner,
        PlaybackRequest_802F2C3C* request);
    virtual ~PlaybackObject_8052F780() { }
    virtual void Play() { state = 4; }
    virtual void Prepare() { state = 2; }
    virtual void Stop() { state = 8; }
    virtual void Pause() { state = 5; }
    virtual void Resume() { state = 4; }
    virtual int Update(float time);

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    PlaybackDefinition_802F2C3C* definition;
};

struct PlaybackObject_8052F750 : PlaybackObject_802F2C3C
{
    PlaybackObject_8052F750(AudioSequenceInstance* owner,
        PlaybackRequest_802F2C3C* request);
    virtual ~PlaybackObject_8052F750() { }
    virtual void Play() { state = 4; }
    virtual void Prepare() { state = 2; }
    virtual void Stop() { state = 8; }
    virtual void Pause() { state = 5; }
    virtual void Resume() { state = 4; }
    virtual int Update(float time);

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    PlaybackDefinition_802F2C3C* definition;
};

extern SlotPool<PlaybackObject_8052F7B0> lbl_8057FAE8;
extern SlotPool<PlaybackObject_8052F780> lbl_8057FB10;
extern SlotPool<PlaybackObject_8052F750> lbl_8057FB38;

inline void* PlaybackObject_8052F7B0::operator new(unsigned long size)
{
    return lbl_8057FAE8.Allocate();
}

inline void* PlaybackObject_8052F780::operator new(unsigned long size)
{
    return lbl_8057FB10.Allocate();
}

inline void* PlaybackObject_8052F750::operator new(unsigned long size)
{
    return lbl_8057FB38.Allocate();
}

inline void PlaybackObject_8052F7B0::operator delete(void* object)
{
    lbl_8057FAE8.Free((PlaybackObject_8052F7B0*)object);
}

inline void PlaybackObject_8052F780::operator delete(void* object)
{
    lbl_8057FB10.Free((PlaybackObject_8052F780*)object);
}

inline void PlaybackObject_8052F750::operator delete(void* object)
{
    lbl_8057FB38.Free((PlaybackObject_8052F750*)object);
}

#endif // UNCLASSIFIED_TU_802F2C3C_H
