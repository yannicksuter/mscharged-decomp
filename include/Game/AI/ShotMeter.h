#ifndef GAME_AI_SHOT_METER_H
#define GAME_AI_SHOT_METER_H

#include "types.h"

class cFielder;

enum eShotMeterState
{
    SHOT_METER_INACTIVE = 0,
    SHOT_METER_ACTIVE = 1,
    SHOT_METER_RELEASED = 2,
    SHOT_METER_STS_ACTIVE = 3,
    SHOT_METER_STS_TRANSISTION = 4,
    SHOT_METER_STS_RELEASED = 5,
};

class ShotMeter
{
public:
    void Reset(cFielder* pFielder);

    /* 0x00 */ eShotMeterState m_eShotMeterState;
    /* 0x04 */ float m_fTime;
    /* 0x08 */ float m_fScoreValue;
    /* 0x0C */ float m_fSpeedValue;
    /* 0x10 */ float m_fSTSValue;
    /* 0x14 */ float mfSShotAimValue;
    /* 0x18 */ u8 mUnknown018[8];
}; // total size: 0x20

#endif // GAME_AI_SHOT_METER_H
