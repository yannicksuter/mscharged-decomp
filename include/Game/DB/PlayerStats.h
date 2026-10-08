#ifndef GAME_DB_PLAYERSTATS_H
#define GAME_DB_PLAYERSTATS_H

#include "types.h"

enum eTeamID
{
    TEAM_INVALID = -1,
};

enum eSidekickID
{
    SK_INVALID = -1,
};

enum eType
{
    TYPE_INVALID = -1,
    TYPE_CHARACTER = 0,
    TYPE_TEAM = 1,
    TYPE_USER = 2,
};

enum ePlayerStats
{
    STATS_INVALID = -1,
    STATS_00 = 0x00,
    STATS_01 = 0x01,
    STATS_02 = 0x02,
    STATS_SHOTS_ON_GOAL = 0x03,
    STATS_STS_ATTEMPTS = 0x04,
    STATS_REGULAR_GOALS = 0x05,
    STATS_ONE_TIMER_GOALS = 0x06,
    STATS_STS_GOALS = 0x07,
    STATS_ASSISTS = 0x08,
    STATS_MEGASTRIKE_ATTEMPTS = 0x09,
    STATS_MEGASTRIKE_GOALS = 0x0A,
    STATS_GOALS_FOR = 0x0B,
    STATS_GOALS_AGAINST = 0x0C,
    STATS_PASSES_MADE = 0x0D,
    STATS_NON_VOLLEY_PASSES = 0x0E,
    STATS_VOLLEY_PASSES = 0x0F,
    STATS_PASSES_RECEIVED = 0x10,
    STATS_FOULS = 0x11,
    STATS_HITS_MADE = 0x12,
    STATS_ATTACK_ATTEMPTS = 0x13,
    STATS_ATTACK_SUCCESSES = 0x14,
    STATS_15 = 0x15,
    STATS_16 = 0x16,
    STATS_BUTTON_PRESSES = 0x17,
    STATS_GAMES_PLAYED = 0x18,
    STATS_POWERUPS_USED = 0x19,
    STATS_MUSHROOMS_USED = 0x1A,
    STATS_DRAWABLE_POWERUPS_USED = 0x1B,
    STATS_STARS_AND_CHAIN_CHOMPS_USED = 0x1C,
    STATS_CAPTAIN_POWERUPS_USED = 0x1D,
    STATS_POWERUPS_HIT = 0x1E,
    STATS_WIN = 0x1F,
    STATS_OT_WIN = 0x20,
    STATS_LOSS = 0x21,
    STATS_OT_LOSS = 0x22,
    STATS_PERFECT_PASSES = 0x23,
    STATS_PASSES_INTERCEPTED = 0x24,
    STATS_25 = 0x25,
    STATS_26 = 0x26,
    NUM_STATS = 0x27,
};

enum eSortOrder
{
    SORT_ASCENDING = 0,
    SORT_DESCENDING = 1,
};

union RECORDTYPE
{
    /* 0x0 */ int mCharacterClass;
    /* 0x0 */ eTeamID mTeamID;
    /* 0x0 */ int mControllerID;
};

struct PlayerStats
{
    /* 0x00 */ u16 mNumLowChargeShots;
    /* 0x02 */ u16 mNumMediumChargeShots;
    /* 0x04 */ u16 mNumHighChargeShots;
    /* 0x06 */ u16 mNumShotsOnGoal;
    /* 0x08 */ u16 mNumRegularGoals;
    /* 0x0A */ u16 mNumOneTimerGoals;
    /* 0x0C */ u16 mNumSTSGoals;
    /* 0x0E */ u16 mNumAssists;
    /* 0x10 */ u16 mNumGoalsFor;
    /* 0x12 */ u16 mNumGoalsAgainst;
    /* 0x14 */ u16 mNumSTSAttempts;
    /* 0x16 */ u16 mNumMegaStrikeAttempts;
    /* 0x18 */ u16 mNumMegaStrikeGoals;
    /* 0x1A */ u16 mNumFouls;
    /* 0x1C */ u16 mNumGamesPlayed;
    /* 0x1E */ u16 mNumPowerupsUsed;
    /* 0x20 */ u16 mNumMushroomsUsed;
    /* 0x22 */ u16 mNumDrawablePowerupsUsed;
    /* 0x24 */ u16 mNumStarsAndChainChompsUsed;
    /* 0x26 */ u16 mNumCaptainPowerupsUsed;
    /* 0x28 */ u16 mNumPowerupsHit;
    /* 0x2A */ u16 mNumPassesMade;
    /* 0x2C */ u16 mNumNonVolleyPasses;
    /* 0x2E */ u16 mNumVolleyPasses;
    /* 0x30 */ u16 mNumPassesReceived;
    /* 0x32 */ u16 mNumHitsMade;
    /* 0x34 */ u16 mNumAttackAttempts;
    /* 0x36 */ u16 mNumSteals;
    /* 0x38 */ u16 unknown_0x38;
    /* 0x3C */ u32 mBallPossessionTime;
    /* 0x40 */ u32 mNumButtonPresses;
    /* 0x44 */ u16 mNumPerfectPasses;
    /* 0x46 */ u16 unknown_0x46;
    /* 0x48 */ u16 unknown_0x48;
    /* 0x4A */ u16 mNumPassesIntercepted;
    /* 0x4C */ RECORDTYPE mRecordType;
    /* 0x50 */ eType mType;
};

inline int GetStatValue(const PlayerStats& stats, ePlayerStats stat)
{
    int value = -1;
    switch (stat)
    {
    case STATS_00: value = stats.mNumLowChargeShots; break;
    case STATS_01: value = stats.mNumMediumChargeShots; break;
    case STATS_02: value = stats.mNumHighChargeShots; break;
    case STATS_SHOTS_ON_GOAL: value = stats.mNumShotsOnGoal; break;
    case STATS_REGULAR_GOALS: value = stats.mNumRegularGoals; break;
    case STATS_ONE_TIMER_GOALS: value = stats.mNumOneTimerGoals; break;
    case STATS_STS_GOALS: value = stats.mNumSTSGoals; break;
    case STATS_ASSISTS: value = stats.mNumAssists; break;
    case STATS_GOALS_FOR: value = stats.mNumGoalsFor; break;
    case STATS_GOALS_AGAINST: value = stats.mNumGoalsAgainst; break;
    case STATS_STS_ATTEMPTS: value = stats.mNumSTSAttempts; break;
    case STATS_MEGASTRIKE_ATTEMPTS: value = stats.mNumMegaStrikeAttempts; break;
    case STATS_MEGASTRIKE_GOALS: value = stats.mNumMegaStrikeGoals; break;
    case STATS_FOULS: value = stats.mNumFouls; break;
    case STATS_GAMES_PLAYED: value = stats.mNumGamesPlayed; break;
    case STATS_POWERUPS_USED: value = stats.mNumPowerupsUsed; break;
    case STATS_MUSHROOMS_USED: value = stats.mNumMushroomsUsed; break;
    case STATS_DRAWABLE_POWERUPS_USED: value = stats.mNumDrawablePowerupsUsed; break;
    case STATS_STARS_AND_CHAIN_CHOMPS_USED: value = stats.mNumStarsAndChainChompsUsed; break;
    case STATS_CAPTAIN_POWERUPS_USED: value = stats.mNumCaptainPowerupsUsed; break;
    case STATS_PASSES_MADE: value = stats.mNumPassesMade; break;
    case STATS_NON_VOLLEY_PASSES: value = stats.mNumNonVolleyPasses; break;
    case STATS_VOLLEY_PASSES: value = stats.mNumVolleyPasses; break;
    case STATS_PASSES_RECEIVED: value = stats.mNumPassesReceived; break;
    case STATS_HITS_MADE: value = stats.mNumHitsMade; break;
    case STATS_ATTACK_ATTEMPTS: value = stats.mNumAttackAttempts; break;
    case STATS_ATTACK_SUCCESSES: value = stats.mNumSteals; break;
    case STATS_15: value = stats.unknown_0x38; break;
    case STATS_16: value = stats.mBallPossessionTime; break;
    case STATS_BUTTON_PRESSES: value = stats.mNumButtonPresses; break;
    case STATS_PERFECT_PASSES: value = stats.mNumPerfectPasses; break;
    case STATS_25: value = stats.unknown_0x46; break;
    case STATS_26: value = stats.unknown_0x48; break;
    case STATS_POWERUPS_HIT: value = stats.mNumPowerupsHit; break;
    case STATS_PASSES_INTERCEPTED: value = stats.mNumPassesIntercepted; break;
    }
    return value;
}


#endif // GAME_DB_PLAYERSTATS_H
