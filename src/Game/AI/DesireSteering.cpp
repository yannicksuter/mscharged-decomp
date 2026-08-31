#include "Game/AI/DesireSteering.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Team.h"
#include <math.h>
#include <stddef.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

extern "C" void fn_8000F178(AvoidController*);
extern "C" void fn_8001DCE0(cFielder*, unsigned short);
extern "C" void fn_8001E304(cFielder*, float, float);
extern "C" bool fn_8001E160(cFielder*);
extern "C" float fn_8002BFA8(PlayerTweaks*, float);
extern "C" float fn_8002BFB8(PlayerTweaks*);
extern "C" float fn_8002C254(PlayerTweaks*);
extern "C" float fn_8002C328(PlayerTweaks*);
extern "C" float fn_8002CE14(PlayerTweaks*);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" void fn_80040368(cFielder*);
extern "C" void fn_8003C7B0(cFielder*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" bool fn_8003E70C(cFielder*);
extern "C" bool fn_8003E8A0(cFielder*);
extern "C" bool fn_8003E8F4(cFielder*);
extern "C" bool fn_8003E948(cFielder*);
extern "C" bool fn_8003E9F0(cFielder*);
extern "C" bool fn_8003EA6C(cFielder*);
extern "C" void fn_8006040C(cGame*, cFielder*);
extern "C" void fn_80060608(cGame*, cFielder*);
extern "C" void fn_80060804(cGame*, cFielder*);
extern "C" cTeam* fn_800D6670(cFielder*);
extern "C" cFielder* fn_800D6734(cFielder*);
extern "C" float fn_800D6A90(cFielder*);
extern "C" float fn_800D6E54(cFielder*);
extern "C" float fn_800D8C84(cFielder*);
extern "C" float fn_800D8CC4(cFielder*);
extern "C" float fn_800D8E3C(cFielder*);
extern "C" float fn_800D8EF8(cFielder*);
extern "C" float fn_800D9070(cFielder*);
extern "C" float fn_800DBC5C(cFielder*, cFielder*);
extern "C" float fn_800DC19C(cFielder*, cBall*);
extern "C" float fn_800DED80(cFielder*);
extern "C" float fn_800DEFD4(cFielder*);
extern "C" float fn_800DFD48(cTeam*);
extern "C" float fn_800DFD74(cTeam*);

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

static UnidentifiedVector2Array sSteeringSpeedScale(
    sSteeringSpeedScalePoints, 6);
static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static unsigned short sDesireSteeringType = 0xFFFF;
static bool sUseAvoidance = true;
static float sMinimumDesiredSpeed = 0.1f;

static float EvaluateSteeringSpeedScale(float distance)
{
    if (sSteeringSpeedScale.mCount == 0)
    {
        return distance;
    }

    if (distance <= sSteeringSpeedScale.mData[0].x)
    {
        return sSteeringSpeedScale.mData[0].y;
    }

    int last = sSteeringSpeedScale.mCount - 1;
    if (distance >= sSteeringSpeedScale.mData[last].x)
    {
        return sSteeringSpeedScale.mData[last].y;
    }

    int upper = 1;
    while (upper < sSteeringSpeedScale.mCount
        && distance > sSteeringSpeedScale.mData[upper].x)
    {
        ++upper;
    }

    int lower = upper - 1;
    float range = sSteeringSpeedScale.mData[upper].x
                - sSteeringSpeedScale.mData[lower].x;
    if (range < 0.0001f)
    {
        range = 0.0001f;
    }
    float percent
        = (distance - sSteeringSpeedScale.mData[lower].x) / range;
    return (1.0f - percent) * sSteeringSpeedScale.mData[lower].y
         + percent * sSteeringSpeedScale.mData[upper].y;
}

DesireSteering::DesireSteering()
    : Desire(34, UnidentifiedStateTransition(lbl_806E20B8))
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

    bool bPassTarget = (g_pBall->meBallState == 5
                           || g_pBall->meBallState == 3)
                    && g_pBall->m_pPassTarget != NULL;
    if (bPassTarget
        && g_pBall->m_pPassTarget == desire->mUnidentifiedFielder
        && receivePass != NULL && receivePass->UnidentifiedIsActive())
    {
        desire->m_v3TempDesiredPos = receivePass->GetAnimStartPosition();
        desire->m_v3TempDesiredPos.z = 0.0f;
        return &desire->m_v3TempDesiredPos;
    }

    if (desire->mUnidentifiedFielder->GetGlobalPad() != NULL)
    {
        desire->m_v3TempDesiredPos.x
            = desire->mUnidentifiedFielder->m_v3Position.x
            + (1.0f / 3.0f) * desire->m_v3DesiredVel.x;
        desire->m_v3TempDesiredPos.y
            = desire->mUnidentifiedFielder->m_v3Position.y
            + (1.0f / 3.0f) * desire->m_v3DesiredVel.y;
        desire->m_v3TempDesiredPos.z
            = desire->mUnidentifiedFielder->m_v3Position.z
            + (1.0f / 3.0f) * desire->m_v3DesiredVel.z;
        return &desire->m_v3TempDesiredPos;
    }

    if (desire->m_fUrgency != 0.0f)
    {
        if (desire->m_fTotalWeight > 0.0f)
        {
            float scale = 1.0f / desire->m_fTotalWeight;
            desire->m_v3TempDesiredPos.x
                = scale * desire->m_v3DesiredPos.x;
            desire->m_v3TempDesiredPos.y
                = scale * desire->m_v3DesiredPos.y;
            desire->m_v3TempDesiredPos.z
                = scale * desire->m_v3DesiredPos.z;
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
    float fRadius = fn_8002BFA8(fn_8003E6E4(desire->mUnidentifiedFielder),
        desire->mUnidentifiedFielder->mUnidentified0A0);
    cField::FixOutOfBoundsPosition(v3FixedPos, fRadius, false);

    float fDeltaX
        = v3FixedPos.x - desire->mUnidentifiedFielder->m_v3Position.x;
    float fDeltaY
        = v3FixedPos.y - desire->mUnidentifiedFielder->m_v3Position.y;
    float fDistSq = fDeltaX * fDeltaX + fDeltaY * fDeltaY;

    if (fDistSq < desire->m_fForcedArrivalRadius
            * desire->m_fForcedArrivalRadius)
    {
        desire->m_ePositionSeekState = PSS_ARRIVED;
        if (desire->m_fDesiredFacingDirection >= 0.0f)
        {
            desire->mUnidentifiedFielder->SetFacingDirection(
                (unsigned short)(int)(desire->m_fDesiredFacingDirection
                    + 0.5f),
                false);
        }
        desire->mUnidentifiedFielder->m_fDesiredSpeed = 0.0f;
        fn_800C5784(desire);
        return;
    }

    desire->m_ePositionSeekState = PSS_UNIDENTIFIED_3;
    float fDesiredSpeed
        = (nlSqrt(fDistSq, true) - desire->m_fForcedArrivalRadius)
        / fDesiredArrivalTime;
    if (fDesiredSpeed < 0.0f)
    {
        fDesiredSpeed = 0.0f;
    }
    float fMaximumSpeed = fn_8002E1B0(desire->mUnidentifiedFielder);
    if (fDesiredSpeed > fMaximumSpeed)
    {
        fDesiredSpeed = fMaximumSpeed;
    }

    if (fDesiredSpeed < sMinimumDesiredSpeed)
    {
        fDesiredSpeed = 0.0f;
        if (desire->m_fDesiredFacingDirection >= 0.0f)
        {
            desire->mUnidentifiedFielder->SetFacingDirection(
                (unsigned short)(int)(desire->m_fDesiredFacingDirection
                    + 0.5f),
                false);
        }
    }
    else
    {
        unsigned short aDirection
            = (unsigned short)(int)(nlATan2f(fDeltaY, fDeltaX)
                * 10430.378f);
        fn_8001DCE0(desire->mUnidentifiedFielder, aDirection);
        desire->mUnidentifiedFielder->SetFacingDirection(
            aDirection, false);
    }

    fMaximumSpeed = fn_8002E1B0(desire->mUnidentifiedFielder);
    if (fDesiredSpeed > fMaximumSpeed)
    {
        fDesiredSpeed = fMaximumSpeed;
    }
    desire->mUnidentifiedFielder->m_fDesiredSpeed = fDesiredSpeed;
}

extern "C" void fn_800C66A4(DesireSteering* desire,
    const nlVector3& v3Pos, eTurboRequest turboRequest,
    float fDeltaT, float fUrgency)
{
    nlVector3 v3FixedPos = v3Pos;
    cFielder* pFielder = desire->mUnidentifiedFielder;
    float fRadius = fn_8002BFA8(
        fn_8003E6E4(pFielder), pFielder->mUnidentified0A0);
    cField::FixOutOfBoundsPosition(v3FixedPos, fRadius, false);

    float fDeltaX = v3FixedPos.x - pFielder->m_v3Position.x;
    float fDeltaY = v3FixedPos.y - pFielder->m_v3Position.y;
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
        fn_8001DCE0(pFielder, aDirection);
    }

    float fSpeedPercent = 0.0f;
    GameTweaks* pGameTweaks = lbl_8056CF08.m_pGameTweaks;
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
            fMinSpeed = fn_8002CE14(fn_8003E6E4(pFielder));
            fMaxSpeed = fn_8002BFB8(fn_8003E6E4(pFielder));
            if (fMinSpeed > fMaxSpeed)
            {
                fMinSpeed = fMaxSpeed;
            }
            break;
        case PSS_FAR_SEEKING:
            fMinSpeed = fn_8002BFB8(fn_8003E6E4(pFielder));
            fMaxSpeed = fn_8002C254(fn_8003E6E4(pFielder));
            break;
        default:
            break;
        }

        if (turboRequest == TR_FORCED_OFF)
        {
            float fRunningSpeed = fn_8002BFB8(fn_8003E6E4(pFielder));
            if (fMaxSpeed > fRunningSpeed)
            {
                fMaxSpeed = fRunningSpeed;
            }
        }
        else if (turboRequest == TR_FORCED_ON
            || (turboRequest == TR_MOVING_TARGET
                && (float)fabs(fDesiredPositionRateOfChange) > 0.0001f))
        {
            fMinSpeed = fn_8002C254(fn_8003E6E4(pFielder));
            fMaxSpeed = fMinSpeed;
        }
    }
    else
    {
        switch (desire->m_ePositionSeekState)
        {
        case PSS_NEAR_SEEKING:
            fMinSpeed = fn_8002CE14(fn_8003E6E4(pFielder));
            fMaxSpeed = fn_8002C328(fn_8003E6E4(pFielder));
            if (fMinSpeed > fMaxSpeed)
            {
                fMinSpeed = fMaxSpeed;
            }
            break;
        case PSS_FAR_SEEKING:
            fMinSpeed = fn_8002C328(fn_8003E6E4(pFielder));
            fMaxSpeed = fn_8002C328(fn_8003E6E4(pFielder));
            break;
        default:
            break;
        }

        if (turboRequest == TR_FORCED_OFF)
        {
            float fRunningSpeed = fn_8002C328(fn_8003E6E4(pFielder));
            if (fMaxSpeed > fRunningSpeed)
            {
                fMaxSpeed = fRunningSpeed;
            }
        }
        else if (turboRequest == TR_FORCED_ON
            || (turboRequest == TR_MOVING_TARGET
                && (float)fabs(fDesiredPositionRateOfChange) > 0.0001f))
        {
            fMinSpeed = fn_8002C328(fn_8003E6E4(pFielder));
            fMaxSpeed = fn_8002C328(fn_8003E6E4(pFielder));
        }
    }

    if ((float)fabs(fDistance) <= 0.0001f)
    {
        fMinSpeed = 0.0f;
        fMaxSpeed = 0.0f;
    }

    float fDesiredSpeed = InterpolateClamped(
        fMinSpeed, fMaxSpeed, fSpeedPercent * fUrgency);
    fDesiredSpeed *= EvaluateSteeringSpeedScale(fDistance);
    float fMaximumSpeed = fn_8002E1B0(pFielder);
    if (fDesiredSpeed > fMaximumSpeed)
    {
        fDesiredSpeed = fMaximumSpeed;
    }
    pFielder->m_fDesiredSpeed = fDesiredSpeed;
}

extern "C" float fn_800C6EB0(cFielder* pFielder)
{
    float result = 0.0f;
    if (fn_800D6E54(pFielder) >= 0.5f)
    {
        return result;
    }
    if (g_pBall->GetOwnerGoalie() != NULL)
    {
        return 1.0f;
    }
    if (pFielder->GetGlobalPad() == NULL
        && fn_800D6A90(pFielder) >= 0.5f)
    {
        return 1.0f;
    }
    if (fn_800DFD74(fn_800D6670(pFielder)) >= 0.5f)
    {
        float fUnmarked = 1.0f - fn_800D8E3C(pFielder);
        return 0.5f * fUnmarked
             + 0.5f * fn_800DC19C(pFielder, g_pBall);
    }
    if (fn_800DFD48(fn_800D6670(pFielder)) >= 0.5f)
    {
        return 1.0f - fn_800D9070(pFielder);
    }
    return 1.0f - fn_800D8C84(pFielder);
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
    if (pFielder->mUnidentified37C == 1
        || pFielder->mUnidentified37C == 2)
    {
        fTransitionToForwardDelta
            = lbl_8056CF08.m_pGameTweaks->nStrafeToRunOutDirectionDelta;
        fTransitionToBackWardsDelta
            = lbl_8056CF08.m_pGameTweaks->nBackwardsToStrafeRunOutDirectionDelta;
    }
    else
    {
        fTransitionToForwardDelta
            = lbl_8056CF08.m_pGameTweaks->nStrafeToRunInDirectionDelta;
        fTransitionToBackWardsDelta
            = lbl_8056CF08.m_pGameTweaks->nBackwardsToStrafeRunInDirectionDelta;
    }

    float fJoggingSpeed = fn_8002BFB8(fn_8003E6E4(pFielder));
    float fRunningSpeed = fn_8002C254(fn_8003E6E4(pFielder));
    float fRunThreshold
        = 0.5f * (fRunningSpeed - fJoggingSpeed) + fJoggingSpeed;

    if (pFielder->m_fDesiredSpeed < 0.1f)
    {
        return STRAFE_IDLE;
    }
    if (pFielder->m_fDesiredSpeed >= fRunThreshold)
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
                     && !fn_8003E70C(pFielder)
                     && pFielder->GetGlobalPad() == NULL
                     && !(fn_8003E948(pFielder)
                          && pFielder->mUnidentified3DC)
                     && !fn_8003EA6C(pFielder)
                     && !fn_8003E948(pFielder)
                     && !fn_8003E8A0(pFielder)
                     && !fn_8003E8F4(pFielder)
                     && fn_800DEFD4(pFielder) == 0.0f
                     && fn_800DED80(pFielder) < 0.2f;

    unsigned short aFacingDirection = pFielder->m_aDesiredMovementDirection;
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
            float fMarkWeight = fn_800DBC5C(pFielder, pMark);
            float fTotalWeight = fFacingWeight + fMarkWeight;
            bool bTurning = pFielder->mUnidentified37C == 1
                         || pFielder->mUnidentified37C == 2
                         || pFielder->mUnidentified37C == 4;
            float fThreshold = bTurning ? 0.5f : 0.75f;
            if (fTotalWeight > fThreshold)
            {
                nlVector3 v3FacingPos = g_pBall->m_v3Position;
                if (pMark != NULL)
                {
                    float fScale = fMarkWeight / fTotalWeight;
                    v3FacingPos.x += fScale * pMark->m_v3Position.x;
                    v3FacingPos.y += fScale * pMark->m_v3Position.y;
                    v3FacingPos.z += fScale * pMark->m_v3Position.z;
                }
                aFacingDirection = (unsigned short)(int)(
                    nlATan2f(v3FacingPos.y - pFielder->m_v3Position.y,
                        v3FacingPos.x - pFielder->m_v3Position.x)
                    * 10430.378f);
            }
        }

        eMovement = fn_800C7348(desire, aFacingDirection,
            pFielder->m_aDesiredMovementDirection);
        if (eMovement == STRAFE_FORWARD)
        {
            aFacingDirection = pFielder->m_aDesiredMovementDirection;
        }
        pFielder->SetFacingDirection(aFacingDirection, false);
    }
    else
    {
        eMovement = pFielder->m_fDesiredSpeed < 0.1f
                  ? STRAFE_IDLE : STRAFE_FORWARD;
    }
    pFielder->mUnidentified37C = eMovement;
}

extern "C" void fn_800C5DBC(DesireSteering* desire, float fDeltaT)
{
    float fRepulsionMult = desire->m_fAvoidanceMult;
    int nThingsToAvoid = desire->m_ThingsToAvoid;
    cFielder* pFielder = desire->mUnidentifiedFielder;

    if (fn_800DEFD4(pFielder) != 0.0f)
    {
        fRepulsionMult = 0.0f;
    }
    else if (pFielder->GetGlobalPad() != NULL)
    {
        if ((fn_8003E8A0(pFielder) && pFielder->mUnidentified3DC)
            || (fn_8003E9F0(pFielder) && !pFielder->mUnidentified3DC))
        {
            nThingsToAvoid = AVOID_NOTHING;
        }
        else if (GameInfoManager::Instance()->GetStadium() == 0x0B
            || GameInfoManager::Instance()->IsRule0x4Equal3())
        {
            nThingsToAvoid = AVOID_UNIDENTIFIED_40;
            if (fn_8001E160(pFielder) && !lbl_806E0E58)
            {
                nThingsToAvoid = AVOID_NOTHING;
            }
        }
        else
        {
            fRepulsionMult = 0.0f;
            nThingsToAvoid = AVOID_UNIDENTIFIED_40;
        }
    }
    else if (fn_800DED80(pFielder) != 0.0f)
    {
        fRepulsionMult = 1.0f;
    }

    if (pFielder->GetGlobalPad() == NULL && fn_8003EA6C(pFielder))
    {
        nThingsToAvoid &= ~(AVOID_FIELDERS | AVOID_GOALIES);
    }
    if (fn_8003E948(pFielder) && pFielder->mUnidentified3DC)
    {
        nThingsToAvoid = AVOID_NOTHING;
    }

    desire->m_pAvoidance->SetThingsToAvoid(nThingsToAvoid);
    desire->m_pAvoidance->m_fRepulsionMult = fRepulsionMult;
    desire->m_pAvoidance->Update(fDeltaT);
    desire->m_ThingsToAvoid = AVOID_EVERYTHING;
    desire->m_fAvoidanceMult = 1.0f;
}

void DesireSteering::UnidentifiedUpdate(
    UnidentifiedDesireUpdate*, float fDeltaT)
{
    fn_8003E948(mUnidentifiedFielder);
    bool bUseAvoidance = !lbl_806E0C50;
    if (bUseAvoidance && lbl_806E0C51
        && mUnidentifiedFielder->m_pTeam->m_nSide == HOME)
    {
        bUseAvoidance = false;
    }
    if (bUseAvoidance && lbl_806E0C52
        && mUnidentifiedFielder->m_pTeam->m_nSide == AWAY)
    {
        bUseAvoidance = false;
    }

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

    if (m_fUrgency != 0.0f)
    {
        nlVector3 v3UnfilteredDesired = m_v3LastDesiredPos;
        if (m_fTotalWeight > 0.0f)
        {
            float scale = 1.0f / m_fTotalWeight;
            m_fUrgency *= scale;
            m_v3DesiredPos.x *= scale;
            m_v3DesiredPos.y *= scale;
            m_v3DesiredPos.z *= scale;
            m_fTotalWeight = 0.0f;
        }

        v3UnfilteredDesired = m_v3DesiredPos;
        UnidentifiedAvoidanceHistory& history = m_AvoidanceHistory;
        int index = history.mUnidentified00C;
        history.mUnidentified014[index] = m_v3DesiredPos;
        history.mUnidentified018[index] = fDeltaT;
        history.mUnidentified00C = (index + 1) % history.mUnidentified008;
        history.mUnidentified01C.x += fDeltaT * m_v3DesiredPos.x;
        history.mUnidentified01C.y += fDeltaT * m_v3DesiredPos.y;
        history.mUnidentified01C.z += fDeltaT * m_v3DesiredPos.z;
        history.mUnidentified028 += fDeltaT;

        if (history.mUnidentified00C == history.mUnidentified010)
        {
            int oldest = history.mUnidentified010;
            history.mUnidentified01C.x -= history.mUnidentified018[oldest]
                                              * history.mUnidentified014[oldest].x;
            history.mUnidentified01C.y -= history.mUnidentified018[oldest]
                                              * history.mUnidentified014[oldest].y;
            history.mUnidentified01C.z -= history.mUnidentified018[oldest]
                                              * history.mUnidentified014[oldest].z;
            history.mUnidentified028 -= history.mUnidentified018[oldest];
            history.mUnidentified010
                = (oldest + 1) % history.mUnidentified008;
        }

        while (history.mUnidentified00C != history.mUnidentified010)
        {
            int oldest = history.mUnidentified010;
            if (history.mUnidentified028
                    - history.mUnidentified018[oldest]
                <= history.mUnidentified004)
            {
                break;
            }
            history.mUnidentified01C.x -= history.mUnidentified018[oldest]
                                              * history.mUnidentified014[oldest].x;
            history.mUnidentified01C.y -= history.mUnidentified018[oldest]
                                              * history.mUnidentified014[oldest].y;
            history.mUnidentified01C.z -= history.mUnidentified018[oldest]
                                              * history.mUnidentified014[oldest].z;
            history.mUnidentified028 -= history.mUnidentified018[oldest];
            history.mUnidentified010
                = (oldest + 1) % history.mUnidentified008;
        }

        if (history.mUnidentified00C != history.mUnidentified010)
        {
            history.UnidentifiedGetValue(m_v3DesiredPos,
                history.mUnidentified028, history.mUnidentified01C);
        }
        else
        {
            m_v3DesiredPos = history.mUnidentified02C;
        }

        fn_800C66A4(this, m_v3DesiredPos,
            TR_FAR_DISTANCE, fDeltaT, m_fUrgency);
        m_v3LastDesiredPos = v3UnfilteredDesired;
    }
    else
    {
        fn_800C574C(this);
    }

    nlPolar desiredVelocity;
    desiredVelocity.a = mUnidentifiedFielder->m_aDesiredMovementDirection;
    desiredVelocity.r = mUnidentifiedFielder->m_fDesiredSpeed;
    nlPolarToCartesian(m_v3DesiredVel, desiredVelocity);
    m_v3DesiredVel.z = 0.0f;

    if (mUnidentifiedFielder->m_eActionState == ACTION_WAIT
        || mUnidentifiedFielder->m_eActionState == ACTION_NEED_ACTION)
    {
        fn_80040368(mUnidentifiedFielder);
    }
    if (sUseAvoidance && bUseAvoidance)
    {
        fn_800C5DBC(this, fDeltaT);
    }
    fn_800C6FDC(this, fDeltaT);
    fn_8003C7B0(mUnidentifiedFielder);

    desiredVelocity.a = mUnidentifiedFielder->m_aDesiredMovementDirection;
    desiredVelocity.r = mUnidentifiedFielder->m_fDesiredSpeed;
    nlPolarToCartesian(m_v3DesiredVel, desiredVelocity);
    m_v3DesiredVel.z = 0.0f;
}

void DesireSteering::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireSteering");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&m_ePositionSeekState - (u8*)&mvDesiredPosition,
        "m_ePositionSeekState");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&m_v3DesiredPos - (u8*)&mvDesiredPosition,
        "m_v3DesiredPos");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fDesiredFacingDirection - (u8*)&mvDesiredPosition,
        "m_fDesiredFacingDirection");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fFacingTotalWeight - (u8*)&mvDesiredPosition,
        "m_fFacingTotalWeight");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&m_v3LastDesiredPos - (u8*)&mvDesiredPosition,
        "m_v3LastDesiredPos");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&m_v3DesiredVel - (u8*)&mvDesiredPosition,
        "m_v3DesiredVel");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&m_v3TempDesiredPos - (u8*)&mvDesiredPosition,
        "m_v3TempDesiredPos");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fTotalWeight - (u8*)&mvDesiredPosition,
        "m_fTotalWeight");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fUrgency - (u8*)&mvDesiredPosition,
        "m_fUrgency");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fDesiredArrivalTime - (u8*)&mvDesiredPosition,
        "m_fDesiredArrivalTime");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fForcedArrivalRadius - (u8*)&mvDesiredPosition,
        "m_fForcedArrivalRadius");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&m_fAvoidanceMult - (u8*)&mvDesiredPosition,
        "m_fAvoidanceMult");
    fn_80338F88(cache, 8, lbl_80533C98[8].size,
        (u8*)&m_ThingsToAvoid - (u8*)&mvDesiredPosition,
        "m_ThingsToAvoid");
    fn_80338F78(cache);
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
    fn_80339450(cache, sDesireSteeringType, data, context);
    fn_8033930C(cache, sDesireSteeringType, data,
        sizeof(DesireSteering) - offset);
}

bool UnidentifiedDesire35::UnidentifiedInitialize(void*)
{
    mUnidentified078 = 10.0f;
    fn_8006040C(g_pGame, mUnidentifiedFielder);
    mUnidentifiedFielder->mUnidentified3F8
        = mUnidentifiedFielder->mUnidentified3FC;
    return true;
}

void UnidentifiedDesire35::UnidentifiedUpdate(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    cFielder* pFielder = mUnidentifiedFielder;
    if (!pFielder->mUnidentified3DC)
    {
        return;
    }
    if (!fn_8002EDC8(pFielder, -1))
    {
        pFielder->fn_8005001C(true);
        fn_80060804(g_pGame, pFielder);
        return;
    }

    float fSpeed = pFielder->m_pBall != NULL
                 ? fn_8002C328(fn_8003E6E4(pFielder))
                 : fn_8002C254(fn_8003E6E4(pFielder));
    pFielder->m_fDesiredSpeed = fSpeed;
    pFielder->mUnidentified3E0 -= fDeltaT;
    if (pFielder->mUnidentified3E0 <= 0.0f)
    {
        *update = FuzzyVariant(FT_INT, 1);
        update->mTemporary = false;
        return;
    }

    pFielder->mUnidentified3F8 -= fDeltaT;
    short nFacingDelta = (short)(pFielder->m_aActualFacingDirection
        - pFielder->m_aDesiredFacingDirection);
    if (pFielder->mUnidentified3F8 <= 0.0f)
    {
        if (pFielder->mUnidentified3DD)
        {
            pFielder->fn_8005001C(true);
            return;
        }

        int nAbsFacingDelta
            = nFacingDelta < 0 ? -nFacingDelta : nFacingDelta;
        if ((unsigned short)nAbsFacingDelta > 0x2000)
        {
            fn_80060608(g_pGame, pFielder);
            if (pFielder->mUnidentified3E0 > 0.0f
                && pFielder->mUnidentified3E0
                    < pFielder->mUnidentified3FC)
            {
                pFielder->mUnidentified3E0
                    = pFielder->mUnidentified3FC;
            }
            if (nFacingDelta < 0)
            {
                pFielder->SetFacingDirection(
                    pFielder->m_aActualFacingDirection + 0x4000, true);
            }
            else
            {
                pFielder->SetFacingDirection(
                    pFielder->m_aActualFacingDirection - 0x4000, true);
            }
            fn_8006040C(g_pGame, pFielder);
            pFielder->mUnidentified3F8 = pFielder->mUnidentified3FC;
        }
    }

    fn_8001DCE0(pFielder, pFielder->m_aActualFacingDirection);
    pFielder->m_aActualMovementDirection
        = pFielder->m_aActualFacingDirection;
    fn_8001E304(pFielder, fSpeed, fDeltaT);
}

void UnidentifiedDesire35::UnidentifiedCleanup()
{
    fn_80060608(g_pGame, mUnidentifiedFielder);
}

UnidentifiedDesire35::~UnidentifiedDesire35()
{
}
