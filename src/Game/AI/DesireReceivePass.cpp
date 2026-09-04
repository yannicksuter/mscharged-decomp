#include "Game/AI/DesireReceivePass.h"

#include "Game/AI/DesireSteering.h"

#include <stddef.h>

#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/AnimInventory.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/FixedUpdateTask.h"
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
#include "Game/UnidentifiedPlayerRadius.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <math.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

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

extern "C" DebugFieldType lbl_80533C98[];
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);
extern "C" void fn_80034438(cFielder*);
extern "C" void fn_8003BA94(cFielder*, float);
extern "C" void fn_80017EA0(cBall*, cFielder*, nlVector3*, bool);
extern "C" void fn_80017EC0(cBall*, float);
extern "C" void fn_800180F4(cBall*, nlVector3*, float);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" bool fn_80035F84(cFielder*, nlVector3*, float*, nlVector3*,
    float*, unsigned short, const LooseBallContactAnimInfo*);
extern "C" bool fn_80036234(cFielder*, nlVector3*, float*, nlVector3*,
    float*, const LooseBallContactAnimInfo*, nlVector3*, unsigned short);
extern "C" void fn_8004F594(int, const char*, ...);
extern "C" bool fn_8003E74C(cFielder*);
extern "C" bool fn_8003E7F8(cFielder*);
extern "C" bool fn_8003E84C(cFielder*);
extern "C" float fn_800155A0(cBall*, int);
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
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
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
extern "C" void fn_80097858(cPlayer*, cPlayer*, int,
    int, int, int, float, float);
extern "C" bool fn_80331C04(cGlobalPad*, int, bool);
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
extern float g_fSimulationTick;

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

    nlVector3 v3Delta;
    v3Delta.Sub2D(
        mEstimated.v3AnimStartPos, mUnidentifiedFielder->m_v3Position);
    if (v3Delta.GetLengthSq2D()
        > lbl_806DC1C4 * lbl_806DC1C4)
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

extern "C" void fn_800C0704(DesireReceivePass* pDesire)
{
    cFielder* pFielder = pDesire->mUnidentifiedFielder;
    if (pFielder->GetGlobalPad() != 0)
    {
        fn_80098098(pFielder);
        if (pFielder->GetGlobalPad()->JustPressed(0x1C, true))
        {
            fn_800C089C(pDesire, fn_80035F34(pFielder));
        }
        else if (pFielder->GetGlobalPad()->JustPressed(0x1B, true))
        {
            fn_800C0AE8(
                pDesire, fn_80035F34(pFielder), 0);
        }

        if (pFielder->m_pBall != 0)
        {
            PlayerTweaks* pTweaks = fn_8003E6E4(pFielder);
            float fMaxSpeed = fn_8002C328(pTweaks);
            float fMinSpeed = fn_8002CE14(
                fn_8003E6E4(pFielder));
            fn_8003C268(pFielder, fMinSpeed, fMaxSpeed);
            return;
        }

        if (pDesire->meDesireSubState != 4)
        {
            unsigned short aDirection = 0;
            if (fn_80036A58(pFielder, &aDirection))
            {
                unsigned short aHitDirection =
                    pFielder->m_aActualFacingDirection;
                cAIPad* pAIPad = pFielder->m_pController;
                if (pAIPad != 0
                    && pAIPad->GetMovementStickMagnitude() > 0.001f)
                {
                    aHitDirection =
                        pAIPad->GetMovementStickDirection();
                }
                pFielder->InitActionHit(0, aHitDirection);
                return;
            }

            if (fn_80036C8C(pFielder, &aDirection))
            {
                pFielder->InitActionSlideAttack(
                    0, aDirection, -1.0f);
                fn_80316968(pDesire);
            }
        }
    }
}

extern "C" void fn_800C089C(
    DesireReceivePass* pDesire, bool bVolleyPass)
{
    float fPassProgress = 0.0f;
    if (g_pBall->m_fTotalPassTime > 0.0f)
    {
        fPassProgress = 1.0f
            - g_pBall->m_tPassTargetTimer.GetSeconds()
                / g_pBall->m_fTotalPassTime;
    }

    bool bSpecialReceive =
        fn_8003E7F8(pDesire->mUnidentifiedFielder)
        || fn_8003E84C(pDesire->mUnidentifiedFielder);
    bool bCanOneTouch =
        !(bSpecialReceive
                && pDesire->mUnidentifiedFielder->m_pBall != 0)
        && !(fPassProgress < lbl_806DC1B4
            && pDesire->mUnidentifiedTimer.GetSeconds()
                < lbl_806DC1B8)
        && !(pDesire->meDesireSubState == 4
            && (pDesire->mbOneTouchShot
                || pDesire->mbOneTouchPass));
    if (!bCanOneTouch)
    {
        return;
    }

    pDesire->mbOneTouchPass = false;
    pDesire->mbOneTouchShot = true;
    pDesire->mbOneTouchVolley = bVolleyPass;
    bSpecialReceive =
        fn_8003E7F8(pDesire->mUnidentifiedFielder)
        || fn_8003E84C(pDesire->mUnidentifiedFielder);
    if (pDesire->meDesireSubState == 4 && !bSpecialReceive)
    {
        pDesire->mbOneTouchShotLate = true;
        return;
    }

    if (pDesire->meDesireSubState != 0)
    {
        pDesire->meDesireSubState = 0;
        pDesire->mEstimated.bLocked = false;
    }

    int eReceiveAnimType = pDesire->meReceiveAnimType;
    if (pDesire->fn_800C0E54()
        && (pDesire->mbOneTouchVolley || bSpecialReceive))
    {
        int eOneTouchReceiveAnimType = 4;
        if (pDesire->fn_800C0E54())
        {
            eOneTouchReceiveAnimType |= 1;
        }
        float fBallContactTime =
            pDesire->mEstimated.fBallContactTime;
        pDesire->mEstimated.fBallContactTime = -1.0f;
        if (bSpecialReceive
            || pDesire->CalcRoughEstimates(
                eOneTouchReceiveAnimType))
        {
            pDesire->meReceiveAnimType =
                eOneTouchReceiveAnimType;
            return;
        }
        pDesire->mEstimated.fBallContactTime =
            fBallContactTime;
        return;
    }

    eReceiveAnimType = 8;
    if (pDesire->fn_800C0E54())
    {
        eReceiveAnimType |= 1;
    }
    pDesire->meReceiveAnimType = eReceiveAnimType;
}

extern "C" void fn_800C0AE8(DesireReceivePass* pDesire,
    bool bVolleyPass, cPlayer* pPassTarget)
{
    float fPassProgress = 0.0f;
    if (g_pBall->m_fTotalPassTime > 0.0f)
    {
        fPassProgress = 1.0f
            - g_pBall->m_tPassTargetTimer.GetSeconds()
                / g_pBall->m_fTotalPassTime;
    }

    bool bSpecialReceive =
        fn_8003E7F8(pDesire->mUnidentifiedFielder)
        || fn_8003E84C(pDesire->mUnidentifiedFielder);
    bool bCanOneTouch =
        !(bSpecialReceive
                && pDesire->mUnidentifiedFielder->m_pBall != 0)
        && !(fPassProgress < lbl_806DC1B4
            && pDesire->mUnidentifiedTimer.GetSeconds()
                < lbl_806DC1B8)
        && !(pDesire->meDesireSubState == 4
            && (pDesire->mbOneTouchShot
                || pDesire->mbOneTouchPass));
    if (!bCanOneTouch)
    {
        return;
    }

    pDesire->mbOneTouchShot = false;
    pDesire->mbOneTouchShotLate = false;
    pDesire->mbOneTouchVolley = false;
    if (pPassTarget == 0)
    {
        pPassTarget = fn_80096F54(
            pDesire->mUnidentifiedFielder, false);
    }
    if (pPassTarget == 0)
    {
        fn_800C089C(pDesire, bVolleyPass);
        return;
    }

    pDesire->mbOneTouchVolley = bVolleyPass;
    pDesire->mbOneTouchPass = true;
    pDesire->mpOneTouchPassTarget = pPassTarget;
    bSpecialReceive =
        fn_8003E7F8(pDesire->mUnidentifiedFielder)
        || fn_8003E84C(pDesire->mUnidentifiedFielder);
    if (pDesire->meDesireSubState == 4 && !bSpecialReceive)
    {
        return;
    }

    if (pDesire->meDesireSubState != 0)
    {
        pDesire->meDesireSubState = 0;
        pDesire->mEstimated.bLocked = false;
    }

    if (pDesire->fn_800C0E54()
        && (pDesire->mbOneTouchVolley || bSpecialReceive))
    {
        int eOneTouchReceiveAnimType = 4;
        if (pDesire->fn_800C0E54())
        {
            eOneTouchReceiveAnimType |= 1;
        }
        float fBallContactTime =
            pDesire->mEstimated.fBallContactTime;
        pDesire->mEstimated.fBallContactTime = -1.0f;
        if (bSpecialReceive
            || pDesire->CalcRoughEstimates(
                eOneTouchReceiveAnimType))
        {
            pDesire->meReceiveAnimType =
                eOneTouchReceiveAnimType;
            return;
        }
        pDesire->mEstimated.fBallContactTime =
            fBallContactTime;
        return;
    }

    int eReceiveAnimType = 2;
    if (pDesire->fn_800C0E54())
    {
        eReceiveAnimType |= 1;
    }
    pDesire->meReceiveAnimType = eReceiveAnimType;
}

void DesireReceivePass::UnidentifiedCleanup()
{
    if (mUnidentifiedFielder->m_pBall == 0)
    {
        fn_80034438(mUnidentifiedFielder);
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
            mUnidentifiedFielder->m_v3Position.x - mv3PassIntercept.x,
            mUnidentifiedFielder->m_v3Position.y - mv3PassIntercept.y,
        };
        result = nlVec2LengthSquared(v2Delta) > lbl_806DC1C4;
        break;
    }
    }
    return result;
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
        mUnidentifiedFielder->m_aActualFacingDirection;
    mUnidentifiedFielder->GetJointPositionFuture(
        &v3ContactOffsetLocal, pAnimInfo->nAnimID,
        mUnidentifiedFielder->m_nBallJointIndex,
        pAnimInfo->fAnimContactFrame / (float)pAnim->m_nNumKeys,
        true, true, false, true);

    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, aFacingDirection);

    nlVector3 v3ContactOffsetWorld;
    v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;
    const float fRotationCos = fCos;
    const float fContactOffsetX = v3ContactOffsetLocal.x;
    v3ContactOffsetWorld.x = (fContactOffsetX * fRotationCos)
        - (v3ContactOffsetLocal.y * fSin);
    v3ContactOffsetWorld.y = (v3ContactOffsetLocal.y * fRotationCos)
        + (fContactOffsetX * fSin);

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
            fn_8004F594(4,
                "DesireReceivePass::CalcRoughEstimates - failed to find an AIR interception point!\n");
            return false;
        }
    }

    if (bUseGroundIntercept)
    {
        float fRadius =
            mUnidentifiedFielder->mUnidentified320->GetRadius();
        float fMaxCatchupSpeed = fn_8002E1B0(mUnidentifiedFielder);
        CalcInterceptXY(mUnidentifiedFielder->m_v3Position,
            fMaxCatchupSpeed, fRadius, g_pBall->m_v3Position,
            g_pBall->m_v3Velocity, nNumIntercepts, fInterceptTimes);

        if (nNumIntercepts == 0)
        {
            fn_8004F594(4,
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
                mUnidentifiedFielder->m_v3Position);

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
                    mUnidentifiedFielder->m_v3Position);
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
        fn_8004F594(4,
            "DesireReceivePass::CalcRoughEstimates - the ball got deflected too much, pass aborted\n");
        return false;
    }

    cField::FixOutOfBoundsPosition(estimated.v3BallContactPos,
        mUnidentifiedFielder->mUnidentified320->GetRadius(), true);

    nlVector3 v3FacingDirection;
    nlVec3Sub(v3FacingDirection, estimated.v3BallContactPos,
        mUnidentifiedFielder->m_v3Position);
    if (!fn_800C0E74())
    {
        nlVec3Sub(v3FacingDirection, g_pBall->m_v3Position,
            mUnidentifiedFielder->m_v3Position);
    }
    estimated.aFacingDirection =
        nlVector3ToAngle(v3FacingDirection);

    fn_80017EC0(g_pBall, estimated.fBallContactTime);
    fn_80017EA0(g_pBall, mUnidentifiedFielder,
        &estimated.v3BallContactPos, fn_800C0E54());

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
        fn_80017EC0(g_pBall, mEstimated.fBallContactTime);
        fn_80017EA0(g_pBall, mUnidentifiedFielder,
            &mEstimated.v3BallContactPos, fn_800C0E54());
    }
    else
    {
        fn_8004F594(4,
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
    case 3:
        nNumAnims = 8;
        pAnimInfo = lbl_804DC280;
        break;
    case 8:
        nNumAnims = 4;
        pAnimInfo = lbl_804DC1F0;
        break;
    case 9:
        nNumAnims = 2;
        pAnimInfo = lbl_804DC310;
        break;
    case 16:
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
    unsigned short aActualFacingDirection =
        mUnidentifiedFielder->m_aActualFacingDirection;
    unsigned short aIncomingDirection =
        nlVector3ToAngle(v3BallDirection) - aActualFacingDirection;

    const LooseBallContactAnimInfo* pBestAnimInfo = 0;
    const LooseBallContactAnimInfo* pReachableAnimInfo = 0;
    float fBestContactOffset = lbl_806E4040;
    nlVector2 v2DistanceToContact = {
        mUnidentifiedFielder->m_v3Position.x - v3BallContactPos.x,
        mUnidentifiedFielder->m_v3Position.y - v3BallContactPos.y,
    };
    float fDistanceToContact = nlVec2Length(v2DistanceToContact);

    const LooseBallContactAnimInfo* pAnimInfo = pAnimInfoList;
    for (int i = 0; i < nNumAnims; ++i, ++pAnimInfo)
    {
        const LooseBallContactAnimInfo* pCurrentAnimInfo = 0;
        if (pAnimInfo->aIncomingAngleMin
            < pAnimInfo->aIncomingAngleMax)
        {
            if (aIncomingDirection
                    >= pAnimInfo->aIncomingAngleMin
                && aIncomingDirection
                    <= pAnimInfo->aIncomingAngleMax)
            {
                pCurrentAnimInfo = &pAnimInfoList[i];
            }
        }
        else
        {
            if (aIncomingDirection
                    >= pAnimInfo->aIncomingAngleMin
                || aIncomingDirection
                    <= pAnimInfo->aIncomingAngleMax)
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
            pCurrentAnimInfo->fAnimContactFrame
                / (float)pAnim->m_nNumKeys,
            true, true, false, true);

        float fSin;
        float fCos;
        nlSinCos(&fSin, &fCos, aFacingDirection);

        v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;
        v3ContactOffsetWorld.x =
            v3ContactOffsetLocal.x * fCos
            - v3ContactOffsetLocal.y * fSin;
        v3ContactOffsetWorld.y =
            v3ContactOffsetLocal.y * fCos
            + v3ContactOffsetLocal.x * fSin;

        float fContactOffset = nlVec2Length(
            *(nlVector2*)&v3ContactOffsetWorld);
        if (fDistanceToContact
            > fContactOffset - lbl_806DC1C4)
        {
            pReachableAnimInfo = pCurrentAnimInfo;
            break;
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
            mUnidentifiedFielder->m_v3Position);
        mEstimated.aFacingTargetDirection =
            nlVector3ToAngle(v3ToTarget);
        mEstimated.aFacingDirection =
            mUnidentifiedFielder->m_aActualFacingDirection;
    }
    else
    {
        v3BallPosition = g_pBall->m_v3Position;
        if (fn_800C0E74())
        {
            unsigned short aFacingDirection =
                mUnidentifiedFielder->m_aActualFacingDirection;
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
            mUnidentifiedFielder->m_v3Position,
            mEstimated.v3BallContactPos,
            mEstimated.aFacingTargetDirection, meReceiveAnimType);

    if ((meReceiveAnimType & 4) && mbOneTouchPass
        && mpOneTouchPassTarget != 0)
    {
        nlVector3 v3ToTarget;
        nlVec3Sub(v3ToTarget, mpOneTouchPassTarget->m_v3Position,
            mUnidentifiedFielder->m_v3Position);
        mEstimated.aFacingTargetDirection =
            nlVector3ToAngle(v3ToTarget);

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
    }

    mEstimated.mUnidentifiedAnimInfo = pBestBallContactAnimInfo;
    mEstimated.nReceivePassAnim = pBestBallContactAnimInfo->nAnimID;

    cSAnim* pBestContactAnim = mUnidentifiedFielder->m_pAnimInventory
                                   ->GetAnim(pBestBallContactAnimInfo->nAnimID);
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

    v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;
    const float fRotationCos = fCos;
    const float fContactOffsetX = v3ContactOffsetLocal.x;
    v3ContactOffsetWorld.x = (fContactOffsetX * fRotationCos)
        - (v3ContactOffsetLocal.y * fSin);
    v3ContactOffsetWorld.y = (v3ContactOffsetLocal.y * fRotationCos)
        + (fContactOffsetX * fSin);

    nlVec3Sub(mEstimated.v3AnimStartPos,
        mEstimated.v3BallContactPos, v3ContactOffsetWorld);
    mEstimated.v3AnimStartPos.z = 0.0f;
    mEstimated.v3BallContactPos.z = v3ContactOffsetWorld.z;
    mEstimated.fAnimStartOffset =
        nlSqrt(v3ContactOffsetWorld.GetLengthSq3D(), true);

    mEstimated.fAnimStartTime = mEstimated.fBallContactTime
        - (pBestBallContactAnimInfo->fAnimContactFrame
              / (float)pBestContactAnim->m_nNumKeys)
            * ((float)pBestContactAnim->m_nNumKeys / lbl_806E403C);
}

bool DesireReceivePass::StartPickupAnimation()
{
    nlVector2 v2Delta = {
        mEstimated.v3AnimStartPos.x
            - mUnidentifiedFielder->m_v3Position.x,
        mEstimated.v3AnimStartPos.y
            - mUnidentifiedFielder->m_v3Position.y,
    };
    float fDistance = nlSqrt(nlVec2LengthSquared(v2Delta), true);
    float fRadius = mUnidentifiedFielder->mUnidentified320->GetRadius();
    if (fDistance - fRadius
        > mEstimated.fAnimStartOffset + lbl_806DC1C0)
    {
        fn_8004F594(4,
            "DesireReceivePass::StartPickupAnimation - position is outside max threshold !\n");
        return false;
    }

    short sFacingDelta = (short)(mEstimated.aFacingTargetDirection
        - mUnidentifiedFielder->m_aActualFacingDirection);
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

extern "C" void fn_800C22CC(DesireReceivePass* pDesire,
    cPlayer* pPasser, bool bVolleyPass, bool bFindPosition,
    bool bPerfectPass, const nlVector3* pv3PassPosition,
    float fMinPassSpeed, float fMaxPassSpeed)
{
    cFielder* pPassTarget = pDesire->mUnidentifiedFielder;
    int eReceiveAnimType = 2;
    if (bVolleyPass)
    {
        if (fn_8003E7F8(pPassTarget)
            || fn_8003E84C(pPassTarget))
        {
            eReceiveAnimType = 16;
        }
        else
        {
            eReceiveAnimType = 3;
        }
    }

    nlVector3 v3PassPosition = pPassTarget->m_v3Position;
    nlVector2 v2BallToTarget = {
        pPassTarget->m_v3Position.x - g_pBall->m_v3Position.x,
        pPassTarget->m_v3Position.y - g_pBall->m_v3Position.y,
    };
    float fPassCharge = NormalizeVal(
                            nlSqrt(nlVec2LengthSquared(v2BallToTarget), true),
                            lbl_806DC1AC, lbl_806DC1B0)
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

    float fPassSpeed = Interpolate(
        fMinPassSpeed, fMaxPassSpeed, fPassCharge);
    if (bFindPosition && lbl_806DC196)
    {
        if (pv3PassPosition != 0)
        {
            v3PassPosition = *pv3PassPosition;
        }
        else
        {
            InterpreterCore* pInterpreter =
                (InterpreterCore*)fn_80311734(pDesire);
            UnidentifiedVariant_80054AB8 passDirection =
                fn_800C33C8(pInterpreter, "PassDirection",
                    pPasser, pPassTarget);
            eFieldDirection eSearchDirection =
                (eFieldDirection)passDirection.mData.i;

            pDesire->m_pSpaceSearch = new (8, false)
                SSearchBestPass(pPasser, pPassTarget,
                    bVolleyPass, bPerfectPass, fPassSpeed);
            pPassTarget->SetSpaceSearch(pDesire->m_pSpaceSearch);
            pPassTarget->m_pSpaceSearch->m_bDebugOn = false;
            pPassTarget->m_pSpaceSearch->FindBestPosition(
                v3PassPosition, pPassTarget->m_v3Position,
                eSearchDirection, &pPasser->m_v3Position,
                6.0f, 0xAAAA);

            float fRadius;
            pPassTarget->m_pPhysicsCharacter->GetRadius(&fRadius);
            fRadius += 0.25f;
            cField::FixOutOfBoundsPosition(
                v3PassPosition, fRadius, true);
        }
    }

    nlVector2 v2BallToPassPosition = {
        v3PassPosition.x - g_pBall->m_v3Position.x,
        v3PassPosition.y - g_pBall->m_v3Position.y,
    };
    float fPassDistance =
        nlSqrt(nlVec2LengthSquared(v2BallToPassPosition), true);
    float fPassTime = nlMaxEquals(
        0.04f, fPassDistance / fPassSpeed);

    nlVector3 v3BallVelocity;
    int eSpinType = SPINTYPE_ROLLING;
    bool bHighArc = false;
    if (bVolleyPass)
    {
        eSpinType = SPINTYPE_BACK;
        int nNumAnims;
        const LooseBallContactAnimInfo* pAnimInfo =
            pDesire->fn_800C1FA4(eReceiveAnimType, nNumAnims);
        cSAnim* pAnim = pPassTarget->m_pAnimInventory
                            ->GetAnim(pAnimInfo->nAnimID);
        nlVector3 v3ContactOffsetLocal;
        pPassTarget->GetJointPositionFuture(
            &v3ContactOffsetLocal, pAnimInfo->nAnimID,
            pPassTarget->m_nBallJointIndex,
            pAnimInfo->fAnimContactFrame / (float)pAnim->m_nNumKeys,
            true, true, false, true);

        float fSin;
        float fCos;
        nlSinCos(&fSin, &fCos,
            pPassTarget->m_aActualFacingDirection);
        nlVector3 v3ContactOffsetWorld;
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
        if (!fn_8003E74C(pPassTarget)
            && v3BallVelocity.z > lbl_806DC19C)
        {
            v3BallVelocity.z = lbl_806DC19C;
            bHighArc = true;
        }
    }
    else
    {
        nlVector3 v3BallToPassPosition;
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
            v3BallVelocity.z = nlMaxEquals(
                0.0f, v3BallVelocity.z);
        }
    }

    fn_80016DF8(g_pBall, pPasser, &v3BallVelocity,
        eSpinType, bVolleyPass && !bHighArc, false);
    pPasser->SetNoPickUpTime(lbl_806DC1A0);
    fn_801B75C8((cFielder*)pPasser, 0, 0, 0, 0);

    if (pPassTarget->CanReceivePass() && !bHighArc)
    {
        PassBallData eventData;
        eventData.pPasser = pPasser;
        eventData.pTarget = pPassTarget;
        eventData.bVolleyPass = bVolleyPass;
        eventData.mPasserControllerID = pPasser->GetGlobalPad() != 0
            ? pPasser->GetGlobalPad()->fn_80332748()
            : -1;
        g_pGame->mUnidentified49C.mEvent16.Dispatch(
            &eventData, Function<PassBallData*>(), true);

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

    if (g_pGame->m_eGameState == 3
        || g_pGame->m_eGameState == 2)
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
        fn_80338EBC(cache, "DesireReceivePass");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbValidPassIntercept - (u8*)&mvDesiredPosition,
        "mbValidPassIntercept");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&mv3PassIntercept - (u8*)&mvDesiredPosition,
        "mv3PassIntercept");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&meReceiveAnimType - (u8*)&mvDesiredPosition,
        "meReceiveAnimType");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&meDesireSubState - (u8*)&mvDesiredPosition,
        "meDesireSubState");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbOneTouchVolley - (u8*)&mvDesiredPosition,
        "mbOneTouchVolley");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbOneTouchShot - (u8*)&mvDesiredPosition,
        "mbOneTouchShot");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbOneTouchShotLate - (u8*)&mvDesiredPosition,
        "mbOneTouchShotLate");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbOneTouchPass - (u8*)&mvDesiredPosition,
        "mbOneTouchPass");
    fn_80338F88(cache, 15, lbl_80533C98[15].size,
        (u8*)&mpOneTouchPassTarget - (u8*)&mvDesiredPosition,
        "mpOneTouchPassTarget");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mEstimated.bLocked - (u8*)&mvDesiredPosition,
        "mEstimated.bLocked");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&mEstimated.v3BallContactPos - (u8*)&mvDesiredPosition,
        "mEstimated.v3BallContactPos");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        (u8*)&mEstimated.v3AnimStartPos - (u8*)&mvDesiredPosition,
        "mEstimated.v3AnimStartPos");
    fn_80338F88(cache, 19, lbl_80533C98[19].size,
        (u8*)&mEstimated.aFacingDirection - (u8*)&mvDesiredPosition,
        "mEstimated.aFacingDirection");
    fn_80338F88(cache, 19, lbl_80533C98[19].size,
        (u8*)&mEstimated.aFacingTargetDirection - (u8*)&mvDesiredPosition,
        "mEstimated.aFacingTargetDirection");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&mEstimated.fBallContactTime - (u8*)&mvDesiredPosition,
        "mEstimated.fBallContactTime");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&mEstimated.fAnimStartOffset - (u8*)&mvDesiredPosition,
        "mEstimated.fAnimStartOffset");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&mEstimated.fAnimStartTime - (u8*)&mvDesiredPosition,
        "mEstimated.fAnimStartTime");
    fn_80338F88(cache, 8, lbl_80533C98[8].size,
        (u8*)&mEstimated.nReceivePassAnim - (u8*)&mvDesiredPosition,
        "mEstimated.nReceivePassAnim");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&mEstimated.fReceivePassAnimTime - (u8*)&mvDesiredPosition,
        "mEstimated.fReceivePassAnimTime");
    fn_80338F78(cache);
}

void DesireReceivePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireReceivePassType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireReceivePassType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = fn_8033930C(cache, sDesireReceivePassType,
        (u8*)this + offset, sizeof(DesireReceivePass) - offset);
    if (data != 0)
    {
        DesireReceivePass* desire =
            (DesireReceivePass*)((u8*)data - offset);
        desire->mpOneTouchPassTarget =
            (cPlayer*)(mpOneTouchPassTarget == 0
                    ? -1
                    : mpOneTouchPassTarget->mUnidentified120);
        fn_80339450(
            cache, sDesireReceivePassType, data, context);
    }
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
        pInterpreter, fn_802DF3E4(pInterpreter, &localHash),
        2, pPlayer, pTarget));
}

DesireReceivePass::~DesireReceivePass()
{
}
