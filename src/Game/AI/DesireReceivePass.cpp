#include "Game/AI/DesireReceivePass.h"
#include "Game/DetInput.h"
#include "Game/Sys/debug.h"

#include "Game/AI/DesireSteering.h"

#include <stddef.h>

#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Variant.inl"
#include "Game/AI/FuzzyRuntimeCall.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/AnimInventory.h"
#include "Game/AI/Fielder.inl"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/InterpreterCore.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/PassBallData.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Player.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Team.h"
#include "Game/CharacterTweaks.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/AI/AvoidableObject.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <math.h>
#include "Game/CharacterTriggers.h"

static const LooseBallContactAnimInfo sReceiveContactAnims[5] = {
    { 0x29, 3.0f, 0x0000, 0xFFFF },
    { 0x2B, 7.0f, 0xE000, 0x2000 },
    { 0x2C, 7.0f, 0xA000, 0xE000 },
    { 0x2E, 7.0f, 0x6000, 0xA000 },
    { 0x2D, 7.0f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo sVolleyReceiveContactAnims[4] = {
    { 0x2A, 9.0f, 0x0000, 0xFFFF },
    { 0x2F, 10.0f, 0xE000, 0x2000 },
    { 0x33, 9.0f, 0x2000, 0x8000 },
    { 0x31, 9.0f, 0x8000, 0xE000 },
};

static const LooseBallContactAnimInfo sVolleyOneTouchShotContactAnims[8] = {
    { 0x3C, 10.0f, 0xE000, 0x2000 },
    { 0x3D, 9.0f, 0xA000, 0xE000 },
    { 0x3F, 9.0f, 0x6000, 0xA000 },
    { 0x3E, 9.0f, 0x2000, 0x6000 },
    { 0x40, 10.0f, 0xE000, 0x2000 },
    { 0x41, 10.0f, 0xA000, 0xE000 },
    { 0x43, 10.0f, 0x6000, 0xA000 },
    { 0x42, 9.5f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo sOneTouchShotContactAnims[8] = {
    { 0x34, 7.0f, 0xE000, 0x2000 },
    { 0x35, 7.0f, 0xA000, 0xE000 },
    { 0x37, 7.0f, 0x6000, 0xA000 },
    { 0x36, 7.0f, 0x2000, 0x6000 },
    { 0x38, 9.0f, 0xE000, 0x2000 },
    { 0x39, 9.0f, 0xA000, 0xE000 },
    { 0x3B, 9.0f, 0x6000, 0xA000 },
    { 0x3A, 9.0f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo sVolleyOneTouchContactAnims[4] = {
    { 0x44, 4.0f, 0xE000, 0x2000 },
    { 0x45, 4.0f, 0xA000, 0xE000 },
    { 0x47, 4.0f, 0x6000, 0xA000 },
    { 0x46, 4.0f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo sSpecialVolleyContactAnims[2] = {
    { 0x32, 4.0f, 0x8000, 0xE000 },
    { 0x30, 4.0f, 0xE000, 0x2000 },
};

extern "C" void fn_8003BA94(cFielder*, float);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" bool fn_80035F84(cFielder*, nlVector3*, float*, nlVector3*,
    float*, unsigned short, const LooseBallContactAnimInfo*);
extern "C" bool fn_80036234(cFielder*, nlVector3*, float*, nlVector3*,
    float*, const LooseBallContactAnimInfo*, nlVector3*, unsigned short);
extern "C" void fn_80015B38(cBall*, bool);
extern "C" void fn_80016DF8(
    cBall*, cPlayer*, nlVector3*, int, bool, bool);
extern "C" void fn_8003EBD0(
    cFielder*, int, UnidentifiedVariantCollection*);
extern "C" float fn_8002CE14(PlayerTweaks*);
extern "C" void fn_8005C650(cGame*);
static float sfReceivePassMaxDuration = 5.0f;
unsigned short DesireReceivePass::sDesireReceivePassType = 0xFFFF;
bool g_bFindReceivePassPosition = true;
float g_fOneTimerMinPassProgress = 0.66f;
float g_fMaxVolleyPassUpSpeed = 30.0f;
float g_fPasserNoPickUpTime = 0.5f;
float g_fGroundPassMaxLift = 3.0f;
float g_fGroundPassMinLift = 0.5f;
float g_fPassDistanceMin = 0.1f;
float g_fPassDistanceMax = 12.5f;
float g_fOneTouchMinPassProgress = 0.1f;
float g_fOneTouchMinDesireAge = 0.1f;
float g_fAnimStartThresholdTicks = 1.0f;
float g_fAnimStartPositionTolerance = 1.9f;
float g_fReceiveArrivalRadius = 0.1f;
float g_fReceiveArrivalRadiusLarge = 0.4f;
float g_fAirContactHeightScale = 0.5f;
float g_fMinContactAnimSpeed = 0.5f;
float g_fMaxContactAnimSpeed = 2.0f;
float g_fBallApproachMinDot = 0.93f;
float g_fBallApproachMaxSpeed = 5.0f;
float g_fMaxSteeringAvoidance = 0.55f;
float sfSpeedAdjustChargeLevel1 = 0.02f;
float sfSpeedAdjustChargeLevel2 = 0.02f;
float sfSpeedAdjustChargeLevel3 = 0.02f;
float sfSpeedAdjustChargeLevelMax = 0.02f;
float g_fSpeedAdjustChargeTotal = sfSpeedAdjustChargeLevel1 + sfSpeedAdjustChargeLevel2
    + sfSpeedAdjustChargeLevel3 + sfSpeedAdjustChargeLevelMax;
static TweakFloatBinding sSpeedAdjustChargeLevel1Binding("sfSpeedAdjustChargeLevel1",
    "Game/Gameplay/Charging/Pass", &sfSpeedAdjustChargeLevel1, true);
static TweakFloatBinding sSpeedAdjustChargeLevel2Binding("sfSpeedAdjustChargeLevel2",
    "Game/Gameplay/Charging/Pass", &sfSpeedAdjustChargeLevel2, true);
static TweakFloatBinding sSpeedAdjustChargeLevel3Binding("sfSpeedAdjustChargeLevel3",
    "Game/Gameplay/Charging/Pass", &sfSpeedAdjustChargeLevel3, true);
static TweakFloatBinding sSpeedAdjustChargeLevelMaxBinding("sfSpeedAdjustChargeLevelMax",
    "Game/Gameplay/Charging/Pass", &sfSpeedAdjustChargeLevelMax, true);


static inline float DoCalculatePassSpeed(const nlVector2& distance,
    float passSpeedMin, float passSpeedMax,
    float passDistMin, float passDistMax)
{
    float fPassCharge = NormalizeVal(
                            nlVec2Length(distance), passDistMin, passDistMax)
        - g_fSpeedAdjustChargeTotal;
    if (GetBallChargeValue(g_pBall, 0) > 1.0f)
    {
        fPassCharge += sfSpeedAdjustChargeLevel1;
    }
    if (GetBallChargeValue(g_pBall, 0) > 2.0f)
    {
        fPassCharge += sfSpeedAdjustChargeLevel2;
    }
    if (GetBallChargeValue(g_pBall, 0) > 3.0f)
    {
        fPassCharge += sfSpeedAdjustChargeLevel3;
    }
    if (GetBallChargeValue(g_pBall, 0) >= 4.0f)
    {
        fPassCharge += sfSpeedAdjustChargeLevelMax;
    }
    fPassCharge = nlMaxEquals(fPassCharge, 0.0f);
    fPassCharge = nlMinEquals(fPassCharge, 1.0f);
    return Interpolate(passSpeedMin, passSpeedMax, fPassCharge);
}

DesireReceivePass::DesireReceivePass()
    : Desire(22, ScriptTransitionFunc("TransDesireReceivePass"))
{
    mEstimated.Reset();
}

bool DesireReceivePass::Initialize(void* context)
{
    Desire::Initialize(context);

    DesireSteering* desire = (DesireSteering*)fn_8002E08C(
        m_pFielder, 34);
    ResetSteeringTargets(desire);

    mEstimated.Reset();
    mbOneTouchVolley = false;
    mbOneTouchShot = false;
    mbOneTouchShotLate = false;
    mbOneTouchPass = false;
    mpOneTouchPassTarget = 0;

    mfInitialBallSpeed = nlSqrt(
        g_pBall->m_v3Velocity.x * g_pBall->m_v3Velocity.x
            + g_pBall->m_v3Velocity.y * g_pBall->m_v3Velocity.y,
        true);

    UnidentifiedVariantCollection* params =
        (UnidentifiedVariantCollection*)context;
    meReceiveAnimType = params->Get(11)->fn_800C2BD4();
    mbValidPassIntercept = params->IsSet(14);
    if (mbValidPassIntercept)
    {
        mv3PassIntercept = params->Get(14)->mData.vector;
    }

    bool result = CalcRoughEstimates(meReceiveAnimType);
    if (!result)
    {
        return result;
    }

    SelectContactAnimation();

    float fMaxDistanceSq = g_fReceiveArrivalRadius * g_fReceiveArrivalRadius;

    nlVector2 v2Delta = {
        mEstimated.v3AnimStartPos.x
            - m_pFielder->mUnidentified024.m_v3Position.x,
        mEstimated.v3AnimStartPos.y
            - m_pFielder->mUnidentified024.m_v3Position.y,
    };
    if (nlVec2LengthSquared(v2Delta) > fMaxDistanceSq)
    {
        meDesireSubState = 0;
        m_pFielder->InitActionRunning();
        fn_8003BA94(m_pFielder, 0.1f);
    }
    else
    {
        result = CalcExactEstimates(true);
        if (result)
        {
            meDesireSubState = 4;
            result = StartPickupAnimation();
        }
    }

    if (result)
    {
        m_pFielder->SetNoPickUpTime(sfReceivePassMaxDuration);
        mMaxDuration = sfReceivePassMaxDuration;
    }
    return result;
}

void DesireReceivePass::Update(DesireUpdate* update, float fDeltaT)
{
    if (update->fn_800C2BD4() == 3)
    {
        bool bUnhandled = false;
        switch (update->GetParameter(8)->fn_800C2BD4())
        {
        case 15:
            *update = 0;
            RequestOneTouchShot(update->GetParameters()->Get(16)->fn_800C2BF8());
            SetPassTransitionTimer();
            break;
        case 14:
        {
            *update = 0;
            cPlayer* pPassTarget = update->GetParameters()->Get(14)->GetPlayer();
            bool bVolleyPass = false;
            if (update->IsParameterSet(16))
            {
                bVolleyPass = update->GetParameters()->Get(16)->fn_800C2BF8();
            }
            RequestOneTouchPass(bVolleyPass, pPassTarget);
            SetPassTransitionTimer();
            break;
        }
        default:
            bUnhandled = true;
            break;
        }
        if (bUnhandled)
        {
            return;
        }
    }

    if (meDesireSubState != 4
        && (!g_pBall->HasActivePassTarget()
            || g_pBall->GetPassTarget() != m_pFielder
            || !m_pFielder->CanReceivePass()))
    {
        *update = 1;
        return;
    }

    mvDesiredPosition = mEstimated.v3AnimStartPos;
    ProcessUserInput();
    if (GetScriptMachine()->fn_800C2F20() != this)
    {
        *update = 1;
        return;
    }

    if (g_pBall->GetPassTarget() == m_pFielder
        && g_pBall->GetPassProgress() > g_fOneTimerMinPassProgress
        && AIsgn(g_pBall->GetPassIntercept().x)
            == AIsgn(m_pFielder->GetTeam()->GetOtherNet()->GetNetLocation().x)
        && mbOneTouchShot && !mbOneTouchVolley)
    {
        fn_8005C650(g_pGame);
    }

    if (mEstimated.bLocked)
    {
        mEstimated.fAnimStartTime -= fDeltaT;
    }
    float fStartThreshold = g_fAnimStartThresholdTicks * g_fSimulationTick;
    switch (meDesireSubState)
    {
    case 0:
    {
        if (!CalcRoughEstimates(meReceiveAnimType))
        {
            *update = 1;
            return;
        }
        SelectContactAnimation();
        if (mEstimated.fAnimStartTime <= fStartThreshold)
        {
            if (!CalcExactEstimates(true) || !StartPickupAnimation())
            {
                *update = 1;
            }
            meDesireSubState = 4;
            return;
        }

        float fArrivalRadius = g_fReceiveArrivalRadius;
        if (!fn_800C0E74())
        {
            fArrivalRadius = g_fReceiveArrivalRadiusLarge;
        }
        DesireSteering* pSteering = (DesireSteering*)fn_8002E08C(m_pFielder, 34);
        SetTimedSteeringTarget(pSteering, mEstimated.v3AnimStartPos,
            mEstimated.aFacingDirection, mEstimated.fAnimStartTime, fArrivalRadius);
        pSteering->SetAvoidanceMultiplier(InterpolateClamped(g_fMaxSteeringAvoidance, 0.0f, g_pBall->GetPassProgress()));
        if (m_pFielder->GetDistanceToDesiredPos() <= fArrivalRadius)
        {
            if (!CalcExactEstimates(true))
            {
                *update = 1;
                return;
            }
            if (mEstimated.fAnimStartTime <= fStartThreshold)
            {
                meDesireSubState = 4;
                if (!StartPickupAnimation())
                {
                    *update = 1;
                }
            }
            else
            {
                meDesireSubState = 2;
                m_pFielder->InitActionIdleTurn(mEstimated.aFacingDirection);
            }
        }
        break;
    }
    case 1:
        if (mEstimated.fAnimStartTime <= fStartThreshold)
        {
            meDesireSubState = 4;
            if (!StartPickupAnimation())
            {
                *update = 1;
            }
        }
        else
        {
            DesireSteering* pSteering = (DesireSteering*)fn_8002E08C(m_pFielder, 34);
            SetTimedSteeringTarget(pSteering, mEstimated.v3AnimStartPos,
                mEstimated.aFacingDirection, mEstimated.fAnimStartTime, g_fReceiveArrivalRadius);
        }
        break;
    case 2:
        if (m_pFielder->IsActionDone())
        {
            meDesireSubState = 3;
            m_pFielder->InitActionWait();
        }
        else if (mEstimated.fAnimStartTime <= fStartThreshold)
        {
            meDesireSubState = 4;
            if (!StartPickupAnimation())
            {
                *update = 1;
            }
        }
        break;
    case 3:
        if (mEstimated.fAnimStartTime <= fStartThreshold)
        {
            meDesireSubState = 4;
            if (!StartPickupAnimation())
            {
                *update = 1;
            }
        }
        break;
    case 4:
        if (m_pFielder->fn_800C2F40() != 0)
        {
            if (m_pFielder->GetGlobalPad() != 0)
            {
                mbOneTouchVolley = m_pFielder->IsActionModifierPressed();
            }
            if (mbOneTouchPass)
            {
                if (meReceiveAnimType & 4)
                {
                    float fMinPassSpeed = GetSlowestVolleyPassSpeed(m_pFielder->GetTweaks());
                    float fMaxPassSpeed = GetFastestVolleyPassSpeed(m_pFielder->GetTweaks());
                    if (!mbOneTouchVolley)
                    {
                        fMinPassSpeed = GetSlowestGroundPassSpeed(m_pFielder->GetTweaks());
                        fMaxPassSpeed = GetFastestGroundPassSpeed(m_pFielder->GetTweaks());
                    }
                    m_pFielder->DoRegularPassing(mpOneTouchPassTarget,
                        mbOneTouchVolley, true, false, false, fMinPassSpeed, fMaxPassSpeed);
                }
                else if (fn_800C0E54())
                {
                    if (fabsf(mEstimated.fReceivePassAnimTime
                            - m_pFielder->GetCurrentAnimController()->get_fTime()) <= 0.06666667f)
                    {
                        m_pFielder->InitActionOneTouchPassFromVolley(mpOneTouchPassTarget, mbOneTouchVolley);
                    }
                    else if (m_pFielder->IsActionDone())
                    {
                        m_pFielder->InitActionPass(mpOneTouchPassTarget, mbOneTouchVolley, 0, true);
                    }
                }
                else
                {
                    m_pFielder->InitActionPass(mpOneTouchPassTarget, mbOneTouchVolley, 0, true);
                }
            }
            else if (mbOneTouchShotLate)
            {
                if (meReceiveAnimType & 4)
                {
                    m_pFielder->InitActionShot(mbOneTouchVolley, true);
                }
                else if (fn_800C0E54())
                {
                    if (fabsf(mEstimated.fReceivePassAnimTime
                            - m_pFielder->GetCurrentAnimController()->get_fTime()) <= 0.06666667f)
                    {
                        m_pFielder->InitActionLateOneTimerFromVolley();
                    }
                    else if (m_pFielder->IsActionDone())
                    {
                        if (m_pFielder->GetGlobalPad() != 0
                            && m_pFielder->GetGlobalPad()->IsPressed(0x1C, true))
                        {
                            if (m_pFielder->ShouldStartCrossBlend(0x14))
                            {
                                *update = 1;
                            }
                        }
                        else
                        {
                            m_pFielder->InitActionShot(mbOneTouchVolley, true);
                        }
                    }
                }
                else if (m_pFielder->GetGlobalPad() != 0
                    && m_pFielder->GetGlobalPad()->IsPressed(0x1C, true))
                {
                    if (m_pFielder->ShouldStartCrossBlend(0x14))
                    {
                        *update = 1;
                    }
                }
                else
                {
                    m_pFielder->InitActionShot(mbOneTouchVolley, true);
                }
            }
            else if (!mbOneTouchShot && IsGroundReceive())
            {
                if (!fn_800C0E74() || m_pFielder->ShouldStartCrossBlend(0x14))
                {
                    m_pFielder->InitActionRunningWB(true);
                    *update = 1;
                }
            }
        }
        if (m_pFielder->IsActionDone())
        {
            *update = 1;
        }
        break;
    }
}

inline int DesireReceivePass::AddVolleyReceiveFlag(int animType, bool bFlag)
{
    if (bFlag)
    {
        animType |= 1;
    }
    return animType;
}

inline bool DesireReceivePass::CanRequestOneTouch()
{
    float fPassProgress = g_pBall->GetPassProgress();
    bool bSpecialReceive = m_pFielder->fn_8003E7F8()
        || m_pFielder->fn_8003E84C();
    if ((bSpecialReceive && m_pFielder->m_pBall != 0)
        || (fPassProgress < g_fOneTouchMinPassProgress
            && mAgeTimer.GetSeconds() < g_fOneTouchMinDesireAge)
        || (meDesireSubState == 4 && (mbOneTouchShot || mbOneTouchPass)))
    {
        return false;
    }
    return true;
}

void DesireReceivePass::ProcessUserInput()
{
    if (m_pFielder->GetGlobalPad() != 0)
    {
        fn_80098098(m_pFielder);
        if (m_pFielder->GetGlobalPad()->JustPressed(0x1C, true))
        {
            RequestOneTouchShot(m_pFielder->IsActionModifierPressed());
        }
        else if (m_pFielder->GetGlobalPad()->JustPressed(0x1B, true))
        {
            RequestOneTouchPass(
                m_pFielder->IsActionModifierPressed(), 0);
        }

        if (m_pFielder->m_pBall != 0)
        {
            PlayerTweaks* pTweaks = m_pFielder->GetTweaks();
            float fMaxSpeed = pTweaks->GetRunningSpeed();
            float fMinSpeed = fn_8002CE14(
                m_pFielder->GetTweaks());
            fn_8003C268(m_pFielder, fMinSpeed, fMaxSpeed);
            return;
        }

        if (meDesireSubState != 4)
        {
            unsigned short aDirection = 0;
            if (fn_80036A58(m_pFielder, &aDirection))
            {
                unsigned short aHitDirection =
                    m_pFielder->mUnidentified024.m_aActualFacingDirection;
                if (m_pFielder->m_pController != 0
                    && m_pFielder->m_pController->GetMovementStickMagnitude() > 0.001f)
                {
                    aHitDirection =
                        m_pFielder->m_pController->GetMovementStickDirection();
                }
                m_pFielder->InitActionHit(0, aHitDirection);
                return;
            }

            if (fn_80036C8C(m_pFielder, &aDirection))
            {
                m_pFielder->InitActionSlideAttack(
                    0, -1.0f, aDirection);
                RequestStateMachineDeactivation(this);
            }
        }
    }
}

void DesireReceivePass::RequestOneTouchShot(bool bVolleyPass)
{
    if (!CanRequestOneTouch())
    {
        return;
    }

    mbOneTouchPass = false;
    mbOneTouchShot = true;
    mbOneTouchVolley = bVolleyPass;
    bool bSpecialReceive =
        m_pFielder->fn_8003E7F8()
        || m_pFielder->fn_8003E84C();
    if (meDesireSubState == 4 && !bSpecialReceive)
    {
        mbOneTouchShotLate = true;
        return;
    }

    if (meDesireSubState != 0)
    {
        meDesireSubState = 0;
        mEstimated.bLocked = false;
    }

    int eReceiveAnimType = meReceiveAnimType;
    if (fn_800C0E54()
        && (mbOneTouchVolley || bSpecialReceive))
    {
        int eOneTouchReceiveAnimType = AddVolleyReceiveFlag(4, fn_800C0E54());
        float fBallContactTime =
            mEstimated.fBallContactTime;
        mEstimated.fBallContactTime = -1.0f;
        if (bSpecialReceive
            || CalcRoughEstimates(
                eOneTouchReceiveAnimType))
        {
            meReceiveAnimType =
                eOneTouchReceiveAnimType;
            return;
        }
        mEstimated.fBallContactTime =
            fBallContactTime;
        return;
    }

    eReceiveAnimType = AddVolleyReceiveFlag(8, fn_800C0E54());
    meReceiveAnimType = eReceiveAnimType;
}

void DesireReceivePass::RequestOneTouchPass(bool bVolleyPass, cPlayer* pPassTarget)
{
    if (!CanRequestOneTouch())
    {
        return;
    }

    mbOneTouchShot = false;
    mbOneTouchShotLate = false;
    mbOneTouchVolley = false;
    if (pPassTarget == 0)
    {
        pPassTarget = fn_80096F54(
            m_pFielder, false);
    }
    if (pPassTarget == 0)
    {
        RequestOneTouchShot(bVolleyPass);
        return;
    }

    mbOneTouchVolley = bVolleyPass;
    mbOneTouchPass = true;
    mpOneTouchPassTarget = pPassTarget;
    bool bSpecialReceive =
        m_pFielder->fn_8003E7F8()
        || m_pFielder->fn_8003E84C();
    if (meDesireSubState == 4 && !bSpecialReceive)
    {
        return;
    }

    if (meDesireSubState != 0)
    {
        meDesireSubState = 0;
        mEstimated.bLocked = false;
    }

    if (fn_800C0E54()
        && (mbOneTouchVolley || bSpecialReceive))
    {
        int eOneTouchReceiveAnimType = AddVolleyReceiveFlag(4, fn_800C0E54());
        float fBallContactTime =
            mEstimated.fBallContactTime;
        mEstimated.fBallContactTime = -1.0f;
        if (bSpecialReceive
            || CalcRoughEstimates(
                eOneTouchReceiveAnimType))
        {
            meReceiveAnimType =
                eOneTouchReceiveAnimType;
            return;
        }
        mEstimated.fBallContactTime =
            fBallContactTime;
        return;
    }

    int eReceiveAnimType = AddVolleyReceiveFlag(2, fn_800C0E54());
    meReceiveAnimType = eReceiveAnimType;
}

void DesireReceivePass::Cleanup()
{
    if (m_pFielder->m_pBall == 0)
    {
        m_pFielder->ClearPassTargetIfAmThePassTarget();
    }

    mEstimated.Reset();

    if (m_pSpaceSearch == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    m_pSpaceSearch = 0;

    if (mbOneTouchShot)
    {
        m_pFielder->SetNoPickUpTime(0.2f);
    }
    else
    {
        m_pFielder->SetNoPickUpTime(0.0f);
    }

    DesireSteering* desire = (DesireSteering*)fn_8002E08C(
        m_pFielder, 34);
    ResetSteeringHistory(desire);
    ResetSteeringAvoidance(desire);
}

bool DesireReceivePass::fn_800C0E54()
{
    return (meReceiveAnimType & 1) || (meReceiveAnimType & 0x10);
}

bool DesireReceivePass::fn_800C0E74()
{
    bool result = true;
    switch (mEstimated.nReceivePassAnim)
    {
    case 41:
    case 42:
    case 52:
    case 53:
    case 54:
    case 55:
    case 60:
    case 61:
    case 62:
    case 63:
        result = false;
        break;
    case 0:
    {
        nlVector2 v2Delta = {
            m_pFielder->mUnidentified024.m_v3Position.x - mv3PassIntercept.x,
            m_pFielder->mUnidentified024.m_v3Position.y - mv3PassIntercept.y,
        };
        result = nlVec2LengthSquared(v2Delta) > g_fReceiveArrivalRadius;
        break;
    }
    }
    return result;
}

void DesireReceivePass::SetPassTransitionTimer()
{
    float fDuration = 0.5f + g_pBall->m_tPassTargetTimer.GetSeconds();
    const TransitionFunc& transition =
        !mOverrideTransition.IsUnset() ? mOverrideTransition : mDefaultTransition;
    AIContext* input = mScriptMachine->mAIContext;
    input->SetTimer(input->GetTimerKey(transition.mFuncHash, 1), fDuration);
}

float DesireReceivePass::GetBallContactHeight(int receiveAnimType)
{
    int nNumAnims;
    const LooseBallContactAnimInfo* pAnimInfo = GetContactAnimInfoList(receiveAnimType, nNumAnims);
    nlVector3 v3ContactOffsetWorld;
    m_pFielder->GetReceivePassBallContactOffset(v3ContactOffsetWorld,
        m_pFielder->mUnidentified024.m_aActualFacingDirection, pAnimInfo);
    return v3ContactOffsetWorld.z;
}

bool DesireReceivePass::CalcRoughEstimates(int receiveAnimType)
{
    if (mEstimated.bLocked)
    {
        return false;
    }

    Estimated estimated = mEstimated;
    bool bUseGroundIntercept = true;
    int nNumIntercepts;
    float fContactHeight = GetBallContactHeight(receiveAnimType);

    float fInterceptTimes[2];
    float fDesiredScale =
        m_pFielder->mUnidentified024.m_fDesiredPlayerScale;
    if (fContactHeight
        > g_fAirContactHeightScale * fDesiredScale)
    {
        g_pBall->PredictLandingSpotAndTime(estimated.v3BallContactPos,
            &nNumIntercepts, fInterceptTimes,
            fContactHeight);
        bUseGroundIntercept = false;

        if (nNumIntercepts == 2)
        {
            float fClosestDistanceSq = 1.0e17f;
            for (int i = 0; i < 2; ++i)
            {
                nlVector3 v3BallPosition;
                fn_800180F4(
                    g_pBall, &v3BallPosition, fInterceptTimes[i]);
                nlVector2 v2Delta = {
                    v3BallPosition.x - mv3PassIntercept.x,
                    v3BallPosition.y - mv3PassIntercept.y,
                };
                float fDistanceSq = nlVec2LengthSquared(v2Delta);
                if (fDistanceSq < fClosestDistanceSq)
                {
                    fClosestDistanceSq = fDistanceSq;
                    estimated.fBallContactTime = fInterceptTimes[i];
                    estimated.v3BallContactPos = v3BallPosition;
                    estimated.v3BallContactPos.z =
                        fContactHeight;
                }
            }
        }
        else if (nNumIntercepts == 1)
        {
            estimated.fBallContactTime = fInterceptTimes[0];
            fn_800180F4(g_pBall, &estimated.v3BallContactPos,
                estimated.fBallContactTime);
        }
        else
        {
            tDebugPrintManager::Print(DC_AI,
                "DesireReceivePass::CalcRoughEstimates - failed to find an AIR interception point!\n");
            return false;
        }
    }

    if (bUseGroundIntercept)
    {
        cBall* pBall = g_pBall;
        CalcInterceptXY(m_pFielder->GetPosition(),
            fn_8002E1B0(m_pFielder), m_pFielder->mUnidentified320->GetRadius(),
            pBall->GetPosition(), pBall->m_v3Velocity,
            nNumIntercepts, fInterceptTimes);

        if (nNumIntercepts == 0)
        {
            tDebugPrintManager::Print(DC_AI,
                "DesireReceivePass::CalcRoughEstimates - failed to find a GROUND interception point!\n");
            return false;
        }

        float fInterceptTime;
        if (nNumIntercepts == 1)
        {
            fInterceptTime = fInterceptTimes[0];
        }
        else
        {
            fInterceptTime = nlMinEquals(
                fInterceptTimes[0], fInterceptTimes[1]);
        }

        fInterceptTimes[0] = fInterceptTime;
        fInterceptTimes[1] = sfReceivePassMaxDuration;
        nNumIntercepts = 2;

        nlVector3 v3FirstBallPosition;
        fn_800180F4(
            g_pBall, &v3FirstBallPosition, fInterceptTimes[0]);

        if (mbValidPassIntercept)
        {
            nlVector3 v3SecondBallPosition;
            fn_800180F4(g_pBall, &v3SecondBallPosition,
                fInterceptTimes[1]);
            nlVector3 v3ClosestPoint =
                GetClosestPointOnLineABFromPointC(v3FirstBallPosition,
                    v3SecondBallPosition, mv3PassIntercept);

            nlVector3 v3BallDirection;
            nlVec3Sub(v3BallDirection, v3ClosestPoint,
                g_pBall->m_v3Position);
            nlVector3 v3FielderDirection;
            nlVec3Sub(v3FielderDirection, v3ClosestPoint,
                m_pFielder->mUnidentified024.m_v3Position);

            float fDot = 0.0f;
            bool bBallDirectionValid;
            float fLengthSq = v3BallDirection.GetLengthSq3D();
            if (fLengthSq == 0.0f)
            {
                bBallDirectionValid = false;
            }
            else
            {
                nlVec3Scale(v3BallDirection,
                    nlRecipSqrt(fLengthSq, true));
                bBallDirectionValid = true;
            }
            if (bBallDirectionValid)
            {
                bool bFielderDirectionValid;
                fLengthSq = v3FielderDirection.GetLengthSq3D();
                if (fLengthSq == 0.0f)
                {
                    bFielderDirectionValid = false;
                }
                else
                {
                    nlVec3Scale(v3FielderDirection,
                        nlRecipSqrt(fLengthSq, true));
                    bFielderDirectionValid = true;
                }
                if (bFielderDirectionValid)
                {
                    fDot = nlVec3DotProduct(
                        v3BallDirection, v3FielderDirection);
                }
            }

            if (fDot > g_fBallApproachMinDot
                && nlVec3Length(g_pBall->m_v3Velocity)
                    < g_fBallApproachMaxSpeed)
            {
                v3ClosestPoint = GetClosestPointOnLineABFromPointC(
                    v3FirstBallPosition, v3SecondBallPosition,
                    m_pFielder->mUnidentified024.m_v3Position);
            }
            float fBlend = NormalizeVal(
                nlVec2Length(*(nlVector2*)&g_pBall->m_v3Velocity)
                    / mfInitialBallSpeed,
                0.5f, 1.0f);
            nlVecLerp(estimated.v3BallContactPos,
                v3FirstBallPosition, v3ClosestPoint, fBlend);
            goto BallContactPositionReady;
        }
        estimated.v3BallContactPos = v3FirstBallPosition;
    BallContactPositionReady:

        nlVector2 v2BallDelta = {
            estimated.v3BallContactPos.x - g_pBall->m_v3Position.x,
            estimated.v3BallContactPos.y - g_pBall->m_v3Position.y,
        };
        float fBallDistance = nlVec2Length(v2BallDelta);
        float fBallSpeed =
            nlVec2Length(*(nlVector2*)&g_pBall->m_v3Velocity);
        estimated.fBallContactTime = fBallDistance / fBallSpeed;
        estimated.fBallContactTime = nlMaxEquals(
            0.0f, estimated.fBallContactTime);
    }

    if (mEstimated.fBallContactTime > 0.0f
        && (float)fabs(mEstimated.fBallContactTime
            - estimated.fBallContactTime)
            > 0.4f)
    {
        tDebugPrintManager::Print(DC_AI,
            "DesireReceivePass::CalcRoughEstimates - the ball got deflected too much, pass aborted\n");
        return false;
    }

    cField::FixOutOfBoundsPosition(estimated.v3BallContactPos,
        m_pFielder->mUnidentified320->GetRadius(), true);

    nlVector3 v3FacingDirection;
    nlVec3Set(v3FacingDirection,
        estimated.v3BallContactPos.x - m_pFielder->mUnidentified024.m_v3Position.x,
        estimated.v3BallContactPos.y - m_pFielder->mUnidentified024.m_v3Position.y,
        estimated.v3BallContactPos.z - m_pFielder->mUnidentified024.m_v3Position.z);
    if (!fn_800C0E74())
    {
        nlVec3Sub(v3FacingDirection, g_pBall->m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
    }
    estimated.aFacingDirection =
        nlVector3ToAngle(v3FacingDirection);

    g_pBall->SetPassTargetTimer(estimated.fBallContactTime);
    g_pBall->SetPassTarget(m_pFielder,
        estimated.v3BallContactPos, fn_800C0E54());

    mEstimated = estimated;
    return true;
}

bool DesireReceivePass::CalcExactEstimates(bool bLocked)
{
    mEstimated.bLocked = bLocked;

    bool bCollideWithFielders =
        g_pBall->m_pPhysicsBall->mbUseTiltForce;
    bool bCollideWithGoalies =
        g_pBall->m_pPhysicsBall->mbUseWindForce;
    g_pBall->m_pPhysicsBall->mbUseTiltForce = false;
    g_pBall->m_pPhysicsBall->mbUseWindForce = false;

    bool result;
    if (fn_800C0E74())
    {
        result = fn_80036234(m_pFielder,
            &mEstimated.v3AnimStartPos, &mEstimated.fAnimStartTime,
            &mEstimated.v3BallContactPos, &mEstimated.fBallContactTime,
            mEstimated.pAnimInfo,
            &mEstimated.v3BallContactPos,
            mEstimated.aFacingTargetDirection);
    }
    else
    {
        result = fn_80035F84(m_pFielder,
            &mEstimated.v3AnimStartPos, &mEstimated.fAnimStartTime,
            &mEstimated.v3BallContactPos, &mEstimated.fBallContactTime,
            mEstimated.aFacingTargetDirection,
            mEstimated.pAnimInfo);
    }

    g_pBall->m_pPhysicsBall->mbUseTiltForce =
        bCollideWithFielders;
    g_pBall->m_pPhysicsBall->mbUseWindForce =
        bCollideWithGoalies;

    if (result)
    {
        g_pBall->SetPassTargetTimer(mEstimated.fBallContactTime);
        g_pBall->SetPassTarget(m_pFielder,
            mEstimated.v3BallContactPos, fn_800C0E54());
    }
    else
    {
        tDebugPrintManager::Print(DC_AI,
            "DesireReceivePass::CalcExactEstimates - failed to find an interception point!\n");
    }
    return result;
}

void DesireReceivePass::SelectContactAnimation()
{
    nlVector3 v3BallPosition;
    if (mbOneTouchShot)
    {
        v3BallPosition = m_pFielder->m_pTeam
                             ->GetOtherNet()->m_v3NetLocation;

        nlVector3 v3ToTarget;
        nlVec3Sub(v3ToTarget, v3BallPosition,
            m_pFielder->mUnidentified024.m_v3Position);
        mEstimated.aFacingTargetDirection =
            nlVector3ToAngle(v3ToTarget);
        mEstimated.aFacingDirection =
            m_pFielder->mUnidentified024.m_aActualFacingDirection;
    }
    else
    {
        v3BallPosition = g_pBall->m_v3Position;
        if (fn_800C0E74())
        {
            unsigned short aFacingDirection =
                m_pFielder->mUnidentified024.m_aActualFacingDirection;
            mEstimated.aFacingDirection = aFacingDirection;
            mEstimated.aFacingTargetDirection = aFacingDirection;
        }
        else
        {
            mEstimated.aFacingTargetDirection =
                mEstimated.aFacingDirection;
        }
    }

    const LooseBallContactAnimInfo* pBestBallContactAnimInfo =
        FindBestContactAnimInfo(v3BallPosition,
            m_pFielder->mUnidentified024.m_v3Position,
            mEstimated.v3BallContactPos,
            mEstimated.aFacingTargetDirection, meReceiveAnimType);

    if ((meReceiveAnimType & 4) && mbOneTouchPass
        && mpOneTouchPassTarget != 0)
    {
        nlVector3 v3ToTarget;
        nlVec3Sub(v3ToTarget, mpOneTouchPassTarget->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        mEstimated.aFacingTargetDirection =
            nlVector3ToAngle(v3ToTarget);
    }

    switch (pBestBallContactAnimInfo->nAnimID)
    {
    case 0x35:
    case 0x39:
    case 0x3D:
    case 0x41:
    case 0x45:
        mEstimated.aFacingTargetDirection += 0x4000;
        break;
    case 0x36:
    case 0x3A:
    case 0x3E:
    case 0x42:
    case 0x46:
        mEstimated.aFacingTargetDirection -= 0x4000;
        break;
    case 0x37:
    case 0x3B:
    case 0x43:
    case 0x47:
    case 0x49:
        mEstimated.aFacingTargetDirection += 0x8000;
        break;
    case 0x3F:
        mEstimated.aFacingTargetDirection += 0x8000;
        break;
    }

    mEstimated.pAnimInfo = pBestBallContactAnimInfo;
    mEstimated.nReceivePassAnim = pBestBallContactAnimInfo->nAnimID;

    cSAnim* pBestContactAnim = m_pFielder->m_pAnimInventory
                                   ->GetAnim(mEstimated.nReceivePassAnim);
    unsigned short aDesiredFacingDirection =
        mEstimated.aFacingTargetDirection;
    mEstimated.fReceivePassAnimTime =
        GetNormalizedContactTime(
            pBestContactAnim, pBestBallContactAnimInfo->fAnimContactFrame);

    nlVector3 v3ContactOffsetWorld;
    m_pFielder->GetReceivePassBallContactOffset(v3ContactOffsetWorld,
        aDesiredFacingDirection, pBestBallContactAnimInfo);

    nlVec3Sub(mEstimated.v3AnimStartPos,
        mEstimated.v3BallContactPos, v3ContactOffsetWorld);
    mEstimated.v3AnimStartPos.z = 0.0f;
    mEstimated.v3BallContactPos.z = v3ContactOffsetWorld.z;
    mEstimated.fAnimStartOffset =
        nlSqrt(v3ContactOffsetWorld.GetLengthSq3D(), true);

    mEstimated.fAnimStartTime = mEstimated.fBallContactTime
        - GetNormalizedContactTime(
            pBestContactAnim, pBestBallContactAnimInfo->fAnimContactFrame)
            * pBestContactAnim->GetDuration();
}

bool DesireReceivePass::StartPickupAnimation()
{
    nlVector2 v2Delta = {
        mEstimated.v3AnimStartPos.x
            - m_pFielder->mUnidentified024.m_v3Position.x,
        mEstimated.v3AnimStartPos.y
            - m_pFielder->mUnidentified024.m_v3Position.y,
    };
    float fDistance = nlSqrt(nlVec2LengthSquared(v2Delta), true);
    float fRadius = m_pFielder->mUnidentified320->GetRadius();
    if (fDistance - fRadius
        > mEstimated.fAnimStartOffset + g_fAnimStartPositionTolerance)
    {
        tDebugPrintManager::Print(DC_AI,
            "DesireReceivePass::StartPickupAnimation - position is outside max threshold !\n");
        return false;
    }

    short sFacingDelta = (short)(mEstimated.aFacingTargetDirection
        - m_pFielder->mUnidentified024.m_aActualFacingDirection);
    if (mbOneTouchShot && !mbOneTouchShotLate)
    {
        m_pFielder->InitActionOneTimer(
            mEstimated.nReceivePassAnim, mEstimated.v3AnimStartPos,
            mEstimated.fReceivePassAnimTime, mbOneTouchVolley,
            sFacingDelta);
    }
    else
    {
        m_pFielder->InitActionReceivePass(
            mEstimated.nReceivePassAnim, mEstimated.v3AnimStartPos,
            sFacingDelta, mEstimated.fReceivePassAnimTime);
    }

    float fAnimTime =
        mEstimated.pAnimInfo->fAnimContactFrame
        / 30.0f;
    float fTimeToIntercept =
        g_pBall->m_tPassTargetTimer.GetSeconds();
    if (fTimeToIntercept < FixedUpdateTask::GetPhysicsUpdateTick())
    {
        fTimeToIntercept = FixedUpdateTask::GetPhysicsUpdateTick();
    }

    float fPlaybackSpeed = nlMinEquals(
        nlMaxEquals(fAnimTime / fTimeToIntercept, g_fMinContactAnimSpeed),
        g_fMaxContactAnimSpeed);
    m_pFielder->m_pCurrentAnimController
        ->m_fPlaybackSpeedScale = fPlaybackSpeed;
    return true;
}

const LooseBallContactAnimInfo* DesireReceivePass::GetContactAnimInfoList(
    int receiveAnimType, int& nNumAnims)
{
    nNumAnims = 0;
    const LooseBallContactAnimInfo* pAnimInfo = 0;
    switch (receiveAnimType)
    {
    case 2:
        nNumAnims = 5;
        pAnimInfo = sReceiveContactAnims;
        break;
    case 4:
    case 5:
        nNumAnims = 4;
        pAnimInfo = sVolleyOneTouchContactAnims;
        break;
    case 8:
        nNumAnims = 8;
        pAnimInfo = sOneTouchShotContactAnims;
        break;
    case 3:
        nNumAnims = 4;
        pAnimInfo = sVolleyReceiveContactAnims;
        break;
    case 16:
        nNumAnims = 2;
        pAnimInfo = sSpecialVolleyContactAnims;
        break;
    case 9:
        nNumAnims = 8;
        pAnimInfo = sVolleyOneTouchShotContactAnims;
        break;
    }
    return pAnimInfo;
}

const LooseBallContactAnimInfo* DesireReceivePass::FindBestContactAnimInfo(
    const nlVector3& v3BallPosition,
    const nlVector3& v3FielderPosition,
    nlVector3& v3BallContactPos,
    unsigned short aFacingDirection, int receiveAnimType)
{
    int nNumAnims;
    const LooseBallContactAnimInfo* pAnimInfoList =
        GetContactAnimInfoList(receiveAnimType, nNumAnims);

    nlVector3 v3BallDirection;
    nlVec3Sub(v3BallDirection, v3BallPosition, v3BallContactPos);
    unsigned short aIncomingDirection = nlAngleDiff(
        nlVector3ToAngle(v3BallDirection),
        m_pFielder->mUnidentified024.m_aActualFacingDirection);

    const LooseBallContactAnimInfo* pBestAnimInfo = 0;
    const LooseBallContactAnimInfo* pReachableAnimInfo = 0;
    float fBestContactOffset = 1e11f;
    float fDistanceToContact = nlSqrt(nlVec3DistanceSquared2D(
        m_pFielder->mUnidentified024.m_v3Position,
        v3BallContactPos), true);

    for (int i = 0; i < nNumAnims; ++i)
    {
        const LooseBallContactAnimInfo* pCurrentAnimInfo = 0;
        if (pAnimInfoList[i].aIncomingAngleMin
            < pAnimInfoList[i].aIncomingAngleMax)
        {
            if (aIncomingDirection
                    >= pAnimInfoList[i].aIncomingAngleMin
                && aIncomingDirection
                    <= pAnimInfoList[i].aIncomingAngleMax)
            {
                pCurrentAnimInfo = &pAnimInfoList[i];
            }
        }
        else
        {
            if (aIncomingDirection
                    >= pAnimInfoList[i].aIncomingAngleMin
                || aIncomingDirection
                    <= pAnimInfoList[i].aIncomingAngleMax)
            {
                pCurrentAnimInfo = &pAnimInfoList[i];
            }
        }

        if (pCurrentAnimInfo == 0)
        {
            continue;
        }

        cSAnim* pAnim = m_pFielder->m_pAnimInventory
                            ->GetAnim(pCurrentAnimInfo->nAnimID);
        nlVector3 v3ContactOffsetWorld;
        nlVector3 v3ContactOffsetLocal;
        m_pFielder->GetJointPositionFuture(
            &v3ContactOffsetLocal, pCurrentAnimInfo->nAnimID,
            m_pFielder->m_nBallJointIndex,
            GetNormalizedContactTime(
                pAnim, pCurrentAnimInfo->fAnimContactFrame),
            true, true, false, true);

        float fSin;
        float fCos;
        nlSinCos(&fSin, &fCos, aFacingDirection);

        v3ContactOffsetWorld.x =
            v3ContactOffsetLocal.x * fCos
            - v3ContactOffsetLocal.y * fSin;
        v3ContactOffsetWorld.y =
            v3ContactOffsetLocal.y * fCos
            + v3ContactOffsetLocal.x * fSin;
        v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;

        float fContactOffset = nlVec2Length(
            *(nlVector2*)&v3ContactOffsetWorld);
        if (fDistanceToContact
            > fContactOffset - g_fReceiveArrivalRadius)
        {
            pReachableAnimInfo = pCurrentAnimInfo;
            continue;
        }
        if (fContactOffset < fBestContactOffset)
        {
            fBestContactOffset = fContactOffset;
            pBestAnimInfo = pCurrentAnimInfo;
        }
    }

    if (pBestAnimInfo == 0 && pReachableAnimInfo == 0)
    {
        pReachableAnimInfo = pAnimInfoList;
    }
    if (pReachableAnimInfo != 0)
    {
        return pReachableAnimInfo;
    }
    return pBestAnimInfo;
}

void DesireReceivePass::FindPassPosition(cPlayer* pPasser,
    bool bVolleyPass, bool bPerfectPass, float fPassSpeed,
    nlVector3& v3Position, float* pfRadius)
{
    cFielder* pFielder = m_pFielder;
    eFieldDirection eSearchDirection;
    InterpreterCore* pInterpreter =
        (InterpreterCore*)GetFuzzyRuntime();
    eSearchDirection = (eFieldDirection)
        fn_800C33C8(pInterpreter, "PassDirection", pPasser,
            pFielder).fn_800C2BD4();

    m_pSpaceSearch = new (8, false)
        SSearchBestPass(pPasser, pFielder,
            bVolleyPass, bPerfectPass, fPassSpeed);
    pFielder->SetSpaceSearch(m_pSpaceSearch);
    pFielder->m_pSpaceSearch->m_bDebugOn = false;
    pFielder->m_pSpaceSearch->FindBestPosition(
        v3Position,
        pFielder->mUnidentified024.m_v3Position,
        eSearchDirection, &pPasser->mUnidentified024.m_v3Position,
        6.0f, 0xAAAA);

    pFielder->m_pPhysicsCharacter->GetRadius(pfRadius);
    *pfRadius += 0.25f;
    cField::FixOutOfBoundsPosition(
        v3Position, *pfRadius, true);
}

void DesireReceivePass::ExecutePass(cPlayer* pPasser, bool bVolleyPass, bool bFindPosition,
    bool bPerfectPass, const nlVector3* pv3PassPosition,
    float fMinPassSpeed, float fMaxPassSpeed)
{
    cFielder* pPassTarget = m_pFielder;
    int eReceiveAnimType = 2;
    if (bVolleyPass)
    {
        if (pPassTarget->fn_8003E7F8()
            || pPassTarget->fn_8003E84C())
        {
            eReceiveAnimType = 16;
        }
        else
        {
            eReceiveAnimType = 3;
        }
    }

    nlVector3 v3PassPosition = pPassTarget->mUnidentified024.m_v3Position;
    nlVector2 v2BallToTarget = {
        pPassTarget->mUnidentified024.m_v3Position.x - g_pBall->GetPosition().x,
        pPassTarget->mUnidentified024.m_v3Position.y - g_pBall->GetPosition().y,
    };
    float fPassSpeed = DoCalculatePassSpeed(v2BallToTarget,
        fMinPassSpeed, fMaxPassSpeed, g_fPassDistanceMin, g_fPassDistanceMax);
    if (bFindPosition && g_bFindReceivePassPosition)
    {
        nlVector3 v3FoundPassPosition;
        if (pv3PassPosition != 0)
        {
            v3FoundPassPosition = *pv3PassPosition;
        }
        else
        {
            float fRadius;
            FindPassPosition(pPasser, bVolleyPass, bPerfectPass, fPassSpeed,
                v3FoundPassPosition, &fRadius);
        }
        v3PassPosition = v3FoundPassPosition;
    }

    nlVector2 v2BallToPassPosition = {
        v3PassPosition.x - g_pBall->m_v3Position.x,
        v3PassPosition.y - g_pBall->m_v3Position.y,
    };
    float fPassDistance =
        nlGetLength2D(v2BallToPassPosition.x, v2BallToPassPosition.y);
    float fPassTime = nlMaxEquals(
        0.04f, fPassDistance / fPassSpeed);

    nlVector3 v3BallVelocity;
    nlVector3 v3BallToPassPosition;
    PassBallData eventData;
    nlVector3 v3ContactOffsetLocal;
    nlVector3 v3ContactOffsetWorld;
    unsigned short aFacingDirection;
    int eSpinType = SPINTYPE_ROLLING;
    bool bHighArc = false;
    if (bVolleyPass)
    {
        float fCos;
        float fSin;
        int nNumAnims;
        eSpinType = SPINTYPE_BACK;
        const LooseBallContactAnimInfo* pAnimInfo =
            GetContactAnimInfoList(eReceiveAnimType, nNumAnims);
        cSAnim* pAnim = m_pFielder->m_pAnimInventory
                            ->GetAnim(pAnimInfo->nAnimID);
        aFacingDirection =
            m_pFielder->mUnidentified024.m_aActualFacingDirection;
        m_pFielder->GetJointPositionFuture(
            &v3ContactOffsetLocal, pAnimInfo->nAnimID,
            m_pFielder->m_nBallJointIndex,
            GetNormalizedContactTime(
                pAnim, pAnimInfo->fAnimContactFrame),
            true, true, false, true);

        nlSinCos(&fSin, &fCos, aFacingDirection);
        v3ContactOffsetWorld.x =
            v3ContactOffsetLocal.x * fCos
            - v3ContactOffsetLocal.y * fSin;
        v3ContactOffsetWorld.y =
            v3ContactOffsetLocal.y * fCos
            + v3ContactOffsetLocal.x * fSin;
        v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;

        v3PassPosition.z = v3ContactOffsetWorld.z;
        g_pBall->ShootAtFast(
            v3BallVelocity, v3PassPosition, fPassTime);
        if (!m_pFielder->fn_8003E74C()
            && v3BallVelocity.z > g_fMaxVolleyPassUpSpeed)
        {
            v3BallVelocity.z = g_fMaxVolleyPassUpSpeed;
            bHighArc = true;
        }
    }
    else
    {
        nlVec3Sub(v3BallToPassPosition,
            v3PassPosition, g_pBall->m_v3Position);
        nlVec3Scale(v3BallVelocity, v3BallToPassPosition,
            1.0f / fPassTime);
        if (g_pBall->m_v3Position.z < 0.36f)
        {
            v3BallVelocity.z = InterpolateRangeClamped(
                g_fGroundPassMinLift, g_fGroundPassMaxLift,
                g_fPassDistanceMin, g_fPassDistanceMax, fPassDistance);
        }
        else
        {
            v3BallVelocity.z = nlMinEquals(
                0.0f, v3BallVelocity.z);
        }
    }

    fn_80016DF8(g_pBall, pPasser, &v3BallVelocity,
        eSpinType, bVolleyPass && !bHighArc, false);
    pPasser->SetNoPickUpTime(g_fPasserNoPickUpTime);
    EmitBallShot((cFielder*)pPasser, BALL_EFFECT_S2S_SUPER_SHOT, 0, 0, 0);

    if (pPassTarget->CanReceivePass() && !bHighArc)
    {
        eventData.pPasser = pPasser;
        eventData.pTarget = pPassTarget;
        eventData.bVolleyPass = bVolleyPass;
        bool bHasGlobalPad = pPasser->GetGlobalPad() != 0;
        eventData.mPasserControllerID = bHasGlobalPad
            ? pPasser->GetGlobalPad()->GetPadID()
            : -1;
        g_pGame->mUnidentified49C.mEvent16.Deliver(&eventData);

        UnidentifiedVariantCollection params;
        params.Set(14, FuzzyVariant(FT_VECTOR, v3PassPosition));
        params.Set(11, FuzzyVariant(FT_INT, eReceiveAnimType));
        fn_8003EBD0(pPassTarget, 22, &params);

        cAIPad* pAIPad = pPasser->m_pController;
        if (pAIPad != 0 && pPassTarget->m_pController == 0)
        {
            pPassTarget->SetAIPad(pAIPad);
            pPasser->SetAIPad(0);
        }
    }
    else
    {
        cAIPad* pAIPad = pPasser->m_pController;
        if (pAIPad != 0 && pPassTarget->m_pController == 0)
        {
            pPassTarget->SetAIPad(pAIPad);
            pPasser->SetAIPad(0);
        }
        fn_80015B38(g_pBall, false);
    }

    if (g_pGame->GetGameState() == 3
        || g_pGame->GetGameState() == 2)
    {
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_tNoPickupTimer.SetSeconds(0.0f);
        fn_80015B38(g_pBall, false);
    }
}
