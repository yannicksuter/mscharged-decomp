#include "Game/DB/StatsTracker.h"

#include <stdio.h>

#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/PassBallData.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

struct PowerupStatsData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ u8 unknown_0x04[8];
    /* 0x0C */ int amount;
};

struct AttackStatsData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ int amount;
    /* 0x08 */ u8 unknown_0x08[8];
    /* 0x10 */ bool track;
};

struct GoalScoredStatsData
{
    /* 0x00 */ u32 goalData;
    /* 0x04 */ nlVector3 shotPosition;
    /* 0x10 */ cPlayer* pScorer;
    /* 0x14 */ cPlayer* pAssister;
    /* 0x18 */ u8 unknown_0x18[8];
    /* 0x20 */ int goalValue;
};

struct MegaStrikeEndData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ s8 attempts;
    /* 0x05 */ s8 goals;
    /* 0x06 */ s8 defendingSide;
    /* 0x07 */ s8 goalValue;
};

struct ReceiveBallStatsData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ int receiveType;
};

struct PenaltyStatsData
{
    /* 0x00 */ cPlayer* pPlayer;
};

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    StatsEventRegistry;

template <typename P1, typename P2>
class StatsTypedEvent2 : public UnidentifiedEventBase
{
public:
    StatsTypedEvent2(const char* name, int length)
        : UnidentifiedEventBase(name, length)
    {
    }

    virtual ~StatsTypedEvent2() { }
    virtual void Disconnect() = 0;
    virtual void Add(Function2<void, P1, P2>&, unsigned int, int) = 0;
};

extern StatsEventRegistry* lbl_806E1D90;
extern BaseGameSceneManager* lbl_806E1860;

extern "C" int fn_801CBE78(int captain);
extern "C" int fn_801CBE7C(int sidekick);
extern "C" const char* fn_801CBE80(int captain);
extern "C" const char* fn_801CBEA8(int sidekick);
extern "C" const char* fn_801B6188(int stadium);
extern "C" void fn_801E2A14(BaseGameSceneManager* manager);
extern "C" int fn_80380C34(FILE* file, long offset, int origin);
extern "C" int fn_8037FA00(FILE* file);

template <>
StatsTracker* nlSingleton<StatsTracker>::s_pInstance = 0;

static const char* STATS_FILE = "statsfile.csv";

template <typename T>
static inline UnidentifiedTypedEvent<T>* FindStatsEvent(const char* name)
{
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    return (UnidentifiedTypedEvent<T>*)event;
}

template <typename P1, typename P2>
static inline StatsTypedEvent2<P1, P2>* FindStatsEvent2(const char* name)
{
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    return (StatsTypedEvent2<P1, P2>*)event;
}

static inline void InitializePlayerStats(
    PlayerStats& stats, int record, eType type)
{
    memset(&stats, 0, sizeof(stats));
    stats.mRecordType.mControllerID = record;
    stats.mType = type;
}

static inline void InitializeTeamStats(TeamStats& stats, eTeamID team)
{
    memset(&stats.mPlayerTotalStats, 0, sizeof(stats.mPlayerTotalStats));
    stats.mPlayerTotalStats.mRecordType.mTeamID = team;
    stats.mPlayerTotalStats.mType = TYPE_TEAM;
    stats.mTeamIndex = team;
    stats.unknown_0x10 = 0;
    stats.unknown_0x12 = 0;
    stats.unknown_0x14 = 0;
    stats.unknown_0x16 = 0;
    stats.unknown_0x04 = 0;
    stats.unknown_0x08 = 0;
    stats.unknown_0x0C = 0;
    stats.mType = TYPE_TEAM;
}

static int GetStatValue(const PlayerStats& stats, ePlayerStats stat);

static inline void AddStatValue(
    PlayerStats& stats, ePlayerStats stat, int amount)
{
    u16* value = 0;
    switch (stat)
    {
    case STATS_00: value = &stats.unknown_0x00; break;
    case STATS_01: value = &stats.unknown_0x02; break;
    case STATS_02: value = &stats.unknown_0x04; break;
    case STATS_SHOTS_ON_GOAL: value = &stats.mNumShotsOnGoal; break;
    case STATS_04: value = &stats.unknown_0x14; break;
    case STATS_05: value = &stats.unknown_0x08; break;
    case STATS_06: value = &stats.unknown_0x0A; break;
    case STATS_07: value = &stats.unknown_0x0C; break;
    case STATS_08: value = &stats.unknown_0x0E; break;
    case STATS_09: value = &stats.unknown_0x16; break;
    case STATS_0A: value = &stats.unknown_0x18; break;
    case STATS_GOALS_FOR: value = &stats.mNumGoalsFor; break;
    case STATS_0C: value = &stats.unknown_0x12; break;
    case STATS_PASSES_MADE: value = &stats.mNumPassesMade; break;
    case STATS_0E: value = &stats.unknown_0x2C; break;
    case STATS_0F: value = &stats.unknown_0x2E; break;
    case STATS_PASSES_RECEIVED: value = &stats.mNumPassesReceived; break;
    case STATS_FOULS: value = &stats.mNumFouls; break;
    case STATS_12: value = &stats.mNumHitsMade; break;
    case STATS_ATTACK_ATTEMPTS: value = &stats.unknown_0x34; break;
    case STATS_ATTACK_SUCCESSES: value = &stats.mNumSteals; break;
    case STATS_15: value = &stats.unknown_0x38; break;
    case STATS_18: value = &stats.unknown_0x1C; break;
    case STATS_19: value = &stats.mNumPowerupsUsed; break;
    case STATS_1A: value = &stats.unknown_0x20; break;
    case STATS_1B: value = &stats.unknown_0x22; break;
    case STATS_1C: value = &stats.unknown_0x24; break;
    case STATS_1D: value = &stats.unknown_0x26; break;
    case STATS_POWERUPS_USED: value = &stats.unknown_0x28; break;
    case STATS_PERFECT_PASSES: value = &stats.mNumPerfectPasses; break;
    case STATS_PASSES_INTERCEPTED:
        value = &stats.mNumPassesIntercepted;
        break;
    case STATS_25: value = &stats.unknown_0x46; break;
    case STATS_26: value = &stats.unknown_0x48; break;
    default: break;
    }

    if (value != 0)
    {
        int result = *value + amount;
        if (result > 999)
        {
            result = 999;
        }
        *value = (u16)result;
    }
    else if (stat == STATS_16)
    {
        stats.unknown_0x3C += amount;
    }
    else if (stat == STATS_17)
    {
        stats.mNumButtonPresses += amount;
    }
}

StatsTracker::StatsTracker()
    : mBasicGameInfo(0)
{
    mIsUserCupWinner = false;
    mHasGameEnded = false;

    m_pSimulator = new (nlMalloc(sizeof(Simulator), 8, false)) Simulator();
    mCumulativeTeamStats[0] = 0;
    mCumulativeTeamStats[1] = 0;
}

void StatsTracker::SetBasicGameInfoPointer(
    BasicGameInfo* pGameInfo, bool initializeStats)
{
    mBasicGameInfo = pGameInfo;
    eTeamID homeid = (eTeamID)mBasicGameInfo->mTeamIndex[0];
    eTeamID awayid = (eTeamID)mBasicGameInfo->mTeamIndex[1];

    mIsUserCupWinner = false;
    mIsOvertime = false;
    mHasGameEnded = false;
    mNumConsecutiveGamesPlayed = 1;
    mNumGamesWon[0] = 0;
    mNumGamesWon[1] = 0;
    mCumulativeTeamStats[0] = &mBasicGameInfo->mSides[0];
    mCumulativeTeamStats[1] = &mBasicGameInfo->mSides[1];

    if (!initializeStats)
    {
        return;
    }

    InitializeTeamStats(*mCumulativeTeamStats[0], homeid);
    InitializeTeamStats(*mCumulativeTeamStats[1], awayid);
    InitializeTeamStats(mCurrentTeamStats[0], homeid);
    InitializeTeamStats(mCurrentTeamStats[1], awayid);

    InitializePlayerStats(
        mCurrentPlayerStats[0][0], fn_801CBE78(homeid), TYPE_CHARACTER);
    InitializePlayerStats(
        mCurrentPlayerStats[1][0], fn_801CBE78(awayid), TYPE_CHARACTER);

    int* homeSidekicks = &mBasicGameInfo->mSidekickIndex[0][0];
    int* awaySidekicks = &mBasicGameInfo->mSidekickIndex[1][0];
    for (int i = 1; i < 5; i++)
    {
        InitializePlayerStats(mCurrentPlayerStats[0][i],
            fn_801CBE7C(homeSidekicks[i - 1]), TYPE_CHARACTER);
        InitializePlayerStats(mCurrentPlayerStats[1][i],
            fn_801CBE7C(awaySidekicks[i - 1]), TYPE_CHARACTER);
    }

    for (int i = 0; i < 4; i++)
    {
        InitializePlayerStats(mCurrentUserStats[i], i, TYPE_USER);
        InitializePlayerStats(mCumulativeUserStats[i], i, TYPE_USER);
    }
}

void StatsTracker::ResetCurrentStats()
{
    mIsOvertime = false;
    mHasGameEnded = false;

    InitializeTeamStats(mCurrentTeamStats[0],
        mCumulativeTeamStats[0]->mTeamIndex);
    InitializeTeamStats(mCurrentTeamStats[1],
        mCumulativeTeamStats[1]->mTeamIndex);

    mNumConsecutiveGamesPlayed++;
    mBasicGameInfo->mFinalScore[0] = 0;
    mBasicGameInfo->mFinalScore[1] = 0;

    for (int i = 0; i < 4; i++)
    {
        InitializePlayerStats(mCurrentUserStats[i], i, TYPE_USER);
    }

    fn_801E2A14(lbl_806E1860);
}

void StatsTracker::CreateEventHandler()
{
    {
        Function<PenaltyStatsData*> callback(OnPenalty);
        FindStatsEvent<PenaltyStatsData>("Penalty")->Add(callback, 0, -1);
    }
    {
        Function<GoalieSaveData*> callback(OnGoalieSave);
        FindStatsEvent<GoalieSaveData>("GoalieSave")->Add(callback, 0, -1);
    }
    {
        Function<PassBallData*> callback(OnPassBall);
        FindStatsEvent<PassBallData>("PassBall")->Add(callback, 0, -1);
    }
    {
        Function<ReceiveBallStatsData*> callback(OnReceiveBall);
        FindStatsEvent<ReceiveBallStatsData>("ReceiveBall")
            ->Add(callback, 0, -1);
    }
    {
        Function<GoalScoredStatsData*> callback(OnGoalScored);
        FindStatsEvent<GoalScoredStatsData>("GoalScored")
            ->Add(callback, 0, -1);
    }
    {
        Function<MegaStrikeEndData*> callback(OnMegastrikeEnd);
        FindStatsEvent<MegaStrikeEndData>("MegastrikeEnd")
            ->Add(callback, 0, -1);
    }
    {
        Function<AttackStatsData*> callback(OnAttackSuccess);
        FindStatsEvent<AttackStatsData>("AttackSuccess")
            ->Add(callback, 0, -1);
    }
    {
        Function<AttackStatsData*> callback(OnAttackAttempt);
        FindStatsEvent<AttackStatsData>("AttackAttempt")
            ->Add(callback, 0, -1);
    }
    {
        Function<PowerupStatsData*> callback(OnPowerupStats);
        FindStatsEvent<PowerupStatsData>("PowerupStats")
            ->Add(callback, 0, -1);
    }
    {
        Function2<void, int, int> callback(OnBallStateChange);
        FindStatsEvent2<int, int>("BallStateChange")
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData07*> callback(
            OnCollisionBallGoalpost);
        FindStatsEvent<UnidentifiedEventData07>("CollisionBallGoalpost")
            ->Add(callback, 0, -1);
    }
}

void StatsTracker::DestroyEventHandler()
{
}

void StatsTracker::OnPowerupStats(PowerupStatsData* data)
{
    if (data->pPlayer != 0)
    {
        Instance()->TrackStat(STATS_POWERUPS_USED,
            data->pPlayer->m_pTeam->m_nSide, data->pPlayer->m_ID,
            data->amount, 0, 0, 0);
    }
}

void StatsTracker::OnAttackSuccess(AttackStatsData* data)
{
    if (data->track && data->pPlayer != 0 && data->pPlayer->m_pBall != 0)
    {
        Instance()->TrackStat(STATS_ATTACK_SUCCESSES,
            data->pPlayer->m_pTeam->m_nSide, data->pPlayer->m_ID,
            data->amount, 0, 0, 0);
    }
}

void StatsTracker::OnAttackAttempt(AttackStatsData* data)
{
    if (data->track)
    {
        Instance()->TrackStat(STATS_ATTACK_ATTEMPTS,
            data->pPlayer->m_pTeam->m_nSide, data->pPlayer->m_ID,
            data->amount, 0, 0, 0);
    }
}

void StatsTracker::OnGoalScored(GoalScoredStatsData* data)
{
    int scorer = data->pScorer != 0 ? data->pScorer->m_ID : -1;
    int assister = data->pAssister != 0 ? data->pAssister->m_ID : -1;
    int side = data->goalData >> 24;
    s_pInstance->TrackStat(STATS_GOALS_FOR, side, scorer, assister,
        data->goalData & 0xFFFF, (data->goalData >> 8) & 0xFF,
        data->goalValue);

    if (data->pScorer != 0)
    {
        s_pInstance->TrackStat(STATS_SHOTS_ON_GOAL, side, scorer, 1, 0, 0, 0);
    }
}

void StatsTracker::OnMegastrikeEnd(MegaStrikeEndData* data)
{
    int side = 1 - data->defendingSide;
    if (data->goals > 0)
    {
        s_pInstance->TrackStat(STATS_GOALS_FOR, side, data->pPlayer->m_ID, -1, 6,
            data->goals, data->goalValue);
    }
    s_pInstance->TrackStat(
        STATS_09, side, data->pPlayer->m_ID, data->attempts, 0, 0, 0);
    s_pInstance->TrackStat(
        STATS_0A, side, data->pPlayer->m_ID, data->goals, 0, 0, 0);
    s_pInstance->TrackStat(STATS_SHOTS_ON_GOAL, side, data->pPlayer->m_ID,
        data->attempts, 0, 0, 0);
}

void StatsTracker::OnReceiveBall(ReceiveBallStatsData* data)
{
    if (data->receiveType == 1)
    {
        s_pInstance->TrackStat(
            STATS_PASSES_RECEIVED, data->pPlayer->m_pTeam->m_nSide,
            data->pPlayer->m_ID, 0, 0, 0, 0);
    }
    else if (data->receiveType == 2)
    {
        s_pInstance->TrackStat(STATS_PASSES_INTERCEPTED,
            data->pPlayer->m_pTeam->m_nSide, data->pPlayer->m_ID,
            0, 0, 0, 0);
    }
}

void StatsTracker::OnPassBall(PassBallData* data)
{
    s_pInstance->TrackStat(STATS_PASSES_MADE,
        data->pPasser->m_pTeam->m_nSide,
        data->pPasser->m_ID, data->mPasserControllerID, 0, 0, 0);
    if (data->bVolleyPass)
    {
        s_pInstance->TrackStat(STATS_0F,
            data->pPasser->m_pTeam->m_nSide,
            data->pPasser->m_ID, data->mPasserControllerID, 0, 0, 0);
    }
    else
    {
        s_pInstance->TrackStat(STATS_0E,
            data->pPasser->m_pTeam->m_nSide,
            data->pPasser->m_ID, data->mPasserControllerID, 0, 0, 0);
    }
}

void StatsTracker::OnPenalty(PenaltyStatsData* data)
{
    s_pInstance->TrackStat(STATS_FOULS, data->pPlayer->m_pTeam->m_nSide,
        data->pPlayer->m_ID, 0, 0, 0, 0);
}

void StatsTracker::OnGoalieSave(GoalieSaveData* data)
{
    cTeam* team = data->pGoalie->m_pTeam->GetOtherTeam();
    cPlayer* shooter = data->pShooter;
    if (shooter != 0)
    {
        s_pInstance->TrackStat(
            STATS_SHOTS_ON_GOAL, team->m_nSide, shooter->m_ID, 1, 0, 0, 0);
    }
}

void StatsTracker::OnBallStateChange(int previousState, int currentState)
{
    if (previousState == 8 && currentState != 8 && g_pBall->m_pShooter != 0)
    {
        s_pInstance->TrackStat(
            STATS_04, g_pBall->m_pShooter->m_pTeam->m_nSide,
            g_pBall->m_pShooter->m_ID, 1, 0, 0, 0);
    }
}

void StatsTracker::OnCollisionBallGoalpost(UnidentifiedEventData07*)
{
    nlVector3 ballVelocity = g_pBall->m_v3Velocity;
    if (g_pBall != 0 && g_pBall->m_pShooter != 0
        && nlSqrt(ballVelocity.GetLengthSq3D(), true) > 0.05f)
    {
        s_pInstance->TrackStat(STATS_SHOTS_ON_GOAL,
            g_pBall->m_pShooter->m_pTeam->m_nSide,
            g_pBall->m_pShooter->m_ID, 1, 0, 0, 0);
    }
}

void StatsTracker::TrackStat(ePlayerStats stat, int homeaway,
    int playerindex, int param0, int, int, int)
{
    if (stat < STATS_00 || stat >= NUM_STATS || homeaway < 0
        || homeaway >= 2)
    {
        return;
    }

    int amount = param0 == 0 ? 1 : param0;
    AddStat(stat, homeaway, playerindex, amount);

    if (playerindex >= 0 && mBasicGameInfo != 0)
    {
        for (int pad = 0; pad < 16; pad++)
        {
            if (mBasicGameInfo->mPadSides[pad] == homeaway)
            {
                AddUserStatByPad(stat, pad, amount);
            }
        }
    }
}

void StatsTracker::GetSortedStats(PlayerStats* source, int numsource,
    int* dest, int numelements, ePlayerStats statType,
    eSortOrder sortOrder)
{
    if (numelements > numsource)
    {
        numelements = numsource;
    }

    for (int i = 0; i < numsource; i++)
    {
        dest[i] = i;
    }

    for (int i = 0; i < numsource - 1; i++)
    {
        for (int j = 0; j < numsource - i - 1; j++)
        {
            int a = GetStatValue(source[dest[j]], statType);
            int b = GetStatValue(source[dest[j + 1]], statType);
            bool move = sortOrder == SORT_ASCENDING ? a > b : a < b;
            if (move)
            {
                int temp = dest[j];
                dest[j] = dest[j + 1];
                dest[j + 1] = temp;
            }
        }
    }

    for (int i = numelements; i < numsource; i++)
    {
        dest[i] = -1;
    }
}

void StatsTracker::GetSortedTeamStats(
    TeamStats* source, int numsource, int* dest, int numelements)
{
    if (numelements > numsource)
    {
        numelements = numsource;
    }

    for (int i = 0; i < numsource; i++)
    {
        dest[i] = i;
    }

    for (int i = 0; i < numsource - 1; i++)
    {
        for (int j = 0; j < numsource - i - 1; j++)
        {
            if (MoveTeamBUp(source[dest[j]], source[dest[j + 1]]))
            {
                int temp = dest[j];
                dest[j] = dest[j + 1];
                dest[j + 1] = temp;
            }
        }
    }

    for (int i = numelements; i < numsource; i++)
    {
        dest[i] = -1;
    }
}

void StatsTracker::CompileEndOfGameStats()
{
    for (int team = 0; team < 2; team++)
    {
        for (int stat = 0; stat < NUM_STATS; stat++)
        {
            AddStatValue(mCumulativeTeamStats[team]->mPlayerTotalStats,
                (ePlayerStats)stat,
                GetStatValue(mCurrentTeamStats[team].mPlayerTotalStats,
                    (ePlayerStats)stat));
        }
    }

    for (int pad = 0; pad < 16; pad++)
    {
        for (int stat = 0; stat < NUM_STATS; stat++)
        {
            AddStatValue(mCumulativeUserStats[pad], (ePlayerStats)stat,
                GetStatValue(mCurrentUserStats[pad], (ePlayerStats)stat));
        }
    }
}

void StatsTracker::SimulateGame()
{
    m_pSimulator->fn_80109E34();
}

void StatsTracker::AddStat(
    ePlayerStats stat, int team, int player, int value)
{
    if (team < 0 || team >= 2)
    {
        return;
    }

    AddStatValue(mCurrentTeamStats[team].mPlayerTotalStats, stat, value);
    if (player >= 0 && player < 5)
    {
        AddStatValue(mCurrentPlayerStats[team][player], stat, value);
    }
}

void StatsTracker::AddUserStatByPad(
    ePlayerStats stat, int pad, int amount)
{
    if (pad >= 0 && pad < 16)
    {
        AddStatValue(mCurrentUserStats[pad], stat, amount);
        AddStatValue(mCumulativeUserStats[pad], stat, amount);
    }
}

void StatsTracker::AddMilestoneUserStat(ePlayerStats stat, int amount)
{
    for (int pad = 0; pad < 16; pad++)
    {
        if (mBasicGameInfo->mPadSides[pad] != -1)
        {
            AddUserStatByPad(stat, pad, amount);
        }
    }
}

void StatsTracker::TrackWinner(int forfeitSide)
{
    if (mHasGameEnded)
    {
        return;
    }

    int homeScore = mBasicGameInfo->mFinalScore[0];
    int awayScore = mBasicGameInfo->mFinalScore[1];
    int winningSide = homeScore > awayScore ? 0 : 1;
    if (forfeitSide == 0 || forfeitSide == 1)
    {
        winningSide = 1 - forfeitSide;
    }

    s_pInstance->TrackStat(mIsOvertime ? STATS_21 : STATS_WIN, winningSide, 0,
        homeScore, awayScore, 0, 0);
    CompileEndOfGameStats();
    mNumGamesWon[winningSide]++;
    mHasGameEnded = true;
}

static int CountNewlines(FILE* file)
{
    fn_80380C34(file, 0, 0);
    int count = 0;
    char character;
    while ((character = fn_8037FA00(file)) != -1)
    {
        if (character == '\n')
        {
            count++;
        }
    }
    return count;
}

void StatsTracker::WriteStats(
    float gameTime, float gameDuration, const char* filename)
{
    if (gameDuration <= 0.0f)
    {
        gameDuration = 10.0f;
    }
    if (filename == 0)
    {
        filename = STATS_FILE;
    }

    bool firstTime = true;
    FILE* file = fopen(filename, "r");
    if (file != 0)
    {
        firstTime = CountNewlines(file) == 0;
        fclose(file);
    }

    file = fopen(filename, firstTime ? "wt" : "at");
    if (file == 0)
    {
        return;
    }

    if (firstTime)
    {
        NLString header;
        header.AppendInPlace("GameID,");
        header.AppendInPlace("Side,");
        header.AppendInPlace("Stadium,");
        header.AppendInPlace("Game Time,");
        header.AppendInPlace("NumHumans,");
        header.AppendInPlace("Captain,");
        header.AppendInPlace("Movement Rating,");
        header.AppendInPlace("Shoot Rating,");
        header.AppendInPlace("Pass Rating,");
        header.AppendInPlace("Defense Rating,");
        header.AppendInPlace("Difficulty,\n");
        fwrite(header.c_str(), 1, header.size(), file);
    }

    for (int side = 0; side < 2; side++)
    {
        int numHumans = 0;
        for (int pad = 0; pad < 16; pad++)
        {
            if (GameInfoManager::Instance()->GetPlayingSide((u16)pad) == side)
            {
                numHumans++;
            }
        }

        NLString line = Format(NLString("{0},{1},{2},{3},{4},{5},"),
            (int)gameTime, side,
            GameInfoManager::Instance()->GetStadium(),
            (int)gameDuration, numHumans,
            GameInfoManager::Instance()->GetTeam((short)side));
        line.AppendInPlace(Format(NLString("{0},{1},{2},{3},\n"),
            0, 0, 0,
            GameInfoManager::Instance()->mCurrentDifficulty[side]));
        fwrite(line.c_str(), 1, line.size(), file);
    }

    fclose(file);
}

bool StatsTracker::MoveTeamBUp(TeamStats b, TeamStats a)
{
    int bGoals = b.mPlayerTotalStats.mNumGoalsFor;
    int aGoals = a.mPlayerTotalStats.mNumGoalsFor;
    if (aGoals > bGoals)
        return true;
    if (bGoals > aGoals)
        return false;

    int bShots = b.mPlayerTotalStats.mNumShotsOnGoal;
    int aShots = a.mPlayerTotalStats.mNumShotsOnGoal;
    if (aShots > bShots)
        return true;
    if (bShots > aShots)
        return false;

    int bHits = b.mPlayerTotalStats.mNumHitsMade;
    int aHits = a.mPlayerTotalStats.mNumHitsMade;
    if (aHits > bHits)
        return true;
    if (bHits > aHits)
        return false;

    int bInterSteals = b.mPlayerTotalStats.mNumPassesIntercepted
                     + b.mPlayerTotalStats.mNumSteals;
    int aInterSteals = a.mPlayerTotalStats.mNumPassesIntercepted
                     + a.mPlayerTotalStats.mNumSteals;
    if (aInterSteals > bInterSteals)
        return true;
    if (bInterSteals > aInterSteals)
        return false;

    int bPowerups = b.mPlayerTotalStats.mNumPowerupsUsed;
    int aPowerups = a.mPlayerTotalStats.mNumPowerupsUsed;
    if (aPowerups > bPowerups)
        return true;
    if (bPowerups > aPowerups)
        return false;

    int bPerfectPasses = b.mPlayerTotalStats.mNumPerfectPasses;
    int aPerfectPasses = a.mPlayerTotalStats.mNumPerfectPasses;
    if (aPerfectPasses > bPerfectPasses)
        return true;
    if (bPerfectPasses > aPerfectPasses)
        return false;

    int bButtonPresses = b.mPlayerTotalStats.mNumButtonPresses;
    int aButtonPresses = a.mPlayerTotalStats.mNumButtonPresses;
    if (aButtonPresses > bButtonPresses)
        return true;
    if (bButtonPresses > aButtonPresses)
        return false;

    return (int)a.mTeamIndex < (int)b.mTeamIndex;
}

void StatsTracker::Track(ePlayerStats stat, int homeaway, int playerindex,
    int param0, int param1, int param2, int param3)
{
    s_pInstance->TrackStat(
        stat, homeaway, playerindex, param0, param1, param2, param3);
}

static int GetStatValue(const PlayerStats& stats, ePlayerStats stat)
{
    switch (stat)
    {
    case STATS_00: return stats.unknown_0x00;
    case STATS_01: return stats.unknown_0x02;
    case STATS_02: return stats.unknown_0x04;
    case STATS_SHOTS_ON_GOAL: return stats.mNumShotsOnGoal;
    case STATS_04: return stats.unknown_0x14;
    case STATS_05: return stats.unknown_0x08;
    case STATS_06: return stats.unknown_0x0A;
    case STATS_07: return stats.unknown_0x0C;
    case STATS_08: return stats.unknown_0x0E;
    case STATS_09: return stats.unknown_0x16;
    case STATS_0A: return stats.unknown_0x18;
    case STATS_GOALS_FOR: return stats.mNumGoalsFor;
    case STATS_0C: return stats.unknown_0x12;
    case STATS_PASSES_MADE: return stats.mNumPassesMade;
    case STATS_0E: return stats.unknown_0x2C;
    case STATS_0F: return stats.unknown_0x2E;
    case STATS_PASSES_RECEIVED: return stats.mNumPassesReceived;
    case STATS_FOULS: return stats.mNumFouls;
    case STATS_12: return stats.mNumHitsMade;
    case STATS_ATTACK_ATTEMPTS: return stats.unknown_0x34;
    case STATS_ATTACK_SUCCESSES: return stats.mNumSteals;
    case STATS_15: return stats.unknown_0x38;
    case STATS_16: return stats.unknown_0x3C;
    case STATS_17: return stats.mNumButtonPresses;
    case STATS_18: return stats.unknown_0x1C;
    case STATS_19: return stats.mNumPowerupsUsed;
    case STATS_1A: return stats.unknown_0x20;
    case STATS_1B: return stats.unknown_0x22;
    case STATS_1C: return stats.unknown_0x24;
    case STATS_1D: return stats.unknown_0x26;
    case STATS_POWERUPS_USED: return stats.unknown_0x28;
    case STATS_PERFECT_PASSES: return stats.mNumPerfectPasses;
    case STATS_PASSES_INTERCEPTED: return stats.mNumPassesIntercepted;
    case STATS_25: return stats.unknown_0x46;
    case STATS_26: return stats.unknown_0x48;
    default: return -1;
    }
}

void StatsTracker::WriteCurrentlyPlaying() const
{
    FILE* file = fopen("currently_playing.txt", "wt");
    if (file == 0)
    {
        return;
    }

    NLString text = Format(
        NLString("Home: {0} with {1}\nAway: {2} with {3}\nStadium: {4}\n"),
        fn_801CBE80(GameInfoManager::Instance()->GetTeam(0)),
        fn_801CBEA8(GameInfoManager::Instance()->GetSidekick(0, 0)),
        fn_801CBE80(GameInfoManager::Instance()->GetTeam(1)),
        fn_801CBEA8(GameInfoManager::Instance()->GetSidekick(1, 0)),
        fn_801B6188(GameInfoManager::Instance()->GetStadium()));

    fwrite(text.c_str(), 1, text.size(), file);
    fclose(file);
}
