#ifndef GAME_AUDIO_AUDIO_EFFECTS_H
#define GAME_AUDIO_AUDIO_EFFECTS_H

#include "Game/Audio/AudioEffect.h"
#include "Game/Audio/AudioGlobals.h"
#include "NL/nlArrayAllocator.h"
#include "NL/nlSlotPool.h"

class AudioEffectFactory
{
public:
    virtual ~AudioEffectFactory();
    virtual void Initialize();
    virtual void Update(float);
    virtual void Shutdown();
    virtual AudioEffectBase* CreateEffect(unsigned int);
    virtual void ReleaseEffect(AudioEffectBase*);
    virtual bool IsInitialized();
};

class VolumeParameter : public AudioEffectParameter
{
public:
    virtual ~VolumeParameter();

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

    float m_Unknown10;
    unsigned int m_Unknown14_00 : 1;
    unsigned int m_Unknown14_01 : 1;
    unsigned int m_Unknown14_02 : 30;

    static SlotPool<VolumeParameter> s_Pool;
};

class Volume : public AudioEffectBase
{
public:
    Volume();
    virtual ~Volume();
    virtual void CreateParameter(unsigned int, void*, bool, AudioEffectParameter**);
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
    virtual ~ControllerSpeakerParameter();
};

class ControllerSpeaker : public AudioEffectBase
{
public:
    ControllerSpeaker();
    virtual ~ControllerSpeaker();
    virtual void CreateParameter(unsigned int, void*, bool, AudioEffectParameter**);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter*, AudioEffectParameter*);
    virtual void OnParameterFinished(AudioEffectParameter*);
    virtual void OnSoundStarted(void*);
    virtual void ApplyToSound(void*);
    virtual void OnSoundStopped();

    static void* operator new(unsigned long)
    {
        return s_Allocator.Allocate();
    }

    static void operator delete(void* pointer)
    {
        s_Allocator.DeleteEntry((ControllerSpeaker*)pointer);
    }

    ControllerSpeakerParameter m_Parameter;
    int m_Unknown3C;
    int m_Unknown40;
    unsigned int m_Unknown44[8];

    static nlArrayAllocator<ControllerSpeaker> s_Allocator;
};

AudioEffectFactory* GetAudioEffectFactory();

#endif // GAME_AUDIO_AUDIO_EFFECTS_H
