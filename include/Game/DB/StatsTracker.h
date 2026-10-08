#ifndef GAME_DB_STATSTRACKER_H
#define GAME_DB_STATSTRACKER_H

#include <mem.h>

#include "Game/DB/PlayerStats.h"
#include "Game/DB/Simmer.h"
#include "Game/PassBallData.h"
#include "NL/nlSingleton.h"
#include "types.h"

struct CupSidekicks
{
    CupSidekicks()
        : mValues()
    {
    }

    eSidekickID mValues[3];
};

struct TeamStats
{
    void Initialize(eTeamID team)
    {
        memset(&mPlayerTotalStats, 0, sizeof(mPlayerTotalStats));
        mPlayerTotalStats.mRecordType.mTeamID = team;
        mPlayerTotalStats.mType = TYPE_TEAM;
        mTeamIndex = team;
        mNumWins = 0;
        mNumLosses = 0;
        mNumOTLosses = 0;
        mNumPoints = 0;
        mSidekicks.mValues[0] = (eSidekickID)0;
        mSidekicks.mValues[1] = (eSidekickID)0;
        mSidekicks.mValues[2] = (eSidekickID)0;
        mSkillLevel = 1;
    }

    TeamStats()
    {
        Initialize((eTeamID)0);
    }

    void SetSidekicks(CupSidekicks sidekicks) { mSidekicks = sidekicks; }

    /* 0x00 */ eTeamID mTeamIndex;
    /* 0x04 */ CupSidekicks mSidekicks;
    /* 0x10 */ u16 mNumWins;
    /* 0x12 */ u16 mNumLosses;
    /* 0x14 */ u16 mNumOTLosses;
    /* 0x16 */ u16 mNumPoints;
    /* 0x18 */ int mSkillLevel;
    /* 0x1C */ PlayerStats mPlayerTotalStats;
};

class BasicGameInfo;
struct PlayerAttackData;
struct GoalScoredStatsData;
struct MegaStrikeEndData;
struct CollisionPowerupStatsData;
struct ReceiveBallData;
struct GoalieSaveData;
struct PenaltyData;
struct CollisionBallGoalpostData;

class StatsTracker : public nlSingleton<StatsTracker>
{
public:
    StatsTracker();

    void SetBasicGameInfoPointer(
        BasicGameInfo* pGameInfo, bool initializeStats);
    void ResetCurrentStats();
    void CreateEventHandler();
    void DestroyEventHandler();

    static void OnPowerupStats(CollisionPowerupStatsData* data);
    static void OnAttackSuccess(PlayerAttackData* data);
    static void OnAttackAttempt(PlayerAttackData* data);
    static void OnGoalScored(GoalScoredStatsData* data);
    static void OnMegastrikeEnd(MegaStrikeEndData* data);
    static void OnReceiveBall(ReceiveBallData* data);
    static void OnPassBall(PassBallData* data);
    static void OnPenalty(PenaltyData* data);
    static void OnGoalieSave(GoalieSaveData* data);
    static void OnBallStateChange(int previousState, int currentState);
    static void OnCollisionBallGoalpost(CollisionBallGoalpostData* data);

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
    void AddUserStatByPlayer(ePlayerStats stat, int team, int player, int amount);
    void AddUserStatByPad(ePlayerStats stat, int pad, int amount);
    void TrackWinner(int forfeitSide);
    void WriteStats(float gameTime, float gameDuration, const char* filename);
    void WriteCurrentlyPlaying() const;
    bool MoveTeamBUp(TeamStats b, TeamStats a);

    bool IsOvertime() const { return mIsOvertime; }
    int GetNumGamesWon(int side) const { return mNumGamesWon[side]; }

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
