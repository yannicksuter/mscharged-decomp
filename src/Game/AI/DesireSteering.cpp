#include "Game/AI/DesireSteering.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
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

extern "C" void fn_8000F178(AvoidController*);
extern "C" float fn_8002BFA8(PlayerTweaks*, float);
extern "C" float fn_8002BFB8(PlayerTweaks*);
extern "C" float fn_8002C254(const PlayerTweaks*);
extern "C" float fn_8002C328(PlayerTweaks*);
extern "C" float fn_8002CE14(PlayerTweaks*);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" void fn_8003C7B0(cFielder*);
extern "C" bool fn_8003E8A0(cFielder*);
extern "C" bool fn_8003E948(cFielder*);
extern "C" void fn_8006040C(cGame*, cFielder*);
extern "C" void fn_80060608(cGame*, cFielder*);
extern "C" void fn_80060804(cGame*, cFielder*);
extern "C" cTeam* fn_800D6670(cFielder*);
extern "C" cFielder* fn_800D6734(cFielder*);
extern "C" float fn_800D6A90(cFielder*);
extern "C" float fn_800D8C84(cFielder*);
extern "C" float fn_800DC19C(cFielder*, cBall*);
extern "C" float fn_800DED80(cFielder*);
extern "C" float fn_800DFD74(cTeam*);

extern float lbl_806DC230;
extern bool lbl_806E0C50;
extern bool lbl_806E0C51;
extern bool lbl_806E0C52;
extern bool lbl_806E0E58;

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

DesireSteering::DesireSteering()
    : Desire(34, UnidentifiedStateTransition(lbl_806E20B8)),
      m_AvoidanceHistory(lbl_806DC230)
{
    m_pAvoidance = NULL;
}

DesireSteering::~DesireSteering()
{
    delete m_pAvoidance;
}

bool DesireSteering::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    if (m_pAvoidance == NULL)
    {
        m_pAvoidance = new (8, false) AvoidController(mUnidentifiedFielder);
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
    mUnidentified078 = -1.0f;
    return result;
}

void DesireSteering::UnidentifiedCleanup()
{
    fn_8000F178(m_pAvoidance);
    m_AvoidanceHistory.UnidentifiedReset();
}

extern "C" void fn_800C574C(DesireSteering* desire)
{
    desire->m_AvoidanceHistory.UnidentifiedReset();
}

extern "C" void fn_800C577C(DesireSteering* desire)
{
    fn_8000F178(desire->m_pAvoidance);
}

extern "C" void fn_800C5784(DesireSteering* desire)
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
    UnidentifiedDesireUpdate*, float fDeltaT)
{
    fn_8003E948(mUnidentifiedFielder);
    bool bUseAvoidance = !lbl_806E0C50
                      && (!lbl_806E0C51
                          || mUnidentifiedFielder->m_pTeam->m_nSide != HOME)
                      && (!lbl_806E0C52
                          || mUnidentifiedFielder->m_pTeam->m_nSide != AWAY);

    if (m_fTotalWeight < 0.0f)
    {
        if (m_fDesiredArrivalTime > 0.0f)
        {
            fn_800C6390(this, m_v3DesiredPos,
                fDeltaT, m_fDesiredArrivalTime);
            m_fDesiredArrivalTime -= fDeltaT;
            fn_800C5784(this);
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

        fn_800C66A4(this, m_v3DesiredPos,
            TR_FAR_DISTANCE, fDeltaT, m_fUrgency);
        m_v3LastDesiredPos = v3UnfilteredDesired;
    }
    else
    {
        fn_800C574C(this);
    }

    nlPolar desiredVelocity;
    desiredVelocity.a = mUnidentifiedFielder->mUnidentified024.m_aDesiredMovementDirection;
    desiredVelocity.r = mUnidentifiedFielder->mUnidentified024.m_fDesiredSpeed;
    nlPolarToCartesian(m_v3DesiredVel, desiredVelocity);
    m_v3DesiredVel.z = 0.0f;

    if (mUnidentifiedFielder->m_eActionState == ACTION_WAIT
        || mUnidentifiedFielder->m_eActionState == ACTION_NEED_ACTION)
    {
        mUnidentifiedFielder->StartRunning();
    }
    if (sUseAvoidance && bUseAvoidance)
    {
        fn_800C5DBC(this, fDeltaT);
    }
    fn_800C6FDC(this, fDeltaT);
    fn_8003C7B0(mUnidentifiedFielder);

    desiredVelocity.a = mUnidentifiedFielder->mUnidentified024.m_aDesiredMovementDirection;
    desiredVelocity.r = mUnidentifiedFielder->mUnidentified024.m_fDesiredSpeed;
    nlPolarToCartesian(m_v3DesiredVel, desiredVelocity);
    m_v3DesiredVel.z = 0.0f;
}

extern "C" void fn_800C5DBC(DesireSteering* desire, float fDeltaT)
{
    float fRepulsionMult = desire->m_fAvoidanceMult;
    int nThingsToAvoid = desire->m_ThingsToAvoid;

    if (ReceivingPass(desire->mUnidentifiedFielder))
    {
        fRepulsionMult = 0.0f;
    }
    else
    {
        bool bHasGlobalPad
            = desire->mUnidentifiedFielder->GetGlobalPad() != NULL;
        if (bHasGlobalPad)
        {
            if ((fn_8003E8A0(desire->mUnidentifiedFielder)
                    && desire->mUnidentifiedFielder->mUnidentified3DC)
                || (desire->mUnidentifiedFielder->fn_8003E9F0()
                    && desire->mUnidentifiedFielder->mUnidentified3DC))
            {
                nThingsToAvoid = AVOID_NOTHING;
            }
            else if (GameInfoManager::Instance()->GetStadium() == 0x0B
                || GameInfoManager::Instance()->IsRule0x4Equal3())
            {
                if (desire->mUnidentifiedFielder->fn_8001E160()
                    && !lbl_806E0E58)
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
        else if (fn_800DED80(desire->mUnidentifiedFielder))
        {
            fRepulsionMult = 1.0f;
        }
    }

    bool bHasGlobalPad
        = desire->mUnidentifiedFielder->GetGlobalPad() != NULL;
    if (!bHasGlobalPad && desire->mUnidentifiedFielder->fn_8003EA6C())
    {
        nThingsToAvoid &= ~(AVOID_FIELDERS | AVOID_POWERUPS);
    }

    if (nThingsToAvoid & AVOID_SIDELINES)
    {
        float fInterceptScore = fn_800DED80(desire->mUnidentifiedFielder);
        bool bHasBall = desire->mUnidentifiedFielder->m_pBall != NULL;
        float fCloseToTheirNet
            = CloseToTheirNet(desire->mUnidentifiedFielder);
        float fCloseToMyNet
            = CloseToMyNet(desire->mUnidentifiedFielder);
        bool bBetweenPosts
            = (float)fabs(desire->mUnidentifiedFielder
                              ->mUnidentified024.m_v3Position.y)
            <= 0.5f * cNet::GetNetWidth() + cNet::GetPostRadius();
        bool bHasGlobalPad
            = desire->mUnidentifiedFielder->GetGlobalPad() != NULL;

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

    if (fn_8003E948(desire->mUnidentifiedFielder)
        && desire->mUnidentifiedFielder->mUnidentified3DC)
    {
        nThingsToAvoid = AVOID_NOTHING;
    }

    desire->m_pAvoidance->SetThingsToAvoid(nThingsToAvoid);
    desire->m_pAvoidance->m_fRepulsionMult = fRepulsionMult;
    desire->m_pAvoidance->Update(fDeltaT);
    desire->m_ThingsToAvoid = AVOID_EVERYTHING;
    desire->m_fAvoidanceMult = 1.0f;
}

extern "C" void fn_800C60C4(DesireSteering* desire,
    const nlVector3& v3Position, float fUrgency, float fWeight)
{
    fn_8003E948(desire->mUnidentifiedFielder);
    if (desire->m_fTotalWeight == 0.0f)
    {
        desire->m_fUrgency = 0.0f;
        desire->m_v3DesiredPos = v3Zero;
    }

    if (desire->m_fTotalWeight >= 0.0f)
    {
        desire->m_fTotalWeight += fWeight;
        desire->m_fUrgency += fUrgency * fWeight;
        desire->m_v3DesiredPos.x += v3Position.x * fWeight;
        desire->m_v3DesiredPos.y += v3Position.y * fWeight;
        desire->m_v3DesiredPos.z = 0.0f;
    }
}

extern "C" void fn_800C61A4(DesireSteering* desire,
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

extern "C" const nlVector3* fn_800C61FC(DesireSteering* desire)
{
    DesireReceivePass* receivePass = (DesireReceivePass*)fn_8002E08C(
        desire->mUnidentifiedFielder, 22);

    if (g_pBall->UnidentifiedHasPassTarget()
        && g_pBall->m_pPassTarget == desire->mUnidentifiedFielder
        && receivePass != NULL && receivePass->UnidentifiedIsActive())
    {
        desire->m_v3TempDesiredPos = receivePass->GetAnimStartPosition();
        desire->m_v3TempDesiredPos.z = 0.0f;
        return &desire->m_v3TempDesiredPos;
    }

    bool bHasGlobalPad
        = desire->mUnidentifiedFielder->GetGlobalPad() != NULL;
    if (bHasGlobalPad)
    {
        nlVec3ScaleAdd(desire->m_v3TempDesiredPos, 0.3333f,
            desire->m_v3DesiredVel,
            desire->mUnidentifiedFielder->mUnidentified024.m_v3Position);
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

extern "C" void fn_800C6390(DesireSteering* desire,
    const nlVector3& v3Position, float fDeltaT, float fDesiredArrivalTime)
{
    nlVector3 v3FixedPos = v3Position;
    float fPlayerScale
        = desire->mUnidentifiedFielder->mUnidentified024.m_fPlayerScale;
    float fRadius = fn_8002BFA8(desire->mUnidentifiedFielder->GetTweaks(),
        fPlayerScale);
    cField::FixOutOfBoundsPosition(v3FixedPos, fRadius, false);

    float fDesiredPositionDistanceSq
        = CalculateDistanceSquared(v3FixedPos, desire->m_v3LastDesiredPos);
    float fDistSq = nlVec3DistanceSquared2D(v3FixedPos,
        desire->mUnidentifiedFielder->mUnidentified024.m_v3Position);

    if (fDistSq < desire->m_fForcedArrivalRadius
            * desire->m_fForcedArrivalRadius)
    {
        desire->m_ePositionSeekState = PSS_ARRIVED;
        if (desire->m_fDesiredFacingDirection >= 0.0f)
        {
            desire->mUnidentifiedFielder->Unknown8(
                (unsigned short)(int)(desire->m_fDesiredFacingDirection
                    + 0.5f),
                false);
        }
        desire->mUnidentifiedFielder->mUnidentified024.m_fDesiredSpeed = 0.0f;
        fn_800C5784(desire);
        return;
    }

    desire->m_ePositionSeekState = PSS_UNIDENTIFIED_3;
    float fDesiredSpeed
        = nlSqrt(fDistSq, true) - desire->m_fForcedArrivalRadius;
    float fMinimumSpeed = 0.0f;
    fDesiredSpeed = nlMinEquals(
        nlMaxEquals(
            (fDesiredSpeed /= fDesiredArrivalTime), fMinimumSpeed),
        fn_8002E1B0(desire->mUnidentifiedFielder));

    if (fDesiredSpeed < sMinimumDesiredSpeed)
    {
        fDesiredSpeed = 0.0f;
        if (desire->m_fDesiredFacingDirection >= 0.0f)
        {
            desire->mUnidentifiedFielder->Unknown8(
                (unsigned short)(int)(desire->m_fDesiredFacingDirection
                    + 0.5f),
                false);
        }
    }
    else
    {
        nlVector3 v3Direction;
        nlVec3Sub(v3Direction, v3FixedPos,
            desire->mUnidentifiedFielder->mUnidentified024.m_v3Position);
        unsigned short aDirection = nlVector3ToAngle(v3Direction);
        desire->mUnidentifiedFielder->fn_8001DCE0(aDirection);
        desire->mUnidentifiedFielder->Unknown8(
            aDirection, false);
    }

    fDesiredSpeed = nlMinEquals(
        fDesiredSpeed, fn_8002E1B0(desire->mUnidentifiedFielder));
    desire->mUnidentifiedFielder->mUnidentified024.m_fDesiredSpeed = fDesiredSpeed;
}

extern "C" void fn_800C66A4(DesireSteering* desire,
    const nlVector3& v3Pos, eTurboRequest turboRequest,
    float fDeltaT, float fUrgency)
{
    nlVector3 v3FixedPos = v3Pos;
    cFielder* pFielder = desire->mUnidentifiedFielder;
    float fRadius = fn_8002BFA8(
        pFielder->GetTweaks(), pFielder->mUnidentified024.m_fPlayerScale);
    cField::FixOutOfBoundsPosition(v3FixedPos, fRadius, false);

    float fDeltaX = v3FixedPos.x - pFielder->mUnidentified024.m_v3Position.x;
    float fDeltaY = v3FixedPos.y - pFielder->mUnidentified024.m_v3Position.y;
    float fDistance = nlSqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY, true);
    float fRadiusScale = fUrgency > 0.0f ? 1.0f / fUrgency : 1.0f;
    float fDesiredPositionRateOfChange = 0.0f;

    if ((float)fabs(fDistance) > 0.0001f)
    {
        float fLastDeltaX = v3FixedPos.x - desire->m_v3LastDesiredPos.x;
        float fLastDeltaY = v3FixedPos.y - desire->m_v3LastDesiredPos.y;
        float fLastDeltaZ = v3FixedPos.z - desire->m_v3LastDesiredPos.z;
        fDesiredPositionRateOfChange
            = nlSqrt(fLastDeltaX * fLastDeltaX
                    + fLastDeltaY * fLastDeltaY
                    + fLastDeltaZ * fLastDeltaZ,
                true)
            / fDeltaT;

        unsigned short aDirection
            = (unsigned short)(int)(nlATan2f(fDeltaY, fDeltaX)
                * 10430.378f);
        pFielder->SetFacingDirection(aDirection, false);
        pFielder->fn_8001DCE0(aDirection);
    }

    float fSpeedPercent = 0.0f;
    GameTweaks* pGameTweaks = gGameTweaks.m_pGameTweaks;
    switch (desire->m_ePositionSeekState)
    {
    case PSS_ARRIVED:
    case PSS_UNIDENTIFIED_3:
    {
        float fArrivalOut = fRadiusScale * pGameTweaks->fArrivalOutRadius;
        fSpeedPercent = NormalizeVal(fDistance, 0.0f, fArrivalOut);
        float fNearSeekOut
            = fRadiusScale * pGameTweaks->fNearSeekOutRadius;
        if (fDistance >= fNearSeekOut)
        {
            desire->m_ePositionSeekState = PSS_FAR_SEEKING;
        }
        else if (fDistance >= fArrivalOut)
        {
            desire->m_ePositionSeekState = PSS_NEAR_SEEKING;
        }
        break;
    }
    case PSS_NEAR_SEEKING:
    {
        float fArrivalIn = fRadiusScale * pGameTweaks->fArrivalInRadius;
        float fNearSeekOut
            = fRadiusScale * pGameTweaks->fNearSeekOutRadius;
        fSpeedPercent
            = NormalizeVal(fDistance, fArrivalIn, fNearSeekOut);
        if (fDistance >= fNearSeekOut)
        {
            desire->m_ePositionSeekState = PSS_FAR_SEEKING;
        }
        else if (fDistance <= fArrivalIn)
        {
            fn_800C5784(desire);
        }
        break;
    }
    case PSS_FAR_SEEKING:
    {
        float fNearSeekIn
            = fRadiusScale * pGameTweaks->fNearSeekInRadius;
        float fNearSeekOut
            = fRadiusScale * pGameTweaks->fNearSeekOutRadius;
        fSpeedPercent
            = NormalizeVal(fDistance, fNearSeekIn, fNearSeekOut);
        if (fDistance < fNearSeekIn)
        {
            desire->m_ePositionSeekState = PSS_NEAR_SEEKING;
        }
        else if (fDistance < fRadiusScale * pGameTweaks->fArrivalInRadius)
        {
            fn_800C5784(desire);
        }
        break;
    }
    default:
        break;
    }

    float fMinSpeed = 0.0f;
    float fMaxSpeed = 0.0f;
    if (pFielder->m_pBall == NULL)
    {
        switch (desire->m_ePositionSeekState)
        {
        case PSS_NEAR_SEEKING:
            fMinSpeed = fn_8002CE14(pFielder->GetTweaks());
            fMaxSpeed = fn_8002BFB8(pFielder->GetTweaks());
            if (fMinSpeed > fMaxSpeed)
            {
                fMinSpeed = fMaxSpeed;
            }
            break;
        case PSS_FAR_SEEKING:
            fMinSpeed = fn_8002BFB8(pFielder->GetTweaks());
            fMaxSpeed = fn_8002C254(pFielder->GetTweaks());
            break;
        default:
            break;
        }

        if (turboRequest == TR_FORCED_OFF)
        {
            float fRunningSpeed = fn_8002BFB8(pFielder->GetTweaks());
            if (fMaxSpeed > fRunningSpeed)
            {
                fMaxSpeed = fRunningSpeed;
            }
        }
        else if (turboRequest == TR_FORCED_ON
            || (turboRequest == TR_MOVING_TARGET
                && (float)fabs(fDesiredPositionRateOfChange) > 0.0001f))
        {
            fMinSpeed = fn_8002C254(pFielder->GetTweaks());
            fMaxSpeed = fMinSpeed;
        }
    }
    else
    {
        switch (desire->m_ePositionSeekState)
        {
        case PSS_NEAR_SEEKING:
            fMinSpeed = fn_8002CE14(pFielder->GetTweaks());
            fMaxSpeed = fn_8002C328(pFielder->GetTweaks());
            if (fMinSpeed > fMaxSpeed)
            {
                fMinSpeed = fMaxSpeed;
            }
            break;
        case PSS_FAR_SEEKING:
            fMinSpeed = fn_8002C328(pFielder->GetTweaks());
            fMaxSpeed = fn_8002C328(pFielder->GetTweaks());
            break;
        default:
            break;
        }

        if (turboRequest == TR_FORCED_OFF)
        {
            float fRunningSpeed = fn_8002C328(pFielder->GetTweaks());
            if (fMaxSpeed > fRunningSpeed)
            {
                fMaxSpeed = fRunningSpeed;
            }
        }
        else if (turboRequest == TR_FORCED_ON
            || (turboRequest == TR_MOVING_TARGET
                && (float)fabs(fDesiredPositionRateOfChange) > 0.0001f))
        {
            fMinSpeed = fn_8002C328(pFielder->GetTweaks());
            fMaxSpeed = fn_8002C328(pFielder->GetTweaks());
        }
    }

    if ((float)fabs(fDistance) <= 0.0001f)
    {
        fMinSpeed = 0.0f;
        fMaxSpeed = 0.0f;
    }

    float fDesiredSpeed = InterpolateClamped(
        fMinSpeed, fMaxSpeed, fSpeedPercent * fUrgency);
    float fSteeringSpeedScale = fDistance;
    sSteeringSpeedScale.Evaluate(fDistance, fSteeringSpeedScale);
    fDesiredSpeed *= fSteeringSpeedScale;
    float fMaximumSpeed = fn_8002E1B0(pFielder);
    if (fDesiredSpeed > fMaximumSpeed)
    {
        fDesiredSpeed = fMaximumSpeed;
    }
    pFielder->mUnidentified024.m_fDesiredSpeed = fDesiredSpeed;
}

extern "C" float fn_800C6EB0(cFielder* pFielder)
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

extern "C" eStrafeDirection fn_800C7348(DesireSteering* desire,
    unsigned short aDesiredFacingDirection,
    unsigned short aDesiredMovementDirection)
{
    cFielder* pFielder = desire->mUnidentifiedFielder;
    short nMovementFacingDelta
        = (short)(aDesiredMovementDirection - aDesiredFacingDirection);

    float fTransitionToForwardDelta;
    float fTransitionToBackWardsDelta;
    if (pFielder->mActionRunningVars.eLastStrafeDirection == 1
        || pFielder->mActionRunningVars.eLastStrafeDirection == 2)
    {
        fTransitionToForwardDelta
            = gGameTweaks.m_pGameTweaks->nStrafeToRunOutDirectionDelta;
        fTransitionToBackWardsDelta
            = gGameTweaks.m_pGameTweaks->nBackwardsToStrafeRunOutDirectionDelta;
    }
    else
    {
        fTransitionToForwardDelta
            = gGameTweaks.m_pGameTweaks->nStrafeToRunInDirectionDelta;
        fTransitionToBackWardsDelta
            = gGameTweaks.m_pGameTweaks->nBackwardsToStrafeRunInDirectionDelta;
    }

    float fJoggingSpeed = fn_8002BFB8(pFielder->GetTweaks());
    float fRunningSpeed = fn_8002C254(pFielder->GetTweaks());
    float fRunThreshold
        = 0.5f * (fRunningSpeed - fJoggingSpeed) + fJoggingSpeed;

    if (pFielder->mUnidentified024.m_fDesiredSpeed < 0.1f)
    {
        return STRAFE_IDLE;
    }
    if (pFielder->mUnidentified024.m_fDesiredSpeed >= fRunThreshold)
    {
        return STRAFE_FORWARD;
    }

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
        && (float)nMovementFacingDelta >= -fTransitionToForwardDelta)
    {
        return STRAFE_LEFT;
    }
    return STRAFE_BACK;
}

extern "C" void fn_800C6FDC(DesireSteering* desire, float)
{
    cFielder* pFielder = desire->mUnidentifiedFielder;
    bool bCanFaceBall = pFielder->m_pBall == NULL
                     && !pFielder->fn_8003E70C()
                     && pFielder->GetGlobalPad() == NULL
                     && !(fn_8003E948(pFielder)
                          && pFielder->mUnidentified3DC)
                     && !pFielder->fn_8003EA6C()
                     && !fn_8003E948(pFielder)
                     && !fn_8003E8A0(pFielder)
                     && !pFielder->fn_8003E8F4()
                     && ReceivingPass(pFielder) == 0.0f
                     && fn_800DED80(pFielder) < 0.2f;

    unsigned short aFacingDirection = pFielder->mUnidentified024.m_aDesiredMovementDirection;
    eStrafeDirection eMovement = STRAFE_IDLE;
    if (bCanFaceBall)
    {
        if (desire->m_fFacingTotalWeight > 0.0f)
        {
            desire->m_fDesiredFacingDirection
                /= desire->m_fFacingTotalWeight;
            desire->m_fFacingTotalWeight = 0.0f;
            aFacingDirection = (unsigned short)(int)(
                desire->m_fDesiredFacingDirection + 0.5f);
        }
        else
        {
            float fFacingWeight = fn_800C6EB0(pFielder);
            cFielder* pMark = fn_800D6734(pFielder);
            float fMarkWeight = InBetweenMyNetAnd(pFielder, pMark);
            float fTotalWeight = fFacingWeight + fMarkWeight;
            bool bTurning = pFielder->mActionRunningVars.eLastStrafeDirection == 1
                         || pFielder->mActionRunningVars.eLastStrafeDirection == 2
                         || pFielder->mActionRunningVars.eLastStrafeDirection == 4;
            float fThreshold = bTurning ? 0.5f : 0.75f;
            if (fTotalWeight > fThreshold)
            {
                nlVector3 v3FacingPos = g_pBall->m_v3Position;
                if (pMark != NULL)
                {
                    float fScale = fMarkWeight / fTotalWeight;
                    v3FacingPos.x += fScale * pMark->mUnidentified024.m_v3Position.x;
                    v3FacingPos.y += fScale * pMark->mUnidentified024.m_v3Position.y;
                    v3FacingPos.z += fScale * pMark->mUnidentified024.m_v3Position.z;
                }
                aFacingDirection = (unsigned short)(int)(
                    nlATan2f(v3FacingPos.y - pFielder->mUnidentified024.m_v3Position.y,
                        v3FacingPos.x - pFielder->mUnidentified024.m_v3Position.x)
                    * 10430.378f);
            }
        }

        eMovement = fn_800C7348(desire, aFacingDirection,
            pFielder->mUnidentified024.m_aDesiredMovementDirection);
        if (eMovement == STRAFE_FORWARD)
        {
            aFacingDirection = pFielder->mUnidentified024.m_aDesiredMovementDirection;
        }
        pFielder->SetFacingDirection(aFacingDirection, false);
    }
    else
    {
        eMovement = pFielder->mUnidentified024.m_fDesiredSpeed < 0.1f
                  ? STRAFE_IDLE : STRAFE_FORWARD;
    }
    pFielder->mActionRunningVars.eLastStrafeDirection = eMovement;
}

void DesireSteering::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireSteering");
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&m_ePositionSeekState - (u8*)&mvDesiredPosition,
        "m_ePositionSeekState");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3DesiredPos - (u8*)&mvDesiredPosition,
        "m_v3DesiredPos");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fDesiredFacingDirection - (u8*)&mvDesiredPosition,
        "m_fDesiredFacingDirection");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fFacingTotalWeight - (u8*)&mvDesiredPosition,
        "m_fFacingTotalWeight");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3LastDesiredPos - (u8*)&mvDesiredPosition,
        "m_v3LastDesiredPos");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3DesiredVel - (u8*)&mvDesiredPosition,
        "m_v3DesiredVel");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3TempDesiredPos - (u8*)&mvDesiredPosition,
        "m_v3TempDesiredPos");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fTotalWeight - (u8*)&mvDesiredPosition,
        "m_fTotalWeight");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fUrgency - (u8*)&mvDesiredPosition,
        "m_fUrgency");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fDesiredArrivalTime - (u8*)&mvDesiredPosition,
        "m_fDesiredArrivalTime");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fForcedArrivalRadius - (u8*)&mvDesiredPosition,
        "m_fForcedArrivalRadius");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fAvoidanceMult - (u8*)&mvDesiredPosition,
        "m_fAvoidanceMult");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&m_ThingsToAvoid - (u8*)&mvDesiredPosition,
        "m_ThingsToAvoid");
    cache->EndType();
}

void DesireSteering::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSteeringType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSteeringType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireSteeringType, data, context);
    cache->WriteData(sDesireSteeringType, data,
        sizeof(DesireSteering) - offset);
}

bool UnidentifiedDesire35::UnidentifiedInitialize(void*)
{
    mUnidentified078 = 10.0f;
    fn_8006040C(g_pGame, mUnidentifiedFielder);
    mUnidentifiedFielder->mUnidentified3F8.mUnidentified00
        = mUnidentifiedFielder->mUnidentified3F8.mUnidentified04;
    return true;
}

void UnidentifiedDesire35::Update(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (!mUnidentifiedFielder->mUnidentified3DC)
    {
        return;
    }
    if (!fn_8002EDC8(mUnidentifiedFielder, -1))
    {
        mUnidentifiedFielder->fn_8005001C(true);
        fn_80060804(g_pGame, mUnidentifiedFielder);
        return;
    }

    float fSpeed = mUnidentifiedFielder->m_pBall != NULL
                 ? fn_8002C328(mUnidentifiedFielder->GetTweaks())
                 : fn_8002C254(mUnidentifiedFielder->GetTweaks());
    mUnidentifiedFielder->mUnidentified024.m_fDesiredSpeed = fSpeed;
    mUnidentifiedFielder->mUnidentified3E0 -= fDeltaT;
    bool bRunning = mUnidentifiedFielder->mUnidentified3E0 > 0.0f;
    if (!bRunning)
    {
        *update = 1;
        return;
    }

    mUnidentifiedFielder->mUnidentified3F8.mUnidentified00 -= fDeltaT;
    short nFacingDelta = (short)(mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection
        - mUnidentifiedFielder->mUnidentified024.m_aDesiredFacingDirection);
    if (mUnidentifiedFielder->mUnidentified3F8.mUnidentified00 <= 0.0f)
    {
        if (mUnidentifiedFielder->mUnidentified3DD)
        {
            mUnidentifiedFielder->fn_8005001C(true);
            return;
        }

        int nAbsFacingDelta
            = nFacingDelta < 0 ? -nFacingDelta : nFacingDelta;
        if ((unsigned short)nAbsFacingDelta > 0x2000)
        {
            fn_80060608(g_pGame, mUnidentifiedFielder);
            if (mUnidentifiedFielder->mUnidentified3E0 > 0.0f
                && mUnidentifiedFielder->mUnidentified3E0
                    < mUnidentifiedFielder->mUnidentified3F8.mUnidentified04)
            {
                mUnidentifiedFielder->mUnidentified3E0
                    = mUnidentifiedFielder->mUnidentified3F8.mUnidentified04;
            }
            if (nFacingDelta < 0)
            {
                mUnidentifiedFielder->SetFacingDirection(
                    mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection + 0x4000, true);
            }
            else
            {
                mUnidentifiedFielder->SetFacingDirection(
                    mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection - 0x4000, true);
            }
            fn_8006040C(g_pGame, mUnidentifiedFielder);
            mUnidentifiedFielder->mUnidentified3F8.mUnidentified00
                = mUnidentifiedFielder->mUnidentified3F8.mUnidentified04;
        }
    }

    mUnidentifiedFielder->fn_8001DCE0(
        mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection);
    mUnidentifiedFielder->mUnidentified024.m_aActualMovementDirection
        = mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
    mUnidentifiedFielder->fn_8001E304(fSpeed, fDeltaT);
}

void UnidentifiedDesire35::UnidentifiedCleanup()
{
    fn_80060608(g_pGame, mUnidentifiedFielder);
}

UnidentifiedDesire35::~UnidentifiedDesire35()
{
}
