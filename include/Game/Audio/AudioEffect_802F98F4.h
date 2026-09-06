#ifndef GAME_AUDIO_AUDIOEFFECT_802F98F4_H
#define GAME_AUDIO_AUDIOEFFECT_802F98F4_H

#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "types.h"

class AudioEffectParameter_802F69A8
{
public:
    AudioEffectParameter_802F69A8()
    {
        m_State.m_Flags.bytes[0] = false;
    }

    virtual ~AudioEffectParameter_802F69A8() { }
    virtual void fn_802F6A60(float);
    virtual bool fn_802F69E8();

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

class AudioEffectBase_802F98F4
{
public:
    AudioEffectBase_802F98F4(const char*);

    virtual ~AudioEffectBase_802F98F4() { }
    virtual void fn_802F6930(unsigned int, void*, bool,
        AudioEffectParameter_802F69A8**);
    virtual void fn_802F9B5C() { }
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8*,
        AudioEffectParameter_802F69A8*) { }
    virtual void fn_802F9B8C() { }
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8*) { }
    virtual void fn_802F692C(void*) { }
    virtual void fn_802F98F0(void*) { }
    virtual void fn_802F98EC() { }
    virtual void fn_802F9974(float);
    virtual void fn_802F9B68(AudioEffectParameter_802F69A8* state) { delete state; }

    bool m_Enabled;
    u8 m_Pad05[3];
    nlDLListSlotPool<AudioEffectParameter_802F69A8*> m_Parameters;
    AudioEffectParameter_802F69A8* m_CurrentParameter;
    AudioEffectParameter_802F69A8* m_ResultParameter;
};

class AuxEffectMap_8035952C
{
public:
    AuxEffectMap_8035952C();
    int fn_80359544(const int& effect);
    int fn_80359590(const int& effect) const;

    int m_Effects[2];
    int m_Indices[2];
};

inline void AudioEffectBase_802F98F4::fn_802F6930(
    unsigned int, void*, bool, AudioEffectParameter_802F69A8** state)
{
    static AudioEffectParameter_802F69A8 value;
    *state = &value;
}

inline bool AudioEffectParameter_802F69A8::fn_802F69E8()
{
    return m_State.m_Flags.bytes[0]
        ? ((AudioEffectBase_802F98F4*)m_State.m_Current.pointer)->m_Enabled
        : (m_State.m_Target.scalar != 0.0f
            && (m_State.m_Current.scalar - m_State.m_Target.scalar > 0.0001f
                || nlNear(m_State.m_Current.scalar, m_State.m_Target.scalar)));
}

inline void AudioEffectParameter_802F69A8::fn_802F6A60(float dt)
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

#endif // GAME_AUDIO_AUDIOEFFECT_802F98F4_H
