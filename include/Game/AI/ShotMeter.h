#ifndef GAME_AI_SHOT_METER_H
#define GAME_AI_SHOT_METER_H

#include "types.h"

class cFielder;

float CalcShotScoreValue(cFielder* pFielder, bool bIsChipShot, bool bWasPerfectPass);

enum eShotMeterState
{
    SHOT_METER_INACTIVE = 0,
    SHOT_METER_ACTIVE = 1,
    SHOT_METER_RELEASED = 2,
    SHOT_METER_STS_ACTIVE = 3,
    SHOT_METER_STS_TRANSITION = 4,
    SHOT_METER_STS_RELEASED = 5,
};

class ShotMeter
{
public:
    ShotMeter()
        : m_eShotMeterState(SHOT_METER_INACTIVE)
        , m_fTime(0.0f)
        , m_fShotDuration(0.0f)
        , m_fTotalDuration(0.0f)
        , m_fScoreValue(0.0f)
        , m_fSpeedValue(0.0f)
        , m_fSTSValue(0.0f)
        , m_fShotAimValue(0.0f)
    {
    }

    void Update(float fDeltaT);
    void Abort();
    void CalcOneTimerValue(cFielder* pFielder, bool bWasPerfectPass);
    void CalcShotAim(cFielder* pFielder);
    void Reset(cFielder* pFielder);
    void ShotReleased(cFielder* pFielder);
    float GetShotAimValue() const
    {
        return m_fShotAimValue;
    }
    float GetTotalDuration() const
    {
        return m_fTotalDuration;
    }
    float GetTime() const
    {
        return m_fTime;
    }
    float GetSpeedValue() const
    {
        return m_fSpeedValue;
    }
    float GetShotDuration() const
    {
        return m_fShotDuration;
    }
    bool UnidentifiedIsCharging() const
    {
        return m_eShotMeterState == SHOT_METER_ACTIVE
            || m_eShotMeterState == SHOT_METER_STS_ACTIVE;
    }
    static bool IsActive(eShotMeterState state)
    {
        bool bShotMeterActive = false;
        if (state == SHOT_METER_ACTIVE || state == SHOT_METER_STS_ACTIVE
            || state == SHOT_METER_STS_TRANSITION)
        {
            bShotMeterActive = true;
        }
        return bShotMeterActive;
    }

    /* 0x00 */ eShotMeterState m_eShotMeterState;
    /* 0x04 */ float m_fTime;
    /* 0x08 */ float m_fShotDuration;
    /* 0x0C */ float m_fTotalDuration;
    /* 0x10 */ float m_fScoreValue;
    /* 0x14 */ float m_fSpeedValue;
    /* 0x18 */ float m_fSTSValue;
    /* 0x1C */ float m_fShotAimValue;

private:
    void ResetValues();
    void CalcSpeedValue();
}; // total size: 0x20

#endif // GAME_AI_SHOT_METER_H
