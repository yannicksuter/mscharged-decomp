#ifndef GAME_AI_FIELDER_ABILITY_H
#define GAME_AI_FIELDER_ABILITY_H

class WaluigiWallManager;

struct ActBowserSuper
{
    ActBowserSuper()
        : nextFireballTime(0.0f)
        , fireballStageTime(0.0f)
        , fireballStageNum(0)
    {
    }

    void fn_800504A4();

    /* 0x00 */ float nextFireballTime;
    /* 0x04 */ float fireballStageTime;
    /* 0x08 */ int fireballStageNum;
}; // size: 0xC

struct WaluigiWallState
{
    WaluigiWallState()
        : mSegmentTimeRemaining(0.0f)
        , mMinSegmentTime(0.0f)
    {
    }

    void fn_800504A8();

    /* 0x00 */ float mSegmentTimeRemaining;
    /* 0x04 */ float mMinSegmentTime;
    /* 0x08 */ WaluigiWallManager* mWallManager;
}; // size: 0xC


extern float gDKSuperShockwaveRadius;

#endif // GAME_AI_FIELDER_ABILITY_H
