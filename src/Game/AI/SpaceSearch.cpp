#include <stddef.h>

#include "Game/AI/SpaceSearch.h"

#include "types.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Fuzzy.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/Game.h"
#include "Game/ScriptTuning.h"
#include "Game/CharacterTweaks.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Field.h"
#include "Game/UnidentifiedStaticStorage.h"

// Not retained in R4QE01: the derived constructors inline this and the linker
// strips the out-of-line copy.
SpaceSearch::SpaceSearch(float fNetDirection)
{
    m_fNetDirection = fNetDirection;
    m_bDebugOn = false;
    m_bDrawSearchSpace = false;
    m_fMaxRadius = 4.0f;
}

static inline float GetCircumference(float fRadius)
{
    return 6.2831855f * fRadius;
}

/**
 * Offset/Address/Size: 0x0 | 0x800A3FDC | size: 0x40
 */
SpaceSearch::~SpaceSearch()
{
}

/**
 * Offset/Address/Size: 0x40 | 0x800A401C | size: 0x5F4
 */
float SpaceSearch::FindBestPosition(
    nlVector3& v3Dest,
    const nlVector3& v3CenterPos,
    eFieldDirection eSearchDir,
    const nlVector3* pv3TargetOrDirection,
    float fMaxRadius,
    unsigned short aSearchCone)
{
    int numAngleSteps;
    int numRadiusSteps;
    unsigned short aDirection;
    nlVector3 v3TowardTarget;
    nlVector3 v3AwayFromTarget;
    float fCustomX;
    float fCustomY;
    nlVector3 v3BestOpenPosition;
    float fOriginalPositionScore;
    float fBestPositionScore;
    int aFromAngle;
    int aToAngle;
    long aDelta;
    float fRadiusDelta;
    nlVector3 v3LastPos;
    int numIterations;
    int i_radius;
    unsigned short aAngleDelta;
    int i_angle;
    nlVector3 v3TestPosition;
    float fScore;
    nlColour colour;

    aDirection = 0;

    switch (eSearchDir)
    {
    case DIR_NONE:
        aSearchCone = 0xFFFF;
        break;
    case DIR_UPFIELD:
        aDirection = (m_fNetDirection > 0.0f) ? 0x8000 : 0;
        break;
    case DIR_DOWNFIELD:
        aDirection = (m_fNetDirection > 0.0f) ? 0 : 0x8000;
        break;
    case DIR_TOWARD_TARGET:
    {
        nlVec3Sub(v3TowardTarget, *pv3TargetOrDirection, v3CenterPos);
        aDirection = (unsigned short)(s32)(10430.378f
                                           * nlATan2f(v3TowardTarget.y, v3TowardTarget.x));
        break;
    }
    case DIR_AWAYFROM_TARGET:
    {
        nlVec3Sub(v3AwayFromTarget, v3CenterPos, *pv3TargetOrDirection);
        aDirection = (unsigned short)(s32)(10430.378f
                                           * nlATan2f(v3AwayFromTarget.y, v3AwayFromTarget.x));
        break;
    }
    case DIR_CUSTOM:
    {
        fCustomX = pv3TargetOrDirection->x;
        fCustomY = pv3TargetOrDirection->y;
        aDirection
            = (unsigned short)(s32)(10430.378f * nlATan2f(fCustomY, fCustomX));
        break;
    }
    }

    if (fMaxRadius <= 0.0f)
    {
        fMaxRadius = 4.0f;
    }

    m_fMaxRadius = fMaxRadius;

    v3BestOpenPosition = v3CenterPos;
    fOriginalPositionScore
        = EvaluatePosition(v3BestOpenPosition, v3CenterPos, eSearchDir, aDirection);
    fBestPositionScore = fOriginalPositionScore;

    nlPolar pLocation = { 0, 0.0f };
    float fMinRadius = 0.0f;

    aFromAngle = (int)((float)aDirection - 0.5f * (float)aSearchCone);
    aToAngle = (int)((float)aDirection + 0.5f * (float)aSearchCone);
    aDelta = (short)(aToAngle - aFromAngle);

    numRadiusSteps = nlMin(5, (int)(0.5f + (fMaxRadius - fMinRadius) / 1.5f));

    fRadiusDelta = (fMaxRadius - fMinRadius) / (float)numRadiusSteps;

    v3LastPos = v3CenterPos;
    numIterations = 0;

    static int maxIterations = 0;

    for (i_radius = 0; i_radius < numRadiusSteps; i_radius++)
    {
        pLocation.a = aFromAngle;
        pLocation.r += fRadiusDelta;
        if (i_radius == numRadiusSteps - 1)
        {
            pLocation.r = fMaxRadius;
        }

        aAngleDelta = (unsigned short)(int)(65536.0f * (1.5f / GetCircumference(pLocation.r)));
        numAngleSteps = nlMin(10,
            (int)(0.5f
                  + (float)(unsigned short)aDelta / (float)aAngleDelta));

        aAngleDelta = (unsigned short)((unsigned short)aDelta / numAngleSteps);

        for (i_angle = 0; i_angle < numAngleSteps; i_angle++)
        {
            numIterations++;
            pLocation.a += aAngleDelta;
            if (i_angle == numAngleSteps - 1)
            {
                pLocation.a = aToAngle;
            }

            v3TestPosition.z = 0.0f;
            nlPolarToCartesian(v3TestPosition, pLocation);
            nlVec3Add(v3TestPosition, v3TestPosition, v3CenterPos);
            v3TestPosition.z = 0.0f;

            cField::FixOutOfBoundsPosition(v3TestPosition, 0.2f, true);

            fScore = EvaluatePosition(
                v3TestPosition, v3CenterPos, eSearchDir, aDirection);

            if (m_bDrawSearchSpace)
            {
                colour.c[0] = 0x99;
                colour.c[1] = 0;
                colour.c[2] = 0x99;
                colour.c[3] = 0xFF;
                g_ShapeRenderer.DrawLine3D(
                    v3LastPos, v3TestPosition, colour, false);
                v3LastPos = v3TestPosition;
            }

            if (fScore > fBestPositionScore)
            {
                fBestPositionScore = fScore;
                v3BestOpenPosition = v3TestPosition;
                if (fScore > 1.0f && !m_bDrawSearchSpace)
                {
                    break;
                }
            }
        }

        if (fBestPositionScore > 1.0f && !m_bDebugOn)
        {
            break;
        }
    }

    if (numIterations > maxIterations)
    {
        maxIterations = numIterations;
    }

    v3Dest = v3BestOpenPosition;
    return fBestPositionScore;
}

/**
 * Offset/Address/Size: 0x634 | 0x800A4610 | size: 0x100
 */
SSearchOpenLane::SSearchOpenLane(cPlayer* pPlayer1, cPlayer* pPlayer2)
    : SpaceSearch(pPlayer1 != NULL ? pPlayer1->m_pTeam->m_pNet->m_fDirection
                                   : pPlayer2->m_pTeam->m_pNet->m_fDirection)
{
    if (pPlayer2 != NULL)
    {
        InitializeForPass(pPlayer1, pPlayer2);
    }
    else
    {
        InitializeForShot(pPlayer1);
    }
}

void SSearchOpenLane::InitializeForPass(
    cPlayer* pBallOwner, cPlayer* pPassTarget)
{
    m_pBallOwner = pBallOwner;
    m_pPassTarget = pPassTarget;
    m_bOtherPosIsTarget = false;

    if (pBallOwner != NULL)
    {
        m_v3OtherPos = pBallOwner->m_DetChar.m_v3Position;
    }
    else
    {
        m_v3OtherPos = g_pBall->m_v3Position;
    }
}

void SSearchOpenLane::InitializeForShot(cPlayer* pBallOwner)
{
    m_pBallOwner = pBallOwner;
    m_pPassTarget = NULL;
    m_v3OtherPos = pBallOwner->GetAIOffNetLocation(NULL);
    m_bOtherPosIsTarget = true;
}

/**
 * Offset/Address/Size: 0x734 | 0x800A4710 | size: 0x1CC
 */
float SSearchOpenLane::EvaluatePosition(const nlVector3& position,
    const nlVector3& v3CenterPos, eFieldDirection eSearchDir,
    unsigned short aDirection)
{
    float fWeightedSum = 0.0f;
    float fTotalWeight = 0.0f;

    float fOpenToPosition;
    if (m_bOtherPosIsTarget)
    {
        fOpenToPosition = LaneOpenness(position, m_v3OtherPos, m_pBallOwner, m_pPassTarget, 0.5f, 1.0f, 1.0f, 0.0f);
    }
    else
    {
        fOpenToPosition = LaneOpenness(m_v3OtherPos, position, m_pBallOwner, m_pPassTarget, 0.5f, 1.0f, 1.0f, 0.0f);
    }

    fWeightedSum += NormalizeVal(fOpenToPosition, 0.0f, 0.8f);
    fTotalWeight += 1.0f;

    if (m_pPassTarget != NULL)
    {
        fWeightedSum += WidePositionOpenness(position,
            m_pPassTarget->m_pTeam->GetOtherTeam(),
            m_pPassTarget,
            false,
            0.0f);
        fTotalWeight += 1.0f;

        const nlVector3& v3Ball = g_pBall->UnidentifiedHasPassTarget()
                                    ? g_pBall->m_v3PassIntercept
                                    : g_pBall->m_v3Position;
        nlVector2 v2Delta = { position.x - v3Ball.x, position.y - v3Ball.y };
        float fNearToBall = NormalizeVal(
            nlSqrt(nlVec2LengthSquared(v2Delta), true), 10.0f, 4.0f);
        fWeightedSum += 1.5f * (1.0f - fNearToBall);
        fTotalWeight += 1.5f;
    }

    float fNearToSideline = NearToSideline(position);
    fWeightedSum += 0.5f * (1.0f - fNearToSideline);
    fTotalWeight += 0.5f;

    if (fTotalWeight > 0.0f)
    {
        return fWeightedSum / fTotalWeight;
    }
    return 0.0f;
}

/**
 * Offset/Address/Size: 0x900 | 0x800A48DC | size: 0x38
 */
SSearchGetOpen::SSearchGetOpen(cPlayer* pPlayer)
    : SpaceSearch(pPlayer->m_pTeam->m_pNet->m_fDirection)
{
    m_pPlayer = pPlayer;
}

/**
 * Offset/Address/Size: 0x938 | 0x800A4914 | size: 0xB8
 */
float SSearchGetOpen::EvaluatePosition(const nlVector3& v3TestPosition,
    const nlVector3& v3CenterPos, eFieldDirection eSearchDir,
    unsigned short aDirection)
{
    float fWeightedSum = 0.0f;
    float fTotalWeight = 0.0f;

    fWeightedSum += PositionOpenness(v3TestPosition,
        m_pPlayer->m_pTeam->GetOtherTeam(),
        m_pPlayer,
        NULL,
        false,
        0.0f);
    fTotalWeight += 1.0f;

    float fNearToSideline = NearToSideline(v3TestPosition);
    fWeightedSum += 0.5f * (1.0f - fNearToSideline);
    fTotalWeight += 0.5f;

    if (fTotalWeight > 0.0f)
    {
        return fWeightedSum / fTotalWeight;
    }
    return 0.0f;
}

static inline void GetDelta2D(
    nlVector2& vDelta, const nlVector2& v2Position, const nlVector3& v3Position)
{
    nlVec2Set(vDelta, v2Position.x - v3Position.x, v2Position.y - v3Position.y);
}

static inline float GetDistanceSquared2D(
    const nlVector2& v2Position, const nlVector3& v3Position)
{
    nlVector2 vDelta;
    GetDelta2D(vDelta, v2Position, v3Position);
    return nlVec2DotProduct(vDelta, vDelta);
}

static inline void CalcIdealShootingPosition(nlVector2& vIdealPosition,
    const nlVector3& v3Position, const nlVector3& v3OffNetPosition, float fIdealDistance)
{
    nlVector2 vDelta;
    nlVec2Set(vDelta, v3OffNetPosition.x - v3Position.x, v3OffNetPosition.y - v3Position.y);
    float fDistSq = nlVec2DotProduct(vDelta, vDelta);
    if (fDistSq <= fIdealDistance * fIdealDistance)
    {
        float fInvDist = nlRecipSqrt(fDistSq, true);
        nlVec2Set(vDelta, fInvDist * vDelta.x, fInvDist * vDelta.y);
        nlVec2Set(vIdealPosition, -fIdealDistance * vDelta.x + v3OffNetPosition.x, -fIdealDistance * vDelta.y + v3OffNetPosition.y);
    }
    else
    {
        float fDist = nlSqrt(fDistSq, true);
        float fScale = (fDist - fIdealDistance) / fDist;
        nlVec2Set(vIdealPosition, fScale * vDelta.x + v3Position.x, fScale * vDelta.y + v3Position.y);
    }
}

/**
 * Offset/Address/Size: 0x9F0 | 0x800A49CC | size: 0x3A8
 */
static float CalcIdealShootingPositionScore(const nlVector3& v3TestPosition,
    const nlVector3& v3OtherPosition, const nlVector3& v3OffNetPosition,
    float fMaxDistance, float fShooting)
{
    nlVector2 vCandidate;
    nlVector2 vOtherCandidate;
    nlVector2 vMove;
    float fScore = 0.0f;

    float fIdealMin = InterpolateClamped(
        g_pGame->m_pFuzzyTweaks->fBadShooterDistanceMin,
        g_pGame->m_pFuzzyTweaks->fGoodShooterDistanceMin,
        fShooting);
    float fIdealMax = InterpolateClamped(
        g_pGame->m_pFuzzyTweaks->fBadShooterDistanceMax,
        g_pGame->m_pFuzzyTweaks->fGoodShooterDistanceMax,
        fShooting);
    float fRange = 0.4f * (fIdealMax - fIdealMin);

    CalcIdealShootingPosition(vCandidate, v3TestPosition, v3OffNetPosition, fIdealMax);

    if (GetDistanceSquared2D(vCandidate, v3TestPosition) < fRange * fRange)
    {
        float fDist = nlSqrt(
            nlVec3DistanceSquared2D(v3TestPosition, v3OffNetPosition), true);
        if (fDist < fIdealMax)
        {
            fScore = NormalizeVal(fDist, fIdealMax - fRange, fIdealMax);
        }
        else
        {
            fScore = NormalizeVal(fDist, fIdealMax + fRange, fIdealMax);
        }
    }
    else
    {
        CalcIdealShootingPosition(vOtherCandidate, v3OtherPosition, v3OffNetPosition, fIdealMax);

        if (nlVec2DotProduct(vOtherCandidate, vOtherCandidate) > 0.1f)
        {
            nlVec2Set(vMove, v3TestPosition.x - v3OtherPosition.x, v3TestPosition.y - v3OtherPosition.y);
            if (nlVec2DotProduct(vMove, vMove) > 0.1f)
            {
                fScore = NormalizeVal(nlSqrt(nlVec2DotProduct(vMove, vMove), true),
                    0.0f,
                    fMaxDistance);
                float fInvMoveLength = nlRecipSqrt(nlVec2DotProduct(vMove, vMove), true);
                nlVec2Set(vMove, fInvMoveLength * vMove.x, fInvMoveLength * vMove.y);
                float fInvCandidateLength = nlRecipSqrt(
                    nlVec2DotProduct(vOtherCandidate, vOtherCandidate), true);
                nlVec2Set(vOtherCandidate, fInvCandidateLength * vOtherCandidate.x, fInvCandidateLength * vOtherCandidate.y);
                fScore *= NormalizeVal(
                    nlVec2DotProduct(vMove, vOtherCandidate), 0.0f, 0.5f);
            }
        }
    }

    return fScore;
}

// Not retained in R4QE01: SSearchRunToNet's constructor inlines this and the
// linker strips the out-of-line copy.
SSearchIdealShot::SSearchIdealShot(cPlayer* pBallOwner)
    : SpaceSearch(pBallOwner->m_pTeam->m_pNet->m_fDirection)
    , m_SSearchOpenLane(pBallOwner, NULL)
{
    m_pGoalie = pBallOwner->m_pTeam->GetOtherTeam()->GetGoalie();
}

/**
 * Offset/Address/Size: 0xDD8 | 0x800A4DB4 | size: 0x1A0
 */
float SSearchIdealShot::EvaluatePosition(const nlVector3& position,
    const nlVector3& v3CenterPos, eFieldDirection eSearchDir,
    unsigned short aDirection)
{
    float fShooting;
    float fWeightedSum = 0.0f;
    float fTotalWeight = 0.0f;

    float fOpenToPosition = LaneOpenness(position,
        m_SSearchOpenLane.m_pBallOwner->GetAIOffNetLocation(NULL),
        m_SSearchOpenLane.m_pBallOwner,
        NULL,
        0.0f,
        0.25f,
        1.0f,
        0.0f);
    fWeightedSum += 0.5f * fOpenToPosition;
    fTotalWeight += 0.5f;

    fWeightedSum += PositionOpenness(position,
        m_SSearchOpenLane.m_pBallOwner->m_pTeam->GetOtherTeam(),
        m_SSearchOpenLane.m_pBallOwner,
        NULL,
        false,
        0.0f);
    fTotalWeight += 1.0f;

    fWeightedSum += 0.5f
                  * PositionIsInFrontOfNet(position, m_pGoalie->m_pTeam->m_pNet);
    fTotalWeight += 0.5f;

    float fNearToGoalie = NearToGoaliePosition(
        &position, &m_pGoalie->m_DetChar.m_v3Position);
    fWeightedSum += 1.0f - fNearToGoalie;
    fTotalWeight += 1.0f;

    if (m_SSearchOpenLane.m_pBallOwner->m_eClassType == FIELDER)
    {
        fShooting = ((cFielder*)m_SSearchOpenLane.m_pBallOwner)->GetTweaks()->fShooting;
    }
    else
    {
        fShooting = 0.5f;
    }

    fWeightedSum += CalcIdealShootingPositionScore(position, v3CenterPos, m_SSearchOpenLane.m_pBallOwner->GetAIOffNetLocation(NULL), m_fMaxRadius, fShooting);
    fTotalWeight += 1.0f;

    if (fTotalWeight > 0.0f)
    {
        return fWeightedSum / fTotalWeight;
    }
    return 0.0f;
}

/**
 * Offset/Address/Size: 0xF78 | 0x800A4F54 | size: 0xF4
 */
SSearchBestPass::SSearchBestPass(cPlayer* pBallOwner, cPlayer* pPassTarget,
    bool bAllowLeadPass, bool bIsPerfectPass, float fPassSpeed)
    : SpaceSearch(pBallOwner != NULL
                      ? pBallOwner->m_pTeam->m_pNet->m_fDirection
                      : pPassTarget->m_pTeam->m_pNet->m_fDirection)
{
    m_fPassSpeed = fPassSpeed;
    m_bAllowLeadPass = bAllowLeadPass;
    m_bIsPerfectPass = bIsPerfectPass;
    m_pBallOwner = pBallOwner;
    m_pPassTarget = pPassTarget;

    if (nlGetLengthSquared2D(pPassTarget->GetVelocity().x,
            pPassTarget->GetVelocity().y)
        >= 1.0f)
    {
        nlVec3Normalize(m_v3PassDirection, pPassTarget->GetVelocity());
    }
}

static float sfBestPassIdealShotWeight = 0.9f;
static float sfBestPassTowardGoalWeight = 1.0f;
static float sfBestPassOpennessWeight = 1.2f;
static float sfBestPassLaneWeight = 1.2f;
static float sfBestPassNearTeammateWeight = 0.5f;
static float sfBestPassFarTeammateWeight = 0.5f;
static float sfBestPassRunDirectionWeight = 0.4f;
static float sfBestPassFormationWeight = 0.4f;

/**
 * Offset/Address/Size: 0x106C | 0x800A5048 | size: 0x658
 */
float SSearchBestPass::EvaluatePosition(const nlVector3& position,
    const nlVector3& v3OtherPosition, eFieldDirection eSearchDir,
    unsigned short aDirection)
{
    nlVector3 v3ToPosition;
    nlVec3Set(v3ToPosition,
        position.x - m_pPassTarget->m_DetChar.m_v3Position.x,
        position.y - m_pPassTarget->m_DetChar.m_v3Position.y,
        0.0f);
    float fDistance = nlGetLength2D(v3ToPosition.x, v3ToPosition.y);
    if (fDistance > 0.0f)
    {
        nlVec3Normalize(v3ToPosition, v3ToPosition);
    }

    float fPassTime = 0.2f;
    cFielder* pPassTarget = (cFielder*)m_pPassTarget;
    if (pPassTarget->m_eClassType == FIELDER)
    {
        float fMaxSpeed = pPassTarget->GetRunningSpeed();
        if (pPassTarget->m_DetChar.m_fActualSpeed >= 1.0f)
        {
            float fTurn = InterpolateRangeClamped(0.0f, 1.0f, -0.95f, -0.5f, nlVec3DotProduct(v3ToPosition, m_v3PassDirection));
            fMaxSpeed = Interpolate(GetJogSpeed(pPassTarget->GetTweaks()),
                0.8f * pPassTarget->GetRunningSpeed(),
                fTurn);
        }

        nlVector2 v2BallToPosition = {
            g_pBall->m_v3Position.x - position.x,
            g_pBall->m_v3Position.y - position.y,
        };
        fPassTime = nlSqrt(nlVec2LengthSquared(v2BallToPosition), true) / m_fPassSpeed;
        if (fDistance / fPassTime > fMaxSpeed)
        {
            return 0.0f;
        }
    }

    float fWeightedSum = 0.0f;
    float fTotalWeight = 0.0f;

    float fAwayFromSideline = 1.0f - NearToSideline(position);
    fWeightedSum += 0.5f * fAwayFromSideline;
    fTotalWeight += 0.5f;
    if (fAwayFromSideline < 0.35f)
    {
        return 0.0f;
    }

    if (NearToGoaliePosition(&position,
            &m_pPassTarget->m_pTeam->GetOtherTeam()->GetGoalie()->m_DetChar.m_v3Position)
        > 0.5f)
    {
        return 0.0f;
    }

    float fInOffensiveZone = InOffensiveZone(m_pPassTarget->m_DetChar.m_v3Position,
        (m_fNetDirection < 0.0f) ? HOME : AWAY);
    nlVector3 v3GoalLine = { 0.0f, 0.0f, 0.0f };
    v3GoalLine.x = cField::GetGoalLineX(-m_fNetDirection);
    if (fInOffensiveZone > 0.0f)
    {
        float fShooting = 0.5f;
        if (m_pPassTarget != NULL && m_pPassTarget->m_eClassType == FIELDER)
        {
            fShooting = ((cFielder*)m_pPassTarget)->GetTweaks()->fShooting;
        }
        float fIdealShot = CalcIdealShootingPositionScore(position, v3OtherPosition, v3GoalLine, m_fMaxRadius, fShooting);
        float fZoneWeight = InterpolateRangeClamped(1.0f, 1.0f, 0.0f, 1.0f, FuzzyNot(fInOffensiveZone));
        fWeightedSum += fIdealShot * (fZoneWeight * sfBestPassIdealShotWeight);
        fTotalWeight += fZoneWeight * sfBestPassIdealShotWeight;
    }
    else
    {
        nlVector3 v3TowardGoal = { 1.0f, 0.0f, 0.0f };
        if (m_pPassTarget->m_pTeam->m_nSide == AWAY)
        {
            nlVec3Set(v3TowardGoal, -1.0f, 0.0f, 0.0f);
        }
        // As in retail, the dot product is passed as the range maximum.
        float fTowardGoal = NormalizeVal(-0.2f, 1.0f, nlVec3DotProduct(v3TowardGoal, v3ToPosition));
        fWeightedSum += fTowardGoal * sfBestPassTowardGoalWeight;
        fTotalWeight += sfBestPassTowardGoalWeight;
    }

    if (m_pPassTarget != NULL)
    {
        if (m_pPassTarget->m_eClassType == FIELDER)
        {
            float fNearFormation = NearToFormationPosition(
                (cFielder*)m_pPassTarget, (nlVector3*)&position);
            fWeightedSum += fNearFormation * sfBestPassFormationWeight;
            fTotalWeight += sfBestPassFormationWeight;
        }

        if (m_pPassTarget->m_DetChar.m_fActualSpeed >= 1.0f
            && m_pPassTarget->m_eClassType == FIELDER)
        {
            nlVector3 v3Move;
            nlVec3Sub(v3Move, position, v3OtherPosition);
            float fLengthSq = nlVec3LengthSquared(v3Move);
            if (fLengthSq > 0.2f)
            {
                nlVec3Normalize(v3Move, v3Move);
                float fDot = FMAX(0.0f, nlVec3DotProduct(v3Move, m_v3PassDirection));
                cFielder* pFielder = (cFielder*)m_pPassTarget;
                float fRunDirection = fDot
                                    * NormalizeVal(pFielder->m_DetChar.m_fActualSpeed, 0.0f, pFielder->GetRunningSpeed());
                fWeightedSum += fRunDirection * sfBestPassRunDirectionWeight;
                fTotalWeight += sfBestPassRunDirectionWeight;
            }
        }

        if (m_pBallOwner != NULL)
        {
            const nlVector3& v3OwnerPosition = m_pBallOwner->m_DetChar.m_v3Position;
            cPlayer* pReceiver = m_pPassTarget;
            float fPrediction = FMIN(0.1f, fPassTime);
            if (!m_bAllowLeadPass)
            {
                float fOpenLane = LaneOpenness(v3OwnerPosition, position, m_pBallOwner, pReceiver, 0.5f, 1.0f, 1.0f, fPrediction);
                fWeightedSum += fOpenLane * sfBestPassLaneWeight;
                fTotalWeight += sfBestPassLaneWeight;
                // As in retail, the prediction time is passed as the incapacitated
                // flag and false as the prediction time.
                float fOpenness = PositionOpenness(position,
                    m_pPassTarget->m_pTeam->GetOtherTeam(),
                    m_pPassTarget,
                    NULL,
                    fPrediction,
                    false);
                fWeightedSum += fOpenness * sfBestPassOpennessWeight;
                fTotalWeight += sfBestPassOpennessWeight;
            }
            else
            {
                float fOpenness = WidePositionOpenness(position,
                    m_pPassTarget->m_pTeam->GetOtherTeam(),
                    m_pPassTarget,
                    false,
                    fPrediction);
                fWeightedSum += fOpenness * sfBestPassOpennessWeight;
                fTotalWeight += sfBestPassOpennessWeight;
            }

            nlVector2 v2FromOwner = {
                position.x - m_pBallOwner->m_DetChar.m_v3Position.x,
                position.y - m_pBallOwner->m_DetChar.m_v3Position.y,
            };
            float fOwnerDistance = nlSqrt(nlVec2LengthSquared(v2FromOwner), true);
            if (m_bIsPerfectPass)
            {
                float fFarTeammate = NormalizeVal(fOwnerDistance,
                    g_pGame->m_pFuzzyTweaks->fFarTeammateConfidenceDistanceMin,
                    g_pGame->m_pFuzzyTweaks->fFarTeammateConfidenceDistanceMax);
                fWeightedSum += fFarTeammate * sfBestPassFarTeammateWeight;
                fTotalWeight += sfBestPassFarTeammateWeight;
            }
            else
            {
                fWeightedSum += sfBestPassNearTeammateWeight
                              * (1.0f
                                  - NormalizeVal(fOwnerDistance,
                                      g_pGame->m_pFuzzyTweaks->fNearTeammateConfidenceDistanceMin,
                                      g_pGame->m_pFuzzyTweaks->fNearTeammateConfidenceDistanceMax));
                fTotalWeight += sfBestPassNearTeammateWeight;
            }
        }
    }

    if (fTotalWeight > 0.0f)
    {
        return fWeightedSum / fTotalWeight;
    }
    return 0.0f;
}

/**
 * Offset/Address/Size: 0x16C4 | 0x800A56A0 | size: 0x12C
 */
SSearchRunToNet::SSearchRunToNet(cPlayer* pPlayer)
    : SpaceSearch(pPlayer->m_pTeam->m_pNet->m_fDirection)
    , m_SSearchIdealShot(pPlayer)
{
}

static const nlVector2 g_vRunToNetDistanceConfidence = { 50.0f, 1.5f };
static const nlVector2 g_vRunToNetFormationDistConfidence = { 10.0f, 1.5f };

/**
 * Offset/Address/Size: 0x1830 | 0x800A580C | size: 0x210
 */
float SSearchRunToNet::EvaluatePosition(const nlVector3& v3TestPosition,
    const nlVector3& v3CenterPos, eFieldDirection eSearchDir,
    unsigned short aDirection)
{
    float fTotalSum = 0.0f;
    float fTotalWeight = 0.0f;
    cFielder* pBallOwner = (cFielder*)m_SSearchIdealShot.m_SSearchOpenLane.m_pBallOwner;

    nlVector3 v3NetPosition = pBallOwner->GetAIOffNetLocation(NULL);

    nlVector2 v2OwnerToNet = {
        pBallOwner->m_DetChar.m_v3Position.x - v3NetPosition.x,
        pBallOwner->m_DetChar.m_v3Position.y - v3NetPosition.y,
    };
    if (nlVec2LengthSquared(v2OwnerToNet) < 100.0f)
    {
        fTotalSum += m_SSearchIdealShot.EvaluatePosition(
            v3TestPosition, v3CenterPos, eSearchDir, aDirection);
        fTotalWeight += 1.0f;
    }
    else
    {
        nlVector2 v2ToNet = { v3NetPosition.x - v3TestPosition.x,
            v3NetPosition.y - v3TestPosition.y };
        float fNormDist = NormalizeVal(nlSqrt(nlVec2LengthSquared(v2ToNet), true),
            g_vRunToNetDistanceConfidence);
        fTotalSum += 5.0f * fNormDist;
        fTotalWeight += 5.0f;

        float fOpenPos = PositionOpenness(v3TestPosition,
            pBallOwner->m_pTeam->GetOtherTeam(),
            pBallOwner,
            NULL,
            false,
            0.0f);
        fTotalSum += 0.5f * fOpenPos;
        fTotalWeight += 0.5f;

        LaneOpenness(v3TestPosition, v3NetPosition, pBallOwner, NULL, 0.25f, 0.5f, 1.0f, 0.0f);

        nlVector3 v3FormationPos;
        pBallOwner->CalculateFormationPosition(v3FormationPos);
        nlVector2 v2ToFormation = { v3FormationPos.x - v3TestPosition.x,
            v3FormationPos.y - v3TestPosition.y };
        float fNormFormDist = NormalizeVal(
            nlSqrt(nlVec2LengthSquared(v2ToFormation), true),
            g_vRunToNetFormationDistConfidence);
        fTotalSum += 0.4f * fNormFormDist;
        fTotalWeight += 0.4f;

        float fCloseToSideline = CloseToSideline(v3TestPosition, NULL, false, NULL);
        fTotalSum += 0.6f * (1.0f - fCloseToSideline);
        fTotalWeight += 0.6f;
    }

    if (fTotalWeight > 0.0f)
    {
        return fTotalSum / fTotalWeight;
    }
    return 0.0f;
}

/**
 * Offset/Address/Size: 0x1A40 | 0x800A5A1C | size: 0x38
 */
SSearchCutAndBreak::SSearchCutAndBreak(cPlayer* pPlayer)
    : SpaceSearch(pPlayer->m_pTeam->m_pNet->m_fDirection)
{
    m_pPlayer = pPlayer;
}

/**
 * Offset/Address/Size: 0x1A78 | 0x800A5A54 | size: 0x1C0
 */
float SSearchCutAndBreak::EvaluatePosition(const nlVector3& v3TestPosition,
    const nlVector3& v3CenterPos, eFieldDirection eSearchDir,
    unsigned short aDirection)
{
    float fShooting;
    float fNearNet = PositionDistanceConfidence(m_pPlayer->GetAIOffNetLocation(NULL),
        v3TestPosition,
        g_pGame->m_pFuzzyTweaks->fNearNetConfidenceDistanceMin,
        g_pGame->m_pFuzzyTweaks->fNearNetConfidenceDistanceMax);
    float fNearGoalie = NearToGoaliePosition(&v3TestPosition,
        &m_pPlayer->m_pTeam->GetOtherTeam()->GetGoalie()->m_DetChar.m_v3Position);
    fNearNet = FMAX(fNearNet, fNearGoalie);
    if (fNearNet > 0.5f)
    {
        return 0.0f;
    }

    float fWeightedSum = 0.0f;
    float fTotalWeight = 0.0f;

    float fInFrontOfNet = PositionIsInFrontOfNet(
        v3TestPosition, m_pPlayer->m_pTeam->GetOtherTeam()->m_pNet);
    fWeightedSum += 0.5f * FGREATER(fInFrontOfNet, 0.5f);
    fTotalWeight += 0.5f;

    fWeightedSum += 0.4f * WidePositionOpenness(v3TestPosition, m_pPlayer->m_pTeam->GetOtherTeam(), m_pPlayer, false, 0.0f);
    fTotalWeight += 0.4f;

    if (m_pPlayer->m_eClassType == FIELDER)
    {
        fShooting = ((cFielder*)m_pPlayer)->GetTweaks()->fShooting;
    }
    else
    {
        fShooting = 0.5f;
    }

    fWeightedSum += 0.3f * PositionShotDistance(v3TestPosition, m_pPlayer->GetAIOffNetLocation(NULL), fShooting);
    fTotalWeight += 0.3f;

    float fResult = 0.0f;
    if (fTotalWeight > 0.0f)
    {
        fResult = fWeightedSum / fTotalWeight;
    }
    return fResult;
}
