#ifndef GAME_DB_BASICGAMEINFO_H
#define GAME_DB_BASICGAMEINFO_H

#include <mem.h>

#include "Game/DB/StatsTracker.h"
#include "types.h"

enum eStadiumID
{
    STAD_INVALID = -1,
    STAD_BATTLEDOME = 0,
    STAD_BOWSER_STADIUM = 1,
    STAD_CRATERFIELD = 2,
    STAD_CRYSTAL_CANYON = 3,
    STAD_DUMP = 4,
    STAD_GALACTIC_STADIUM = 5,
    STAD_KONGA_COLISEUM = 6,
    STAD_LAVA_PIT = 7,
    STAD_PIPELINE_CENTRAL = 8,
    STAD_STORM_SHIP = 9,
    STAD_THE_PALACE = 10,
    STAD_THUNDER_ISLAND = 11,
    STAD_UNDERGROUND = 12,
    STAD_VICE = 13,
    STAD_WASTELANDS = 14,
    STAD_SAND_TOMB = 15,
    STAD_TUTORIAL_FIELD = 16,
    STAD_LOW_POLY_STADIUM = 17,
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

    eTeamID GetTeam(short side) const
    {
        eTeamID team = (eTeamID)mTeamIndex[side];
        return team;
    }
    eSidekickID GetSidekick(short side, int slot) const
    {
        eSidekickID sidekick = (eSidekickID)mSidekickIndex[side][slot];
        return sidekick;
    }
    void SetFinalScore(int side, short score) { mFinalScore[side] = score; }
    short GetFinalScore(short side) const { return mFinalScore[side]; }
    int GetWinningSide() const
    {
        if (mFinalScore[0] == 0 && mFinalScore[1] == 0)
        {
            return -1;
        }
        return mFinalScore[0] <= mFinalScore[1];
    }
    void SetSidekick(int side, int sidekick, int slot)
    {
        for (int i = 0; i < 3; i++)
        {
            if (slot < 0 || slot == i)
            {
                mSidekickIndex[side][i] = sidekick;
            }
        }
    }

    /* 0x000 */ int mTeamIndex[2];
    /* 0x008 */ int mSidekickIndex[2][3];
    /* 0x020 */ int mStadiumIndex;
    /* 0x024 */ TeamStats mSides[2];
    /* 0x104 */ s16 mPadSides[16];
    /* 0x124 */ s16 mFinalScore[2];
};

#endif // GAME_DB_BASICGAMEINFO_H
