#ifndef GAME_GOALIE_FATIGUE_H
#define GAME_GOALIE_FATIGUE_H

#include "types.h"

class GoalieFatigue
{
public:
    GoalieFatigue();
    void Reset();
    void Update(float dt);
    void RegisterShot(float fLevel);

    /* 0x00 */ f32 mfEnergyLevel;
    /* 0x04 */ f32 mfRecoverRate;
    /* 0x08 */ f32 mfTimeSinceLastSave;
    /* 0x0C */ f32 mfHotStreakTimer;
}; // total size: 0x10

#endif // GAME_GOALIE_FATIGUE_H
