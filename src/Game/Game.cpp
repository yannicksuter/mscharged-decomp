#include "Game/NetworkMessageRegistry.h"
#include "Game/DetInput.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/Weather.h"
#include "Game/Sys/debug.h"
#include "Game/NetworkDiagnostics.h"

#include "Game/Task/GameRenderTask.h"

#include "Game/AI/FilteredRandom.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/AISandbox.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/Scripts/ScriptCaching.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/Ball.h"
#include "Game/BasicStadium.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/OverlayManager.h"
#include "Game/OverlayHandlerHUD.h"
#include "Game/Camera/tu_800F9460.h"
#include "Game/DebugWriteCache.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Formation.h"
#include "Game/GameInfo.h"
#include "Game/Audio/GameStreams.h"
#include "Game/Audio/AudioResourceRuntime.h"
#include "Game/CharacterTemplate.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/GameProgress.inl"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/NetworkSession.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Player.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/PeachPhoto.h"
#include "Game/Render/ElectricFence.h"
#include "Game/ReplayChoreo.h"
#include "Game/ReplayManager.h"
#include "Game/Render/Presentation.h"
#include "Game/NisPlayer.h"
#include "Game/Render/MegaBallIndicators.h"
#include "Game/NetTournManager.h"
#include "Game/Render/NetMesh.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/Scripts/ScriptDefines.h"
#include "Game/Physics/PhysicsShockwave.h"
#include "Game/InputManager.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/GameplayCam.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/clock.h"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/Team.h"
#include "Game/Terrain.h"
#include "Game/CrowdRiot.h"
#include "Game/ScriptTuning.h"
#include "Game/GameTweaks.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakValueInt.h"
#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkEvents.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBindMember.inl"
#include "NL/nlFunction.inl"
#include "NL/nlConfig.h"
#include "NL/nlMain.h"
#include "NL/nlMath.h"
#include "NL/nlMath.inl"
#include "NL/nlMemory.h"
#include "NL/nlPolygonRegion.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include <stdlib.h>
#include "Game/Render/NumberDisplay.h"
#include "Game/InputRouter.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"
#include "Game/DB/StadiumInfo.h"

extern "C" void fn_80015B38(cBall* pBall, bool bParam);
extern "C" void fn_800406B0(cFielder* pFielder, float numBalls, float accuracy);

struct PlayerDistanceSnapshot
{
    inline void RegisterDebugFields(DebugWriteCache* cache);
    u8 mPlayerIndices[100];
    float mDistances[100];
    float mBallDistances[10];
};

EventDispatcher* GetGameEventDispatcher();
void FreeLightningStrikeData(LightningStrikeData* node);
void FreeShotAtGoalData(ShotAtGoalData* node);
void FreeNISData(NISData* node);
void FreeCollisionCrowdData(CollisionCrowdData* node);
void FreePlayerAttackData(PlayerAttackData* node);
void PostResetCallback(unsigned long param1, unsigned long param2);

// Tweak "Megastrike/Score differential": when positive, it overrides the
// mega strike shot count (SetMegaStrikeGameplay).
TweakValueInt gMegaStrikeShotCountOverride("Score differential", "Megastrike", -1, false);

// Game tuning values (.sdata), in retail order.
// Countdown beeps played before the end of a match.
int gEndGameCountdownBeeps = 5;
// Field tilt force scale (SetFieldTilt).
float gFieldTiltForceScale = 0.8f;
// Time the field takes to level out, and the hold before it starts.
float gFieldLevelTime = 3.0f;
float gFieldTiltHoldTime = 3.0f;
// Weather tilt range scale on each axis.
float gWeatherTiltXRangeScale = 0.5f;
float gWeatherTiltYRangeScale = 8.0f;
float gFieldTiltXLimit = 4.0f;
float gFieldTiltYLimit = 10.0f;
// Weather tilt ramp-up period.
float gWeatherTiltRampDuration = 12.0f;
// Score-difference tilt: clamp and scale.
float gScoreTiltMaxDifference = 6.0f;
float gScoreTiltScale = 1.0f;
// Weather tilt target speed range.
float gWeatherTiltInitialSpeed = 12.0f;
float gWeatherTiltFinalSpeed = 1.0f;
// Weather tilt direction jitter.
int gWeatherTiltDirectionJitter = 1000;
// Seek speeds: weather tilt, levelling during shoot-to-score, score tilt.
float gWeatherTiltSpeedSeekRate = 1.0f;
float gMegaStrikeTiltLevelRate = 2.0f;
float gScoreTiltSeekRate = 2.0f;
int gMegaStrikeMeterBatchSize = 3;


// .sbss, in retail order. The first two are debug overrides that force the
// weather and score field tilt on.
bool gForceWeatherTilt;
bool gForceScoreTilt;
cGame* g_pGame;
bool gNoGameClock;

static inline int GetPlayerIndex(cPlayer* pPlayer)
{
    return pPlayer->mUnidentified120;
}

inline void cGame::ResetGameFields()
{
    float zero = 0.0f;
    m_bBallInNet = false;
    m_nLastTeamToScore = 1;
    m_uMegastrikeNumShots = 0;
    m_uMegastrikeCurShot = 0;
    m_uMegastrikeGoals = 0;
    m_nMegastrikeDefendingTeam = 0;
    m_uMegastrikeResults = 0;
    mpMegaStrikeShooter = 0;
    mbCaptainShotToScoreOn = false;
    mbMegaStrikePositiveNet = false;
    mbMegaStrikePlayerReady = false;
    m_pScorer = 0;
    m_pAssister = 0;
    m_pTeamTouch[1] = 0;
    m_pTeamTouch[0] = 0;
    for (int i = 0; i < 10; i++)
    {
        m_pRandomPlayersArray[i] = 0;
    }
    mfTiltLevelTimer = zero;
    mfXTilt = zero;
    mfYTilt = zero;
    mfXTiltMin = -1.0f;
    mfXTiltMax = 1.0f;
    mfYTiltMin = -1.0f;
    mfYTiltMax = 1.0f;
    mfTiltSpeed = zero;
    mfDesiredTiltSpeed = zero;
    mUnidentified0A0 = zero;
    maTiltDir = 0;
    maDesiredTiltDir = 0;
    muTiltFrames = 0;
    float initialTilt = -zero;
    fn_8005B330(&mTiltDirection, initialTilt, initialTilt);
    muCountdownBeepsRemaining = gEndGameCountdownBeeps;
    mbMegaStrikePlayerReadySent = false;
    mbMegaStrikeCleanupPending = false;
}

inline void cGame::ResetCharacters()
{
    RandomizePlayerUpdateOrder();
    for (int i = 0; i < 2; i++)
    {
        g_pTeams[i]->fn_800A6248();
        g_pTeams[i]->ResetCharacters();
    }
}

static inline void InitializeChallengeTeamScore(cTeam* team, int side)
{
    team->m_nScore = 0;
    int score = g_pStrikerChallenge->GetScore(side);
    score += team->m_nScore;
    team->m_nScore = score;
}

static inline void DeliverGoalScored(cGame* game, GoalScoredData* data)
{
    if (game->GetGameState() != 4)
    {
        game->mUnidentified49C.mGoalScoredEvent.Deliver(data);
    }
}

static inline int GetTeamScoreDifference(cTeam* team)
{
    return team->GetScore() - team->GetOtherTeam()->GetScore();
}

inline void cGame::UpdatePowerUpObjects(float fDeltaT)
{
    for (int i = 0; i < 25; i++)
    {
        if (g_pPowerups[i] != 0)
        {
            g_pPowerups[i]->Update(fDeltaT);
        }
    }
}

static inline int GetSyncPlayerIndex(cPlayer* pPlayer)
{
    return pPlayer == 0 ? -1 : GetPlayerIndex(pPlayer);
}

// Walks the players in their current randomized update order.
class RandomPlayerIterator
{
public:
    RandomPlayerIterator(cGame* game)
        : mGame(game)
        , mIndex(0)
    {
    }
    bool HasNext() const { return mIndex < 10; }
    cPlayer* const& GetPlayer() const { return mGame->m_pRandomPlayersArray[mIndex]; }
    int GetIndex() const { return mIndex; }
    void Next() { ++mIndex; }

private:
    cGame* mGame;
    int mIndex;
};

float GetGameSimulationMilliseconds()
{
    return 1000.0f * GetFixedUpdateTask()->mSimulationTime;
}
float GetGameTickerMilliseconds()
{
    return nlTicksToMilliseconds(nlGetTicker());
}
void fn_80056CF4(void* terrainIndex, int weatherType, bool startCrowdRiot)
{
    ++lbl_806E2130;

    cGame* game = new (nlMalloc(0x10F4, 8, false)) cGame(terrainIndex, weatherType, startCrowdRiot);
    g_pGame = game;

    cTeam* team = new (8, false) cTeam(0);
    g_pTeams[0] = team;

    team = new (8, false) cTeam(1);
    g_pTeams[1] = team;

    cField::Init(g_pTeams[0]->m_pNet, g_pTeams[1]->m_pNet);

    if (AISandbox::s_pInstance == 0)
    {
        AISandbox::s_pInstance = new (8, false) AISandbox();
    }
    if (lbl_806E12C8 == 0)
    {
        PhysicsPatchManager* memory
            = new (nlMalloc(sizeof(PhysicsPatchManager), 8, false))
                PhysicsPatchManager();
        lbl_806E12C8 = memory;
    }
    if (UnidentifiedCameraEffects::Instance() == 0)
    {
        UnidentifiedCameraEffects* memory = new (nlMalloc(
            sizeof(UnidentifiedCameraEffects), 8, false))
            UnidentifiedCameraEffects;
        UnidentifiedCameraEffects::s_pInstance = memory;
    }
    if (gpNumberDisplay == 0)
    {
        NumberDisplay* numberDisplay
            = static_cast<NumberDisplay*>(
                nlMalloc(0x28, 8, false));
        numberDisplay
            = new (numberDisplay) NumberDisplay();
        gpNumberDisplay = numberDisplay;
    }

    FormationManager::LoadFormationSets();
    --lbl_806E2130;
    SetRenderWorldEffects(true);
    WorldDarkening::Instance().fn_801AF550();
}
void fn_80056EA8()
{
    g_pGame->ChangeGameState(4);
}
void DestroyGame()
{
    bool bWriteStats = GetConfigBool(Config::Global(), "save_stats", false);
    if (bWriteStats)
    {
        StatsTracker::Instance()->WriteStats(
            g_pGame->m_fGameDuration, -1.0f, 0);
    }

    if (AISandbox::s_pInstance != 0)
    {
        delete AISandbox::s_pInstance;
        AISandbox::s_pInstance = 0;
    }
    if (lbl_806E12C8 != 0)
    {
        delete lbl_806E12C8;
        lbl_806E12C8 = 0;
    }
    if (UnidentifiedCameraEffects::Instance() != 0)
    {
        delete UnidentifiedCameraEffects::Instance();
        UnidentifiedCameraEffects::s_pInstance = 0;
    }
    if (gpNumberDisplay != 0)
    {
        delete gpNumberDisplay;
        gpNumberDisplay = 0;
    }

    delete g_pTeams[0];
    delete g_pTeams[1];
    g_pTeams[0] = 0;
    g_pTeams[1] = 0;

    delete g_pGame;
    g_pGame = 0;

    FormationManager::UnloadFormationSets();
    SetRenderWorldEffects(true);
}
void DestroyPowerups()
{
    g_pGame->ResetPowerups(false);
    CompactPowerups();
}
static inline const char* SuddenDeathEventName();
static inline const char* GameOverEventName();

cGame::cGame(void* terrainIndex, int weatherType, bool startCrowdRiot)
    : mReceivedMegaStrikeMeter((bool*)mReceivedMegaStrikeMeterStorage, 100)
    , mPendingMegaStrikeMeter((bool*)mPendingMegaStrikeMeterStorage, 16)
{
    mpTerrain = 0;
    mpWeatherManager = 0;
    mpCrowdRiot = 0;
    m_eGameState = -1;

    m_pPostResetClock = new (nlMalloc(sizeof(Clock), 8, false))
        Clock(0.0f, 0.5f, 1.0f, 2, PostResetCallback);
    m_pPostResetClock->m_uParam1 = (unsigned long)this;

    mpTerrain = new (nlMalloc(sizeof(Terrain), 8, false))
        Terrain((int)terrainIndex);

    mpWeatherManager = new (nlMalloc(sizeof(WeatherManager), 8, false)) WeatherManager();

    mpWeatherManager->Initialize(weatherType);

    mpCrowdRiot = new (nlMalloc(sizeof(CrowdRiot), 8, false))
        CrowdRiot(startCrowdRiot);

    m_pFuzzyTweaks = new (nlMalloc(sizeof(FuzzyTweaks), 8, false))
        FuzzyTweaks("/ini/FuzzyTweaks.ini", "/Game/Fuzzy");
    gGameTweaks.m_pGameTweaks->fn_800756B4();

    ResetGameFields();
    mReceivedMegaStrikeMeter.mHead = 0;
    mReceivedMegaStrikeMeter.mCount = 0;
    mPendingMegaStrikeMeter.mHead = 0;
    mPendingMegaStrikeMeter.mCount = 0;

    m_fGameDuration = gGameTweaks.m_pGameTweaks->fGameDuration;
    m_pGameClock = new (nlMalloc(sizeof(Clock), 8, false))
        Clock(0.0f, 999999.0f, 1.0f, 2, 0);
    m_pGameClock->Stop();
    m_pPostGameDoneClock = new (nlMalloc(sizeof(Clock), 8, false))
        Clock(0.0f, 1.5f, 1.0f, 2, 0);

    bool noClock = GetTweakBool("user/No Clock", false);
    gNoGameClock = noClock;
    cGame* game = g_pGame;
    if (game != 0 && game->m_pGameClock != 0)
    {
        if (noClock)
        {
            if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
                game->m_pGameClock->Stop();
        }
        else
        {
            if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
                game->m_pGameClock->Start();
        }
    }

    if (GetConfigBool(Config::Global(), "save_stats", false))
    {
        StatsTracker::Instance()->WriteCurrentlyPlaying();
    }

    RegisterEventListeners();

    mpAIContext = new (nlMalloc(sizeof(AIContext), 8, false))
        AIContext(
            this, 0, new (nlMalloc(sizeof(FuzzyAIRuntime), 8, false))
                         FuzzyAIRuntime());
    gNetworkMessageRegistry->RegisterReceiver(34, this);
    gNetworkMessageRegistry->RegisterReceiver(35, this);

    float avoidableWidth = 1.0f;
    nlVector3 avoidableCenter = { 20.6f, 0.0f, 0.0f };
    mpBoundaryAvoidables[0] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, avoidableWidth, 25.0f);
    avoidableCenter.x *= -1.0f;
    mpBoundaryAvoidables[1] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, avoidableWidth, 25.0f);

    if (GameInfoManager::Instance()->GetStadium() == 15)
        avoidableWidth = 6.0f;
    else if (GameInfoManager::Instance()->GetStadium() == 11)
        avoidableWidth = 3.0f;

    avoidableCenter.x = 0.0f;
    avoidableCenter.y = 12.5f;
    avoidableCenter.z = 0.0f;
    mpBoundaryAvoidables[2] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, 41.2f, avoidableWidth);
    avoidableCenter.y *= -1.0f;
    mpBoundaryAvoidables[3] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, 41.2f, avoidableWidth);
}
static inline const char* SuddenDeathEventName() { return "SuddenDeath"; }
static inline const char* GameOverEventName() { return "GameOver"; }

cGame::~cGame()
{
    StopSuddenDeathMusic();

    delete m_pPostResetClock;
    delete m_pGameClock;

    delete mpTerrain;

    delete mpWeatherManager;

    delete mpCrowdRiot;

    delete m_pFuzzyTweaks;
    delete m_pPostGameDoneClock;

    mpAIContext->Cleanup(true, true);
    delete mpAIContext;

    gNetworkMessageRegistry->UnregisterReceiver(34);
    gNetworkMessageRegistry->UnregisterReceiver(35);

    for (int i = 0; i < 4; i++)
    {
        delete mpBoundaryAvoidables[i];
    }

    if (lbl_806E0C74 != 0)
    {
        delete lbl_806E0C74;
        lbl_806E0C74 = 0;
    }

    gNextAvoidableObjectId = 0;
}
void cGame::ResetMegaStrikeMeterQueues()
{
    mReceivedMegaStrikeMeter.mHead = 0;
    mReceivedMegaStrikeMeter.mCount = 0;
    mPendingMegaStrikeMeter.mHead = 0;
    mPendingMegaStrikeMeter.mCount = 0;
}
void cGame::SendMegaStrikeMeter(bool value)
{
    mPendingMegaStrikeMeter.Push(value);

    if (mPendingMegaStrikeMeter.mCount < gMegaStrikeMeterBatchSize)
    {
        return;
    }

    int count = mPendingMegaStrikeMeter.mCount;
    NetworkMessageType35 message;
    message.mCount = count;
    for (int i = 0; i < count; i++)
    {
        message.mValues[i] = mPendingMegaStrikeMeter.Pop();
    }

    u8 buffer[50];
    s8 i;
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    int playerCount = g_pNetworkSessionBase->GetNumMachines();
    for (i = 0; i < playerCount; i++)
    {
        if (i != g_pNetworkSessionBase->GetLocalMachineId())
        {
            g_pNetworkSessionBase->Send(i, buffer, size, true);
        }
    }
}
void cGame::SendRemainingMegaStrikeMeter()
{
    tDebugPrintManager::Print(DC_NETWORK, "SendRemainingMegaStrikeMeter %d\n", mPendingMegaStrikeMeter.mCount);

    while (mPendingMegaStrikeMeter.mCount > 0)
    {
        int count = mPendingMegaStrikeMeter.mCount;
        if (count > 8)
        {
            count = 8;
        }

        NetworkMessageType35 message;
        message.mCount = count;
        for (int i = 0; i < count; i++)
        {
            message.mValues[i]
                = mPendingMegaStrikeMeter.Pop();
        }

        u8 buffer[50];
        s8 i;
        int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
        int playerCount = g_pNetworkSessionBase->GetNumMachines();
        for (i = 0; i < playerCount; i++)
        {
            if (i != g_pNetworkSessionBase->GetLocalMachineId())
            {
                g_pNetworkSessionBase->Send(i, buffer, size, true);
            }
        }
    }
}
void cGame::InitMegaStrikeGameplay()
{
    DebugWriteCache* output = gNetworkSyncState->GetWriteCache();
    if (output != 0)
    {
        char buffer[256];
        int frame = GetFixedUpdateTask()->GetFrame();
        nlSNPrintf(buffer, sizeof(buffer), "InitMegaStrikeGameplay called at %d\n", frame);
        output->WriteText(buffer);
        tDebugPrintManager::Print(DC_NETWORK, buffer);
    }

    g_pBall->m_uGoalType = 6;

    float accuracy = mpMegaStrikeShooter->mUnidentified394;
    float numBalls = mpMegaStrikeShooter->mUnidentified390;
    Goalie* pGoalie = mpMegaStrikeShooter->m_pTeam->GetOtherTeam()->GetGoalie();
    pGoalie->InitActionMegaStrike(numBalls, accuracy);
    mpMegaStrikeShooter->EndAction();
    fn_80038158(mpMegaStrikeShooter, 0);
}
void cGame::CleanupMegaStrikeGameplay()
{
    Goalie* pGoalie = mpMegaStrikeShooter->m_pTeam->GetOtherTeam()->GetGoalie();
    if (m_uMegastrikeGoals != 0)
    {
        pGoalie->InitActionMove(false);
    }
    else
    {
        if (pGoalie->m_pBall == 0 && g_pBall->m_pOwner != 0)
        {
            g_pBall->m_pOwner->ReleaseBall(0);
        }
        pGoalie->PickupBall(g_pBall);
        pGoalie->InitActionMoveWB();
    }
}
void cGame::ClearMegaStrikeCleanupPending()
{
    mbMegaStrikeCleanupPending = false;
}
void cGame::SetMegaStrikeGameplay(bool enabled, int defendingSide, int numShots)
{
    mbCaptainShotToScoreOn = enabled;
    if (enabled)
    {
        mbMegaStrikePositiveNet = g_pTeams[defendingSide]->m_pNet->m_v3NetLocation.x > 0.0f;
        if (gMegaStrikeShotCountOverride.mValue > 0)
        {
            m_uMegastrikeNumShots = gMegaStrikeShotCountOverride.mValue;
        }
        else
        {
            m_uMegastrikeNumShots = numShots;
        }
        m_nMegastrikeDefendingTeam = defendingSide;
    }
    else
    {
        m_uMegastrikeNumShots = 0;
        mReceivedMegaStrikeMeter.mHead = 0;
        mReceivedMegaStrikeMeter.mCount = 0;
        mPendingMegaStrikeMeter.mHead = 0;
        mPendingMegaStrikeMeter.mCount = 0;
    }

    m_uMegastrikeCurShot = 0;
    m_uMegastrikeGoals = 0;
    mbMegaStrikePlayerReady = false;
    mbMegaStrikePlayerReadySent = false;
}
void cGame::StartSlowDown(float timeScale, float transitionTime)
{
    if (g_pNetworkSessionBase->GetNumMachines() > 1 && timeScale < 0.3f)
    {
        timeScale = 0.3f;
    }

    if (FixedUpdateTask::GetTargetTimeScale() != 1.0f || 1.0f != timeScale)
    {
        if (g_pGame->m_eGameState != 4)
        {
            if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
            {
                m_pGameClock->Stop();
            }

            if (FixedUpdateTask::GetTargetTimeScale() == 1.0f)
            {
                unsigned long soundID = 0xCE5CBAC7;
                StopSound(soundID, g_pGame);
                PlaySound(10, soundID, "PPass Slowdown", g_pGame);

                u32 hash = nlStringLowerHash("Slow-mo_Captain_Hit");
                ApplyAudioTransition(&hash, 0, 0);
            }

            g_pOverlayManager->GetScene((SceneList)89)->SetVisible(false);
            gpNumberDisplay->mVisible = false;

            if (transitionTime <= 0.0f)
            {
                FixedUpdateTask::SetTimeScale(timeScale);
                ParticleUpdateTask::sInstance->SetTimeScale(timeScale);
            }
            else
            {
                FixedUpdateTask::SetTimeScale(timeScale, transitionTime);
                ParticleUpdateTask::sInstance->SetTimeScale(timeScale);
            }
        }
    }
}
float cGame::GetNormalizedGameTime()
{
    return m_pGameClock->m_fTimer / m_fGameDuration;
}
float cGame::GetGameTime()
{
    return m_pGameClock->m_fTimer;
}
void cGame::fn_800586C0()
{
    if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
    {
        m_pGameClock->Start();
    }
}
void cGame::fn_80058704()
{
    if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
    {
        m_pGameClock->Stop();
    }
}
void cGame::ResetForKickOff()
{
    ++lbl_806E2130;
    if (mbMegaStrikeCleanupPending)
    {
        CleanupMegaStrikeGameplay();
        mbMegaStrikeCleanupPending = false;
    }
    mUnidentified49C.mGetReadyForKickoffEvent.Queue();

    SetFieldTilt(0, 0.0f, 0.0f);
    gNPCManager->ResetNPCs();
    ResetCharacters();

    ResetBall(g_pBall, false);
    m_bBallInNet = false;
    ResetPowerups(false);
    lbl_806E12C8->ResetEffects();
    m_pScorer = 0;
    m_pAssister = 0;

    for (int i = 0; i < 2; i++)
    {
        m_pTeamTouch[i] = g_pTeams[i]->GetCaptain();
    }

    SetRenderWorldEffects(true);
    EndPeachPhoto(&gPeachPhotoState, true);
    m_pPostResetClock->Reset(0.0f, 0.5f, 1.0f);
    m_pPostResetClock->Start();
    ReplayChoreo::Instance().Finish();
    cCameraManager::Remove((eCameraType)13, true);

    GameplayCamera* camera = cCameraManager::GetCamera<GameplayCamera>(eCameraType_Gameplay);
    if (camera != 0)
    {
        camera->SetForceNeutralAndNearZoom(true);
    }
    StopDisplayingElectricFence();
    --lbl_806E2130;
}
void cGame::fn_80058A78(float seconds)
{
    m_pPostResetClock->Reset(0.0f, seconds, 1.0f);
    m_pPostResetClock->Start();
}
void PostResetCallback(unsigned long, unsigned long)
{
    cGame* game = g_pGame;
    game->ResumeAfterPresentation();
    game->mUnidentified49C.mKickoffEvent.Queue();

    GameplayCamera* camera = cCameraManager::GetCamera<GameplayCamera>(eCameraType_Gameplay);
    if (camera != 0)
    {
        camera->SetForceNeutralAndNearZoom(false);
    }
}
void cGame::BeginGame(bool bRematch, bool bStraightToKickoff)
{
    ++lbl_806E2130;
    FixedUpdateTask::SetTimeScale(1.0f);
    ParticleUpdateTask::sInstance->SetTimeScale(1.0f);

    Function<DetermDataEvent*> callback(BindMember(this, &cGame::ReceiveCustomDetermData));
    GetInputRouter();
    GetDetermDataEventQueue()->Add(callback, 0, -1);

    if (m_eGameState != 0)
    {
        ChangeGameState(0);
    }

    ResetGameFields();

    SetMegaStrikeGameplay(false, 0, 0);
    ResetCachedPlayerDistances();
    mpWeatherManager->Reset();
    mpWeatherManager->Stop(true);
    ResetCharacters();
    ResetBall(g_pBall, false);
    m_bBallInNet = false;
    ResetPowerups(true);
    EndPeachPhoto(&gPeachPhotoState, true);
    EmissionManager::Instance()->KillAll();
    BasicStadium::GetCurrentStadium()->ResetEffects();
    for (float elapsed = 0.0f; elapsed < 3.0f; elapsed += 0.0333f)
    {
        static_cast<World*>(BasicStadium::GetCurrentStadium())->Update(0.0333f, true);
        EmissionManager::Instance()->Update(0.0333f);
    }
    gNPCManager->ResetNPCs();

    m_pGameClock->Reset(0.0f, 999999.0f, 1.0f);
    m_pGameClock->Stop();
    m_pPostGameDoneClock->Reset(0.0f, 1.5f, 1.0f);
    m_pPostGameDoneClock->Stop();
    gpNumberDisplay->Reset();
    if (!GameInfoManager::Instance()->IsInMode4())
    {
        gpNumberDisplay->SetScores(0, 0);
        g_pTeams[0]->m_nScore = 0;
        g_pTeams[1]->m_nScore = 0;
    }
    else
    {
        gpNumberDisplay->SetScores(g_pStrikerChallenge->mScore[0], g_pStrikerChallenge->mScore[1]);
        InitializeChallengeTeamScore(g_pTeams[0], 0);
        InitializeChallengeTeamScore(g_pTeams[1], 1);
    }
    for (int i = 0; i < 10; i++)
    {
        g_pCharacters[i]->fn_80022E60();
        cCharacter* character = g_pCharacters[i];
        character->m_Dirt = 0.0f;
        character->mUnidentified16C = 0;
        g_pCharacters[i]->m_MinDirt = 0.0f;
    }
    GetPresentation()->Reset();
    ReplayChoreo::Instance().FlushHighlights();
    ReplayChoreo::Instance().Finish();
    if (bStraightToKickoff)
    {
        ChangeGameState(1);
        FixedUpdateTask* task = GetFixedUpdateTask();
        task->mSimulationStarted = true;
    }
    else
    {
        GetPresentation()->PlayGameBegin();
    }

    --lbl_806E2130;
    ReplayManager::Instance()->ResetSnapshots();
    if (IsNetworkOrRecordedGame())
    {
        SetScriptTimeBudget(-1.0f);
        gAIProfilingClock = GetGameSimulationMilliseconds;
        gAIActivityClock = GetGameSimulationMilliseconds;
    }
    else
    {
        SetScriptTimeBudget(1.0f);
        gAIProfilingClock = GetGameTickerMilliseconds;
        gAIActivityClock = GetGameSimulationMilliseconds;
    }
}
void cGame::CheckForGoal()
{
    struct GoalScoredDataExt
    {
        GoalScoredData data;
        int sideOfInterest;
    };

    int nSide;

    if (g_pBall->GetInNet(nSide) && !m_bBallInNet)
    {
        nSide = (nSide + 1) % 2;
        m_nLastTeamToScore = nSide;
        g_pTeams[nSide]->m_nScore += 1;

        if (GameInfoManager::Instance()->IsInMode4()
            && g_pStrikerChallenge->mCondition == 2 && nSide == 1)
        {
            ChangeGameState(3);
        }
        else if (m_eGameState == 6)
        {
            ChangeGameState(3);
        }
        else if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 1
            && g_pTeams[nSide]->GetScore() >= GameInfoManager::Instance()->GetCurrentSettings()->GoalLimit)
        {
            ChangeGameState(3);
        }
        else
        {
            ChangeGameState(2);
        }

        if (g_pBall->m_pLastTouch == NULL)
        {
            g_pBall->m_pLastTouch = g_pTeams[nSide]->GetCaptain();
        }

        if (m_pScorer == NULL)
        {
            m_pScorer = g_pTeams[nSide]->GetCaptain();
        }

        if (m_pScorer != NULL && m_pScorer != g_pBall->m_pLastTouch
            && g_pBall->m_pLastTouch->m_eClassType != GOALIE)
        {
            float fDirection = g_pBall->m_pLastTouch->m_pTeam->m_pNet->m_v3NetLocation.x
                * g_pBall->m_v3Position.x;
            if (fDirection >= 0.0f)
            {
                g_pBall->m_uGoalType = 5;
            }
            else
            {
                g_pBall->m_uGoalType = 3;
            }

            SetPotentialScorer(g_pBall->m_pLastTouch);
        }
        else if (m_pScorer != NULL)
        {
            if (nSide != m_pScorer->m_pTeam->m_nSide)
            {
                g_pBall->m_uGoalType = 5;
            }
        }

        // A scorer still holding the ball (a captain carried in) lets go of it.
        if (m_pScorer != NULL && m_pScorer->m_pBall != NULL)
        {
            m_pScorer->ReleaseBall(0);
            if (m_pScorer->m_eClassType == FIELDER)
            {
                ((cFielder*)m_pScorer)->ShootBallDueToContact(m_pScorer->mUnidentified024.m_v3Velocity);
                g_pBall->m_uGoalType = 7;
            }
        }

        GoalScoredDataExt goalScored;
        goalScored.data.uNumGoalsScored = 1;
        goalScored.data.uTeamIndex = nSide;
        goalScored.data.uGoalType = g_pBall->m_uGoalType;
        goalScored.data.v3ShotPosition = g_pBall->m_v3ShotOrigin;
        goalScored.data.pScorer = m_pScorer;
        goalScored.data.pAssister = m_pAssister;

        if (m_pScorer != NULL && m_pScorer->GetGlobalPad() != NULL)
        {
            goalScored.sideOfInterest = m_pScorer->GetGlobalPad()->GetPadID();
        }
        else
        {
            goalScored.sideOfInterest = -1;
        }

        goalScored.data.pLastTouch[0] = m_pTeamTouch[0];
        goalScored.data.pLastTouch[1] = m_pTeamTouch[1];
        m_bBallInNet = true;

        DeliverGoalScored(g_pGame, &goalScored.data);

        if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
        {
            unsigned long soundID;
            if (GetTeamScoreDifference(g_pTeams[nSide]) == 0)
            {
                soundID = 0x9D796D7D;
            }
            else if (GetTeamScoreDifference(g_pTeams[nSide]) == 1)
            {
                soundID = 0xC272152B;
                if (nSide == 0)
                {
                    soundID = 0x04F9F242;
                }
            }
            else
            {
                soundID = 0xB8511A3D;
                if (nSide == 0)
                {
                    soundID = 0x3E363074;
                }
            }
            PlayCrowdReaction(soundID);
        }

        Goalie::HandleGoalScored(nSide);
        g_pBall->m_uGoalType = 4;
    }
}
void cGame::BlowUpPowerups(
    const nlPolygonRegion& region,
    float fExplosionRadius)
{
    for (int i = 0; i < 25; i++)
    {
        if (g_pPowerups[i] != 0)
        {
            nlVector2 position;
            position.x = g_pPowerups[i]->m_v3Position.x;
            position.y = g_pPowerups[i]->m_v3Position.y;
            if (region.ContainsPoint2D(position))
            {
                g_pPowerups[i]->fn_8009D74C(fExplosionRadius, false);
            }
        }
    }
}
void cGame::ResetPowerups(bool clearPowerUps)
{
    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        if (pTeam != 0)
        {
            if (clearPowerUps)
            {
                pTeam->ClearAllPowerUps();
                pTeam->ClearCurrentPowerUp();
            }
            pTeam->mfPowerupMeter = 0.0f;
        }
    }

    for (int i = 0; i < 25; i++)
    {
        PowerupBase* pPowerup = g_pPowerups[i];
        if (pPowerup != 0)
        {
            pPowerup->Destroy(true);
            g_pPowerups[i] = 0;
        }
    }
}
void cGame::ResetCachedPlayerDistances()
{
    for (int i = 0; i < 2; i++)
    {
        g_pTeams[i]->fn_800A607C();
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 5; k++)
            {
                m_nClosestPlayers[i][j][k] = g_pTeams[j]->GetPlayer(k);
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            m_fCachedPlayerDistances[i][j] = 0.0f;
        }
    }

    for (int i = 0; i < 10; i++)
    {
        m_fCachedBallPlayerDistances[i] = 0.0f;
    }
}
void cGame::CopyPlayerDistanceSnapshot(void* snapshotData)
{
    PlayerDistanceSnapshot* snapshot = static_cast<PlayerDistanceSnapshot*>(snapshotData);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 5; k++)
            {
                cPlayer* pPlayer = m_nClosestPlayers[i][j][k];
                snapshot->mPlayerIndices[i * 10 + j * 5 + k] = pPlayer == 0 ? -1 : GetPlayerIndex(pPlayer);
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            snapshot->mDistances[i * 10 + j]
                = m_fCachedPlayerDistances[i][j];
        }
    }

    for (int i = 0; i < 10; i++)
    {
        snapshot->mBallDistances[i]
            = m_fCachedBallPlayerDistances[i];
    }
}
void cGame::PlayEndGamePresentation()
{
    GetPresentation()->PlayGoalEffects("Goal_endgame");
    GetPresentation()->Call("GameEndNoSuddenDeath", "");
}

void cGame::SendNISLoadedCustomDeterm(u8 machineBits)
{
    struct Message
    {
        u8 type;
        u8 machineBits;
    } message;

    message.type = 29;
    message.machineBits = machineBits;

    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, "Sending NIS Loaded %d at frame %d\n", message.machineBits, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}
void cGame::SendMegaStrike(
    int side, int playerId, float numBalls, float accuracy)
{
    struct Message
    {
        u8 type;
        u8 side;
        u8 playerId;
        u8 padding;
        float numBalls;
        float accuracy;
    } message;

    message.type = 181;
    message.side = side;
    message.playerId = playerId;
    message.padding = 0;
    message.numBalls = numBalls;
    message.accuracy = accuracy;

    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, "Sending MegaStrike Side %d PlayerID %d NumBalls %f Accuracy %f at frame %d\n", message.side, message.playerId, message.numBalls, message.accuracy, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}
void cGame::SendMegaStrikePlayerReady()
{
    u8 message = 183;
    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, "Sending MegaStrikePlayerReady at frame %d\n", frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}
void cGame::SendMegaStrikeKillCursor()
{
    u8 message = 185;
    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, "Sending SendMegaStrikeKillCursor at frame %d\n", frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}
void cGame::SendMegaStrikeGoalie(unsigned int side, unsigned int target, float score)
{
    struct Message
    {
        u8 type;
        u8 side;
        u8 target;
        u8 padding;
        float score;
    } message;

    message.type = 182;
    message.side = side;
    message.target = target;
    message.padding = 0;
    message.score = score;

    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, "Sending MegaStrikeGoalie Side %d CurTarget %d Score %f at frame %d\n", message.side, message.target, message.score, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}
void cGame::SendSlowDownEnd()
{
    u8 message = 222;
    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, "Sending Slow Down End at frame %d\n", frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}
void cGame::ReceiveCustomDetermData(DetermDataEvent* pEvent)
{
    u8 type = pEvent->mData[0];

    DebugWriteCache* output = gNetworkSyncState->GetWriteCache();
    if (output != 0)
    {
        char buffer[256];
        nlSNPrintf(buffer, sizeof(buffer), "Recv CustomDeterm %d size %d at frame %d\n", type, pEvent->mSize,
            GetFixedUpdateTask()->GetFrame());
        output->WriteText(buffer);
    }

    switch (type)
    {
    case 5:
    {
        // Which players are on screen, one bit per player.
        u8* data = pEvent->mData;
        for (int i = 0; i < 2; i++)
        {
            cTeam* pTeam = g_pTeams[i];
            u8 flags = data[1 + i];
            for (int j = 0; j < 5; j++)
            {
                cPlayer* pPlayer = pTeam->GetPlayer(j);
                pPlayer->mUnidentified024.m_bOnScreen = (flags & (u8)(1 << j)) != 0;
            }
        }
        break;
    }

    case 29:
        GetPresentation()->ReceiveNisLoaded(pEvent->mData[1]);
        break;

    case 181:
    {
        cFielder* pFielder = g_pTeams[pEvent->mData[1]]->GetFielder(pEvent->mData[2]);
        tDebugPrintManager::Print(DC_NETWORK, "Received MegaStrike Side %d PlayerID %d NumBalls %f Accuracy %f at frame %d\n", pEvent->mData[1],
            pEvent->mData[2], *(float*)&pEvent->mData[4], *(float*)&pEvent->mData[8],
            gInputManager->mFrameProvider->GetFrame());
        fn_800406B0(pFielder, *(float*)&pEvent->mData[4], *(float*)&pEvent->mData[8]);
        break;
    }

    case 183:
        tDebugPrintManager::Print(DC_NETWORK, "Received MegaStrikePlayerReady at frame %d\n",
            gInputManager->mFrameProvider->GetFrame());
        mbMegaStrikePlayerReady = true;
        break;

    case 185:
        tDebugPrintManager::Print(DC_NETWORK, "Received MegaStrikeKillCursor at frame %d\n",
            gInputManager->mFrameProvider->GetFrame());
        ResetMegaBallPointer();
        break;

    case 182:
    {
        Goalie* pGoalie = g_pTeams[pEvent->mData[1]]->GetGoalie();
        tDebugPrintManager::Print(DC_NETWORK, "Received MegaStrikeGoalie Side %d CurTarget %d Score %f at frame %d\n", pEvent->mData[1],
            pEvent->mData[2], *(float*)&pEvent->mData[4],
            gInputManager->mFrameProvider->GetFrame());
        pGoalie->fn_80084D70(pEvent->mData[2], *(float*)&pEvent->mData[4]);
        break;
    }

    case 222:
        // SlowDownEnd: back to full speed after a captain hit.
        tDebugPrintManager::Print(DC_NETWORK, "Received SlowDownEnd at frame %d\n",
            gInputManager->mFrameProvider->GetFrame());
        if (g_pGame->m_eGameState != 4)
        {
            StopSound(0xCE5CBAC7, g_pGame);
            if (FixedUpdateTask::GetTargetTimeScale() < 1.0f)
            {
                FixedUpdateTask::SetTimeScale(1.0f);
                ParticleUpdateTask::sInstance->SetTimeScale(1.0f);
                if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
                {
                    m_pGameClock->Start();
                }

                PlaySound(10, 0x54A8A6A0, 0, 0);
                u32 hash = nlStringLowerHash("Slow-mo_Captain_Hit");
                ApplyAudioTransition(&hash, true, 0);
                g_pOverlayManager->GetScene((SceneList)89)->SetVisible(true);
                gpNumberDisplay->mVisible = true;
            }
        }
        break;

    case 188:
    {
        // CleanupMegastrikeGameplay
        tDebugPrintManager::Print(DC_NETWORK, "Received CleanupMegastrikeGameplay at frame %d\n",
            gInputManager->mFrameProvider->GetFrame());
        CleanupMegaStrikeGameplay();
        break;
    }

    default:
        tDebugPrintManager::Print(DC_NETWORK, "Unknown custom determ data %d..discarding\n", type);
        break;
    }
}
int cGame::ProcessMessage(NetworkMessage* message)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    s8 machine = roster->MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK, "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mSource);
        return 1;
    }

    if (NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        s8 tournamentIndex = NetTournManager::Instance()->TournamentIdxToMachineIdx(machine);
        if (tournamentIndex < 0 || tournamentIndex >= g_pNetworkSessionBase->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK, "Discarded message type %d.  TournamentIdxToMachineIdx changed ID %d to ID %d, but invalid\n",
                (u8)message->GetType(), machine, tournamentIndex);
            return 1;
        }
    }

    switch ((u8)message->GetType())
    {
    case 34:
        if (mbCaptainShotToScoreOn)
        {
            ReceiveMegaBallPointerUpdate(message);
        }
        break;

    case 35:
    {
        NetworkMessageType35* pMessage = (NetworkMessageType35*)message;
        for (int i = 0; i < pMessage->mCount; i++)
        {
            mReceivedMegaStrikeMeter.Push(pMessage->mValues[i]);
        }
        break;
    }
    }

    return 1;
}
void cGame::PreUpdate(float deltaTime)
{
    for (int i = 0; i < 2; i++)
    {
        g_pTeams[i]->PreUpdate(deltaTime);
    }
}
void cGame::RandomizePlayerUpdateOrder()
{
    int i;
    for (i = 0; i < 5; i++)
    {
        m_pRandomPlayersArray[i] = g_pTeams[0]->GetPlayer(i);
    }
    for (i = 0; i < 5; i++)
    {
        m_pRandomPlayersArray[5 + i] = g_pTeams[1]->GetPlayer(i);
    }

    static FilteredRandomRange randgen;
    for (i = 0; i < 10; i++)
    {
        int j = randgen.genrand(10);
        if (j != i)
        {
            cPlayer* temp = m_pRandomPlayersArray[i];
            m_pRandomPlayersArray[i] = m_pRandomPlayersArray[j];
            m_pRandomPlayersArray[j] = temp;
        }
    }
}
cPlayer* gPlayerDistanceSortReference;

void cGame::SendPlayerVisibility()
{
    --lbl_806E2130;

    if (g_pNetworkSessionBase->GetLocalMachineId() == 0
        && !gNetworkInputRecording->mPlaybackReady
        && gInputManager->mFrameProvider->GetFrame() % 10 == 0)
    {
        u8 message[3];
        message[0] = 5;

        for (int i = 0; i < 2; i++)
        {
            u8 flags = 0;
            cTeam* pTeam = g_pTeams[i];
            for (int j = 0; j < 5; j++)
            {
                if (pTeam->GetPlayer(j)->fn_8001E184())
                {
                    flags |= 1 << j;
                }
            }
            message[i + 1] = flags;
        }

        GetInputRouter()->QueueDetermData(message, sizeof(message));
    }

    ++lbl_806E2130;
}
void cGame::Update(float fDeltaT)
{
    if (m_eGameState == 4)
    {
        return;
    }

    AIPadManager::UpdateAccelerationHistory();
    SendPlayerVisibility();
    UpdateCachedGameData(fDeltaT);
    UpdateShockwaves(fDeltaT);

    // Retail calls this here and drops the result.
    IsNetworkOrRecordedGame();

    // Time is up once fewer than a tenth of a second remain.
    if ((unsigned int)(10.0f * (m_fGameDuration - m_pGameClock->m_fTimer)) == 0
        && !IsBallShotActive(g_pBall))
    {
        if (g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore)
        {
            if (m_eGameState == 5)
            {
                if (GameInfoManager::Instance()->IsInMode4()
                    && g_pStrikerChallenge->mCurrentChallenge == 2
                    && g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore)
                {
                    ChangeGameState(3);
                    StatsTracker::Instance()->TrackWinner(-1);
                }
                else
                {
                    ChangeGameState(6);
                    StatsTracker::Instance()->mIsOvertime = true;
                }
            }
        }
        else if (m_eGameState != 3)
        {
            ChangeGameState(3);
            StatsTracker::Instance()->TrackWinner(-1);
        }
    }

    for (int i = 0; i < 2; i++)
    {
        FuzzyScriptSetCurrentTeam(g_pTeams[i]);
        g_pTeams[i]->Update(fDeltaT);
        FuzzyScriptClearGlobals();
    }

    mpAIContext->Update(true, fDeltaT);

    for (int i = 0; i < 10; i++)
    {
        g_pCurrentlyUpdatingCharacter = m_pRandomPlayersArray[i];

        if (m_pRandomPlayersArray[i]->m_eClassType == FIELDER)
        {
            FuzzyScriptSetCurrentFielder((cFielder*)m_pRandomPlayersArray[i]);
        }
        else
        {
            FuzzyScriptSetCurrentTeam(m_pRandomPlayersArray[i]->m_pTeam);
        }

        g_pCurrentlyUpdatingTeam = m_pRandomPlayersArray[i]->m_pTeam;
        m_pRandomPlayersArray[i]->Update(fDeltaT);
        FuzzyScriptClearGlobals();
    }

    g_pCurrentlyUpdatingCharacter = 0;
    g_pBall->Update(fDeltaT);
    mpWeatherManager->Update(fDeltaT);
    lbl_806E12C8->Update(fDeltaT);

    if (IsGameplayOrOvertime())
    {
        CheckForGoal();

        // Countdown beeps over the last seconds of the match.
        if (muCountdownBeepsRemaining != 0)
        {
            if (GetGameDuration() - GetGameTime()
                < 5.0f - (float)(int)(gEndGameCountdownBeeps - muCountdownBeepsRemaining))
            {
                muCountdownBeepsRemaining--;
                PlaySound(15, 0x97E84AE4, 0, 0);
            }
        }
    }

    UpdatePowerUpObjects(fDeltaT);

    // Field tilt.
    int homeCaptainAction = g_pTeams[0]->GetCaptain()->m_eActionState;
    int awayCaptainAction = g_pTeams[1]->GetCaptain()->m_eActionState;

    if (mbCaptainShotToScoreOn || homeCaptainAction == 12 || homeCaptainAction == 11
        || awayCaptainAction == 11 || awayCaptainAction == 12)
    {
        // Level the field out while a captain shoots to score.
        maTiltDir = 0;
        mfTiltLevelTimer = 0.0f;
        mfTiltSpeed = 0.0f;
        mfDesiredTiltSpeed = 0.0f;
        maDesiredTiltDir = 0;
        muTiltFrames = 0;
        mfXTilt = cCharacter::SeekSpeedExponential(
            mfXTilt, 0.0f, gMegaStrikeTiltLevelRate, fDeltaT);
        mfYTilt = cCharacter::SeekSpeedExponential(
            mfYTilt, 0.0f, gMegaStrikeTiltLevelRate, fDeltaT);
        SetFieldTilt(0, mfXTilt, mfYTilt);
    }
    else if (GameInfoManager::Instance()->IsRule0x4Equal4() || gForceScoreTilt)
    {
        // The field tilts towards the side that is behind.
        float fMin = -gScoreTiltMaxDifference;
        float fMax = gScoreTiltMaxDifference;
        float fDiff = (float)(g_pTeams[1]->m_nScore - g_pTeams[0]->m_nScore);
        fDiff = nlMinEquals(nlMaxEquals(fDiff, fMin), fMax);

        mfXTilt = cCharacter::SeekSpeedExponential(
            mfXTilt, 0.0f, gScoreTiltSeekRate, fDeltaT);
        mfYTilt = cCharacter::SeekSpeedExponential(
            mfYTilt, fDiff * gScoreTiltScale, gScoreTiltSeekRate, fDeltaT);
        SetFieldTilt(0, mfXTilt, mfYTilt);
    }
    else if ((GameInfoManager::Instance()->GetStadium() == 9 || gForceWeatherTilt)
        && !GetConfigBool(Config::Global(), "no_weather", false)
        && !GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        // Weather tilt: wander towards random targets, ramping up over time.
        mUnidentified0A0 += fDeltaT;
        if (mUnidentified0A0 > gWeatherTiltRampDuration)
        {
            mUnidentified0A0 = 0.0f;
        }

        if (++muTiltFrames >= 15)
        {
            muTiltFrames = 0;

            float fRamp = 1.0f;
            float fFraction = mUnidentified0A0 / gWeatherTiltRampDuration;
            fRamp = (fRamp <= fFraction) ? fRamp : fFraction;

            float fHalfRamp = 0.5f * fRamp;
            float fRangeX = fHalfRamp * gWeatherTiltXRangeScale;
            float fRangeY = fHalfRamp * gWeatherTiltYRangeScale;

            if (fRangeX > 0.0f)
            {
                mfXTiltMin = -fRangeX - nlRandomf(fRangeX);
                mfXTiltMax = fRangeX + nlRandomf(fRangeX);
            }
            else
            {
                mfXTiltMin = 0.0f;
                mfXTiltMax = 0.0f;
            }

            mfYTiltMin = -fRangeY - nlRandomf(fRangeY);
            mfYTiltMax = fRangeY + nlRandomf(fRangeY);
            mfDesiredTiltSpeed = nlRandomf(Interpolate(gWeatherTiltInitialSpeed, gWeatherTiltFinalSpeed, fRamp));
            maDesiredTiltDir = maTiltDir + nlRandom(gWeatherTiltDirectionJitter) - gWeatherTiltDirectionJitter / 2;
        }

        // Turn back once the tilt leaves its range.
        if (mfXTilt < mfXTiltMin || mfXTilt > mfXTiltMax)
        {
            u16 aAway = (mfXTilt < 0.0f) ? 0 : 0x8000;
            if (nlAbsAngle(nlAngleDelta(maDesiredTiltDir, aAway)) > 0x4000)
            {
                maDesiredTiltDir = 0x8000 - maDesiredTiltDir;
            }
            mfDesiredTiltSpeed = 0.75f * gWeatherTiltFinalSpeed;
        }

        if (mfYTilt < mfYTiltMin || mfYTilt > mfYTiltMax)
        {
            int aAway = (mfYTilt < 0.0f) ? 0x4000 : 0xC000;
            if (nlAbsAngle((s16)(maDesiredTiltDir - aAway)) > 0x4000)
            {
                maDesiredTiltDir = -maDesiredTiltDir;
            }
            mfDesiredTiltSpeed = 0.75f * gWeatherTiltFinalSpeed;
        }

        maTiltDir = SeekDirection(
            maTiltDir, maDesiredTiltDir, 30000.0f, 3000.0f, fDeltaT);
        mfTiltSpeed = cCharacter::SeekSpeedExponential(
            mfTiltSpeed, mfDesiredTiltSpeed, gWeatherTiltSpeedSeekRate, fDeltaT);

        float fDX;
        float fDY;
        nlPolarToCartesian(fDX, fDY, maTiltDir, mfTiltSpeed);
        mfXTilt = fDX * fDeltaT + mfXTilt;
        mfYTilt = fDY * fDeltaT + mfYTilt;
        SetFieldTilt(0, mfXTilt, mfYTilt);
    }
    else if (fabsf(mfXTilt) > 0.001f || fabsf(mfYTilt) > 0.001f)
    {
        // No tilt source: hold, then level the field out.
        float fTimer = mfTiltLevelTimer;
        float fZero = 0.0f;
        if (fTimer < fZero)
        {
            mfTiltLevelTimer = fTimer - fDeltaT;
            if (mfTiltLevelTimer < -gFieldTiltHoldTime)
            {
                mfTiltLevelTimer = fZero;
            }
        }
        else
        {
            float fNewTimer = fTimer + fDeltaT;
            float fX = fZero;
            float fY = fZero;
            if (fNewTimer >= gFieldLevelTime - fDeltaT)
            {
                fNewTimer = fZero;
            }
            else
            {
                float fStep = fDeltaT / (gFieldLevelTime - fNewTimer);
                fStep = (fStep >= fZero) ? fStep : fZero;
                fStep = (fStep <= 1.0f) ? fStep : 1.0f;
                fX = Interpolate(mfXTilt, 0.0f, fStep);
                fY = Interpolate(mfYTilt, 0.0f, fStep);
            }
            SetFieldTilt(0, fX, fY);
            mfTiltLevelTimer = fNewTimer;
        }
    }

    // EnterPostGame
    if (m_pPostGameDoneClock->m_clockState == CLOCK_DONE)
    {
        m_pPostGameDoneClock->Reset(0.0f, 1.5f, 1.0f);

        int awayScore = g_pTeams[1]->m_nScore;
        int homeScore = g_pTeams[0]->m_nScore;
        int nWinner = homeScore < awayScore;
        if ((GameInfoManager::Instance()->IsInMode4()
                && g_pStrikerChallenge->mCondition == 2
                && g_pTeams[1]->m_nScore > 0)
            || (g_pStrikerChallenge->mCurrentChallenge == 2
                && g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore))
        {
            NisPlayer* pNisPlayer = NisPlayer::Instance();
            pNisPlayer->mWinnerSide[0] = 1;
        }
        else
        {
            NisPlayer::Instance()->mWinnerSide[0] = nWinner;
        }

        UnidentifiedCameraEffects::Instance()->ResetForPresentation((void*)nWinner);
        if (!DuringEndOfGamePresentation(GetPresentation()))
        {
            PlayEndGamePresentation();
        }
    }

    UnidentifiedCameraEffects::Instance()->UnidentifiedNoOp();
}
extern "C" void fn_8005B330(
    nlVector3* pVector, float fXAxisTilt, float fYAxisTilt)
{
    float fSin;
    float fCos;

    nlSinCos(&fSin, &fCos, ((s32)(65536.0f * fYAxisTilt)) / 360);

    nlVec3Set(*pVector, fSin, 0.0f, fCos);

    nlSinCos(&fSin, &fCos, ((s32)(65536.0f * fXAxisTilt)) / 360);

    pVector->y = fSin;
    pVector->z = pVector->z * fCos;

    float temp_f1 = nlRecipSqrt(pVector->GetLengthSq3D(), true);
    nlVec3Scale(*pVector, temp_f1);
}
int ComparePlayerDistances(
    cPlayer* const* first, cPlayer* const* second)
{
    int referenceIndex = GetPlayerIndex(gPlayerDistanceSortReference);
    cPlayer* firstPlayer = *first;
    int firstIndex = GetPlayerIndex(firstPlayer);
    cPlayer* secondPlayer = *second;
    float firstDistance = g_pGame->fn_8005B748(referenceIndex, firstIndex);
    float secondDistance = g_pGame->fn_8005B748(
        referenceIndex, GetPlayerIndex(secondPlayer));

    if (firstDistance == secondDistance)
    {
        return 0;
    }
    if (firstDistance < secondDistance)
    {
        return -1;
    }
    return 1;
}
void cGame::UpdateCachedGameData(float fDeltaT)
{
    g_FuzzyQuestionCache.Clear();
    ResetScriptFrameTime(&g_FuzzyQuestionCache);

    float fBallRadius = g_pBall->m_pPhysicsBall->GetRadius();
    for (int i = 0; i < 10; i++)
    {
        float fPlayerRadius
            = static_cast<cPlayer*>(g_pCharacters[i])->mUnidentified320->GetRadius();

        cPlayer* pPlayer = static_cast<cPlayer*>(g_pCharacters[i]);
        cBall* pBall = g_pBall;
        nlVector2 v2BallDistance;
        v2BallDistance.x
            = pBall->m_v3Position.x - pPlayer->mUnidentified024.m_v3Position.x;
        v2BallDistance.y
            = pBall->m_v3Position.y - pPlayer->mUnidentified024.m_v3Position.y;
        m_fCachedBallPlayerDistances[i] = nlVec2Length(v2BallDistance);
        m_fCachedBallPlayerDistances[i]
            -= fBallRadius + fPlayerRadius;

        for (int j = 0; j < 10; j++)
        {
            if (i <= j)
            {
                m_fCachedPlayerDistances[i][j] = 0.0f;
            }
            else
            {
                cPlayer* pPlayer = static_cast<cPlayer*>(g_pCharacters[i]);
                cPlayer* pOtherPlayer = static_cast<cPlayer*>(g_pCharacters[j]);
                nlVector2 v2PlayerDistance;
                v2PlayerDistance.x = pPlayer->mUnidentified024.m_v3Position.x
                                   - pOtherPlayer->mUnidentified024.m_v3Position.x;
                v2PlayerDistance.y = pPlayer->mUnidentified024.m_v3Position.y
                                   - pOtherPlayer->mUnidentified024.m_v3Position.y;
                m_fCachedPlayerDistances[i][j]
                    = nlVec2Length(v2PlayerDistance);
                m_fCachedPlayerDistances[i][j]
                    -= fPlayerRadius
                     + static_cast<cPlayer*>(g_pCharacters[j])->mUnidentified320->GetRadius();
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {
        gPlayerDistanceSortReference = static_cast<cPlayer*>(g_pCharacters[i]);
        for (int j = 0; j < 2; j++)
        {
            nlQSort(m_nClosestPlayers[i][j], 5, ComparePlayerDistances);
        }
    }
    gPlayerDistanceSortReference = 0;
}
float cGame::fn_8005B748(int firstIndex, int secondIndex)
{
    if (firstIndex > secondIndex)
    {
        return m_fCachedPlayerDistances[firstIndex][secondIndex];
    }
    return m_fCachedPlayerDistances[secondIndex][firstIndex];
}
cPlayer* cGame::GetClosestPlayer(int playerIndex, int side, int rank)
{
    return m_nClosestPlayers[playerIndex][side][rank];
}
void cGame::SetPotentialScorer(cPlayer* pPlayer)
{
    cPlayer* pOldScorer = m_pScorer;

    if (pOldScorer != 0 && pPlayer != 0 && pOldScorer != pPlayer
        && pOldScorer->IsOnSameTeam(pPlayer))
    {
        m_pAssister = m_pScorer;
    }
    else
    {
        m_pAssister = 0;
    }

    m_pScorer = pPlayer;

    if (pPlayer != 0 && pPlayer->m_eClassType == FIELDER)
    {
        m_pTeamTouch[pPlayer->m_pTeam->m_nSide] = pPlayer;
    }
}
// Sync log type ids, registered on first use (SyncLog).
u16 gPlayerDistanceSyncType = 0xFFFF;
u16 gGameStateSyncType = 0xFFFF;

inline void PlayerDistanceSnapshot::RegisterDebugFields(DebugWriteCache* cache)
{
    if (gPlayerDistanceSyncType == 0xFFFF)
    {
        gPlayerDistanceSyncType = cache->BeginType("GenDetPlayerC");
        cache->AddArrayField(DEBUG_FIELD_U8, gDebugFieldTypes[DEBUG_FIELD_U8].size, 100, 0, "m_nClosestPlayers[0]");
        cache->AddArrayField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, 100,
            (u8*)mDistances - (u8*)this, "m_fCachedPlayerDistances[0]");
        cache->AddArrayField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, 10,
            (u8*)mBallDistances - (u8*)this, "m_fCachedBallPlayerDistances[0]");
        cache->EndType();
    }
}

inline void cGame::RegisterDetermGameFields(DebugWriteCache* cache)
{
    if (gGameStateSyncType == 0xFFFF)
    {
        gGameStateSyncType = cache->BeginType("DetermGameData");
        cache->AddField(DEBUG_FIELD_ENUM, gDebugFieldTypes[DEBUG_FIELD_ENUM].size, 0, "m_eGameState");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&m_fGameDuration - (u8*)&m_eGameState, "m_fGameDuration");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&m_bBallInNet - (u8*)&m_eGameState, "m_bBallInNet");
        cache->AddField(DEBUG_FIELD_INT, gDebugFieldTypes[DEBUG_FIELD_INT].size, (u8*)&m_nLastTeamToScore - (u8*)&m_eGameState, "m_nLastTeamToScore");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&m_uMegastrikeNumShots - (u8*)&m_eGameState, "m_uMegastrikeNumShots");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&m_uMegastrikeCurShot - (u8*)&m_eGameState, "m_uMegastrikeCurShot");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&m_uMegastrikeGoals - (u8*)&m_eGameState, "m_uMegastrikeGoals");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&m_nMegastrikeDefendingTeam - (u8*)&m_eGameState, "m_uMegastrikeDefendingTeam");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&m_uMegastrikeResults - (u8*)&m_eGameState, "m_uMegastrikeResults");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mbCaptainShotToScoreOn - (u8*)&m_eGameState, "mbMegaStrikeGameplay");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mbMegaStrikePositiveNet - (u8*)&m_eGameState, "mbMegaStrikePositiveNet");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mbMegaStrikePlayerReady - (u8*)&m_eGameState, "mbMegaStrikePlayerReady");
        cache->AddField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, (u8*)&m_pScorer - (u8*)&m_eGameState, "m_pScorer");
        cache->AddField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, (u8*)&m_pAssister - (u8*)&m_eGameState, "m_pAssister");
        cache->AddArrayField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, 2, (u8*)m_pTeamTouch - (u8*)&m_eGameState, "m_pTeamTouch");
        cache->AddArrayField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, 10, (u8*)m_pRandomPlayersArray - (u8*)&m_eGameState, "m_pRandomPlayersArray");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfTiltLevelTimer - (u8*)&m_eGameState, "mfCheatTilt");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfXTilt - (u8*)&m_eGameState, "mfXTilt");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfYTilt - (u8*)&m_eGameState, "mfYTilt");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfXTiltMin - (u8*)&m_eGameState, "mfXTiltMin");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfXTiltMax - (u8*)&m_eGameState, "mfXTiltMax");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfYTiltMin - (u8*)&m_eGameState, "mfYTiltMin");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfYTiltMax - (u8*)&m_eGameState, "mfYTiltMax");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfTiltSpeed - (u8*)&m_eGameState, "mfTiltSpeed");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mfDesiredTiltSpeed - (u8*)&m_eGameState, "mfDesiredTiltSpeed");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified0A0 - (u8*)&m_eGameState, "mfTiltTime");
        cache->AddField(DEBUG_FIELD_ANGLE, gDebugFieldTypes[DEBUG_FIELD_ANGLE].size, (u8*)&maTiltDir - (u8*)&m_eGameState, "maTiltDir");
        cache->AddField(DEBUG_FIELD_ANGLE, gDebugFieldTypes[DEBUG_FIELD_ANGLE].size, (u8*)&maDesiredTiltDir - (u8*)&m_eGameState, "maDesiredTiltDir");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&muTiltFrames - (u8*)&m_eGameState, "muTiltFrames");
        cache->AddField(DEBUG_FIELD_VECTOR3, gDebugFieldTypes[DEBUG_FIELD_VECTOR3].size, (u8*)&mTiltDirection - (u8*)&m_eGameState, "m_vUpVectorTilt");
        cache->EndType();
    }

}

void cGame::SyncLog(void* context, DebugWriteCache* cache)
{
    RegisterDetermGameFields(cache);

    // The log copy of m_eGameState..m_vUpVectorTilt stores player indices
    // instead of pointers.
    struct DetermGameDataCopy
    {
        u8 mUnidentified00[0x2C];
        int mScorer;
        int mAssister;
        int mTeamTouch[2];
        int mRandomPlayers[10];
        u8 mUnidentified64[0x3C];
    };

    DetermGameDataCopy* pCopy = (DetermGameDataCopy*)cache->WriteData(
        gGameStateSyncType, &m_eGameState, sizeof(DetermGameDataCopy));
    if (pCopy != 0)
    {
        pCopy->mScorer = GetSyncPlayerIndex(m_pScorer);
        pCopy->mAssister = GetSyncPlayerIndex(m_pAssister);
        pCopy->mTeamTouch[0] = GetSyncPlayerIndex(m_pTeamTouch[0]);
        pCopy->mTeamTouch[1] = GetSyncPlayerIndex(m_pTeamTouch[1]);
        int* pRandomPlayers = pCopy->mRandomPlayers;
        for (RandomPlayerIterator players(this); players.HasNext(); players.Next())
        {
            cPlayer* pPlayer = players.GetPlayer();
            pRandomPlayers[players.GetIndex()] = pPlayer == 0 ? -1 : GetPlayerIndex(pPlayer);
        }
        cache->ChecksumData(gGameStateSyncType, pCopy, context);
    }

    PlayerDistanceSnapshot snapshot;
    CopyPlayerDistanceSnapshot(&snapshot);

    snapshot.RegisterDebugFields(cache);

    cache->ChecksumData(gPlayerDistanceSyncType, &snapshot, context);
    cache->WriteData(gPlayerDistanceSyncType, &snapshot, sizeof(snapshot));

    mpCrowdRiot->SyncLog(context, cache);
    mpTerrain->SyncLog(context, cache);
    mpWeatherManager->SyncLog(context, cache);
    lbl_806E12C8->SyncLog(context, cache);
    NetMesh::spPositiveXNetMesh->SyncLog(context, cache);
    NetMesh::spNegativeXNetMesh->SyncLog(context, cache);
}
void cGame::ChecksumState(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&m_eGameState, sizeof(m_eGameState));
    runningChecksum->ChecksumData(&m_bBallInNet, sizeof(m_bBallInNet));
    runningChecksum->ChecksumData(&m_nLastTeamToScore, sizeof(m_nLastTeamToScore));
}
void cGame::ChangeGameState(int state)
{
    DebugWriteCache* output = gNetworkSyncState->GetWriteCache();
    if (output != 0)
    {
        char buffer[256];
        int frame = GetFixedUpdateTask()->GetFrame();
        nlSNPrintf(
            buffer, sizeof(buffer), "Changing game state %d to %d at frame %d\n", m_eGameState, state, frame);
        output->WriteText(buffer);
        tDebugPrintManager::Print(DC_NETWORK, buffer);
        if (FormatNetworkCallStack(6, buffer, sizeof(buffer)) != 0)
        {
            output->WriteText(buffer);
        }
    }

    if (state != m_eGameState)
    {
        if (m_eGameState == 6 && state == 3)
        {
            StopSuddenDeathMusic();
        }

        if (state == 3)
        {
            if ((GameInfoManager::Instance()->IsInMode4()
                    && g_pStrikerChallenge->mCondition == 2
                    && g_pTeams[1]->m_nScore > 0)
                || (g_pStrikerChallenge->mCurrentChallenge == 2
                    && g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore))
            {
                PlayCrowdReaction(0xEF3369E0);
            }
            else
            {
                cTeam* pTeam = g_pTeams[0];
                bool useAlternateMusic = false;
                if (pTeam->m_nScore - pTeam->GetOtherTeam()->m_nScore > 0
                    && GetStadiumUnknown0x10(
                        GameInfoManager::Instance()->GetStadium()))
                {
                    useAlternateMusic = true;
                }

                unsigned long soundID = 0xEF3369E0;
                if (useAlternateMusic)
                {
                    soundID = 0x1E859DCD;
                }
                PlayCrowdReaction(soundID);
            }
        }

        if (m_eGameState == 5)
        {
            unsigned long soundID = GetStadiumSoundID(
                GameInfoManager::Instance()->GetStadium());
            PauseSound(soundID, this);
        }

        InitGameState(state);
    }
}
void cGame::InitGameState(int state)
{
    if (m_eGameState == 5 && state == 6)
    {
        mUnidentified49C.mSuddenDeathEvent.Queue();
    }

    m_eGameState = state;
    switch (state)
    {
    case 1:
        if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            m_pGameClock->Stop();
        }
        ResetForKickOff();
        break;

    case 0:
        if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            m_pGameClock->Stop();
        }
        break;
    case 2:
        if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            m_pGameClock->Stop();
        }
        for (int i = 0; i < 2; i++)
        {
            cTeam* team = g_pTeams[i];
            for (int j = 0; j < 4; j++)
            {
                team->GetFielder(j)->EndBlur();
            }
        }
        break;

    case 3:
        if (!DuringMegaStrikeEndPresentation(GetPresentation()))
        {
            PlaySound(10, 0x42F55573, 0, 0);
        }
        m_pPostGameDoneClock->Start();
        if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            m_pGameClock->Stop();
        }
        for (int i = 0; i < 2; i++)
        {
            cTeam* team = g_pTeams[i];
            for (int j = 0; j < 4; j++)
            {
                cFielder* fielder = team->GetFielder(j);
                fielder->EndBlur();
                if (!fielder->IsShattered())
                {
                    fielder->mUnidentified178 = 1.0f;
                }
            }
        }
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_tNoPickupTimer.SetSeconds(0.0f);
        fn_80015B38(g_pBall, false);
        StopSound(GetStadiumSoundID(GameInfoManager::Instance()->GetStadium()), this);
        gpNumberDisplay->mExpanded = true;
        gpNumberDisplay->mHoldUntilKickoff = true;
        break;

    case 5:
    {
        unsigned long soundID = GetStadiumSoundID(GameInfoManager::Instance()->GetStadium());
        if (IsSoundTracked(soundID, this))
        {
            ResumeSound(soundID, this);
        }
        else
        {
            PlayTrackedOwnedSound(18, soundID, 0, "Gameplay Music", this, true);
        }
        break;
    }

    case 6:
        StopSound(GetStadiumSoundID(GameInfoManager::Instance()->GetStadium()), this);
        break;
    }

    if (state == 5 || state == 6)
    {
        if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            m_pGameClock->Start();
        }
        for (int i = 0; i < 2; i++)
        {
            cTeam* team = g_pTeams[i];
            for (int j = 0; j < 4; j++)
            {
                cFielder* fielder = team->GetFielder(j);
                if (fielder->fn_8002E060() == 31)
                {
                    fielder->EndDesire();
                }
            }
        }
    }
}
void cGame::LoadTerrain(int terrain)
{
    g_pGame->mpTerrain->Load(terrain);
}
void cGame::SetDifficulty(
    int diff0, int diff1, int diff2, bool param4)
{
    bool param5 = !param4;
    if (diff0 != -1)
    {
        g_pTeams[0]->SetDifficulty(diff0, param4, param5);
        param5 = false;
    }
    if (diff1 != -1)
    {
        g_pTeams[1]->SetDifficulty(diff1, param4, param5);
    }
}
void DeliverShotPresentationEvent(cGame* pGame)
{
    pGame->mUnidentified49C.mShotPresentationEvent.Deliver();
}
void DeliverShotPresentationEndEvent(cGame* pGame)
{
    pGame->mUnidentified49C.mShotPresentationEndEvent.Deliver();
}
void DeliverCaptainClashPresentationEvent(cGame* pGame)
{
    pGame->mUnidentified49C.mCaptainClashPresentationEvent.Deliver();
}
void DeliverWindupPresentationEvent(cGame* pGame)
{
    pGame->mUnidentified49C.mWindupPresentationEvent.Deliver();
}
void DeliverWindupPresentationEndEvent(cGame* pGame)
{
    pGame->mUnidentified49C.mWindupPresentationEndEvent.Deliver();
}
void cGame::SendPauseGameEvent()
{
    mUnidentified49C.mPauseGameEvent.Queue(Function<FnVoidVoid>());
}
void cGame::SendResumingGameEvent()
{
    mUnidentified49C.mResumingGameEvent.Queue(Function<FnVoidVoid>());
}
extern "C" void fn_8005D210(cGame* pGame, LightningStrikeData* pData)
{
    pGame->mUnidentified49C.mLightningStrikeEvent.Queue(
        pData, Function<LightningStrikeData*>(FreeLightningStrikeData));
}
void DeliverGoalieSaveEvent(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieSaveEvent.Deliver((GoalieSaveData*)pData);
}
void DeliverGoalieKickEvent(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieKickEvent.Deliver((GoalieSaveData*)pData);
}
extern "C" void fn_8005D74C(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieCatchEvent.Deliver((GoalieSaveData*)pData);
}
void DeliverGoalieExertEvent(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieExertEvent.Deliver((GoalieSaveData*)pData);
}
void cGame::SetMegaStrikeShotResult(int shotIndex, bool scored)
{
    if (scored)
    {
        m_uMegastrikeResults |= 1 << shotIndex;
    }
    else
    {
        m_uMegastrikeResults &= ~(1 << shotIndex);
    }
}
void FinishMegaStrike(cGame* pGame)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }

    unsigned int side = 1 - pGame->m_nMegastrikeDefendingTeam;
    cPlayer* pCaptain = g_pTeams[side]->GetCaptain();

    MegaStrikeEndData data;
    data.defendingSide = pGame->m_nMegastrikeDefendingTeam;
    data.goals = pGame->m_uMegastrikeGoals;
    data.attempts = pGame->m_uMegastrikeNumShots;
    data.unknown_08 = pGame->m_uMegastrikeResults;
    data.pPlayer = pCaptain;
    data.goalValue = -1;

    g_pBall->m_pLastTouch = g_pTeams[side]->GetCaptain();
    pGame->m_pTeamTouch[side] = pCaptain;
    pGame->m_pScorer = pCaptain;
    pGame->m_pAssister = 0;

    if (pGame->m_uMegastrikeGoals != 0)
    {
        pGame->m_nLastTeamToScore = side;
        g_pBall->m_uGoalType = 6;
        g_pTeams[side]->m_nScore += pGame->m_uMegastrikeGoals;
        int score;

        if (GameInfoManager::Instance()->IsInMode4()
            && g_pStrikerChallenge->mCondition == 2 && side == 1)
        {
            pGame->ChangeGameState(3);
        }
        else if (pGame->m_eGameState == 6)
        {
            pGame->ChangeGameState(3);
        }
        else if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 1
            && (score = g_pTeams[side]->m_nScore,
                score >= GameInfoManager::Instance()->GetCurrentSettings()->GoalLimit))
        {
            pGame->ChangeGameState(3);
        }
        else
        {
            pGame->ChangeGameState(2);
        }

        if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
        {
            unsigned long soundID = 0xC274C205;
            if (side == 0)
            {
                soundID = 0x04FC9F1C;
            }
            PlayCrowdReaction(soundID);
        }

        if (pCaptain->GetGlobalPad() != 0)
        {
            data.goalValue = pGame->m_pScorer->GetGlobalPad()->GetPadID();
        }
        Goalie::HandleGoalScored(side);
    }

    g_pGame->SetMegaStrikeGameplay(false, 0, 0);
    g_pGame->mUnidentified49C.mMegaStrikeEndEvent.Deliver(&data);
    g_pBall->m_uGoalType = 4;
    SetPlayerAudioController(0);
}
void cGame::ResumeAfterPresentation()
{
    if (GetAudioPauseDepth() > 1)
    {
        ResumeAllAudio();
    }
    ResumeSuddenDeathMusic();

    static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible(OVERLAY_HUD, true, true);
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDIn(0.25f);
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene((SceneList)89))->DisplayNewScore();

    if (mpWeatherManager != 0)
    {
        SandTombWeather* weather = static_cast<SandTombWeather*>(mpWeatherManager->GetWeather(7));
        if (weather != 0)
        {
            weather->InvalidateSandPatches();
        }
    }
}
void cGame::QueueChainNisEnd(ShotAtGoalData* data)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_ShotAtGoalDataPool.Free(data);
        return;
    }
    mUnidentified49C.mChainNisEndEvent.Queue(
        data, Function<ShotAtGoalData*>(FreeShotAtGoalData));
}
void cGame::QueueNIS(NISData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_NISDataPool.Free(pData);
        return;
    }
    mUnidentified49C.mNISEvent.Queue(
        pData, Function<NISData*>(FreeNISData));
}
void QueueCollisionCrowdEvent(cGame* pGame, CollisionCrowdData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_CollisionCrowdDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mCollisionCrowdEvent.Queue(
        pData, Function<CollisionCrowdData*>(FreeCollisionCrowdData));
}
void DeliverGoalieDekeAttackAttemptEvent(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieDekeAttackAttemptEvent.Deliver((PlayerAttackData*)pData);
}
void DeliverGoalieDekeAttackSuccessEvent(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieDekeAttackSuccessEvent.Deliver((PlayerAttackData*)pData);
}
void DeliverGoalieSlamAttackAttemptEvent(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieSlamAttackAttemptEvent.Deliver((PlayerAttackData*)pData);
}
void DeliverGoalieSlamAttackSuccessEvent(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mGoalieSlamAttackSuccessEvent.Deliver((PlayerAttackData*)pData);
}
void QueueAttackAttemptEvent(cGame* pGame, PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_PlayerAttackDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mAttackAttemptEvent.Queue(
        pData, Function<PlayerAttackData*>(FreePlayerAttackData));
}
extern "C" void fn_8005ED64(cGame* pGame, PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_PlayerAttackDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mAttackSuccessEvent.Queue(
        pData, Function<PlayerAttackData*>(FreePlayerAttackData));
}
extern "C" void fn_8005EED0(cGame* pGame, ShotAtGoalData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_ShotAtGoalDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mShotAtGoalEvent.Queue(
        pData, Function<ShotAtGoalData*>(FreeShotAtGoalData));
}
void DeliverWindupShotEvent(cGame* pGame, ShotAtGoalData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mWindupShotEvent.Deliver(pData);
}
void DeliverMegaStrikeMeterStartEvent(cGame* pGame, MegaStrikeMeterData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mMegaStrikeMeterStartEvent.Deliver(pData);
}
void DeliverMegaStrikeMeterFirstEvent(cGame* pGame, MegaStrikeMeterData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mMegaStrikeMeterFirstEvent.Deliver(pData);
}
void DeliverMegaStrikeMeterSecondEvent(cGame* pGame, MegaStrikeMeterData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mMegaStrikeMeterSecondEvent.Deliver(pData);
}
void DeliverMegaStrikeIntroEvent(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    PauseAllAudio();
    pGame->mUnidentified49C.mMegaStrikeIntroEvent.Deliver(pFielder);
}
void DeliverMegaStrikeMeterEndEvent(cGame* pGame)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mMegaStrikeMeterEndEvent.Deliver();
}
void DeliverPeachFlashEvent(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mPeachFlashEvent.Deliver(pEventData);
}
void DeliverPeachCameraFlashEvent(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mPeachCameraFlashEvent.Deliver(pEventData);
}
void DeliverPeachCamerasDownEvent(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mPeachCamerasDownEvent.Deliver(pEventData);
}
void DeliverPeachCamerasAwayEvent(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mPeachCamerasAwayEvent.Deliver(pEventData);
}
extern "C" void fn_8006040C(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mWaluigiWallStartEvent.Deliver(pFielder);
}
extern "C" void fn_80060608(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mWaluigiWallEndEvent.Deliver(pFielder);
}
extern "C" void fn_80060804(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mWaluigiWallAbortEvent.Deliver(pFielder);
}
extern "C" void fn_80060A00(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mSuperPresentationEvent.Deliver(pFielder);
}
void cGame::fn_80060BFC(CollisionBulletBillData& data)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    mUnidentified49C.mBulletBillExplodeEvent.Deliver(&data);
}
void DeliverMontyReappearEvent(cGame* pGame, const CharacterImpactEvent* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mMontyReappearEvent.Deliver((CharacterImpactEvent*)pEventData);
}
extern "C" void fn_80060FF4(cGame* pGame, const CharacterImpactEvent* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mHammerBroHammerEvent.Deliver((CharacterImpactEvent*)pEventData);
}
extern "C" void fn_800611F0(cGame* pGame, const void* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mWarioGroundPoundEvent.Deliver((CharacterImpactEvent*)pEventData);
}
GameEventQueue::GameEventQueue()
    : mPauseGameEvent(GetGameEventDispatcher(), "PauseGame", -1)
    , mResumingGameEvent(GetGameEventDispatcher(), "ResumingGame", -1)
    , mGameOverEvent(GetGameEventDispatcher(), "GameOver", -1)
    , mGameIsWonEvent(GetGameEventDispatcher(), "GameIsWon", -1)
    , mPresentationBypassEvent(GetGameEventDispatcher(), "PresentationBypass", -1)
    , mNISEvent(GetGameEventDispatcher(), "NIS", -1)
    , mGoalScoredEvent("GoalScored", -1)
    , mEnterStartScreenEvent(GetFixedUpdateEventDispatcher(), "EnterStartScreen", -1)
    , mDirectionBeginEvent(GetFixedUpdateEventDispatcher(), "DirectionBegin", -1)
    , mCharacterDirectionEndEvent("CharacterDirectionEnd", -1)
    , mResetEffectsEvent("ResetEffects", -1)
    , mGetReadyForKickoffEvent(GetFixedUpdateEventDispatcher(), "GetReadyForKickoff", -1)
    , mKickoffEvent(GetFixedUpdateEventDispatcher(), "Kickoff", -1)
    , mSuddenDeathEvent(GetFixedUpdateEventDispatcher(), "SuddenDeath", -1)
    , mBallStateChangeEvent("BallStateChange", -1)
    , mReceiveBallEvent("ReceiveBall", -1)
    , mPassBallEvent("PassBall", -1)
    , mGoalieSaveEvent("GoalieSave", -1)
    , mGoalieKickEvent("GoalieKick", -1)
    , mCollisionBallGoalieEvent("CollisionBallGoalie", -1)
    , mGoalieCatchEvent("GoalieCatch", -1)
    , mGoalieExertEvent("GoalieExert", -1)
    , mShotAtGoalEvent(GetFixedUpdateEventDispatcher(), "ShotAtGoal", -1)
    , mWindupShotEvent("WindupShot", -1)
    , mGoalieDekeAttackAttemptEvent("GoalieDekeAttackAttempt", -1)
    , mGoalieDekeAttackSuccessEvent("GoalieDekeAttackSuccess", -1)
    , mGoalieSlamAttackAttemptEvent("GoalieSlamAttackAttempt", -1)
    , mGoalieSlamAttackSuccessEvent("GoalieSlamAttackSuccess", -1)
    , mAttackAttemptEvent(GetFixedUpdateEventDispatcher(), "AttackAttempt", -1)
    , mAttackSuccessEvent(GetFixedUpdateEventDispatcher(), "AttackSuccess", -1)
    , mCharGetElectrocutedEvent(GetFixedUpdateEventDispatcher(), "CharGetElectrocuted", -1)
    , mEvent31(GetFixedUpdateEventDispatcher(), "PowerupStats", -1)
    , mCollisionCrowdEvent(GetFixedUpdateEventDispatcher(), "CollisionCrowd", -1)
    , mCollisionChainPlayerEvent(GetFixedUpdateEventDispatcher(), "CollisionChainPlayer", -1)
    , mCollisionWindDebrisPlayerEvent(GetFixedUpdateEventDispatcher(), "CollisionWindDebrisPlayer", -1)
    , mCollisionExplosionFragmentPlayerEvent(GetFixedUpdateEventDispatcher(), "CollisionExplosionFragmentPLayer", -1)
    , mChainNisStartEvent(GetFixedUpdateEventDispatcher(), "ChainNisStart", -1)
    , mChainNisEndEvent(GetFixedUpdateEventDispatcher(), "ChainNisEnd", -1)
    , mPenaltyEvent(GetFixedUpdateEventDispatcher(), "Penalty", -1)
    , mAwardPowerupStuffEvent(GetFixedUpdateEventDispatcher(), "AwardPowerupStuff", -1)
    , mMegaStrikeMeterStartEvent("MegaStrikeMeterStart", -1)
    , mMegaStrikeMeterFirstEvent("MegaStrikeMeterFirst", -1)
    , mMegaStrikeMeterSecondEvent("MegaStrikeMeterSecond", -1)
    , mMegaStrikeMeterEndEvent("MegaStrikeMeterEnd", -1)
    , mLightningStrikeEvent(GetFixedUpdateEventDispatcher(), "LightningStrike", -1)
    , mMegaStrikeStartEvent(GetFixedUpdateEventDispatcher(), "MegastrikeStart", -1)
    , mMegaStrikeIntroEvent("MegaStrikeIntro", -1)
    , mMegaStrikeEndEvent("MegastrikeEnd", -1)
    , mShotPresentationEvent("ShotPresentation", -1)
    , mShotPresentationEndEvent("ShotPresentationEnd", -1)
    , mCaptainClashPresentationEvent("CaptainClashPresentation", -1)
    , mCaptainClashPresentationEndEvent("CaptainClashPresentationEnd", -1)
    , mWindupPresentationEvent("WindupPresentation", -1)
    , mWindupPresentationEndEvent("WindupPresentationEnd", -1)
    , mPeachCameraFlashEvent("PeachCameraFlash", -1)
    , mPeachFlashEvent("PeachFlash", -1)
    , mPeachCamerasDownEvent("PeachCamerasDown", -1)
    , mPeachCamerasAwayEvent("PeachCamerasAway", -1)
    , mWaluigiWallStartEvent("WaluigiWallStart", -1)
    , mWaluigiWallEndEvent("WaluigiWallEnd", -1)
    , mWaluigiWallAbortEvent("WaluigiWallAbort", -1)
    , mWarioGasStartEvent("WarioGasStart", -1)
    , mWarioGasEndEvent("WarioGasEnd", -1)
    , mBulletBillExplodeEvent("BulletBillExplode", -1)
    , mSuperPresentationEvent("SuperPresentation", -1)
    , mStatsPowerupHitDataEvent(GetFixedUpdateEventDispatcher(), "StatsPowerupHitData", -1)
    , mCameraRumbleStartEvent(GetFixedUpdateEventDispatcher(), "CameraRumbleStart", -1)
    , mCameraRumbleEndEvent(GetFixedUpdateEventDispatcher(), "CameraRumbleEnd", -1)
    , mExplodableExplodeEvent(GetFixedUpdateEventDispatcher(), "ExplodableExplode", -1)
    , mExplodableExplosionEndEvent(GetFixedUpdateEventDispatcher(), "ExplodableExplosionEnd", -1)
    , mSilenceAllSoundsEvent(GetFixedUpdateEventDispatcher(), "SilenceAllSounds", -1)
    , mMontyReappearEvent("MontyReappear", -1)
    , mHammerBroHammerEvent("HammerBroHammer", -1)
    , mWarioGroundPoundEvent("WarioGroundPound", -1)
    , mPowerupAcquireEvent(GetFixedUpdateEventDispatcher(), "PowerupAquire", -1)
{
}

inline void cGame::RegisterEventListeners()
{
    UnidentifiedFindEvent<UnidentifiedEventNoData>(SuddenDeathEventName(), -1)
        ->Add(Function<FnVoidVoid>(BindMember(this, &cGame::OnSuddenDeath)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>(GameOverEventName(), -1)
        ->Add(Function<FnVoidVoid>(BindMember(this, &cGame::OnGameOver)), 0, -1);
}

void cGame::OnSuddenDeath()
{
    PlaySuddenDeathMusic();
}
void cGame::OnGameOver()
{
    lbl_806E12C8->ResetEffects();
    StopSuddenDeathMusic();
}
void SetFieldTilt(int relative, float xTilt, float yTilt)
{
    cGame* game = g_pGame;
    if (game != 0)
    {
        if (relative != 0)
        {
            xTilt += game->mfXTilt;
            yTilt += game->mfYTilt;
        }

        const float xLimit = gFieldTiltXLimit;
        xTilt = xTilt >= -xLimit ? xTilt : -xLimit;
        xTilt = xTilt <= xLimit ? xTilt : xLimit;
        const float yLimit = gFieldTiltYLimit;
        yTilt = yTilt >= -yLimit ? yTilt : -yLimit;
        yTilt = yTilt <= yLimit ? yTilt : yLimit;

        fn_8005B330(&game->mTiltDirection, -xTilt, -yTilt);

        g_pGame->mfXTilt = xTilt;
        g_pGame->mfYTilt = yTilt;
        if (!GameInfoManager::Instance()->IsRule0x4Equal4())
        {
            g_pGame->mfTiltLevelTimer = -0.01f;
        }
    }

    cCameraManager::SetWorldUpVectorTilt(-xTilt, -yTilt);
    if (g_pBall != 0 && g_pBall->m_pPhysicsBall != 0)
    {
        if (nlAbs(xTilt) > 0.01f || nlAbs(yTilt) > 0.01f)
        {
            nlVector3 tiltForce = { 0 };
            tiltForce.x = yTilt * gFieldTiltForceScale;
            tiltForce.y = xTilt * gFieldTiltForceScale;
            g_pBall->m_pPhysicsBall->mv3TiltForce = tiltForce;
            g_pBall->m_pPhysicsBall->mbUseTiltForce = true;
        }
        else
        {
            g_pBall->m_pPhysicsBall->mbUseTiltForce = false;
        }
    }
}
#include "NL/nlBind_impl.h"

void cGame::QueueCharacterElectrocuted(CollisionPlayerWallData* data)
{
    mUnidentified49C.mCharGetElectrocutedEvent.Queue(
        data, Function<CollisionPlayerWallData*>(FreeCollisionPlayerWallData));
}

#include "Game/GameEventCallbacks.inl"

#include "Game/EventBase.inl"

#include "NL/nlDLListContainer.inl"
