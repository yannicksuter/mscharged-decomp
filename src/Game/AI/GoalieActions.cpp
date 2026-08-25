#include "Game/Goalie.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/FilteredRandom.h"
#include "Game/AI/GoalieLooseBall.h"
#include "Game/Ball.h"
#include "Game/CharacterTriggers.h"
#include "Game/CharacterTweaks.h"
#include "Game/Field.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/Team.h"
#include "math.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static int gOffplayDejected[5] = { 0x99, 0x9A, 0x9B, 0x9C, 0x9D };

struct UnidentifiedGoalieActionState
{
    u8 mUnidentified00[0x18];
    int mUnidentified18;
    u8 mUnidentified1C[0x04];
    bool mUnidentified20;
};

struct UnidentifiedGoalieSkillTweaks
{
    u8 mUnidentified000[0x194];
    float* mpLooseBallChaseDistance;
};

extern UnidentifiedGoalieActionState* lbl_806E0C94;
extern float lbl_806DBB08;
extern float lbl_806DBBBC;
extern float lbl_806DBC80;
extern float lbl_806DBC8C;
extern unsigned char lbl_806DBC90;
extern float lbl_806DBC94;
extern float lbl_806DBC98;
extern float lbl_806DBD04;
extern float lbl_806DBD0C;
extern float lbl_806DBD10;
extern float lbl_806DBD14;
extern float lbl_806DBD24;
extern float lbl_806DBD28;
extern void* lbl_806E0E00;

extern "C" void fn_800797DC(
    Goalie* pGoalie, int nParam, float fDeltaT, float fParam);
extern "C" float fn_800776B4();
extern "C" bool fn_8007AE38(Goalie* pGoalie);
extern "C" bool fn_8007BF68(Goalie* pGoalie, bool bParam);
extern "C" bool fn_8007C590(Goalie* pGoalie);
extern "C" bool fn_8007D644(Goalie* pGoalie);
extern "C" void fn_8007E940(Goalie* pGoalie, float fFudgeDist);
extern "C" float fn_8007ECB4(Goalie* pGoalie, float fTimeToContact,
    unsigned int uSaveType, bool bFromTakeoff, bool bFindFailSave);
extern "C" float fn_8007EDF4(Goalie* pGoalie, float fParam);
extern "C" bool fn_8007B9A0(
    Goalie* pGoalie, const nlVector3& v3Position, float fParam);
extern "C" bool fn_8007B214(Goalie* pGoalie);
extern "C" float fn_8007BEEC(Goalie* pGoalie, cFielder* pTarget);
extern "C" void fn_800809D0(
    Goalie* pGoalie, cFielder* pTarget, bool bParam);
extern "C" bool fn_8007C904(
    Goalie* pGoalie, const nlVector3& v3Position);
extern "C" void fn_8004F204(cFielder* pFielder);
extern "C" void fn_8005E408(
    void* pManager, const PlayerAttackData* pData);
extern "C" void fn_800EBBFC(
    unsigned int nParam0, unsigned int nParam1, int nParam2, int nParam3);
extern "C" bool fn_80331C04(
    cGlobalPad* pGlobalPad, int nButton, bool bRemap);
extern "C" void fn_80098468(cPlayer* pPlayer, float fParam);
extern "C" void fn_8009591C(cPlayer* pPlayer, bool bParam);
extern "C" cFielder* fn_8009664C(
    Goalie* pGoalie, const nlVector3& v3Position, bool bParam);
extern "C" UnidentifiedGoalieSkillTweaks* fn_800A636C(void* pTeam);
extern "C" cPlayer* fn_800A6A84(cTeam* pTeam, int nIndex);
extern "C" void fn_80139D1C(int nParam, void* pParam);
extern "C" float fn_8016D52C(float fHeight, float fEndTime,
    nlVector3& v3Position, nlVector3& v3Velocity, float& fTargetHeight,
    bool bParam);
extern "C" bool fn_8016EA10(const nlVector3& v3Position,
    float fSpeed1, float fSpeed2, nlVector3& v3TargetPosition,
    nlVector3& v3TargetVelocity, float& fInterceptTime,
    float& fClosestDistance, float fMaxTime);
extern "C" void fn_8008CED8(Goalie* pGoalie, float fTargetTime,
    const nlVector3& v3TargetPosition,
    const nlVector3& v3TargetVelocity);

float OpenTo(cPlayer* pFromFielder, cPlayer* pToFielder);

inline void Goalie::InitActionPassInterceptSave()
{
    SetGoalieAction(GOALIEACTION_SAVE, 0);
    PlayBlendedAnims(mBlendInfo.mfStartTime, 1.5f, -1);
    m_pPhysicsCharacter->m_CanCollideWithBall = true;
    mnOffplayPending = GOALIE_OFFPLAY_NONE;
    mbBallImpacted = false;
    mbIsDown = true;
    MakeExertEvent();
}

inline void Goalie::InitActionPursueBallCarrier()
{
    SetGoalieAction(GOALIEACTION_PURSUE_BALL_CARRIER, 0);
    mpLooseBallInfo = &LooseBallAnims::mTrapBallInfo;
    mbPlayMiss = false;
}

inline void Goalie::InitActionPursueBallPounce()
{
    SetGoalieAction(GOALIEACTION_PURSUE_BALL_POUNCE, 0);
    SetAnimState(mpLooseBallInfo->mnAnimID, true, 0.2f, false, false);

    GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
    InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
        pTweaks->fRunningDirectionSeekFalloff);

    mbPickedUp = false;
    mbIsDown = true;
}

inline void Goalie::InitActionPursueRecover()
{
    SetGoalieAction(GOALIEACTION_DIVE_RECOVER, 0);

    int animID = 0x98;
    if (m_pBall == 0)
    {
        animID = 0x97;
    }

    SetAnimState(animID, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    mbPickedUp = false;
    mbIsDown = true;
}

inline void Goalie::InitActionLooseBallCatch()
{
    SetGoalieAction(GOALIEACTION_LOOSEBALL_CATCH, 0);
    mv3LocalContactPosition.x = 0.2f;
    mbIsDown = true;
    mpSaveData = GoalieSave::FindBestSave(mBlendInfo,
        mv3LocalContactPosition,
        mv3LocalContactVelocity,
        mfTargetTime,
        false,
        0x80001,
        true);
    mpLooseBallInfo = 0;
    mMoveDirection = GOALIEDIR_IDLE;

    if (mpSaveData == 0)
    {
        InitActionLooseBallSetup();
    }
}

void Goalie::MoveDirectionCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    Goalie* pGoalie = (Goalie*)nParam;
    float result = 0.0f;
    if (pGoalie->mv3LocalNavTarget.y < 0.0f)
    {
        result = 1.0f;
    }
    blender->m_fDesiredWeight = result;
}

void Goalie::MoveWeightCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    Goalie* pGoalie = (Goalie*)nParam;
    blender->m_fDesiredWeight
        = (s32)(u16)abs_s16(pGoalie->maLocalAngle) / 32768.0f;
}

void Goalie::StrafeSynchronizedSpeedCallback(
    unsigned int nParam, cPN_SAnimController* controller)
{
    Goalie* pGoalie = (Goalie*)nParam;
    controller->m_fPlaybackSpeedScale = pGoalie->mfSpeedScale;
}

void Goalie::ActionSaveSetup(float deltaTime)
{
    float deflectResult = CheckForDelflectAwayFromNet();

    if (deflectResult < 0.0f)
    {
        return;
    }

    if (deflectResult > 0.0f)
    {
        InitActionSaveSetup(false);
        return;
    }

    if (mnOffplayPending != 0)
    {
        InitActionMove(true);
        return;
    }

    mfWaitTime -= deltaTime;
    if (mfWaitTime < 0.02f)
    {
        InitActionSave();
    }
}

void Goalie::RunWeightCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    const Goalie* pGoalie = (Goalie*)nParam;

    s16 diff = (s16)(pGoalie->m_aDesiredFacingDirection
                     - pGoalie->m_aActualFacingDirection);

    s16 minClampedDiff;
    if (diff < -0x31C4)
    {
        minClampedDiff = -0x31C4;
    }
    else
    {
        minClampedDiff = diff;
    }

    s16 clampedDiff;
    if (minClampedDiff > 0x31C4)
    {
        clampedDiff = 0x31C4;
    }
    else
    {
        clampedDiff = minClampedDiff;
    }

    blender->m_fDesiredWeight
        = (float)(clampedDiff + 0x31C4) / 25480.0f;
}

void Goalie::ActionDiveRecover(float fDeltaT)
{
    CheckForLimbEndZoneCollision();

    if (m_pBall == 0)
    {
        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        if (mFatigue.mfEnergyLevel < pTweaks->fGetupEnergyHigh)
        {
            float result = InterpolateRangeClamped(
                pTweaks->fGetupSpeedLow,
                1.0f,
                pTweaks->fGetupEnergyLow,
                pTweaks->fGetupEnergyHigh,
                mFatigue.mfEnergyLevel);
            m_pCurrentAnimController->m_fPlaybackSpeedScale = result;
        }

        if (ShouldStartCrossBlend(5))
        {
            InitActionMove(false);
        }
    }
    else
    {
        if (ShouldStartCrossBlend(6))
        {
            InitActionMoveWB();
        }
    }
}

void Goalie::ActionPass(float deltaTime)
{
    if (m_pBall != 0)
    {
        if (mpPassTarget != 0)
        {
            float dx = mpPassTarget->m_v3Position.x - m_v3Position.x;
            float dy = mpPassTarget->m_v3Position.y - m_v3Position.y;
            float angleRad = nlATan2f(dy, dx);

            m_aDesiredFacingDirection
                = (unsigned short)(s32)(10430.378f * angleRad);
        }
        else
        {
            if (m_pTeam->m_pNet->m_v3NetLocation.x > 0.0f)
            {
                m_aDesiredFacingDirection = 0x8000;
            }
            else
            {
                m_aDesiredFacingDirection = 0;
            }
        }

        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        float fThrowingDirectionSeekSpeed
            = pTweaks->fThrowingDirectionSeekSpeed;
        float fThrowingDirectionSeekFalloff
            = pTweaks->fThrowingDirectionSeekFalloff;
        unsigned short newFacing = SeekDirection(
            m_aActualFacingDirection,
            m_aDesiredFacingDirection,
            fThrowingDirectionSeekSpeed,
            fThrowingDirectionSeekFalloff,
            deltaTime);
        SetFacingDirection(newFacing, true);
    }
    if (ShouldStartCrossBlend(5))
    {
        InitActionMove(false);
    }
}

void Goalie::ActionPassIntercept(float deltaTime)
{
    if (muBallDeflectCount != g_pBall->m_bBallDeflectCount)
    {
        InitActionMove(true);
        return;
    }

    mfWaitTime -= deltaTime;

    switch (mnSubstate)
    {
    case 1:
        if (mfWaitTime <= 0.02f)
        {
            InitActionPassInterceptSave();
        }
        return;

    case 4:
    {
        u16 ballAngle;
        float dx = mv3TargetPosition.x - m_v3Position.x;
        float dy = mv3TargetPosition.y - m_v3Position.y;
        float angleToTarget = nlATan2f(dy, dx);
        u32 targetAngle = (u16)(s32)(10430.378f * angleToTarget);

        dx = g_pBall->m_v3Position.x - m_v3Position.x;
        dy = g_pBall->m_v3Position.y - m_v3Position.y;
        float angleToBall = nlATan2f(dy, dx);
        ballAngle = (u16)(s32)(10430.378f * angleToBall);

        s16 angleDiff
            = (s16)(targetAngle - m_aActualFacingDirection);
        int animID = ChooseRunAnim(angleDiff, mv3TargetPosition, 1.0f);

        s16 ballAngleDiff = (s16)(ballAngle - targetAngle);
        ballAngleDiff
            = ballAngleDiff < 0 ? -ballAngleDiff : ballAngleDiff;
        u16 absBallAngleDiff = (u16)ballAngleDiff;

        if (absBallAngleDiff > 0x4000 && animID != 5
            && (m_eAnimID == 5 || m_eAnimID == 0x25))
        {
            targetAngle += 0x8000;
        }

        if (mfWaitTime > 0.25f && animID != 5)
        {
            PlayNewAnim(animID);
            InitMovementFromAnim(0, v3Zero, 0.0f, false);

            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            float fRunningDirectionSeekSpeed
                = pTweaks->fRunningDirectionSeekSpeed;
            float fRunningDirectionSeekFalloff
                = pTweaks->fRunningDirectionSeekFalloff;
            unsigned short newFacing = SeekDirection(
                m_aActualFacingDirection,
                targetAngle,
                fRunningDirectionSeekSpeed,
                fRunningDirectionSeekFalloff,
                deltaTime);
            SetFacingDirection(newFacing, true);
            return;
        }

        if (CanInterceptPass())
        {
            if (mfWaitTime <= 0.02f)
            {
                InitActionPassInterceptSave();
                return;
            }

            mnSubstate = 1;
            PlayNewAnim(5);

            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
                pTweaks->fRunningDirectionSeekFalloff);
            return;
        }

        float tmp = GoalieSave::mfCrouchDuration;
        if (g_pBall->m_tPassTargetTimer.GetSeconds() < tmp
            && IsCloseToPlane(
                mv3TargetPosition, m_v3Position, 1.2f))
        {
            if (mGoalieActionState == GOALIEACTION_STS_RECOVER)
            {
                return;
            }

            mbIsDown = false;
            mCrouchType = GOALIECROUCH_PASS;
            SetGoalieAction(GOALIEACTION_PRE_CROUCH, 0);
            PlayNewAnim(0x2C);
            InitMovementFromAnim(0, v3Zero, 0.0f, false);
            return;
        }

        mUrgency = URGENCY_HIGH;
        InitActionMove(true);
    }
    }
}

void Goalie::ActionPreCrouch(float deltaTime)
{
    nlVector3 targetPos = g_pBall->m_v3Position;

    if (!CheckForSTSAttack())
    {
        if (g_pBall->GetOwnerFielder() != 0)
        {
            cFielder* pOwnerFielder = g_pBall->GetOwnerFielder();
            if (IsOnSameTeam((cPlayer*)pOwnerFielder))
            {
                InitActionMove(false);
            }
            else if (IsWithinPounceRange())
            {
                if (!fn_8007D740())
                {
                    InitActionPursueBallCarrier();
                }

                if (!fn_8007D740())
                {
                    InitActionPursueBallPounce();

                    if (g_pBall->GetOwnerFielder() != 0
                        && (g_pBall->GetOwnerFielder()->m_eCharacterClass
                                == (eCharacterClass)0x05
                            || g_pBall->GetOwnerFielder()->m_eCharacterClass
                                   == (eCharacterClass)0x0A
                            || g_pBall->GetOwnerFielder()->m_eCharacterClass
                                   == (eCharacterClass)0x0F)
                        && g_pBall->m_v3Position.z > 0.66f)
                    {
                        mbPlayMiss = true;
                    }
                }
            }
            else
            {
                if (pOwnerFielder->m_eActionState != ACTION_UNKNOWN_15
                    && pOwnerFielder->m_eActionState
                           != ACTION_SHOOT_TO_SCORE
                    && pOwnerFielder->m_eActionState != ACTION_SHOT)
                {
                    InitActionMove(true);
                }
                else if (mCrouchType != GOALIECROUCH_SHOT)
                {
                    InitActionMove(true);
                }
            }
        }
        else if (g_pBall->m_pPassTarget == 0)
        {
            if (mpShooter == 0
                || mpShooter->m_eActionState != ACTION_LOOSE_BALL_SHOT
                || mCrouchType != GOALIECROUCH_LOOSEBALL)
            {
                InitActionMove(true);
            }
        }
        else
        {
            if (mCrouchType != GOALIECROUCH_PASS)
            {
                InitActionMove(true);
            }

            targetPos = g_pBall->m_pPassTarget->m_v3Position;
        }

        if (mGoalieActionState == GOALIEACTION_PRE_CROUCH)
        {
            float dx = targetPos.x - m_v3Position.x;
            float dy = targetPos.y - m_v3Position.y;
            float angle = nlATan2f(dy, dx);

            m_aDesiredFacingDirection
                = (unsigned short)(s32)(10430.378f * angle);

            unsigned short newFacing = SeekDirection(
                m_aActualFacingDirection,
                m_aDesiredFacingDirection,
                75000.0f,
                4000.0f,
                deltaTime);
            SetFacingDirection(newFacing, true);
        }
    }
}

void Goalie::ActionPursueBallCarrier(float fDeltaT)
{
    if (!CheckForSTSAttack())
    {
        if (fn_8007C73C())
        {
            CleanupStun();
            ChooseSwatAnim(1);
            SetGoalieAction((eGoalieActionState)0x1F, 0);
            m_fDesiredSpeed = 0.0f;
            m_fActualSpeed = 0.0f;
            SetVelocity(v3Zero);

            if (m_pBall != 0)
            {
                fn_80139D1C(1, GetGlobalPad());
                ReleaseBall(false);
            }

            fn_80098468(this, 0.2f);
            if (GetGlobalPad() != 0)
            {
                fn_8009591C(this, false);
            }
            mbGrabMonty = false;
        }
        else
        {
            cFielder* pOwnerFielder = g_pBall->GetOwnerFielder();

            if (mnOffplayPending != 0 || lbl_806E0C94->mUnidentified20
                || lbl_806E0C94->mUnidentified18 == 3
                || pOwnerFielder == 0
                || IsOnSameTeam((cPlayer*)pOwnerFielder)
                || !IsOpponentBallCarrierInRange())
            {
                InitActionMove(true);
                return;
            }

            if (fn_8007BC40())
            {
                return;
            }

            bool bWallBlock = mfWallBlock > 0.0f;
            if (bWallBlock)
            {
                InitActionMove(false);
                return;
            }

            nlVector3& ballPos = g_pBall->m_v3Position;
            GetLocalPoint(mv3LocalContactPosition, ballPos, m_v3Position, m_aActualFacingDirection);

            nlVector3 ballDelta;
            nlVec3Set(ballDelta,
                ballPos.x - m_v3Position.x,
                ballPos.y - m_v3Position.y,
                ballPos.z - m_v3Position.z);

            nlVector3 desiredPos;
            nlVector3 desiredDir;
            nlVector3 desiredOffset;
            unsigned short desiredAngle;
            FindDesiredGoaliePosition(
                desiredPos, desiredDir, desiredOffset, desiredAngle, 0);

            float pickupDistance = mpLooseBallInfo->mfPickupDistance;
            float pounceRange = 0.5f * pickupDistance;
            float ballDistSq = ballDelta.GetLengthSq3D();
            float pickupDistSq = pickupDistance * pickupDistance;
            float pounceRangeSq = pounceRange * pounceRange;
            float thresholdDist;
            if (ballDistSq > pickupDistSq)
            {
                thresholdDist = pickupDistance;
            }
            else if (ballDistSq < pounceRangeSq)
            {
                thresholdDist = pounceRange;
            }
            else
            {
                thresholdDist = nlSqrt(ballDistSq, true);
            }

            float scale
                = -thresholdDist / nlSqrt(desiredDir.GetLengthSq3D(), true);

            float desiredZ = (scale * desiredDir.z) + desiredOffset.z;
            float desiredY = (scale * desiredDir.y) + desiredOffset.y;
            float desiredX = (scale * desiredDir.x) + desiredOffset.x;
            nlVec3Set(desiredPos, desiredX, desiredY, desiredZ);

            nlVector3 moveDir;
            float moveZ = desiredPos.z - m_v3Position.z;
            float moveY = desiredPos.y - m_v3Position.y;
            float moveX = desiredPos.x - m_v3Position.x;
            nlVec3Set(moveDir, moveX, moveY, moveZ);

            float dotProduct = (moveDir.x * ballDelta.x)
                             + (moveDir.y * ballDelta.y) + (moveDir.z * ballDelta.z);

            if (dotProduct > 0.0f)
            {
                ballDelta = moveDir;
            }

            float dx = ballDelta.x;
            float dy = ballDelta.y;
            float angle = nlATan2f(dy, dx);
            m_aDesiredFacingDirection
                = (u16)(s32)(10430.378f * angle);

            float pickupDistanceSq = mpLooseBallInfo->mfPickupDistance
                                   * mpLooseBallInfo->mfPickupDistance;

            nlVector3 opponentLocalPos;
            GetLocalPoint(opponentLocalPos, pOwnerFielder->m_v3Position, m_v3Position, m_aActualFacingDirection);

            nlVector2 dist1Delta;
            dist1Delta.x = mv3LocalContactPosition.x
                         - mpLooseBallInfo->mv3PickupPos.x;
            dist1Delta.y = mv3LocalContactPosition.y
                         - mpLooseBallInfo->mv3PickupPos.y;
            float dist1Sq = (dist1Delta.x * dist1Delta.x)
                          + (dist1Delta.y * dist1Delta.y);

            nlVector2 dist2Delta;
            dist2Delta.x
                = opponentLocalPos.x - mpLooseBallInfo->mv3PickupPos.x;
            dist2Delta.y
                = opponentLocalPos.y - mpLooseBallInfo->mv3PickupPos.y;
            float dist2Sq = (dist2Delta.x * dist2Delta.x)
                          + (dist2Delta.y * dist2Delta.y);

            float dist3Sq = nlGetLengthSquared2D(
                mv3LocalContactPosition.x, mv3LocalContactPosition.y);

            nlVector2 dist4Delta;
            dist4Delta.x
                = pOwnerFielder->m_v3Position.x - m_v3Position.x;
            dist4Delta.y
                = pOwnerFielder->m_v3Position.y - m_v3Position.y;
            float dist4Sq = (dist4Delta.x * dist4Delta.x)
                          + (dist4Delta.y * dist4Delta.y);

            if ((mv3LocalContactPosition.x < -0.35f)
                || (dist1Sq > 0.36f && dist2Sq > 0.36f
                    && dist3Sq > pickupDistanceSq
                    && dist4Sq > pickupDistanceSq))
            {
                s16 angleDiff = (s16)(m_aDesiredFacingDirection
                                      - m_aActualFacingDirection);
                int animID = ChooseRunAnim(angleDiff, ballPos, 1.0f);
                PlayNewAnim(animID);

                float speedScale = 1.5f;
                if (speedScale
                    != m_pCurrentAnimController->m_fPlaybackSpeedScale)
                {
                    m_pCurrentAnimController->m_fPlaybackSpeedScale
                        = speedScale;
                }

                GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
                InitMovementFromAnimSeek(
                    pTweaks->fRunningDirectionSeekSpeed,
                    pTweaks->fRunningDirectionSeekFalloff);
                return;
            }

            if (IsWithinPounceRange() && !fn_8007D740())
            {
                InitActionPursueBallPounce();

                if (g_pBall->GetOwnerFielder() != 0
                    && (g_pBall->GetOwnerFielder()->m_eCharacterClass
                            == (eCharacterClass)0x05
                        || g_pBall->GetOwnerFielder()->m_eCharacterClass
                               == (eCharacterClass)0x0A
                        || g_pBall->GetOwnerFielder()->m_eCharacterClass
                               == (eCharacterClass)0x0F)
                    && g_pBall->m_v3Position.z > 0.66f)
                {
                    mbPlayMiss = true;
                }
            }
        }
    }
}

void Goalie::ActionPursueBallPounce(float fDeltaT)
{
    if (fn_8007C73C())
    {
        CleanupStun();
        ChooseSwatAnim(1);
        SetGoalieAction((eGoalieActionState)0x1F, 0);
        m_fDesiredSpeed = 0.0f;
        m_fActualSpeed = 0.0f;
        SetVelocity(v3Zero);

        if (m_pBall != 0)
        {
            fn_80139D1C(1, GetGlobalPad());
            ReleaseBall(false);
        }

        fn_80098468(this, 0.2f);
        if (GetGlobalPad() != 0)
        {
            fn_8009591C(this, false);
        }
        mbGrabMonty = false;
        return;
    }

    float animTime = m_pCurrentAnimController->m_fTime;

    if (m_pBall == 0)
    {
        cBall* pBall = g_pBall;
        cPlayer* pOwner = pBall->m_pOwner;

        if (pOwner == 0)
        {
            SetGoalieAction(GOALIEACTION_LOOSEBALL_PICKUP, 0);
            mbIsDown = true;
            mfTargetTime = 0.0f;
            mfWaitTime = -1.0f;
            return;
        }

        if (IsOnSameTeam(pOwner))
        {
            InitActionMove(true);
            return;
        }

        if (CalculateDistanceSquared(
                GetJointPosition(m_nBallJointIndex), pBall->m_v3Position)
                < 0.16000001f
            || CalculateDistanceSquared(
                   GetJointPosition(m_nLeftHandJointIndex), pBall->m_v3Position)
                   < 0.16000001f
            || CalculateDistanceSquared(
                   GetJointPosition(m_nRightHandJointIndex), pBall->m_v3Position)
                   < 0.16000001f)
        {
            ExecutePounce(pOwner, true);
            return;
        }

        float pickupTime = mpLooseBallInfo->mfPickupTime;
        if (animTime < pickupTime
            && g_pBall->m_tShotTimer.m_uPackedTime == 0)
        {
            bool bWallBlock = mfWallBlock > 0.0f;
            if (!bWallBlock)
            {
                float ratio
                    = nlMaxEquals(animTime / pickupTime, 0.0f);
                ratio = nlMinEquals(ratio, 1.0f);
                float interpValue
                    = ratio * (ratio * ((-2.0f * ratio) + 3.0f));

                if (interpValue < 0.99f && !mbPlayMiss)
                {
                    TrackTarget(g_pBall->m_v3Position, interpValue, fDeltaT * lbl_806DBD28);
                }
            }
        }
    }

    if (animTime > 0.95f)
    {
        if (m_eAnimID != 0x85)
        {
            CheckForLimbEndZoneCollision();

            if (m_pBall == 0)
            {
                GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
                if (mFatigue.mfEnergyLevel < pTweaks->fGetupEnergyHigh)
                {
                    float speed = InterpolateRangeClamped(
                        pTweaks->fGetupSpeedLow,
                        1.0f,
                        pTweaks->fGetupEnergyLow,
                        pTweaks->fGetupEnergyHigh,
                        mFatigue.mfEnergyLevel);
                    m_pCurrentAnimController->m_fPlaybackSpeedScale = speed;
                }

                if (ShouldStartCrossBlend(5))
                {
                    InitActionMove(false);
                    return;
                }
            }
            else if (ShouldStartCrossBlend(6))
            {
                InitActionMoveWB();
                return;
            }
        }
        else
        {
            InitActionPursueRecover();
        }
    }
}

void Goalie::fn_8008A610(float fDeltaT)
{
    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || (mpTarget->m_pBall != 0
            && !fn_8007B9A0(
                this, mpTarget->m_v3Position, lbl_806DBC98)))
    {
        InitActionMove(false);
        return;
    }

    bool bAnimHeld
        = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
       && m_pCurrentAnimController->m_fTime == 0.0f;
    if (bAnimHeld)
    {
        InitActionMove(false);
    }

    if (mPursueDekeState == 0)
    {
        bool bWallBlock = mfWallBlock > 0.0f;
        if (bWallBlock)
        {
            InitActionMove(false);
            return;
        }

        switch (mPursueDekeType)
        {
        case 1:
        case 2:
            if (mpTarget->m_eActionState != (eFielderActionState)1)
            {
                break;
            }

            float animFrames
                = (float)mpTarget->m_pCurrentAnimController->m_pSAnim
                      ->m_nNumKeys;
            float pickupFrames;
            float threshold
                = fn_8007BEEC(this, mpTarget) * animFrames;
            pickupFrames = mpLooseBallInfo->mfPickupTime
                         * mpLooseBallInfo->mfAnimDuration;
            pickupFrames = 30.0f * pickupFrames;
            float targetFrame = mpTarget->m_pCurrentAnimController->m_fTime
                              * (float)mpTarget->m_pCurrentAnimController
                                    ->m_pSAnim->m_nNumKeys;
            if (targetFrame > threshold - pickupFrames)
            {
                mPursueDekeType = 0;
            }
            break;

        default:
            break;
        }

        switch (mPursueDekeType)
        {
        case 6:
        {
            if (mpTarget->m_eActionState != (eFielderActionState)1)
            {
                InitActionMove(false);
                return;
            }

            if (!mpTarget->mUnidentified475)
            {
                mPursueDekeType = 4;
                return;
            }

            mbDoIntercept = true;
            nlVector3 v3Direction;
            nlVector3 v3Focus;
            unsigned short aFacing;
            FindDesiredGoaliePosition(mv3TargetPosition, v3Direction, v3Focus, aFacing, 0);
            mv3NavTarget = mv3TargetPosition;
            m_aDesiredFacingDirection = aFacing;
            mUrgency = URGENCY_HIGH;
            fn_800797DC(
                this, 1, fDeltaT, 0.2f + lbl_806DBB08);
            return;
        }

        case 0:
        case 3:
        case 5:
        {
            if (!mpTarget->mUnidentified475)
            {
                InitActionMove(false);
                return;
            }

            mpLooseBallInfo = &LooseBallAnims::mLooseBallKickInfo[1];
            mbDoNavigate = false;

            nlVector2 v2Delta;
            v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
            v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
            float distSq = nlVec2LengthSquared(v2Delta);
            float radius;
            mpTarget->m_pPhysicsCharacter->GetRadius(&radius);

            if (!mpTarget->mUnidentified475
                || distSq
                       > (mpLooseBallInfo->mfPickupDistance + radius
                             + lbl_806DBD10)
                             * (mpLooseBallInfo->mfPickupDistance + radius
                                 + lbl_806DBD10))
            {
                if (mpTarget->m_pBall == 0)
                {
                    InitActionMove(false);
                    return;
                }

                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mUrgency = URGENCY_HIGH;
                fn_800797DC(this, 2, fDeltaT, lbl_806DBBBC);
                return;
            }

            const LooseBallInfo* pCloseInfo
                = &LooseBallAnims::mLooseBallKickInfo[2];
            float closeDistance = pCloseInfo->mfPickupDistance + radius;
            if (distSq <= closeDistance * closeDistance)
            {
                mpLooseBallInfo = pCloseInfo;
            }

            PlayNewAnim(mpLooseBallInfo->mnAnimID);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            mPursueDekeState = 1;
            return;
        }

        case 7:
        {
            mbDoNavigate = false;

            nlVector2 v2Delta;
            v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
            v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
            float distSq = nlVec2LengthSquared(v2Delta);
            float radius;
            mpTarget->m_pPhysicsCharacter->GetRadius(&radius);

            if (distSq
                > (radius + lbl_806DBD14) * (radius + lbl_806DBD14))
            {
                if (mpTarget->m_pBall == 0)
                {
                    InitActionMove(false);
                    return;
                }

                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mUrgency = URGENCY_HIGH;
                fn_800797DC(this, 2, fDeltaT, lbl_806DBBBC);
                return;
            }

            if (mpTarget->fn_8003E6FC())
            {
                return;
            }

            PlayNewAnim(0xAD);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            SetGoalieAction((eGoalieActionState)0x1F, 0);
            mbGrabMonty = true;
            mpMonty = mpTarget;
            fn_8004F204(mpTarget);
            g_pBall->m_bBallPathChangeCount = 0;
            m_fDesiredSpeed = 0.0f;
            m_fActualSpeed = 0.0f;
            SetVelocity(v3Zero);
            fn_800EBBFC(mUnidentified318, 0x76520305, 0, 0);
            return;
        }

        case 1:
        case 2:
        {
            if (!mpTarget->mUnidentified475)
            {
                InitActionMove(false);
                return;
            }

            mbDoNavigate = false;
            nlVector2 v2Delta;
            v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
            v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
            float distSq = nlVec2LengthSquared(v2Delta);
            float radius;
            mpTarget->m_pPhysicsCharacter->GetRadius(&radius);

            if (distSq
                > (mpLooseBallInfo->mfPickupDistance + radius
                      + lbl_806DBD0C)
                      * (mpLooseBallInfo->mfPickupDistance + radius
                          + lbl_806DBD0C))
            {
                if (mpTarget->m_pBall == 0)
                {
                    InitActionMove(false);
                    return;
                }

                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mUrgency = URGENCY_HIGH;
                fn_800797DC(this, 2, fDeltaT, lbl_806DBBBC);
                return;
            }

            PlayNewAnim(mpLooseBallInfo->mnAnimID);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            mPursueDekeState = 1;
            return;
        }

        case 4:
        {
            nlVector3 v3Target = mpTarget->m_v3Position;
            if (mpTarget->mUnidentified475)
            {
                mPursueDekeType = 0;
            }

            if (mpTarget->m_eCharacterClass == (eCharacterClass)0x10)
            {
                nlVector3 v3Direction;
                nlSinCos(&v3Direction.y, &v3Direction.x, m_aActualFacingDirection);
                v3Direction.z = 0.0f;
                nlVec3ScaleAdd(v3Target, lbl_806DBD24, v3Direction, mpTarget->m_v3Position);
            }

            float dx = v3Target.x - m_v3Position.x;
            float dy = v3Target.y - m_v3Position.y;
            mv3NavTarget = v3Target;
            float angle = nlATan2f(dy, dx);
            m_aDesiredFacingDirection
                = (u16)(s32)(10430.378f * angle);
            mUrgency = URGENCY_HIGH;
            fn_800797DC(this, 2, fDeltaT, 0.3f);
            return;
        }
        }
    }
    else if (mPursueDekeState == 1)
    {
        if (!mpTarget->mUnidentified475)
        {
            float pickupTime = mpLooseBallInfo->mfPickupTime;
            float animTime = m_pCurrentAnimController->m_fTime;
            if (animTime < pickupTime - 0.1f)
            {
                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mPursueDekeState = 0;
                fn_800797DC(this, 2, fDeltaT, 0.3f);
            }
            else
            {
                mPursueDekeState = 2;
            }
            return;
        }

        if (mpTarget->IsFallenDown())
        {
            return;
        }

        float animTime = m_pCurrentAnimController->m_fTime;
        float pickupTime = mpLooseBallInfo->mfPickupTime;
        float ratio = animTime / pickupTime;
        ratio = nlMaxEquals(ratio, 0.0f);
        ratio = nlMinEquals(ratio, 1.0f);
        float interpValue
            = ratio * (ratio * ((-2.0f * ratio) + 3.0f));

        if (interpValue < 0.99f)
        {
            bool bWallBlock = mfWallBlock > 0.0f;
            if (!bWallBlock)
            {
                TrackTarget(mpTarget->m_v3Position, interpValue, fDeltaT * lbl_806DBC8C);
            }
            return;
        }

        nlVector2 v2Delta;
        v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
        v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
        float distSq = nlVec2LengthSquared(v2Delta);
        float radius;
        mpTarget->m_pPhysicsCharacter->GetRadius(&radius);
        if (distSq
            < (mpLooseBallInfo->mfPickupDistance + radius + 0.8f)
                  * (mpLooseBallInfo->mfPickupDistance + radius + 0.8f))
        {
            fn_800809D0(this, mpTarget, false);
        }
    }
}

void Goalie::ActionOffplay(float fDeltaT)
{
    if (ShouldStartCrossBlend(0x99))
    {
        int animID;
        int currentAnimID = m_eAnimID;
        if (currentAnimID == 0x92 || currentAnimID == 0x94
            || currentAnimID == 0x96)
        {
            animID = 0x96;
        }
        else if (currentAnimID == 0x91 || currentAnimID == 0x93
                 || currentAnimID == 0x95)
        {
            animID = 0x95;
        }
        else
        {
            static FilteredRandomRange randgenDejected;
            int index = randgenDejected.genrand(5);
            animID = gOffplayDejected[index];
        }

        SetAnimState(animID, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    CheckForLimbEndZoneCollision();
}

void Goalie::ActionLooseBallPursueBouncing(float deltaTime)
{
    if (IsPassThreat() || mnOffplayPending != GOALIE_OFFPLAY_NONE
        || !IsLooseBallClose(0.0f) || g_pBall->m_pOwner != 0)
    {
        InitActionMove(true);
        return;
    }

    bool bWallBlock = mfWallBlock > 0.0f;
    if (bWallBlock)
    {
        InitActionMove(true);
        return;
    }

    if (muBallDeflectCount != g_pBall->m_bBallDeflectCount)
    {
        InitActionLooseBallSetup();
        return;
    }

    mfTargetTime -= deltaTime;
    if (mfTargetTime < 0.1f
        || (g_pBall->m_v3Position.z < 1.0f
            && g_pBall->m_v3Velocity.z < 3.0f))
    {
        InitActionLooseBallSetup();
        return;
    }

    nlVector3 v3TargetPos;
    nlVector3 v3TargetVel;
    FakeBallWorld::GetPredictedBallPosition(
        mfTargetTime, v3TargetPos, v3TargetVel);

    if (mUnidentified254 == 0)
    {
        nlVector3 delta;
        nlVec3Sub2D(delta, m_v3Position, v3TargetPos);
        if (delta.GetLengthSq2D() < mfTargetDist)
        {
            PlayNewAnim(5);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);

            GetLocalPoint(mv3LocalContactPosition, v3TargetPos, m_v3Position, m_aActualFacingDirection);
            GetLocalPoint(mv3LocalContactVelocity, v3TargetVel, v3Zero, m_aActualFacingDirection);

            InitActionLooseBallCatch();
            return;
        }
    }

    const nlVector3& pos = m_v3Position;
    float dx = v3TargetPos.x - pos.x;
    float dy = v3TargetPos.y - pos.y;
    float angle = nlATan2f(dy, dx);
    m_aDesiredFacingDirection = (u16)(s32)(10430.378f * angle);

    if (CalculateDistanceSquared(v3TargetPos, mv3TargetPosition)
        > mfTargetDist)
    {
        InitActionLooseBallSetup();
        return;
    }

    if (m_eAnimID != 0x24)
    {
        PlayNewAnim(0x24);
        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
            pTweaks->fRunningDirectionSeekFalloff);
    }

    CheckForLimbEndZoneCollision();
}

void Goalie::ActionSnapBall(float fDeltaT)
{
    unsigned short aRootRot;
    float fTimeLeft;
    nlVector3 v3TargetPos;
    nlVector3 v3RootPos;

    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || lbl_806E0C94->mUnidentified20
        || lbl_806E0C94->mUnidentified18 == 3)
    {
        if (m_pBall != 0)
        {
            ReleaseBall(false);
        }
        InitActionMove(true);
        return;
    }

    if (mUnidentified254 == 0 && g_pBall->m_pOwner != this)
    {
        TacklePlayer(g_pBall->m_pOwner);
        StealBall(g_pBall->m_pOwner);

        fTimeLeft = m_tNoPickupTimer.GetSeconds();

        if (fTimeLeft > 0.0f)
        {
            GetCurrentAnimFuture(m_nBallJointIndex,
                m_pCurrentAnimController->m_fTime,
                v3TargetPos,
                v3RootPos,
                aRootRot);

            float interpFactor;
            float invInterpFactor;

            interpFactor = (1.0f / mfWaitTime) * (mfWaitTime - fTimeLeft);
            invInterpFactor = 1.0f - interpFactor;

            v3TargetPos.x = (invInterpFactor * g_pBall->m_v3Position.x)
                          + (interpFactor * v3TargetPos.x);
            v3TargetPos.y = (invInterpFactor * g_pBall->m_v3Position.y)
                          + (interpFactor * v3TargetPos.y);
            v3TargetPos.z = (invInterpFactor * g_pBall->m_v3Position.z)
                          + (interpFactor * v3TargetPos.z);

            g_pBall->SetPosition(v3TargetPos);
            return;
        }

        PickupBall(g_pBall);
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
        mbPickedUp = true;
        return;
    }

    if (m_pBall == 0)
    {
        InitActionMove(true);
        return;
    }

    bool shouldMoveWB
        = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
       && m_pCurrentAnimController->m_fTime == 1.0f;

    if (shouldMoveWB)
    {
        InitActionMoveWB();
    }
}

void Goalie::ActionGrabBall(float fDeltaT)
{
    bool bShouldInitMove = true;
    if (mUnidentified254 == 0)
    {
        cPN_SAnimController* pController = m_pCurrentAnimController;
        bShouldInitMove = false;
        if (pController->m_ePlayMode == PM_HOLD
            && pController->m_fTime == 1.0f)
        {
            bShouldInitMove = true;
        }
    }

    if (bShouldInitMove)
    {
        if (m_pBall == 0)
        {
            InitActionMove(true);
            return;
        }
        InitActionMoveWB();
        return;
    }

    if (g_pBall->m_pOwner != this)
    {
        if (g_pBall->GetOwnerFielder() == 0)
        {
            InitActionMove(false);
            return;
        }

        float pickupTime = mpLooseBallInfo->mfPickupTime;
        float fTimeThreshold = 0.1f + pickupTime;
        float fCurrentTime = m_pCurrentAnimController->m_fTime;
        if (fCurrentTime < fTimeThreshold)
        {
            float fInterpFactor = fCurrentTime / fTimeThreshold;
            TrackTarget(g_pBall->m_v3Position, fInterpFactor, fDeltaT * lbl_806DBC8C);

            const nlVector3& jointPos
                = GetJointPosition(m_nBallJointIndex);

            nlVector3 delta;
            nlVec3Set(delta,
                g_pBall->m_v3Position.x - jointPos.x,
                g_pBall->m_v3Position.y - jointPos.y,
                g_pBall->m_v3Position.z - jointPos.z);

            if (nlGetLengthSquared3D(delta.x, delta.y, delta.z) < 0.25f)
            {
                StealBall(g_pBall->m_pOwner);
                PickupBall(g_pBall);
                m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                m_pPhysicsCharacter->m_CanCollideWithWall = true;
                mbPickedUp = true;
                EmitGoalieCatch(this, "goalie_catch", false);
            }
        }
    }
}

void Goalie::fn_8008B718(float fDeltaT)
{
    nlVector3 pos;

    if (m_v3Position.z > 0.0f)
    {
        nlVector3 adjustedPos = m_v3Position;
        adjustedPos.z -= fDeltaT * lbl_806DBC94;
        if (adjustedPos.z < 0.0f)
        {
            adjustedPos.z = 0.0f;
        }
        SetPosition(adjustedPos);
    }

    cPN_SAnimController* pController = m_pCurrentAnimController;
    bool bShouldInitMove = false;
    if (pController->m_ePlayMode == PM_HOLD
        && pController->m_fTime == 1.0f)
    {
        bShouldInitMove = true;
    }

    if (bShouldInitMove)
    {
        InitActionMove(false);
        return;
    }

    pos = m_v3Position;
    float goalLineX = cField::GetGoalLineX(1U);
    float absX = (float)fabs(pos.x);
    float adjustedGoalLineX = goalLineX + 1.5f;

    if (absX > adjustedGoalLineX)
    {
        float radius;
        m_pPhysicsCharacter->GetRadius(&radius);

        float netWidth = cNet::m_fNetWidth;
        float netWidthAdjusted
            = (0.5f * netWidth) - radius
            - (absX - adjustedGoalLineX);

        pos.x = nlMinEquals(
            nlMaxEquals(pos.x, -adjustedGoalLineX), adjustedGoalLineX);
        pos.y = nlMinEquals(
            nlMaxEquals(pos.y, -netWidthAdjusted), netWidthAdjusted);
        SetPosition(pos);
    }
}

void Goalie::InitActionPass(bool useTarget)
{
    int animID;

    SetGoalieAction(GOALIEACTION_PASS, 0);
    mpPassTarget = 0;

    if (useTarget)
    {
        cPlayer* pPassTarget = FindOpenPassTarget();
        mpPassTarget = pPassTarget;

        if (mpPassTarget != 0 && IsTargetViable(mpPassTarget))
        {
            if (fn_8007C904(this, mpPassTarget->m_v3Position))
            {
                animID = 0;
            }
            else
            {
                GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;

                nlVector2 v2Distance;
                v2Distance.x
                    = m_v3Position.x - mpPassTarget->m_v3Position.x;
                v2Distance.y
                    = m_v3Position.y - mpPassTarget->m_v3Position.y;
                float fDistanceSq = nlVec2LengthSquared(v2Distance);
                float fKickDistanceSq
                    = nlGetLengthSquared1D(pTweaks->fKickDistanceMin);
                float fOverhandThrowDistanceSq = nlGetLengthSquared1D(
                    pTweaks->fOverhandThrowDistanceMin);
                float fOpenTo = OpenTo(this, mpPassTarget);

                if (GetGlobalPad() != 0)
                {
                    if (fn_80331C04(GetGlobalPad(), 0x17, true))
                    {
                        animID = 2;
                    }
                    else if (fDistanceSq > fOverhandThrowDistanceSq
                             || fOpenTo < 0.85f)
                    {
                        animID = 0;
                    }
                    else
                    {
                        animID = 1;
                    }
                }
                else if (fDistanceSq > fKickDistanceSq)
                {
                    animID = 2;
                }
                else if (fDistanceSq > fOverhandThrowDistanceSq
                         || fOpenTo < 0.85f)
                {
                    animID = 0;
                }
                else
                {
                    animID = 1;
                }
            }
        }
        else
        {
            mpPassTarget = 0;
        }
    }

    if (mpPassTarget == 0)
    {
        nlVector3 v3Position = m_v3Position;
        float fXOffset;
        if (m_v3Position.x < 0.0f)
        {
            fXOffset = 4.0f;
        }
        else
        {
            fXOffset = -4.0f;
        }
        v3Position.x += fXOffset;
        animID = fn_8007C904(this, v3Position) ? 0 : 2;
    }

    SetAnimState(animID, true, 0.2f, false, false);
    GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
    InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
        pTweaks->fRunningDirectionSeekFalloff);
}

void Goalie::InitActionPreCrouch(eGoalieCrouchType crouchType)
{
    if (mGoalieActionState == GOALIEACTION_STS_RECOVER)
    {
        return;
    }

    mCrouchType = crouchType;
    mbIsDown = false;
    SetGoalieAction(GOALIEACTION_PRE_CROUCH, 0);
    PlayNewAnim(0x2C);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
}

void Goalie::fn_8008BBB0(
    cFielder* pTarget, int nPursueDekeType)
{
    if (fn_8007D740())
    {
        return;
    }

    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_13, 0);
    mPursueDekeType = nPursueDekeType;
    mPursueDekeState = 0;
    mpTarget = pTarget;
    mbIsDown = false;

    switch (nPursueDekeType)
    {
    case 1:
    case 2:
        mpLooseBallInfo = &LooseBallAnims::mUnknownD0BC;
        break;
    default:
        break;
    }

    if (mUnidentified254 != 0)
    {
        fn_80097648(0.1f);
    }

    mUrgency = URGENCY_HIGH;
    mnSubstate = 1;
    fn_8008A610(0.0f);

    if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_13)
    {
        PlayerAttackData data;
        data.pAttacker = this;
        data.nAttackerPadID = -1;
        data.pTarget = mpTarget;
        data.mUnidentified0C = 2;
        data.mUnidentified10 = false;
        fn_8005E408(lbl_806E0C94, &data);
    }
}

void Goalie::InitActionLooseBallPickup(
    float fDistance, bool bStartPickup)
{
    SetGoalieAction(GOALIEACTION_LOOSEBALL_PICKUP, 0);
    SetAnimState(mpLooseBallInfo->mnAnimID, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mMoveDirection = GOALIEDIR_IDLE;
    mfTargetTime = 0.0f;
    mfWaitTime = -1.0f;
    mbPickedUp = false;
    mbIsDown = true;

    if (fDistance < mpLooseBallInfo->mfPickupDistance)
    {
        float fRemaining = mpLooseBallInfo->mfPickupDistance - fDistance;
        float fPickupTime = mpLooseBallInfo->mfPickupTime;
        mfTargetTime
            = fRemaining * fPickupTime / mpLooseBallInfo->mfPickupDistance;

        cPN_SAnimController* pController = m_pCurrentAnimController;
        float fNewAnimTime = mfTargetTime;
        pController->SetTime(fNewAnimTime);
    }

    if (bStartPickup)
    {
        fn_8007B214(this);
    }
}

void Goalie::InitActionSaveReposition()
{
    mv3NavTarget = mv3TargetPosition;
    mMoveDirection = GOALIEDIR_IDLE;
    SetGoalieAction(GOALIEACTION_SAVE_REPOSITION, 0);

    nlVector3 v3Delta;
    nlVec3Sub2D(v3Delta, m_v3Position, mv3NavTarget);
    mfTargetDist = nlGetLengthSquared2D(v3Delta.x, v3Delta.y);

    float fBallDy = g_pBall->m_v3Position.y - m_v3Position.y;
    float fBallDx = g_pBall->m_v3Position.x - m_v3Position.x;
    m_aDesiredFacingDirection
        = (u16)(s32)(nlATan2f(fBallDy, fBallDx) * 10430.378f);

    fn_800797DC(this, 0, 0.0f, lbl_806DBBBC);
    if (mfWaitTime > 0.4f)
    {
        mUrgency = URGENCY_MED;
    }
    else
    {
        mUrgency = URGENCY_HIGH;
    }
}

void Goalie::InitActionLooseBallPursueRolling()
{
    mv3NavTarget = mv3TargetPosition;
    if (mGoalieActionState != GOALIEACTION_LOOSEBALL_PURSUE_ROLLING)
    {
        SetGoalieAction(GOALIEACTION_LOOSEBALL_PURSUE_ROLLING, 0);
    }

    float fDy = mv3TargetPosition.y - m_v3Position.y;
    float fDx = mv3TargetPosition.x - m_v3Position.x;
    m_aDesiredFacingDirection
        = (u16)(s32)(nlATan2f(fDy, fDx) * 10430.378f);

    mv3NavTarget = mv3TargetPosition;
    mUrgency = URGENCY_MED;
    mbIsDown = false;
}

void Goalie::InitActionLooseBallSetup()
{
    if (fn_8007C590(this))
    {
        return;
    }

    if (!IsLooseBallClose(*fn_800A636C(lbl_806E0E00)
                ->mpLooseBallChaseDistance))
    {
        InitActionMove(true);
        return;
    }

    g_pBall->m_uGoalType = 4;

    m_pPhysicsCharacter->m_CanCollideWithBall = true;
    mbDoHeadTrack = true;
    mbPickedUp = false;
    mbIsDown = false;

    const nlVector3* pBallVelocity = &g_pBall->m_v3Velocity;
    nlVector3 v3BallPosition = g_pBall->m_v3Position;
    const nlVector3& v3NetBase = m_pTeam->m_pNet->m_v3NetLocation;
    muBallDeflectCount = g_pBall->m_bBallDeflectCount;
    mUnidentified3E4 = g_pBall->mUnidentified008;

    bool bInCone = fn_8007D644(this);
    float fBallSpeed = pBallVelocity->x * pBallVelocity->x
                     + pBallVelocity->y * pBallVelocity->y
                     + pBallVelocity->z * pBallVelocity->z;
    float fAbsBallX;

    if (fBallSpeed > lbl_806DBC80 * lbl_806DBC80 && bInCone)
    {
        float fAbsGoalieX = (float)fabs(m_v3Position.x);
        fAbsBallX = (float)fabs(v3BallPosition.x);
        if (fAbsBallX < fAbsGoalieX - 1.5f)
        {
            float fTimeTilSave
                = fn_8007EDF4(this, fn_800776B4());

            if (fTimeTilSave > 0.0f && fTimeTilSave < 2.0f)
            {
                if (mUnidentified254 != 0)
                {
                    muSaveType = 0x0000FFFC;
                }
                else
                {
                    muSaveType = 0x0000FFFF;
                }

                mbShouldMiss = false;
                mfTimeTilSave = fn_8007ECB4(
                    this, fTimeTilSave, muSaveType, false, false);

                if (mfTimeTilSave > 0.0f)
                {
                    if (1.0f + mBlendInfo.mv3BlendedSavePos.z
                        >= mv3LocalContactPosition.z)
                    {
                        mfWaitTime = mfTimeTilSave
                                   - mBlendInfo.mfMilestoneTime[2];
                        if (mfWaitTime < lbl_806DBD04)
                        {
                            InitActionSave();
                            return;
                        }

                        if (lbl_806DBC90 && fn_8007AE38(this))
                        {
                            InitActionSaveReposition();
                            return;
                        }

                        SetGoalieAction(GOALIEACTION_SAVE_SETUP, 0);
                        SetAnimState(7, true, 0.2f, false, false);

                        GoalieTweaks* pTweaks
                            = (GoalieTweaks*)m_pTweaks;
                        InitMovementFromAnimSeek(
                            pTweaks->fSaveDirectionSeekSpeed,
                            pTweaks->fSaveDirectionSeekFalloff);
                        return;
                    }

                    mbShouldMiss = false;
                    if (fn_8007BF68(this, true))
                    {
                        return;
                    }
                }
            }
        }
    }

    if (pBallVelocity->z < 3.0f && v3BallPosition.z < 1.5f)
    {
        if (mUnidentified254 == 0 && bInCone
            && fBallSpeed > 0.25f
            && fn_8007B9A0(this, v3BallPosition, 5.0f))
        {
            nlVector3 v3GuessBallPos;
            nlVector3 v3GuessBallVel;
            mpLooseBallInfo = LooseBallAnims::GetDesperationInfo(1);
            FakeBallWorld::GetPredictedBallPosition(
                mpLooseBallInfo->mfPickupTime
                    * mpLooseBallInfo->mfAnimDuration,
                v3GuessBallPos,
                v3GuessBallVel);

            float fPanicLineX = cField::GetGoalLineX(1U) - 2.0f;
            float fAbsGuessX = (float)fabs(v3GuessBallPos.x);

            if (fAbsGuessX > fPanicLineX)
            {
                SetGoalieAction(
                    GOALIEACTION_LOOSEBALL_DESPERATE, 0);
                mbIsDown = true;

                if ((float)fabs(v3BallPosition.x) >= fPanicLineX)
                {
                    mv3TargetPosition = v3BallPosition;
                }
                else
                {
                    float fGoalLineX2 = cField::GetGoalLineX(1U);
                    if (fAbsGuessX < fGoalLineX2)
                    {
                        mv3TargetPosition = v3GuessBallPos;
                    }
                    else
                    {
                        mv3TargetPosition.x
                            = v3NetBase.x > 0.0f
                                ? fPanicLineX
                                : -fPanicLineX;
                        float fGuessY = v3GuessBallPos.y;
                        float fBallPosY = v3BallPosition.y;
                        float fBallPosX = v3BallPosition.x;
                        float fTargetX = mv3TargetPosition.x;
                        float fDiffY = fBallPosY - fGuessY;
                        float fGuessX = v3GuessBallPos.x;
                        float fDiffXTarget = fBallPosX - fTargetX;
                        float fDiffXOrig = fBallPosX - fGuessX;
                        mv3TargetPosition.y
                            = fBallPosY
                            - fDiffXTarget * fDiffY / fDiffXOrig;
                    }
                }

                mv3TargetPosition.z = 0.0f;
                fn_8007E940(this, 0.75f);
                PlayNewAnim(mpLooseBallInfo->mnAnimID);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
                return;
            }

            float fInterceptTime;
            float fClosestDist;
            bool bFound = fn_8016EA10(m_v3Position,
                1.0f,
                6.0f,
                mv3TargetPosition,
                mv3TargetVelocity,
                fInterceptTime,
                fClosestDist,
                3.0f);

            if (bFound && mv3TargetPosition.z < 1.0f
                && fClosestDist < 0.75f)
            {
                SetGoalieAction(
                    GOALIEACTION_LOOSEBALL_DESPERATE, 0);
                mbIsDown = true;

                float fLimitX = cField::GetGoalLineX(1U) - 0.5f;
                if ((float)fabs(mv3TargetPosition.x) > fLimitX)
                {
                    if ((float)fabs(v3BallPosition.x) > fLimitX)
                    {
                        mv3TargetPosition = v3BallPosition;
                    }
                    else
                    {
                        mv3TargetPosition.y = v3BallPosition.y
                                            - (v3BallPosition.x - fLimitX)
                                                  * (v3BallPosition.y
                                                      - mv3TargetPosition.y)
                                                  / (v3BallPosition.x
                                                      - mv3TargetPosition.x);
                        mv3TargetPosition.x
                            = v3NetBase.x > 0.0f
                                ? fLimitX
                                : -fLimitX;
                    }
                }

                fn_8007E940(this, 0.75f);
                float fAnimTime = mpLooseBallInfo->mfPickupTime
                                * mpLooseBallInfo->mfAnimDuration;
                mfTargetTime = fInterceptTime - fAnimTime;

                if (mfTargetTime < 0.02f)
                {
                    PlayNewAnim(mpLooseBallInfo->mnAnimID);
                    InitMovementFromAnim(0, v3Zero, 1.0f, false);
                    return;
                }

                mv3NavTarget = mv3TargetPosition;
                float fDyNav
                    = v3BallPosition.y - m_v3Position.y;
                float fDxNav
                    = v3BallPosition.x - m_v3Position.x;
                m_aDesiredFacingDirection
                    = (u16)(s32)(nlATan2f(fDyNav, fDxNav)
                                 * 10430.378f);

                if (mfTargetTime > 1.0f)
                {
                    mUrgency = URGENCY_LOW;
                }
                else if (mfTargetTime > 0.5f)
                {
                    mUrgency = URGENCY_MED;
                }
                else
                {
                    mUrgency = URGENCY_HIGH;
                }

                fn_800797DC(this, 2, 0.0f, 0.0f);
                return;
            }
        }

        fAbsBallX = (float)fabs(v3BallPosition.x);
        float fMinKickLine
            = 0.35f
                * (cField::GetGoalLineX(1U)
                    - cField::GetPenaltyBoxX(1U))
            + cField::GetPenaltyBoxX(1U);
        bool bDoGrab = false;

        for (int nPlayerIndex = 0; nPlayerIndex < 4;
            ++nPlayerIndex)
        {
            cPlayer* pPlayer
                = fn_800A6A84(m_pTeam, nPlayerIndex);
            if (pPlayer->GetGlobalPad() != 0)
            {
                bDoGrab = true;
                break;
            }
        }

        if (!IsLooseBallClose(0.0f))
        {
            cFielder* pOpponent
                = fn_8009664C(this, v3BallPosition, true);
            if (pOpponent != 0)
            {
                nlVector2 v2OpponentDistance;
                v2OpponentDistance.x
                    = pOpponent->m_v3Position.x - v3BallPosition.x;
                v2OpponentDistance.y
                    = pOpponent->m_v3Position.y - v3BallPosition.y;
                float fOppDistSq
                    = nlVec2LengthSquared(v2OpponentDistance);

                nlVector2 v2GoalieDistance;
                v2GoalieDistance.x
                    = m_v3Position.x - v3BallPosition.x;
                v2GoalieDistance.y
                    = m_v3Position.y - v3BallPosition.y;
                float fGoalieDistSq
                    = nlVec2LengthSquared(v2GoalieDistance);

                if (fGoalieDistSq > fOppDistSq)
                {
                    if (mGoalieActionState == GOALIEACTION_MOVE)
                    {
                        return;
                    }
                    InitActionMove(true);
                    return;
                }
            }
        }
        else if (mUnidentified254 != 0)
        {
            bDoGrab = false;
        }
        else if (fAbsBallX > fMinKickLine)
        {
            bDoGrab = true;
        }
        else
        {
            cPlayer* pPassTarget = FindOpenPassTarget();
            if (pPassTarget != 0)
            {
                nlVector3 v3BallDelta;
                nlVec3Sub(
                    v3BallDelta, v3BallPosition, m_v3Position);

                nlVector3 v3TargetDelta;
                nlVec3Sub(v3TargetDelta,
                    pPassTarget->m_v3Position,
                    m_v3Position);

                float fBallDist
                    = nlSqrt(v3BallDelta.GetLengthSq3D(), true);
                float fInvDist = 1.0f / fBallDist;
                nlVec3Scale(v3BallDelta, fInvDist);

                float fInvTargetDist = nlRecipSqrt(
                    v3TargetDelta.GetLengthSq3D(), true);
                nlVec3Scale(v3TargetDelta, fInvTargetDist);

                nlVector3 v3Right;
                nlVec3Set(v3Right,
                    m_m4WorldMatrix.m11,
                    m_m4WorldMatrix.m12,
                    m_m4WorldMatrix.m13);

                if (fBallDist < 1.2f)
                {
                    bDoGrab = true;
                }
                else
                {
                    float fDotBallTarget = nlVec3DotProduct(
                        v3BallDelta, v3TargetDelta);
                    if (fDotBallTarget < 0.7071f)
                    {
                        bDoGrab = true;
                    }
                    else
                    {
                        float fDotRight = nlVec3DotProduct(
                            v3BallDelta, v3Right);
                        if (fDotRight < 0.0f)
                        {
                            bDoGrab = true;
                        }
                        else
                        {
                            cFielder* pOpp = fn_8009664C(
                                this, v3BallPosition, true);
                            if (pOpp != 0)
                            {
                                nlVector2 v2OpponentDistance;
                                v2OpponentDistance.x
                                    = pOpp->m_v3Position.x
                                    - v3BallPosition.x;
                                v2OpponentDistance.y
                                    = pOpp->m_v3Position.y
                                    - v3BallPosition.y;
                                if (nlVec2LengthSquared(
                                        v2OpponentDistance)
                                    < fBallDist * fBallDist)
                                {
                                    bDoGrab = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!bDoGrab && mUnidentified254 == 0)
        {
            float fAbsGoalieX = (float)fabs(m_v3Position.x);
            float fAbsBallXPos = (float)fabs(v3BallPosition.x);
            float fAbsBallYDist
                = (float)fabs(v3BallPosition.y - m_v3Position.y);
            float fDiffX = fAbsBallXPos - fAbsGoalieX;
            u16 nAbsAngle = (u16)(s32)(nlATan2f(fAbsBallYDist, fDiffX) * 10430.378f);

            if (fAbsBallX > fMinKickLine
                || nAbsAngle < 0x4E34)
            {
                bDoGrab = true;
            }
        }

        if (bDoGrab)
        {
            FakeBallWorld::GetPredictedBallPosition(
                LooseBallAnims::mpLooseBallInfo->mfPickupTime
                    * LooseBallAnims::mpLooseBallInfo->mfAnimDuration,
                mv3TargetPosition,
                mv3TargetVelocity);

            GetLocalPoint(mv3LocalContactPosition,
                mv3TargetPosition,
                m_v3Position,
                m_aActualFacingDirection);

            nlVector3 v3CurLocalPos;
            GetLocalPoint(v3CurLocalPos,
                v3BallPosition,
                m_v3Position,
                m_aActualFacingDirection);

            bool bInFront = v3CurLocalPos.x >= 0.0f;
            mpLooseBallInfo = LooseBallAnims::FindLooseBallAnim(
                mv3LocalContactPosition, bInFront, 0.0f);
        }
        else
        {
            mpLooseBallInfo
                = &LooseBallAnims::mLooseBallKickInfo[1];
        }

        float fInterceptTime;
        float fClosestDist;
        bool bFound = fn_8016EA10(m_v3Position,
            1.0f,
            6.0f,
            mv3TargetPosition,
            mv3TargetVelocity,
            fInterceptTime,
            fClosestDist,
            3.0f);

        if (bFound && mv3TargetPosition.z < 1.0f
            && fClosestDist < 0.4f)
        {
            nlVector2 v2Distance;
            v2Distance.x
                = mv3TargetPosition.x - m_v3Position.x;
            v2Distance.y
                = mv3TargetPosition.y - m_v3Position.y;
            float fPickupDist = mpLooseBallInfo->mfPickupDistance;
            float fReachDist = 0.4f + fPickupDist;
            float fDistSq = nlVec2LengthSquared(v2Distance);
            float fReachDistSq
                = nlGetLengthSquared1D(fReachDist);
            float fAnimTime = mpLooseBallInfo->mfPickupTime
                            * mpLooseBallInfo->mfAnimDuration;
            float fTargetTime = fInterceptTime - fAnimTime;

            if (fDistSq <= fReachDistSq && fTargetTime < 0.02f)
            {
                float fDist = nlSqrt(fDistSq, true);
                InitActionLooseBallPickup(fDist, false);
                return;
            }

            InitActionLooseBallPursueRolling();
            return;
        }

        if (mGoalieActionState == GOALIEACTION_MOVE)
        {
            return;
        }
        InitActionMove(true);
        return;
    }

    int nNumSolutions;
    float pSolutions[2];
    CalcInterceptXY(m_v3Position,
        0.85f * ((GoalieTweaks*)m_pTweaks)->fRunningSpeed,
        0.5f,
        v3BallPosition,
        *pBallVelocity,
        nNumSolutions,
        pSolutions);

    if (nNumSolutions != 0)
    {
        float fBestTime;
        if (nNumSolutions == 2)
        {
            fBestTime = pSolutions[0] < pSolutions[1]
                          ? pSolutions[0]
                          : pSolutions[1];
        }
        else
        {
            fBestTime = pSolutions[0];
        }

        if (fBestTime < 5.0f)
        {
            nlVector3 v3IntPos;
            nlVector3 v3IntVel;
            float fTargetHeight;
            float fHeightTime = fn_8016D52C(
                3.0f, fBestTime, v3IntPos, v3IntVel, fTargetHeight, false);

            if (fHeightTime >= 0.0f)
            {
                if (fn_8007B9A0(this, v3IntPos, 8.0f))
                {
                    if (fTargetHeight < 3.0f)
                    {
                        SetGoalieAction(
                            GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING,
                            0);
                        mv3TargetPosition = v3IntPos;
                        mfTargetTime = fHeightTime;
                        mfTargetDist = 1.4f;
                        mbIsDown = false;

                        float fDyFace
                            = v3IntPos.y - m_v3Position.y;
                        float fDxFace
                            = v3IntPos.x - m_v3Position.x;
                        m_aDesiredFacingDirection
                            = (u16)(s32)(nlATan2f(
                                             fDyFace, fDxFace)
                                         * 10430.378f);

                        s16 nAngDiff
                            = m_aDesiredFacingDirection
                            - m_aActualFacingDirection;
                        int nAnimID = ChooseRunAnim(
                            nAngDiff, v3IntPos, 1.0f);
                        PlayNewAnim(nAnimID);
                        InitMovementFromAnimSeek(
                            ((GoalieTweaks*)m_pTweaks)
                                ->fRunningDirectionSeekSpeed,
                            ((GoalieTweaks*)m_pTweaks)
                                ->fRunningDirectionSeekFalloff);
                        return;
                    }

                    mbShouldMiss = false;
                    fn_8008CED8(
                        this, fHeightTime, v3IntPos, v3IntVel);
                    return;
                }

                if (mGoalieActionState == GOALIEACTION_MOVE)
                {
                    return;
                }
                InitActionMove(true);
                return;
            }
        }
    }

    SetGoalieAction(GOALIEACTION_LOOSEBALL_SETUP, 0);
    float fDyFace = v3BallPosition.y - m_v3Position.y;
    float fDxFace = v3BallPosition.x - m_v3Position.x;
    m_aDesiredFacingDirection
        = (u16)(s32)(nlATan2f(fDyFace, fDxFace) * 10430.378f);

    s16 nAngDiff
        = m_aDesiredFacingDirection - m_aActualFacingDirection;
    int nAnimID = ChooseRunAnim(nAngDiff, v3BallPosition, 1.0f);
    PlayNewAnim(nAnimID);
    InitMovementFromAnimSeek(
        ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekSpeed,
        ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekFalloff);
}
