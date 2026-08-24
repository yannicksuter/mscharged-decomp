#ifndef GAME_DB_BASICGAMEINFO_H
#define GAME_DB_BASICGAMEINFO_H

#include "types.h"

enum eTeamID
{
    TEAM_INVALID = -1,
};

enum eSidekickID
{
    SK_INVALID = -1,
};

enum eStadiumID
{
    STAD_INVALID = -1,
};

extern "C" void* memset(void* dst, int value, unsigned long size);

/**
 * The 0x70-byte per-side block BasicGameInfo constructs before it resets
 * itself. Only its construction is retained in R4QE01, so every field keeps an
 * offset-derived name.
 */
struct BasicGameInfoSide
{
    BasicGameInfoSide()
    {
        unknown_0x04 = 0;
        unknown_0x08 = 0;
        unknown_0x0C = 0;
        memset(&unknown_0x1C, 0, 0x54);
        unknown_0x68 = 0;
        unknown_0x6C = 1;
        unknown_0x00 = 0;
        unknown_0x10 = 0;
        unknown_0x12 = 0;
        unknown_0x14 = 0;
        unknown_0x16 = 0;
        unknown_0x04 = 0;
        unknown_0x08 = 0;
        unknown_0x0C = 0;
        unknown_0x18 = 1;
    }

    /* 0x00 */ int unknown_0x00;
    /* 0x04 */ int unknown_0x04;
    /* 0x08 */ int unknown_0x08;
    /* 0x0C */ int unknown_0x0C;
    /* 0x10 */ s16 unknown_0x10;
    /* 0x12 */ s16 unknown_0x12;
    /* 0x14 */ s16 unknown_0x14;
    /* 0x16 */ s16 unknown_0x16;
    /* 0x18 */ int unknown_0x18;
    /* 0x1C */ u8 unknown_0x1C[0x4C];
    /* 0x68 */ int unknown_0x68;
    /* 0x6C */ int unknown_0x6C;
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
    /* 0x024 */ BasicGameInfoSide mSides[2];
    /* 0x104 */ s16 mPadSides[16];
    /* 0x124 */ s16 unknown_0x124;
    /* 0x126 */ s16 unknown_0x126;
};

#endif // GAME_DB_BASICGAMEINFO_H
