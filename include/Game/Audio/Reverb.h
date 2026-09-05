#ifndef GAME_AUDIO_REVERB_H
#define GAME_AUDIO_REVERB_H

#include "Game/Audio/AudioEffect_802F98F4.h"
#include "NL/nlSlotPool.h"

class ReverbParameter_80363D60 : public AudioEffectParameter_802F69A8
{
public:
    ReverbParameter_80363D60();
    virtual ~ReverbParameter_80363D60() { }

    static void* operator new(unsigned long)
    {
        ReverbParameter_80363D60* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((ReverbParameter_80363D60*)pointer);
    }

    bool tempDisableFX;
    float time;
    float preDelay;
    float damping;
    float coloration;
    float crosstalk;
    float mix;
    float auxvol;

    static SlotPool<ReverbParameter_80363D60> s_Pool;
};

class Reverb : public AudioEffectBase_802F98F4
{
public:
    Reverb();
    virtual ~Reverb() { }
    virtual void fn_802F6930(unsigned int, void*, bool, AudioEffectParameter_802F69A8**);
    virtual void fn_802F9B5C()
    {
        m_Final = m_Initial;
    }
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8*, AudioEffectParameter_802F69A8*);
    virtual void fn_802F9B8C();
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8*) { }
    virtual void fn_802F692C(void*);
    virtual void fn_802F98F0(void*);

    static void* operator new(unsigned long)
    {
        Reverb* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((Reverb*)pointer);
    }

    ReverbParameter_80363D60 m_Initial;
    ReverbParameter_80363D60 m_Final;

    static SlotPool<Reverb> s_Pool;
};

#endif // GAME_AUDIO_REVERB_H
