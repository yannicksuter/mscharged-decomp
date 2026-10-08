#include "NL/nlDLListContainer.inl"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feHelpFuncs_decl.h"

#include <stdio.h>

#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/OverlayManager.h"
#include "Game/PassBallData.h"
#include "Game/Team.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlFunction.inl"

struct GoalScoredStatsData
{
    /* 0x00 */ GoalScoredData data;
    /* 0x20 */ int nScorerPadID;
};

template <typename P1, typename P2>
class StatsTypedEvent2 : public EventBase
{
public:
    StatsTypedEvent2(const char* name, int length)
        : EventBase(name, length)
    {
    }

    virtual ~StatsTypedEvent2() { }
    virtual void Disconnect(void*) = 0;
    virtual void Add(Function2<void, P1, P2>, unsigned int, int) = 0;
};

template <>
StatsTracker* nlSingleton<StatsTracker>::s_pInstance = 0;

static const char* STATS_FILE = "statsfile.csv";

template <typename T>
static inline UnidentifiedTypedEvent<T>* FindStatsEvent(const char* name)
{
    unsigned int hash = HashEventName(name, -1);
    EventRegistryValue* foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    EventBase* event = foundEvent != 0 ? foundEvent->event : 0;
    return (UnidentifiedTypedEvent<T>*)event;
}

template <typename P1, typename P2>
static inline StatsTypedEvent2<P1, P2>* FindStatsEvent2(const char* name)
{
    unsigned int hash = HashEventName(name, -1);
    EventRegistryValue* foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    EventBase* event = foundEvent != 0 ? foundEvent->event : 0;
    return (StatsTypedEvent2<P1, P2>*)event;
}

static inline void InitializePlayerStats(
    PlayerStats& stats, int record, eType type)
{
    memset(&stats, 0, sizeof(stats));
    stats.mRecordType.mControllerID = record;
    stats.mType = type;
}

static inline void AddStatValue(
    PlayerStats& stats, ePlayerStats stat, int amount)
{
    switch (stat)
    {
    case STATS_00:
        stats.mNumLowChargeShots += amount;
        break;
    case STATS_01:
        stats.mNumMediumChargeShots += amount;
        break;
    case STATS_02:
        stats.mNumHighChargeShots += amount;
        break;
    case STATS_SHOTS_ON_GOAL:
        stats.mNumShotsOnGoal += amount;
        stats.mNumShotsOnGoal = stats.mNumShotsOnGoal <= 999U ? stats.mNumShotsOnGoal : 999U;
        break;
    case STATS_REGULAR_GOALS:
        stats.mNumRegularGoals += amount;
        break;
    case STATS_ONE_TIMER_GOALS:
        stats.mNumOneTimerGoals += amount;
        break;
    case STATS_STS_GOALS:
        stats.mNumSTSGoals += amount;
        break;
    case STATS_ASSISTS:
        stats.mNumAssists += amount;
        break;
    case STATS_GOALS_FOR:
        stats.mNumGoalsFor += amount;
        stats.mNumGoalsFor = stats.mNumGoalsFor <= 999U ? stats.mNumGoalsFor : 999U;
        break;
    case STATS_GOALS_AGAINST:
        stats.mNumGoalsAgainst += amount;
        stats.mNumGoalsAgainst = stats.mNumGoalsAgainst <= 999U ? stats.mNumGoalsAgainst : 999U;
        break;
    case STATS_STS_ATTEMPTS:
        stats.mNumSTSAttempts += amount;
        stats.mNumSTSAttempts = stats.mNumSTSAttempts <= 999U ? stats.mNumSTSAttempts : 999U;
        break;
    case STATS_MEGASTRIKE_ATTEMPTS:
        stats.mNumMegaStrikeAttempts += amount;
        stats.mNumMegaStrikeAttempts = stats.mNumMegaStrikeAttempts <= 999U ? stats.mNumMegaStrikeAttempts : 999U;
        break;
    case STATS_MEGASTRIKE_GOALS:
        stats.mNumMegaStrikeGoals += amount;
        stats.mNumMegaStrikeGoals = stats.mNumMegaStrikeGoals <= 999U ? stats.mNumMegaStrikeGoals : 999U;
        break;
    case STATS_FOULS:
        stats.mNumFouls += amount;
        break;
    case STATS_GAMES_PLAYED:
        stats.mNumGamesPlayed += amount;
        break;
    case STATS_POWERUPS_USED:
        stats.mNumPowerupsUsed += amount;
        break;
    case STATS_MUSHROOMS_USED:
        stats.mNumMushroomsUsed += amount;
        break;
    case STATS_DRAWABLE_POWERUPS_USED:
        stats.mNumDrawablePowerupsUsed += amount;
        break;
    case STATS_STARS_AND_CHAIN_CHOMPS_USED:
        stats.mNumStarsAndChainChompsUsed += amount;
        break;
    case STATS_CAPTAIN_POWERUPS_USED:
        stats.mNumCaptainPowerupsUsed += amount;
        break;
    case STATS_PASSES_MADE:
        stats.mNumPassesMade += amount;
        break;
    case STATS_NON_VOLLEY_PASSES:
        stats.mNumNonVolleyPasses += amount;
        break;
    case STATS_VOLLEY_PASSES:
        stats.mNumVolleyPasses += amount;
        break;
    case STATS_PASSES_RECEIVED:
        stats.mNumPassesReceived += amount;
        break;
    case STATS_HITS_MADE:
        stats.mNumHitsMade += amount;
        stats.mNumHitsMade = stats.mNumHitsMade <= 999U ? stats.mNumHitsMade : 999U;
        break;
    case STATS_ATTACK_ATTEMPTS:
        stats.mNumAttackAttempts += amount;
        break;
    case STATS_ATTACK_SUCCESSES:
        stats.mNumSteals += amount;
        stats.mNumSteals = stats.mNumSteals <= 999U ? stats.mNumSteals : 999U;
        break;
    case STATS_15:
        stats.unknown_0x38 += amount;
        break;
    case STATS_16:
        stats.mBallPossessionTime += amount;
        break;
    case STATS_BUTTON_PRESSES:
        stats.mNumButtonPresses += amount;
        break;
    case STATS_PERFECT_PASSES:
        stats.mNumPerfectPasses += amount;
        break;
    case STATS_25:
        stats.unknown_0x46 += amount;
        break;
    case STATS_26:
        stats.unknown_0x48 += amount;
        break;
    case STATS_POWERUPS_HIT:
        stats.mNumPowerupsHit += amount;
        break;
    case STATS_PASSES_INTERCEPTED:
        stats.mNumPassesIntercepted += amount;
        break;
    default:
        break;
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
    eSidekickID homesk;
    eSidekickID awaysk;
    eTeamID homeid;
    eTeamID awayid;
    eCharacterClass characterClass;
    int i;
    int j;

    mBasicGameInfo = pGameInfo;
    homeid = (eTeamID)mBasicGameInfo->mTeamIndex[0];
    awayid = (eTeamID)mBasicGameInfo->mTeamIndex[1];

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

    mCumulativeTeamStats[0]->Initialize(homeid);
    mCumulativeTeamStats[1]->Initialize(awayid);
    mCurrentTeamStats[0].Initialize(homeid);
    mCurrentTeamStats[1].Initialize(awayid);

    characterClass = (eCharacterClass)ConvertToCharacterClass(homeid);
    InitializePlayerStats(
        mCurrentPlayerStats[0][0], characterClass, TYPE_CHARACTER);

    characterClass = (eCharacterClass)ConvertToCharacterClass(awayid);
    InitializePlayerStats(
        mCurrentPlayerStats[1][0], characterClass, TYPE_CHARACTER);

    i = 1;
    do
    {
        homesk = mBasicGameInfo->GetSidekick(0, i - 1);
        awaysk = mBasicGameInfo->GetSidekick(1, i - 1);
        characterClass = (eCharacterClass)ConvertToCharacterClass(homesk);
        InitializePlayerStats(mCurrentPlayerStats[0][i],
            characterClass, TYPE_CHARACTER);
        characterClass = (eCharacterClass)ConvertToCharacterClass(awaysk);
        InitializePlayerStats(mCurrentPlayerStats[1][i],
            characterClass, TYPE_CHARACTER);
        i++;
    } while (i < sizeof(mCurrentPlayerStats[0]) / sizeof(mCurrentPlayerStats[0][0]));

    j = 0;
    do
    {
        InitializePlayerStats(mCurrentUserStats[j], j, TYPE_USER);
        InitializePlayerStats(mCumulativeUserStats[j], j, TYPE_USER);
        j++;
    } while (j < 4);
}

void StatsTracker::ResetCurrentStats()
{
    mIsOvertime = false;
    mHasGameEnded = false;

    mCumulativeTeamStats[0]->Initialize(
        mCurrentTeamStats[0].mTeamIndex);
    mCumulativeTeamStats[1]->Initialize(
        mCurrentTeamStats[1].mTeamIndex);

    mNumConsecutiveGamesPlayed++;
    mBasicGameInfo->SetFinalScore(0, 0);
    mBasicGameInfo->SetFinalScore(1, 0);

    for (int i = 0; i < 4; i++)
    {
        InitializePlayerStats(mCurrentUserStats[i], i, TYPE_USER);
    }

    static_cast<OverlayManager*>(g_pOverlayManager)->ResetStrikerTimesVariants();
}

#include "Game/DB/StatsTracker.inl"

void StatsTracker::CreateEventHandler()
{
    FindStatsEvent<PenaltyData>("Penalty")->Add(Function<PenaltyData*>(OnPenalty), 0, -1);
    FindStatsEvent<GoalieSaveData>("GoalieSave")->Add(Function<GoalieSaveData*>(OnGoalieSave), 0, -1);
    FindStatsEvent<PassBallData>("PassBall")->Add(Function<PassBallData*>(OnPassBall), 0, -1);
    FindStatsEvent<ReceiveBallData>("ReceiveBall")->Add(Function<ReceiveBallData*>(OnReceiveBall), 0, -1);
    FindStatsEvent<GoalScoredStatsData>("GoalScored")->Add(Function<GoalScoredStatsData*>(OnGoalScored), 0, -1);
    FindStatsEvent<MegaStrikeEndData>("MegastrikeEnd")->Add(Function<MegaStrikeEndData*>(OnMegastrikeEnd), 0, -1);
    FindStatsEvent<PlayerAttackData>("AttackSuccess")->Add(Function<PlayerAttackData*>(OnAttackSuccess), 0, -1);
    FindStatsEvent<PlayerAttackData>("AttackAttempt")->Add(Function<PlayerAttackData*>(OnAttackAttempt), 0, -1);
    FindStatsEvent<CollisionPowerupStatsData>("PowerupStats")->Add(Function<CollisionPowerupStatsData*>(OnPowerupStats), 0, -1);
    FindStatsEvent2<int, int>("BallStateChange")->Add(Function2<void, int, int>(OnBallStateChange), 0, -1);
    FindStatsEvent<CollisionBallGoalpostData>("CollisionBallGoalpost")->Add(Function<CollisionBallGoalpostData*>(OnCollisionBallGoalpost), 0, -1);
}

void StatsTracker::DestroyEventHandler()
{
}

void StatsTracker::OnPowerupStats(CollisionPowerupStatsData* data)
{
    if (data->pPlayer != 0)
    {
        Instance()->TrackStat(STATS_POWERUPS_HIT,
            data->pPlayer->m_pTeam->m_nSide, data->pPlayer->mUnidentified1E4.m_ID,
            data->nThrowerPadID, 0, 0, 0);
    }
}

void StatsTracker::OnAttackSuccess(PlayerAttackData* data)
{
    if (data->mUnidentified10 && data->pAttacker != 0 && data->pAttacker->m_pBall != 0)
    {
        Instance()->TrackStat(STATS_ATTACK_SUCCESSES,
            data->pAttacker->m_pTeam->m_nSide, data->pAttacker->mUnidentified1E4.m_ID,
            data->nAttackerPadID, 0, 0, 0);
    }
}

void StatsTracker::OnAttackAttempt(PlayerAttackData* data)
{
    if (data->mUnidentified10)
    {
        Instance()->TrackStat(STATS_ATTACK_ATTEMPTS,
            data->pAttacker->m_pTeam->m_nSide, data->pAttacker->mUnidentified1E4.m_ID,
            data->nAttackerPadID, 0, 0, 0);
    }
}

void StatsTracker::OnGoalScored(GoalScoredStatsData* data)
{
    s_pInstance->TrackStat(STATS_GOALS_FOR, data->data.uTeamIndex,
        data->data.pScorer != 0 ? data->data.pScorer->mUnidentified1E4.m_ID : -1,
        data->data.pAssister != 0 ? data->data.pAssister->mUnidentified1E4.m_ID : -1,
        data->data.uGoalType, data->data.uNumGoalsScored, data->nScorerPadID);

    bool scoreTied = g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore;
    if (g_pGame != 0)
    {
        float gameDuration = g_pGame->m_fGameDuration;
        if (!((unsigned int)(10.0f * (gameDuration - g_pGame->GetGameTime())) == 0
                && !scoreTied
                && GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0))
        {
            int teamScore = g_pTeams[data->data.uTeamIndex]->m_nScore;
            if (teamScore < GameInfoManager::Instance()->GetCurrentSettings()->GoalLimit
                || GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType != 1)
            {
                goto skipTrackWinner;
            }
        }
        s_pInstance->TrackWinner(-1);
    }

skipTrackWinner:
    if (data->data.uTeamIndex != data->data.pScorer->m_pTeam->m_nSide)
    {
        s_pInstance->TrackStat(
            STATS_SHOTS_ON_GOAL, data->data.uTeamIndex, 0, 1, 0, 0, 0);
    }
    else
    {
        s_pInstance->TrackStat(STATS_SHOTS_ON_GOAL,
            data->data.pScorer->m_pTeam->m_nSide,
            data->data.pScorer->mUnidentified1E4.m_ID, 1, 0, 0, 0);
    }
}

void StatsTracker::OnMegastrikeEnd(MegaStrikeEndData* data)
{
    int side = 1 - data->defendingSide;
    if (data->goals > 0)
    {
        s_pInstance->TrackStat(STATS_GOALS_FOR, side, data->pPlayer->mUnidentified1E4.m_ID, -1, 6,
            data->goals, data->goalValue);

        bool scoreTied = g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore;
        if (g_pGame != 0)
        {
            float gameDuration = g_pGame->m_fGameDuration;
            if (!((unsigned int)(10.0f * (gameDuration - g_pGame->GetGameTime())) == 0
                    && !scoreTied
                    && GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0))
            {
                int teamScore = g_pTeams[side]->m_nScore;
                if (teamScore < GameInfoManager::Instance()->GetCurrentSettings()->GoalLimit
                    || GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType != 1)
                {
                    goto skipTrackWinner;
                }
            }
            s_pInstance->TrackWinner(-1);
        }
    }

skipTrackWinner:
    s_pInstance->TrackStat(
        STATS_MEGASTRIKE_ATTEMPTS, side, data->pPlayer->mUnidentified1E4.m_ID, data->attempts, 0, 0, 0);
    s_pInstance->TrackStat(
        STATS_MEGASTRIKE_GOALS, side, data->pPlayer->mUnidentified1E4.m_ID, data->goals, 0, 0, 0);
    s_pInstance->TrackStat(STATS_SHOTS_ON_GOAL, side, data->pPlayer->mUnidentified1E4.m_ID,
        data->attempts, 0, 0, 0);
}

void StatsTracker::OnReceiveBall(ReceiveBallData* data)
{
    if (data->eResult == RECEIVEBALL_PASS_COMPLETE)
    {
        s_pInstance->TrackStat(
            STATS_PASSES_RECEIVED, data->pReceiver->m_pTeam->m_nSide,
            data->pReceiver->mUnidentified1E4.m_ID, 0, 0, 0, 0);
    }
    else if (data->eResult == RECEIVEBALL_PASS_INTERCEPT)
    {
        s_pInstance->TrackStat(STATS_PASSES_INTERCEPTED,
            data->pReceiver->m_pTeam->m_nSide, data->pReceiver->mUnidentified1E4.m_ID,
            0, 0, 0, 0);
    }
}

void StatsTracker::OnPassBall(PassBallData* data)
{
    s_pInstance->TrackStat(STATS_PASSES_MADE,
        data->pPasser->m_pTeam->m_nSide,
        data->pPasser->mUnidentified1E4.m_ID, data->mPasserControllerID, 0, 0, 0);
    if (data->bVolleyPass)
    {
        s_pInstance->TrackStat(STATS_VOLLEY_PASSES,
            data->pPasser->m_pTeam->m_nSide,
            data->pPasser->mUnidentified1E4.m_ID, data->mPasserControllerID, 0, 0, 0);
    }
    else
    {
        s_pInstance->TrackStat(STATS_NON_VOLLEY_PASSES,
            data->pPasser->m_pTeam->m_nSide,
            data->pPasser->mUnidentified1E4.m_ID, data->mPasserControllerID, 0, 0, 0);
    }
}

void StatsTracker::OnPenalty(PenaltyData* data)
{
    s_pInstance->TrackStat(STATS_FOULS, data->pFouler->m_pTeam->m_nSide,
        data->pFouler->mUnidentified1E4.m_ID, 0, 0, 0, 0);
}

void StatsTracker::OnGoalieSave(GoalieSaveData* data)
{
    cTeam* team = data->pGoalie->m_pTeam->GetOtherTeam();
    cPlayer* shooter = data->pShooter;
    if (shooter != 0)
    {
        s_pInstance->TrackStat(
            STATS_SHOTS_ON_GOAL, team->m_nSide, shooter->mUnidentified1E4.m_ID, 1, 0, 0, 0);
    }
}

void StatsTracker::OnBallStateChange(int previousState, int currentState)
{
    if (previousState == 8 && currentState != 8 && g_pBall->m_pShooter != 0)
    {
        s_pInstance->TrackStat(
            STATS_STS_ATTEMPTS, g_pBall->m_pShooter->m_pTeam->m_nSide,
            g_pBall->m_pShooter->mUnidentified1E4.m_ID, 1, 0, 0, 0);
    }
}

void StatsTracker::OnCollisionBallGoalpost(CollisionBallGoalpostData*)
{
    nlVector3 ballVelocity = g_pBall->m_v3Velocity;
    if (g_pBall != 0 && g_pBall->m_pShooter != 0
        && nlSqrt(ballVelocity.GetLengthSq3D(), true) > 0.05f)
    {
        s_pInstance->TrackStat(STATS_SHOTS_ON_GOAL,
            g_pBall->m_pShooter->m_pTeam->m_nSide,
            g_pBall->m_pShooter->mUnidentified1E4.m_ID, 1, 0, 0, 0);
    }
}

static unsigned char GetGlobalPadID(int homeaway, int playerindex, int& padid)
{
    cTeam* team = g_pTeams[homeaway];
    if (team != 0)
    {
        DetInput* globalPad = team->GetPlayer(playerindex)->GetGlobalPad();
        if (globalPad != 0)
        {
            padid = globalPad->GetPadID();
            return true;
        }
    }

    padid = -1;
    return false;
}

void StatsTracker::TrackStat(ePlayerStats stat, int homeaway,
    int playerindex, int param0, int param1, int param2, int param3)
{
    switch (stat)
    {
    case STATS_SHOTS_ON_GOAL:
        AddStat(STATS_SHOTS_ON_GOAL, homeaway, playerindex, param0);
        AddUserStatByPlayer(STATS_SHOTS_ON_GOAL, homeaway, playerindex, param0);
        break;
    case STATS_00:
    case STATS_01:
    case STATS_02:
        AddStat(stat, homeaway, playerindex, 1);
        AddUserStatByPlayer(stat, homeaway, playerindex, 1);
        break;
    case STATS_GOALS_FOR:
        AddStat(STATS_GOALS_FOR, homeaway, playerindex, param2);
        AddUserStatByPad(STATS_GOALS_FOR, param3, param2);
        switch (param1)
        {
        case 1:
            Track(STATS_ONE_TIMER_GOALS, homeaway, playerindex, param2, param3, 0, 0);
            break;
        case 0:
        case 7:
            Track(STATS_REGULAR_GOALS, homeaway, playerindex, param2, param3, 0, 0);
            break;
        case 2:
            Track(STATS_STS_GOALS, homeaway, playerindex, param2, param3, 0, 0);
            break;
        }
        if (param0 >= 0)
            Track(STATS_ASSISTS, homeaway, param0, 0, 0, 0, 0);
        Track(STATS_GOALS_AGAINST, homeaway == 0, playerindex, param2, 0, 0, 0);
        break;
    case STATS_GOALS_AGAINST:
        AddStat(STATS_GOALS_AGAINST, homeaway, -1, param0);
        for (u32 i = 0; i < 5; i++)
            AddUserStatByPlayer(STATS_GOALS_AGAINST, homeaway, i, param0);
        break;
    case STATS_26:
        AddStat(STATS_26, homeaway, playerindex, param0);
        AddUserStatByPad(STATS_26, param1, param0);
        break;
    case STATS_REGULAR_GOALS:
    case STATS_ONE_TIMER_GOALS:
    case STATS_STS_GOALS:
        AddStat(stat, homeaway, playerindex, param0);
        AddUserStatByPad(stat, param1, param0);
        break;
    case STATS_ASSISTS:
        AddStat(STATS_ASSISTS, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_ASSISTS, homeaway, playerindex, 1);
        break;
    case STATS_FOULS:
        AddStat(STATS_FOULS, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_FOULS, homeaway, playerindex, 1);
        break;
    case STATS_WIN:
    {
        mIsUserCupWinner = false;
        AddStat(STATS_WIN, homeaway, -1, 1);
        mBasicGameInfo->SetFinalScore(0, param0);
        mBasicGameInfo->SetFinalScore(1, param1);
        Track(STATS_LOSS, homeaway == 0, 0, 0, 0, 0, 0);
        GameInfoManager* gameInfoManager = GameInfoManager::Instance();
        CupManager* cupManager = CupManager::Instance();
        if (gameInfoManager->IsInMode3() == 1)
        {
            if (mBasicGameInfo->GetTeam(homeaway) == cupManager->GetUserSelectedCupTeam())
                cupManager->GetCupRecord().mCurrentRecord.mValues[0]++;
        }
        break;
    }
    case STATS_OT_WIN:
    {
        mIsUserCupWinner = false;
        AddStat(STATS_OT_WIN, homeaway, -1, 1);
        mBasicGameInfo->SetFinalScore(0, param0);
        mBasicGameInfo->SetFinalScore(1, param1);
        Track(STATS_OT_LOSS, homeaway == 0, 0, 0, 0, 0, 0);
        GameInfoManager* gameInfoManager = GameInfoManager::Instance();
        CupManager* cupManager = CupManager::Instance();
        if (gameInfoManager->IsInMode3() == 1)
        {
            if (mBasicGameInfo->GetTeam(homeaway) == cupManager->GetUserSelectedCupTeam())
                cupManager->GetCupRecord().mCurrentRecord.mValues[0]++;
        }
        break;
    }
    case STATS_LOSS:
        AddStat(STATS_LOSS, homeaway, -1, 1);
        if (GameInfoManager::Instance()->IsInMode3() == 1)
        {
            if (mBasicGameInfo->GetTeam(homeaway) == CupManager::Instance()->GetUserSelectedCupTeam())
                CupManager::Instance()->GetCupRecord().mCurrentRecord.mValues[1]++;
        }
        break;
    case STATS_OT_LOSS:
        AddStat(STATS_OT_LOSS, homeaway, -1, 1);
        if (GameInfoManager::Instance()->IsInMode3() == 1)
        {
            if (mBasicGameInfo->GetTeam(homeaway) == CupManager::Instance()->GetUserSelectedCupTeam())
                CupManager::Instance()->GetCupRecord().mCurrentRecord.mValues[2]++;
        }
        break;
    case STATS_15:
        AddStat(STATS_15, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_15, homeaway, playerindex, 1);
        break;
    case STATS_POWERUPS_USED:
        AddStat(STATS_POWERUPS_USED, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_POWERUPS_USED, homeaway, playerindex, 1);
        break;
    case STATS_MUSHROOMS_USED:
        AddStat(STATS_MUSHROOMS_USED, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_MUSHROOMS_USED, homeaway, playerindex, 1);
        break;
    case STATS_DRAWABLE_POWERUPS_USED:
        AddStat(STATS_DRAWABLE_POWERUPS_USED, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_DRAWABLE_POWERUPS_USED, homeaway, playerindex, 1);
        break;
    case STATS_STARS_AND_CHAIN_CHOMPS_USED:
        AddStat(STATS_STARS_AND_CHAIN_CHOMPS_USED, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_STARS_AND_CHAIN_CHOMPS_USED, homeaway, playerindex, 1);
        break;
    case STATS_CAPTAIN_POWERUPS_USED:
        AddStat(STATS_CAPTAIN_POWERUPS_USED, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_CAPTAIN_POWERUPS_USED, homeaway, playerindex, 1);
        break;
    case STATS_POWERUPS_HIT:
        AddStat(STATS_POWERUPS_HIT, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_POWERUPS_HIT, param0, 1);
        break;
    case STATS_PASSES_MADE:
        AddStat(STATS_PASSES_MADE, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_PASSES_MADE, param0, 1);
        break;
    case STATS_NON_VOLLEY_PASSES:
        AddStat(STATS_NON_VOLLEY_PASSES, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_NON_VOLLEY_PASSES, param0, 1);
        break;
    case STATS_VOLLEY_PASSES:
        AddStat(STATS_VOLLEY_PASSES, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_VOLLEY_PASSES, param0, 1);
        break;
    case STATS_PASSES_RECEIVED:
        AddStat(STATS_PASSES_RECEIVED, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_PASSES_RECEIVED, homeaway, playerindex, 1);
        break;
    case STATS_PASSES_INTERCEPTED:
        AddStat(STATS_PASSES_INTERCEPTED, homeaway, playerindex, 1);
        break;
    case STATS_16:
        AddStat(STATS_16, homeaway, playerindex, param0);
        AddUserStatByPlayer(STATS_16, homeaway, playerindex, param0);
        break;
    case STATS_ATTACK_SUCCESSES:
        AddStat(STATS_ATTACK_SUCCESSES, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_ATTACK_SUCCESSES, param0, 1);
        break;
    case STATS_ATTACK_ATTEMPTS:
        AddStat(STATS_ATTACK_ATTEMPTS, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_ATTACK_ATTEMPTS, param0, 1);
        break;
    case STATS_BUTTON_PRESSES:
        AddStat(STATS_BUTTON_PRESSES, homeaway, playerindex, param1);
        AddUserStatByPad(STATS_BUTTON_PRESSES, param0, param1);
        break;
    case STATS_HITS_MADE:
        AddStat(STATS_HITS_MADE, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_HITS_MADE, homeaway, playerindex, 1);
        break;
    case STATS_25:
        AddStat(STATS_25, homeaway, playerindex, 1);
        AddUserStatByPlayer(STATS_25, homeaway, playerindex, 1);
        break;
    case STATS_PERFECT_PASSES:
        AddStat(STATS_PERFECT_PASSES, homeaway, playerindex, 1);
        AddUserStatByPad(STATS_PERFECT_PASSES, param0, 1);
        break;
    case STATS_STS_ATTEMPTS:
        AddStat(STATS_STS_ATTEMPTS, homeaway, playerindex, param0);
        AddUserStatByPlayer(STATS_STS_ATTEMPTS, homeaway, playerindex, param0);
        break;
    case STATS_MEGASTRIKE_ATTEMPTS:
        AddStat(STATS_MEGASTRIKE_ATTEMPTS, homeaway, playerindex, param0);
        AddUserStatByPlayer(STATS_MEGASTRIKE_ATTEMPTS, homeaway, playerindex, param0);
        break;
    case STATS_MEGASTRIKE_GOALS:
        AddStat(STATS_MEGASTRIKE_GOALS, homeaway, playerindex, param0);
        AddUserStatByPlayer(STATS_MEGASTRIKE_GOALS, homeaway, playerindex, param0);
        break;
    default:
        break;
    }
}

static inline int CompareInt(eSortOrder sortOrder, int a, int b)
{
    if (a < b)
        return sortOrder == SORT_DESCENDING ? 1 : -1;
    if (a > b)
        return sortOrder == SORT_DESCENDING ? -1 : 1;
    return 0;
}

void StatsTracker::GetSortedStats(PlayerStats* source, int numsource,
    int* dest, int numelements, ePlayerStats statType,
    eSortOrder sortOrder)
{
    int tempsorted[64];

    for (int i = 0; i < numsource; i++)
    {
        tempsorted[i] = i;
    }

    unsigned char swapped;
    do
    {
        swapped = 0;
        for (int i = 0; i < numsource; i++)
        {
            bool doswap;

            if (i + 1 >= numsource)
                break;

            int nexti = i + 1;

            doswap = false;
            switch (statType)
            {
            case STATS_SHOTS_ON_GOAL:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumShotsOnGoal,
                    source[tempsorted[nexti]].mNumShotsOnGoal) == 1;
                break;
            case STATS_GOALS_FOR:
            {
                unsigned int human = CupManager::Instance()->mCurrentCup->IsHumanTeam(
                    source[tempsorted[nexti]].mRecordType.mTeamID);
                int comparison = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumGoalsFor,
                    source[tempsorted[nexti]].mNumGoalsFor);
                if (comparison == 1)
                    doswap = true;
                else if (comparison == 0)
                {
                    if (human)
                        doswap = true;
                    else
                        doswap = false;
                }
                else
                    doswap = false;
                break;
            }
            case STATS_GOALS_AGAINST:
            {
                unsigned int human = CupManager::Instance()->mCurrentCup->IsHumanTeam(
                    source[tempsorted[nexti]].mRecordType.mTeamID);
                int comparison = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumGoalsAgainst,
                    source[tempsorted[nexti]].mNumGoalsAgainst);
                if (comparison == 1)
                    doswap = true;
                else if (comparison == 0)
                {
                    if (human)
                        doswap = true;
                    else
                        doswap = false;
                }
                else
                    doswap = false;
                break;
            }
            case STATS_ASSISTS:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumAssists,
                    source[tempsorted[nexti]].mNumAssists) == 1;
                break;
            case STATS_FOULS:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumFouls,
                    source[tempsorted[nexti]].mNumFouls) == 1;
                break;
            case STATS_POWERUPS_USED:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumPowerupsUsed,
                    source[tempsorted[nexti]].mNumPowerupsUsed) == 1;
                break;
            case STATS_POWERUPS_HIT:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumPowerupsHit,
                    source[tempsorted[nexti]].mNumPowerupsHit) == 1;
                break;
            case STATS_26:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].unknown_0x48,
                    source[tempsorted[nexti]].unknown_0x48) == 1;
                break;
            case STATS_PASSES_MADE:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumPassesMade,
                    source[tempsorted[nexti]].mNumPassesMade) == 1;
                break;
            case STATS_PASSES_RECEIVED:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumPassesMade,
                    source[tempsorted[nexti]].mNumPassesMade) == 1;
                break;
            case STATS_PASSES_INTERCEPTED:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumPassesIntercepted,
                    source[tempsorted[nexti]].mNumPassesIntercepted) == 1;
                break;
            case STATS_16:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mBallPossessionTime,
                    source[tempsorted[nexti]].mBallPossessionTime) == 1;
                break;
            case STATS_GAMES_PLAYED:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumGamesPlayed,
                    source[tempsorted[nexti]].mNumGamesPlayed) == 1;
                break;
            case STATS_ATTACK_SUCCESSES:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumSteals,
                    source[tempsorted[nexti]].mNumSteals) == 1;
                break;
            case STATS_BUTTON_PRESSES:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumButtonPresses,
                    source[tempsorted[nexti]].mNumButtonPresses) == 1;
                break;
            case STATS_HITS_MADE:
                doswap = CompareInt(sortOrder,
                    source[tempsorted[i]].mNumHitsMade,
                    source[tempsorted[nexti]].mNumHitsMade) == 1;
                break;
            }

            if (doswap)
            {
                int temp = tempsorted[i + 1];
                tempsorted[i + 1] = tempsorted[i];
                tempsorted[i] = temp;
                swapped = 1;
            }
        }
    } while (swapped);

    for (int i = 0; i < numelements; i++)
    {
        dest[i] = tempsorted[i];
    }
}

void StatsTracker::GetSortedTeamStats(
    TeamStats* source, int numsource, int* dest, int numelements)
{
    int tempsorted[10];

    for (int i = 0; i < numsource; i++)
    {
        tempsorted[i] = i;
    }

    unsigned char swapped;
    do
    {
        swapped = 0;
        for (int i = 0; i < numsource; i++)
        {
            if (i + 1 >= numsource)
                break;

            int a = source[tempsorted[i]].mNumPoints;
            int b = source[tempsorted[i + 1]].mNumPoints;
            int aGoalDiff = source[tempsorted[i]].mPlayerTotalStats.mNumGoalsFor
                - source[tempsorted[i]].mPlayerTotalStats.mNumGoalsAgainst;
            int bGoalDiff = source[tempsorted[i + 1]].mPlayerTotalStats.mNumGoalsFor
                - source[tempsorted[i + 1]].mPlayerTotalStats.mNumGoalsAgainst;
            unsigned int humanA = CupManager::Instance()->mCurrentCup->IsHumanTeam(
                source[tempsorted[i]].mTeamIndex);
            unsigned int humanB = CupManager::Instance()->mCurrentCup->IsHumanTeam(
                source[tempsorted[i + 1]].mTeamIndex);

            if (a < b
                || (a == b && aGoalDiff < bGoalDiff)
                || (a == b && a != 0 && !humanA && humanB && aGoalDiff == bGoalDiff)
                || (a == b && a == 0 && humanA && !humanB && aGoalDiff == bGoalDiff))
            {
                int temp = tempsorted[i + 1];
                tempsorted[i + 1] = tempsorted[i];
                tempsorted[i] = temp;
                swapped = 1;
            }
            else if (a == b && humanA && humanB)
            {
                if (MoveTeamBUp(source[tempsorted[i]], source[tempsorted[i + 1]]))
                {
                    int temp = tempsorted[i + 1];
                    tempsorted[i + 1] = tempsorted[i];
                    tempsorted[i] = temp;
                    swapped = 1;
                }
            }
        }
    } while (swapped);

    for (int i = 0; i < numelements; i++)
    {
        dest[i] = tempsorted[i];
    }
}

static inline void AccumulateUserStats(PlayerStats* total, const PlayerStats& current)
{
    total->mNumLowChargeShots += current.mNumLowChargeShots;
    total->mNumMediumChargeShots += current.mNumMediumChargeShots;
    total->mNumHighChargeShots += current.mNumHighChargeShots;
    total->mNumShotsOnGoal += current.mNumShotsOnGoal;
    total->mNumRegularGoals += current.mNumRegularGoals;
    total->mNumOneTimerGoals += current.mNumOneTimerGoals;
    total->mNumSTSGoals += current.mNumSTSGoals;
    total->mNumAssists += current.mNumAssists;
    total->mNumGoalsFor += current.mNumGoalsFor;
    total->mNumGoalsAgainst = current.mNumGoalsAgainst;
    total->mNumSTSAttempts += current.mNumSTSAttempts;
    total->mNumMegaStrikeAttempts += current.mNumMegaStrikeAttempts;
    total->mNumMegaStrikeGoals += current.mNumMegaStrikeGoals;
    total->mNumFouls += current.mNumFouls;
    total->mNumGamesPlayed = current.mNumGamesPlayed;
    total->mNumPowerupsUsed += current.mNumPowerupsUsed;
    total->mNumMushroomsUsed += current.mNumMushroomsUsed;
    total->mNumDrawablePowerupsUsed += current.mNumDrawablePowerupsUsed;
    total->mNumStarsAndChainChompsUsed += current.mNumStarsAndChainChompsUsed;
    total->mNumCaptainPowerupsUsed += current.mNumCaptainPowerupsUsed;
    total->mNumPassesMade += current.mNumPassesMade;
    total->mNumNonVolleyPasses += current.mNumNonVolleyPasses;
    total->mNumVolleyPasses += current.mNumVolleyPasses;
    total->mNumPassesReceived += current.mNumPassesReceived;
    total->mNumHitsMade += current.mNumHitsMade;
    total->mNumAttackAttempts += current.mNumAttackAttempts;
    total->mNumSteals += current.mNumSteals;
    total->unknown_0x38 += current.unknown_0x38;
    total->mBallPossessionTime += current.mBallPossessionTime;
    total->mNumButtonPresses += current.mNumButtonPresses;
    total->mNumPerfectPasses += current.mNumPerfectPasses;
    total->unknown_0x46 += current.unknown_0x46;
    total->unknown_0x48 += current.unknown_0x48;
    total->mNumPowerupsHit += current.mNumPowerupsHit;
    total->mNumPassesIntercepted += current.mNumPassesIntercepted;
}

void StatsTracker::CompileEndOfGameStats()
{
    if (GameInfoManager::Instance()->IsInMode3())
    {
        int homeAwayIndex[2] = {-1, -1};
        CupManager* cup = CupManager::Instance();
        eTeamID homeid = (eTeamID)mBasicGameInfo->mTeamIndex[0];
        eTeamID awayid = (eTeamID)mBasicGameInfo->mTeamIndex[1];
        int numTeams = cup->GetNumPlayingTeams();
        int previousTeam = cup->GetFinalOpponentTeam();
        if (cup->GetCurrentRoundType() == 2)
        {
            if (homeid == previousTeam)
                homeAwayIndex[0] = numTeams;
            else if (awayid == previousTeam)
                homeAwayIndex[1] = numTeams;
        }
        for (int i = 0; i < numTeams; i++)
        {
            if (homeid == cup->GetTeamStatsByIndex(i).mTeamIndex)
                homeAwayIndex[0] = i;
            if (awayid == cup->GetTeamStatsByIndex(i).mTeamIndex)
                homeAwayIndex[1] = i;
        }
        int tempStat;
        for (int homeaway = 0; homeaway < 2; homeaway++)
        {
            int team = homeAwayIndex[homeaway];
            bool otherSide;
            if (homeaway == 0)
                otherSide = true;
            else
                otherSide = false;
            TeamStats* cumulative = cup->pGetTeamStatsByIndex((u16)team);
            if (mBasicGameInfo->mFinalScore[(short)homeaway] != -5)
            {
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumShotsOnGoal;
                cumulative->mPlayerTotalStats.mNumShotsOnGoal += tempStat;
                if (cup->GetCurrentRoundType() == 0)
                {
                    tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumGoalsFor;
                    cumulative->mPlayerTotalStats.mNumGoalsFor += tempStat;
                    if (cumulative->mPlayerTotalStats.mNumGoalsFor > 999)
                        cumulative->mPlayerTotalStats.mNumGoalsFor = 999;
                    tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumGoalsAgainst;
                    cumulative->mPlayerTotalStats.mNumGoalsAgainst += tempStat;
                    if (cumulative->mPlayerTotalStats.mNumGoalsAgainst > 999)
                        cumulative->mPlayerTotalStats.mNumGoalsAgainst = 999;
                }
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumAssists;
                cumulative->mPlayerTotalStats.mNumAssists += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumFouls;
                cumulative->mPlayerTotalStats.mNumFouls += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPowerupsUsed;
                cumulative->mPlayerTotalStats.mNumPowerupsUsed += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPowerupsHit;
                cumulative->mPlayerTotalStats.mNumPowerupsHit += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.unknown_0x48;
                cumulative->mPlayerTotalStats.unknown_0x48 += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPassesMade;
                cumulative->mPlayerTotalStats.mNumPassesMade += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPassesReceived;
                cumulative->mPlayerTotalStats.mNumPassesReceived += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPassesIntercepted;
                cumulative->mPlayerTotalStats.mNumPassesIntercepted += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumHitsMade;
                cumulative->mPlayerTotalStats.mNumHitsMade += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumSteals;
                cumulative->mPlayerTotalStats.mNumSteals += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumOneTimerGoals;
                cumulative->mPlayerTotalStats.mNumOneTimerGoals += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mBallPossessionTime;
                cumulative->mPlayerTotalStats.mBallPossessionTime += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumButtonPresses;
                cumulative->mPlayerTotalStats.mNumButtonPresses += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.unknown_0x46;
                cumulative->mPlayerTotalStats.unknown_0x46 += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPerfectPasses;
                cumulative->mPlayerTotalStats.mNumPerfectPasses += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumSTSAttempts;
                cumulative->mPlayerTotalStats.mNumSTSAttempts += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumMegaStrikeAttempts;
                cumulative->mPlayerTotalStats.mNumMegaStrikeAttempts += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumMegaStrikeGoals;
                cumulative->mPlayerTotalStats.mNumMegaStrikeGoals += tempStat;
                cumulative->mPlayerTotalStats.mNumGamesPlayed++;
                if (GameInfoManager::Instance()->IsInMode3())
                {
                    if (cumulative->mTeamIndex == cup->GetUserSelectedCupTeam())
                    {
                        // Milestone accumulation was removed; the cup-team check remains.
                    }
                }
            }
            else
            {
                mBasicGameInfo->mFinalScore[(short)homeaway] = 0;
                if (cup->GetCurrentRoundType() == 0)
                {
                    u16 otherGoals = mCumulativeTeamStats[otherSide]->mPlayerTotalStats.mNumGoalsFor;
                    mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumGoalsAgainst = otherGoals;
                    cumulative->mPlayerTotalStats.mNumGoalsAgainst += otherGoals;
                }
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumShotsOnGoal = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumGoalsFor = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumAssists = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumFouls = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPowerupsUsed = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPowerupsHit = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.unknown_0x48 = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPassesMade = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPassesReceived = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPassesIntercepted = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumHitsMade = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumSteals = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumOneTimerGoals = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumButtonPresses = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.unknown_0x46 = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumPerfectPasses = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumSTSAttempts = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumMegaStrikeAttempts = 0;
                mCumulativeTeamStats[homeaway]->mPlayerTotalStats.mNumMegaStrikeGoals = 0;
            }
            if (cup->GetCurrentRoundType() == 0)
            {
                tempStat = mCumulativeTeamStats[homeaway]->mNumWins;
                cumulative->mNumWins += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mNumLosses;
                cumulative->mNumLosses += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mNumOTLosses;
                cumulative->mNumOTLosses += tempStat;
                tempStat = mCumulativeTeamStats[homeaway]->mNumPoints;
                cumulative->mNumPoints += tempStat;
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        AccumulateUserStats(&GameInfoManager::Instance()->unknown_0x128[i].mStats, mCurrentUserStats[i]);
    }
}

void StatsTracker::SimulateGame()
{
    m_pSimulator->SimulateGame();
}

void StatsTracker::AddStat(
    ePlayerStats stat, int team, int player, int value)
{
    switch (stat)
    {
    case STATS_GOALS_AGAINST:
    {
        int start = (player == -1) ? 0 : player;
        int end = (player == -1) ? 5 : start + 1;
        for (int i = start; i < end; i++)
        {
            AddStatValue(mCurrentPlayerStats[team][i], stat, value);
        }
        break;
    }
    case STATS_WIN:
    case STATS_OT_WIN:
    case STATS_LOSS:
    case STATS_OT_LOSS:
        break;
    default:
        AddStatValue(mCurrentPlayerStats[team][player], stat, value);
        break;
    }

    switch (stat)
    {
    case STATS_WIN:
        mCumulativeTeamStats[team]->mNumWins++;
        mCumulativeTeamStats[team]->mNumPoints += 3;
        break;
    case STATS_OT_WIN:
        mCumulativeTeamStats[team]->mNumWins++;
        mCumulativeTeamStats[team]->mNumPoints += 3;
        break;
    case STATS_LOSS:
        mCumulativeTeamStats[team]->mNumLosses++;
        break;
    case STATS_OT_LOSS:
        mCumulativeTeamStats[team]->mNumOTLosses++;
        mCumulativeTeamStats[team]->mNumPoints++;
        break;
    default:
        AddStatValue(mCumulativeTeamStats[team]->mPlayerTotalStats, stat, value);
        break;
    }

    switch (stat)
    {
    case STATS_WIN:
        mCurrentTeamStats[team].mNumWins++;
        mCurrentTeamStats[team].mNumPoints += 3;
        break;
    case STATS_OT_WIN:
        mCurrentTeamStats[team].mNumWins++;
        mCurrentTeamStats[team].mNumPoints += 3;
        break;
    case STATS_LOSS:
        mCurrentTeamStats[team].mNumLosses++;
        break;
    case STATS_OT_LOSS:
        mCurrentTeamStats[team].mNumPoints++;
        break;
    default:
        AddStatValue(mCurrentTeamStats[team].mPlayerTotalStats, stat, value);
        break;
    }
}

void StatsTracker::AddUserStatByPlayer(ePlayerStats stat, int team, int player, int amount)
{
    int pad;
    unsigned char hasPad = GetGlobalPadID(team, player, pad);
    if (hasPad)
        AddUserStatByPad(stat, pad, amount);
}

void StatsTracker::AddUserStatByPad(ePlayerStats stat, int pad, int amount)
{
    if (pad < 0)
        return;
    AddStatValue(mCurrentUserStats[pad], stat, amount);
    AddStatValue(mCumulativeUserStats[pad], stat, amount);
}

void StatsTracker::TrackWinner(int forfeitSide)
{
    int homeScore = 0;
    int awayScore = 0;
    unsigned char wasForfeit = 0;
    long winningSide;

    if (g_pTeams[0] != 0 && g_pTeams[1] != 0)
    {
        homeScore = g_pTeams[0]->m_nScore;
        awayScore = g_pTeams[1]->m_nScore;
    }

    if (forfeitSide == 0)
    {
        homeScore = -5;
        if (awayScore < 7)
        {
            s_pInstance->TrackStat(
                STATS_GOALS_FOR, 1, 0, 0, 0, 7 - awayScore, 0);
            awayScore = 7;
        }
        wasForfeit = 1;
    }
    else if (forfeitSide == 1)
    {
        awayScore = -5;
        if (homeScore < 7)
        {
            s_pInstance->TrackStat(
                STATS_GOALS_FOR, 0, 0, 0, 0, 7 - homeScore, 0);
            homeScore = 7;
        }
        wasForfeit = 1;
    }

    winningSide = awayScore >= homeScore;

    if (!mHasGameEnded)
    {
        if (GameInfoManager::Instance()->IsInOddCupMode())
        {
            if (mIsOvertime && !wasForfeit)
            {
                s_pInstance->TrackStat(STATS_OT_WIN, winningSide, 0,
                    homeScore, awayScore, 0, 0);
                if (GameInfoManager::Instance()->IsInMode3())
                {
                    CupManager::s_pInstance->SetRoundResult(true, winningSide);
                }
            }
            else
            {
                s_pInstance->TrackStat(STATS_WIN, winningSide, 0,
                    homeScore, awayScore, 0, 0);
                if (GameInfoManager::Instance()->IsInMode3())
                {
                    CupManager::s_pInstance->SetRoundResult(false, winningSide);
                }
            }

            if (GameInfoManager::Instance()->IsInMode3())
            {
                s_pInstance->CompileEndOfGameStats();
            }
        }
        else
        {
            mBasicGameInfo->mFinalScore[0] = homeScore;
            mBasicGameInfo->mFinalScore[1] = awayScore;
            s_pInstance->mNumGamesWon[winningSide]++;
        }
        mHasGameEnded = true;
    }
}

static int CountNewlines(FILE* file)
{
    fseek(file, 0, 0);
    int count = 0;
    char character;
    while ((character = fgetc(file)) != -1)
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
    int gameID = 0;
    unsigned char firstTime = 1;

    if (gameDuration <= 0.0f)
    {
        gameDuration =
            (float)GameInfoManager::Instance()->GetCurrentSettings()->GameTime;
    }
    if (filename == 0)
    {
        filename = STATS_FILE;
    }

    FILE* pFile = fopen(filename, "r");
    if (pFile != 0)
    {
        firstTime = 0;
        gameID = (int)((CountNewlines(pFile) - 1) * 0.5f);
        fclose(pFile);
    }

    pFile = fopen(filename, firstTime ? "wt" : "at");
    if (pFile == 0)
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
        header.AppendInPlace("Difficulty,");
        header[header.size() - 1] = '\n';
        size_t bytesToWrite = header.size();
        fwrite(header.c_str(), 1, bytesToWrite, pFile);
    }

    int numHumans[2] = { 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        if (GameInfoManager::Instance()->GetPlayingSide((u16)i) == 0)
        {
            numHumans[0]++;
        }
        else if (GameInfoManager::Instance()->GetPlayingSide((u16)i) == 1)
        {
            numHumans[1]++;
        }
    }

    NLString stats;
    for (int team = 0; team < 2; team++)
    {
        stats = Format(NLString("{0},{1},{2},{3},{4},{5},"),
            gameID, team, GameInfoManager::Instance()->GetStadium(),
            gameDuration, numHumans[team],
            GameInfoManager::Instance()->GetTeam((short)team));

        stats = stats.Append(Format(NLString("{0},{1},{2},{3},"),
            g_pTeams[team]->GetAverageMovementRating(),
            g_pTeams[team]->GetAverageShootingRating(),
            g_pTeams[team]->GetAveragePassingRating(),
            g_pTeams[team]->GetAverageDefenseRating()));

        int possession = GetStatValue(
            mCumulativeTeamStats[team]->mPlayerTotalStats, STATS_16);
        possession = (int)(possession / gameTime);
        int difficulty = GameInfoManager::Instance()->GetDifficulty((short)team);
        stats.AppendInPlace(Format(NLString("{0},"), difficulty));
        stats[stats.size() - 1] = '\n';
        size_t bytesToWrite = stats.size();
        fwrite(stats.c_str(), 1, bytesToWrite, pFile);
    }

    fclose(pFile);
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

void StatsTracker::WriteCurrentlyPlaying() const
{
    FILE* file = fopen("currently_playing.txt", "wt");
    if (file == 0)
    {
        return;
    }

    NLString text = Format(
        NLString("Home: {0} with {1}\nAway: {2} with {3}\nStadium: {4}\n"),
        GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(0)),
        GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 0)),
        GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(1)),
        GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0)),
        GetStadiumName(GameInfoManager::Instance()->GetStadium()));

    fwrite(text.c_str(), 1, text.size(), file);
    fclose(file);
}
