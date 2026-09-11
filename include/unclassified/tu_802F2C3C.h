#ifndef UNCLASSIFIED_TU_802F2C3C_H
#define UNCLASSIFIED_TU_802F2C3C_H

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

struct ModifierDefinition_802F2C3C
{
    u8 pad_00[0xC];
    u32 kind;
};

struct ModifierNode_802F2C3C
{
    ModifierDefinition_802F2C3C* definition;
    float value;
};

struct RpcListEntry_802F2C3C
{
    RpcListEntry_802F2C3C* next;
    RpcListEntry_802F2C3C* previous;
    ModifierNode_802F2C3C* node;
};

struct SoundDefinition_802F2C3C
{
    u8 pad_00[0x24];
    u32 modifierCount;
    ModifierNode_802F2C3C** modifiers;
};

struct SoundInstance_802F2C3C
{
    XSoundHandle* owner;
    SoundDefinition_802F2C3C* definition;
    void* voices;
    SlotPoolBase* entryPool;
    RpcListEntry_802F2C3C* rpcEntries;
    s32 state;
    float previousTime;
    float currentTime;
    u8 pad_20[0x30];
    float pitch;
};

struct PlaybackSequence_802F2C3C
{
    u32 field_00;
    u32 count;
    PlaybackRequest_802F2C3C* requests;
};

struct PlaybackOwner_802F2C3C
{
    /* 0x00 */ PlaybackOwner_802F2C3C* next;
    /* 0x04 */ SoundInstance_802F2C3C* instance;
    /* 0x08 */ PlaybackSequence_802F2C3C* sequence;
    /* 0x0C */ PlaybackObject_802F2C3C* objects;
    /* 0x10 */ float volumeOffset;
    /* 0x14 */ float pitchOffset;
    /* 0x18 */ bool started;
}; // size: 0x1C

// Shared pool the playback owners come from; the unit that defines it also
// defines their constructor and destructor.
extern SlotPool<PlaybackOwner_802F2C3C> sPlaybackOwnerPool_802F3E20;

struct PlaybackObject_802F2C3C
{
    PlaybackObject_802F2C3C(PlaybackOwner_802F2C3C* owner)
        : next(0), owner(owner), state(0), startTime(0.0f)
    {
    }

    virtual ~PlaybackObject_802F2C3C();
    virtual void UnidentifiedVirtual0C() = 0;
    virtual void UnidentifiedVirtual10() = 0;
    virtual void UnidentifiedVirtual14() = 0;
    virtual void UnidentifiedVirtual18() = 0;
    virtual void UnidentifiedVirtual1C() = 0;
    virtual int UnidentifiedVirtual20(float time) = 0;
    virtual void UnidentifiedVirtual24() { }
    virtual void UnidentifiedVirtual28() { }
    virtual u32 UnidentifiedVirtual2C(u32* results) { return 0; }

    /* 0x04 */ PlaybackObject_802F2C3C* next;
    PlaybackOwner_802F2C3C* owner;
    s32 state;
    float startTime;
};

struct PlaybackObject_8052F7B0 : PlaybackObject_802F2C3C
{
    PlaybackObject_8052F7B0(PlaybackOwner_802F2C3C* owner,
        PlaybackRequest_802F2C3C* request);
    virtual ~PlaybackObject_8052F7B0();
    virtual void UnidentifiedVirtual0C()
    {
        flags = 1;
        UnidentifiedVirtual20(0.0f);
    }
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual int UnidentifiedVirtual20(float time);
    virtual u32 UnidentifiedVirtual2C(u32* results);
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
    PlaybackObject_8052F780(PlaybackOwner_802F2C3C* owner,
        PlaybackRequest_802F2C3C* request);
    virtual ~PlaybackObject_8052F780() { }
    virtual void UnidentifiedVirtual0C() { state = 4; }
    virtual void UnidentifiedVirtual10() { state = 2; }
    virtual void UnidentifiedVirtual14() { state = 8; }
    virtual void UnidentifiedVirtual18() { state = 5; }
    virtual void UnidentifiedVirtual1C() { state = 4; }
    virtual int UnidentifiedVirtual20(float time);

    static void* operator new(unsigned long size);
    static void operator delete(void* object);

    PlaybackDefinition_802F2C3C* definition;
};

struct PlaybackObject_8052F750 : PlaybackObject_802F2C3C
{
    PlaybackObject_8052F750(PlaybackOwner_802F2C3C* owner,
        PlaybackRequest_802F2C3C* request);
    virtual ~PlaybackObject_8052F750() { }
    virtual void UnidentifiedVirtual0C() { state = 4; }
    virtual void UnidentifiedVirtual10() { state = 2; }
    virtual void UnidentifiedVirtual14() { state = 8; }
    virtual void UnidentifiedVirtual18() { state = 5; }
    virtual void UnidentifiedVirtual1C() { state = 4; }
    virtual int UnidentifiedVirtual20(float time);

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
