#ifndef GAME_AUDIO_LOW_PASS_FILTER_H
#define GAME_AUDIO_LOW_PASS_FILTER_H

#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioEffect.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

class LowPassFilterParameter : public AudioEffectParameter
{
public:
    LowPassFilterParameter();

    static void* operator new(unsigned long)
    {
        LowPassFilterParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((LowPassFilterParameter*)pointer);
    }

    u32 m_On;
    u32 m_Frequency;

    static SlotPool<LowPassFilterParameter> s_Pool;
};

class LowPassFilter : public AudioEffectBase
{
public:
    virtual void CreateParameter(unsigned int definition, const void* context,
        bool disabled, AudioEffectParameter** output);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);
    virtual void EndBlend();
    virtual void OnParameterFinished(AudioEffectParameter* parameter);
    virtual void ApplyToSound(void* handle);

    static void* operator new(unsigned long)
    {
        LowPassFilter* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((LowPassFilter*)pointer);
    }

    LowPassFilterParameter m_Initial;
    LowPassFilterParameter m_Final;
    u32 m_FilterCount;

    static SlotPool<LowPassFilter> s_Pool;
};

#endif // GAME_AUDIO_LOW_PASS_FILTER_H
