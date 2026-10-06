#include "NL/nlDLListContainer.inl"
#include "Game/AI/DesireSteering.h"
#include "NL/nlMath.inl"
#include "Game/AI/AvoidController.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/CharacterTriggers.inl"
#include "Game/DebugWriteCache.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include <math.h>
#include <stddef.h>

extern "C" float fn_8002CE14(PlayerTweaks*);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" bool fn_8003E8A0(const cFielder*);
extern "C" bool fn_8003E948(const cFielder*);
extern "C" void fn_80060608(cGame*, cFielder*);

bool gForceSidelineAvoidance = false;

static nlVector2 sSteeringSpeedScalePoints[] = {
    { 0.0f, 0.0f },
    { 0.1f, 0.2f },
    { 0.3f, 0.4f },
    { 0.5f, 0.6f },
    { 1.0f, 0.8f },
    { 1.5f, 1.0f },
};

static nlPiecewiseLinearCurve sSteeringSpeedScale(
    sSteeringSpeedScalePoints, 6);
static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static unsigned short sDesireSteeringType = 0xFFFF;
static bool sUseAvoidance = true;
static float sMinimumDesiredSpeed = 0.1f;
float gSteeringHistoryDuration = 0.3f;

static inline bool IsNotNearlyZero(float value, float zero)
{
    bool bNearlyZero = (float)fabs(value - zero) <= 0.0001f;
    return !bNearlyZero;
}

DesireSteering::DesireSteering()
    : Desire(34, UnsetTransitionFunc(g_UnsetTransitionFunc)),
      m_AvoidanceHistory(gSteeringHistoryDuration)
{
    m_pAvoidance = NULL;
}

DesireSteering::~DesireSteering()
{
    delete m_pAvoidance;
}

bool DesireSteering::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    if (m_pAvoidance == NULL)
    {
        m_pAvoidance = new (8, false) AvoidController(m_pFielder);
    }

    m_v3DesiredPos = v3Zero;
    m_v3LastDesiredPos = v3Zero;
    m_v3DesiredVel = v3Zero;
    m_AvoidanceHistory.UnidentifiedReset();
    m_v3LastDesiredPos = v3Zero;
    m_v3DesiredPos = v3Zero;
    m_v3DesiredVel = v3Zero;
    m_fDesiredFacingDirection = -1.0f;
    m_fFacingTotalWeight = 0.0f;
    m_v3TempDesiredPos = v3Zero;
    m_fTotalWeight = 0.0f;
    m_fUrgency = 0.0f;
    m_fDesiredArrivalTime = -1.0f;
    m_ePositionSeekState = PSS_ARRIVED;
    m_fForcedArrivalRadius = 0.0f;
    fn_8000F178(m_pAvoidance);
    fn_8000F178(m_pAvoidance);
    m_fAvoidanceMult = 1.0f;
    m_ThingsToAvoid = AVOID_EVERYTHING;
    mMaxDuration = -1.0f;
    return result;
}

void DesireSteering::Cleanup()
{
    fn_8000F178(m_pAvoidance);
    m_AvoidanceHistory.UnidentifiedReset();
}

void ResetSteeringHistory(DesireSteering* desire)
{
    desire->m_AvoidanceHistory.UnidentifiedReset();
}

void ResetSteeringAvoidance(DesireSteering* desire)
{
    fn_8000F178(desire->m_pAvoidance);
}

void ResetSteeringTargets(DesireSteering* desire)
{
    desire->m_v3LastDesiredPos = desire->m_v3DesiredPos;
    desire->m_v3DesiredPos = v3Zero;
    desire->m_v3DesiredVel = v3Zero;
    desire->m_fDesiredFacingDirection = -1.0f;
    desire->m_fFacingTotalWeight = 0.0f;
    desire->m_v3TempDesiredPos = v3Zero;
    desire->m_fTotalWeight = 0.0f;
    desire->m_fUrgency = 0.0f;
    desire->m_fDesiredArrivalTime = -1.0f;
    desire->m_ePositionSeekState = PSS_ARRIVED;
    desire->m_fForcedArrivalRadius = 0.0f;
}

void DesireSteering::Update(
    DesireUpdate*, float fDeltaT)
{
    fn_8003E948(m_pFielder);
    bool bUseAvoidance = !lbl_806E0C50
                      && (!lbl_806E0C51
                          || m_pFielder->m_pTeam->m_nSide != HOME)
                      && (!lbl_806E0C52
                          || m_pFielder->m_pTeam->m_nSide != AWAY);

    if (m_fTotalWeight < 0.0f)
    {
        if (m_fDesiredArrivalTime > 0.0f)
        {
            SeekTimedSteeringTarget(this, m_v3DesiredPos,
                fDeltaT, m_fDesiredArrivalTime);
            m_fDesiredArrivalTime -= fDeltaT;
            ResetSteeringTargets(this);
            bUseAvoidance = false;
        }
        else
        {
            m_fTotalWeight = 1.0f;
        }
    }

    if (m_fUrgency)
    {
        nlVector3 v3UnfilteredDesired = m_v3LastDesiredPos;
        if (m_fTotalWeight > 0.0f)
        {
            m_fUrgency /= m_fTotalWeight;
            nlVec3Scale(
                v3UnfilteredDesired, m_v3DesiredPos, 1.0f / m_fTotalWeight);
            m_fTotalWeight = 0.0f;
        }

        m_AvoidanceHistory.Update(
            m_v3DesiredPos, v3UnfilteredDesired, fDeltaT);

        SeekSteeringTarget(this, m_v3DesiredPos,
            TR_FAR_DISTANCE, fDeltaT, m_fUrgency);
        m_v3LastDesiredPos = v3UnfilteredDesired;
    }
    else
    {
        ResetSteeringHistory(this);
    }

    nlPolar desiredVelocity;
    desiredVelocity.a = m_pFielder->mUnidentified024.m_aDesiredMovementDirection;
    desiredVelocity.r = m_pFielder->mUnidentified024.m_fDesiredSpeed;
    nlPolarToCartesian(m_v3DesiredVel, desiredVelocity);
    m_v3DesiredVel.z = 0.0f;

    if (m_pFielder->m_eActionState == ACTION_WAIT
        || m_pFielder->m_eActionState == ACTION_NEED_ACTION)
    {
        m_pFielder->StartRunning();
    }
    if (sUseAvoidance && bUseAvoidance)
    {
        fn_800C5DBC(this, fDeltaT);
    }
    fn_800C6FDC(this, fDeltaT);
    fn_8003C7B0(m_pFielder);

    desiredVelocity.a = m_pFielder->mUnidentified024.m_aDesiredMovementDirection;
    desiredVelocity.r = m_pFielder->mUnidentified024.m_fDesiredSpeed;
    nlPolarToCartesian(m_v3DesiredVel, desiredVelocity);
    m_v3DesiredVel.z = 0.0f;
}

extern "C" void fn_800C5DBC(DesireSteering* desire, float fDeltaT)
{
    bool bHasGlobalPad;
    float fRepulsionMult = desire->m_fAvoidanceMult;
    int nThingsToAvoid = desire->m_ThingsToAvoid;

    if (ReceivingPass(desire->m_pFielder))
    {
        fRepulsionMult = 0.0f;
    }
    else
    {
        bHasGlobalPad = desire->m_pFielder->GetGlobalPad() != NULL;
        if (bHasGlobalPad)
        {
            if ((fn_8003E8A0(desire->m_pFielder)
                    && desire->m_pFielder->mUnidentified3DC)
                || (desire->m_pFielder->fn_8003E9F0()
                    && desire->m_pFielder->mUnidentified3DC))
            {
                nThingsToAvoid = AVOID_NOTHING;
            }
            else if (GameInfoManager::Instance()->GetStadium() == 0x0B
                || GameInfoManager::Instance()->IsRule0x4Equal3())
            {
                if (desire->m_pFielder->fn_8001E160()
                    && !gForceSidelineAvoidance)
                {
                    nThingsToAvoid = AVOID_NOTHING;
                }
                else
                {
                    nThingsToAvoid = AVOID_SIDELINES;
                }
            }
            else
            {
                fRepulsionMult = 0.0f;
                nThingsToAvoid = AVOID_SIDELINES;
            }
        }
        else if (fn_800DED80(desire->m_pFielder))
        {
            fRepulsionMult = 1.0f;
        }
    }

    bHasGlobalPad = desire->m_pFielder->GetGlobalPad() != NULL;
    if (!bHasGlobalPad && desire->m_pFielder->fn_8003EA6C())
    {
        nThingsToAvoid &= ~(AVOID_FIELDERS | AVOID_POWERUPS);
    }

    if (nThingsToAvoid & AVOID_SIDELINES)
    {
        float fInterceptScore = fn_800DED80(desire->m_pFielder);
        bool bHasBall = desire->m_pFielder->HasBall();
        float fCloseToTheirNet
            = CloseToTheirNet(desire->m_pFielder);
        float fCloseToMyNet
            = CloseToMyNet(desire->m_pFielder);
        bool bBetweenPosts
            = (float)fabs(desire->m_pFielder
                              ->mUnidentified024.m_v3Position.y)
            <= 0.5f * cNet::GetNetWidth() + cNet::GetPostRadius();
        bHasGlobalPad = desire->m_pFielder->GetGlobalPad() != NULL;

        if (bHasGlobalPad)
        {
            if ((!bHasBall && fInterceptScore >= 0.75f)
                || (fCloseToTheirNet > 0.5f && bBetweenPosts)
                || (fCloseToMyNet > 0.5f && bBetweenPosts))
            {
                nThingsToAvoid &= ~AVOID_SIDELINES;
            }
        }
        else if ((!bHasBall && fInterceptScore >= 0.75f)
            || (bHasBall && fCloseToTheirNet > 0.5f && bBetweenPosts))
        {
            nThingsToAvoid &= ~AVOID_SIDELINES;
        }
    }

    if (fn_8003E948(desire->m_pFielder)
        && desire->m_pFielder->mUnidentified3DC)
    {
        nThingsToAvoid = AVOID_NOTHING;
    }

    desire->m_pAvoidance->SetThingsToAvoid(nThingsToAvoid);
    desire->m_pAvoidance->m_fRepulsionMult = fRepulsionMult;
    desire->m_pAvoidance->Update(fDeltaT);
    desire->m_ThingsToAvoid = AVOID_EVERYTHING;
    desire->m_fAvoidanceMult = 1.0f;
}

void AddSteeringTarget(DesireSteering* desire,
    const nlVector3& v3Position, float fUrgency, float fWeight)
{
    fn_8003E948(desire->m_pFielder);
    if (desire->m_fTotalWeight == 0.0f)
    {
        desire->m_fUrgency = 0.0f;
        desire->m_v3DesiredPos = v3Zero;
    }

    if (desire->m_fTotalWeight >= 0.0f)
    {
        desire->m_fTotalWeight += fWeight;
        desire->m_fUrgency += fUrgency * fWeight;
        nlVec3ScaleAdd(desire->m_v3DesiredPos, fWeight,
            v3Position, desire->m_v3DesiredPos);
        desire->m_v3DesiredPos.z = 0.0f;
    }
}

void SetTimedSteeringTarget(DesireSteering* desire,
    const nlVector3& v3Position, unsigned short aFacingDirection,
    float fArrivalTime, float fForcedArrivalRadius)
{
    desire->m_fTotalWeight = -1.0f;
    desire->m_fUrgency = 1.0f;
    desire->m_v3DesiredPos = v3Position;
    desire->m_fDesiredArrivalTime = fArrivalTime;
    desire->m_fForcedArrivalRadius = fForcedArrivalRadius;
    desire->m_fDesiredFacingDirection = (float)aFacingDirection;
}

const nlVector3* GetSteeringTargetPosition(DesireSteering* desire)
{
    DesireReceivePass* receivePass = (DesireReceivePass*)fn_8002E08C(
        desire->m_pFielder, 22);

    if (g_pBall->UnidentifiedHasPassTarget()
        && g_pBall->m_pPassTarget == desire->m_pFielder
        && receivePass != NULL && receivePass->IsActive())
    {
        desire->m_v3TempDesiredPos = receivePass->GetAnimStartPosition();
        desire->m_v3TempDesiredPos.z = 0.0f;
        return &desire->m_v3TempDesiredPos;
    }

    bool bHasGlobalPad
        = desire->m_pFielder->GetGlobalPad() != NULL;
    if (bHasGlobalPad)
    {
        nlVec3ScaleAdd(desire->m_v3TempDesiredPos, 0.3333f,
            desire->m_v3DesiredVel,
            desire->m_pFielder->mUnidentified024.m_v3Position);
        return &desire->m_v3TempDesiredPos;
    }

    if (desire->m_fUrgency)
    {
        if (desire->m_fTotalWeight > 0.0f)
        {
            float scale = 1.0f / desire->m_fTotalWeight;
            nlVec3Scale(desire->m_v3TempDesiredPos,
                desire->m_v3DesiredPos, scale);
        }
        else
        {
            desire->m_v3TempDesiredPos = desire->m_v3DesiredPos;
        }
        desire->m_v3TempDesiredPos.z = 0.0f;
        return &desire->m_v3TempDesiredPos;
    }

    return &desire->m_v3LastDesiredPos;
}

void SeekTimedSteeringTarget(DesireSteering* desire,
    const nlVector3& v3Position, float fDeltaT, float fDesiredArrivalTime)
{
    nlVector3 v3FixedPos = v3Position;
    float fPlayerScale
        = desire->m_pFielder->mUnidentified024.m_fPlayerScale;
    float fRadius = fn_8002BFA8(desire->m_pFielder->GetTweaks(),
        fPlayerScale);
    cField::FixOutOfBoundsPosition(v3FixedPos, fRadius, false);

    nlVector3 v3DeltaFromDesired;
    nlVec3Sub(v3DeltaFromDesired, v3FixedPos, desire->m_v3LastDesiredPos);
    float fDistSq = nlVec3DistanceSquared2D(v3FixedPos,
        desire->m_pFielder->mUnidentified024.m_v3Position);

    if (fDistSq < desire->m_fForcedArrivalRadius
            * desire->m_fForcedArrivalRadius)
    {
        desire->m_ePositionSeekState = PSS_ARRIVED;
        if (desire->m_fDesiredFacingDirection >= 0.0f)
        {
            desire->m_pFielder->Unknown8(
                (unsigned short)(desire->m_fDesiredFacingDirection
                    + 0.5f),
                false);
        }
        desire->m_pFielder->mUnidentified024.m_fDesiredSpeed = 0.0f;
        ResetSteeringTargets(desire);
        return;
    }

    desire->m_ePositionSeekState = PSS_TIMED_SEEKING;
    float fDesiredSpeed
        = nlSqrt(fDistSq, true) - desire->m_fForcedArrivalRadius;
    float fMinimumSpeed = 0.0f;
    fDesiredSpeed = nlMinEquals(
        nlMaxEquals(
            (fDesiredSpeed /= fDesiredArrivalTime), fMinimumSpeed),
        fn_8002E1B0(desire->m_pFielder));

    if (fDesiredSpeed < sMinimumDesiredSpeed)
    {
        fDesiredSpeed = 0.0f;
        if (desire->m_fDesiredFacingDirection >= 0.0f)
        {
            desire->m_pFielder->Unknown8(
                (unsigned short)(desire->m_fDesiredFacingDirection
                    + 0.5f),
                false);
        }
    }
    else
    {
        nlVector3 v3Direction;
        nlVec3Sub(v3Direction, v3FixedPos,
            desire->m_pFielder->mUnidentified024.m_v3Position);
        unsigned short aDirection = nlVector3ToAngle(v3Direction);
        desire->m_pFielder->fn_8001DCE0(aDirection);
        desire->m_pFielder->Unknown8(
            aDirection, false);
    }

    fDesiredSpeed = nlMinEquals(
        fDesiredSpeed, fn_8002E1B0(desire->m_pFielder));
    desire->m_pFielder->mUnidentified024.m_fDesiredSpeed = fDesiredSpeed;
}

static inline float GetSteeringSpeedScale(float distance)
{
    float scale = distance;
    sSteeringSpeedScale.Evaluate(distance, scale);
    return scale;
}

void SeekSteeringTarget(DesireSteering* desire,
    const nlVector3& v3Pos, eTurboRequest turboRequest,
    float fDeltaT, float fUrgency)
{
    nlVector3 v3FixedPos = v3Pos;
    float fPlayerScale = desire->m_pFielder->mUnidentified024.m_fPlayerScale;
    float fRadius = fn_8002BFA8(
        desire->m_pFielder->GetTweaks(), fPlayerScale);
    cField::FixOutOfBoundsPosition(v3FixedPos, fRadius, false);

    nlVector3 v3DeltaFromDesired;
    nlVec3Sub(v3DeltaFromDesired, v3FixedPos,
        desire->m_v3LastDesiredPos);
    float fDistance = nlVec3Distance2D(v3FixedPos,
        desire->m_pFielder->mUnidentified024.m_v3Position);
    float fDesiredPositionRateOfChange = 0.0f;
    float fRadiusScale = fUrgency > 0.0f ? 1.0f / fUrgency : 1.0f;
    float fMinimumSpeedScale = 1.0f;

    if (IsNotNearlyZero(fDistance, 0.0f))
    {
        fDesiredPositionRateOfChange
            = nlSqrt(v3DeltaFromDesired.x * v3DeltaFromDesired.x
                    + v3DeltaFromDesired.y * v3DeltaFromDesired.y
                    + v3DeltaFromDesired.z * v3DeltaFromDesired.z,
                true)
            / fDeltaT;

        nlVector3 v3Direction;
        nlVec3Sub(v3Direction, v3FixedPos,
            desire->m_pFielder->mUnidentified024.m_v3Position);
        unsigned short aDirection = nlVector3ToAngle(v3Direction);
        desire->m_pFielder->Unknown8(aDirection, false);
        desire->m_pFielder->fn_8001DCE0(aDirection);
    }

    float fSpeedPercent = 0.0f;
    switch (desire->m_ePositionSeekState)
    {
    case PSS_ARRIVED:
    case PSS_TIMED_SEEKING:
    {
        float fOutRad = fRadiusScale * gGameTweaks.m_pGameTweaks->fArrivalOutRadius;
        fSpeedPercent = NormalizeVal(fDistance, 0.0f, fOutRad);
        float fNearSeekOut
            = fRadiusScale * gGameTweaks.m_pGameTweaks->fNearSeekOutRadius;
        if (fDistance >= fNearSeekOut)
        {
            desire->m_ePositionSeekState = PSS_FAR_SEEKING;
        }
        else if (fDistance >= fRadiusScale
                     * gGameTweaks.m_pGameTweaks->fArrivalOutRadius)
        {
            desire->m_ePositionSeekState = PSS_NEAR_SEEKING;
        }
        break;
    }
    case PSS_NEAR_SEEKING:
    {
        float fOutRad = fRadiusScale
                      * gGameTweaks.m_pGameTweaks->fNearSeekOutRadius.GetValue();
        float fInRad = fRadiusScale
                     * gGameTweaks.m_pGameTweaks->fArrivalInRadius.GetValue();
        fSpeedPercent = NormalizeVal(fDistance, fInRad, fOutRad);
        float fNearSeekOut
            = fRadiusScale * gGameTweaks.m_pGameTweaks->fNearSeekOutRadius;
        if (fDistance >= fNearSeekOut)
        {
            desire->m_ePositionSeekState = PSS_FAR_SEEKING;
        }
        else if (fDistance <= fRadiusScale
                     * gGameTweaks.m_pGameTweaks->fArrivalInRadius)
        {
            ResetSteeringTargets(desire);
            desire->m_ePositionSeekState = PSS_ARRIVED;
        }
        break;
    }
    case PSS_FAR_SEEKING:
    {
        float fOutRad = fRadiusScale
                      * gGameTweaks.m_pGameTweaks->fNearSeekOutRadius.GetValue();
        float fInRad = fRadiusScale
                     * gGameTweaks.m_pGameTweaks->fNearSeekInRadius.GetValue();
        fSpeedPercent = NormalizeVal(fDistance, fInRad, fOutRad);
        float fNearSeekIn
            = fRadiusScale * gGameTweaks.m_pGameTweaks->fNearSeekInRadius;
        if (fDistance < fNearSeekIn)
        {
            desire->m_ePositionSeekState = PSS_NEAR_SEEKING;
        }
        else if (fDistance < fRadiusScale * gGameTweaks.m_pGameTweaks->fArrivalInRadius)
        {
            ResetSteeringTargets(desire);
            desire->m_ePositionSeekState = PSS_ARRIVED;
        }
        break;
    }
    default:
        break;
    }

    float fMaxSpeed = 0.0f;
    float fMinSpeed = fMaxSpeed;
    if (desire->m_pFielder->m_pBall == NULL)
    {
        switch (desire->m_ePositionSeekState)
        {
        case PSS_ARRIVED:
            fMinSpeed = 0.0f;
            fMaxSpeed = 0.0f;
            break;
        case PSS_NEAR_SEEKING:
            fMinSpeed = fMinimumSpeedScale
                      * fn_8002CE14(desire->m_pFielder->GetTweaks());
            fMaxSpeed = GetRunSpeed(desire->m_pFielder->GetTweaks());
            fMinSpeed = nlMinEquals(fMinSpeed, fMaxSpeed);
            break;
        case PSS_FAR_SEEKING:
            fMinSpeed = GetRunSpeed(desire->m_pFielder->GetTweaks());
            fMaxSpeed = fn_8002C254(desire->m_pFielder->GetTweaks());
            break;
        default:
            break;
        }

        if (turboRequest == TR_FORCED_OFF)
        {
            float fRunningSpeed = GetRunSpeed(desire->m_pFielder->GetTweaks());
            fMaxSpeed = nlMinEquals(fMaxSpeed, fRunningSpeed);
        }
        else if (turboRequest == TR_FORCED_ON
            || (turboRequest == TR_MOVING_TARGET
                && IsNotNearlyZero(fDesiredPositionRateOfChange, 0.0f)))
        {
            fMinSpeed = fn_8002C254(desire->m_pFielder->GetTweaks());
            fMaxSpeed = fMinSpeed;
        }
    }
    else
    {
        switch (desire->m_ePositionSeekState)
        {
        case PSS_ARRIVED:
            fMinSpeed = 0.0f;
            fMaxSpeed = 0.0f;
            break;
        case PSS_NEAR_SEEKING:
            fMinSpeed = fMinimumSpeedScale
                      * fn_8002CE14(desire->m_pFielder->GetTweaks());
            fMaxSpeed = desire->m_pFielder->GetTweaks()->GetRunningSpeed();
            fMinSpeed = nlMinEquals(fMinSpeed, fMaxSpeed);
            break;
        case PSS_FAR_SEEKING:
            fMinSpeed = desire->m_pFielder->GetTweaks()->GetRunningSpeed();
            fMaxSpeed = desire->m_pFielder->GetTweaks()->GetRunningSpeed();
            break;
        default:
            break;
        }

        if (turboRequest == TR_FORCED_OFF)
        {
            float fRunningSpeed = desire->m_pFielder->GetTweaks()->GetRunningSpeed();
            fMaxSpeed = nlMinEquals(fMaxSpeed, fRunningSpeed);
        }
        else if (turboRequest == TR_FORCED_ON
            || (turboRequest == TR_MOVING_TARGET
                && IsNotNearlyZero(fDesiredPositionRateOfChange, 0.0f)))
        {
            fMinSpeed = desire->m_pFielder->GetTweaks()->GetRunningSpeed();
            fMaxSpeed = desire->m_pFielder->GetTweaks()->GetRunningSpeed();
        }
    }

    if (!IsNotNearlyZero(fDistance, 0.0f))
    {
        fMinSpeed = 0.0f;
        fMaxSpeed = 0.0f;
    }

    float fDesiredSpeed = InterpolateClamped(
        fMinSpeed, fMaxSpeed, fSpeedPercent * fUrgency);
    fDesiredSpeed *= GetSteeringSpeedScale(fDistance);
    fDesiredSpeed = nlMinEquals(
        fDesiredSpeed, fn_8002E1B0(desire->m_pFielder));
    desire->m_pFielder->mUnidentified024.m_fDesiredSpeed = fDesiredSpeed;
}

float GetBallFacingWeight(cFielder* pFielder)
{
    float result = 0.0f;
    if (StrategicBallOwner(pFielder) >= 0.5f)
    {
        if (g_pBall->GetOwnerGoalie() != NULL)
        {
            result = 1.0f;
        }
        else
        {
            bool bHasPad = pFielder->GetGlobalPad() != NULL;
            if (!bHasPad && fn_800D6A90(pFielder) >= 0.5f)
            {
                result = 1.0f;
            }
            else if (Defensive(fn_800D6670(pFielder)) >= 0.5f)
            {
                result = 1.0f - FarToMyNet(pFielder);
                float fBall = fn_800DC19C(pFielder, g_pBall);
                result = fBall / 2.0f + result / 2.0f;
            }
            else if (Offensive(fn_800D6670(pFielder)) >= 0.5f)
            {
                result = 1.0f - FarToTheirNet(pFielder);
            }
            else
            {
                result = 1.0f - FarToBall(pFielder);
            }
        }
    }
    return result;
}

static float GetMarkFacingWeight(cFielder* TheFielder)
{
    cFielder* mark = fn_800D6734(TheFielder);
    float inBetween = InBetweenMyNetAnd(TheFielder, mark);
    return inBetween;
}

extern "C" void fn_800C6FDC(DesireSteering* desire, float)
{
    bool bCanFaceBall = desire->m_pFielder->m_pBall == NULL
                     && !desire->m_pFielder->fn_8003E70C()
                     && !HasGlobalPad(desire->m_pFielder)
                     && !(fn_8003E948(desire->m_pFielder)
                          && desire->m_pFielder->mUnidentified3DC)
                     && !desire->m_pFielder->fn_8003EA6C()
                     && !fn_8003E948(desire->m_pFielder)
                     && !fn_8003E8A0(desire->m_pFielder)
                     && !desire->m_pFielder->fn_8003E8F4()
                     && !(bool)ReceivingPass(desire->m_pFielder)
                     && fn_800DED80(desire->m_pFielder) < 0.2f;

    int aFacingDirection = desire->m_pFielder->mUnidentified024.m_aDesiredMovementDirection;
    eStrafeDirection eMovement = STRAFE_IDLE;
    if (bCanFaceBall)
    {
        if (desire->m_fFacingTotalWeight > 0.0f)
        {
            desire->m_fDesiredFacingDirection
                /= desire->m_fFacingTotalWeight;
            desire->m_fFacingTotalWeight = 0.0f;
            aFacingDirection = (int)(
                desire->m_fDesiredFacingDirection + 0.5f);
        }
        else
        {
            float fFacingWeight = GetBallFacingWeight(desire->m_pFielder);
            float fMarkWeight = GetMarkFacingWeight(desire->m_pFielder);
            float fTotalWeight = fFacingWeight + fMarkWeight;
            bool bTurning = true;
            bool bStrafing = true;
            eStrafeDirection eLastMovement
                = desire->m_pFielder->mActionRunningVars.eLastStrafeDirection;
            if (eLastMovement != STRAFE_LEFT
                && eLastMovement != STRAFE_RIGHT)
            {
                bStrafing = false;
            }
            if (!bStrafing && eLastMovement != STRAFE_BACK)
            {
                bTurning = false;
            }
            float fThreshold = bTurning ? 0.5f : 0.75f;
            if (fTotalWeight > fThreshold)
            {
                nlVector3 v3FacingPos;
                nlVec3Scale(v3FacingPos, g_pBall->m_v3Position, 1.0f);
                cFielder* pMark = desire->m_pFielder->GetMark();
                if (pMark != NULL)
                {
                    nlVec3ScaleAdd(v3FacingPos, fMarkWeight / fTotalWeight,
                        pMark->mUnidentified024.m_v3Position, v3FacingPos);
                }
                float fDeltaX = v3FacingPos.x
                              - desire->m_pFielder->mUnidentified024.m_v3Position.x;
                float fDeltaY = v3FacingPos.y
                              - desire->m_pFielder->mUnidentified024.m_v3Position.y;
                aFacingDirection = (unsigned short)(int)(
                    nlATan2f(fDeltaY, fDeltaX)
                    * 10430.378f);
            }
        }

        eMovement = GetSteeringStrafeDirection(desire, (unsigned short)aFacingDirection,
            desire->m_pFielder->mUnidentified024.m_aDesiredMovementDirection);
        if (eMovement == STRAFE_FORWARD)
        {
            aFacingDirection = desire->m_pFielder->mUnidentified024.m_aDesiredMovementDirection;
        }
        desire->m_pFielder->Unknown8((unsigned short)aFacingDirection, false);
    }
    else
    {
        eMovement = desire->m_pFielder->mUnidentified024.m_fDesiredSpeed < 0.1f
                  ? STRAFE_IDLE : STRAFE_FORWARD;
    }
    desire->m_pFielder->mActionRunningVars.eLastStrafeDirection = eMovement;
}

eStrafeDirection GetSteeringStrafeDirection(DesireSteering* desire,
    unsigned short aDesiredFacingDirection,
    unsigned short aDesiredMovementDirection)
{
    cFielder* pFielder = desire->m_pFielder;
    short nMovementFacingDelta
        = (short)(aDesiredMovementDirection - aDesiredFacingDirection);

    float fTransitionToForwardDelta;
    float fTransitionToBackWardsDelta;
    switch (pFielder->mActionRunningVars.eLastStrafeDirection)
    {
    case STRAFE_RIGHT:
    case STRAFE_LEFT:
        fTransitionToForwardDelta
            = gGameTweaks.m_pGameTweaks->nStrafeToRunOutDirectionDelta;
        fTransitionToBackWardsDelta
            = gGameTweaks.m_pGameTweaks->nBackwardsToStrafeRunOutDirectionDelta;
        break;
    case STRAFE_IDLE:
    case STRAFE_FORWARD:
    case STRAFE_BACK:
    default:
        fTransitionToForwardDelta
            = gGameTweaks.m_pGameTweaks->nStrafeToRunInDirectionDelta;
        fTransitionToBackWardsDelta
            = gGameTweaks.m_pGameTweaks->nBackwardsToStrafeRunInDirectionDelta;
        break;
    }

    float fRunThreshold
        = 0.5f * (fn_8002C254(desire->m_pFielder->GetTweaks())
              - GetRunSpeed(pFielder->GetTweaks()))
        + GetRunSpeed(desire->m_pFielder->GetTweaks());
    float fDesiredSpeed = desire->m_pFielder->mUnidentified024.m_fDesiredSpeed;

    if (fDesiredSpeed < 0.1f)
    {
        return STRAFE_IDLE;
    }
    if (fDesiredSpeed < fRunThreshold)
    {
        int nAbsDelta = nMovementFacingDelta < 0
                      ? -nMovementFacingDelta : nMovementFacingDelta;
        if ((float)(unsigned short)nAbsDelta < fTransitionToForwardDelta)
        {
            return STRAFE_FORWARD;
        }
        if ((float)nMovementFacingDelta > -fTransitionToBackWardsDelta
            && (float)nMovementFacingDelta <= fTransitionToForwardDelta)
        {
            return STRAFE_RIGHT;
        }
        if ((float)nMovementFacingDelta < fTransitionToBackWardsDelta
            && (float)nMovementFacingDelta >= fTransitionToForwardDelta)
        {
            return STRAFE_LEFT;
        }
        return STRAFE_BACK;
    }
    return STRAFE_FORWARD;
}

bool UnidentifiedDesire35::Initialize(void*)
{
    mMaxDuration = 10.0f;
    fn_8006040C(g_pGame, m_pFielder);
    m_pFielder->mUnidentified3F8.mUnidentified00
        = m_pFielder->mUnidentified3F8.mUnidentified04;
    return true;
}

void UnidentifiedDesire35::Update(
    DesireUpdate* update, float fDeltaT)
{
    if (!m_pFielder->mUnidentified3DC)
    {
        return;
    }
    if (!fn_8002EDC8(m_pFielder, -1))
    {
        m_pFielder->fn_8005001C(true);
        fn_80060804(g_pGame, m_pFielder);
        return;
    }

    float fSpeed = m_pFielder->m_pBall != NULL
                 ? m_pFielder->GetTweaks()->GetRunningSpeed()
                 : fn_8002C254(m_pFielder->GetTweaks());
    m_pFielder->mUnidentified024.m_fDesiredSpeed = fSpeed;
    m_pFielder->mUnidentified3E0 -= fDeltaT;
    bool bRunning = m_pFielder->mUnidentified3E0 > 0.0f;
    if (!bRunning)
    {
        *update = 1;
        return;
    }

    m_pFielder->mUnidentified3F8.mUnidentified00 -= fDeltaT;
    short nFacingDelta = (short)(m_pFielder->mUnidentified024.m_aActualFacingDirection
        - m_pFielder->mUnidentified024.m_aDesiredFacingDirection);
    if (m_pFielder->mUnidentified3F8.mUnidentified00 <= 0.0f)
    {
        if (m_pFielder->mUnidentified3DD)
        {
            m_pFielder->fn_8005001C(true);
            return;
        }

        int nAbsFacingDelta
            = nFacingDelta < 0 ? -nFacingDelta : nFacingDelta;
        if ((unsigned short)nAbsFacingDelta > 0x2000)
        {
            fn_80060608(g_pGame, m_pFielder);
            if (m_pFielder->mUnidentified3E0 > 0.0f
                && m_pFielder->mUnidentified3E0
                    < m_pFielder->mUnidentified3F8.mUnidentified04)
            {
                m_pFielder->mUnidentified3E0
                    = m_pFielder->mUnidentified3F8.mUnidentified04;
            }
            if (nFacingDelta < 0)
            {
                m_pFielder->SetFacingDirection(
                    m_pFielder->mUnidentified024.m_aActualFacingDirection + 0x4000, true);
            }
            else
            {
                m_pFielder->SetFacingDirection(
                    m_pFielder->mUnidentified024.m_aActualFacingDirection - 0x4000, true);
            }
            fn_8006040C(g_pGame, m_pFielder);
            m_pFielder->mUnidentified3F8.mUnidentified00
                = m_pFielder->mUnidentified3F8.mUnidentified04;
        }
    }

    m_pFielder->fn_8001DCE0(
        m_pFielder->mUnidentified024.m_aActualFacingDirection);
    m_pFielder->mUnidentified024.m_aActualMovementDirection
        = m_pFielder->mUnidentified024.m_aActualFacingDirection;
    m_pFielder->fn_8001E304(fSpeed, fDeltaT);
}

void UnidentifiedDesire35::Cleanup()
{
    fn_80060608(g_pGame, m_pFielder);
}

#include "Game/AI/DesireSteeringDebug.inl"
