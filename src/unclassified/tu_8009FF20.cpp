#include "Game/AI/ShotMeter.h"

class cFielder;

extern "C" void* fn_8003E6E4(cFielder* pFielder);
extern "C" float fn_8002C7E8(void* pCharacterData);
extern "C" float fn_8002C7F4(void* pCharacterData);

extern "C" void fn_800A0110(ShotMeter* pMeter)
{
    pMeter->m_eShotMeterState = SHOT_METER_INACTIVE;
    pMeter->m_fTime = 0.0f;
    pMeter->m_fSTSValue = 0.0f;
    pMeter->mfSShotAimValue = 0.0f;
    pMeter->mUnidentified018 = 0.0f;
}

void ShotMeter::Reset(cFielder* pFielder)
{
    m_eShotMeterState = SHOT_METER_ACTIVE;
    m_fTime = 0.0f;
    m_fSTSValue = 0.0f;
    mfSShotAimValue = 0.0f;
    mUnidentified018 = 0.0f;

    m_fScoreValue = fn_8002C7E8(fn_8003E6E4(pFielder));
    m_fSpeedValue = fn_8002C7F4(fn_8003E6E4(pFielder));
}
