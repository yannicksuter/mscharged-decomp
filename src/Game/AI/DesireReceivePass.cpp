#include "Game/AI/DesireReceivePass.h"
#include "Game/DetInput.h"
#include "Game/Sys/debug.h"

#include "Game/AI/DesireSteering.h"

#include <stddef.h>

#include "Game/AI/DesireUpdate.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/AnimInventory.h"
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
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/AI/AvoidableObject.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <math.h>

static const LooseBallContactAnimInfo lbl_804DC1B0[5] = {
    { 0x29, 3.0f, 0x0000, 0xFFFF },
    { 0x2B, 7.0f, 0xE000, 0x2000 },
    { 0x2C, 7.0f, 0xA000, 0xE000 },
    { 0x2E, 7.0f, 0x6000, 0xA000 },
    { 0x2D, 7.0f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo lbl_804DC1F0[4] = {
    { 0x2A, 9.0f, 0x0000, 0xFFFF },
    { 0x2F, 10.0f, 0xE000, 0x2000 },
    { 0x33, 9.0f, 0x2000, 0x8000 },
    { 0x31, 9.0f, 0x0000, 0x0000 },
};

static const LooseBallContactAnimInfo lbl_804DC220[8] = {
    { 0x3C, 10.0f, 0xE000, 0x2000 },
    { 0x3D, 9.0f, 0xA000, 0xE000 },
    { 0x3F, 9.0f, 0x6000, 0xA000 },
    { 0x3E, 9.0f, 0x2000, 0x6000 },
    { 0x40, 10.0f, 0xE000, 0x2000 },
    { 0x41, 10.0f, 0xA000, 0xE000 },
    { 0x43, 10.0f, 0x6000, 0xA000 },
    { 0x42, 9.5f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo lbl_804DC280[8] = {
    { 0x34, 7.0f, 0xE000, 0x2000 },
    { 0x35, 7.0f, 0xA000, 0xE000 },
    { 0x37, 7.0f, 0x6000, 0xA000 },
    { 0x36, 7.0f, 0x2000, 0x6000 },
    { 0x38, 9.0f, 0xE000, 0x2000 },
    { 0x39, 9.0f, 0xA000, 0xE000 },
    { 0x3B, 9.0f, 0x6000, 0xA000 },
    { 0x3A, 9.0f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo lbl_804DC2E0[4] = {
    { 0x44, 4.0f, 0xE000, 0x2000 },
    { 0x45, 4.0f, 0xA000, 0xE000 },
    { 0x47, 4.0f, 0x6000, 0xA000 },
    { 0x46, 4.0f, 0x2000, 0x6000 },
};

static const LooseBallContactAnimInfo lbl_804DC310[2] = {
    { 0x32, 4.0f, 0x0000, 0x0000 },
    { 0x30, 4.0f, 0xE000, 0x2000 },
};

extern "C" void fn_8003BA94(cFielder*, float);
extern "C" void fn_800180F4(cBall*, nlVector3*, float);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" bool fn_80035F84(cFielder*, nlVector3*, float*, nlVector3*,
    float*, unsigned short, const LooseBallContactAnimInfo*);
extern "C" bool fn_80036234(cFielder*, nlVector3*, float*, nlVector3*,
    float*, const LooseBallContactAnimInfo*, nlVector3*, unsigned short);
extern "C" void fn_80015B38(cBall*, bool);
extern "C" void fn_80016DF8(
    cBall*, cPlayer*, nlVector3*, int, bool, bool);
extern "C" void fn_801B75C8(cFielder*, int, int, int, int);
extern "C" void fn_8003EBD0(
    cFielder*, int, UnidentifiedVariantCollection*);
extern "C" void fn_80098098(cFielder*);
extern "C" bool fn_80035F34(cFielder*);
extern "C" bool fn_80036A58(cFielder*, unsigned short*);
extern "C" bool fn_80036C8C(cFielder*, unsigned short*);
extern "C" float fn_8002C328(PlayerTweaks*);
extern "C" float fn_8002CE14(PlayerTweaks*);
extern "C" void fn_8003C268(cFielder*, float, float);
extern "C" void fn_80316968(void*);
extern "C" cFielder* fn_80096F54(cPlayer*, bool);
extern "C" float fn_8004028C(cFielder*);
extern "C" void fn_8005C650(cGame*);
extern "C" float fn_8002CFC4(PlayerTweaks*);
extern "C" float fn_8002C730(PlayerTweaks*);
extern "C" float fn_8002C6E8(PlayerTweaks*);
extern "C" float fn_8002C678(PlayerTweaks*);
extern "C" void* fn_80311734(void*);
extern "C" UnidentifiedVariant_80054AB8* fn_80312360(
    InterpreterCore*, void*, int, cPlayer*, cPlayer*);
extern "C" UnidentifiedVariant_80054AB8 fn_800C33C8(
    InterpreterCore*, const char*, cPlayer*, cPlayer*);
extern "C" UnidentifiedVariant_80054AB8 fn_800C33D8(
    InterpreterCore*, cPlayer*, const char*, cPlayer*);
extern "C" UnidentifiedVariant_80054AB8 fn_800C3448(
    InterpreterCore*, const unsigned int&, cPlayer*, cPlayer*);

static float lbl_806DC190 = 5.0f;
static unsigned short sDesireReceivePassType = 0xFFFF;
bool lbl_806DC196 = true;
float lbl_806DC198 = 0.66f;
float lbl_806DC19C = 30.0f;
float lbl_806DC1A0 = 0.5f;
float lbl_806DC1A4 = 3.0f;
float lbl_806DC1A8 = 0.5f;
float lbl_806DC1AC = 0.1f;
float lbl_806DC1B0 = 12.5f;
float lbl_806DC1B4 = 0.1f;
float lbl_806DC1B8 = 0.1f;
float lbl_806DC1BC = 1.0f;
float lbl_806DC1C0 = 1.9f;
float lbl_806DC1C4 = 0.1f;
float lbl_806DC1C8 = 0.4f;
float lbl_806DC1CC = 0.5f;
float lbl_806DC1D0 = 0.5f;
float lbl_806DC1D4 = 2.0f;
float lbl_806DC1D8 = 0.93f;
float lbl_806DC1DC = 5.0f;
float lbl_806DC1E0 = 0.55f;
float lbl_806DC1E4 = 0.02f;
float lbl_806DC1E8 = 0.02f;
float lbl_806DC1EC = 0.02f;
float lbl_806DC1F0 = 0.02f;
extern float lbl_806E4008;
extern float lbl_806E4018;
extern float lbl_806E4024;
extern float lbl_806E4030;
extern float lbl_806E4034;
extern float lbl_806E4038;
extern float lbl_806E403C;
extern float lbl_806E4040;
float lbl_806E0E48 = lbl_806DC1E4 + lbl_806DC1E8
    + lbl_806DC1EC + lbl_806DC1F0;
static TweakFloatBinding lbl_8056DA88("sfSpeedAdjustChargeLevel1",
    "Game/Gameplay/Charging/Pass", &lbl_806DC1E4, true);
static TweakFloatBinding lbl_8056DAA8("sfSpeedAdjustChargeLevel2",
    "Game/Gameplay/Charging/Pass", &lbl_806DC1E8, true);
static TweakFloatBinding lbl_8056DAC8("sfSpeedAdjustChargeLevel3",
    "Game/Gameplay/Charging/Pass", &lbl_806DC1EC, true);
static TweakFloatBinding lbl_8056DAE8("sfSpeedAdjustChargeLevelMax",
    "Game/Gameplay/Charging/Pass", &lbl_806DC1F0, true);

extern float g_fSimulationTick;

static inline float GetNormalizedContactTime(
    const cSAnim* anim, float contactFrame)
{
    return contactFrame / (float)anim->m_nNumKeys;
}

static inline float DoCalculatePassSpeed(const nlVector2& distance,
    float passSpeedMin, float passSpeedMax,
    float passDistMin, float passDistMax)
{
    float fPassCharge = NormalizeVal(
                            nlVec2Length(distance), passDistMin, passDistMax)
        - lbl_806E0E48;
    if (fn_800155A0(g_pBall, 0) > 1.0f)
    {
        fPassCharge += lbl_806DC1E4;
    }
    if (fn_800155A0(g_pBall, 0) > 2.0f)
    {
        fPassCharge += lbl_806DC1E8;
    }
    if (fn_800155A0(g_pBall, 0) > 3.0f)
    {
        fPassCharge += lbl_806DC1EC;
    }
    if (fn_800155A0(g_pBall, 0) >= 4.0f)
    {
        fPassCharge += lbl_806DC1F0;
    }
    fPassCharge = nlMaxEquals(fPassCharge, 0.0f);
    fPassCharge = nlMinEquals(fPassCharge, 1.0f);
    return Interpolate(passSpeedMin, passSpeedMax, fPassCharge);
}

DesireReceivePass::DesireReceivePass()
    : Desire(22, UnidentifiedStringHash("TransDesireReceivePass"))
    , mEstimated()
{
}

bool DesireReceivePass::UnidentifiedInitialize(void* context)
{
    Desire::UnidentifiedInitialize(context);

    DesireSteering* desire = (DesireSteering*)fn_8002E08C(
        mUnidentifiedFielder, 34);
    fn_800C5784(desire);

    mEstimated.Reset();
    mbOneTouchVolley = false;
    mbOneTouchShot = false;
    mbOneTouchShotLate = false;
    mbOneTouchPass = false;
    mpOneTouchPassTarget = 0;

    mUnidentifiedB4 = nlSqrt(
        g_pBall->m_v3Velocity.x * g_pBall->m_v3Velocity.x
            + g_pBall->m_v3Velocity.y * g_pBall->m_v3Velocity.y,
        true);

    UnidentifiedVariantCollection* params =
        (UnidentifiedVariantCollection*)context;
    meReceiveAnimType = params->Get(11)->mData.i;
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

    fn_800C1A08();

    float fMaxDistanceSq = lbl_806DC1C4 * lbl_806DC1C4;

    nlVector2 v2Delta = {
        mEstimated.v3AnimStartPos.x
            - mUnidentifiedFielder->mUnidentified024.m_v3Position.x,
        mEstimated.v3AnimStartPos.y
            - mUnidentifiedFielder->mUnidentified024.m_v3Position.y,
    };
    if (nlVec2LengthSquared(v2Delta) > fMaxDistanceSq)
    {
        meDesireSubState = 0;
        mUnidentifiedFielder->InitActionRunning();
        fn_8003BA94(mUnidentifiedFielder, 0.1f);
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
        mUnidentifiedFielder->SetNoPickUpTime(lbl_806DC190);
        mUnidentified078 = lbl_806DC190;
    }
    return result;
}

void DesireReceivePass::Update(UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (update->fn_800C2BD4() == 3)
    {
        bool bUnhandled = false;
        switch (update->fn_800C2C00(8)->fn_800C2BD4())
        {
        case 15:
            *update = 0;
            fn_800C089C(update->fn_800C2C08()->Get(16)->fn_800C2BF8());
            fn_800C0F14();
            break;
        case 14:
        {
            *update = 0;
            cPlayer* pPassTarget = update->fn_800C2C08()->Get(14)->GetPlayer();
            bool bVolleyPass = false;
            if (update->fn_800C2C10(16))
            {
                bVolleyPass = update->fn_800C2C08()->Get(16)->fn_800C2BF8();
            }
            fn_800C0AE8(bVolleyPass, pPassTarget);
            fn_800C0F14();
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
            || g_pBall->fn_800C2EC0() != mUnidentifiedFielder
            || !mUnidentifiedFielder->CanReceivePass()))
    {
        *update = 1;
        return;
    }

    mvDesiredPosition = mEstimated.v3AnimStartPos;
    fn_800C0704();
    if (fn_800C2F28()->fn_800C2F20() != this)
    {
        *update = 1;
        return;
    }

    if (g_pBall->fn_800C2EC0() == mUnidentifiedFielder
        && g_pBall->fn_800C2EC8() > lbl_806DC198
        && AIsgn(g_pBall->fn_800C2F18().x)
            == AIsgn(mUnidentifiedFielder->GetTeam()->GetOtherNet()->fn_800C2F30().x)
        && mbOneTouchShot && !mbOneTouchVolley)
    {
        fn_8005C650(g_pGame);
    }

    if (mEstimated.bLocked)
    {
        mEstimated.fAnimStartTime -= fDeltaT;
    }
    float fStartThreshold = lbl_806DC1BC * g_fSimulationTick;
    switch (meDesireSubState)
    {
    case 0:
    {
        if (!CalcRoughEstimates(meReceiveAnimType))
        {
            *update = 1;
            return;
        }
        fn_800C1A08();
        if (mEstimated.fAnimStartTime <= fStartThreshold)
        {
            if (!CalcExactEstimates(true) || !StartPickupAnimation())
            {
                *update = 1;
            }
            meDesireSubState = 4;
            return;
        }

        float fArrivalRadius = lbl_806DC1C4;
        if (!fn_800C0E74())
        {
            fArrivalRadius = lbl_806DC1C8;
        }
        DesireSteering* pSteering = (DesireSteering*)fn_8002E08C(mUnidentifiedFielder, 34);
        fn_800C61A4(pSteering, mEstimated.v3AnimStartPos,
            mEstimated.aFacingDirection, mEstimated.fAnimStartTime, fArrivalRadius);
        pSteering->fn_800C2F48(InterpolateClamped(lbl_806DC1E0, 0.0f, g_pBall->fn_800C2EC8()));
        if (fn_8004028C(mUnidentifiedFielder) <= fArrivalRadius)
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
                mUnidentifiedFielder->InitActionIdleTurn(mEstimated.aFacingDirection);
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
            DesireSteering* pSteering = (DesireSteering*)fn_8002E08C(mUnidentifiedFielder, 34);
            fn_800C61A4(pSteering, mEstimated.v3AnimStartPos,
                mEstimated.aFacingDirection, mEstimated.fAnimStartTime, lbl_806DC1C4);
        }
        break;
    case 2:
        if (mUnidentifiedFielder->IsActionDone())
        {
            meDesireSubState = 3;
            mUnidentifiedFielder->InitActionWait();
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
        if (mUnidentifiedFielder->fn_800C2F40() != 0)
        {
            if (mUnidentifiedFielder->GetGlobalPad() != 0)
            {
                mbOneTouchVolley = fn_80035F34(mUnidentifiedFielder);
            }
            if (mbOneTouchPass)
            {
                if (meReceiveAnimType & 4)
                {
                    float fMinPassSpeed = fn_8002CFC4(mUnidentifiedFielder->GetTweaks());
                    float fMaxPassSpeed = fn_8002C730(mUnidentifiedFielder->GetTweaks());
                    if (!mbOneTouchVolley)
                    {
                        fMinPassSpeed = fn_8002C6E8(mUnidentifiedFielder->GetTweaks());
                        fMaxPassSpeed = fn_8002C678(mUnidentifiedFielder->GetTweaks());
                    }
                    mUnidentifiedFielder->DoRegularPassing(mpOneTouchPassTarget,
                        mbOneTouchVolley, true, false, false, fMinPassSpeed, fMaxPassSpeed);
                }
                else if (fn_800C0E54())
                {
                    if (fabsf(mEstimated.fReceivePassAnimTime
                            - mUnidentifiedFielder->fn_800C2F64()->get_fTime()) <= 0.06666667f)
                    {
                        mUnidentifiedFielder->InitActionOneTouchPassFromVolley(mpOneTouchPassTarget, mbOneTouchVolley);
                    }
                    else if (mUnidentifiedFielder->IsActionDone())
                    {
                        mUnidentifiedFielder->InitActionPass(mpOneTouchPassTarget, mbOneTouchVolley, 0, true);
                    }
                }
                else
                {
                    mUnidentifiedFielder->InitActionPass(mpOneTouchPassTarget, mbOneTouchVolley, 0, true);
                }
            }
            else if (mbOneTouchShotLate)
            {
                if (meReceiveAnimType & 4)
                {
                    mUnidentifiedFielder->fn_8004B86C(mbOneTouchVolley, true);
                }
                else if (fn_800C0E54())
                {
                    if (fabsf(mEstimated.fReceivePassAnimTime
                            - mUnidentifiedFielder->fn_800C2F64()->get_fTime()) <= 0.06666667f)
                    {
                        mUnidentifiedFielder->InitActionLateOneTimerFromVolley();
                    }
                    else if (mUnidentifiedFielder->IsActionDone())
                    {
                        if (mUnidentifiedFielder->GetGlobalPad() != 0
                            && mUnidentifiedFielder->GetGlobalPad()->IsPressed(0x1C, true))
                        {
                            if (mUnidentifiedFielder->ShouldStartCrossBlend(0x14))
                            {
                                *update = 1;
                            }
                        }
                        else
                        {
                            mUnidentifiedFielder->fn_8004B86C(mbOneTouchVolley, true);
                        }
                    }
                }
                else if (mUnidentifiedFielder->GetGlobalPad() != 0
                    && mUnidentifiedFielder->GetGlobalPad()->IsPressed(0x1C, true))
                {
                    if (mUnidentifiedFielder->ShouldStartCrossBlend(0x14))
                    {
                        *update = 1;
                    }
                }
                else
                {
                    mUnidentifiedFielder->fn_8004B86C(mbOneTouchVolley, true);
                }
            }
            else if (!mbOneTouchShot && fn_800C2F6C())
            {
                if (!fn_800C0E74() || mUnidentifiedFielder->ShouldStartCrossBlend(0x14))
                {
                    mUnidentifiedFielder->InitActionRunningWB(true);
                    *update = 1;
                }
            }
        }
        if (mUnidentifiedFielder->IsActionDone())
        {
            *update = 1;
        }
        break;
    }
}

inline int DesireReceivePass::UnidentifiedAddReceiveFlags(int animType, bool bFlag)
{
    if (bFlag)
    {
        animType |= 1;
    }
    return animType;
}

inline bool DesireReceivePass::UnidentifiedCanOneTouch()
{
    float fPassProgress = g_pBall->fn_800C2EC8();
    bool bSpecialReceive = mUnidentifiedFielder->fn_8003E7F8()
        || mUnidentifiedFielder->fn_8003E84C();
    if ((bSpecialReceive && mUnidentifiedFielder->m_pBall != 0)
        || (fPassProgress < lbl_806DC1B4
            && mUnidentifiedTimer.GetSeconds() < lbl_806DC1B8)
        || (meDesireSubState == 4 && (mbOneTouchShot || mbOneTouchPass)))
    {
        return false;
    }
    return true;
}

void DesireReceivePass::fn_800C0704()
{
    if (mUnidentifiedFielder->GetGlobalPad() != 0)
    {
        fn_80098098(mUnidentifiedFielder);
        if (mUnidentifiedFielder->GetGlobalPad()->JustPressed(0x1C, true))
        {
            fn_800C089C(fn_80035F34(mUnidentifiedFielder));
        }
        else if (mUnidentifiedFielder->GetGlobalPad()->JustPressed(0x1B, true))
        {
            fn_800C0AE8(
                fn_80035F34(mUnidentifiedFielder), 0);
        }

        if (mUnidentifiedFielder->m_pBall != 0)
        {
            PlayerTweaks* pTweaks = mUnidentifiedFielder->GetTweaks();
            float fMaxSpeed = fn_8002C328(pTweaks);
            float fMinSpeed = fn_8002CE14(
                mUnidentifiedFielder->GetTweaks());
            fn_8003C268(mUnidentifiedFielder, fMinSpeed, fMaxSpeed);
            return;
        }

        if (meDesireSubState != 4)
        {
            unsigned short aDirection = 0;
            if (fn_80036A58(mUnidentifiedFielder, &aDirection))
            {
                unsigned short aHitDirection =
                    mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
                if (mUnidentifiedFielder->m_pController != 0
                    && mUnidentifiedFielder->m_pController->GetMovementStickMagnitude() > 0.001f)
                {
                    aHitDirection =
                        mUnidentifiedFielder->m_pController->GetMovementStickDirection();
                }
                mUnidentifiedFielder->InitActionHit(0, aHitDirection);
                return;
            }

            if (fn_80036C8C(mUnidentifiedFielder, &aDirection))
            {
                mUnidentifiedFielder->InitActionSlideAttack(
                    0, aDirection, -1.0f);
                fn_80316968(this);
            }
        }
    }
}

void DesireReceivePass::fn_800C089C(bool bVolleyPass)
{
    if (!UnidentifiedCanOneTouch())
    {
        return;
    }

    mbOneTouchPass = false;
    mbOneTouchShot = true;
    mbOneTouchVolley = bVolleyPass;
    bool bSpecialReceive =
        mUnidentifiedFielder->fn_8003E7F8()
        || mUnidentifiedFielder->fn_8003E84C();
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
        int eOneTouchReceiveAnimType = UnidentifiedAddReceiveFlags(4, fn_800C0E54());
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

    eReceiveAnimType = UnidentifiedAddReceiveFlags(8, fn_800C0E54());
    meReceiveAnimType = eReceiveAnimType;
}

void DesireReceivePass::fn_800C0AE8(bool bVolleyPass, cPlayer* pPassTarget)
{
    if (!UnidentifiedCanOneTouch())
    {
        return;
    }

    mbOneTouchShot = false;
    mbOneTouchShotLate = false;
    mbOneTouchVolley = false;
    if (pPassTarget == 0)
    {
        pPassTarget = fn_80096F54(
            mUnidentifiedFielder, false);
    }
    if (pPassTarget == 0)
    {
        fn_800C089C(bVolleyPass);
        return;
    }

    mbOneTouchVolley = bVolleyPass;
    mbOneTouchPass = true;
    mpOneTouchPassTarget = pPassTarget;
    bool bSpecialReceive =
        mUnidentifiedFielder->fn_8003E7F8()
        || mUnidentifiedFielder->fn_8003E84C();
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
        int eOneTouchReceiveAnimType = UnidentifiedAddReceiveFlags(4, fn_800C0E54());
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

    int eReceiveAnimType = UnidentifiedAddReceiveFlags(2, fn_800C0E54());
    meReceiveAnimType = eReceiveAnimType;
}

void DesireReceivePass::UnidentifiedCleanup()
{
    if (mUnidentifiedFielder->m_pBall == 0)
    {
        mUnidentifiedFielder->ClearPassTargetIfAmThePassTarget();
    }

    mEstimated.Reset();

    if (m_pSpaceSearch == mUnidentifiedFielder->m_pSpaceSearch)
    {
        mUnidentifiedFielder->SetSpaceSearch(0);
    }
    m_pSpaceSearch = 0;

    if (mbOneTouchShot)
    {
        mUnidentifiedFielder->SetNoPickUpTime(0.2f);
    }
    else
    {
        mUnidentifiedFielder->SetNoPickUpTime(0.0f);
    }

    DesireSteering* desire = (DesireSteering*)fn_8002E08C(
        mUnidentifiedFielder, 34);
    fn_800C574C(desire);
    fn_800C577C(desire);
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
            mUnidentifiedFielder->mUnidentified024.m_v3Position.x - mv3PassIntercept.x,
            mUnidentifiedFielder->mUnidentified024.m_v3Position.y - mv3PassIntercept.y,
        };
        result = nlVec2LengthSquared(v2Delta) > lbl_806DC1C4;
        break;
    }
    }
    return result;
}

void DesireReceivePass::fn_800C0F14()
{
    float fDuration = 0.5f + g_pBall->m_tPassTargetTimer.GetSeconds();
    const UnidentifiedStateTransition& transition =
        !mUnidentified070.UnidentifiedIsUnset() ? mUnidentified070 : mUnidentified068;
    UnidentifiedFielderInput* input = mUnidentified018->mUnidentified064;
    input->fn_8030FA10(input->fn_8030F9B4(transition.mUnidentifiedHash, 1), fDuration);
}

bool DesireReceivePass::CalcRoughEstimates(int receiveAnimType)
{
    if (mEstimated.bLocked)
    {
        return false;
    }

    Estimated estimated = mEstimated;
    bool bUseGroundIntercept = true;
    int nNumAnims;
    const LooseBallContactAnimInfo* pAnimInfo =
        fn_800C1FA4(receiveAnimType, nNumAnims);

    cSAnim* pAnim = mUnidentifiedFielder->m_pAnimInventory
                        ->GetAnim(pAnimInfo->nAnimID);
    nlVector3 v3ContactOffsetLocal;
    unsigned short aFacingDirection =
        mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
    mUnidentifiedFielder->GetJointPositionFuture(
        &v3ContactOffsetLocal, pAnimInfo->nAnimID,
        mUnidentifiedFielder->m_nBallJointIndex,
        pAnimInfo->fAnimContactFrame / (float)pAnim->m_nNumKeys,
        true, true, false, true);

    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, aFacingDirection);

    nlVector3 v3ContactOffsetWorld;
    nlVec3Set(v3ContactOffsetWorld,
        v3ContactOffsetLocal.x * fCos - v3ContactOffsetLocal.y * fSin,
        v3ContactOffsetLocal.y * fCos + v3ContactOffsetLocal.x * fSin,
        v3ContactOffsetLocal.z);

    int nNumIntercepts;
    float fInterceptTimes[2];
    if (v3ContactOffsetWorld.z
        > lbl_806DC1CC * mv3PassIntercept.z)
    {
        g_pBall->PredictLandingSpotAndTime(estimated.v3BallContactPos,
            &nNumIntercepts, fInterceptTimes,
            v3ContactOffsetWorld.z);
        bUseGroundIntercept = false;

        if (nNumIntercepts == 2)
        {
            float fClosestDistanceSq = lbl_806E4030;
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
                        v3ContactOffsetWorld.z;
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
        float fRadius =
            mUnidentifiedFielder->mUnidentified320->GetRadius();
        float fMaxCatchupSpeed = fn_8002E1B0(mUnidentifiedFielder);
        CalcInterceptXY(mUnidentifiedFielder->mUnidentified024.m_v3Position,
            fMaxCatchupSpeed, fRadius, g_pBall->m_v3Position,
            g_pBall->m_v3Velocity, nNumIntercepts, fInterceptTimes);

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
        fInterceptTimes[1] = lbl_806DC190;
        nNumIntercepts = 2;

        nlVector3 v3FirstBallPosition;
        fn_800180F4(
            g_pBall, &v3FirstBallPosition, fInterceptTime);

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
                mUnidentifiedFielder->mUnidentified024.m_v3Position);

            float fDot = 0.0f;
            bool bBallDirectionValid = false;
            float fLengthSq = v3BallDirection.GetLengthSq3D();
            if (fLengthSq != 0.0f)
            {
                nlVec3Scale(v3BallDirection,
                    nlRecipSqrt(fLengthSq, true));
                bBallDirectionValid = true;
            }
            if (bBallDirectionValid)
            {
                bool bFielderDirectionValid = false;
                fLengthSq = v3FielderDirection.GetLengthSq3D();
                if (fLengthSq != 0.0f)
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

            if (fDot > lbl_806DC1D8
                && nlVec3Length(g_pBall->m_v3Velocity)
                    < lbl_806DC1DC)
            {
                v3ClosestPoint = GetClosestPointOnLineABFromPointC(
                    v3FirstBallPosition, v3SecondBallPosition,
                    mUnidentifiedFielder->mUnidentified024.m_v3Position);
                float fBlend = NormalizeVal(
                    nlVec2Length(*(nlVector2*)&g_pBall->m_v3Velocity)
                        / mUnidentifiedB4,
                    lbl_806E4024, lbl_806E4018);
                nlVecLerp(estimated.v3BallContactPos,
                    v3FirstBallPosition, v3ClosestPoint, fBlend);
            }
            else
            {
                estimated.v3BallContactPos = v3FirstBallPosition;
            }
        }
        else
        {
            estimated.v3BallContactPos = v3FirstBallPosition;
        }

        nlVector2 v2BallDelta = {
            estimated.v3BallContactPos.x - g_pBall->m_v3Position.x,
            estimated.v3BallContactPos.y - g_pBall->m_v3Position.y,
        };
        float fBallDistance = nlVec2Length(v2BallDelta);
        float fBallSpeed =
            nlVec2Length(*(nlVector2*)&g_pBall->m_v3Velocity);
        estimated.fBallContactTime = nlMaxEquals(
            lbl_806E4008, fBallDistance / fBallSpeed);
    }

    if (mEstimated.fBallContactTime > 0.0f
        && (float)fabs(mEstimated.fBallContactTime
            - estimated.fBallContactTime)
            > lbl_806E4034)
    {
        tDebugPrintManager::Print(DC_AI,
            "DesireReceivePass::CalcRoughEstimates - the ball got deflected too much, pass aborted\n");
        return false;
    }

    cField::FixOutOfBoundsPosition(estimated.v3BallContactPos,
        mUnidentifiedFielder->mUnidentified320->GetRadius(), true);

    nlVector3 v3FacingDirection;
    nlVec3Sub(v3FacingDirection, estimated.v3BallContactPos,
        mUnidentifiedFielder->mUnidentified024.m_v3Position);
    if (!fn_800C0E74())
    {
        nlVec3Sub(v3FacingDirection, g_pBall->m_v3Position,
            mUnidentifiedFielder->mUnidentified024.m_v3Position);
    }
    estimated.aFacingDirection =
        nlVector3ToAngle(v3FacingDirection);

    g_pBall->SetPassTargetTimer(estimated.fBallContactTime);
    g_pBall->SetPassTarget(mUnidentifiedFielder,
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
        result = fn_80036234(mUnidentifiedFielder,
            &mEstimated.v3AnimStartPos, &mEstimated.fAnimStartTime,
            &mEstimated.v3BallContactPos, &mEstimated.fBallContactTime,
            mEstimated.mUnidentifiedAnimInfo,
            &mEstimated.v3BallContactPos,
            mEstimated.aFacingTargetDirection);
    }
    else
    {
        result = fn_80035F84(mUnidentifiedFielder,
            &mEstimated.v3AnimStartPos, &mEstimated.fAnimStartTime,
            &mEstimated.v3BallContactPos, &mEstimated.fBallContactTime,
            mEstimated.aFacingTargetDirection,
            mEstimated.mUnidentifiedAnimInfo);
    }

    g_pBall->m_pPhysicsBall->mbUseTiltForce =
        bCollideWithFielders;
    g_pBall->m_pPhysicsBall->mbUseWindForce =
        bCollideWithGoalies;

    if (result)
    {
        g_pBall->SetPassTargetTimer(mEstimated.fBallContactTime);
        g_pBall->SetPassTarget(mUnidentifiedFielder,
            mEstimated.v3BallContactPos, fn_800C0E54());
    }
    else
    {
        tDebugPrintManager::Print(DC_AI,
            "DesireReceivePass::CalcExactEstimates - failed to find an interception point!\n");
    }
    return result;
}

const LooseBallContactAnimInfo* DesireReceivePass::fn_800C1FA4(
    int receiveAnimType, int& nNumAnims)
{
    nNumAnims = 0;
    const LooseBallContactAnimInfo* pAnimInfo = 0;
    switch (receiveAnimType)
    {
    case 2:
        nNumAnims = 5;
        pAnimInfo = lbl_804DC1B0;
        break;
    case 4:
    case 5:
        nNumAnims = 4;
        pAnimInfo = lbl_804DC2E0;
        break;
    case 8:
        nNumAnims = 8;
        pAnimInfo = lbl_804DC280;
        break;
    case 3:
        nNumAnims = 4;
        pAnimInfo = lbl_804DC1F0;
        break;
    case 16:
        nNumAnims = 2;
        pAnimInfo = lbl_804DC310;
        break;
    case 9:
        nNumAnims = 8;
        pAnimInfo = lbl_804DC220;
        break;
    }
    return pAnimInfo;
}

const LooseBallContactAnimInfo* DesireReceivePass::fn_800C2048(
    const nlVector3& v3BallPosition,
    const nlVector3& v3FielderPosition,
    nlVector3& v3BallContactPos,
    unsigned short aFacingDirection, int receiveAnimType)
{
    int nNumAnims;
    const LooseBallContactAnimInfo* pAnimInfoList =
        fn_800C1FA4(receiveAnimType, nNumAnims);

    nlVector3 v3BallDirection;
    nlVec3Sub(v3BallDirection, v3BallPosition, v3BallContactPos);
    unsigned short aIncomingDirection = nlAngleDiff(
        nlVector3ToAngle(v3BallDirection),
        mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection);

    const LooseBallContactAnimInfo* pBestAnimInfo = 0;
    const LooseBallContactAnimInfo* pReachableAnimInfo = 0;
    float fBestContactOffset = lbl_806E4040;
    float fDistanceToContact = nlSqrt(nlVec3DistanceSquared2D(
        mUnidentifiedFielder->mUnidentified024.m_v3Position,
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

        cSAnim* pAnim = mUnidentifiedFielder->m_pAnimInventory
                            ->GetAnim(pCurrentAnimInfo->nAnimID);
        nlVector3 v3ContactOffsetWorld;
        nlVector3 v3ContactOffsetLocal;
        mUnidentifiedFielder->GetJointPositionFuture(
            &v3ContactOffsetLocal, pCurrentAnimInfo->nAnimID,
            mUnidentifiedFielder->m_nBallJointIndex,
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
            > fContactOffset - lbl_806DC1C4)
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

void DesireReceivePass::fn_800C1A08()
{
    nlVector3 v3BallPosition;
    if (mbOneTouchShot)
    {
        v3BallPosition = mUnidentifiedFielder->m_pTeam
                             ->GetOtherNet()->m_v3NetLocation;

        nlVector3 v3ToTarget;
        nlVec3Sub(v3ToTarget, v3BallPosition,
            mUnidentifiedFielder->mUnidentified024.m_v3Position);
        mEstimated.aFacingTargetDirection =
            nlVector3ToAngle(v3ToTarget);
        mEstimated.aFacingDirection =
            mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
    }
    else
    {
        v3BallPosition = g_pBall->m_v3Position;
        if (fn_800C0E74())
        {
            unsigned short aFacingDirection =
                mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
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
        fn_800C2048(v3BallPosition,
            mUnidentifiedFielder->mUnidentified024.m_v3Position,
            mEstimated.v3BallContactPos,
            mEstimated.aFacingTargetDirection, meReceiveAnimType);

    if ((meReceiveAnimType & 4) && mbOneTouchPass
        && mpOneTouchPassTarget != 0)
    {
        nlVector3 v3ToTarget;
        nlVec3Sub(v3ToTarget, mpOneTouchPassTarget->mUnidentified024.m_v3Position,
            mUnidentifiedFielder->mUnidentified024.m_v3Position);
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

    mEstimated.mUnidentifiedAnimInfo = pBestBallContactAnimInfo;
    mEstimated.nReceivePassAnim = pBestBallContactAnimInfo->nAnimID;

    cSAnim* pBestContactAnim = mUnidentifiedFielder->m_pAnimInventory
                                   ->GetAnim(mEstimated.nReceivePassAnim);
    unsigned short aDesiredFacingDirection =
        mEstimated.aFacingTargetDirection;
    mEstimated.fReceivePassAnimTime =
        pBestBallContactAnimInfo->fAnimContactFrame
        / (float)pBestContactAnim->m_nNumKeys;

    nlVector3 v3ContactOffsetWorld;
    nlVector3 v3ContactOffsetLocal;
    mUnidentifiedFielder->GetJointPositionFuture(
        &v3ContactOffsetLocal, pBestBallContactAnimInfo->nAnimID,
        mUnidentifiedFielder->m_nBallJointIndex,
        pBestBallContactAnimInfo->fAnimContactFrame
            / (float)mUnidentifiedFielder->m_pAnimInventory
                  ->GetAnim(pBestBallContactAnimInfo->nAnimID)->m_nNumKeys,
        true, true, false, true);

    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, aDesiredFacingDirection);

    nlVec3Set(v3ContactOffsetWorld,
        v3ContactOffsetLocal.x * fCos - v3ContactOffsetLocal.y * fSin,
        v3ContactOffsetLocal.y * fCos + v3ContactOffsetLocal.x * fSin,
        v3ContactOffsetLocal.z);

    nlVec3Sub(mEstimated.v3AnimStartPos,
        mEstimated.v3BallContactPos, v3ContactOffsetWorld);
    mEstimated.v3AnimStartPos.z = 0.0f;
    mEstimated.v3BallContactPos.z = v3ContactOffsetWorld.z;
    mEstimated.fAnimStartOffset =
        nlSqrt(v3ContactOffsetWorld.GetLengthSq3D(), true);

    float fAnimDuration = pBestContactAnim->GetDuration();
    float fAnimContactTime = pBestBallContactAnimInfo->fAnimContactFrame
        / (float)pBestContactAnim->m_nNumKeys;
    mEstimated.fAnimStartTime = mEstimated.fBallContactTime
        - fAnimContactTime * fAnimDuration;
}

bool DesireReceivePass::StartPickupAnimation()
{
    nlVector2 v2Delta = {
        mEstimated.v3AnimStartPos.x
            - mUnidentifiedFielder->mUnidentified024.m_v3Position.x,
        mEstimated.v3AnimStartPos.y
            - mUnidentifiedFielder->mUnidentified024.m_v3Position.y,
    };
    float fDistance = nlSqrt(nlVec2LengthSquared(v2Delta), true);
    float fRadius = mUnidentifiedFielder->mUnidentified320->GetRadius();
    if (fDistance - fRadius
        > mEstimated.fAnimStartOffset + lbl_806DC1C0)
    {
        tDebugPrintManager::Print(DC_AI,
            "DesireReceivePass::StartPickupAnimation - position is outside max threshold !\n");
        return false;
    }

    short sFacingDelta = (short)(mEstimated.aFacingTargetDirection
        - mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection);
    if (mbOneTouchShot && !mbOneTouchShotLate)
    {
        mUnidentifiedFielder->InitActionOneTimer(
            mEstimated.nReceivePassAnim, mEstimated.v3AnimStartPos,
            mEstimated.fReceivePassAnimTime, mbOneTouchVolley,
            sFacingDelta);
    }
    else
    {
        mUnidentifiedFielder->InitActionReceivePass(
            mEstimated.nReceivePassAnim, mEstimated.v3AnimStartPos,
            sFacingDelta, mEstimated.fReceivePassAnimTime);
    }

    float fAnimTime =
        mEstimated.mUnidentifiedAnimInfo->fAnimContactFrame
        / lbl_806E403C;
    float fTimeToIntercept =
        g_pBall->m_tPassTargetTimer.GetSeconds();
    if (fTimeToIntercept < FixedUpdateTask::GetPhysicsUpdateTick())
    {
        fTimeToIntercept = FixedUpdateTask::GetPhysicsUpdateTick();
    }

    float fPlaybackSpeed = nlMinEquals(
        nlMaxEquals(fAnimTime / fTimeToIntercept, lbl_806DC1D0),
        lbl_806DC1D4);
    mUnidentifiedFielder->m_pCurrentAnimController
        ->m_fPlaybackSpeedScale = fPlaybackSpeed;
    return true;
}

void DesireReceivePass::fn_800C22CC(cPlayer* pPasser, bool bVolleyPass, bool bFindPosition,
    bool bPerfectPass, const nlVector3* pv3PassPosition,
    float fMinPassSpeed, float fMaxPassSpeed)
{
    cFielder* pPassTarget = mUnidentifiedFielder;
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
        pPassTarget->mUnidentified024.m_v3Position.x - g_pBall->m_v3Position.x,
        pPassTarget->mUnidentified024.m_v3Position.y - g_pBall->m_v3Position.y,
    };
    float fPassSpeed = DoCalculatePassSpeed(v2BallToTarget,
        fMinPassSpeed, fMaxPassSpeed, lbl_806DC1AC, lbl_806DC1B0);
    if (bFindPosition && lbl_806DC196)
    {
        nlVector3 v3FoundPassPosition;
        if (pv3PassPosition != 0)
        {
            v3FoundPassPosition = *pv3PassPosition;
        }
        else
        {
            eFieldDirection eSearchDirection;
            cFielder* pFielder = mUnidentifiedFielder;
            InterpreterCore* pInterpreter =
                (InterpreterCore*)fn_80311734(this);
            eSearchDirection = (eFieldDirection)
                fn_800C33C8(pInterpreter, "PassDirection", pPasser,
                    pFielder).fn_800C2BD4();

            m_pSpaceSearch = new (8, false)
                SSearchBestPass(pPasser, pFielder,
                    bVolleyPass, bPerfectPass, fPassSpeed);
            pFielder->SetSpaceSearch(m_pSpaceSearch);
            pFielder->m_pSpaceSearch->m_bDebugOn = false;
            pFielder->m_pSpaceSearch->FindBestPosition(
                v3FoundPassPosition,
                pFielder->mUnidentified024.m_v3Position,
                eSearchDirection, &pPasser->mUnidentified024.m_v3Position,
                6.0f, 0xAAAA);

            float fRadius;
            pFielder->m_pPhysicsCharacter->GetRadius(&fRadius);
            fRadius += 0.25f;
            cField::FixOutOfBoundsPosition(
                v3FoundPassPosition, fRadius, true);
        }
        v3PassPosition = v3FoundPassPosition;
    }

    nlVector2 v2BallToPassPosition = {
        v3PassPosition.x - g_pBall->m_v3Position.x,
        v3PassPosition.y - g_pBall->m_v3Position.y,
    };
    float fPassDistance =
        nlVec2Length(v2BallToPassPosition);
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
            fn_800C1FA4(eReceiveAnimType, nNumAnims);
        cSAnim* pAnim = mUnidentifiedFielder->m_pAnimInventory
                            ->GetAnim(pAnimInfo->nAnimID);
        aFacingDirection =
            mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
        mUnidentifiedFielder->GetJointPositionFuture(
            &v3ContactOffsetLocal, pAnimInfo->nAnimID,
            mUnidentifiedFielder->m_nBallJointIndex,
            GetNormalizedContactTime(
                pAnim, pAnimInfo->fAnimContactFrame),
            true, true, false, true);

        nlSinCos(&fSin, &fCos, aFacingDirection);
        v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;
        v3ContactOffsetWorld.x =
            v3ContactOffsetLocal.x * fCos
            - v3ContactOffsetLocal.y * fSin;
        v3ContactOffsetWorld.y =
            v3ContactOffsetLocal.y * fCos
            + v3ContactOffsetLocal.x * fSin;

        v3PassPosition.z = v3ContactOffsetWorld.z;
        g_pBall->ShootAtFast(
            v3BallVelocity, v3PassPosition, fPassTime);
        if (!mUnidentifiedFielder->fn_8003E74C()
            && v3BallVelocity.z > lbl_806DC19C)
        {
            v3BallVelocity.z = lbl_806DC19C;
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
                lbl_806DC1A8, lbl_806DC1A4,
                lbl_806DC1AC, lbl_806DC1B0, fPassDistance);
        }
        else
        {
            v3BallVelocity.z = nlMinEquals(
                0.0f, v3BallVelocity.z);
        }
    }

    fn_80016DF8(g_pBall, pPasser, &v3BallVelocity,
        eSpinType, bVolleyPass && !bHighArc, false);
    pPasser->SetNoPickUpTime(lbl_806DC1A0);
    fn_801B75C8((cFielder*)pPasser, 0, 0, 0, 0);

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

void DesireReceivePass::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field =
        cache->BeginType("DesireReceivePass");
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbValidPassIntercept - (u8*)&mvDesiredPosition,
        "mbValidPassIntercept");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&mv3PassIntercept - (u8*)&mvDesiredPosition,
        "mv3PassIntercept");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&meReceiveAnimType - (u8*)&mvDesiredPosition,
        "meReceiveAnimType");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&meDesireSubState - (u8*)&mvDesiredPosition,
        "meDesireSubState");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchVolley - (u8*)&mvDesiredPosition,
        "mbOneTouchVolley");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchShot - (u8*)&mvDesiredPosition,
        "mbOneTouchShot");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchShotLate - (u8*)&mvDesiredPosition,
        "mbOneTouchShotLate");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbOneTouchPass - (u8*)&mvDesiredPosition,
        "mbOneTouchPass");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpOneTouchPassTarget - (u8*)&mvDesiredPosition,
        "mpOneTouchPassTarget");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mEstimated.bLocked - (u8*)&mvDesiredPosition,
        "mEstimated.bLocked");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&mEstimated.v3BallContactPos - (u8*)&mvDesiredPosition,
        "mEstimated.v3BallContactPos");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&mEstimated.v3AnimStartPos - (u8*)&mvDesiredPosition,
        "mEstimated.v3AnimStartPos");
    cache->AddField(19, gDebugFieldTypes[19].size,
        (u8*)&mEstimated.aFacingDirection - (u8*)&mvDesiredPosition,
        "mEstimated.aFacingDirection");
    cache->AddField(19, gDebugFieldTypes[19].size,
        (u8*)&mEstimated.aFacingTargetDirection - (u8*)&mvDesiredPosition,
        "mEstimated.aFacingTargetDirection");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fBallContactTime - (u8*)&mvDesiredPosition,
        "mEstimated.fBallContactTime");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fAnimStartOffset - (u8*)&mvDesiredPosition,
        "mEstimated.fAnimStartOffset");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fAnimStartTime - (u8*)&mvDesiredPosition,
        "mEstimated.fAnimStartTime");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&mEstimated.nReceivePassAnim - (u8*)&mvDesiredPosition,
        "mEstimated.nReceivePassAnim");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&mEstimated.fReceivePassAnimTime - (u8*)&mvDesiredPosition,
        "mEstimated.fReceivePassAnimTime");
    cache->EndType();
}

void DesireReceivePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireReceivePassType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireReceivePassType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireReceivePassType,
        (u8*)this + offset, sizeof(DesireReceivePass) - offset);
    if (data != 0)
    {
        DesireReceivePass* desire =
            (DesireReceivePass*)((u8*)data - offset);
        desire->mpOneTouchPassTarget =
            (cPlayer*)(mpOneTouchPassTarget == 0
                    ? -1
                    : mpOneTouchPassTarget->mUnidentified120);
        cache->ChecksumData(sDesireReceivePassType, data, context);
    }
}

int Variant::fn_800C2BD4() const
{
    return mData.i;
}

bool Variant::fn_800C2BF8() const
{
    return mData.b;
}

extern "C" UnidentifiedVariant_80054AB8 fn_800C33C8(
    InterpreterCore* pInterpreter, const char* pFunctionName,
    cPlayer* pPlayer, cPlayer* pTarget)
{
    return fn_800C33D8(
        pInterpreter, pPlayer, pFunctionName, pTarget);
}

extern "C" UnidentifiedVariant_80054AB8 fn_800C33D8(
    InterpreterCore* pInterpreter, cPlayer* pPlayer,
    const char* pFunctionName, cPlayer* pTarget)
{
    unsigned int functionHash = nlStringHash(pFunctionName);
    return fn_800C3448(
        pInterpreter, functionHash, pPlayer, pTarget);
}

extern "C" UnidentifiedVariant_80054AB8 fn_800C3448(
    InterpreterCore* pInterpreter, const unsigned int& functionHash,
    cPlayer* pPlayer, cPlayer* pTarget)
{
    unsigned int localHash = functionHash;
    return UnidentifiedVariant_80054AB8(fn_80312360(
        pInterpreter, pInterpreter->FindFunctionEntryPoint(localHash),
        2, pPlayer, pTarget));
}

DesireReceivePass::~DesireReceivePass()
{
}
