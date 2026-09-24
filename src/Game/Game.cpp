#include "Game/NetworkMessageRegistry.h"
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
#include "Game/AI/FielderInput.h"
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
#include "Game/CharacterTemplate.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/GameProgress.h"
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
#include "Game/Render/NumberDisplay.h"
#include "Game/InputRouter.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"
#include "Game/DB/StadiumInfo.h"

extern PowerupBase* g_pPowerups[];
extern "C" void fn_8031A0FC(float value);
extern float (*lbl_806DF560)();
extern float (*lbl_806DF564)();
extern "C" const nlVector3 lbl_804DBFE8;
bool fn_80287B7C(Presentation* state);
extern "C" void fn_80015B38(cBall* pBall, bool bParam);

struct UnidentifiedGameStatic
{
    u8 mUnidentified000[0x0C];
    int mUnidentified00C;
};

struct UnidentifiedGameSnapshot
{
    u8 mPlayerIndices[100];
    float mDistances[100];
    float mUnidentified1F4[10];
};

class UnidentifiedCallbackTarget
{
};

typedef void (UnidentifiedCallbackTarget::*UnidentifiedCallbackWithArg)(
    void* value);
typedef void (UnidentifiedCallbackTarget::*UnidentifiedCallbackNoArg)();

struct UnidentifiedCallbackWithArgBinding
{
    virtual void UnidentifiedVirtual();

    UnidentifiedCallbackWithArg mCallback;
    UnidentifiedCallbackTarget* mTarget;
};

struct UnidentifiedCallbackNoArgBinding
{
    virtual void UnidentifiedVirtual();

    UnidentifiedCallbackNoArg mCallback;
    UnidentifiedCallbackTarget* mTarget;
};

struct UnidentifiedRegistrationNode
{
    UnidentifiedRegistrationNode* mNext;
};

extern "C" EventDispatcher* fn_800721C4();
extern "C" void fn_8007214C(ShotAtGoalData* node);
extern "C" int GetAudioPauseDepth();
extern "C" void ResumeAllAudio();
extern "C" void fn_800EDC2C();
extern "C" void fn_801E999C(BaseSceneHandler* scene);
extern "C" void fn_8008EFE8(Goalie* pGoalie, float param2, float param3);
extern "C" void fn_80038158(cFielder* pFielder, int param2);
extern "C" void fn_802F4E84(unsigned long* hash, int param2, int param3);
extern "C" void fn_8031A02C(ScriptQuestionCache* cache);
extern "C" void fn_80058ABC(unsigned long param1, unsigned long param2);
extern "C" void fn_80061B1C(int nParam, float fParam1, float fParam2);
extern "C" void fn_8001847C(cBall* pBall, bool bParam);
extern "C" void fn_8005B330(nlVector3* pVector, float fXAxisTilt, float fYAxisTilt);
extern void PlaySuddenDeathMusic();
extern void StopSuddenDeathMusic();
extern int gNextAvoidableObjectId;

struct Unidentified0C74
{
    virtual ~Unidentified0C74();
};

extern Unidentified0C74* lbl_806E0C74;

extern UnidentifiedGameStatic lbl_8056B9A0;
extern cPlayer* lbl_806E0C9C;
extern BaseGameSceneManager* g_pOverlayManager;
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

extern "C" float lbl_806DBA6C;
extern "C" float lbl_806DBA80;
extern "C" float lbl_806DBA84;
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
extern bool lbl_806E0C98;
extern int lbl_806DBA68;
extern "C" int lbl_806DBAAC;

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
        PhysicsPatchManager_801740D0* memory
            = new (nlMalloc(sizeof(PhysicsPatchManager_801740D0), 8, false))
                PhysicsPatchManager_801740D0();
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
    m_pTeamTouch[0] = m_pTeamTouch[1] = 0;
    m_pRandomPlayersArray[0] = 0;
    m_pRandomPlayersArray[1] = 0;
    m_pRandomPlayersArray[2] = 0;
    m_pRandomPlayersArray[3] = 0;
    m_pRandomPlayersArray[4] = 0;
    m_pRandomPlayersArray[5] = 0;
    m_pRandomPlayersArray[6] = 0;
    m_pRandomPlayersArray[7] = 0;
    m_pRandomPlayersArray[8] = 0;
    m_pRandomPlayersArray[9] = 0;
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
            {
                game->m_pGameClock->Stop();
            }
        }
        else if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0)
        {
            game->m_pGameClock->Start();
        }
    }
    if (GetConfigBool(Config::Global(), "save_stats", false))
    {
        StatsTracker::Instance()->WriteCurrentlyPlaying();
    }

    UnidentifiedFindEvent<UnidentifiedEventNoData>("SuddenDeath", -1)
        ->Add(Function<FnVoidVoid>(BindMember(this, &cGame::OnSuddenDeath)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)
        ->Add(Function<FnVoidVoid>(BindMember(this, &cGame::OnGameOver)), 0, -1);

    mUnidentified014 = new (nlMalloc(sizeof(UnidentifiedFielderInput), 8, false))
        UnidentifiedFielderInput(
            this, 0, new (nlMalloc(sizeof(UnidentifiedFuzzyRuntime), 8, false))
                         UnidentifiedFuzzyRuntime());
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

    mUnidentified014->fn_8030F74C(true, true);
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
    fn_8008EFE8(pGoalie, param2, param3);
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

void cGame::BeginGame(bool param1, bool param2)
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
    m_pTeamTouch[0] = m_pTeamTouch[1] = 0;
    m_pRandomPlayersArray[0] = 0;
    m_pRandomPlayersArray[1] = 0;
    m_pRandomPlayersArray[2] = 0;
    m_pRandomPlayersArray[3] = 0;
    m_pRandomPlayersArray[4] = 0;
    m_pRandomPlayersArray[5] = 0;
    m_pRandomPlayersArray[6] = 0;
    m_pRandomPlayersArray[7] = 0;
    m_pRandomPlayersArray[8] = 0;
    m_pRandomPlayersArray[9] = 0;
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
    float sinTilt;
    float cosTilt;
    float tilt = -kGameTweakZero;
    nlSinCos(&sinTilt, &cosTilt, (s32)(lbl_806E374C * tilt) / 360);
    nlVec3Set(mTiltDirection, sinTilt, kGameTweakZero, cosTilt);
    nlSinCos(&sinTilt, &cosTilt, (s32)(lbl_806E374C * tilt) / 360);
    mTiltDirection.y = sinTilt;
    mTiltDirection.z *= cosTilt;
    nlVec3Scale(mTiltDirection, nlRecipSqrt(mTiltDirection.GetLengthSq3D(), true));

    mUnidentified0B8 = lbl_806DBA68;
    mUnidentified0BC = false;
    mUnidentified0BD = false;

    fn_80058498(false, 0, 0);
    fn_80059A1C();
    mpWeatherManager->Reset();
    mpWeatherManager->Stop(true);
    RandomizePlayerUpdateOrder();
    for (int i = 0; i < 2; i++)
    {
        g_pTeams[i]->fn_800A6248();
        g_pTeams[i]->ResetCharacters();
    }
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
    gNPCManager->fn_801ABF8C();

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
        g_pTeams[0]->m_nScore = 0;
        g_pTeams[0]->m_nScore += g_pStrikerChallenge->mScore[0];
        g_pTeams[1]->m_nScore = 0;
        g_pTeams[1]->m_nScore += g_pStrikerChallenge->mScore[1];
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
    if (param2)
    {
        ChangeGameState(1);
        FixedUpdateTask* task = GetFixedUpdateTask();
        task->mUnidentified38 = true;
    }
    else
    {
        GetPresentation()->PlayHighlights();
    }

    --lbl_806E2130;
    ReplayManager::Instance()->ResetSnapshots();
    if (IsNetworkOrRecordedGame())
    {
        fn_8031A0FC(lbl_806E3740);
        lbl_806DF560 = fn_80056CA4;
        lbl_806DF564 = fn_80056CA4;
    }
    else
    {
        fn_8031A0FC(lbl_806E3748);
        lbl_806DF560 = fn_80056CD0;
        lbl_806DF564 = fn_80056CA4;
    }
}

void cGame::fn_80058498(bool param1, int param2, int param3)
{
    mbCaptainShotToScoreOn = param1;
    if (param1)
    {
        mUnidentified041 = g_pTeams[param2]->m_pNet->m_v3NetLocation.x > 0.0f;
        if (lbl_8056B9A0.mUnidentified00C > 0)
        {
            mUnidentified028 = lbl_8056B9A0.mUnidentified00C;
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

                unsigned long hash = nlStringLowerHash(lbl_804FB294);
                fn_802F4E84(&hash, 0, 0);
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
    mUnidentified49C.mEvent11.Queue(Function<FnVoidVoid>());

    fn_80061B1C(0, 0.0f, 0.0f);
    gNPCManager->fn_801ABF8C();
    RandomizePlayerUpdateOrder();

    for (int i = 0; i < 2; i++)
    {
        g_pTeams[i]->fn_800A6248();
        g_pTeams[i]->ResetCharacters();
    }

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
    game->mUnidentified49C.mEvent12.Queue(Function<FnVoidVoid>());

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

void cGame::fn_80059D80(u8 param1)
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

void cGame::fn_80059F40(u8 param1, u8 param2, float param3)
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
        mUnidentified49C.mEvent13.Queue(Function<FnVoidVoid>());
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
        if (!fn_80287B7C(GetPresentation()))
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

void cGame::SendPauseGameEvent()
{
    mUnidentified49C.mEvent00.Queue(Function<FnVoidVoid>());
}

void cGame::SendResumingGameEvent()
{
    mUnidentified49C.mEvent01.Queue(Function<FnVoidVoid>());
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

        float cosTilt;
        float sinTilt;
        nlSinCos(&sinTilt, &cosTilt, (s32)(lbl_806E374C * -yTilt) / 360);
        const float cosine = cosTilt;
        const float sine = sinTilt;
        nlVec3Set(game->mTiltDirection, sine, kGameTweakZero, cosine);
        nlSinCos(&sinTilt, &cosTilt, (s32)(lbl_806E374C * -xTilt) / 360);
        game->mTiltDirection.y = sinTilt;
        game->mTiltDirection.z *= cosTilt;
        float invLength = nlRecipSqrt(game->mTiltDirection.GetLengthSq3D(), true);
        nlVec3Scale(game->mTiltDirection, invLength);

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
    lbl_805842EC.Clear();
    fn_8031A02C(&lbl_805842EC);

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
    static_cast<OverlayManager*>(g_pOverlayManager)->fn_801E2498(lbl_806E3770);
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

extern "C" void fn_80070960(
    UnidentifiedCallbackWithArgBinding* binding, void* value)
{
    (binding->mTarget->*binding->mCallback)(value);
}

extern "C" void fn_800709FC(UnidentifiedCallbackNoArgBinding* binding)
{
    (binding->mTarget->*binding->mCallback)();
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

extern "C" void fn_800721AC(CollisionPlayerWallData* node)
{
    g_CollisionPlayerWallDataPool.Free(node);
}

extern "C" EventDispatcher* fn_800721C4()
{
    return &gDispatchEventsTask->dispatcher;
}

extern "C" const float lbl_806E374C = 65536.0f;
extern "C" const float lbl_806E3770 = 0.25f;

extern "C" char lbl_804FB2F4[] = "Sending NIS Loaded %d at frame %d\n";
extern "C" char lbl_804FB318[] = "Sending MegaStrike Side %d PlayerID %d NumBalls %f Accuracy %f at frame %d\n";
extern "C" char lbl_804FB364[] = "Sending MegaStrikePlayerReady at frame %d\n";
extern "C" char lbl_804FB390[] = "Sending SendMegaStrikeKillCursor at frame %d\n";
extern "C" char lbl_804FB3C0[] = "Sending MegaStrikeGoalie Side %d CurTarget %d Score %f at frame %d\n";
extern "C" char lbl_804FB404[] = "Sending Slow Down End at frame %d\n";
extern "C" char lbl_804FB66C[] = "Changing game state %d to %d at frame %d\n";

#include "NL/nlBind_impl.h"
