#include "Game/GoalieFatigue.h"

void GoalieFatigue::Update(float dt)
{
    if (mfEnergyLevel < 100.0f)
    {
        mfEnergyLevel = (mfRecoverRate * dt) + mfEnergyLevel;
    }

    if (mfHotStreakTimer > 0.0f)
    {
        mfHotStreakTimer = mfHotStreakTimer - dt;
        if (mfHotStreakTimer <= 0.0f)
        {
            mfEnergyLevel = 100.0f;
        }
    }
}

void GoalieFatigue::RegisterShot(float fLevel)
{
    mfEnergyLevel -= fLevel;
    if (mfEnergyLevel < 100.0f)
    {
        mfHotStreakTimer = 0.0f;
        if (mfEnergyLevel < 0.0f)
        {
            mfEnergyLevel = 0.0f;
        }
    }
}
