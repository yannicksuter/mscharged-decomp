#ifndef GAME_BLINKER_H
#define GAME_BLINKER_H

#include "types.h"

class glModel;

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
    Blinker(unsigned long arg0, unsigned long arg1, unsigned long arg2);
    void Blink(glModel* model);
    void Update(float fDeltaT);

    /* 0x00 */ unsigned long unknown_0x00[4];
    /* 0x10 */ unsigned long m_Textures[4];
    /* 0x20 */ float m_fTime;
    /* 0x24 */ eBlinkState m_State;
    /* 0x28 */ float m_fBlinkTimes[4];
    /* 0x38 */ bool m_bJustDoubleBlinked;
    /* 0x39 */ u8 unknown_0x39[3];
};

#endif // GAME_BLINKER_H
