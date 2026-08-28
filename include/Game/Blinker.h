#ifndef GAME_BLINKER_H
#define GAME_BLINKER_H

#include "NL/gl/glModel.h"

enum eBlinkState
{
    Blink_Open = 0,
    Blink_HalfClosed = 1,
    Blink_Closed = 2,
    Blink_HalfOpen = 3,
    Blink_Num = 4,
};

class Blinker
{
public:
    Blinker(unsigned long texture0, unsigned long texture1,
        unsigned long texture2);

    void Blink(glModel* model);
    void Update(float fDeltaT);

    /* 0x00 */ unsigned long m_Textures[4];
    /* 0x10 */ unsigned long m_ResolvedTextures[4];
    /* 0x20 */ float m_fTime;
    /* 0x24 */ eBlinkState m_State;
    /* 0x28 */ float m_fBlinkTimes[4];
    /* 0x38 */ bool m_bJustDoubleBlinked;
    /* 0x39 */ u8 unknown_0x39[3];
};

#endif // GAME_BLINKER_H
