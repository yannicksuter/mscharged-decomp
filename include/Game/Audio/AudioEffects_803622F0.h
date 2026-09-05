#ifndef GAME_AUDIO_AUDIO_EFFECTS_803622F0_H
#define GAME_AUDIO_AUDIO_EFFECTS_803622F0_H

#include "Game/Audio/AudioEffect_802F98F4.h"
#include "NL/nlArrayAllocator.h"
#include "NL/nlSlotPool.h"

class AudioEffectFactory_803622F8
{
public:
    virtual ~AudioEffectFactory_803622F8();
    virtual void fn_0C();
    virtual void fn_10(float);
    virtual void fn_14();
    virtual AudioEffectBase_802F98F4* fn_18(unsigned int);
    virtual void fn_1C(AudioEffectBase_802F98F4*);
    virtual bool fn_20();
};

class VolumeParameter_80362EC8 : public AudioEffectParameter_802F69A8
{
public:
    virtual ~VolumeParameter_80362EC8();

    static void* operator new(unsigned long)
    {
        VolumeParameter_80362EC8* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((VolumeParameter_80362EC8*)pointer);
    }

    float m_Unknown10;
    unsigned int m_Unknown14_00 : 1;
    unsigned int m_Unknown14_01 : 1;
    unsigned int m_Unknown14_02 : 30;

    static SlotPool<VolumeParameter_80362EC8> s_Pool;
};

class Volume : public AudioEffectBase_802F98F4
{
public:
    Volume();
    virtual ~Volume();
    virtual void fn_802F6930(unsigned int, void*, bool, AudioEffectParameter_802F69A8**);
    virtual void fn_802F9B5C();
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8*, AudioEffectParameter_802F69A8*);
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8*);
    virtual void fn_802F98F0(void*);

    static void* operator new(unsigned long)
    {
        Volume* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((Volume*)pointer);
    }

    VolumeParameter_80362EC8 m_Initial;
    VolumeParameter_80362EC8 m_Final;

    static SlotPool<Volume> s_Pool;
};

class ControllerSpeakerParameter_80362EF0 : public AudioEffectParameter_802F69A8
{
public:
    virtual ~ControllerSpeakerParameter_80362EF0();
};

class ControllerSpeaker : public AudioEffectBase_802F98F4
{
public:
    ControllerSpeaker();
    virtual ~ControllerSpeaker();
    virtual void fn_802F6930(unsigned int, void*, bool, AudioEffectParameter_802F69A8**);
    virtual void fn_802F9B5C();
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8*, AudioEffectParameter_802F69A8*);
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8*);
    virtual void fn_802F692C(void*);
    virtual void fn_802F98F0(void*);
    virtual void fn_802F98EC();

    static void* operator new(unsigned long)
    {
        return s_Allocator.Allocate();
    }

    static void operator delete(void* pointer)
    {
        s_Allocator.DeleteEntry((ControllerSpeaker*)pointer);
    }

    ControllerSpeakerParameter_80362EF0 m_Parameter;
    int m_Unknown3C;
    int m_Unknown40;
    unsigned int m_Unknown44[8];

    static nlArrayAllocator<ControllerSpeaker> s_Allocator;
};

extern "C" void fn_803622F0(bool);
extern "C" AudioEffectFactory_803622F8* fn_803622F8();

#endif // GAME_AUDIO_AUDIO_EFFECTS_803622F0_H
