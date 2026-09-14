#ifndef GAME_GOALIE_FATIGUE_H
#define GAME_GOALIE_FATIGUE_H

#include "types.h"

class DebugWriteCache;

class GoalieFatigue
{
public:
    float GetEnergyLevel() const
    {
        return mfEnergyLevel;
    }
    void Reset()
    {
        mfEnergyLevel = 100.0f;
        mfTimeSinceLastSave = 0.0f;
        mfHotStreakTimer = 0.0f;
        mfRecoverRate = 1.0f;
    }
    void Update(float dt);
    void RegisterShot(float fLevel);
    void UnidentifiedSyncLog(void* context, DebugWriteCache* cache);

    /* 0x00 */ f32 mfEnergyLevel;
    /* 0x04 */ f32 mfRecoverRate;
    /* 0x08 */ f32 mfTimeSinceLastSave;
    /* 0x0C */ f32 mfHotStreakTimer;
}; // total size: 0x10

#endif // GAME_GOALIE_FATIGUE_H
