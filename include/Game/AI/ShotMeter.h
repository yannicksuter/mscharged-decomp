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
    void Update(float fDeltaT);
    void Abort();
    void CalcOneTimerValue(cFielder* pFielder, bool bWasPerfectPass);
    void CalcShotAim(cFielder* pFielder);
    void Reset(cFielder* pFielder);
    void ShotReleased(cFielder* pFielder);
    float GetShotAimValue() const
    {
        return mfSShotAimValue;
    }
    static bool IsActive(eShotMeterState state)
    {
        bool bShotMeterActive = false;
        if (state == SHOT_METER_ACTIVE || state == SHOT_METER_STS_ACTIVE
            || state == SHOT_METER_STS_TRANSISTION)
        {
            bShotMeterActive = true;
        }
        return bShotMeterActive;
    }

    /* 0x00 */ eShotMeterState m_eShotMeterState;
    /* 0x04 */ float m_fTime;
    /* 0x08 */ float mUnidentified008;
    /* 0x0C */ float mUnidentified00C;
    /* 0x10 */ float m_fScoreValue;
    /* 0x14 */ float m_fSpeedValue;
    /* 0x18 */ float m_fSTSValue;
    /* 0x1C */ float mfSShotAimValue;
}; // total size: 0x20

#endif // GAME_AI_SHOT_METER_H
