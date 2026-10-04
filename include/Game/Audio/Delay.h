#ifndef GAME_AUDIO_DELAY_H
#define GAME_AUDIO_DELAY_H

#include "Game/Audio/AudioEffect.h"
#include "NL/nlSlotPool.h"

struct AXFX_DELAY;

class DelayParameter : public AudioEffectParameter
{
public:
    DelayParameter();
    virtual ~DelayParameter() { }
    void ApplySettings(AXFX_DELAY* delay);

    static void* operator new(unsigned long)
    {
        DelayParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((DelayParameter*)pointer);
    }

    u32 m_Delay[3];
    u32 m_Feedback[3];
    u32 m_Output[3];
    float m_AuxVolume;

    static SlotPool<DelayParameter> s_Pool;
};

class Delay : public AudioEffectBase
{
public:
    Delay();
    virtual void CreateParameter(unsigned int definition, const void* context, bool negate,
        AudioEffectParameter** output);
    virtual void BeginBlend()
    {
        m_Final = m_Initial;
    }
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);
    virtual void EndBlend();
    virtual void OnParameterFinished(AudioEffectParameter*) { }
    virtual void OnSoundStarted(void*);
    virtual void ApplyToSound(void* handle);

    static void* operator new(unsigned long)
    {
        Delay* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((Delay*)pointer);
    }

    DelayParameter m_Initial;
    DelayParameter m_Final;

    static SlotPool<Delay> s_Pool;
};

extern bool gDelayOverrideEnabled;
extern float gDelayOverrideVolume;

void SetDefaultDelaySettings(AXFX_DELAY* delay);

#endif // GAME_AUDIO_DELAY_H
