#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "revolution/os/OSTime.h"
#include "Game/CharacterTweaks.h"

#include <stddef.h>

#include "Game/Team.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/ShotMeter.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/TeamPlayMachine.h"
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
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/TweakRegistry.h"
#include "NL/nlLocalization.h"
#include "NL/nlMain.h"
#include "NL/nlString.h"
#include "Game/Render/PeachPhoto.h"
#include "Game/InputManager.h"
#include "Game/NetworkInput.h"

#include <stdlib.h>
#include "Game/CharacterTriggers.h"

cTeam* g_pTeams[2] = { NULL, NULL };
cTeam* g_pCurrentlyUpdatingTeam;
float gPowerupToggleDelay = 0.5f;
float gTeamPowerupAwardInterval = 7.5f;
float gMeterRumbleRandomRange = 10.0f;
float gMeterRumbleChanceThreshold = 1.0f;
float gMeterRumbleDifficultyScale = 1.75f;
u16 gGenDetTeamDebugType = 0xFFFF;
u16 gDetTeamDebugType = 0xFFFF;
bool gAlwaysAwardPeriodicPowerups;
unsigned long gCaptainChantLastPlayTime[2];

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

int CompareFieldersByTeamRelativeX(const void*, const void*);
void UpdateTeamTimers(cTeam*, float);
unsigned long GetTeamCaptainChantCue(cTeam*);
void UpdateTeamCaptainChant(cTeam*);
void AssignTeamRoles(cTeam*, bool);
extern "C" void fn_80015B38(cBall*, bool);


static inline cAIPad* GetPlayerController(const cPlayer* player)
{
    return player->m_pController;
}

/**
 * Offset/Address/Size: 0x3294 | 0x800A8FE0 | size: 0x70
 */
float cTeam::GetAverageDefenseRating()
{
    float result = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        result += fn_8002BE38(GetFielder(i)->GetNormalTweaks());
    }
    return result / 4.0f;
}

/**
 * Offset/Address/Size: 0x3234 | 0x800A8F80 | size: 0x60
 */
float cTeam::GetAveragePassingRating()
{
    float fPassRating = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        fPassRating += GetFielder(i)->GetNormalTweaks()->fPassing;
    }
    return fPassRating / 4.0f;
}

/**
 * Offset/Address/Size: 0x31D4 | 0x800A8F20 | size: 0x60
 */
float cTeam::GetAverageShootingRating()
{
    float fShootRating = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        fShootRating += GetFielder(i)->GetNormalTweaks()->fShooting;
    }
    return fShootRating / 4.0f;
}

/**
 * Offset/Address/Size: 0x3174 | 0x800A8EC0 | size: 0x60
 */
float cTeam::GetAverageMovementRating()
{
    float fMovementRating = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        fMovementRating += GetFielder(i)->GetNormalTweaks()
                               ->fMovementSpeed;
    }
    return fMovementRating / 4.0f;
}

/**
 * Offset/Address/Size: 0x309C | 0x800A8DE8 | size: 0xD8
 */
void cTeam::ChecksumState(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&mfPowerupMeter, sizeof(mfPowerupMeter));
    runningChecksum->ChecksumData(&mfAttackIndicatorProgress, sizeof(mfAttackIndicatorProgress));
    runningChecksum->ChecksumData(&mfShotScore, sizeof(mfShotScore));
    runningChecksum->ChecksumData(&mfPowerupTimer, sizeof(mfPowerupTimer));
    runningChecksum->ChecksumData(&mpCurrentSituation, sizeof(mpCurrentSituation));
    runningChecksum->ChecksumData(&meCurrentTeamStyle, sizeof(meCurrentTeamStyle));
    runningChecksum->ChecksumData(&mfBallInTimes, sizeof(mfBallInTimes));

    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->ChecksumState(runningChecksum);
    }
}

/**
 * Offset/Address/Size: 0x2BB4 | 0x800A8900 | size: 0x4E8
 */
void cTeam::SyncLog(void* context, DebugWriteCache* cache)
{
    WriteTeamStateLog(context, cache);

    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->SyncLog(context, cache);
        char buffer[32];
        nlSNPrintf(buffer, sizeof(buffer), "PLAYER %d DONE\n\n",
            i + m_nSide * 5);
        cache->WriteText(buffer);
    }
}

inline void cTeam::WriteTeamStateLog(void* context, DebugWriteCache* cache)
{
    if (gDetTeamDebugType == 0xFFFF)
    {
        gDetTeamDebugType = cache->BeginType("DetTeam");
        cache->AddField(8, gDebugFieldTypes[8].size, 0, "m_nSide");
        cache->AddField(8, gDebugFieldTypes[8].size,
            (u8*)&m_nScore - (u8*)this, "m_nScore");
        cache->AddField(17, gDebugFieldTypes[17].size,
            (u8*)&mfPowerupMeter - (u8*)this, "mfPowerupMeter");
        cache->AddField(17, gDebugFieldTypes[17].size,
            (u8*)&mfPowerupTimer - (u8*)this, "mfPowerupTimer");
        cache->AddField(14, gDebugFieldTypes[14].size,
            (u8*)&mpCurrentSituation - (u8*)this, "meCurrentSituation");
        cache->AddField(14, gDebugFieldTypes[14].size,
            (u8*)&meCurrentTeamStyle - (u8*)this, "meCurrentTeamStyle");
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtTeamStyleTimer - (u8*)this, "mtTeamStyleTimer");
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtMarkTimer - (u8*)this, "mtMarkTimer");
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtRoleTimer - (u8*)this, "mtRoleTimer");
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtDefensiveZoneTimer - (u8*)this, "mtDefensiveZoneTimer");
        cache->AddField(20, gDebugFieldTypes[20].size,
            (u8*)&mtToggleTimer - (u8*)this, "mtToggleTimer");
        cache->AddArrayField(17, gDebugFieldTypes[17].size, 4,
            (u8*)&mfBallInTimes - (u8*)this, "mfBallInTimes[]");
        cache->AddArrayField(22, gDebugFieldTypes[22].size, 4,
            (u8*)&mvBallInterceptPosition - (u8*)this,
            "mvBallInterceptPosition[]");
        cache->AddField(15, gDebugFieldTypes[15].size,
            (u8*)&mpBestBallInterceptor - (u8*)this, "mpBestBallInterceptor");
        cache->EndType();
    }

    cTeam* copy = (cTeam*)cache->WriteData(
        gDetTeamDebugType, this, offsetof(cTeam, m_ePowerupList));
    if (copy != NULL)
    {
        *(int*)&copy->mpBestBallInterceptor = mpBestBallInterceptor == NULL
            ? -1
            : mpBestBallInterceptor->mUnidentified120;
        cache->ChecksumData(gDetTeamDebugType, copy, context);
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
        = m_pAIContext->mScriptMachine->mTransition.mValue.mFuncHash;

    if (gGenDetTeamDebugType == 0xFFFF)
    {
        gGenDetTeamDebugType = cache->BeginType("GenDetTeam");
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 2, 0,
            "m_ePowupType[]");
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 2,
            (u8*)&data.m_nPowerups - (u8*)&data,
            "m_nPowerups[]");
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 4,
            (u8*)&data.m_nAIOrdFs - (u8*)&data,
            "m_nAIOrdFs[]");
        cache->AddArrayField(8, gDebugFieldTypes[8].size, 4,
            (u8*)&data.BallIntOrdFs - (u8*)&data,
            "BallIntOrdFs[]");
        cache->AddField(2, gDebugFieldTypes[2].size,
            (u8*)&data.m_nTeamPlayTransFunc - (u8*)&data,
            "m_nTeamPlayTransFunc");
        cache->EndType();
    }

    cache->ChecksumData(gGenDetTeamDebugType, &data, context);
    cache->WriteData(gGenDetTeamDebugType, &data, sizeof(data));

}

/**
 * Offset/Address/Size: 0x2B38 | 0x800A8884 | size: 0x7C
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
 * Offset/Address/Size: 0x2ABC | 0x800A8808 | size: 0x7C
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
 * Offset/Address/Size: 0x2AB4 | 0x800A8800 | size: 0x8
 */
cFielder* cTeam::GetStriker() const
{
    return m_pAIOrderedFielders[0];
}

/**
 * Offset/Address/Size: 0x2AAC | 0x800A87F8 | size: 0x8
 */
cFielder* cTeam::GetCaptain()
{
    return (cFielder*)m_pPlayers[0];
}

/**
 * Offset/Address/Size: 0x2750 | 0x800A849C | size: 0x35C
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
        pMyFielder->ClearMarks();
        bool bUnidentified = pMyFielder->IsInFallAction()
                          || pMyFielder->IsShattered()
                          || Incapacitated(pMyFielder);
        if (!bUnidentified)
        {
            fDownfield = NearTo(
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
        pUnidentifiedBestFielder->AddMark(pUnidentifiedFielder);
    }

    float fFielderMarkScores[4][4];
    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pMyFielder = GetFielder(i_fielder);
        bool bUnidentifiedMyFielder = pMyFielder->IsInFallAction()
                                   || pMyFielder->IsShattered();

        nlVector3 v3UnidentifiedPosition;
        pMyFielder->CalculateFormationPosition(v3UnidentifiedPosition);

        for (int i_otherf = 0; i_otherf < 4; i_otherf++)
        {
            pOppFielder = GetOtherTeam()->GetFielder(i_otherf);
            bool bUnidentifiedOppFielder = pOppFielder->IsInFallAction()
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
        GetFielder(i_fielder)->AddMark(
            GetOtherTeam()->GetFielder(pMarkIDs[i_fielder]));
    }

    mtMarkTimer.SetSeconds(0.5f);
}

/**
 * Offset/Address/Size: 0x2680 | 0x800A83CC | size: 0xD0
 */
void AssignTeamRoles(cTeam* pTeam, bool bSituationChanged)
{
    if (pTeam->mtRoleTimer.m_uPackedTime == 0 || bSituationChanged)
    {
        unsigned int* pFielderFormationPos
            = pTeam->m_pFormationManager->GetHighestWeightFielderOrder();
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
 * Offset/Address/Size: 0x2538 | 0x800A8284 | size: 0x148
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
 * Offset/Address/Size: 0x234C | 0x800A8098 | size: 0x1EC
 */
void cTeam::UpdateShotScore()
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

            float fScoreValue = CalcShotScoreValue(
                g_pBall->GetOwnerFielder(), bIsChipShot, false);
            mfShotScore = fScoreValue;
            mfAttackIndicatorProgress = nlMinEquals(
                nlMaxEquals(
                    g_pBall->GetOwnerFielder()->GetShotProbability(
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
                float fScoreValue = CalcShotScoreValue(
                    g_pBall->GetPassTargetFielder(),
                    g_pBall->GetPassTargetFielder()->bIsModified, false);
                mfShotScore = fScoreValue;
                mfAttackIndicatorProgress = nlMinEquals(
                    nlMaxEquals(
                        g_pBall->GetPassTargetFielder()->GetShotProbability(
                            fScoreValue)
                            / 100.0f,
                        0.0f),
                    1.0f);
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x21AC | 0x800A7EF8 | size: 0x1A0
 */
void cTeam::UpdateTeamAI(float fDeltaT)
{
    if (mtTeamStyleTimer.m_uPackedTime == 0)
    {
        meCurrentTeamStyle = TEAM_STYLE_AGGRESSIVE;
        mtTeamStyleTimer.SetSeconds(1.0f);
    }

    qsort(m_pFieldersByTeamRelativeX, 4, 4, CompareFieldersByTeamRelativeX);

    bool bSituationChanged = AssignSituation();
    if (bSituationChanged)
    {
        m_pFormationManager->ChooseNewFormations();
    }

    m_pFormationManager->Update(fDeltaT);
    AssignTeamRoles(this, bSituationChanged);
    AssignMarks(bSituationChanged);

    if (!UserControlledT(this)
        && ShootToScoreMeter::instance.m_bMeterVisible)
    {
        float fRumbleChance = gMeterRumbleDifficultyScale * Difficult(this);
        if (nlRandomf(gMeterRumbleRandomRange, &nlDefaultSeed) < fRumbleChance
            && fRumbleChance > gMeterRumbleChanceThreshold)
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

    m_pAIContext->Update(true, fDeltaT);
}

/**
 * Offset/Address/Size: 0x215C | 0x800A7EA8 | size: 0x50
 */
int CompareFieldersByTeamRelativeX(const void* a, const void* b)
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
 * Offset/Address/Size: 0x1CC4 | 0x800A7A10 | size: 0x498
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
        float speed = ((cFielder*)pPlayer)->GetRunningSpeed();
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
            float fContactHeight = ((cFielder*)pPlayer)->GetAirInterceptHeight(1);
            float fBallHeight = g_pBall->m_v3Position.z;
            if (fBallHeight > fContactHeight)
            {
                float fOtherContactHeight
                    = ((cFielder*)pPlayer)->GetAirInterceptHeight(0);
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
                    if (g_pBall->UnidentifiedHasPassTarget())
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
 * Offset/Address/Size: 0x1CBC | 0x800A7A08 | size: 0x8
 */
bool cTeam::CalculateFormationPosition(nlVector3& v3DestPosition,
    cFielder* pFielder, bool bInPosition)
{
    return m_pFormationManager->CalculateFielderPosition(
        v3DestPosition, pFielder, bInPosition);
}

/**
 * Offset/Address/Size: 0x1C4C | 0x800A7998 | size: 0x70
 */
void cTeam::StopPlayingAllTrackedSFX()
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
 * Offset/Address/Size: 0x1838 | 0x800A7584 | size: 0x414
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

        pFielder->Reset(v3NewPosition, aNewFacingDirection);
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
                    SetFielderFrozenState(pFielder, 4, 99999.0f);
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
                        SetFielderFrozenState(pFielder, 4, 99999.0f);
                    }
                    nMissingSidekicks--;
                }
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x145C | 0x800A71A8 | size: 0x3DC
 */
void cTeam::UpdateControllers()
{
    for (int i = 0; i < 16; i++)
    {
        cAIPad* pAIPad = GetAIPad(i);
        if (pAIPad == NULL)
        {
            continue;
        }

        if (m_nSide != (s16)GameInfoManager::Instance()->GetPlayingSide((u16)i))
        {
            int nAvailableFielders = 0;
            bool bRemovedGoalie = false;
            for (int j = 0; j < 5; j++)
            {
                if (m_pPlayers[j]->m_pController == pAIPad)
                {
                    m_pPlayers[j]->SetAIPad(NULL);
                    if (m_pPlayers[j]->m_eClassType == GOALIE)
                    {
                        bRemovedGoalie = true;
                    }
                    else if (!((cFielder*)m_pPlayers[j])->IsFallenDown()
                        && !((cFielder*)m_pPlayers[j])->IsStuck()
                        && !((cFielder*)m_pPlayers[j])->IsShattered()
                        && !((cFielder*)m_pPlayers[j])->IsMegaStrikeFrozen())
                    {
                        nAvailableFielders++;
                    }
                    break;
                }
            }

            if (bRemovedGoalie == true)
            {
                for (int j = 0; j < 4 && nAvailableFielders > 0; j++)
                {
                    if (m_pPlayers[j]->m_pController != NULL)
                    {
                        m_pPlayers[j]->SwapController(false);
                    }
                }
            }

            if (nAvailableFielders > 0)
            {
                for (int j = 0; j < 4 && nAvailableFielders > 0; j++)
                {
                    if (m_pPlayers[j]->m_pController != NULL
                        && (((cFielder*)m_pPlayers[j])->IsFallenDown()
                            || ((cFielder*)m_pPlayers[j])->IsStuck()
                            || ((cFielder*)m_pPlayers[j])->IsShattered()
                            || ((cFielder*)m_pPlayers[j])->IsMegaStrikeFrozen()))
                    {
                        m_pPlayers[j]->SwapController(false);
                        nAvailableFielders--;
                    }
                }
            }
        }
        else
        {
            int playerIdx;
            for (playerIdx = 0; playerIdx < 5; playerIdx++)
            {
                if (m_pPlayers[playerIdx]->m_pController == pAIPad)
                {
                    break;
                }
            }

            if (playerIdx == 5)
            {
                bool bAssigned = false;
                for (int j = 0; j < 4; j++)
                {
                    if (m_pPlayers[j]->m_pController == NULL
                        && !((cFielder*)m_pPlayers[j])->IsFallenDown()
                        && !((cFielder*)m_pPlayers[j])->IsStuck()
                        && !((cFielder*)m_pPlayers[j])->IsShattered()
                        && !((cFielder*)m_pPlayers[j])->IsMegaStrikeFrozen())
                    {
                        bAssigned = true;
                        m_pPlayers[j]->SetAIPad(pAIPad);
                        break;
                    }
                }
                if (!bAssigned)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        if (m_pPlayers[j]->m_pController == NULL)
                        {
                            m_pPlayers[j]->SetAIPad(pAIPad);
                            break;
                        }
                    }
                }
            }
        }
    }

    cPlayer* pOwner = g_pBall->GetOwner();
    if (pOwner != NULL)
    {
        if (pOwner->m_eClassType == GOALIE)
        {
            if (!(g_pGame->IsGameplayOrOvertime() && !((Goalie*)pOwner)->mbNoUserControl))
            {
                return;
            }
        }
        if (pOwner->m_pTeam != this)
        {
            return;
        }
        if (pOwner->m_pController != NULL)
        {
            return;
        }

        for (int j = 0; j < 5; j++)
        {
            if (m_pPlayers[j]->m_pController != NULL)
            {
                cAIPad* pPad = GetPlayerController(m_pPlayers[j]);
                m_pPlayers[j]->SetAIPad(NULL);
                pOwner->SetAIPad(pPad);
                break;
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x12D0 | 0x800A701C | size: 0x18C
 */
void UpdateTeamCaptainChant(cTeam* pTeam)
{
    if (!GetStadiumUnknown0x10(
            GameInfoManager::Instance()->GetStadium()))
    {
        return;
    }

    unsigned long nCueId = GetTeamCaptainChantCue(pTeam);
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
        && !pCaptain->IsSuperPowerActive())
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
        if (nCurrentTime - gCaptainChantLastPlayTime[0] <= 1200)
        {
            return;
        }

        if (!GetTweakBool("user/RestrictStreams", false))
        {
            PlayCaptainChant(14, nCueId, pTeam);
        }
        gCaptainChantLastPlayTime[0] = nCurrentTime;
    }
    else
    {
        StopCaptainChant(nCueId, pTeam);
    }
}

/**
 * Offset/Address/Size: 0x1194 | 0x800A6EE0 | size: 0x13C
 */
unsigned long GetTeamCaptainChantCue(cTeam* pTeam)
{
    unsigned long result = 0;
    switch (pTeam->m_pPlayers[0]->mUnidentified11C->unknown_0x14)
    {
    case 9:
        result = 0xF1B432C3;
        break;
    case 11:
        result = 0xE8DC557A;
        break;
    case 10:
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
    case 15:
        result = 0xAE3706CD;
        break;
    case 16:
        result = 0xF2D97BFD;
        break;
    case 13:
        result = 0x42C3BE45;
        break;
    case 14:
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
 * Offset/Address/Size: 0x1190 | 0x800A6EDC | size: 0x4
 */
void cTeam::StopGameplayEffectsAndSounds()
{
    UpdateTeamCaptainChant(this);
}

/**
 * Offset/Address/Size: 0x10CC | 0x800A6E18 | size: 0xC4
 */
void cTeam::Update(float fDeltaT)
{
    g_pCurrentlyUpdatingTeam = this;
    UpdateTeamTimers(this, fDeltaT);
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
    UpdateShotScore();
    UpdateTeamCaptainChant(this);
}

/**
 * Offset/Address/Size: 0xF48 | 0x800A6C94 | size: 0x184
 */
void UpdateTeamTimers(cTeam* pTeam, float fDeltaT)
{
    if ((g_pGame->IsGameplayOrOvertime()
            || g_pGame->GetGameState() == 1)
        && !g_pGame->IsCaptainShotToScoreOn())
    {
        pTeam->mfPowerupTimer -= fDeltaT;
        if (pTeam->mfPowerupTimer < 0.0f)
        {
            pTeam->mfPowerupTimer = gTeamPowerupAwardInterval;
            if (GameInfoManager::Instance()->IsRule0x0Equal10()
                || gAlwaysAwardPeriodicPowerups)
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
 * Offset/Address/Size: 0xEE0 | 0x800A6C2C | size: 0x68
 */
void cTeam::PreUpdate(float fDeltaT)
{
    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->PreUpdate(fDeltaT);
    }
}

/**
 * Offset/Address/Size: 0xE38 | 0x800A6B84 | size: 0xA8
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
 * Offset/Address/Size: 0xD7C | 0x800A6AC8 | size: 0xBC
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
 * Offset/Address/Size: 0xD60 | 0x800A6AAC | size: 0x1C
 */
cNet* cTeam::GetOtherNet()
{
    return g_pTeams[m_nSide == HOME ? AWAY : HOME]->m_pNet;
}

/**
 * Offset/Address/Size: 0xD48 | 0x800A6A94 | size: 0x18
 */
cTeam* cTeam::GetOtherTeam()
{
    return g_pTeams[m_nSide == HOME ? AWAY : HOME];
}

/**
 * Offset/Address/Size: 0xD38 | 0x800A6A84 | size: 0x10
 */
cPlayer* cTeam::GetPlayer(int nIndex)
{
    return m_pPlayers[nIndex];
}

/**
 * Offset/Address/Size: 0xD28 | 0x800A6A74 | size: 0x10
 */
cFielder* cTeam::GetFielder(int nIndex)
{
    return (cFielder*)m_pPlayers[nIndex];
}

/**
 * Offset/Address/Size: 0xCB0 | 0x800A69FC | size: 0x78
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
 * Offset/Address/Size: 0xC28 | 0x800A6974 | size: 0x88
 */
cPlayer* cTeam::GetControlledPlayer(cGlobalPad* pController)
{
    IsNetworkOrRecordedGame();
    cPlayer* pRetval = NULL;
    for (int i = 0; i < 5; i++)
    {
        DetInput* pInput = m_pPlayers[i]->GetGlobalPad();
        cGlobalPad* pLocalPad = NULL;
        if (pInput != NULL)
        {
            pLocalPad = ((NetworkPeerChannel*)pInput->m_pMyUser)->GetLocalChannelPad();
        }
        if (pLocalPad == pController)
        {
            pRetval = m_pPlayers[i];
            break;
        }
    }
    return pRetval;
}

extern "C" ScriptMachine* fn_800A6968(cTeam* pTeam)
{
    return pTeam->m_pAIContext->mScriptMachine;
}

FuzzyRuntimeBase* GetTeamFuzzyRuntime(cTeam* pTeam)
{
    return pTeam->m_pAIContext->mRuntime;
}

/**
 * Offset/Address/Size: 0xC08 | 0x800A6954 | size: 0x8
 */
Goalie* cTeam::GetGoalie()
{
    return (Goalie*)m_pPlayers[4];
}

/**
 * Offset/Address/Size: 0xC00 | 0x800A694C | size: 0x8
 */
void cTeam::SetGoalie(Goalie* pGoalie)
{
    m_pPlayers[4] = pGoalie;
}

/**
 * Offset/Address/Size: 0xBDC | 0x800A6928 | size: 0x24
 */
void cTeam::SetPlayer(cPlayer* pPlayer, int nIndex)
{
    m_pPlayers[nIndex] = pPlayer;
    if (nIndex < 4)
    {
        m_pAIOrderedFielders[nIndex] = (cFielder*)pPlayer;
        m_pBallInterceptOrderedFielders[nIndex] = (cFielder*)pPlayer;
        m_pFieldersByTeamRelativeX[nIndex] = (cFielder*)pPlayer;
    }
}

/**
 * Offset/Address/Size: 0xB84 | 0x800A68D0 | size: 0x58
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
 * Offset/Address/Size: 0xB6C | 0x800A68B8 | size: 0x18
 */
void cTeam::SetIsPowerUpNew(int index, bool isNew)
{
    if (index >= 0)
    {
        m_ePowerupList[index].bIsNew = isNew;
    }
}

/**
 * Offset/Address/Size: 0xB14 | 0x800A6860 | size: 0x58
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
 * Offset/Address/Size: 0xAF8 | 0x800A6844 | size: 0x1C
 */
PowerUpTeamType cTeam::GetCurrentPowerUp() const
{
    return m_ePowerupList[0];
}

/**
 * Offset/Address/Size: 0xA18 | 0x800A6764 | size: 0xE0
 */
bool cTeam::fn_800A6764() const
{
    cFielder* pCaptain = (cFielder*)m_pPlayers[0];
    int nCaptainPowerup = pCaptain->mUnidentified11C->unknown_0x14;
    bool bCaptainPowerupActive
        = m_ePowerupList[0].eType == nCaptainPowerup;
    bCaptainPowerupActive
        |= m_ePowerupList[1].eType == nCaptainPowerup;

    if (pCaptain->IsSuperGrowActive()
        || IsBowserSuperPowerActive(pCaptain)
        || IsWaluigiSuperPowerActive(pCaptain)
        || pCaptain->IsWarioSuperPowerActive()
        || pCaptain->IsPeteySuperPowerActive())
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
 * Offset/Address/Size: 0x9A8 | 0x800A66F4 | size: 0x70
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
            EmitPowerupIcon(pFielder, nPowerupIndex);
            return true;
        }
    }
    return false;
}

/**
 * Offset/Address/Size: 0x89C | 0x800A65E8 | size: 0x10C
 */
bool cTeam::TogglePowerup(bool bIsSilent)
{
    bool result = false;
    if (mtToggleTimer.m_uPackedTime != 0
        || IsBowserSuperPowerActive((cFielder*)m_pPlayers[0])
        || IsWaluigiSuperPowerActive((cFielder*)m_pPlayers[0])
        || ((cFielder*)m_pPlayers[0])->IsWarioSuperPowerActive()
        || ((cFielder*)m_pPlayers[0])->IsPeteySuperPowerActive())
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
            mtToggleTimer.SetSeconds(gPowerupToggleDelay);
        }

        HUDOverlay* HUD
            = (HUDOverlay*)g_pOverlayManager->GetScene(OVERLAY_HUD);
        HUD->SwapPowerUps(m_nSide);
        return true;
    }
    return false;
}

/**
 * Offset/Address/Size: 0x814 | 0x800A6560 | size: 0x88
 */
bool cTeam::fn_800A6560()
{
    bool result = false;
    if (mtToggleTimer.m_uPackedTime == 0
        && !IsBowserSuperPowerActive((cFielder*)m_pPlayers[0])
        && !IsWaluigiSuperPowerActive((cFielder*)m_pPlayers[0])
        && !((cFielder*)m_pPlayers[0])->IsWarioSuperPowerActive()
        && !((cFielder*)m_pPlayers[0])->IsPeteySuperPowerActive())
    {
        result = true;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x704 | 0x800A6450 | size: 0x110
 */
void cTeam::ClearCurrentPowerUp()
{
    m_ePowerupList[0].eType = POWER_UP_NONE;
    m_ePowerupList[0].nnumOfPowerups = 0;
    mtToggleTimer.UnidentifiedClear();

    for (int i = 0; i < 2; i++)
    {
        if (m_ePowerupList[i].eType == POWER_UP_NONE)
        {
            int j = i;
            do
            {
                j++;
            } while (j < 2 && m_ePowerupList[j].eType == POWER_UP_NONE);

            if (j < 2)
            {
                m_ePowerupList[i].eType = m_ePowerupList[j].eType;
                m_ePowerupList[i].nnumOfPowerups = m_ePowerupList[j].nnumOfPowerups;
                m_ePowerupList[j].eType = POWER_UP_NONE;
                m_ePowerupList[j].nnumOfPowerups = 0;
            }
        }
    }

    // The release build retains these state checks with no further action.
    if (g_pGame == NULL)
    {
        return;
    }
    if (g_pGame->IsGameplayOrOvertime() || g_pGame->GetGameState() != 2)
    {
        return;
    }
}

/**
 * Offset/Address/Size: 0x6E8 | 0x800A6434 | size: 0x1C
 */
void cTeam::ClearAllPowerUps()
{
    m_ePowerupList[0].eType = POWER_UP_NONE;
    m_ePowerupList[0].nnumOfPowerups = 0;
    m_ePowerupList[1].eType = POWER_UP_NONE;
    m_ePowerupList[1].nnumOfPowerups = 0;
}

void cTeam::SetDifficulty(int difficulty, bool blend, bool reload)
{
    if (difficulty < 0)
    {
        difficulty = 3;
    }

    fn_800A636C(this)->Init(difficulty, blend, reload);
}

/**
 * Offset/Address/Size: 0x63C | 0x800A6388 | size: 0x84
 */
float fn_800A6388(cTeam* team)
{
    float result = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        if (team->GetFielder(i)->IsInFallAction())
        {
            result += 1.0f;
        }
    }
    return result;
}

SkillTweaks* fn_800A636C(cTeam* pTeam)
{
    return gGameTweaks.mSkillTweaks[pTeam->m_nSide];
}

/**
 * Offset/Address/Size: 0x59C | 0x800A62E8 | size: 0x84
 */
cTeam::~cTeam()
{
    delete m_pNet;
    delete m_pFormationManager;
    m_pAIContext->Cleanup(true, true);
    delete m_pAIContext;
}

/**
 * Offset/Address/Size: 0x4FC | 0x800A6248 | size: 0xA0
 */
void cTeam::ResetAI()
{
    for (int i = 0; i < 4; i++)
    {
        mfBallInTimes[i] = 0.0f;
    }

    m_pFormationManager->ResetToDefaults();
    m_pAIContext->mScriptMachine->Reset(false);

    for (int i = 0; i < 4; i++)
    {
        m_pAIOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        m_pBallInterceptOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        m_pFieldersByTeamRelativeX[i] = (cFielder*)m_pPlayers[i];
    }
}

/**
 * Offset/Address/Size: 0x330 | 0x800A607C | size: 0x1CC
 */
void cTeam::Reset()
{
    for (int i = 0; i < 4; i++)
    {
        mfBallInTimes[i] = 0.0f;
    }

    m_pFormationManager->ResetToDefaults();
    m_pAIContext->mScriptMachine->Reset(false);

    for (int i = 0; i < 4; i++)
    {
        m_pAIOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        m_pBallInterceptOrderedFielders[i] = (cFielder*)m_pPlayers[i];
        m_pFieldersByTeamRelativeX[i] = (cFielder*)m_pPlayers[i];
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
    mfAttackIndicatorProgress = 0.0f;
    mfShotScore = 0.0f;

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
 * Offset/Address/Size: 0x0 | 0x800A5D4C | size: 0x330
 */
cTeam::cTeam(int nSide)
{
    m_nScore = 0;
    mfPowerupMeter = 0.0f;
    mfPowerupTimer = 0.0f;
    mpCurrentSituation = SITUATION_OFFENSE;
    meCurrentTeamStyle = TEAM_STYLE_MODERATE;
    mfAttackIndicatorProgress = 0.0f;
    mfShotScore = 0.0f;

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
        m_pFieldersByTeamRelativeX[i] = NULL;
    }
    mUnidentified0F4 = 0;

    m_pNet = new (8, false) cNet(nSide);
    m_pFormationManager = new (8, false) FormationManager(this);
    m_pAIContext = new (8, false) AIContext(this,
        new (8, false) TeamPlayMachine(),
        new (8, false) FuzzyAIRuntime());
    m_pAIContext->mScriptMachine->Initialize();
}
