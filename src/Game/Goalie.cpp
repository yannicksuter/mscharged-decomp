#include "Game/Goalie.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/GoalieLooseBall.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/Team.h"

#include <math.h>

extern void KillDaze(cPlayer* player);
extern "C" bool fn_800EBBFC(
    int nParam0, unsigned int nParam1, void* pParam2, void* pParam3);
extern "C" void fn_8005D948(
    void* pGame, const GoalieSaveData* pData);
extern "C" void fn_8005E9FC(
    void* pManager, const PlayerAttackData* pData);
extern void* lbl_806E0C94;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

bool Goalie::mbPosGoalieNetCheck;
bool Goalie::mbNegGoalieNetCheck;

bool Goalie::IsTargetViable(cPlayer* pTarget)
{
    if ((float)fabs(pTarget->m_v3Position.x)
            > (float)fabs(static_cast<cPlayer*>(this)->m_v3Position.x)
        && fabsf(pTarget->m_v3Position.y) < cField::GetPenaltyBoxY())
    {
        return false;
    }

    return true;
}

void Goalie::MakeExertEvent()
{
    GoalieSaveData pSaveData;

    pSaveData.pGoalie = this;
    pSaveData.v3BallVelocity = v3Zero;
    pSaveData.fWowFactor = 0.0f;
    pSaveData.isSTS = 0;

    pSaveData.saveType = g_pBall->m_uGoalType;
    pSaveData.pShooter = g_pBall->m_pShooter;

    if (mpSaveData != 0)
    {
        pSaveData.padding = mpSaveData->muSaveType;
    }
    else
    {
        pSaveData.padding = 3;
    }

    fn_8005D948(lbl_806E0C94, &pSaveData);
}

void Goalie::SetGoalieAction(
    eGoalieActionState newGoalieState, int newSubstate)
{
    CleanGoalieAction();
    mPrevGoalieActionState = mGoalieActionState;
    mGoalieActionState = newGoalieState;
    mnSubstate = newSubstate;
}

void Goalie::SaveBlendCallback(
    unsigned int nParam, cPN_SAnimController* pAnimCtrl)
{
    Goalie* pThis = reinterpret_cast<Goalie*>(nParam & ~3U);
    unsigned int saveDataIndex = nParam & 3U;

    SaveData* pSaveData = pThis->mBlendInfo.mpSaveData[saveDataIndex];
    if (pSaveData == 0)
    {
        return;
    }

    float fTime = pAnimCtrl->m_fTime;
    int milestoneIndex = 0;

    while (milestoneIndex < 4
           && fTime >= pSaveData->mfMilestonePercent[milestoneIndex])
    {
        milestoneIndex++;
    }

    pAnimCtrl->m_fPlaybackSpeedScale
        = pThis->mBlendInfo.mfMilestoneScale[saveDataIndex][milestoneIndex];
}

static inline int GetAnimID(SaveBlendInfo& blend, int index)
{
    return blend.mpSaveData[index]->mnAnimID;
}

cPoseNode* Goalie::SetupBlender(bool bPrimary, const float* fStartPercent,
    int nMainAnimID, int nMilestone)
{
    float fBlend;
    int index1;
    cPN_SAnimController* pSaveController1;
    int index2;

    if (bPrimary)
    {
        fBlend = mBlendInfo.mfSaveBlendPrimary;
        index1 = 0;
        index2 = 1;
    }
    else
    {
        fBlend = mBlendInfo.mfSaveBlendSecondary;
        index1 = 2;
        index2 = 3;
    }
    int animID = GetAnimID(mBlendInfo, index1);
    pSaveController1 = NewAnimController(animID, false, false, SaveBlendCallback, index1 + (unsigned int)this);
    pSaveController1->m_fPlaybackSpeedScale
        = mBlendInfo.mfMilestoneScale[index1][nMilestone];
    if (fStartPercent[index1] > 0.0f)
    {
        pSaveController1->SetTime(fStartPercent[index1]);
    }
    cPoseNode* result = pSaveController1;
    if (nMainAnimID == animID)
    {
        m_pCurrentAnimController = pSaveController1;
    }
    else
    {
        pSaveController1->m_bIgnoreTriggers = true;
    }
    if (fBlend >= 0.001f)
    {
        animID = GetAnimID(mBlendInfo, index2);
        cPN_SAnimController* pSaveController2
            = NewAnimController(animID, false, false, SaveBlendCallback, index2 + (unsigned int)this);
        pSaveController2->m_fPlaybackSpeedScale
            = mBlendInfo.mfMilestoneScale[index2][nMilestone];
        if (fStartPercent[index2] > 0.0f)
        {
            pSaveController2->SetTime(fStartPercent[index2]);
        }
        if (nMainAnimID == animID)
        {
            m_pCurrentAnimController = pSaveController2;
        }
        else
        {
            pSaveController2->m_bIgnoreTriggers = true;
        }

        cPN_SingleAxisBlender* pPoseNode
            = new cPN_SingleAxisBlender(2, 0, 0, 0.1f);
        pPoseNode->m_fDesiredWeight = fBlend;
        pPoseNode->m_fSmoothedWeight = fBlend;
        pPoseNode->SetChild(0, pSaveController1);
        pPoseNode->SetChild(1, pSaveController2);
        result = pPoseNode;
    }
    return result;
}

void Goalie::PlayBlendedAnims(
    float fStartTime, float fParam2, int nMilestone)
{
    static float fDefaultStartPercent[] = { 0.4f, 0.7f };

    cPoseNode* pMainNode;
    cPoseNode* pNode1;
    cPoseNode* pNode2;
    float fStartPercent[4];
    int nMainAnimID;
    int milestone;

    if (mUnidentified254 != 0)
    {
        fn_80097648(0.1f);
    }

    if (mBlendInfo.mfSaveBlendComposite <= 0.001f
        && mBlendInfo.mfSaveBlendPrimary <= 0.001f)
    {
        SetAnimState(mpSaveData->mnAnimID, true, 0.2f, false, false);

        if (nMilestone >= 0 && nMilestone < 2)
        {
            fStartTime = mBlendInfo.mfMilestoneTime[nMilestone];
            if (fStartTime <= 0.0f)
            {
                float* pDefaultStartPercent = fDefaultStartPercent;
                fStartTime = pDefaultStartPercent[nMilestone]
                           * (mpSaveData->mfDuration
                               * mpSaveData->mfMilestonePercent[2]);
            }
        }

        if (fStartTime > 0.0f && fStartTime < mpSaveData->mfDuration)
        {
            cPN_SAnimController* pController = m_pCurrentAnimController;
            float fAnimTime = fStartTime / mpSaveData->mfDuration;
            pController->SetTime(fAnimTime);
        }
    }
    else
    {
        nMainAnimID = mpSaveData->mnAnimID;

        if (nMilestone >= 0)
        {
            milestone = nMilestone;

            if (mBlendInfo.mfMilestoneTime[nMilestone] > 0.0f)
            {
                for (int i = 0; i < 4; i++)
                {
                    SaveData* pData = mBlendInfo.mpSaveData[i];
                    if (pData != 0)
                    {
                        fStartPercent[i]
                            = pData->mfMilestonePercent[nMilestone];
                    }
                }
            }
            else
            {
                float* pDefaultStartPercent = fDefaultStartPercent;
                float fDefaultStart = pDefaultStartPercent[nMilestone];

                for (int i = 0; i < 4; i++)
                {
                    SaveData* pData = mBlendInfo.mpSaveData[i];
                    if (pData != 0)
                    {
                        fStartPercent[i]
                            = fDefaultStart * pData->mfMilestonePercent[2];
                    }
                }
            }
        }
        else if (fStartTime > 0.0f)
        {
            float fPrevMilestone = 0.0f;

            milestone = 0;
            while (milestone < 4
                   && fStartTime >= mBlendInfo.mfMilestoneTime[milestone])
            {
                if (mBlendInfo.mfMilestoneTime[milestone] > 0.0f)
                {
                    fPrevMilestone
                        = mBlendInfo.mfMilestoneTime[milestone];
                }
                milestone++;
            }

            fStartTime = NormalizeVal(fStartTime, fPrevMilestone, mBlendInfo.mfMilestoneTime[milestone]);

            int prevMilestone = milestone - 1;

            for (int i = 0; i < 4; i++)
            {
                SaveData* pData = mBlendInfo.mpSaveData[i];
                if (pData != 0)
                {
                    float fStart = 0.0f;
                    if (milestone > 0)
                    {
                        fStart
                            = pData->mfMilestonePercent[prevMilestone];
                    }

                    fStartPercent[i] = Interpolate(fStart,
                        pData->mfMilestonePercent[milestone],
                        fStartTime);
                }
            }
        }
        else
        {
            fStartPercent[0] = 0.0f;
            fStartPercent[1] = 0.0f;
            fStartPercent[2] = 0.0f;
            fStartPercent[3] = 0.0f;
            milestone = 0;
        }

        pNode1
            = SetupBlender(true, fStartPercent, nMainAnimID, milestone);

        if (mBlendInfo.mfSaveBlendComposite >= 0.001f)
        {
            pNode2 = SetupBlender(
                false, fStartPercent, nMainAnimID, milestone);
            cPN_SingleAxisBlender* pBlend
                = new cPN_SingleAxisBlender(2, 0, 0, 0.1f);

            pBlend->m_fDesiredWeight
                = mBlendInfo.mfSaveBlendComposite;
            pBlend->m_fSmoothedWeight
                = mBlendInfo.mfSaveBlendComposite;
            pBlend->SetChild(0, pNode1);
            pBlend->SetChild(1, pNode2);

            pMainNode = pBlend;
        }
        else
        {
            pMainNode = pNode1;
        }

        cPN_Blender* pBlender
            = new cPN_Blender(m_pAILayer[0], pMainNode, 0.1f);

        m_pAILayer[0] = pBlender;
        SetAnimID(nMainAnimID);
    }

    if (mbShouldMiss)
    {
        InitMovementFromAnim(0, v3Zero, 1.0f, true);
    }
    else
    {
        nlVector3 v3AnimMoveAdjust;
        nlVec3Sub(v3AnimMoveAdjust, mv3LocalContactPosition, mBlendInfo.mv3BlendedSavePos);
        GetWorldPoint(v3AnimMoveAdjust, v3AnimMoveAdjust, v3Zero, m_aDesiredFacingDirection);

        float fMaxLengthSq = fParam2 * fParam2;
        float fLengthSq = nlVec3LengthSquared(v3AnimMoveAdjust);
        if (fLengthSq > fMaxLengthSq)
        {
            float fScale = fParam2 * nlRecipSqrt(fLengthSq, true);
            nlVec3Scale(v3AnimMoveAdjust, v3AnimMoveAdjust, fScale);
        }

        InitMovementFromAnim(
            0, v3AnimMoveAdjust, mpSaveData->mfMilestonePercent[2], true);
    }
}

void Goalie::PlayNewAnim(int nAnimID)
{
    if (nAnimID == m_eAnimID)
    {
        cPN_SAnimController* pController = m_pCurrentAnimController;
        bool bSkipSetAnimState = false;

        if (pController->m_ePlayMode == PM_HOLD
            && pController->m_fTime == 1.0f)
        {
            bSkipSetAnimState = true;
        }

        if (!bSkipSetAnimState)
        {
            return;
        }
    }

    SetAnimState(nAnimID, true, 0.2f, false, false);
}

int Goalie::ChooseRunAnim(
    short nAngle, const nlVector3& rTargetPos, float fThreshold)
{
    int nCurrentAnimID = m_eAnimID;
    unsigned short nAbsAngle;
    nlVector2 v3Delta;
    v3Delta.x = rTargetPos.x - m_v3Position.x;
    v3Delta.y = rTargetPos.y - m_v3Position.y;

    if (nlGetLengthSquared2D(v3Delta.x, v3Delta.y)
        < nlGetLengthSquared1D(fThreshold))
    {
        mMoveDirection = GOALIEDIR_IDLE;
        return 5;
    }

    nAbsAngle = (u16)abs_s16(nAngle);

    mMoveDirection = GOALIEDIR_FORWARD;

    if (((nCurrentAnimID == 0x22) || (nCurrentAnimID == 0x23))
        && (m_pCurrentAnimController->m_fTime < 0.92f))
    {
        return nCurrentAnimID;
    }
    if (nAbsAngle <= 0x2AF8 || nCurrentAnimID == 0x24)
    {
        return 0x24;
    }
    if (nAngle > 0)
    {
        return 0x23;
    }

    return 0x22;
}

void Goalie::CleanupStun()
{
    if (mbStunEffectActive)
    {
        KillDaze(this);
        mbStunEffectActive = false;
    }
}

void Goalie::ChooseSwatAnim(int nParam)
{
    fn_80097648(0.1f);
    if (nParam != 0)
    {
        fn_8009750C();
    }
}

PhysicsGoalie* Goalie::GetPhysicsGoalie()
{
    return (PhysicsGoalie*)m_pPhysicsCharacter;
}

void Goalie::SetDesiredSaveFacing(const nlVector3& v3BallPosition)
{
    if (m_v3Position.x > (cField::GetGoalLineX(1U) - 0.1f))
    {
        m_aDesiredFacingDirection = 0x8000;
        return;
    }

    if (m_v3Position.x < (0.1f - cField::GetGoalLineX(1U)))
    {
        m_aDesiredFacingDirection = 0;
        return;
    }

    nlVector3 v3Facing;
    nlVector3 v3G2Ball;
    nlVector3 v3BallVelocity;
    nlVector3 v3G2Post1;
    nlVector3 v3G2Post2;

    nlVec3Set(v3G2Ball,
        v3BallPosition.x - m_v3Position.x,
        v3BallPosition.y - m_v3Position.y,
        v3BallPosition.z - m_v3Position.z);

    float fBallOffMagSq = nlVec3DotProduct(v3G2Ball, v3G2Ball);
    nlVec3Scale(v3G2Ball, nlRecipSqrt(fBallOffMagSq, true));

    cBall* pBall = g_pBall;
    float fBallVelocityMagSq = nlVec3DotProduct(
        pBall->m_v3Velocity, pBall->m_v3Velocity);
    if (fBallVelocityMagSq > 0.01f)
    {
        nlVec3Scale(v3BallVelocity, pBall->m_v3Velocity, -nlRecipSqrt(fBallVelocityMagSq, true));
        nlVec3WeightedSum(
            v3G2Ball, 0.5f, v3G2Ball, 0.5f, v3BallVelocity);
    }

    m_pTeam->m_pNet->GetPostLocation(v3G2Post1, 0, 0.5f);
    m_pTeam->m_pNet->GetPostLocation(v3G2Post2, 1, 0.5f);

    nlVec3Sub(v3G2Post1, v3G2Post1, m_v3Position);
    nlVec3Sub(v3G2Post2, v3G2Post2, m_v3Position);

    float fLeftDot = nlVec3DotProduct(v3G2Ball, v3G2Post1);
    float fRightDot = nlVec3DotProduct(v3G2Ball, v3G2Post2);

    if ((fLeftDot > 0.0f) || (fRightDot > 0.0f))
    {
        if (fLeftDot > fRightDot)
        {
            nlVec3Set(v3Facing,
                v3G2Post1.y,
                -v3G2Post1.x,
                0.0f);

            if (nlVec3DotProduct(v3Facing, v3G2Post2) > 0.0f)
            {
                nlVec3Scale(v3Facing, -1.0f);
            }
        }
        else
        {
            nlVec3Set(v3Facing,
                v3G2Post2.y,
                -v3G2Post2.x,
                0.0f);

            if (nlVec3DotProduct(v3Facing, v3G2Post1) > 0.0f)
            {
                nlVec3Scale(v3Facing, -1.0f);
            }
        }
    }
    else
    {
        v3Facing = v3G2Ball;
    }

    if (fBallOffMagSq < 1.44f)
    {
        nlVector3 v3BallToGoal;
        nlVec3Sub(v3BallToGoal,
            v3BallPosition,
            m_pTeam->m_pNet->m_v3NetLocation);
        float fLengthSq = nlVec3LengthSquared(v3Facing);

        float fRecip = nlRecipSqrt(fLengthSq, true);
        nlVec3Scale(v3Facing, fRecip);

        float fRecip2 = nlRecipSqrt(
            nlVec3LengthSquared(v3BallToGoal), true);
        nlVec3Scale(v3BallToGoal, fRecip2);

        nlVec3WeightedSum(
            v3Facing, 0.5f, v3Facing, 0.5f, v3BallToGoal);
    }

    m_aDesiredFacingDirection = (s16)(nlATan2f(
                                          v3Facing.y, v3Facing.x)
                                      * (32768.0f / 3.14159265f));
}

void Goalie::TrackTarget(
    const nlVector3& v3Target, float fRatio, float fParam3)
{
    nlVector3 v3FutureBallPos;
    nlVector3 v3FuturePos;
    unsigned short aRot;

    GetCurrentAnimFuture(m_nBallJointIndex, mpLooseBallInfo->mfPickupTime, v3FutureBallPos, v3FuturePos, aRot);

    float fZero = 0.0f;
    float fDeltaY = v3Target.y - v3FutureBallPos.y;
    float fAngleDeltaY = v3Target.y - m_v3Position.y;
    float fDeltaX = v3Target.x - v3FutureBallPos.x;
    float fAngleDeltaX = v3Target.x - m_v3Position.x;

    nlVector3 v3Velocity;
    v3Velocity.y = fDeltaY;
    v3Velocity.x = fDeltaX;
    v3Velocity.z = fZero;

    float fAngleToTarget = nlATan2f(fAngleDeltaY, fAngleDeltaX);

    unsigned short aAngleToTarget
        = (u16)(s32)(10430.378f * fAngleToTarget);
    float fFutureAngleDeltaX
        = v3FutureBallPos.x - m_v3Position.x;
    float fFutureAngleDeltaY
        = v3FutureBallPos.y - m_v3Position.y;
    s16 aDiff = (s16)(aAngleToTarget
                      - (u16)(s32)(10430.378f
                                   * nlATan2f(fFutureAngleDeltaY, fFutureAngleDeltaX)));
    s32 iTurn = ((s32)(1024.0f * fRatio) * aDiff) / 1024;
    SetFacingDirection((u16)(iTurn + m_aActualFacingDirection), true);

    nlVec3Scale(v3Velocity, fRatio);

    v3Velocity.x = nlMinEquals(
        nlMaxEquals(v3Velocity.x, -fParam3), fParam3);
    v3Velocity.y = nlMinEquals(
        nlMaxEquals(v3Velocity.y, -fParam3), fParam3);

    nlVec3Add(v3FuturePos, v3Velocity, m_v3Position);

    SetPosition(v3FuturePos);
}

void Goalie::TacklePlayer(cPlayer* pPlayer)
{
    cFielder* pFielder = static_cast<cFielder*>(pPlayer);
    if (pPlayer != 0 && pPlayer->m_eClassType == FIELDER
        && !pFielder->IsFallenDown() && !pFielder->fn_8003E6FC())
    {
        fn_800EBBFC(9, 0x06024E5D, 0, 0);

        bool bHadBall = false;
        if (pPlayer->m_pBall != 0)
        {
            pPlayer->ReleaseBall(false);
            bHadBall = true;
        }

        if (IsOnSameTeam(pFielder))
        {
            if (bHadBall)
            {
                pFielder->EndDesire();
                pFielder->EndAction();
            }
            return;
        }

        pFielder->InitActionSlideAttackReact(this, false);
    }
}

void Goalie::StealBall(cPlayer* pPlayer)
{
    if (pPlayer == 0)
    {
        return;
    }
    if (pPlayer->m_eClassType != FIELDER)
    {
        return;
    }
    if (pPlayer->m_pBall == 0)
    {
        return;
    }

    pPlayer->ReleaseBall(false);

    cFielder* pFielder = static_cast<cFielder*>(pPlayer);
    if (pFielder->fn_8002E060() == FIELDERDESIRE_FINISH_ACTION)
    {
        return;
    }

    pFielder->EndDesire();
    pFielder->EndAction();
}

void Goalie::WhackSTSPlayer(cFielder* pFielder)
{
    if (pFielder == 0)
    {
        return;
    }
    if (pFielder->fn_8003E6FC())
    {
        return;
    }

    pFielder->fn_80047240(
        pFielder, m_aActualFacingDirection, 2, true, false);
    fn_800EBBFC(pFielder->mUnidentified318, 0x3642C41B, 0, 0);

    mbDoHeadTrack = false;

    nlVector3 v3BallVel;
    const float fBallVelMult = -0.5f;
    nlVec3Scale(v3BallVel, m_v3Position, fBallVelMult);

    float yRand = nlRandomf(5.0f);
    if (nlRandom(100) > 50)
    {
        yRand *= -1.0f;
    }
    v3BallVel.y += yRand;

    v3BallVel.z = 4.0f + nlRandomf(2.0f);

    g_pBall->SetVelocity(v3BallVel, SPINTYPE_FORWARD, 0);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.12f);

    PlayerAttackData data;
    data.pAttacker = this;
    data.nAttackerPadID = -1;
    data.pTarget = pFielder;
    data.mUnidentified0C = 2;
    data.mUnidentified10 = false;
    fn_8005E9FC(lbl_806E0C94, &data);
}
