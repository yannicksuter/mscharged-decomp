#ifndef GAME_AUDIO_AUDIO_LISTENER_H
#define GAME_AUDIO_AUDIO_LISTENER_H

#include "NL/nlMath.h"
#include "types.h"

class AudioListener
{
public:
    AudioListener()
        : m_HasTransform(true), m_Enabled(true), m_TransformValid(true)
    {
        nlVec3Set(m_Position, 0.0f, 0.0f, 0.0f);
        nlVec3Set(m_View, 0.0f, 1.0f, 0.0f);
        nlVec3Set(m_Up, 0.0f, 0.0f, 1.0f);
    }

    void SetPosition(const nlVector3& position)
    {
        m_Position = position;
        m_HasTransform = true;
    }

    void SetView(const nlVector3& view)
    {
        m_View = view;
        m_HasTransform = true;
    }

    void SetUp(const nlVector3& up)
    {
        m_Up = up;
        m_HasTransform = true;
    }

    virtual void SetHasTransform(bool);
    virtual bool HasTransform();
    virtual bool IsTransformValid();
    virtual void SetTransformValid(bool valid);
    virtual void SetEnabled(bool enabled);
    virtual bool IsEnabled();
    virtual void Update(float) = 0;

    /* 0x04 */ nlVector3 m_Position;
    /* 0x10 */ nlVector3 m_View;
    /* 0x1C */ nlVector3 m_Up;
    /* 0x28 */ bool m_HasTransform;
    /* 0x29 */ bool m_Enabled;
    /* 0x2A */ bool m_TransformValid;
    /* 0x2B */ u8 m_Unknown2B;
};

#endif // GAME_AUDIO_AUDIO_LISTENER_H
