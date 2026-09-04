#include "Game/Game.h"

#include "Game/Task/GameRenderTask.h"

#include "Game/AI/FilteredRandom.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AISandbox.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/Scripts/ScriptCaching.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/Camera/tu_800F9460.h"
#include "Game/DebugWriteCache.h"
#include "Game/Field.h"
#include "Game/Formation.h"
#include "Game/GameInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/NetworkSession.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Player.h"
#include "Game/UnidentifiedPlayerRadius.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Sys/clock.h"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/Team.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlConfig.h"
#include "NL/nlMain.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_801AE530.h"
#include "unclassified/tu_80332DC0.h"
#include "unclassified/tu_80338898.h"
#include "Game/DB/StadiumInfo.h"

extern PowerupBase* g_pPowerups[];

struct UnidentifiedGameStatic
{
    u8 mUnidentified000[0x0C];
    int mUnidentified00C;
};

struct UnidentifiedOnlineState
{
    u8 mUnidentified000[4];
    bool mUnidentified004;
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

struct UnidentifiedRegistrationList
{
    u8 mUnidentified000[0x0C];
    UnidentifiedRegistrationNode* mHead;
};

extern "C" EventDispatcher* fn_80111678();
extern "C" EventDispatcher* fn_800721C4();
extern "C" bool fn_802B6AF8(
    const UnidentifiedGameRegion* param1, const nlVector2* param2);
extern "C" int fn_800A9210(void* param1, int param2);
extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" int fn_800ECCCC();
extern "C" void fn_800ECB50();
extern "C" void fn_800EDC2C();
extern "C" void fn_801E230C(
    BaseGameSceneManager* manager, SceneList scene, bool param3, bool param4);
extern "C" void fn_801E2498(BaseGameSceneManager* manager, float param2);
extern "C" void fn_801E999C(BaseSceneHandler* scene);
extern "C" void* fn_800AA060(void* param1, int param2);
extern "C" void fn_800AF404(void* param1);
extern "C" void fn_800EDB9C();
extern "C" void fn_800EDCAC();
extern "C" bool fn_8001E184(cPlayer* pPlayer);
extern "C" int fn_80323A58(int param1, char* buffer, u32 size);
extern "C" void fn_8008EFE8(Goalie* pGoalie, float param2, float param3);
extern "C" void fn_80038158(cFielder* pFielder, int param2);
extern "C" int fn_8032C830(
    void* param1, UnidentifiedNetworkMessage_80126D84* message,
    void* buffer, u32 size);
extern "C" float fn_80111D3C();
extern "C" void fn_80111D28(float timeScale);
extern "C" void fn_80111D4C(float timeScale, float transitionTime);
extern "C" bool fn_800EBBFC(
    int param1, unsigned long soundID, const void* name, void* context);
extern "C" void fn_800EC12C(unsigned long soundID, void* context);
extern "C" void fn_802F4E84(unsigned long* hash, int param2, int param3);
extern "C" cGame* fn_800570B0(
    cGame* game, void* param1, int param2, bool param3);
extern "C" cTeam* fn_800A5D4C(cTeam* team, int side);
extern "C" void fn_800A62E8(cTeam* team, int deleteObject);
extern "C" void fn_8031A02C(ScriptQuestionCache* cache);
extern "C" void fn_800ED92C(unsigned long soundID);
extern "C" void fn_800EC2A4(unsigned long soundID, cGame* game);

extern UnidentifiedGameStatic lbl_8056B9A0;
extern cPlayer* lbl_806E0C9C;
extern void* lbl_806E2100;
extern int lbl_806E2130;
extern UnidentifiedOnlineState* lbl_806E2164;
extern BaseGameSceneManager* lbl_806E1860;
extern AISandbox* lbl_806E0B88;
extern UnidentifiedRegistrationList lbl_805713E8;
extern UnidentifiedRegistrationList lbl_80571820;
extern UnidentifiedRegistrationList lbl_80571988;
extern UnidentifiedRegistrationList lbl_80571438;
extern UnidentifiedRegistrationList lbl_80571960;
extern UnidentifiedRegistrationList lbl_80571348;
extern cPlayer* lbl_8056B800[10];
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

extern "C" const float lbl_806E373C;
extern "C" const float lbl_806E374C;
extern "C" const float lbl_806E3740;
extern "C" const float lbl_806E3748;
extern "C" const float lbl_806E376C;
extern "C" const float lbl_806E3770;

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
    return fn_802AAA28(nlGetTicker());
}

void fn_80056CF4(void* param1, int param2, bool param3)
{
    ++lbl_806E2130;

    cGame* game = static_cast<cGame*>(nlMalloc(0x10F4, 8, false));
    if (game != 0)
    {
        game = fn_800570B0(game, param1, param2, param3);
    }
    g_pGame = game;

    cTeam* team = static_cast<cTeam*>(nlMalloc(0xF8, 8, false));
    if (team != 0)
    {
        team = fn_800A5D4C(team, 0);
    }
    g_pTeams[0] = team;

    team = static_cast<cTeam*>(nlMalloc(0xF8, 8, false));
    if (team != 0)
    {
        team = fn_800A5D4C(team, 1);
    }
    g_pTeams[1] = team;

    cField::Init(g_pTeams[0]->m_pNet, g_pTeams[1]->m_pNet);

    if (lbl_806E0B88 == 0)
    {
        lbl_806E0B88 = new (8, false) AISandbox();
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
        UnidentifiedNumberDisplay_801AE530* numberDisplay
            = static_cast<UnidentifiedNumberDisplay_801AE530*>(
                nlMalloc(0x28, 8, false));
        numberDisplay
            = new (numberDisplay) UnidentifiedNumberDisplay_801AE530();
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
            g_pGame->m_fGameDuration, lbl_806E3740, 0);
    }

    if (lbl_806E0B88 != 0)
    {
        delete lbl_806E0B88;
        lbl_806E0B88 = 0;
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

    fn_800A62E8(g_pTeams[0], 1);
    fn_800A62E8(g_pTeams[1], 1);
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

void cGame::fn_80058180()
{
    fn_8004F594(16, lbl_804FB238, mUnidentified134.mSize);

    while (mUnidentified134.mSize > 0)
    {
        int count = mUnidentified134.mSize;
        if (count > 8)
        {
            count = 8;
        }

        UnidentifiedNetworkMessage_80126D84 message;
        message.mCount = count;
        for (int i = 0; i < count; i++)
        {
            message.mValues[i]
                = mUnidentified134.UnidentifiedRemoveStart();
        }

        char buffer[50];
        s8 i;
        int size = fn_8032C830(
            lbl_806E2100, &message, buffer, sizeof(buffer));
        int playerCount = fn_80338BF0(lbl_806E20D8);
        for (i = 0; i < playerCount; i++)
        {
            if (i != fn_80338C20(lbl_806E20D8))
            {
                lbl_806E20D8->Send(i, buffer, size, true);
            }
        }
    }
}

void cGame::fn_8005830C()
{
    DebugWriteCache* output = fn_80338950(lbl_806E2168);
    if (output != 0)
    {
        char buffer[256];
        int frame = GetFixedUpdateTask()->GetFrame();
        nlSNPrintf(buffer, sizeof(buffer), lbl_804FB25C, frame);
        fn_8033919C(output, buffer);
        fn_8004F594(16, buffer);
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

void cGame::fn_80058498(bool param1, int param2, int param3)
{
    mUnidentified040 = param1;
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
    if (fn_80338BF0(lbl_806E20D8) > 1 && timeScale < lbl_806E376C)
    {
        timeScale = lbl_806E376C;
    }

    if (fn_80111D3C() != lbl_806E3748 || lbl_806E3748 != timeScale)
    {
        if (g_pGame->m_eGameState != 4)
        {
            if (GameInfoManager::Instance()->GetCurrentSettings()->unknown_0x04 == 0)
            {
                m_pGameClock->Stop();
            }

            if (fn_80111D3C() == lbl_806E3748)
            {
                unsigned long soundID = 0xCE5CBAC7;
                fn_800EC12C(soundID, g_pGame);
                fn_800EBBFC(10, soundID, lbl_804FB284, g_pGame);

                unsigned long hash = nlStringLowerHash(lbl_804FB294);
                fn_802F4E84(&hash, 0, 0);
            }

            lbl_806E1860->GetScene((SceneList)89)->SetVisible(false);
            gpNumberDisplay->mUnidentified004 = false;

            if (transitionTime <= lbl_806E373C)
            {
                fn_80111D28(timeScale);
                ParticleUpdateTask::sInstance->SetTimeScale(timeScale);
            }
            else
            {
                fn_80111D4C(timeScale, transitionTime);
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
    if (GameInfoManager::Instance()->GetCurrentSettings()->unknown_0x04 == 0)
    {
        m_pGameClock->Start();
    }
}

void cGame::fn_80058704()
{
    if (GameInfoManager::Instance()->GetCurrentSettings()->unknown_0x04 == 0)
    {
        m_pGameClock->Stop();
    }
}

void cGame::fn_80058A78(float seconds)
{
    m_pPostResetClock->Reset(0.0f, seconds, 1.0f);
    m_pPostResetClock->Start();
}

void cGame::BlowUpPowerups(
    const UnidentifiedGameRegion& v3ExplosionPosition,
    float fExplosionRadius)
{
    for (int i = 0; i < 25; i++)
    {
        if (g_pPowerups[i] != 0)
        {
            nlVector2 position;
            position.x = g_pPowerups[i]->m_v3Position.x;
            position.y = g_pPowerups[i]->m_v3Position.y;
            if (fn_802B6AF8(&v3ExplosionPosition, &position))
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

    u32 frame = lbl_806E2138->mFrameProvider->GetFrame();
    fn_8004F594(16, lbl_804FB2F4, message.param1, frame);
    fn_80333908(fn_803330AC(), &message, sizeof(message));
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

    u32 frame = lbl_806E2138->mFrameProvider->GetFrame();
    fn_8004F594(16, lbl_804FB318, message.param1, message.param2, message.param3, message.param4, frame);
    fn_80333908(fn_803330AC(), &message, sizeof(message));
}

void cGame::fn_80059E78()
{
    u8 message = 183;
    u32 frame = lbl_806E2138->mFrameProvider->GetFrame();
    fn_8004F594(16, lbl_804FB364, frame);
    fn_80333908(fn_803330AC(), &message, sizeof(message));
}

void cGame::fn_80059EDC()
{
    u8 message = 185;
    u32 frame = lbl_806E2138->mFrameProvider->GetFrame();
    fn_8004F594(16, lbl_804FB390, frame);
    fn_80333908(fn_803330AC(), &message, sizeof(message));
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

    u32 frame = lbl_806E2138->mFrameProvider->GetFrame();
    fn_8004F594(16, lbl_804FB3C0, message.param1, message.param2, message.param3, frame);
    fn_80333908(fn_803330AC(), &message, sizeof(message));
}

void cGame::fn_80059FC4()
{
    u8 message = 222;
    u32 frame = lbl_806E2138->mFrameProvider->GetFrame();
    fn_8004F594(16, lbl_804FB404, frame);
    fn_80333908(fn_803330AC(), &message, sizeof(message));
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

    if (fn_80338C20(lbl_806E20D8) == 0
        && !lbl_806E2164->mUnidentified004
        && lbl_806E2138->mFrameProvider->GetFrame() % 10 == 0)
    {
        u8 message[3];
        message[0] = 5;

        for (int i = 0; i < 2; i++)
        {
            u8 flags = 0;
            cTeam* pTeam = g_pTeams[i];
            for (int j = 0; j < 5; j++)
            {
                if (fn_8001E184(pTeam->GetPlayer(j)))
                {
                    flags |= 1 << j;
                }
            }
            message[i + 1] = flags;
        }

        fn_80333908(fn_803330AC(), message, sizeof(message));
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
    runningChecksum->ChecksumData(&mUnidentified024, sizeof(mUnidentified024));
}

void cGame::ChangeGameState(int state)
{
    DebugWriteCache* output = fn_80338950(lbl_806E2168);
    if (output != 0)
    {
        char buffer[256];
        int frame = GetFixedUpdateTask()->GetFrame();
        nlSNPrintf(
            buffer, sizeof(buffer), lbl_804FB66C, m_eGameState, state, frame);
        fn_8033919C(output, buffer);
        fn_8004F594(16, buffer);
        if (fn_80323A58(6, buffer, sizeof(buffer)) != 0)
        {
            fn_8033919C(output, buffer);
        }
    }

    if (state != m_eGameState)
    {
        if (m_eGameState == 6 && state == 3)
        {
            fn_800EDCAC();
        }

        if (state == 3)
        {
            if ((GameInfoManager::Instance()->IsInMode4()
                    && lbl_806E0FA0->mCondition == 2
                    && g_pTeams[1]->m_nScore > 0)
                || (lbl_806E0FA0->mCurrentChallenge == 2
                    && g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore))
            {
                fn_800ED92C(0xEF3369E0);
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
                fn_800ED92C(soundID);
            }
        }

        if (m_eGameState == 5)
        {
            unsigned long soundID = GetStadiumSoundID(
                GameInfoManager::Instance()->GetStadium());
            fn_800EC2A4(soundID, this);
        }

        InitGameState(state);
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
            = lbl_8056B800[i]->mUnidentified320->GetRadius();

        cPlayer* pPlayer = lbl_8056B800[i];
        cBall* pBall = g_pBall;
        nlVector2 v2BallDistance;
        v2BallDistance.x
            = pBall->m_v3Position.x - pPlayer->m_v3Position.x;
        v2BallDistance.y
            = pBall->m_v3Position.y - pPlayer->m_v3Position.y;
        m_fCachedBallPlayerDistances[i] = nlVec2Length(v2BallDistance);
        m_fCachedBallPlayerDistances[i]
            -= fBallRadius + fPlayerRadius;

        for (int j = 0; j < 10; j++)
        {
            if (i <= j)
            {
                m_fCachedPlayerDistances[i][j] = lbl_806E373C;
            }
            else
            {
                cPlayer* pPlayer = lbl_8056B800[i];
                cPlayer* pOtherPlayer = lbl_8056B800[j];
                nlVector2 v2PlayerDistance;
                v2PlayerDistance.x = pPlayer->m_v3Position.x
                                   - pOtherPlayer->m_v3Position.x;
                v2PlayerDistance.y = pPlayer->m_v3Position.y
                                   - pOtherPlayer->m_v3Position.y;
                m_fCachedPlayerDistances[i][j]
                    = nlVec2Length(v2PlayerDistance);
                m_fCachedPlayerDistances[i][j]
                    -= fPlayerRadius
                     + lbl_8056B800[j]->mUnidentified320->GetRadius();
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {
        lbl_806E0C9C = lbl_8056B800[i];
        for (int j = 0; j < 2; j++)
        {
            nlQSort(m_nClosestPlayers[i][j], 5, fn_8005B45C);
        }
    }
    lbl_806E0C9C = 0;
}

extern "C" int fn_8005C5CC(void* param1, int param2)
{
    return fn_800A9210(g_pGame->mUnidentified10D8, param2);
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
    if (fn_800ECCCC() > 1)
    {
        fn_800ECB50();
    }
    fn_800EDC2C();

    fn_801E230C(lbl_806E1860, (SceneList)89, true, true);
    fn_801E2498(lbl_806E1860, lbl_806E3770);
    fn_801E999C(lbl_806E1860->GetScene((SceneList)89));

    if (mUnidentified10DC != 0)
    {
        void* param = fn_800AA060(mUnidentified10DC, 7);
        if (param != 0)
        {
            fn_800AF404(param);
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
    , mEvent07(fn_80111678(), "EnterStartScreen", -1)
    , mEvent08(fn_80111678(), "DirectionBegin", -1)
    , mEvent09("CharacterDirectionEnd", -1)
    , mEvent10("ResetEffects", -1)
    , mEvent11(fn_80111678(), "GetReadyForKickoff", -1)
    , mEvent12(fn_80111678(), "Kickoff", -1)
    , mEvent13(fn_80111678(), "SuddenDeath", -1)
    , mEvent14("BallStateChange", -1)
    , mEvent15("ReceiveBall", -1)
    , mEvent16("PassBall", -1)
    , mEvent17("GoalieSave", -1)
    , mEvent18("GoalieKick", -1)
    , mEvent19("CollisionBallGoalie", -1)
    , mEvent20("GoalieCatch", -1)
    , mEvent21("GoalieExert", -1)
    , mEvent22(fn_80111678(), "ShotAtGoal", -1)
    , mEvent23("WindupShot", -1)
    , mEvent24("GoalieDekeAttackAttempt", -1)
    , mEvent25("GoalieDekeAttackSuccess", -1)
    , mEvent26("GoalieSlamAttackAttempt", -1)
    , mEvent27("GoalieSlamAttackSuccess", -1)
    , mEvent28(fn_80111678(), "AttackAttempt", -1)
    , mEvent29(fn_80111678(), "AttackSuccess", -1)
    , mEvent30(fn_80111678(), "CharGetElectrocuted", -1)
    , mEvent31(fn_80111678(), "PowerupStats", -1)
    , mEvent32(fn_80111678(), "CollisionCrowd", -1)
    , mEvent33(fn_80111678(), "CollisionChainPlayer", -1)
    , mEvent34(fn_80111678(), "CollisionWindDebrisPlayer", -1)
    , mEvent35(fn_80111678(), "CollisionExplosionFragmentPLayer", -1)
    , mEvent36(fn_80111678(), "ChainNisStart", -1)
    , mEvent37(fn_80111678(), "ChainNisEnd", -1)
    , mEvent38(fn_80111678(), "Penalty", -1)
    , mEvent39(fn_80111678(), "AwardPowerupStuff", -1)
    , mEvent40("MegaStrikeMeterStart", -1)
    , mEvent41("MegaStrikeMeterFirst", -1)
    , mEvent42("MegaStrikeMeterSecond", -1)
    , mEvent43("MegaStrikeMeterEnd", -1)
    , mEvent44(fn_80111678(), "LightningStrike", -1)
    , mEvent45(fn_80111678(), "MegastrikeStart", -1)
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
    , mEvent65(fn_80111678(), "StatsPowerupHitData", -1)
    , mEvent66(fn_80111678(), "CameraRumbleStart", -1)
    , mEvent67(fn_80111678(), "CameraRumbleEnd", -1)
    , mEvent68(fn_80111678(), "ExplodableExplode", -1)
    , mEvent69(fn_80111678(), "ExplodableExplosionEnd", -1)
    , mEvent70(fn_80111678(), "SilenceAllSounds", -1)
    , mEvent71("MontyReappear", -1)
    , mEvent72("HammerBroHammer", -1)
    , mEvent73("WarioGroundPound", -1)
    , mEvent74(fn_80111678(), "PowerupAquire", -1)
{
}

extern "C" void fn_80061AF0()
{
    fn_800EDB9C();
}

extern "C" void fn_80061AF4()
{
    lbl_806E12C8->ResetEffects();
    fn_800EDCAC();
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

extern "C" void fn_80072134(UnidentifiedRegistrationNode* node)
{
    node->mNext = lbl_805713E8.mHead;
    lbl_805713E8.mHead = node;
}

extern "C" void fn_8007214C(UnidentifiedRegistrationNode* node)
{
    node->mNext = lbl_80571820.mHead;
    lbl_80571820.mHead = node;
}

extern "C" void fn_80072164(UnidentifiedRegistrationNode* node)
{
    node->mNext = lbl_80571988.mHead;
    lbl_80571988.mHead = node;
}

extern "C" void fn_8007217C(UnidentifiedRegistrationNode* node)
{
    node->mNext = lbl_80571438.mHead;
    lbl_80571438.mHead = node;
}

extern "C" void fn_80072194(UnidentifiedRegistrationNode* node)
{
    node->mNext = lbl_80571960.mHead;
    lbl_80571960.mHead = node;
}

extern "C" void fn_800721AC(UnidentifiedRegistrationNode* node)
{
    node->mNext = lbl_80571348.mHead;
    lbl_80571348.mHead = node;
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
