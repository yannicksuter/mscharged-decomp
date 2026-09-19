#ifndef GAME_AUDIO_PLAT3DSOUNDSRC_H
#define GAME_AUDIO_PLAT3DSOUNDSRC_H

#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/XSoundHandle.h"

class PlatAudioListener : public AudioListener
{
public:
    virtual void Update(float deltaTime);

    /* 0x2C */ nlVector3 m_Unknown2C;
    /* 0x38 */ nlVector3 m_Unknown38;
    /* 0x44 */ nlVector3 m_Unknown44;
};

class Plat3dSoundSrc : public XSoundOwner
{
public:
    virtual ~Plat3dSoundSrc();
    virtual void Update(PlatAudioListener* listener, float deltaTime);

    /* 0x20 */ float m_Unknown20;
    /* 0x24 */ float m_Unknown24;
    /* 0x28 */ nlVector3 m_Unknown28;
    /* 0x34 */ nlVector3 m_Unknown34;
    /* 0x40 */ float m_Unknown40;
    /* 0x44 */ u32 m_Unknown44;
};

#endif // GAME_AUDIO_PLAT3DSOUNDSRC_H
