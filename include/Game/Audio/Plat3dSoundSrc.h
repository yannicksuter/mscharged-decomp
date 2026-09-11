#ifndef GAME_AUDIO_PLAT3DSOUNDSRC_H
#define GAME_AUDIO_PLAT3DSOUNDSRC_H

#include "Game/Audio/AudioSystem.h"

class PlatAudioListener : public AudioListener
{
public:
    virtual void Update(float deltaTime);

    /* 0x2C */ nlVector3 m_Unknown2C;
    /* 0x38 */ nlVector3 m_Unknown38;
    /* 0x44 */ nlVector3 m_Unknown44;
};

class Plat3dSoundSrc
{
public:
    Plat3dSoundSrc()
    {
        nlVec3Set(m_Unknown04.m_Value, 0.0f, 0.0f, 0.0f);
        count.references = 0;
        count.field_8000 = count.field_4000 = count.field_2000 = count.field_1000 = false;
    }

    virtual ~Plat3dSoundSrc();
    virtual void Update(PlatAudioListener* listener, float deltaTime);

    /* 0x04 */ union
    {
        nlVector3 m_Value;
        const nlVector3* m_Pointer;
    } m_Unknown04;
    /* 0x10 */ float m_Unknown10;
    /* 0x14 */ float m_Unknown14;
    /* 0x18 */ float m_Unknown18;
    /* 0x1C */ union
    {
        u32 m_Unknown1C;
        struct
        {
            s32 references : 16;
            u32 field_8000 : 1;
            u32 field_4000 : 1;
            u32 field_2000 : 1;
            u32 field_1000 : 1;
            u32 field_0FFF : 12;
        } count;
    };
    /* 0x20 */ float m_Unknown20;
    /* 0x24 */ float m_Unknown24;
    /* 0x28 */ nlVector3 m_Unknown28;
    /* 0x34 */ nlVector3 m_Unknown34;
    /* 0x40 */ float m_Unknown40;
    /* 0x44 */ u32 m_Unknown44;
};

#endif // GAME_AUDIO_PLAT3DSOUNDSRC_H
