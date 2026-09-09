#ifndef GAME_NETWORK_GAME_CONFIG_H
#define GAME_NETWORK_GAME_CONFIG_H

#include "types.h"
#include "Game/DB/UserOptions.h"

struct RecordedGameConfig
{
    /* 0x00 */ int mStadium;
    /* 0x04 */ int mHomeTeam;
    /* 0x08 */ int mHomeSidekicks[3];
    /* 0x14 */ int mAwayTeam;
    /* 0x18 */ int mAwaySidekicks[3];
    /* 0x24 */ GameplaySettings::eSkillLevel mSkillLevel;
    /* 0x28 */ int mWinBy;
    /* 0x2C */ int mGameTime;
    /* 0x30 */ int mGameGoals;
    /* 0x34 */ int mBestSeries;
    /* 0x38 */ s16 mPlayingSides[16];
}; // size: 0x58

// Per-machine start payload consumed by the session base initializer.
struct NetworkGameStartInfo
{
    /* 0x00 */ unsigned int mConfigSize;
    /* 0x04 */ RecordedGameConfig* mConfig;
    /* 0x08 */ u32 mSeed;
    /* 0x0C */ int mMachineCount;
    /* 0x10 */ int mMyMachineId;
    /* 0x14 */ int mPlayerCounts[4];
};


#endif // GAME_NETWORK_GAME_CONFIG_H
