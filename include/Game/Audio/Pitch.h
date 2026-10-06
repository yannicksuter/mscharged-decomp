#ifndef GAME_AUDIO_PITCH_H
#define GAME_AUDIO_PITCH_H

#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "Game/Audio/AudioEffect.h"
#include "Game/Audio/AudioConfig.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

class PitchParameter : public AudioEffectParameter
{
public:
    PitchParameter();
    virtual ~PitchParameter();

    static void* operator new(unsigned long)
    {
        PitchParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((PitchParameter*)pointer);
    }

    float m_Semitones;

    static SlotPool<PitchParameter> s_Pool;
};

class Pitch : public AudioEffectBase
{
public:
    virtual void CreateParameter(unsigned int definition, const void* context,
        bool negate, AudioEffectParameter** output);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);
    virtual void EndBlend();
    virtual void OnParameterFinished(AudioEffectParameter* parameter);
    virtual void ApplyToSound(void* handle);

    static void* operator new(unsigned long)
    {
        Pitch* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((Pitch*)pointer);
    }

    PitchParameter m_Initial;
    PitchParameter m_Final;

    static SlotPool<Pitch> s_Pool;
};

#endif // GAME_AUDIO_PITCH_H
