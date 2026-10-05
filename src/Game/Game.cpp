#include "Game/NetworkMessageRegistry.h"
#include "Game/DetInput.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/Game.h"
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
#include "Game/AI/AvoidableObject.h"
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
#include "Game/Audio/RegistryPools.h"
#include "Game/UnidentifiedStaticStorage.h"
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
#include "NL/nlBindMember.h"
#include "NL/nlFunction.inl"
#include "NL/nlConfig.h"
#include "NL/nlMain.h"
#include "NL/nlMath.h"
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

extern PowerupBase* g_pPowerups[];
extern "C" const nlVector3 lbl_804DBFE8;
extern "C" void fn_80015B38(cBall* pBall, bool bParam);
extern "C" bool fn_800167E8(cBall* pBall);
extern "C" void fn_800406B0(cFielder* pFielder, float numBalls, float accuracy);

struct UnidentifiedGameSnapshot
{
    u8 mPlayerIndices[100];
    float mDistances[100];
    float mUnidentified1F4[10];
};

struct UnidentifiedRegistrationNode
{
    UnidentifiedRegistrationNode* mNext;
};

extern "C" EventDispatcher* fn_800721C4();
extern "C" void fn_80072134(LightningStrikeData* node);
extern "C" void fn_8007214C(ShotAtGoalData* node);
extern "C" void fn_80072164(UnidentifiedRegistrationNode* node);
extern "C" void fn_8007217C(UnidentifiedRegistrationNode* node);
extern "C" void fn_80072194(PlayerAttackData* node);
extern "C" int GetAudioPauseDepth();
extern "C" void ResumeAllAudio();
extern "C" void fn_800EDC2C();
extern "C" void fn_801E999C(BaseSceneHandler* scene);
extern "C" void fn_80058ABC(unsigned long param1, unsigned long param2);
extern void PlaySuddenDeathMusic();
extern void StopSuddenDeathMusic();

// Tweak "Megastrike/Score differential": when positive, it overrides the
// mega strike shot count (fn_80058498).
TweakValueInt lbl_8056B9A0("Score differential", "Megastrike", -1, false);
extern "C" char lbl_804FB2F4[];
extern "C" char lbl_804FB318[];
extern "C" char lbl_804FB364[];
extern "C" char lbl_804FB390[];
extern "C" char lbl_804FB3C0[];
extern "C" char lbl_804FB404[];
extern "C" char lbl_804FB25C[];
extern "C" char lbl_804FB284[];
extern "C" char lbl_804FB294[];
extern "C" char lbl_804FB238[];
extern "C" char lbl_804FB060[];
extern "C" char lbl_804FB66C[];
extern "C" char lbl_804FB428[];
extern "C" char lbl_804FB454[];
extern "C" char lbl_804FB4A4[];
extern "C" char lbl_804FB4D0[];
extern "C" char lbl_804FB4FC[];
extern "C" char lbl_804FB544[];
extern "C" char lbl_804FB568[];
extern "C" char lbl_804FB598[];
extern "C" char lbl_804FB5C4[];
extern "C" char lbl_804FB604[];

extern "C" const float lbl_806E3798;
extern "C" const float lbl_806E379C;
extern "C" const float kGameTweakZero;
extern "C" const float lbl_806E374C;
extern "C" const float lbl_806E3740;
extern "C" const float lbl_806E3748;
extern "C" const float lbl_806E376C;
extern "C" const float lbl_806E3770;
extern "C" const float lbl_806E3774;
extern "C" const float lbl_806E3744;
extern "C" const float lbl_806E3750;
extern "C" const float lbl_806E3754;
extern "C" const float lbl_806E3758;
extern "C" const float lbl_806E375C;
extern "C" const float lbl_806E3760;
extern "C" const float lbl_806E3764;
extern "C" const float lbl_806E3768;
extern "C" const float lbl_806E3778;
extern "C" const float lbl_806E377C;
extern "C" const float lbl_806E3780;
extern "C" const float lbl_806E3784;
extern "C" const float lbl_806E3788;
extern "C" const float lbl_806E378C;

// Game tuning values (.sdata), in retail order.
// Countdown beeps played before the end of a match.
int lbl_806DBA68 = 5;
// Field tilt force scale (fn_80061B1C).
extern "C" float lbl_806DBA6C = 0.8f;
// Time the field takes to level out, and the hold before it starts.
extern "C" float lbl_806DBA70 = 3.0f;
extern "C" float lbl_806DBA74 = 3.0f;
// Weather tilt range scale on each axis.
extern "C" float lbl_806DBA78 = 0.5f;
extern "C" float lbl_806DBA7C = 8.0f;
extern "C" float lbl_806DBA80 = 4.0f;
extern "C" float lbl_806DBA84 = 10.0f;
// Weather tilt ramp-up period.
extern "C" float lbl_806DBA88 = 12.0f;
// Score-difference tilt: clamp and scale.
extern "C" float lbl_806DBA8C = 6.0f;
extern "C" float lbl_806DBA90 = 1.0f;
// Weather tilt target speed range.
extern "C" float lbl_806DBA94 = 12.0f;
extern "C" float lbl_806DBA98 = 1.0f;
// Weather tilt direction jitter.
extern "C" int lbl_806DBA9C = 1000;
// Seek speeds: weather tilt, levelling during shoot-to-score, score tilt.
extern "C" float lbl_806DBAA0 = 1.0f;
extern "C" float lbl_806DBAA4 = 2.0f;
extern "C" float lbl_806DBAA8 = 2.0f;
extern "C" int lbl_806DBAAC = 3;
// Sync log type ids, registered on first use (fn_8005B840).
extern "C" u16 lbl_806DBAB2 = 0xFFFF;
extern "C" u16 lbl_806DBAB4 = 0xFFFF;

// .sbss, in retail order. The first two are debug overrides that force the
// weather and score field tilt on.
extern "C" {
bool lbl_806E0C90;
bool lbl_806E0C91;
}
cGame* g_pGame;
bool lbl_806E0C98;
cPlayer* lbl_806E0C9C;

static inline int GetUnidentifiedPlayerIndex(cPlayer* pPlayer)
{
    return pPlayer->mUnidentified120;
}

float fn_80056CA4()
{
    return 1000.0f * GetFixedUpdateTask()->mSimulationTime;
}

float fn_80056CD0()
{
    return nlTicksToMilliseconds(nlGetTicker());
}

void fn_80056CF4(void* param1, int param2, bool param3)
{
    ++lbl_806E2130;

    cGame* game = new (nlMalloc(0x10F4, 8, false)) cGame(param1, param2, param3);
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

inline void cGame::ResetGameFields()
{
    mUnidentified020 = false;
    m_nLastTeamToScore = 1;
    mUnidentified028 = 0;
    mUnidentified02C = 0;
    mUnidentified030 = 0;
    mUnidentified034 = 0;
    mUnidentified038 = 0;
    mUnidentified03C = 0;
    mbCaptainShotToScoreOn = false;
    mUnidentified041 = false;
    mUnidentified042 = false;
    m_pScorer = 0;
    m_pAssister = 0;
    m_pTeamTouch[1] = 0;
    m_pTeamTouch[0] = 0;
    for (int i = 0; i < 10; i++)
    {
        m_pRandomPlayersArray[i] = 0;
    }
    mUnidentified07C = kGameTweakZero;
    mUnidentified080 = kGameTweakZero;
    mUnidentified084 = kGameTweakZero;
    mUnidentified088 = lbl_806E3740;
    mUnidentified08C = lbl_806E3748;
    mUnidentified090 = lbl_806E3740;
    mUnidentified094 = lbl_806E3748;
    mUnidentified098 = kGameTweakZero;
    mUnidentified09C = kGameTweakZero;
    mUnidentified0A0 = kGameTweakZero;
    mUnidentified0A4 = 0;
    mUnidentified0A6 = 0;
    mUnidentified0A8 = 0;
    float initialTilt = -kGameTweakZero;
    fn_8005B330(&mTiltDirection, initialTilt, initialTilt);
    mUnidentified0B8 = lbl_806DBA68;
    mUnidentified0BC = false;
    mUnidentified0BD = false;
}

inline void cGame::RegisterEventListeners()
{
    UnidentifiedFindEvent<UnidentifiedEventNoData>("SuddenDeath", -1)
        ->Add(Function<FnVoidVoid>(BindMember(this, &cGame::OnSuddenDeath)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)
        ->Add(Function<FnVoidVoid>(BindMember(this, &cGame::OnGameOver)), 0, -1);
}

cGame::cGame(void* param1, int param2, bool param3)
    : mUnidentified0C0((bool*)mUnidentified0D0, 0, 0, 100)
    , mUnidentified134((bool*)mUnidentified144, 0, 0, 16)
{
    mpTerrain = 0;
    mpWeatherManager = 0;
    mUnidentified10E0 = 0;
    m_eGameState = -1;

    m_pPostResetClock = new (nlMalloc(sizeof(Clock), 8, false))
        Clock(kGameTweakZero, lbl_806E3744, lbl_806E3748, 2, fn_80058ABC);
    m_pPostResetClock->m_uParam1 = (unsigned long)this;

    mpTerrain = new (nlMalloc(sizeof(Terrain), 8, false))
        Terrain((int)param1);

    mpWeatherManager = new (nlMalloc(sizeof(WeatherManager), 8, false)) WeatherManager();

    mpWeatherManager->Initialize(param2);

    mUnidentified10E0 = new (nlMalloc(sizeof(CrowdRiot), 8, false))
        CrowdRiot(param3);

    m_pFuzzyTweaks = new (nlMalloc(sizeof(FuzzyTweaks), 8, false))
        FuzzyTweaks("/ini/FuzzyTweaks.ini", "/Game/Fuzzy");
    gGameTweaks.m_pGameTweaks->fn_800756B4();

    ResetGameFields();
    mUnidentified0C0.mStart = 0;
    mUnidentified0C0.mSize = 0;
    mUnidentified134.mStart = 0;
    mUnidentified134.mSize = 0;

    m_fGameDuration = gGameTweaks.m_pGameTweaks->fGameDuration;
    m_pGameClock = new (nlMalloc(sizeof(Clock), 8, false))
        Clock(kGameTweakZero, lbl_806E3750, lbl_806E3748, 2, 0);
    m_pGameClock->Stop();
    m_pPostGameDoneClock = new (nlMalloc(sizeof(Clock), 8, false))
        Clock(kGameTweakZero, lbl_806E3754, lbl_806E3748, 2, 0);

    bool noClock = GetTweakBool("user/No Clock", false);
    lbl_806E0C98 = noClock;
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

    mUnidentified014 = new (nlMalloc(sizeof(AIContext), 8, false))
        AIContext(
            this, 0, new (nlMalloc(sizeof(FuzzyAIRuntime), 8, false))
                         FuzzyAIRuntime());
    gNetworkMessageRegistry->RegisterReceiver(34, this);
    gNetworkMessageRegistry->RegisterReceiver(35, this);

    float avoidableWidth = lbl_806E3748;
    nlVector3 avoidableCenter = { 20.6f, 0.0f, 0.0f };
    mUnidentified10E4[0] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, avoidableWidth, lbl_806E3758);
    avoidableCenter.x *= lbl_806E3740;
    mUnidentified10E4[1] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, avoidableWidth, lbl_806E3758);

    if (GameInfoManager::Instance()->GetStadium() == 15)
        avoidableWidth = lbl_806E375C;
    else if (GameInfoManager::Instance()->GetStadium() == 11)
        avoidableWidth = lbl_806E3760;

    avoidableCenter.x = kGameTweakZero;
    avoidableCenter.y = lbl_806E3764;
    avoidableCenter.z = kGameTweakZero;
    mUnidentified10E4[2] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, lbl_806E3768, avoidableWidth);
    avoidableCenter.y *= lbl_806E3740;
    mUnidentified10E4[3] = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(1, avoidableCenter, lbl_806E3768, avoidableWidth);
}

cGame::~cGame()
{
    StopSuddenDeathMusic();

    delete m_pPostResetClock;
    delete m_pGameClock;

    delete mpTerrain;

    delete mpWeatherManager;

    delete mUnidentified10E0;

    delete m_pFuzzyTweaks;
    delete m_pPostGameDoneClock;

    mUnidentified014->Cleanup(true, true);
    delete mUnidentified014;

    gNetworkMessageRegistry->UnregisterReceiver(34);
    gNetworkMessageRegistry->UnregisterReceiver(35);

    for (int i = 0; i < 4; i++)
    {
        delete mUnidentified10E4[i];
    }

    if (lbl_806E0C74 != 0)
    {
        delete lbl_806E0C74;
        lbl_806E0C74 = 0;
    }

    gNextAvoidableObjectId = 0;
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
            g_pGame->m_fGameDuration, lbl_806E3740, 0);
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

void cGame::fn_80057FC0()
{
    mUnidentified0C0.mStart = 0;
    mUnidentified0C0.mSize = 0;
    mUnidentified134.mStart = 0;
    mUnidentified134.mSize = 0;
}

void cGame::fn_80057FD8(bool param1)
{
    u8* ptr;
    if (mUnidentified134.mSize >= mUnidentified134.mCapacity)
    {
        ptr = 0;
    }
    else
    {
        ptr = (u8*)mUnidentified134.mData
            + ((mUnidentified134.mStart + mUnidentified134.mSize)
                % mUnidentified134.mCapacity);
        mUnidentified134.mSize++;
    }
    *ptr = param1;

    if (mUnidentified134.mSize < lbl_806DBAAC)
    {
        return;
    }

    int count = mUnidentified134.mSize;
    NetworkMessageType35 message;
    message.mCount = count;
    for (int i = 0; i < count; i++)
    {
        message.mValues[i] = mUnidentified134.UnidentifiedRemoveStart();
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

void cGame::fn_80058180()
{
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB238, mUnidentified134.mSize);

    while (mUnidentified134.mSize > 0)
    {
        int count = mUnidentified134.mSize;
        if (count > 8)
        {
            count = 8;
        }

        NetworkMessageType35 message;
        message.mCount = count;
        for (int i = 0; i < count; i++)
        {
            message.mValues[i]
                = mUnidentified134.UnidentifiedRemoveStart();
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

void cGame::fn_8005830C()
{
    DebugWriteCache* output = gNetworkSyncState->GetWriteCache();
    if (output != 0)
    {
        char buffer[256];
        int frame = GetFixedUpdateTask()->GetFrame();
        nlSNPrintf(buffer, sizeof(buffer), lbl_804FB25C, frame);
        output->WriteText(buffer);
        tDebugPrintManager::Print(DC_NETWORK, buffer);
    }

    g_pBall->m_uGoalType = 6;

    float param3 = mUnidentified03C->mUnidentified394;
    float param2 = mUnidentified03C->mUnidentified390;
    Goalie* pGoalie = mUnidentified03C->m_pTeam->GetOtherTeam()->GetGoalie();
    pGoalie->InitActionMegaStrike(param2, param3);
    mUnidentified03C->EndAction();
    fn_80038158(mUnidentified03C, 0);
}

void cGame::fn_80058400()
{
    Goalie* pGoalie = mUnidentified03C->m_pTeam->GetOtherTeam()->GetGoalie();
    if (mUnidentified030 != 0)
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

void cGame::fn_8005848C()
{
    mUnidentified0BD = false;
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

void cGame::BeginGame(bool bRematch, bool bStraightToKickoff)
{
    ++lbl_806E2130;
    FixedUpdateTask::SetTimeScale(lbl_806E3748);
    ParticleUpdateTask::sInstance->SetTimeScale(lbl_806E3748);

    Function<DetermDataEvent*> callback(BindMember(this, &cGame::fn_8005A028));
    GetInputRouter();
    GetDetermDataEventQueue()->Add(callback, 0, -1);

    if (m_eGameState != 0)
    {
        ChangeGameState(0);
    }

    ResetGameFields();

    fn_80058498(false, 0, 0);
    fn_80059A1C();
    mpWeatherManager->Reset();
    mpWeatherManager->Stop(true);
    ResetCharacters();
    fn_8001847C(g_pBall, false);
    mUnidentified020 = false;
    ResetPowerups(true);
    EndPeachPhoto(&gPeachPhotoState, true);
    EmissionManager::Instance()->KillAll();
    BasicStadium::GetCurrentStadium()->ResetEffects();
    for (float elapsed = kGameTweakZero; elapsed < lbl_806E3760; elapsed += lbl_806E3774)
    {
        static_cast<World*>(BasicStadium::GetCurrentStadium())->Update(lbl_806E3774, true);
        EmissionManager::Instance()->Update(lbl_806E3774);
    }
    gNPCManager->ResetNPCs();

    m_pGameClock->Reset(kGameTweakZero, lbl_806E3750, lbl_806E3748);
    m_pGameClock->Stop();
    m_pPostGameDoneClock->Reset(kGameTweakZero, lbl_806E3754, lbl_806E3748);
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
        character->m_Dirt = kGameTweakZero;
        character->mUnidentified16C = 0;
        g_pCharacters[i]->m_MinDirt = kGameTweakZero;
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
        SetScriptTimeBudget(lbl_806E3740);
        gAIProfilingClock = fn_80056CA4;
        gAIActivityClock = fn_80056CA4;
    }
    else
    {
        SetScriptTimeBudget(lbl_806E3748);
        gAIProfilingClock = fn_80056CD0;
        gAIActivityClock = fn_80056CA4;
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

    if (g_pBall->GetInNet(nSide) && !mUnidentified020)
    {
        nSide = (nSide + 1) % 2;
        m_nLastTeamToScore = nSide;
        g_pTeams[nSide]->m_nScore += 1;
        int score;

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
            && (score = g_pTeams[nSide]->m_nScore,
                score >= GameInfoManager::Instance()->GetCurrentSettings()->GoalLimit))
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
            if (fDirection >= kGameTweakZero)
            {
                g_pBall->m_uGoalType = 5;
            }
            else
            {
                g_pBall->m_uGoalType = 3;
            }

            cPlayer* pPlayer = g_pBall->m_pLastTouch;
            if (m_pScorer != NULL && pPlayer != NULL && m_pScorer != pPlayer
                && m_pScorer->IsOnSameTeam(pPlayer))
            {
                m_pAssister = m_pScorer;
            }
            else
            {
                m_pAssister = NULL;
            }

            m_pScorer = pPlayer;

            if (pPlayer != NULL && pPlayer->m_eClassType == FIELDER)
            {
                m_pTeamTouch[pPlayer->m_pTeam->m_nSide] = pPlayer;
            }
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
        mUnidentified020 = true;

        cGame* game = g_pGame;
        if (game->m_eGameState != 4)
        {
            game->mUnidentified49C.mEvent06.Deliver(&goalScored.data);
        }

        if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
        {
            unsigned long soundID;
            if (g_pTeams[nSide]->m_nScore - g_pTeams[nSide]->GetOtherTeam()->m_nScore == 0)
            {
                soundID = 0x9D796D7D;
            }
            else if (g_pTeams[nSide]->m_nScore - g_pTeams[nSide]->GetOtherTeam()->m_nScore == 1)
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
    }
}

void cGame::fn_80058498(bool param1, int param2, int param3)
{
    mbCaptainShotToScoreOn = param1;
    if (param1)
    {
        mUnidentified041 = g_pTeams[param2]->m_pNet->m_v3NetLocation.x > 0.0f;
        if (lbl_8056B9A0.value > 0)
        {
            mUnidentified028 = lbl_8056B9A0.value;
        }
        else
        {
            mUnidentified028 = param3;
        }
        mUnidentified034 = param2;
    }
    else
    {
        mUnidentified028 = 0;
        mUnidentified0C0.mStart = 0;
        mUnidentified0C0.mSize = 0;
        mUnidentified134.mStart = 0;
        mUnidentified134.mSize = 0;
    }

    mUnidentified02C = 0;
    mUnidentified030 = 0;
    mUnidentified042 = false;
    mUnidentified0BC = false;
}

void cGame::fn_80058528(float timeScale, float transitionTime)
{
    if (g_pNetworkSessionBase->GetNumMachines() > 1 && timeScale < lbl_806E376C)
    {
        timeScale = lbl_806E376C;
    }

    if (FixedUpdateTask::GetTargetTimeScale() != lbl_806E3748 || lbl_806E3748 != timeScale)
    {
        if (g_pGame->m_eGameState != 4)
        {
            if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
            {
                m_pGameClock->Stop();
            }

            if (FixedUpdateTask::GetTargetTimeScale() == lbl_806E3748)
            {
                unsigned long soundID = 0xCE5CBAC7;
                StopSound(soundID, g_pGame);
                PlaySound(10, soundID, lbl_804FB284, g_pGame);

                u32 hash = nlStringLowerHash(lbl_804FB294);
                ApplyAudioTransition(&hash, 0, 0);
            }

            g_pOverlayManager->GetScene((SceneList)89)->SetVisible(false);
            gpNumberDisplay->mVisible = false;

            if (transitionTime <= kGameTweakZero)
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

void cGame::fn_80058748()
{
    ++lbl_806E2130;
    if (mUnidentified0BD)
    {
        fn_80058400();
        mUnidentified0BD = false;
    }
    mUnidentified49C.mEvent11.Queue();

    fn_80061B1C(0, 0.0f, 0.0f);
    gNPCManager->ResetNPCs();
    ResetCharacters();

    fn_8001847C(g_pBall, false);
    mUnidentified020 = false;
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
    m_pPostResetClock->Reset(0.0f, seconds, lbl_806E3748);
    m_pPostResetClock->Start();
}

extern "C" void fn_80058ABC(unsigned long, unsigned long)
{
    cGame* game = g_pGame;
    game->fn_8005DF38();
    game->mUnidentified49C.mEvent12.Queue();

    GameplayCamera* camera = cCameraManager::GetCamera<GameplayCamera>(eCameraType_Gameplay);
    if (camera != 0)
    {
        camera->SetForceNeutralAndNearZoom(false);
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

void cGame::fn_80059A1C()
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

void cGame::fn_80059B70(void* param1)
{
    UnidentifiedGameSnapshot* snapshot = static_cast<UnidentifiedGameSnapshot*>(param1);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 5; k++)
            {
                cPlayer* pPlayer = m_nClosestPlayers[i][j][k];
                snapshot->mPlayerIndices[i * 10 + j * 5 + k] = pPlayer == 0 ? -1 : GetUnidentifiedPlayerIndex(pPlayer);
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
        snapshot->mUnidentified1F4[i]
            = m_fCachedBallPlayerDistances[i];
    }
}

void cGame::SendNISLoadedCustomDeterm(u8 param1)
{
    struct Message
    {
        u8 type;
        u8 param1;
    } message;

    message.type = 29;
    message.param1 = param1;

    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB2F4, message.param1, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}

void cGame::fn_80059DEC(
    int param1, int param2, float param3, float param4)
{
    struct Message
    {
        u8 type;
        u8 param1;
        u8 param2;
        u8 padding;
        float param3;
        float param4;
    } message;

    message.type = 181;
    message.param1 = param1;
    message.param2 = param2;
    message.padding = 0;
    message.param3 = param3;
    message.param4 = param4;

    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB318, message.param1, message.param2, message.param3, message.param4, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}

void cGame::fn_80059E78()
{
    u8 message = 183;
    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB364, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}

void cGame::fn_80059EDC()
{
    u8 message = 185;
    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB390, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}

void cGame::fn_80059F40(unsigned int param1, unsigned int param2, float param3)
{
    struct Message
    {
        u8 type;
        u8 param1;
        u8 param2;
        u8 padding;
        float param3;
    } message;

    message.type = 182;
    message.param1 = param1;
    message.param2 = param2;
    message.padding = 0;
    message.param3 = param3;

    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB3C0, message.param1, message.param2, message.param3, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}

void cGame::fn_80059FC4()
{
    u8 message = 222;
    u32 frame = gInputManager->mFrameProvider->GetFrame();
    tDebugPrintManager::Print(DC_NETWORK, lbl_804FB404, frame);
    GetInputRouter()->QueueDetermData(&message, sizeof(message));
}

// Receives the custom deterministic data queued by the Send* helpers above.
void cGame::fn_8005A028(DetermDataEvent* pEvent)
{
    u8 type = pEvent->mData[0];

    DebugWriteCache* output = gNetworkSyncState->GetWriteCache();
    if (output != 0)
    {
        char buffer[256];
        nlSNPrintf(buffer, sizeof(buffer), lbl_804FB428, type, pEvent->mSize,
            GetFixedUpdateTask()->GetFrame());
        output->WriteText(buffer);
    }

    switch (type)
    {
    case 5:
    {
        // Which players are on screen, one bit per player.
        for (int i = 0; i < 2; i++)
        {
            cTeam* pTeam = g_pTeams[i];
            u8 flags = pEvent->mData[1 + i];
            for (int j = 0; j < 5; j++)
            {
                pTeam->GetPlayer(j)->mUnidentified024.m_bOnScreen = (flags & (u8)(1 << j)) != 0;
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
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB454, pEvent->mData[1],
            pEvent->mData[2], *(float*)&pEvent->mData[4], *(float*)&pEvent->mData[8],
            gInputManager->mFrameProvider->GetFrame());
        fn_800406B0(pFielder, *(float*)&pEvent->mData[4], *(float*)&pEvent->mData[8]);
        break;
    }

    case 183:
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB4A4,
            gInputManager->mFrameProvider->GetFrame());
        mUnidentified042 = true;
        break;

    case 185:
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB4D0,
            gInputManager->mFrameProvider->GetFrame());
        ResetMegaBallPointer();
        break;

    case 182:
    {
        Goalie* pGoalie = g_pTeams[pEvent->mData[1]]->GetGoalie();
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB4FC, pEvent->mData[1],
            pEvent->mData[2], *(float*)&pEvent->mData[4],
            gInputManager->mFrameProvider->GetFrame());
        pGoalie->fn_80084D70(pEvent->mData[2], *(float*)&pEvent->mData[4]);
        break;
    }

    case 222:
        // SlowDownEnd: back to full speed after a captain hit.
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB544,
            gInputManager->mFrameProvider->GetFrame());
        if (g_pGame->m_eGameState != 4)
        {
            StopSound(0xCE5CBAC7, g_pGame);
            if (FixedUpdateTask::GetTargetTimeScale() < lbl_806E3748)
            {
                FixedUpdateTask::SetTimeScale(lbl_806E3748);
                ParticleUpdateTask::sInstance->SetTimeScale(lbl_806E3748);
                if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
                {
                    m_pGameClock->Start();
                }

                PlaySound(10, 0x54A8A6A0, 0, 0);
                u32 hash = nlStringLowerHash(lbl_804FB294);
                ApplyAudioTransition(&hash, true, 0);
                g_pOverlayManager->GetScene((SceneList)89)->SetVisible(true);
                gpNumberDisplay->mVisible = true;
            }
        }
        break;

    case 188:
    {
        // CleanupMegastrikeGameplay
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB568,
            gInputManager->mFrameProvider->GetFrame());
        Goalie* pGoalie = mUnidentified03C->m_pTeam->GetOtherTeam()->GetGoalie();
        if (mUnidentified030 != 0)
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
        break;
    }

    default:
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB598, type);
        break;
    }
}

int cGame::ProcessMessage(NetworkMessage* message)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    s8 machine = roster->MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK, lbl_804FB5C4,
            (u8)message->GetType(), message->mSource);
        return 1;
    }

    if (NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        s8 tournamentIndex = NetTournManager::Instance()->TournamentIdxToMachineIdx(machine);
        if (tournamentIndex < 0 || tournamentIndex >= g_pNetworkSessionBase->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK, lbl_804FB604,
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
            u8* ptr;
            if (mUnidentified0C0.mCapacity <= mUnidentified0C0.mSize)
            {
                ptr = 0;
            }
            else
            {
                ptr = (u8*)mUnidentified0C0.mData
                    + ((mUnidentified0C0.mStart + mUnidentified0C0.mSize++)
                        % mUnidentified0C0.mCapacity);
            }
            *ptr = pMessage->mValues[i];
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

extern "C" void fn_8005A7E8()
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

// cGame::Update: one fixed-update step of the match.
void cGame::fn_8005A8FC(float fDeltaT)
{
    if (m_eGameState == 4)
    {
        return;
    }

    AIPadManager::UpdateAccelerationHistory();
    fn_8005A7E8();
    fn_8005B508();
    UpdateShockwaves(fDeltaT);

    // Retail calls this here and drops the result.
    IsNetworkOrRecordedGame();

    // Time is up once fewer than a tenth of a second remain.
    if ((unsigned int)(lbl_806E3778 * (m_fGameDuration - m_pGameClock->m_fTimer)) == 0
        && !fn_800167E8(g_pBall))
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

    mUnidentified014->Update(true, fDeltaT);

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
        if (mUnidentified0B8 != 0)
        {
            float fBeepTime = lbl_806E377C - (float)(int)(lbl_806DBA68 - mUnidentified0B8);
            if (m_fGameDuration - m_pGameClock->m_fTimer < fBeepTime)
            {
                mUnidentified0B8--;
                PlaySound(15, 0x97E84AE4, 0, 0);
            }
        }
    }

    for (int i = 0; i < 25; i++)
    {
        if (g_pPowerups[i] != 0)
        {
            g_pPowerups[i]->Update(fDeltaT);
        }
    }

    // Field tilt.
    int homeCaptainAction = g_pTeams[0]->GetCaptain()->m_eActionState;
    int awayCaptainAction = g_pTeams[1]->GetCaptain()->m_eActionState;

    if (mbCaptainShotToScoreOn || homeCaptainAction == 12 || homeCaptainAction == 11
        || awayCaptainAction == 11 || awayCaptainAction == 12)
    {
        // Level the field out while a captain shoots to score.
        mUnidentified0A4 = 0;
        mUnidentified07C = kGameTweakZero;
        mUnidentified098 = kGameTweakZero;
        mUnidentified09C = kGameTweakZero;
        mUnidentified0A6 = 0;
        mUnidentified0A8 = 0;
        mUnidentified080 = cCharacter::SeekSpeedExponential(
            mUnidentified080, kGameTweakZero, lbl_806DBAA4, fDeltaT);
        mUnidentified084 = cCharacter::SeekSpeedExponential(
            mUnidentified084, kGameTweakZero, lbl_806DBAA4, fDeltaT);
        fn_80061B1C(0, mUnidentified080, mUnidentified084);
    }
    else if (GameInfoManager::Instance()->IsRule0x4Equal4() || lbl_806E0C91)
    {
        // The field tilts towards the side that is behind.
        float fMax = lbl_806DBA8C;
        float fDiff = (float)(g_pTeams[1]->m_nScore - g_pTeams[0]->m_nScore);
        fDiff = (fDiff >= -fMax) ? fDiff : -fMax;
        fDiff = (fDiff <= fMax) ? fDiff : fMax;

        mUnidentified080 = cCharacter::SeekSpeedExponential(
            mUnidentified080, kGameTweakZero, lbl_806DBAA8, fDeltaT);
        mUnidentified084 = cCharacter::SeekSpeedExponential(
            mUnidentified084, fDiff * lbl_806DBA90, lbl_806DBAA8, fDeltaT);
        fn_80061B1C(0, mUnidentified080, mUnidentified084);
    }
    else if ((GameInfoManager::Instance()->GetStadium() == 9 || lbl_806E0C90)
        && !GetConfigBool(Config::Global(), "no_weather", false)
        && !GameInfoManager::Instance()->IsRule0x4Equal1())
    {
        // Weather tilt: wander towards random targets, ramping up over time.
        mUnidentified0A0 += fDeltaT;
        if (mUnidentified0A0 > lbl_806DBA88)
        {
            mUnidentified0A0 = kGameTweakZero;
        }

        if (++mUnidentified0A8 >= 15)
        {
            mUnidentified0A8 = 0;

            float fRamp = lbl_806E3748;
            float fFraction = mUnidentified0A0 / lbl_806DBA88;
            fRamp = (fRamp <= fFraction) ? fRamp : fFraction;

            float fHalfRamp = lbl_806E3744 * fRamp;
            float fRangeX = fHalfRamp * lbl_806DBA78;
            float fRangeY = fHalfRamp * lbl_806DBA7C;

            if (fRangeX > kGameTweakZero)
            {
                mUnidentified088 = -fRangeX - nlRandomf(fRangeX);
                mUnidentified08C = fRangeX + nlRandomf(fRangeX);
            }
            else
            {
                mUnidentified088 = kGameTweakZero;
                mUnidentified08C = kGameTweakZero;
            }

            mUnidentified090 = -fRangeY - nlRandomf(fRangeY);
            mUnidentified094 = fRangeY + nlRandomf(fRangeY);
            mUnidentified09C = nlRandomf(Interpolate(lbl_806DBA94, lbl_806DBA98, fRamp));
            mUnidentified0A6 = mUnidentified0A4 + nlRandom(lbl_806DBA9C) - lbl_806DBA9C / 2;
        }

        // Turn back once the tilt leaves its range.
        if (mUnidentified080 < mUnidentified088 || mUnidentified080 > mUnidentified08C)
        {
            u16 aAway = (mUnidentified080 < kGameTweakZero) ? 0 : 0x8000;
            s16 aDiff = mUnidentified0A6 - aAway;
            if (aDiff < 0)
            {
                aDiff = -aDiff;
            }
            if ((u16)aDiff > 0x4000)
            {
                mUnidentified0A6 = 0x8000 - mUnidentified0A6;
            }
            mUnidentified09C = lbl_806E3780 * lbl_806DBA98;
        }

        if (mUnidentified084 < mUnidentified090 || mUnidentified084 > mUnidentified094)
        {
            u16 aAway = (mUnidentified084 < kGameTweakZero) ? 0x4000 : 0xC000;
            s16 aDiff = mUnidentified0A6 - aAway;
            if (aDiff < 0)
            {
                aDiff = -aDiff;
            }
            if ((u16)aDiff > 0x4000)
            {
                mUnidentified0A6 = -mUnidentified0A6;
            }
            mUnidentified09C = lbl_806E3780 * lbl_806DBA98;
        }

        mUnidentified0A4 = SeekDirection(
            mUnidentified0A4, mUnidentified0A6, lbl_806E3784, lbl_806E3788, fDeltaT);
        mUnidentified098 = cCharacter::SeekSpeedExponential(
            mUnidentified098, mUnidentified09C, lbl_806DBAA0, fDeltaT);

        float fDX;
        float fDY;
        nlPolarToCartesian(fDX, fDY, mUnidentified0A4, mUnidentified098);
        mUnidentified080 = fDX * fDeltaT + mUnidentified080;
        mUnidentified084 = fDY * fDeltaT + mUnidentified084;
        fn_80061B1C(0, mUnidentified080, mUnidentified084);
    }
    else if (fabsf(mUnidentified080) > lbl_806E378C || fabsf(mUnidentified084) > lbl_806E378C)
    {
        // No tilt source: hold, then level the field out.
        float fTimer = mUnidentified07C;
        float fZero = kGameTweakZero;
        if (fTimer < fZero)
        {
            mUnidentified07C = fTimer - fDeltaT;
            if (mUnidentified07C < -lbl_806DBA74)
            {
                mUnidentified07C = fZero;
            }
        }
        else
        {
            float fNewTimer = fTimer + fDeltaT;
            float fX;
            float fY = fZero;
            if (fNewTimer >= lbl_806DBA70 - fDeltaT)
            {
                fNewTimer = fZero;
                fX = fZero;
            }
            else
            {
                float fStep = fDeltaT / (lbl_806DBA70 - fNewTimer);
                fStep = (fStep >= fZero) ? fStep : fZero;
                fStep = (fStep <= lbl_806E3748) ? fStep : lbl_806E3748;
                fX = Interpolate(mUnidentified080, kGameTweakZero, fStep);
                fY = Interpolate(mUnidentified084, kGameTweakZero, fStep);
            }
            fn_80061B1C(0, fX, fY);
            mUnidentified07C = fNewTimer;
        }
    }

    // EnterPostGame
    if (m_pPostGameDoneClock->m_clockState == CLOCK_DONE)
    {
        m_pPostGameDoneClock->Reset(kGameTweakZero, lbl_806E3754, lbl_806E3748);

        int nWinner = g_pTeams[1]->m_nScore > g_pTeams[0]->m_nScore;
        if ((GameInfoManager::Instance()->IsInMode4()
                && g_pStrikerChallenge->mCondition == 2
                && g_pTeams[1]->m_nScore > 0)
            || (g_pStrikerChallenge->mCurrentChallenge == 2
                && g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore))
        {
            NisPlayer::Instance()->mWinnerSide[0] = 1;
        }
        else
        {
            NisPlayer::Instance()->mWinnerSide[0] = nWinner;
        }

        UnidentifiedCameraEffects::Instance()->ResetForPresentation((void*)nWinner);
        if (!DuringEndOfGamePresentation(GetPresentation()))
        {
            GetPresentation()->PlayGoalEffects("Goal_endgame");
            GetPresentation()->Call("GameEndNoSuddenDeath", "");
        }
    }

    UnidentifiedCameraEffects::Instance()->UnidentifiedNoOp();
}

float cGame::fn_8005B748(int param1, int param2)
{
    if (param1 > param2)
    {
        return m_fCachedPlayerDistances[param1][param2];
    }
    return m_fCachedPlayerDistances[param2][param1];
}

cPlayer* cGame::fn_8005B780(int param1, int param2, int param3)
{
    return m_nClosestPlayers[param1][param2][param3];
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

static inline int GetSyncPlayerIndex(cPlayer* pPlayer)
{
    return pPlayer == 0 ? -1 : GetUnidentifiedPlayerIndex(pPlayer);
}

// Writes the deterministic game state to the network sync log.
void cGame::fn_8005B840(void* context, DebugWriteCache* cache)
{
    if (lbl_806DBAB4 == 0xFFFF)
    {
        lbl_806DBAB4 = cache->BeginType("DetermGameData");
        cache->AddField(DEBUG_FIELD_ENUM, gDebugFieldTypes[DEBUG_FIELD_ENUM].size, 0, "m_eGameState");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&m_fGameDuration - (u8*)&m_eGameState, "m_fGameDuration");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mUnidentified020 - (u8*)&m_eGameState, "m_bBallInNet");
        cache->AddField(DEBUG_FIELD_INT, gDebugFieldTypes[DEBUG_FIELD_INT].size, (u8*)&m_nLastTeamToScore - (u8*)&m_eGameState, "m_nLastTeamToScore");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&mUnidentified028 - (u8*)&m_eGameState, "m_uMegastrikeNumShots");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&mUnidentified02C - (u8*)&m_eGameState, "m_uMegastrikeCurShot");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&mUnidentified030 - (u8*)&m_eGameState, "m_uMegastrikeGoals");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&mUnidentified034 - (u8*)&m_eGameState, "m_uMegastrikeDefendingTeam");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&mUnidentified038 - (u8*)&m_eGameState, "m_uMegastrikeResults");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mbCaptainShotToScoreOn - (u8*)&m_eGameState, "mbMegaStrikeGameplay");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mUnidentified041 - (u8*)&m_eGameState, "mbMegaStrikePositiveNet");
        cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mUnidentified042 - (u8*)&m_eGameState, "mbMegaStrikePlayerReady");
        cache->AddField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, (u8*)&m_pScorer - (u8*)&m_eGameState, "m_pScorer");
        cache->AddField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, (u8*)&m_pAssister - (u8*)&m_eGameState, "m_pAssister");
        cache->AddArrayField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, 2, (u8*)m_pTeamTouch - (u8*)&m_eGameState, "m_pTeamTouch");
        cache->AddArrayField(DEBUG_FIELD_POINTER, gDebugFieldTypes[DEBUG_FIELD_POINTER].size, 10, (u8*)m_pRandomPlayersArray - (u8*)&m_eGameState, "m_pRandomPlayersArray");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified07C - (u8*)&m_eGameState, "mfCheatTilt");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified080 - (u8*)&m_eGameState, "mfXTilt");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified084 - (u8*)&m_eGameState, "mfYTilt");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified088 - (u8*)&m_eGameState, "mfXTiltMin");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified08C - (u8*)&m_eGameState, "mfXTiltMax");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified090 - (u8*)&m_eGameState, "mfYTiltMin");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified094 - (u8*)&m_eGameState, "mfYTiltMax");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified098 - (u8*)&m_eGameState, "mfTiltSpeed");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified09C - (u8*)&m_eGameState, "mfDesiredTiltSpeed");
        cache->AddField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, (u8*)&mUnidentified0A0 - (u8*)&m_eGameState, "mfTiltTime");
        cache->AddField(DEBUG_FIELD_ANGLE, gDebugFieldTypes[DEBUG_FIELD_ANGLE].size, (u8*)&mUnidentified0A4 - (u8*)&m_eGameState, "maTiltDir");
        cache->AddField(DEBUG_FIELD_ANGLE, gDebugFieldTypes[DEBUG_FIELD_ANGLE].size, (u8*)&mUnidentified0A6 - (u8*)&m_eGameState, "maDesiredTiltDir");
        cache->AddField(DEBUG_FIELD_UNSIGNED_INT, gDebugFieldTypes[DEBUG_FIELD_UNSIGNED_INT].size, (u8*)&mUnidentified0A8 - (u8*)&m_eGameState, "muTiltFrames");
        cache->AddField(DEBUG_FIELD_VECTOR3, gDebugFieldTypes[DEBUG_FIELD_VECTOR3].size, (u8*)&mTiltDirection - (u8*)&m_eGameState, "m_vUpVectorTilt");
        cache->EndType();
    }

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
        lbl_806DBAB4, &m_eGameState, sizeof(DetermGameDataCopy));
    if (pCopy != 0)
    {
        pCopy->mScorer = GetSyncPlayerIndex(m_pScorer);
        pCopy->mAssister = GetSyncPlayerIndex(m_pAssister);
        pCopy->mTeamTouch[0] = GetSyncPlayerIndex(m_pTeamTouch[0]);
        pCopy->mTeamTouch[1] = GetSyncPlayerIndex(m_pTeamTouch[1]);
        int* pRandomPlayers = pCopy->mRandomPlayers;
        for (int i = 0; i < 10; i++)
        {
            pRandomPlayers[i] = GetSyncPlayerIndex(m_pRandomPlayersArray[i]);
        }
        cache->ChecksumData(lbl_806DBAB4, pCopy, context);
    }

    UnidentifiedGameSnapshot snapshot;
    fn_80059B70(&snapshot);

    if (lbl_806DBAB2 == 0xFFFF)
    {
        lbl_806DBAB2 = cache->BeginType("GenDetPlayerC");
        cache->AddArrayField(DEBUG_FIELD_U8, gDebugFieldTypes[DEBUG_FIELD_U8].size, 100, 0, "m_nClosestPlayers[0]");
        cache->AddArrayField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, 100,
            (u8*)snapshot.mDistances - (u8*)&snapshot, "m_fCachedPlayerDistances[0]");
        cache->AddArrayField(DEBUG_FIELD_FLOAT, gDebugFieldTypes[DEBUG_FIELD_FLOAT].size, 10,
            (u8*)snapshot.mUnidentified1F4 - (u8*)&snapshot, "m_fCachedBallPlayerDistances[0]");
        cache->EndType();
    }

    cache->ChecksumData(lbl_806DBAB2, &snapshot, context);
    cache->WriteData(lbl_806DBAB2, &snapshot, sizeof(snapshot));

    mUnidentified10E0->SyncLog(context, cache);
    mpTerrain->SyncLog(context, cache);
    mpWeatherManager->SyncLog(context, cache);
    lbl_806E12C8->SyncLog(context, cache);
    NetMesh::spPositiveXNetMesh->SyncLog(context, cache);
    NetMesh::spNegativeXNetMesh->SyncLog(context, cache);
}

void cGame::fn_8005BF50(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&m_eGameState, sizeof(m_eGameState));
    runningChecksum->ChecksumData(&mUnidentified020, sizeof(mUnidentified020));
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
            buffer, sizeof(buffer), lbl_804FB66C, m_eGameState, state, frame);
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
        mUnidentified49C.mEvent13.Queue();
    }

    m_eGameState = state;
    switch (state)
    {
    case 1:
        if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            m_pGameClock->Stop();
        }
        fn_80058748();
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
                    fielder->mUnidentified178 = lbl_806E3748;
                }
            }
        }
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_tNoPickupTimer.SetSeconds(kGameTweakZero);
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

// ShotPresentation
extern "C" void fn_8005C650(cGame* pGame)
{
    pGame->mUnidentified49C.mEvent48.Deliver();
}

// ShotPresentationEnd
extern "C" void fn_8005C830(cGame* pGame)
{
    pGame->mUnidentified49C.mEvent49.Deliver();
}

// CaptainClashPresentation
extern "C" void fn_8005CA10(cGame* pGame)
{
    pGame->mUnidentified49C.mEvent50.Deliver();
}

// WindupPresentation
extern "C" void fn_8005CBF0(cGame* pGame)
{
    pGame->mUnidentified49C.mEvent52.Deliver();
}

// WindupPresentationEnd
extern "C" void fn_8005CDD0(cGame* pGame)
{
    pGame->mUnidentified49C.mEvent53.Deliver();
}

void cGame::SendPauseGameEvent()
{
    mUnidentified49C.mEvent00.Queue(Function<FnVoidVoid>());
}

void cGame::SendResumingGameEvent()
{
    mUnidentified49C.mEvent01.Queue(Function<FnVoidVoid>());
}

// LightningStrike
extern "C" void fn_8005D210(cGame* pGame, LightningStrikeData* pData)
{
    pGame->mUnidentified49C.mEvent44.Queue(
        pData, Function<LightningStrikeData*>(fn_80072134));
}

// GoalieSave
extern "C" void fn_8005D354(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent17.Deliver((GoalieSaveData*)pData);
}

// GoalieKick
extern "C" void fn_8005D550(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent18.Deliver((GoalieSaveData*)pData);
}

// GoalieCatch
extern "C" void fn_8005D74C(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent20.Deliver((GoalieSaveData*)pData);
}

// GoalieExert
extern "C" void fn_8005D948(cGame* pGame, const GoalieSaveData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent21.Deliver((GoalieSaveData*)pData);
}

// MegastrikeEnd: the captain's mega strike is over. Its goals count for the
// attacking side, the game state moves on and the result is announced.
extern "C" void fn_8005DB7C(cGame* pGame)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }

    unsigned int side = 1 - pGame->mUnidentified034;
    cPlayer* pCaptain = g_pTeams[side]->GetCaptain();

    MegaStrikeEndData data;
    data.defendingSide = pGame->mUnidentified034;
    data.goals = pGame->mUnidentified030;
    data.attempts = pGame->mUnidentified028;
    data.unknown_08 = pGame->mUnidentified038;
    data.pPlayer = pCaptain;
    data.goalValue = -1;

    g_pBall->m_pLastTouch = g_pTeams[side]->GetCaptain();
    pGame->m_pTeamTouch[side] = pCaptain;
    pGame->m_pScorer = pCaptain;
    pGame->m_pAssister = 0;

    if (pGame->mUnidentified030 != 0)
    {
        pGame->m_nLastTeamToScore = side;
        g_pBall->m_uGoalType = 6;
        g_pTeams[side]->m_nScore += pGame->mUnidentified030;
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

    g_pGame->fn_80058498(false, 0, 0);
    g_pGame->mUnidentified49C.mEvent47.Deliver(&data);
    g_pBall->m_uGoalType = 4;
    SetPlayerAudioController(0);
}

void cGame::QueueChainNisEnd(ShotAtGoalData* data)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_ShotAtGoalDataPool.Free(data);
        return;
    }
    mUnidentified49C.mEvent37.Queue(
        data, Function<ShotAtGoalData*>(fn_8007214C));
}

void cGame::fn_8005E130(NISData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_NISDataPool.Free(pData);
        return;
    }
    mUnidentified49C.mEvent05.Queue(
        (UnidentifiedEventData_80065E10*)pData,
        Function<UnidentifiedEventData_80065E10*>(
            (void (*)(UnidentifiedEventData_80065E10*))fn_80072164));
}

// CollisionCrowd
extern "C" void fn_8005E29C(cGame* pGame, CollisionCrowdData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_CollisionCrowdDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mEvent32.Queue(
        pData, Function<CollisionCrowdData*>(
                   (void (*)(CollisionCrowdData*))fn_8007217C));
}

// GoalieDekeAttackAttempt
extern "C" void fn_8005E408(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent24.Deliver((PlayerAttackData*)pData);
}

// GoalieDekeAttackSuccess
extern "C" void fn_8005E604(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent25.Deliver((PlayerAttackData*)pData);
}

// GoalieSlamAttackAttempt
extern "C" void fn_8005E800(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent26.Deliver((PlayerAttackData*)pData);
}

// GoalieSlamAttackSuccess
extern "C" void fn_8005E9FC(cGame* pGame, const PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent27.Deliver((PlayerAttackData*)pData);
}

// AttackAttempt
extern "C" void fn_8005EBF8(cGame* pGame, PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_PlayerAttackDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mEvent28.Queue(
        pData, Function<PlayerAttackData*>(fn_80072194));
}

// AttackSuccess
extern "C" void fn_8005ED64(cGame* pGame, PlayerAttackData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_PlayerAttackDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mEvent29.Queue(
        pData, Function<PlayerAttackData*>(fn_80072194));
}

// ShotAtGoal
extern "C" void fn_8005EED0(cGame* pGame, ShotAtGoalData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        g_ShotAtGoalDataPool.Free(pData);
        return;
    }
    pGame->mUnidentified49C.mEvent22.Queue(
        pData, Function<ShotAtGoalData*>(fn_8007214C));
}

// WindupShot
extern "C" void fn_8005F03C(cGame* pGame, ShotAtGoalData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent23.Deliver(pData);
}

// MegaStrikeMeterStart
extern "C" void fn_8005F238(cGame* pGame, MegaStrikeMeterData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent40.Deliver(pData);
}

// MegaStrikeMeterFirst
extern "C" void fn_8005F434(cGame* pGame, MegaStrikeMeterData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent41.Deliver(pData);
}

// MegaStrikeMeterSecond
extern "C" void fn_8005F630(cGame* pGame, MegaStrikeMeterData* pData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent42.Deliver(pData);
}

// MegaStrikeIntro
extern "C" void fn_8005F82C(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    PauseAllAudio();
    pGame->mUnidentified49C.mEvent46.Deliver(pFielder);
}

// MegaStrikeMeterEnd
extern "C" void fn_8005FA2C(cGame* pGame)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent43.Deliver();
}

// PeachFlash
extern "C" void fn_8005FC1C(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent55.Deliver(pEventData);
}

// PeachCameraFlash
extern "C" void fn_8005FE18(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent54.Deliver(pEventData);
}

// PeachCamerasDown
extern "C" void fn_80060014(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent56.Deliver(pEventData);
}

// PeachCamerasAway
extern "C" void fn_80060210(cGame* pGame, PeachPhotoData* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent57.Deliver(pEventData);
}

// WaluigiWallStart
extern "C" void fn_8006040C(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent58.Deliver(pFielder);
}

// WaluigiWallEnd
extern "C" void fn_80060608(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent59.Deliver(pFielder);
}

// WaluigiWallAbort
extern "C" void fn_80060804(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent60.Deliver(pFielder);
}

// SuperPresentation
extern "C" void fn_80060A00(cGame* pGame, cFielder* pFielder)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent64.Deliver(pFielder);
}

// BulletBillExplode
void cGame::fn_80060BFC(CollisionBulletBillData& data)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    mUnidentified49C.mEvent63.Deliver(&data);
}

// MontyReappear
extern "C" void fn_80060DF8(cGame* pGame, const CharacterImpactEvent* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent71.Deliver((CharacterImpactEvent*)pEventData);
}

// HammerBroHammer
extern "C" void fn_80060FF4(cGame* pGame, const CharacterImpactEvent* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent72.Deliver((CharacterImpactEvent*)pEventData);
}

// WarioGroundPound
extern "C" void fn_800611F0(cGame* pGame, const void* pEventData)
{
    if (g_pGame->m_eGameState == 4)
    {
        return;
    }
    pGame->mUnidentified49C.mEvent73.Deliver((CharacterImpactEvent*)pEventData);
}

extern "C" void fn_80061B1C(int relative, float xTilt, float yTilt)
{
    cGame* game = g_pGame;
    if (game != 0)
    {
        if (relative != 0)
        {
            xTilt += game->mUnidentified080;
            yTilt += game->mUnidentified084;
        }

        const float xLimit = lbl_806DBA80;
        xTilt = xTilt >= -xLimit ? xTilt : -xLimit;
        xTilt = xTilt <= xLimit ? xTilt : xLimit;
        const float yLimit = lbl_806DBA84;
        yTilt = yTilt >= -yLimit ? yTilt : -yLimit;
        yTilt = yTilt <= yLimit ? yTilt : yLimit;

        fn_8005B330(&game->mTiltDirection, -xTilt, -yTilt);

        g_pGame->mUnidentified080 = xTilt;
        g_pGame->mUnidentified084 = yTilt;
        if (!GameInfoManager::Instance()->IsRule0x4Equal4())
        {
            g_pGame->mUnidentified07C = lbl_806E3798;
        }
    }

    cCameraManager::SetWorldUpVectorTilt(-xTilt, -yTilt);
    if (g_pBall != 0 && g_pBall->m_pPhysicsBall != 0)
    {
        if (nlAbs(xTilt) > lbl_806E379C || nlAbs(yTilt) > lbl_806E379C)
        {
            nlVector3 tiltForce = { 0 };
            tiltForce.x = yTilt * lbl_806DBA6C;
            tiltForce.y = xTilt * lbl_806DBA6C;
            g_pBall->m_pPhysicsBall->mv3TiltForce = tiltForce;
            g_pBall->m_pPhysicsBall->mbUseTiltForce = true;
        }
        else
        {
            g_pBall->m_pPhysicsBall->mbUseTiltForce = false;
        }
    }
}

extern "C" void fn_8005B330(
    nlVector3* pVector, float fXAxisTilt, float fYAxisTilt)
{
    float fSin;
    float fCos;

    nlSinCos(&fSin, &fCos, ((s32)(lbl_806E374C * fYAxisTilt)) / 360);

    nlVec3Set(*pVector, fSin, 0.0f, fCos);

    nlSinCos(&fSin, &fCos, ((s32)(lbl_806E374C * fXAxisTilt)) / 360);

    pVector->y = fSin;
    pVector->z = pVector->z * fCos;

    float temp_f1 = nlRecipSqrt(pVector->GetLengthSq3D(), true);
    nlVec3Scale(*pVector, temp_f1);
}

extern "C" int fn_8005B45C(
    cPlayer* const* param1, cPlayer* const* param2)
{
    int referenceIndex = GetUnidentifiedPlayerIndex(lbl_806E0C9C);
    cPlayer* firstPlayer = *param1;
    int firstIndex = GetUnidentifiedPlayerIndex(firstPlayer);
    cPlayer* secondPlayer = *param2;
    float first = g_pGame->fn_8005B748(referenceIndex, firstIndex);
    float second = g_pGame->fn_8005B748(
        referenceIndex, GetUnidentifiedPlayerIndex(secondPlayer));

    if (first == second)
    {
        return 0;
    }
    if (first < second)
    {
        return -1;
    }
    return 1;
}

void cGame::fn_8005B508()
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
                m_fCachedPlayerDistances[i][j] = kGameTweakZero;
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
        lbl_806E0C9C = static_cast<cPlayer*>(g_pCharacters[i]);
        for (int j = 0; j < 2; j++)
        {
            nlQSort(m_nClosestPlayers[i][j], 5, fn_8005B45C);
        }
    }
    lbl_806E0C9C = 0;
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

void cGame::fn_8005DB44(int param1, bool param2)
{
    if (param2)
    {
        mUnidentified038 |= 1 << param1;
    }
    else
    {
        mUnidentified038 &= ~(1 << param1);
    }
}

void cGame::fn_8005DF38()
{
    if (GetAudioPauseDepth() > 1)
    {
        ResumeAllAudio();
    }
    ResumeSuddenDeathMusic();

    static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible(OVERLAY_HUD, true, true);
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDIn(lbl_806E3770);
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

UnidentifiedGameEventQueue::UnidentifiedGameEventQueue()
    : mEvent00(fn_800721C4(), "PauseGame", -1)
    , mEvent01(fn_800721C4(), "ResumingGame", -1)
    , mEvent02(fn_800721C4(), "GameOver", -1)
    , mEvent03(fn_800721C4(), "GameIsWon", -1)
    , mEvent04(fn_800721C4(), "PresentationBypass", -1)
    , mEvent05(fn_800721C4(), "NIS", -1)
    , mEvent06("GoalScored", -1)
    , mEvent07(GetFixedUpdateEventDispatcher(), "EnterStartScreen", -1)
    , mEvent08(GetFixedUpdateEventDispatcher(), "DirectionBegin", -1)
    , mEvent09("CharacterDirectionEnd", -1)
    , mEvent10("ResetEffects", -1)
    , mEvent11(GetFixedUpdateEventDispatcher(), "GetReadyForKickoff", -1)
    , mEvent12(GetFixedUpdateEventDispatcher(), "Kickoff", -1)
    , mEvent13(GetFixedUpdateEventDispatcher(), "SuddenDeath", -1)
    , mEvent14("BallStateChange", -1)
    , mEvent15("ReceiveBall", -1)
    , mEvent16("PassBall", -1)
    , mEvent17("GoalieSave", -1)
    , mEvent18("GoalieKick", -1)
    , mEvent19("CollisionBallGoalie", -1)
    , mEvent20("GoalieCatch", -1)
    , mEvent21("GoalieExert", -1)
    , mEvent22(GetFixedUpdateEventDispatcher(), "ShotAtGoal", -1)
    , mEvent23("WindupShot", -1)
    , mEvent24("GoalieDekeAttackAttempt", -1)
    , mEvent25("GoalieDekeAttackSuccess", -1)
    , mEvent26("GoalieSlamAttackAttempt", -1)
    , mEvent27("GoalieSlamAttackSuccess", -1)
    , mEvent28(GetFixedUpdateEventDispatcher(), "AttackAttempt", -1)
    , mEvent29(GetFixedUpdateEventDispatcher(), "AttackSuccess", -1)
    , mEvent30(GetFixedUpdateEventDispatcher(), "CharGetElectrocuted", -1)
    , mEvent31(GetFixedUpdateEventDispatcher(), "PowerupStats", -1)
    , mEvent32(GetFixedUpdateEventDispatcher(), "CollisionCrowd", -1)
    , mEvent33(GetFixedUpdateEventDispatcher(), "CollisionChainPlayer", -1)
    , mEvent34(GetFixedUpdateEventDispatcher(), "CollisionWindDebrisPlayer", -1)
    , mEvent35(GetFixedUpdateEventDispatcher(), "CollisionExplosionFragmentPLayer", -1)
    , mEvent36(GetFixedUpdateEventDispatcher(), "ChainNisStart", -1)
    , mEvent37(GetFixedUpdateEventDispatcher(), "ChainNisEnd", -1)
    , mEvent38(GetFixedUpdateEventDispatcher(), "Penalty", -1)
    , mEvent39(GetFixedUpdateEventDispatcher(), "AwardPowerupStuff", -1)
    , mEvent40("MegaStrikeMeterStart", -1)
    , mEvent41("MegaStrikeMeterFirst", -1)
    , mEvent42("MegaStrikeMeterSecond", -1)
    , mEvent43("MegaStrikeMeterEnd", -1)
    , mEvent44(GetFixedUpdateEventDispatcher(), "LightningStrike", -1)
    , mEvent45(GetFixedUpdateEventDispatcher(), "MegastrikeStart", -1)
    , mEvent46("MegaStrikeIntro", -1)
    , mEvent47("MegastrikeEnd", -1)
    , mEvent48("ShotPresentation", -1)
    , mEvent49("ShotPresentationEnd", -1)
    , mEvent50("CaptainClashPresentation", -1)
    , mEvent51("CaptainClashPresentationEnd", -1)
    , mEvent52("WindupPresentation", -1)
    , mEvent53("WindupPresentationEnd", -1)
    , mEvent54("PeachCameraFlash", -1)
    , mEvent55("PeachFlash", -1)
    , mEvent56("PeachCamerasDown", -1)
    , mEvent57("PeachCamerasAway", -1)
    , mEvent58("WaluigiWallStart", -1)
    , mEvent59("WaluigiWallEnd", -1)
    , mEvent60("WaluigiWallAbort", -1)
    , mEvent61("WarioGasStart", -1)
    , mEvent62("WarioGasEnd", -1)
    , mEvent63("BulletBillExplode", -1)
    , mEvent64("SuperPresentation", -1)
    , mEvent65(GetFixedUpdateEventDispatcher(), "StatsPowerupHitData", -1)
    , mEvent66(GetFixedUpdateEventDispatcher(), "CameraRumbleStart", -1)
    , mEvent67(GetFixedUpdateEventDispatcher(), "CameraRumbleEnd", -1)
    , mEvent68(GetFixedUpdateEventDispatcher(), "ExplodableExplode", -1)
    , mEvent69(GetFixedUpdateEventDispatcher(), "ExplodableExplosionEnd", -1)
    , mEvent70(GetFixedUpdateEventDispatcher(), "SilenceAllSounds", -1)
    , mEvent71("MontyReappear", -1)
    , mEvent72("HammerBroHammer", -1)
    , mEvent73("WarioGroundPound", -1)
    , mEvent74(GetFixedUpdateEventDispatcher(), "PowerupAquire", -1)
{
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

extern "C" void fn_80072134(LightningStrikeData* node)
{
    g_LightningStrikeDataPool.Free(node);
}

extern "C" void fn_8007214C(ShotAtGoalData* node)
{
    g_ShotAtGoalDataPool.Free(node);
}

extern "C" void fn_80072164(UnidentifiedRegistrationNode* node)
{
    node->mNext = (UnidentifiedRegistrationNode*)g_NISDataPool.m_FreeList;
    g_NISDataPool.m_FreeList = (SlotPoolEntry*)node;
}

extern "C" void fn_8007217C(UnidentifiedRegistrationNode* node)
{
    node->mNext = (UnidentifiedRegistrationNode*)g_CollisionCrowdDataPool.m_FreeList;
    g_CollisionCrowdDataPool.m_FreeList = (SlotPoolEntry*)node;
}

extern "C" void fn_80072194(PlayerAttackData* node)
{
    g_PlayerAttackDataPool.Free(node);
}

void FreeCollisionPlayerWallData(CollisionPlayerWallData* node)
{
    g_CollisionPlayerWallDataPool.Free(node);
}

extern "C" EventDispatcher* fn_800721C4()
{
    return &gDispatchEventsTask->dispatcher;
}

// .sdata2 constants, in retail order. They are defined after every use so
// the functions above keep loading them by name.
extern "C" const float kGameTweakZero = 0.0f;
extern "C" const float lbl_806E3740 = -1.0f;
extern "C" const float lbl_806E3744 = 0.5f;
extern "C" const float lbl_806E3748 = 1.0f;
extern "C" const float lbl_806E374C = 65536.0f;
extern "C" const float lbl_806E3750 = 999999.0f;
extern "C" const float lbl_806E3754 = 1.5f;
extern "C" const float lbl_806E3758 = 25.0f;
extern "C" const float lbl_806E375C = 6.0f;
extern "C" const float lbl_806E3760 = 3.0f;
extern "C" const float lbl_806E3764 = 12.5f;
extern "C" const float lbl_806E3768 = 41.2f;
extern "C" const float lbl_806E376C = 0.3f;
extern "C" const float lbl_806E3770 = 0.25f;
extern "C" const float lbl_806E3774 = 0.0333f;
extern "C" const float lbl_806E3778 = 10.0f;
extern "C" const float lbl_806E377C = 5.0f;
extern "C" const float lbl_806E3780 = 0.75f;
extern "C" const float lbl_806E3784 = 30000.0f;
extern "C" const float lbl_806E3788 = 3000.0f;
extern "C" const float lbl_806E378C = 0.001f;
extern "C" const float lbl_806E3798 = -0.01f;
extern "C" const float lbl_806E379C = 0.01f;

extern "C" char lbl_804FB238[] = "SendRemainingMegaStrikeMeter %d\n";
extern "C" char lbl_804FB25C[] = "InitMegaStrikeGameplay called at %d\n";
extern "C" char lbl_804FB284[] = "PPass Slowdown";
extern "C" char lbl_804FB294[] = "Slow-mo_Captain_Hit";
extern "C" char lbl_804FB2F4[] = "Sending NIS Loaded %d at frame %d\n";
extern "C" char lbl_804FB318[] = "Sending MegaStrike Side %d PlayerID %d NumBalls %f Accuracy %f at frame %d\n";
extern "C" char lbl_804FB364[] = "Sending MegaStrikePlayerReady at frame %d\n";
extern "C" char lbl_804FB390[] = "Sending SendMegaStrikeKillCursor at frame %d\n";
extern "C" char lbl_804FB3C0[] = "Sending MegaStrikeGoalie Side %d CurTarget %d Score %f at frame %d\n";
extern "C" char lbl_804FB404[] = "Sending Slow Down End at frame %d\n";
extern "C" char lbl_804FB428[] = "Recv CustomDeterm %d size %d at frame %d\n";
extern "C" char lbl_804FB454[] = "Received MegaStrike Side %d PlayerID %d NumBalls %f Accuracy %f at frame %d\n";
extern "C" char lbl_804FB4A4[] = "Received MegaStrikePlayerReady at frame %d\n";
extern "C" char lbl_804FB4D0[] = "Received MegaStrikeKillCursor at frame %d\n";
extern "C" char lbl_804FB4FC[] = "Received MegaStrikeGoalie Side %d CurTarget %d Score %f at frame %d\n";
extern "C" char lbl_804FB544[] = "Received SlowDownEnd at frame %d\n";
extern "C" char lbl_804FB568[] = "Received CleanupMegastrikeGameplay at frame %d\n";
extern "C" char lbl_804FB598[] = "Unknown custom determ data %d..discarding\n";
extern "C" char lbl_804FB5C4[] = "Discarded message type %d because from unknown connection %x\n";
extern "C" char lbl_804FB604[] = "Discarded message type %d.  TournamentIdxToMachineIdx changed ID %d to ID %d, but invalid\n";
extern "C" char lbl_804FB66C[] = "Changing game state %d to %d at frame %d\n";

#include "NL/nlBind_impl.h"

TweakValueInt::~TweakValueInt()
{
}

void TweakValueInt::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        value = ((TweakValueInt*)other)->value;
        break;
    case 2:
        value = *((TweakIntBinding*)other)->m_pValue;
        break;
    }
}

int TweakValueInt::GetStorageKind()
{
    return 1;
}

int TweakValueInt::GetValueType()
{
    return 3;
}

void* TweakValueInt::GetValueAddress()
{
    return &value;
}

void TweakValueInt::FormatValue(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", value);
}

void TweakValueInt::ParseValue(
    const char* string)
{
    value = atoi(string);
}

void TweakValueInt::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = kGameTweakZero;
    *maximum = kGameTweakZero;
    *increment = kGameTweakZero;
}

void TweakValueInt::UnidentifiedVirtual18()
{
}
