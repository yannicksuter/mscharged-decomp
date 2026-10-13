#ifndef GAME_AUDIO_AUDIOEFFECT_H
#define GAME_AUDIO_AUDIOEFFECT_H

#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "types.h"

enum eAudioAuxiliary
{
    AUDIO_AUX_A = 0,
    AUDIO_AUX_B = 1,
};

enum eAudioAuxEffect
{
    AUDIO_AUX_DELAY = 0,
    AUDIO_AUX_REVERB = 1,
};

class AudioEffectParameter
{
public:
    AudioEffectParameter()
    {
#if !defined(VERSION_R4QP01_REV1)
        m_State.m_Flags.bytes[0] = false;
#endif
    }

    virtual ~AudioEffectParameter() { }
    virtual void Update(float);
    virtual bool IsFinished();

    float GetTargetScalar() const { return m_State.m_Target.scalar; }
    float GetCurrentScalar() const { return m_State.m_Current.scalar; }

    bool IsOwnerBound() const
    {
#if defined(VERSION_R4QP01_REV1)
        // Revision 1 stores the owner flag in the low bit of the target word.
        return (m_State.m_Target.word & 1) != 0;
#else
        return m_State.m_Flags.bytes[0] != 0;
#endif
    }

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
#if !defined(VERSION_R4QP01_REV1)
        union Flags
        {
            u32 word;
            u8 bytes[4];
        } m_Flags;
#endif
    } m_State;
};

class AudioEffectBase
{
public:
    AudioEffectBase(const char*);

    virtual ~AudioEffectBase() { }
    virtual void CreateParameter(unsigned int, const void*, bool,
        AudioEffectParameter**);
    virtual void BeginBlend() { }
    virtual void BlendParameter(AudioEffectParameter*,
        AudioEffectParameter*) { }
    virtual void EndBlend() { }
    virtual void OnParameterFinished(AudioEffectParameter*) { }
    virtual void OnSoundStarted(void*) { }
    virtual void ApplyToSound(void*) { }
    virtual void OnSoundStopped(void*) { }
    virtual void Update(float);
    virtual void ReleaseParameter(AudioEffectParameter* state) { delete state; }

    AudioEffectParameter* CreateParameter(unsigned int definition, const void* data,
        bool immediate)
    {
        AudioEffectParameter* parameter = 0;
        CreateParameter(definition, data, immediate, &parameter);
        return parameter;
    }

    bool AddParameter(AudioEffectParameter* parameter, float value)
    {
        parameter->m_State.m_Target.scalar = value;
        parameter->m_State.m_Current.scalar = 0.0f;
        m_Parameters.AddEnd(parameter);
        return true;
    }

    bool AddParameter(AudioEffectParameter* parameter, void* owner)
    {
        parameter->m_State.m_Current.pointer = owner;
#if defined(VERSION_R4QP01_REV1)
        parameter->m_State.m_Target.word |= 1;
#else
        parameter->m_State.m_Flags.bytes[0] = true;
#endif
        m_Parameters.AddEnd(parameter);
        return true;
    }

    bool m_Finished;
    u8 m_Pad05[3];
    nlDLListSlotPool<AudioEffectParameter*> m_Parameters;
    AudioEffectParameter* m_CurrentParameter;
    AudioEffectParameter* m_ResultParameter;
};

class AuxEffectMap
{
public:
    AuxEffectMap();
    ~AuxEffectMap() { }
    int AssignAuxiliary(const int& effect);
    int GetAuxiliary(const int& effect) const;

    int& GetAuxiliaryIndex(const int& effect)
    {
        return m_Indices[effect];
    }

    int m_Effects[2];
    int m_Indices[2];
};

inline void AudioEffectBase::CreateParameter(
    unsigned int, const void*, bool, AudioEffectParameter** state)
{
    static AudioEffectParameter value;
    *state = &value;
}

inline bool AudioEffectParameter::IsFinished()
{
    return IsOwnerBound()
        ? ((AudioEffectBase*)m_State.m_Current.pointer)->m_Finished
        : (GetTargetScalar()
            && (GetCurrentScalar() - GetTargetScalar() > 0.0001f
                || nlNear(GetCurrentScalar(), GetTargetScalar())));
}

inline void AudioEffectParameter::Update(float dt)
{
    if (!IsOwnerBound() && m_State.m_Target.scalar != 0.0f)
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

inline void* AllocateAudioEffectMemory(unsigned long size)
{
    return nlMalloc(size, 8, false);
}

inline void FreeAudioEffectMemory(void* pointer)
{
    nlFree(pointer);
}

#endif // GAME_AUDIO_AUDIOEFFECT_H
