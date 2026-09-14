#include "revolution/os/OSTime.h"

#include <stddef.h>

#include "Game/Team.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DetInput.h"
#include "Game/DebugWriteCache.h"
#include "Game/Formation.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/OverlayHandlerHUD.h"
#include "Game/Player.h"
#include "Game/Render/ShootToScoreMeter.h"
#include "Game/RumbleActions.h"
#include "Game/Sys/audio.h"
#include "Game/TweakRegistry.h"
#include "NL/nlLocalization.h"
#include "NL/nlMain.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "Game/Render/PeachPhoto.h"
#include "Game/InputManager.h"
#include "Game/NetworkInput.h"

#include <stdlib.h>

cTeam* g_pTeams[2] = { NULL, NULL };
cTeam* g_pCurrentlyUpdatingTeam;
float lbl_806DBEF0 = 0.5f;
float lbl_806DBEF4 = 7.5f;
u16 lbl_806DBF04 = 0xFFFF;
u16 lbl_806DBF06 = 0xFFFF;
char lbl_806DBF08[] = "m_nSide";
char lbl_806DBF10[] = "DetTeam";
bool lbl_806E0E04;
unsigned long lbl_806E0E08[2];

char lbl_80500A10[] = "PLAYER %d DONE\n\n";
char lbl_80500A24[] = "m_nTeamPlayTransFunc";
char lbl_80500A3C[] = "BallIntOrdFs[]";
char lbl_80500A4C[] = "m_nAIOrdFs[]";
char lbl_80500A5C[] = "m_nPowerups[]";
char lbl_80500A6C[] = "m_ePowupType[]";
char lbl_80500A7C[] = "GenDetTeam";
char lbl_80500A88[] = "mpBestBallInterceptor";
char lbl_80500AA0[] = "mvBallInterceptPosition[]";
char lbl_80500AC0[] = "mfBallInTimes[]";
char lbl_80500AD0[] = "mtToggleTimer";
char lbl_80500AE0[] = "mtDefensiveZoneTimer";
char lbl_80500AF8[] = "mtRoleTimer";
char lbl_80500B04[] = "mtMarkTimer";
char lbl_80500B10[] = "mtTeamStyleTimer";
char lbl_80500B24[] = "meCurrentTeamStyle";
char lbl_80500B38[] = "meCurrentSituation";
char lbl_80500B4C[] = "mfPowerupTimer";
char lbl_80500B5C[] = "mfPowerupMeter";
char lbl_80500B6C[] = "m_nScore";

struct GenDetTeam
{
    ePowerUpType m_ePowupType[2];
    int m_nPowerups[2];
    int m_nAIOrdFs[4];
    int BallIntOrdFs[4];
    u32 m_nTeamPlayTransFunc;
};

static const unsigned short g_aAdvantagePlayerFacingDirections[5] = {
    0,
    0xCB20,
    0,
    0,
    0,
};
static const unsigned short g_aNeutralPlayerFacingDirections[5] = {
    0xEA60,
    0xCB20,
    0,
    0,
    0,
};

inline float max_float(float a, float b)
{
    return (a >= b) ? a : b;
}

inline float min_float(float a, float b)
{
    return (a <= b) ? a : b;
}

static inline float WeightedScore2(float fScoreA, float fWeightA,
    float fScoreB, float fWeightB)
{
    return fScoreA * fWeightA + fScoreB * fWeightB;
}

extern "C" void fn_800A6C94(cTeam*, float);
extern "C" unsigned long fn_800A6EE0(cTeam*);
extern "C" void fn_800A701C(cTeam*);
extern "C" void fn_800A83CC(cTeam*, bool);
extern "C" float fn_800A0508(cFielder*, bool, bool);
extern "C" float fn_80034F98(cFielder*, float);
extern "C" void fn_80015B38(cBall*, bool);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" float fn_800394A8(cFielder*, int);
extern "C" void fn_800180F4(cBall*, nlVector3*, float);
extern "C" bool fn_8003E8A0(const cFielder* pFielder);
extern "C" bool fn_8003E948(const cFielder* pFielder);
extern "C" bool fn_8003E99C(const cFielder* pFielder);
extern "C" bool fn_8003E6EC(cFielder* pFielder);
extern "C" void fn_80031A30(cFielder* pFielder, int nParam, float fParam);
extern "C" void fn_801BB6A4(cFielder* pFielder, int nPowerupIndex);

extern "C" void fn_800A2290(
    SkillTweaks* pTweaks, int difficulty, int param2, bool param3);
extern "C" float fn_8002BE38(PlayerTweaks*);
extern "C" bool fn_800381B4(cFielder*, nlVector3*);
extern "C" cPlayer* fn_800DF790(cTeam*);

/**
 * Offset/Address/Size: 0x32AC | 0x800A8FE0 | size: 0x70
 */
float cTeam::fn_800A8FE0()
{
    float result = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        result += fn_8002BE38(GetFielder(i)->UnidentifiedGetTweaks());
    }
    return result / 4.0f;
}

/**
 * Offset/Address/Size: 0x324C | 0x800A8F80 | size: 0x60
 */
float cTeam::fn_800A8F80()
{
    float fPassRating = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        fPassRating += GetFielder(i)->UnidentifiedGetTweaks()->fPassing;
    }
    return fPassRating / 4.0f;
}

/**
 * Offset/Address/Size: 0x31EC | 0x800A8F20 | size: 0x60
 */
float cTeam::fn_800A8F20()
{
    float fShootRating = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        fShootRating += GetFielder(i)->UnidentifiedGetTweaks()->fShooting;
    }
    return fShootRating / 4.0f;
}

/**
 * Offset/Address/Size: 0x318C | 0x800A8EC0 | size: 0x60
 */
float cTeam::fn_800A8EC0()
{
    float fMovementRating = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        fMovementRating += GetFielder(i)->UnidentifiedGetTweaks()
                               ->mUnidentified034;
    }
    return fMovementRating / 4.0f;
}

/**
 * Offset/Address/Size: 0x5B4 | 0x800A62E8 | size: 0x84
 */
cTeam::~cTeam()
{
    delete m_pNet;
    delete m_pFormationManager;
    mUnidentified0F0->fn_8030F74C(true, true);
    delete mUnidentified0F0;
}

/**
 * Offset/Address/Size: 0x514 | 0x800A6248 | size: 0xA0
 */
void cTeam::fn_800A6248()
{
    for (int i = 0; i < 4; i++)
    {
        mfBallInTimes[i] = 0.0f;
    }

    m_pFormationManager->fn_80050D24();
    mUnidentified0F0->mUnidentified18->UnidentifiedVirtual4(false);

    for (int i = 0; i < 4; i++)
    {
        m_pAIOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        m_pBallInterceptOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        mUnidentified0D8[i] = (cFielder*)m_pPlayers[i];
    }
}

/**
 * Offset/Address/Size: 0x348 | 0x800A607C | size: 0x1CC
 */
void cTeam::fn_800A607C()
{
    for (int i = 0; i < 4; i++)
    {
        mfBallInTimes[i] = 0.0f;
    }

    m_pFormationManager->fn_80050D24();
    mUnidentified0F0->mUnidentified18->UnidentifiedVirtual4(false);

    for (int i = 0; i < 4; i++)
    {
        m_pAIOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        m_pBallInterceptOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        mUnidentified0D8[i] = (cFielder*)m_pPlayers[i];
    }

    int nScore = m_nScore;

    m_ePowerupList[0].nnumOfPowerups = 0;
    m_ePowerupList[0].eType = POWER_UP_NONE;
    m_ePowerupList[0].bIsNew = false;
    m_ePowerupList[1].nnumOfPowerups = 0;
    m_ePowerupList[1].eType = POWER_UP_NONE;
    m_ePowerupList[1].bIsNew = false;

    m_nScore = 0;
    mfPowerupMeter = 0.0f;
    mfPowerupTimer = 0.0f;
    mpCurrentSituation = SITUATION_OFFENSE;
    meCurrentTeamStyle = TEAM_STYLE_MODERATE;
    mUnidentified00C = 0.0f;
    mUnidentified010 = 0.0f;

    mtTeamStyleTimer.UnidentifiedClear();
    mtMarkTimer.UnidentifiedClear();
    mtRoleTimer.UnidentifiedClear();
    mtToggleTimer.UnidentifiedClear();
    mtDefensiveZoneTimer.UnidentifiedClear();

    for (int i = 0; i < 4; i++)
    {
        mfBallInTimes[i] = 0.0f;
        nlVec3Set(mvBallInterceptPosition[i], 0.0f, 0.0f, 0.0f);
    }
    mpBestBallInterceptor = NULL;

    if (GameInfoManager::Instance()->IsInMode4())
    {
        m_nScore = nScore;
    }
}

/**
 * Offset/Address/Size: 0x18 | 0x800A5D4C | size: 0x330
 */
cTeam::cTeam(int nSide)
{
    m_nScore = 0;
    mfPowerupMeter = 0.0f;
    mfPowerupTimer = 0.0f;
    mpCurrentSituation = SITUATION_OFFENSE;
    meCurrentTeamStyle = TEAM_STYLE_MODERATE;
    mUnidentified00C = 0.0f;
    mUnidentified010 = 0.0f;

    mtTeamStyleTimer.UnidentifiedClear();
    mtMarkTimer.UnidentifiedClear();
    mtRoleTimer.UnidentifiedClear();
    mtToggleTimer.UnidentifiedClear();
    mtDefensiveZoneTimer.UnidentifiedClear();

    for (int i = 0; i < 4; i++)
    {
        mfBallInTimes[i] = 0.0f;
        nlVec3Set(mvBallInterceptPosition[i], 0.0f, 0.0f, 0.0f);
    }
    mpBestBallInterceptor = NULL;

    m_nSide = nSide;
    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i] = NULL;
    }
    for (int i = 0; i < 4; i++)
    {
        m_pAIOrderedFielders[i] = NULL;
        m_pBallInterceptOrderedFielders[i] = NULL;
        mUnidentified0D8[i] = NULL;
    }
    mUnidentified0F4 = 0;

    m_pNet = new (8, false) cNet(nSide);
    m_pFormationManager = new (8, false) FormationManager(this);
    mUnidentified0F0 = new (8, false) UnidentifiedFielderInput(this,
        new (8, false) TeamPlayMachine(),
        new (8, false) UnidentifiedFuzzyRuntime());
    mUnidentified0F0->mUnidentified18->UnidentifiedVirtual2();
}

SkillTweaks* fn_800A636C(cTeam* pTeam)
{
    return gGameTweaks.mUnidentified18[pTeam->m_nSide];
}

/**
 * Offset/Address/Size: 0x654 | 0x800A6388 | size: 0x84
 */
float fn_800A6388(cTeam* team)
{
    float result = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        if (team->GetFielder(i)->fn_800344B0())
        {
            result += 1.0f;
        }
    }
    return result;
}

void cTeam::SetDifficulty(int difficulty, int param2, bool param3)
{
    if (difficulty < 0)
    {
        difficulty = 3;
    }

    fn_800A2290(fn_800A636C(this), difficulty, param2, param3);
}

/**
 * Offset/Address/Size: 0x700 | 0x800A6434 | size: 0x1C
 */
void cTeam::ClearAllPowerUps()
{
    m_ePowerupList[0].eType = POWER_UP_NONE;
    m_ePowerupList[0].nnumOfPowerups = 0;
    m_ePowerupList[1].eType = POWER_UP_NONE;
    m_ePowerupList[1].nnumOfPowerups = 0;
}

/**
 * Offset/Address/Size: 0x82C | 0x800A6560 | size: 0x88
 */
bool cTeam::fn_800A6560()
{
    bool result = false;
    if (mtToggleTimer.m_uPackedTime == 0
        && !fn_8003E8A0((cFielder*)m_pPlayers[0])
        && !fn_8003E948((cFielder*)m_pPlayers[0])
        && !fn_8003E99C((cFielder*)m_pPlayers[0])
        && !((cFielder*)m_pPlayers[0])->fn_8003E9F0())
    {
        result = true;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x8B4 | 0x800A65E8 | size: 0x10C
 */
bool cTeam::TogglePowerup(bool bIsSilent)
{
    bool result = false;
    if (mtToggleTimer.m_uPackedTime != 0
        || fn_8003E8A0((cFielder*)m_pPlayers[0])
        || fn_8003E948((cFielder*)m_pPlayers[0])
        || fn_8003E99C((cFielder*)m_pPlayers[0])
        || ((cFielder*)m_pPlayers[0])->fn_8003E9F0())
    {
        result = true;
    }

    if (!result)
    {
        if (m_ePowerupList[0].eType != POWER_UP_NONE
            && m_ePowerupList[1].eType != POWER_UP_NONE)
        {
            PowerUpTeamType eTemp = m_ePowerupList[1];
            m_ePowerupList[1] = m_ePowerupList[0];
            m_ePowerupList[0] = eTemp;
            mtToggleTimer.SetSeconds(lbl_806DBEF0);
        }

        HUDOverlay* HUD
            = (HUDOverlay*)g_pOverlayManager->GetScene(OVERLAY_HUD);
        HUD->SwapPowerUps(m_nSide);
        return true;
    }
    return false;
}

/**
 * Offset/Address/Size: 0xA30 | 0x800A6764 | size: 0xE0
 */
bool cTeam::fn_800A6764() const
{
    cFielder* pCaptain = (cFielder*)m_pPlayers[0];
    int nCaptainPowerup = pCaptain->mUnidentified11C->unknown_0x14;
    bool bCaptainPowerupActive
        = m_ePowerupList[0].eType == nCaptainPowerup;
    bCaptainPowerupActive
        |= m_ePowerupList[1].eType == nCaptainPowerup;

    if (pCaptain->fn_8003E74C()
        || fn_8003E8A0(pCaptain)
        || fn_8003E948(pCaptain)
        || fn_8003E99C(pCaptain)
        || pCaptain->fn_8003E9F0())
    {
        bCaptainPowerupActive = true;
    }

    if (pCaptain->mUnidentified024.m_eCharacterClass == (eCharacterClass)5
        && gPeachPhotoState.state == 1)
    {
        bCaptainPowerupActive = true;
    }

    return bCaptainPowerupActive;
}

/**
 * Offset/Address/Size: 0x9C0 | 0x800A66F4 | size: 0x70
 */
bool cTeam::IncrementPowerupMeter(
    float fAdjustAmount, cFielder* pFielder, bool param3)
{
    mfPowerupMeter += fAdjustAmount;
    if (mfPowerupMeter >= 1.0f)
    {
        mfPowerupMeter -= 1.0f;
        int nPowerupIndex = PowerupBase::AwardPowerup(this, pFielder, param3);
        if (nPowerupIndex != -1)
        {
            fn_801BB6A4(pFielder, nPowerupIndex);
            return true;
        }
    }
    return false;
}

/**
 * Offset/Address/Size: 0xB10 | 0x800A6844 | size: 0x1C
 */
PowerUpTeamType cTeam::GetCurrentPowerUp() const
{
    return m_ePowerupList[0];
}

/**
 * Offset/Address/Size: 0xB2C | 0x800A6860 | size: 0x58
 */
PowerUpTeamType cTeam::GetPowerUpByIndex(int index) const
{
    PowerUpTeamType eDummy;
    if (index >= 0)
    {
        return m_ePowerupList[index];
    }
    eDummy.eType = POWER_UP_NONE;
    eDummy.nnumOfPowerups = 0;
    return eDummy;
}

/**
 * Offset/Address/Size: 0xB84 | 0x800A68B8 | size: 0x18
 */
void cTeam::SetIsPowerUpNew(int index, bool isNew)
{
    if (index >= 0)
    {
        m_ePowerupList[index].bIsNew = isNew;
    }
}

/**
 * Offset/Address/Size: 0xB9C | 0x800A68D0 | size: 0x58
 */
int cTeam::SetCurrentPowerUp(
    ePowerUpType eNewPowerUpType, int nnumOfPowerups)
{
    unsigned char bGivenNewPowerup = 0;
    for (int a = 0; a < 2; ++a)
    {
        if (m_ePowerupList[a].eType == POWER_UP_NONE && !bGivenNewPowerup)
        {
            m_ePowerupList[a].eType = eNewPowerUpType;
            m_ePowerupList[a].nnumOfPowerups = nnumOfPowerups;
            m_ePowerupList[a].bIsNew = 1;
            bGivenNewPowerup = 1;
        }
    }
    return bGivenNewPowerup;
}

/**
 * Offset/Address/Size: 0xBF4 | 0x800A6928 | size: 0x24
 */
void cTeam::SetPlayer(cPlayer* pPlayer, int nIndex)
{
    m_pPlayers[nIndex] = pPlayer;
    if (nIndex < 4)
    {
        m_pAIOrderedFielders[nIndex] = (cFielder*)pPlayer;
        m_pBallInterceptOrderedFielders[nIndex] = (cFielder*)pPlayer;
        mUnidentified0D8[nIndex] = (cFielder*)pPlayer;
    }
}

/**
 * Offset/Address/Size: 0xC18 | 0x800A694C | size: 0x8
 */
void cTeam::SetGoalie(Goalie* pGoalie)
{
    m_pPlayers[4] = pGoalie;
}

/**
 * Offset/Address/Size: 0xC20 | 0x800A6954 | size: 0x8
 */
Goalie* cTeam::GetGoalie()
{
    return (Goalie*)m_pPlayers[4];
}

extern "C" UnidentifiedFuzzyRuntimeBase* fn_800A695C(cTeam* pTeam)
{
    return pTeam->mUnidentified0F0->mUnidentified14;
}

extern "C" UnidentifiedScriptMachine* fn_800A6968(cTeam* pTeam)
{
    return pTeam->mUnidentified0F0->mUnidentified18;
}

/**
 * Offset/Address/Size: 0xC40 | 0x800A6974 | size: 0x88
 */
cPlayer* cTeam::GetControlledPlayer(cGlobalPad* pController)
{
    IsNetworkOrRecordedGame();
    cPlayer* pRetval = NULL;
    for (int i = 0; i < 5; i++)
    {
        DetInput* pUnidentifiedInput = m_pPlayers[i]->GetGlobalPad();
        cGlobalPad* pUnidentifiedController = NULL;
        if (pUnidentifiedInput != NULL)
        {
            pUnidentifiedController = ((NetworkPeerChannel*)pUnidentifiedInput->m_pMyUser)->GetLocalChannelPad();
        }
        if (pUnidentifiedController == pController)
        {
            pRetval = m_pPlayers[i];
            break;
        }
    }
    return pRetval;
}

/**
 * Offset/Address/Size: 0xCC8 | 0x800A69FC | size: 0x78
 */
int cTeam::GetNumAssignedControllers()
{
    int mySide, numAssignedControllers;
    unsigned short i;
    short playingSide;

    numAssignedControllers = 0;
    for (i = 0; i < 16; i++)
    {
        mySide = m_nSide;
        playingSide = GameInfoManager::Instance()->GetPlayingSide(i);
        if (playingSide == mySide)
        {
            numAssignedControllers++;
        }
    }
    return numAssignedControllers;
}

/**
 * Offset/Address/Size: 0xD40 | 0x800A6A74 | size: 0x10
 */
cFielder* cTeam::GetFielder(int nIndex)
{
    return (cFielder*)m_pPlayers[nIndex];
}

/**
 * Offset/Address/Size: 0xD50 | 0x800A6A84 | size: 0x10
 */
cPlayer* cTeam::GetPlayer(int nIndex)
{
    return m_pPlayers[nIndex];
}

/**
 * Offset/Address/Size: 0xD60 | 0x800A6A94 | size: 0x18
 */
cTeam* cTeam::GetOtherTeam()
{
    return g_pTeams[m_nSide == HOME ? AWAY : HOME];
}

/**
 * Offset/Address/Size: 0xD78 | 0x800A6AAC | size: 0x1C
 */
cNet* cTeam::GetOtherNet()
{
    return g_pTeams[m_nSide == HOME ? AWAY : HOME]->m_pNet;
}

/**
 * Offset/Address/Size: 0xD94 | 0x800A6AC8 | size: 0xBC
 */
nlVector3 cTeam::GetAIOffNetLocation(const nlVector3* v3ReferencePos)
{
    nlVector3 v3NetLocation = GetOtherNet()->m_v3NetLocation;
    float yCoord = (v3ReferencePos != NULL) ? v3ReferencePos->y : 0.0f;
    float fNetWidth = cNet::m_fNetWidth;
    fNetWidth = 0.5f * fNetWidth;

    if (yCoord < 0.0f)
    {
        yCoord = max_float(yCoord, -1.0f * fNetWidth);
        v3NetLocation.y = yCoord;
    }
    else
    {
        yCoord = min_float(yCoord, fNetWidth);
        v3NetLocation.y = yCoord;
    }

    return v3NetLocation;
}

/**
 * Offset/Address/Size: 0xE50 | 0x800A6B84 | size: 0xA8
 */
nlVector3 cTeam::GetAIDefNetLocation(const nlVector3* v3ReferencePos)
{
    nlVector3 v3NetLocation = m_pNet->m_v3NetLocation;
    float yCoord = (v3ReferencePos != NULL) ? v3ReferencePos->y : 0.0f;

    float fNetWidth = cNet::m_fNetWidth;
    fNetWidth = 0.5f * fNetWidth;

    if (yCoord < 0.0f)
    {
        yCoord = max_float(yCoord, -1.0f * fNetWidth);
        v3NetLocation.y = yCoord;
    }
    else
    {
        yCoord = min_float(yCoord, fNetWidth);
        v3NetLocation.y = yCoord;
    }

    return v3NetLocation;
}

/**
 * Offset/Address/Size: 0xEF8 | 0x800A6C2C | size: 0x68
 */
void cTeam::PreUpdate(float fDeltaT)
{
    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->PreUpdate(fDeltaT);
    }
}

/**
 * Offset/Address/Size: 0xF60 | 0x800A6C94 | size: 0x184
 */
extern "C" void fn_800A6C94(cTeam* pTeam, float fDeltaT)
{
    if ((g_pGame->IsGameplayOrOvertime()
            || g_pGame->GetGameState() == 1)
        && !g_pGame->mbCaptainShotToScoreOn)
    {
        pTeam->mfPowerupTimer -= fDeltaT;
        if (pTeam->mfPowerupTimer < 0.0f)
        {
            pTeam->mfPowerupTimer = lbl_806DBEF4;
            if (GameInfoManager::Instance()->IsRule0x0Equal10()
                || lbl_806E0E04)
            {
                PowerupBase::AwardPowerup(pTeam, NULL, false);
            }
        }
    }

    if (g_pGame->IsGameplayOrOvertime())
    {
        pTeam->mtTeamStyleTimer.Countdown(fDeltaT, 0.0f);
        pTeam->mtMarkTimer.Countdown(fDeltaT, 0.0f);
        pTeam->mtRoleTimer.Countdown(fDeltaT, 0.0f);
        pTeam->mtToggleTimer.Countdown(fDeltaT, 0.0f);

        float offensive = Offensive(pTeam);
        if (offensive)
        {
            if (Stalling(pTeam) < 1.0f)
            {
                pTeam->mtDefensiveZoneTimer.Countup(fDeltaT, 10.0f);
            }
        }
        else
        {
            pTeam->mtDefensiveZoneTimer.Countdown(
                2.0f * fDeltaT, 0.0f);
        }
    }
}

/**
 * Offset/Address/Size: 0x10E4 | 0x800A6E18 | size: 0xC4
 */
void cTeam::Update(float fDeltaT)
{
    g_pCurrentlyUpdatingTeam = this;
    fn_800A6C94(this, fDeltaT);
    CalculateNewBallInterceptTimes();

    if (mpBestBallInterceptor == NULL)
    {
        mpBestBallInterceptor = m_pBallInterceptOrderedFielders[0];
    }
    else if (mpBestBallInterceptor != m_pBallInterceptOrderedFielders[0])
    {
        float fScore1 = AbleToInterceptBall(mpBestBallInterceptor);
        float fScore2 = AbleToInterceptBall(m_pBallInterceptOrderedFielders[0]);
        if (fScore2 - fScore1 > 0.125f)
        {
            mpBestBallInterceptor = m_pBallInterceptOrderedFielders[0];
        }
    }

    UpdateTeamAI(fDeltaT);
    fn_800A8098();
    fn_800A701C(this);
}

/**
 * Offset/Address/Size: 0x11A8 | 0x800A6EDC | size: 0x4
 */
void cTeam::StopGameplayEffectsAndSounds()
{
    fn_800A701C(this);
}

/**
 * Offset/Address/Size: 0x11AC | 0x800A6EE0 | size: 0x13C
 */
extern "C" unsigned long fn_800A6EE0(cTeam* pTeam)
{
    unsigned long result = 0;
    switch (pTeam->m_pPlayers[0]->mUnidentified11C->unknown_0x14)
    {
    case 9:
        result = 0xF1B432C3;
        break;
    case 10:
        result = 0xE8DC557A;
        break;
    case 11:
        result = 0xB7B862AC;
        break;
    case 12:
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            result = 0x52F53867;
        }
        else
        {
            result = 0x2424F09D;
        }
        break;
    case 13:
        result = 0xAE3706CD;
        break;
    case 14:
        result = 0xF2D97BFD;
        break;
    case 15:
        result = 0x42C3BE45;
        break;
    case 16:
        result = 0xB83BDCC5;
        break;
    case 17:
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            result = 0x392661A3;
        }
        else if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangSpanish)
        {
            result = 0x62BEEA94;
        }
        else
        {
            result = 0xB62C6459;
        }
        break;
    case 18:
        result = 0x505B79C8;
        break;
    case 19:
        result = 0x0A9FD837;
        break;
    case 20:
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            result = 0x8192CDBC;
        }
        else if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangGerman)
        {
            result = 0xE3E1D62C;
        }
        else
        {
            result = 0x4E5AC452;
        }
        break;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x12E8 | 0x800A701C | size: 0x18C
 */
extern "C" void fn_800A701C(cTeam* pTeam)
{
    if (!GetStadiumUnknown0x10(
            GameInfoManager::Instance()->GetStadium()))
    {
        return;
    }

    unsigned long nCueId = fn_800A6EE0(pTeam);
    if (nCueId == 0)
    {
        return;
    }

    bool bPlayCaptainChant = false;
    cFielder* pCaptain = (cFielder*)pTeam->m_pPlayers[0];
    int nCaptainPowerup = pCaptain->mUnidentified11C->unknown_0x14;
    bool bCaptainPowerupActive
        = pTeam->m_ePowerupList[0].eType == nCaptainPowerup;
    bCaptainPowerupActive
        |= pTeam->m_ePowerupList[1].eType == nCaptainPowerup;

    if (bCaptainPowerupActive
        && GameInfoManager::Instance()->GetRule0x0() != 9
        && !fn_8003E6EC(pCaptain))
    {
        bPlayCaptainChant = true;
    }

    if (bPlayCaptainChant && g_pGame->IsGameplayOrOvertime())
    {
        if (IsSoundTracked(nCueId, pTeam))
        {
            return;
        }

        unsigned long nCurrentTime = (unsigned long)(OSGetTime()
            / ((*(unsigned long*)0x800000F8 >> 2) / 1000));
        if (nCurrentTime - lbl_806E0E08[0] <= 1200)
        {
            return;
        }

        if (!GetTweakBool("user/RestrictStreams", false))
        {
            PlayCaptainChant(14, nCueId, pTeam);
        }
        lbl_806E0E08[0] = nCurrentTime;
    }
    else
    {
        StopCaptainChant(nCueId, pTeam);
    }
}

/**
 * Offset/Address/Size: 0x1850 | 0x800A7584 | size: 0x414
 */
void cTeam::ResetCharacters()
{
    const unsigned short* pFacingDirectionTable;
    unsigned char bFlipPositions;
    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->SetAIPad(NULL);
    }

    UpdateControllers();

    const FormationSpec* pFormation;
    if (g_pGame->m_nLastTeamToScore
            != g_pTeams[m_nSide == HOME ? AWAY : HOME]->m_nSide
        || (g_pTeams[m_nSide == HOME ? AWAY : HOME]->m_nScore == 0
            && m_nScore == 0))
    {
        pFacingDirectionTable = g_aNeutralPlayerFacingDirections;
        pFormation = FormationManager::GetFormationSpec(
            (eFormation)nlStringHash("Kickoff Neutral"));
    }
    else
    {
        pFacingDirectionTable = g_aAdvantagePlayerFacingDirections;
        pFormation = FormationManager::GetFormationSpec(
            (eFormation)nlStringHash("Kickoff Advantage"));
    }

    bFlipPositions = 0;
    if (GetOtherTeam()->m_pNet->m_v3NetLocation.x < 0.0f)
    {
        bFlipPositions = 1;
    }

    int i;
    for (i = 0; i < 5; i++)
    {
        cFielder* pFielder = GetFielder(i);
        pFielder->m_Dirt = 0.0f;
        pFielder->mUnidentified16C = 0;
        pFielder->m_MinDirt = 0.0f;

        if (i == 0 && GetNumAssignedControllers() > 1)
        {
            cPlayer* pPlayers[5] = {
                m_pPlayers[0],
                m_pPlayers[1],
                m_pPlayers[2],
                m_pPlayers[3],
                m_pPlayers[4],
            };

            for (int j = 0; j < 5; j++)
            {
                int nRandomPlayer = nlRandom(5);
                if (nRandomPlayer != j)
                {
                    cPlayer* pTemp = pPlayers[j];
                    pPlayers[j] = pPlayers[nRandomPlayer];
                    pPlayers[nRandomPlayer] = pTemp;
                }
            }

            for (int j = 0; j < 5; j++)
            {
                cAIPad* pPad;
                cPlayer* pPlayer = pPlayers[j];
                if (pPlayer->m_pController != NULL)
                {
                    if (pFielder != pPlayer)
                    {
                        pPad = pFielder->m_pController;
                        pFielder->SetAIPad(pPlayer->m_pController);
                        pPlayer->SetAIPad(pPad);
                    }
                    break;
                }
            }
        }

        nlVector3 v3NewPosition;
        unsigned short aNewFacingDirection = pFacingDirectionTable[i];

        if (i < 4)
        {
            nlVector2 v2Position;
            pFormation->m_Positions[i].GetLocationForTeam(
                v2Position, pFielder->m_pTeam->m_nSide);
            nlVec3Set(v3NewPosition, v2Position.x, v2Position.y, 0.0f);
        }
        else
        {
            nlVec3Set(v3NewPosition,
                bFlipPositions ? 18.0f : -18.0f, 0.0f, 0.0f);
        }

        if (m_nScore == 0 && GetOtherTeam()->m_nScore == 0)
        {
            aNewFacingDirection = 0;
            if (i == 0)
            {
                v3NewPosition.y = 0.0f;
            }
        }

        if (bFlipPositions)
        {
            aNewFacingDirection += ((s16)(0x4000 - aNewFacingDirection)) * 2;
        }
        else
        {
            v3NewPosition.y = -v3NewPosition.y;
        }

        pFielder->Unknown10(v3NewPosition, aNewFacingDirection);
    }

    s32 side = m_nSide;
    s32 i_player = 0;
    do
    {
        g_pTeams[side]->m_pPlayers[i_player]->StopPlayingAllTrackedSFX();
        i_player++;
    } while (i_player < 5);
    mfPowerupTimer = 0.0f;

    if (GameInfoManager::Instance()->IsInMode4())
    {
        int nMissingSidekicks
            = g_pStrikerChallenge->mMissingSidekicks[m_nSide];
        if (!(g_pStrikerChallenge->mCurrentChallenge != 4
                && g_pStrikerChallenge->mCurrentChallenge != 5))
        {
            for (int i = 0; i < nMissingSidekicks; i++)
            {
                cFielder* pFielder = GetFielder(i);
                if (!pFielder->IsShattered())
                {
                    fn_80031A30(pFielder, 4, 99999.0f);
                }
            }
        }
        else
        {
            for (int i = 3; i >= 0; i--)
            {
                if (nMissingSidekicks > 0)
                {
                    cFielder* pFielder = GetFielder(i);
                    if (!pFielder->IsShattered())
                    {
                        fn_80031A30(pFielder, 4, 99999.0f);
                    }
                    nMissingSidekicks--;
                }
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x1CD4 | 0x800A7A08 | size: 0x8
 */
bool cTeam::CalculateFormationPosition(nlVector3& v3DestPosition,
    cFielder* pFielder, bool bInPosition,
    float fBallPosFormationWeight)
{
    return m_pFormationManager->CalculateFielderPosition(
        v3DestPosition, pFielder, bInPosition, fBallPosFormationWeight);
}

/**
 * Offset/Address/Size: 0x1CDC | 0x800A7A10 | size: 0x498
 */
void cTeam::CalculateNewBallInterceptTimes()
{
    cPlayer* pPlayer;
    nlVector3* pBallPosition;
    float fScores[4];

    for (int i = 0; i < 4; i++)
    {
        pPlayer = GetPlayer(i);
        float interceptTime = -1.0f;
        float speed = fn_8002E1B0((cFielder*)pPlayer);
        float radius = pPlayer->mUnidentified320->GetRadius();

        if (Incapacitated(pPlayer))
        {
            interceptTime = 5.0f;
            fn_800180F4(
                g_pBall, &mvBallInterceptPosition[i], interceptTime);
        }
        else if (pPlayer->m_pBall != NULL)
        {
            interceptTime = 0.0f;
            mvBallInterceptPosition[i] = g_pBall->m_v3Position;
        }
        else if (g_pBall->GetPassTargetFielder() == pPlayer)
        {
            interceptTime = g_pBall->m_tPassTargetTimer.GetSeconds();
            mvBallInterceptPosition[i] = g_pBall->m_v3PassIntercept;
        }
        else
        {
            int nNumSolutions;
            float pSolutions[2];
            float fContactHeight = fn_800394A8((cFielder*)pPlayer, 1);
            float fBallHeight = g_pBall->m_v3Position.z;
            if (fBallHeight > fContactHeight)
            {
                float fOtherContactHeight
                    = fn_800394A8((cFielder*)pPlayer, 0);
                if (fBallHeight < fOtherContactHeight)
                {
                    fOtherContactHeight = fContactHeight;
                }

                nlVector3 v3PredictedLandingSpot;
                interceptTime = g_pBall->PredictLandingSpotAndTime(
                    v3PredictedLandingSpot, NULL, NULL,
                    fOtherContactHeight);
                if (interceptTime > 0.0f)
                {
                    nlVector2 v2Delta = {
                        v3PredictedLandingSpot.x
                            - pPlayer->mUnidentified024.m_v3Position.x,
                        v3PredictedLandingSpot.y
                            - pPlayer->mUnidentified024.m_v3Position.y,
                    };
                    float fDistance
                        = nlSqrt(nlVec2LengthSquared(v2Delta), true)
                        - radius;
                    interceptTime = (fDistance < 0.0f)
                        ? 0.0f
                        : fDistance / speed;
                }
                mvBallInterceptPosition[i] = v3PredictedLandingSpot;
            }

            if (interceptTime < 0.0f)
            {
                nNumSolutions = 0;
                pBallPosition = &g_pBall->m_v3Position;
                nlVector3* pAIVelocity = g_pBall->GetAIVelocity();
                CalcInterceptXY(pPlayer->mUnidentified024.m_v3Position,
                    speed, radius, *pBallPosition, *pAIVelocity,
                    nNumSolutions, pSolutions);

                if (nNumSolutions != 0)
                {
                    if (nNumSolutions == 2)
                    {
                        float solution1 = pSolutions[1];
                        interceptTime = pSolutions[0];
                        interceptTime = (interceptTime <= solution1)
                            ? interceptTime
                            : solution1;
                    }
                    else
                    {
                        interceptTime = pSolutions[0];
                    }

                    fn_800180F4(g_pBall, &mvBallInterceptPosition[i],
                        (interceptTime <= 2.0f) ? interceptTime : 2.0f);
                }
                else
                {
                    bool bUsePassTarget = false;
                    bool bAirborne = true;
                    if (g_pBall->meBallState != 5
                        && g_pBall->meBallState != 3)
                    {
                        bAirborne = false;
                    }
                    if (bAirborne && g_pBall->m_pPassTarget != NULL)
                    {
                        bUsePassTarget = true;
                    }
                    if (bUsePassTarget)
                    {
                        mvBallInterceptPosition[i]
                            = g_pBall->m_v3PassIntercept;
                        nlVector2 v2Delta = {
                            mvBallInterceptPosition[i].x
                                - pPlayer->mUnidentified024.m_v3Position.x,
                            mvBallInterceptPosition[i].y
                                - pPlayer->mUnidentified024.m_v3Position.y,
                        };
                        float fDistance
                            = nlSqrt(nlVec2LengthSquared(v2Delta), true)
                            - radius;
                        interceptTime = (fDistance < 0.0f)
                            ? 0.0f
                            : fDistance / speed;
                    }
                    else
                    {
                        mvBallInterceptPosition[i] = g_pBall->m_v3Position;
                        nlVector2 v2Delta = {
                            mvBallInterceptPosition[i].x
                                - pPlayer->mUnidentified024.m_v3Position.x,
                            mvBallInterceptPosition[i].y
                                - pPlayer->mUnidentified024.m_v3Position.y,
                        };
                        float fDistance
                            = nlSqrt(nlVec2LengthSquared(v2Delta), true)
                            - radius;
                        interceptTime = (fDistance < 0.0f)
                            ? 0.0f
                            : fDistance / speed;
                    }
                }
            }
        }

        cField::FixOutOfBoundsPosition(
            mvBallInterceptPosition[i], 0.2f, true);
        mfBallInTimes[i] = interceptTime;
        fScores[i] = AbleToInterceptBall(pPlayer);
        m_pBallInterceptOrderedFielders[i] = (cFielder*)pPlayer;
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = i + 1; j < 4; j++)
        {
            if (fScores[j] > fScores[i])
            {
                float fScore = fScores[j];
                fScores[j] = fScores[i];
                fScores[i] = fScore;

                cFielder* pFielder = m_pBallInterceptOrderedFielders[j];
                m_pBallInterceptOrderedFielders[j]
                    = m_pBallInterceptOrderedFielders[i];
                m_pBallInterceptOrderedFielders[i] = pFielder;
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x1C64 | 0x800A7998 | size: 0x70
 */
void cTeam::fn_800A7998()
{
    s32 side = m_nSide;
    s32 i_player = 0;
    do
    {
        g_pTeams[side]->m_pPlayers[i_player]->StopPlayingAllTrackedSFX();
        i_player++;
    } while (i_player < 5);
}

/**
 * Offset/Address/Size: 0x2174 | 0x800A7EA8 | size: 0x50
 */
extern "C" int fn_800A7EA8(const void* a, const void* b)
{
    cFielder* p1 = *(cFielder**)a;
    cFielder* p2 = *(cFielder**)b;

    float fPosition1 = p1->mUnidentified024.m_v3Position.x;
    float fPosition2 = p2->mUnidentified024.m_v3Position.x;
    if (p1->m_pTeam->m_nSide == AWAY)
    {
        fPosition1 = -fPosition1;
        fPosition2 = -fPosition2;
    }

    if (fPosition1 == fPosition2)
    {
        return 0;
    }
    if (fPosition1 > fPosition2)
    {
        return -1;
    }
    return 1;
}

/**
 * Offset/Address/Size: 0x21C4 | 0x800A7EF8 | size: 0x1A0
 */
void cTeam::UpdateTeamAI(float fDeltaT)
{
    if (mtTeamStyleTimer.m_uPackedTime == 0)
    {
        meCurrentTeamStyle = TEAM_STYLE_AGGRESSIVE;
        mtTeamStyleTimer.SetSeconds(1.0f);
    }

    qsort(mUnidentified0D8, 4, 4, fn_800A7EA8);

    bool bSituationChanged = AssignSituation();
    if (bSituationChanged)
    {
        m_pFormationManager->ChooseNewFormations();
    }

    m_pFormationManager->Update(fDeltaT);
    fn_800A83CC(this, bSituationChanged);
    AssignMarks(bSituationChanged);

    if (!UserControlledT(this)
        && ShootToScoreMeter::instance.m_bMeterVisible)
    {
        float fRumbleChance = 1.75f * Difficult(this);
        if (nlRandomf(10.0f, &nlDefaultSeed) < fRumbleChance
            && fRumbleChance > 1.0f)
        {
            cFielder* pBallOwner = g_pBall->GetOwnerFielder();
            if (pBallOwner != NULL && pBallOwner != GetCaptain()
                && !GetCaptain()->IsOnSameTeam(pBallOwner))
            {
                u16 angle = (u16)nlRandom(0xFFFF, &nlDefaultSeed);
                ShootToScoreMeter::instance.RumbleMeter(angle);
                PlayRumbleAction(1, pBallOwner->GetGlobalPad());
            }
        }
    }

    mUnidentified0F0->fn_8030F800(true, fDeltaT);
}

/**
 * Offset/Address/Size: 0x2364 | 0x800A8098 | size: 0x1EC
 */
void cTeam::fn_800A8098()
{
    if (g_pBall->GetOwnerFielder() != NULL)
    {
        cPlayer* pCaptain = m_pPlayers[0];
        if (pCaptain->IsOnSameTeam(g_pBall->GetOwnerFielder()))
        {
            bool bIsChipShot = g_pBall->GetOwnerFielder()->bIsModified;
            if (g_pBall->GetOwnerFielder()->GetGlobalPad() != NULL)
            {
                bIsChipShot = g_pBall->GetOwnerFielder()->GetGlobalPad()->IsPressed(0x17, true);
            }

            float fScoreValue = fn_800A0508(
                g_pBall->GetOwnerFielder(), bIsChipShot, false);
            mUnidentified010 = fScoreValue;
            mUnidentified00C = nlMinEquals(
                nlMaxEquals(
                    fn_80034F98(g_pBall->GetOwnerFielder(),
                        fScoreValue)
                        / 100.0f,
                    0.0f),
                1.0f);
        }
    }
    else
    {
        bool bUnidentified = false;
        if (g_pBall->meBallState == 5
            || g_pBall->meBallState == 3)
        {
            if (g_pBall->m_pPassTarget != NULL)
            {
                bUnidentified = true;
            }
        }
        if (bUnidentified
            && g_pBall->GetPassTargetFielder() != NULL)
        {
            cPlayer* pCaptain = m_pPlayers[0];
            if (pCaptain->IsOnSameTeam(
                    g_pBall->GetPassTargetFielder()))
            {
                float fScoreValue = fn_800A0508(
                    g_pBall->GetPassTargetFielder(),
                    g_pBall->GetPassTargetFielder()->bIsModified, false);
                mUnidentified010 = fScoreValue;
                mUnidentified00C = nlMinEquals(
                    nlMaxEquals(
                        fn_80034F98(g_pBall->GetPassTargetFielder(),
                            fScoreValue)
                            / 100.0f,
                        0.0f),
                    1.0f);
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x2550 | 0x800A8284 | size: 0x148
 */
bool cTeam::AssignSituation()
{
    cPlayer* pBallOwner = g_pBall->m_pOwner;
    eSituation eLastSituation = mpCurrentSituation;

    if (pBallOwner == NULL)
    {
        pBallOwner = g_pBall->m_pPassTarget;
        if ((pBallOwner != NULL) && (pBallOwner->m_eClassType == FIELDER))
        {
            if (!ReceivingPass((cFielder*)pBallOwner))
            {
                nlPrintf("cTeam::AssignSituation - caught bad pass case, with no proper receiver.\n");
                fn_80015B38(g_pBall, false);
                pBallOwner = NULL;
            }
        }
    }

    if (pBallOwner != NULL)
    {
        if (pBallOwner->m_pTeam == this
            && pBallOwner->m_eClassType != GOALIE)
        {
            if (mpCurrentSituation != SITUATION_OFFENSE)
            {
                mpCurrentSituation = SITUATION_OFFENSE;
                meCurrentTeamStyle = TEAM_STYLE_AGGRESSIVE;
                mtTeamStyleTimer.SetSeconds(1.0f);
            }
        }
        else if (mpCurrentSituation != SITUATION_DEFENSE)
        {
            mpCurrentSituation = SITUATION_DEFENSE;
            meCurrentTeamStyle = TEAM_STYLE_AGGRESSIVE;
            mtTeamStyleTimer.SetSeconds(1.0f);
        }
    }
    else if (mpCurrentSituation != SITUATION_LOOSE)
    {
        mpCurrentSituation = SITUATION_LOOSE;
        meCurrentTeamStyle = TEAM_STYLE_AGGRESSIVE;
        mtTeamStyleTimer.SetSeconds(1.0f);
    }

    return eLastSituation != mpCurrentSituation;
}

/**
 * Offset/Address/Size: 0x2698 | 0x800A83CC | size: 0xD0
 */
extern "C" void fn_800A83CC(cTeam* pTeam, bool bSituationChanged)
{
    if (pTeam->mtRoleTimer.m_uPackedTime == 0 || bSituationChanged)
    {
        unsigned int* pFielderFormationPos
            = pTeam->m_pFormationManager->fn_80052034();
        if (pFielderFormationPos != NULL)
        {
            switch (pTeam->mpCurrentSituation)
            {
            case SITUATION_OFFENSE:
            case SITUATION_DEFENSE:
            case SITUATION_LOOSE:
                unsigned int posIndex = pFielderFormationPos[0];
                cFielder* pFielder = (cFielder*)pTeam->m_pPlayers[0];
                pTeam->m_pAIOrderedFielders[posIndex] = pFielder;
                pFielder->m_eRole = (eRole)posIndex;

                posIndex = pFielderFormationPos[1];
                pFielder = (cFielder*)pTeam->m_pPlayers[1];
                pTeam->m_pAIOrderedFielders[posIndex] = pFielder;
                pFielder->m_eRole = (eRole)posIndex;

                posIndex = pFielderFormationPos[2];
                pFielder = (cFielder*)pTeam->m_pPlayers[2];
                pTeam->m_pAIOrderedFielders[posIndex] = pFielder;
                pFielder->m_eRole = (eRole)posIndex;

                posIndex = pFielderFormationPos[3];
                pFielder = (cFielder*)pTeam->m_pPlayers[3];
                pTeam->m_pAIOrderedFielders[posIndex] = pFielder;
                pFielder->m_eRole = (eRole)posIndex;
                break;
            }

            pTeam->mtRoleTimer.SetSeconds(0.33f);
        }
    }
}

/**
 * Offset/Address/Size: 0x2768 | 0x800A849C | size: 0x35C
 */
void cTeam::AssignMarks(bool bForceReMark)
{
    cFielder* pMyFielder;
    cFielder* pOppFielder;

    if (mpCurrentSituation == SITUATION_OFFENSE)
    {
        return;
    }

    if (mtMarkTimer.m_uPackedTime != 0 && !bForceReMark)
    {
        return;
    }

    cFielder* pUnidentifiedFielder =
        (cFielder*)fn_800DF790(GetOtherTeam());
    float fDownfield;
    float fScore;
    cFielder* pUnidentifiedBestFielder = NULL;
    float fUnidentifiedBestScore = 0.0f;

    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pMyFielder = GetFielder(i_fielder);
        pMyFielder->fn_800306DC();
        bool bUnidentified = pMyFielder->fn_800344B0()
                          || pMyFielder->IsShattered()
                          || Incapacitated(pMyFielder);
        if (!bUnidentified)
        {
            fDownfield = DownfieldFrom(
                pMyFielder, pUnidentifiedFielder);
            float fInBetween =
                InBetweenMyNetAnd(pMyFielder, pUnidentifiedFielder);
            fScore = WeightedScore2(
                fInBetween, 0.4f, fDownfield, 0.6f);
            if (fScore > fUnidentifiedBestScore)
            {
                fUnidentifiedBestScore = fScore;
                pUnidentifiedBestFielder = pMyFielder;
            }
        }
    }

    if (pUnidentifiedBestFielder != NULL
        && pUnidentifiedFielder != NULL)
    {
        pUnidentifiedBestFielder->fn_800306A0(pUnidentifiedFielder);
    }

    float fFielderMarkScores[4][4];
    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pMyFielder = GetFielder(i_fielder);
        bool bUnidentifiedMyFielder = pMyFielder->fn_800344B0()
                                   || pMyFielder->IsShattered();

        nlVector3 v3UnidentifiedPosition;
        fn_800381B4(pMyFielder, &v3UnidentifiedPosition);

        for (int i_otherf = 0; i_otherf < 4; i_otherf++)
        {
            pOppFielder = GetOtherTeam()->GetFielder(i_otherf);
            bool bUnidentifiedOppFielder = pOppFielder->fn_800344B0()
                                        || pOppFielder->IsShattered();

            if (bUnidentifiedMyFielder && !bUnidentifiedOppFielder)
            {
                fFielderMarkScores[i_fielder][i_otherf] = 200.0f;
            }
            else
            {
                fFielderMarkScores[i_fielder][i_otherf] = 0.5f
                    * nlSqrt(nlVec3DistanceSquared2D(
                        pOppFielder->mUnidentified024.m_v3Position,
                        v3UnidentifiedPosition), true);
                fFielderMarkScores[i_fielder][i_otherf] += 0.5f
                    * nlSqrt(nlVec3DistanceSquared2D(
                        pOppFielder->mUnidentified024.m_v3Position,
                        pMyFielder->mUnidentified024.m_v3Position), true);
            }
        }
    }

    unsigned int pMarkIDs[4];
    SortToMinOrMaxTotalSum(pMarkIDs, fFielderMarkScores, true);

    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pMarkIDs[i_fielder]
            = nlMin(nlMax((int)pMarkIDs[i_fielder], 0), 3);
        GetFielder(i_fielder)->fn_800306A0(
            GetOtherTeam()->GetFielder(pMarkIDs[i_fielder]));
    }

    mtMarkTimer.SetSeconds(0.5f);
}

/**
 * Offset/Address/Size: 0x2AC4 | 0x800A87F8 | size: 0x8
 */
cFielder* cTeam::GetCaptain()
{
    return (cFielder*)m_pPlayers[0];
}

/**
 * Offset/Address/Size: 0x2ACC | 0x800A8800 | size: 0x8
 */
cFielder* cTeam::GetStriker() const
{
    return m_pAIOrderedFielders[0];
}

/**
 * Offset/Address/Size: 0x2AD4 | 0x800A8808 | size: 0x7C
 */
cFielder* cTeam::GetFrontMostFielder()
{
    cFielder* pFielder;
    cFielder* pFrontMostFielder = NULL;

    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pFielder = (cFielder*)m_pPlayers[i_fielder];
        if ((pFrontMostFielder == NULL)
            || (pFielder->mUnidentified1E4.m_v3AIPosition.x > pFrontMostFielder->mUnidentified1E4.m_v3AIPosition.x))
        {
            pFrontMostFielder = pFielder;
        }
    }

    return pFrontMostFielder;
}

/**
 * Offset/Address/Size: 0x2B50 | 0x800A8884 | size: 0x7C
 */
cFielder* cTeam::GetRearMostFielder()
{
    cFielder* pFielder;
    cFielder* pRearMostFielder = NULL;

    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pFielder = (cFielder*)m_pPlayers[i_fielder];
        if ((pRearMostFielder == NULL)
            || (pFielder->mUnidentified1E4.m_v3AIPosition.x < pRearMostFielder->mUnidentified1E4.m_v3AIPosition.x))
        {
            pRearMostFielder = pFielder;
        }
    }

    return pRearMostFielder;
}

/**
 * Offset/Address/Size: 0x2BCC | 0x800A8900 | size: 0x4E8
 */
void cTeam::fn_800A8900(void* context, DebugWriteCache* cache)
{
    if (lbl_806DBF06 == 0xFFFF)
    {
        lbl_806DBF06 = cache->BeginType(lbl_806DBF10);
        cache->AddField(8, gDebugFieldTypes[8].size, 0, lbl_806DBF08);
        cache->AddField(8, gDebugFieldTypes[8].size,
            (u8*)&m_nScore - (u8*)this, lbl_80500B6C);
        cache->AddField(17, gDebugFieldTypes[17].size,
            (u8*)&mfPowerupMeter - (u8*)this, lbl_80500B5C);
        cache->AddField(17, gDebugFieldTypes[17].size,
            (u8*)&mfPowerupTimer - (u8*)this, lbl_80500B4C);
        cache->AddField(14, gDebugFieldTypes[14].size,
            (u8*)&mpCurrentSituation - (u8*)this, lbl_80500B38);
        cache->AddField(14, gDebugFieldTypes[14].size,
            (u8*)&meCurrentTeamStyle - (u8*)this, lbl_80500B24);
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtTeamStyleTimer - (u8*)this, lbl_80500B10);
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtMarkTimer - (u8*)this, lbl_80500B04);
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtRoleTimer - (u8*)this, lbl_80500AF8);
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtDefensiveZoneTimer - (u8*)this, lbl_80500AE0);
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtToggleTimer - (u8*)this, lbl_80500AD0);
        cache->AddArrayField(17, gDebugFieldTypes[17].size, 4,
            (u8*)&mfBallInTimes - (u8*)this, lbl_80500AC0);
        cache->AddArrayField(22, gDebugFieldTypes[22].size, 4,
            (u8*)&mvBallInterceptPosition - (u8*)this,
            lbl_80500AA0);
        cache->AddField(15, gDebugFieldTypes[15].size,
            (u8*)&mpBestBallInterceptor - (u8*)this, lbl_80500A88);
        cache->EndType();
    }

    cTeam* copy = (cTeam*)cache->WriteData(
        lbl_806DBF06, this, offsetof(cTeam, m_ePowerupList));
    if (copy != NULL)
    {
        *(int*)&copy->mpBestBallInterceptor = mpBestBallInterceptor == NULL
            ? -1
            : mpBestBallInterceptor->mUnidentified120;
        cache->ChecksumData(lbl_806DBF06, copy, context);
    }

    GenDetTeam data;
    for (int i = 0; i < 2; i++)
    {
        data.m_ePowupType[i] = m_ePowerupList[i].eType;
        data.m_nPowerups[i] = m_ePowerupList[i].nnumOfPowerups;
    }
    for (int i = 0; i < 4; i++)
    {
        data.m_nAIOrdFs[i] = m_pAIOrderedFielders[i] == NULL
            ? -1
            : m_pAIOrderedFielders[i]->mUnidentified120;
        data.BallIntOrdFs[i]
            = m_pBallInterceptOrderedFielders[i] == NULL
            ? -1
            : m_pBallInterceptOrderedFielders[i]->mUnidentified120;
    }
    data.m_nTeamPlayTransFunc
        = mUnidentified0F0->mUnidentified18->mUnidentified00C.mUnidentifiedHash;

    if (lbl_806DBF04 == 0xFFFF)
    {
        lbl_806DBF04 = cache->BeginType(lbl_80500A7C);
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 2, 0,
            lbl_80500A6C);
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 2,
            (u8*)&data.m_nPowerups - (u8*)&data,
            lbl_80500A5C);
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 4,
            (u8*)&data.m_nAIOrdFs - (u8*)&data,
            lbl_80500A4C);
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 4,
            (u8*)&data.BallIntOrdFs - (u8*)&data,
            lbl_80500A3C);
        cache->AddField(2, gDebugFieldTypes[2].size,
            (u8*)&data.m_nTeamPlayTransFunc - (u8*)&data,
            lbl_80500A24);
        cache->EndType();
    }

    cache->ChecksumData(lbl_806DBF04, &data, context);
    cache->WriteData(lbl_806DBF04, &data, sizeof(data));

    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->Unknown11(context, cache);
        char buffer[32];
        nlSNPrintf(buffer, sizeof(buffer), lbl_80500A10,
            i + m_nSide * 5);
        cache->WriteText(buffer);
    }
}

/**
 * Offset/Address/Size: 0x30B4 | 0x800A8DE8 | size: 0xD8
 */
void cTeam::fn_800A8DE8(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&mfPowerupMeter, sizeof(mfPowerupMeter));
    runningChecksum->ChecksumData(&mUnidentified00C, sizeof(mUnidentified00C));
    runningChecksum->ChecksumData(&mUnidentified010, sizeof(mUnidentified010));
    runningChecksum->ChecksumData(&mfPowerupTimer, sizeof(mfPowerupTimer));
    runningChecksum->ChecksumData(&mpCurrentSituation, sizeof(mpCurrentSituation));
    runningChecksum->ChecksumData(&meCurrentTeamStyle, sizeof(meCurrentTeamStyle));
    runningChecksum->ChecksumData(&mfBallInTimes, sizeof(mfBallInTimes));

    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->Unknown12(runningChecksum);
    }
}

void nlTask::StateTransition(unsigned int, unsigned int)
{
}
