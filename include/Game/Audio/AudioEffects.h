#ifndef GAME_AUDIO_AUDIO_EFFECTS_H
#define GAME_AUDIO_AUDIO_EFFECTS_H

#include "Game/Audio/AudioEffect.h"
#include "NL/nlArrayAllocator.h"
#include "NL/nlSlotPool.h"
#include <string.h>

enum eAudioEffectID
{
    AUDIO_EFFECT_VOLUME = 0xCE5C5677,
    AUDIO_EFFECT_CONTROLLER_SPEAKER = 0x7DDB838E,
    AUDIO_EFFECT_REVERB = 0xC457F745,
    AUDIO_EFFECT_DELAY = 0x04F5A46E,
    AUDIO_EFFECT_LOW_PASS_FILTER = 0x0BCF338E,
    AUDIO_EFFECT_PITCH = 0x05D11E37,
    AUDIO_EFFECT_CATEGORY_VOLUME = 0xFA8EC255,
};

class AudioEffectFactory
{
public:
    virtual ~AudioEffectFactory() { }
    virtual void Initialize();
    virtual void Update(float);
    virtual void Shutdown();
    virtual AudioEffectBase* CreateEffect(unsigned int effectId);
    virtual void ReleaseEffect(AudioEffectBase* effect);
    virtual bool IsInitialized();
};

class VolumeParameter : public AudioEffectParameter
{
public:
    virtual ~VolumeParameter() { }

    static void* operator new(unsigned long)
    {
        VolumeParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((VolumeParameter*)pointer);
    }

    float m_VolumeOffset;
    unsigned int m_PauseOnZero : 1;
    unsigned int m_StopOnZero : 1;
    unsigned int m_Unknown14_02 : 30;

    static SlotPool<VolumeParameter> s_Pool;
};

class Volume : public AudioEffectBase
{
public:
    Volume()
        : AudioEffectBase("Volume")
    {
        m_Initial.m_VolumeOffset = 0.0f;
        m_Initial.m_StopOnZero = m_Initial.m_PauseOnZero = false;
        m_CurrentParameter = &m_Initial;
        m_ResultParameter = &m_Final;
    }
    virtual void CreateParameter(unsigned int, const void*, bool, AudioEffectParameter**);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter*, AudioEffectParameter*);
    virtual void OnParameterFinished(AudioEffectParameter*);
    virtual void ApplyToSound(void*);

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

    VolumeParameter m_Initial;
    VolumeParameter m_Final;

    static SlotPool<Volume> s_Pool;
};

class ControllerSpeakerParameter : public AudioEffectParameter
{
public:
    virtual ~ControllerSpeakerParameter() { }
};

class ControllerSpeaker : public AudioEffectBase
{
public:
    ControllerSpeaker()
        : AudioEffectBase("ControllerSpeaker")
        , m_Channel(-1)
        , m_ActiveSoundCount(0)
    {
        memset(m_Unknown44, 0, sizeof(m_Unknown44));
    }
    virtual void CreateParameter(unsigned int, const void*, bool, AudioEffectParameter**);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter*, AudioEffectParameter*);
    virtual void OnParameterFinished(AudioEffectParameter*);
    virtual void OnSoundStarted(void*);
    virtual void ApplyToSound(void*);
    virtual void OnSoundStopped(void*);

    static void* operator new(unsigned long)
    {
        return s_Allocator.Allocate();
    }

    static void operator delete(void* pointer)
    {
        s_Allocator.DeleteEntry((ControllerSpeaker*)pointer);
    }

    ControllerSpeakerParameter m_Parameter;
    int m_Channel;
    int m_ActiveSoundCount;
    unsigned int m_Unknown44[8];

    static nlArrayAllocator<ControllerSpeaker> s_Allocator;
};

void SetControllerSpeakerEnabled(bool enabled);
AudioEffectFactory* GetAudioEffectFactory();

#endif // GAME_AUDIO_AUDIO_EFFECTS_H
