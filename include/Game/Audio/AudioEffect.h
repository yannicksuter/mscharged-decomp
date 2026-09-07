#ifndef GAME_AUDIO_AUDIOEFFECT_H
#define GAME_AUDIO_AUDIOEFFECT_H

#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "types.h"
struct AXFX_DELAY;
struct AXFX_REVERBHI;

class AudioEffectParameter
{
public:
    AudioEffectParameter()
    {
        m_State.m_Flags.bytes[0] = false;
    }

    virtual ~AudioEffectParameter() { }
    virtual void Update(float);
    virtual bool IsFinished();

    struct State
    {
        union Value
        {
            u32 word;
            float scalar;
            void* pointer;
        };

        Value m_Current;
        Value m_Target;
        union Flags
        {
            u32 word;
            u8 bytes[4];
        } m_Flags;
    } m_State;
};

class AudioEffectBase
{
public:
    AudioEffectBase(const char*);

    virtual ~AudioEffectBase() { }
    virtual void CreateParameter(unsigned int, void*, bool,
        AudioEffectParameter**);
    virtual void BeginBlend() { }
    virtual void BlendParameter(AudioEffectParameter*,
        AudioEffectParameter*) { }
    virtual void EndBlend() { }
    virtual void OnParameterFinished(AudioEffectParameter*) { }
    virtual void OnSoundStarted(void*) { }
    virtual void ApplyToSound(void*) { }
    virtual void OnSoundStopped() { }
    virtual void Update(float);
    virtual void ReleaseParameter(AudioEffectParameter* state) { delete state; }

    bool m_Enabled;
    u8 m_Pad05[3];
    nlDLListSlotPool<AudioEffectParameter*> m_Parameters;
    AudioEffectParameter* m_CurrentParameter;
    AudioEffectParameter* m_ResultParameter;
};

class AuxEffectMap
{
public:
    AuxEffectMap();
    int AssignAuxiliary(const int& effect);
    int GetAuxiliary(const int& effect) const;

    int m_Effects[2];
    int m_Indices[2];
};

inline void AudioEffectBase::CreateParameter(
    unsigned int, void*, bool, AudioEffectParameter** state)
{
    static AudioEffectParameter value;
    *state = &value;
}

inline bool AudioEffectParameter::IsFinished()
{
    return m_State.m_Flags.bytes[0]
        ? ((AudioEffectBase*)m_State.m_Current.pointer)->m_Enabled
        : (m_State.m_Target.scalar != 0.0f
            && (m_State.m_Current.scalar - m_State.m_Target.scalar > 0.0001f
                || nlNear(m_State.m_Current.scalar, m_State.m_Target.scalar)));
}

inline void AudioEffectParameter::Update(float dt)
{
    if (!m_State.m_Flags.bytes[0] && m_State.m_Target.scalar != 0.0f)
    {
        m_State.m_Current.scalar += dt;
        m_State.m_Current.scalar =
            (m_State.m_Current.scalar - m_State.m_Target.scalar > 0.0001f
                || nlNear(m_State.m_Current.scalar, m_State.m_Target.scalar))
            ? m_State.m_Target.scalar
            : m_State.m_Current.scalar;
    }
}

extern AuxEffectMap* g_pAuxEffectMap;
void SetDefaultDelaySettings(AXFX_DELAY* delay);
void SetDefaultReverbSettings(AXFX_REVERBHI* reverb);

#endif // GAME_AUDIO_AUDIOEFFECT_H
