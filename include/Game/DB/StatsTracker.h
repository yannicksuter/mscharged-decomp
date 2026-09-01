#ifndef GAME_DB_STATSTRACKER_H
#define GAME_DB_STATSTRACKER_H

#include <mem.h>

#include "Game/PassBallData.h"
#include "Game/DB/Simmer.h"
#include "NL/nlSingleton.h"
#include "types.h"

enum eTeamID
{
    TEAM_INVALID = -1,
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
    STATS_04 = 0x04,
    STATS_05 = 0x05,
    STATS_06 = 0x06,
    STATS_07 = 0x07,
    STATS_08 = 0x08,
    STATS_09 = 0x09,
    STATS_0A = 0x0A,
    STATS_GOALS_FOR = 0x0B,
    STATS_0C = 0x0C,
    STATS_PASSES_MADE = 0x0D,
    STATS_0E = 0x0E,
    STATS_0F = 0x0F,
    STATS_PASSES_RECEIVED = 0x10,
    STATS_FOULS = 0x11,
    STATS_12 = 0x12,
    STATS_ATTACK_ATTEMPTS = 0x13,
    STATS_ATTACK_SUCCESSES = 0x14,
    STATS_15 = 0x15,
    STATS_16 = 0x16,
    STATS_17 = 0x17,
    STATS_18 = 0x18,
    STATS_19 = 0x19,
    STATS_1A = 0x1A,
    STATS_1B = 0x1B,
    STATS_1C = 0x1C,
    STATS_1D = 0x1D,
    STATS_POWERUPS_USED = 0x1E,
    STATS_WIN = 0x1F,
    STATS_LOSS = 0x20,
    STATS_21 = 0x21,
    STATS_22 = 0x22,
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
    /* 0x00 */ u16 unknown_0x00;
    /* 0x02 */ u16 unknown_0x02;
    /* 0x04 */ u16 unknown_0x04;
    /* 0x06 */ u16 mNumShotsOnGoal;
    /* 0x08 */ u16 unknown_0x08;
    /* 0x0A */ u16 unknown_0x0A;
    /* 0x0C */ u16 unknown_0x0C;
    /* 0x0E */ u16 unknown_0x0E;
    /* 0x10 */ u16 mNumGoalsFor;
    /* 0x12 */ u16 unknown_0x12;
    /* 0x14 */ u16 unknown_0x14;
    /* 0x16 */ u16 unknown_0x16;
    /* 0x18 */ u16 unknown_0x18;
    /* 0x1A */ u16 mNumFouls;
    /* 0x1C */ u16 unknown_0x1C;
    /* 0x1E */ u16 mNumPowerupsUsed;
    /* 0x20 */ u16 unknown_0x20;
    /* 0x22 */ u16 unknown_0x22;
    /* 0x24 */ u16 unknown_0x24;
    /* 0x26 */ u16 unknown_0x26;
    /* 0x28 */ u16 unknown_0x28;
    /* 0x2A */ u16 mNumPassesMade;
    /* 0x2C */ u16 unknown_0x2C;
    /* 0x2E */ u16 unknown_0x2E;
    /* 0x30 */ u16 mNumPassesReceived;
    /* 0x32 */ u16 mNumHitsMade;
    /* 0x34 */ u16 unknown_0x34;
    /* 0x36 */ u16 mNumSteals;
    /* 0x38 */ u16 unknown_0x38;
    /* 0x3A */ u16 padding_0x3A;
    /* 0x3C */ u32 unknown_0x3C;
    /* 0x40 */ u32 mNumButtonPresses;
    /* 0x44 */ u16 mNumPerfectPasses;
    /* 0x46 */ u16 unknown_0x46;
    /* 0x48 */ u16 unknown_0x48;
    /* 0x4A */ u16 mNumPassesIntercepted;
    /* 0x4C */ RECORDTYPE mRecordType;
    /* 0x50 */ eType mType;
};

struct TeamStats
{
    TeamStats()
    {
        unknown_0x04 = 0;
        unknown_0x08 = 0;
        unknown_0x0C = 0;
        memset(&mPlayerTotalStats, 0, sizeof(mPlayerTotalStats));
        mPlayerTotalStats.mRecordType.mTeamID = (eTeamID)0;
        mPlayerTotalStats.mType = TYPE_TEAM;
        mTeamIndex = (eTeamID)0;
        unknown_0x10 = 0;
        unknown_0x12 = 0;
        unknown_0x14 = 0;
        unknown_0x16 = 0;
        unknown_0x04 = 0;
        unknown_0x08 = 0;
        unknown_0x0C = 0;
        mType = TYPE_TEAM;
    }

    /* 0x00 */ eTeamID mTeamIndex;
    /* 0x04 */ int unknown_0x04;
    /* 0x08 */ int unknown_0x08;
    /* 0x0C */ int unknown_0x0C;
    /* 0x10 */ u16 unknown_0x10;
    /* 0x12 */ u16 unknown_0x12;
    /* 0x14 */ u16 unknown_0x14;
    /* 0x16 */ u16 unknown_0x16;
    /* 0x18 */ eType mType;
    /* 0x1C */ PlayerStats mPlayerTotalStats;
};

class BasicGameInfo;
struct AttackStatsData;
struct GoalScoredStatsData;
struct MegaStrikeEndData;
struct PowerupStatsData;
struct ReceiveBallStatsData;
struct GoalieSaveData;
struct PenaltyStatsData;
struct UnidentifiedEventData07;

class StatsTracker : public nlSingleton<StatsTracker>
{
public:
    StatsTracker();

    void SetBasicGameInfoPointer(
        BasicGameInfo* pGameInfo, bool initializeStats);
    void ResetCurrentStats();
    void CreateEventHandler();
    void DestroyEventHandler();

    static void OnPowerupStats(PowerupStatsData* data);
    static void OnAttackSuccess(AttackStatsData* data);
    static void OnAttackAttempt(AttackStatsData* data);
    static void OnGoalScored(GoalScoredStatsData* data);
    static void OnMegastrikeEnd(MegaStrikeEndData* data);
    static void OnReceiveBall(ReceiveBallStatsData* data);
    static void OnPassBall(PassBallData* data);
    static void OnPenalty(PenaltyStatsData* data);
    static void OnGoalieSave(GoalieSaveData* data);
    static void OnBallStateChange(int previousState, int currentState);
    static void OnCollisionBallGoalpost(UnidentifiedEventData07* data);

    void TrackStat(ePlayerStats stat, int homeaway, int playerindex,
        int param0, int param1, int param2, int param3);
    static void Track(ePlayerStats stat, int homeaway, int playerindex,
        int param0, int param1, int param2, int param3);
    void GetSortedStats(PlayerStats* source, int numsource, int* dest,
        int numelements, ePlayerStats statType, eSortOrder sortOrder);
    void GetSortedTeamStats(
        TeamStats* source, int numsource, int* dest, int numelements);
    void CompileEndOfGameStats();
    void SimulateGame();
    void AddStat(ePlayerStats stat, int team, int player, int value);
    void AddUserStatByPad(ePlayerStats stat, int pad, int amount);
    void AddMilestoneUserStat(ePlayerStats stat, int amount);
    void TrackWinner(int forfeitSide);
    void WriteStats(float gameTime, float gameDuration, const char* filename);
    void WriteCurrentlyPlaying() const;
    bool MoveTeamBUp(TeamStats b, TeamStats a);

    bool IsOvertime() const { return mIsOvertime; }

    /* 0x000 */ BasicGameInfo* mBasicGameInfo;
    /* 0x004 */ TeamStats* mCumulativeTeamStats[2];
    /* 0x00C */ TeamStats mCurrentTeamStats[2];
    /* 0x0EC */ PlayerStats mCurrentPlayerStats[2][5];
    /* 0x434 */ PlayerStats mCurrentUserStats[16];
    /* 0x974 */ PlayerStats mCumulativeUserStats[16];
    /* 0xEB4 */ u16 mNumConsecutiveGamesPlayed;
    /* 0xEB6 */ u8 padding_0xEB6[2];
    /* 0xEB8 */ int mNumGamesWon[2];
    /* 0xEC0 */ Simulator* m_pSimulator;
    /* 0xEC4 */ bool mIsUserCupWinner;
    /* 0xEC5 */ bool mIsOvertime;
    /* 0xEC6 */ bool mHasGameEnded;
    /* 0xEC7 */ u8 padding_0xEC7;
};

#endif // GAME_DB_STATSTRACKER_H
