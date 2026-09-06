#ifndef GAME_AUDIO_AUDIO_RUNTIME_GROUP_802F98F4_H
#define GAME_AUDIO_AUDIO_RUNTIME_GROUP_802F98F4_H

#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"

class AudioRuntimeGroup_802F98F4;
struct AudioValue_80362B78;

class AudioState_802F69A8
{
public:
    AudioState_802F69A8()
        : m_Unknown0C(false)
    {
    }

    virtual ~AudioState_802F69A8() { }
    virtual void fn_802F6A60(float);
    virtual bool fn_802F69E8();

    union
    {
        float m_Unknown04;
        AudioRuntimeGroup_802F98F4* m_Group04;
    };
    float m_Unknown08;
    bool m_Unknown0C;
};

class AudioRuntimeGroup_802F98F4
{
public:
    AudioRuntimeGroup_802F98F4(const char*);

    virtual ~AudioRuntimeGroup_802F98F4() { }
    virtual void fn_802F6930(u32, const AudioValue_80362B78&, bool,
        AudioState_802F69A8**);
    virtual void fn_802F9B5C() { }
    virtual void fn_802F9B60(AudioState_802F69A8*, AudioState_802F69A8*) { }
    virtual void fn_802F9B8C() { }
    virtual void fn_802F9B64(AudioState_802F69A8*) { }
    virtual void fn_802F692C(u32) { }
    virtual void fn_802F98F0(u32) { }
    virtual void fn_802F98EC(u32) { }
    virtual void fn_802F9974(float);
    virtual void fn_802F9B68(AudioState_802F69A8* state) { delete state; }

    bool m_Unknown04;
    nlDLListSlotPool<AudioState_802F69A8*> m_Unknown08;
    AudioState_802F69A8* m_Unknown24;
    AudioState_802F69A8* m_Unknown28;
};

inline void AudioRuntimeGroup_802F98F4::fn_802F6930(
    u32, const AudioValue_80362B78&, bool, AudioState_802F69A8** state)
{
    static AudioState_802F69A8 value;
    *state = &value;
}

inline bool AudioState_802F69A8::fn_802F69E8()
{
    return m_Unknown0C ? m_Group04->m_Unknown04
                      : (m_Unknown08 != 0.0f
                          && (m_Unknown04 - m_Unknown08 > 0.0001f
                              || nlNear(m_Unknown04, m_Unknown08)));
}

inline void AudioState_802F69A8::fn_802F6A60(float dt)
{
    if (!m_Unknown0C && m_Unknown08 != 0.0f)
    {
        m_Unknown04 += dt;
        m_Unknown04 = (m_Unknown04 - m_Unknown08 > 0.0001f
                         || nlNear(m_Unknown04, m_Unknown08))
            ? m_Unknown08
            : m_Unknown04;
    }
}

#endif // GAME_AUDIO_AUDIO_RUNTIME_GROUP_802F98F4_H
