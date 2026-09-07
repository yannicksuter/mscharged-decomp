#include <stddef.h>

#include "Game/Team.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Formation.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/OverlayHandlerHUD.h"
#include "Game/Player.h"
#include "NL/nlMain.h"
#include "NL/nlTask.h"
#include "Game/Render/PeachPhoto.h"
#include "unclassified/tu_80332770.h"
#include "unclassified/tu_80336B2C.h"

cTeam* g_pTeams[2] = { NULL, NULL };
cTeam* g_pCurrentlyUpdatingTeam;
float lbl_806DBEF0 = 0.5f;
float lbl_806DBEF4 = 7.5f;
bool lbl_806E0E04;

extern "C" void fn_800A6C94(cTeam*, float);
extern "C" void fn_800A701C(cTeam*);
extern "C" void fn_800A7A10(cTeam*);
extern "C" void fn_800A7EF8(cTeam*, float);
extern "C" void fn_800A8098(cTeam*);
extern "C" float fn_800D7E0C(cPlayer*);
extern "C" bool fn_8003E8A0(const cFielder* pFielder);
extern "C" bool fn_8003E948(const cFielder* pFielder);
extern "C" bool fn_8003E99C(const cFielder* pFielder);
extern "C" void fn_801BB6A4(cFielder* pFielder, int nPowerupIndex);

extern "C" void fn_800A2290(
    SkillTweaks* pTweaks, int difficulty, int param2, bool param3);
extern "C" float fn_8002BE38(PlayerTweaks*);

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
    if (mUnidentified040.m_uPackedTime == 0
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
    if (mUnidentified040.m_uPackedTime != 0
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
            mUnidentified040.SetSeconds(lbl_806DBEF0);
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

    if (pCaptain->m_eCharacterClass == (eCharacterClass)5
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
    float fAdjustAmount, cFielder* pFielder, bool)
{
    mfPowerupMeter += fAdjustAmount;
    if (mfPowerupMeter >= 1.0f)
    {
        mfPowerupMeter -= 1.0f;
        int nPowerupIndex = PowerupBase::AwardPowerup(this, pFielder);
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
            pUnidentifiedController = GetLocalChannelPad(
                (UnidentifiedNetworkPeerChannel*)pUnidentifiedInput->m_pMyUser);
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
            if (!GameInfoManager::Instance()->IsRule0x0Equal10()
                && lbl_806E0E04)
            {
                PowerupBase::AwardPowerup(pTeam, NULL);
            }
        }
    }

    if (g_pGame->IsGameplayOrOvertime())
    {
        pTeam->mtTeamStyleTimer.Countdown(fDeltaT, 0.0f);
        pTeam->mtMarkTimer.Countdown(fDeltaT, 0.0f);
        pTeam->mtRoleTimer.Countdown(fDeltaT, 0.0f);
        pTeam->mUnidentified040.Countdown(fDeltaT, 0.0f);

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
    fn_800A7A10(this);

    if (mpBestBallInterceptor == NULL)
    {
        mpBestBallInterceptor = m_pBallInterceptOrderedFielders[0];
    }
    else if (mpBestBallInterceptor != m_pBallInterceptOrderedFielders[0])
    {
        float fScore1 = fn_800D7E0C(mpBestBallInterceptor);
        float fScore2 = fn_800D7E0C(m_pBallInterceptOrderedFielders[0]);
        if (fScore2 - fScore1 > 0.125f)
        {
            mpBestBallInterceptor = m_pBallInterceptOrderedFielders[0];
        }
    }

    fn_800A7EF8(this, fDeltaT);
    fn_800A8098(this);
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
 * Offset/Address/Size: 0x2174 | 0x800A7EA8 | size: 0x50
 */
extern "C" int fn_800A7EA8(const void* a, const void* b)
{
    cFielder* p1 = *(cFielder**)a;
    cFielder* p2 = *(cFielder**)b;

    float fPosition1 = p1->m_v3Position.x;
    float fPosition2 = p2->m_v3Position.x;
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
            || (pFielder->m_v3AIPosition.x > pFrontMostFielder->m_v3AIPosition.x))
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
            || (pFielder->m_v3AIPosition.x < pRearMostFielder->m_v3AIPosition.x))
        {
            pRearMostFielder = pFielder;
        }
    }

    return pRearMostFielder;
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
    runningChecksum->ChecksumData(&meCurrentSituation, sizeof(meCurrentSituation));
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
