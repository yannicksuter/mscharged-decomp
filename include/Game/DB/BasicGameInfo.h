#ifndef GAME_DB_BASICGAMEINFO_H
#define GAME_DB_BASICGAMEINFO_H

#include <mem.h>

#include "Game/DB/StatsTracker.h"
#include "types.h"

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
