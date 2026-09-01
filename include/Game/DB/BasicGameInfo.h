#ifndef GAME_DB_BASICGAMEINFO_H
#define GAME_DB_BASICGAMEINFO_H

#include <mem.h>

#include "Game/DB/StatsTracker.h"
#include "types.h"

enum eSidekickID
{
    SK_INVALID = -1,
};

enum eStadiumID
{
    STAD_INVALID = -1,
};

/**
 * Charged expands the predecessor's 0x20-byte match descriptor to 0x128 bytes:
 * two captains, three sidekicks per side, the stadium, and a much larger pad
 * side table. Only the fields R4QE01 actually references are named.
 */
struct BasicGameInfo
{
    BasicGameInfo();

    void Reset(bool clearTeams);

    /* 0x000 */ int mTeamIndex[2];
    /* 0x008 */ int mSidekickIndex[2][3];
    /* 0x020 */ int mStadiumIndex;
    /* 0x024 */ TeamStats mSides[2];
    /* 0x104 */ s16 mPadSides[16];
    /* 0x124 */ s16 mFinalScore[2];
};

#endif // GAME_DB_BASICGAMEINFO_H
