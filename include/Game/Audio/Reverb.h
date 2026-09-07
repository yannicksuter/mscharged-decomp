#ifndef GAME_AUDIO_REVERB_H
#define GAME_AUDIO_REVERB_H

#include "Game/Audio/AudioEffect.h"
#include "NL/nlSlotPool.h"

class ReverbParameter : public AudioEffectParameter
{
public:
    ReverbParameter();
    virtual ~ReverbParameter() { }

    static void* operator new(unsigned long)
    {
        ReverbParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((ReverbParameter*)pointer);
    }

    bool tempDisableFX;
    float time;
    float preDelay;
    float damping;
    float coloration;
    float crosstalk;
    float mix;
    float auxvol;

    static SlotPool<ReverbParameter> s_Pool;
};

class Reverb : public AudioEffectBase
{
public:
    Reverb();
    virtual ~Reverb() { }
    virtual void CreateParameter(unsigned int, void*, bool, AudioEffectParameter**);
    virtual void BeginBlend()
    {
        m_Final = m_Initial;
    }
    virtual void BlendParameter(AudioEffectParameter*, AudioEffectParameter*);
    virtual void EndBlend();
    virtual void OnParameterFinished(AudioEffectParameter*) { }
    virtual void OnSoundStarted(void*);
    virtual void ApplyToSound(void*);

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

    ReverbParameter m_Initial;
    ReverbParameter m_Final;

    static SlotPool<Reverb> s_Pool;
};

#endif // GAME_AUDIO_REVERB_H
