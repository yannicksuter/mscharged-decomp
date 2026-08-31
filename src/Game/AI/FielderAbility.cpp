#include "Game/AI/Fielder.h"
#include "unclassified/tu_8019FE24.h"

#include "Game/AI/Fuzzy.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/CharacterTweaks.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/SAnim.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/globalpad.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "types.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static float gUnidentified804FAD80[4] = { 0.15f, 0.4f, 0.0f, 0.0f };

extern float lbl_806DB9C8;
extern float lbl_806DB9CC;
extern float lbl_806DB9D0;
extern float lbl_806DB9D4;
extern float lbl_806DB9D8;
extern float lbl_806DB9DC;
extern float lbl_806DB9E0;
extern float lbl_806DB9E4;
extern float lbl_806DB9E8;
extern float lbl_806DB9EC;
extern float lbl_806DB9F0;
extern float lbl_806DB9F4;
extern float lbl_806DB9F8;
extern float lbl_806DB9FC;
extern float lbl_806DBA00;
extern float lbl_806DBA04;
extern float lbl_806DBA08;
extern int lbl_806DBA0C;
extern float lbl_806DBA10;
extern float lbl_806E0C78;
extern bool lbl_806E0C7C;
extern float lbl_806E364C;
extern float lbl_806E3650;
extern float lbl_806E3658;

extern const FuzzyVariant fvNotSet;

struct UnidentifiedAbilityEvent
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ nlVector3 v3Position;
    /* 0x10 */ float fParam1;
    /* 0x14 */ float fParam2;
};

struct UnidentifiedAbilityEffect
{
    /* 0x00 */ u8 mUnknown00[0x08];
    /* 0x08 */ void* mUnidentified08;
};

extern "C" void fn_8002E3F8(cFielder* pFielder);
extern "C" bool fn_8002EDC8(cFielder* pFielder, int nParam);
extern "C" void fn_8002FE54(cFielder* pFielder);
extern "C" void fn_800301E8(cFielder* pFielder);
extern "C" bool fn_80034964(cFielder* pFielder);
extern "C" void fn_80038158(cFielder* pFielder, int nParam);
extern "C" void fn_80040368(cFielder* pFielder);
extern "C" void* fn_8002E1A4(cFielder* pFielder);
extern "C" bool fn_80319FEC(void* pParam, int nAction);
extern "C" void fn_80319E58(void* pParam, int nAction);
extern "C" void fn_80319E84(void* pParam, int nAction, int nParam1, int nParam2);
extern "C" bool fn_8002D2C4(nlVector3* v3Position, float fParam, int nParam);
extern "C" void fn_80060014(cGame* pGame, void* pEvent);
extern "C" void fn_8005FC1C(cGame* pGame, void* pEvent);
extern "C" void fn_8005FE18(cGame* pGame, void* pEvent);
extern "C" void fn_80060210(cGame* pGame, void* pEvent);
extern "C" void fn_800EC12C(unsigned long soundID, void* pParam);
extern "C" void fn_800EBBFC(
    int nParam, unsigned long soundID, const void* pParam, void* pContext);
extern "C" void fn_800F026C(float* pParams, float fParam1, float fParam2);
extern "C" void fn_80061B1C(int nParam, float fParam1, float fParam2);
extern "C" void fn_80111D7C(float fParam);
extern "C" void fn_80139D1C(int nPreset, cGlobalPad* pPad);
extern "C" void fn_801765C8(
    cFielder* pFielder, const nlVector3* v3Position, float fParam);
extern "C" void fn_80178D0C(void* pParam);
extern "C" void fn_801B897C(cFielder* pFielder);
extern "C" void fn_801BAF98(cFielder* pFielder);
extern "C" void fn_801BB0DC(cFielder* pFielder);
extern "C" void fn_801BB120(cFielder* pFielder);
extern "C" bool fn_80332770(void);

void cFielder::fn_8004F828()
{
    fn_8002E3F8(this);
    SetAction((eFielderActionState)0x1D);
    SetAnimState(0x68, true, 0.2f, false, false);
    muInvincibleStatus |= 1;
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    InitMovementDecelerateExponential(lbl_806DB9C8);
    Unknown8(m_aActualFacingDirection, false);
    m_aDesiredMovementDirection = m_aActualMovementDirection;
    m_fDesiredSpeed = 0.0f;
    fn_801BAF98(this);
}

void cFielder::fn_8004F8E8()
{
    fn_801BB120(this);
    fn_800F026C(gUnidentified804FAD80, lbl_806DB9D0, lbl_806DB9D4);
    fn_80061B1C(1, lbl_806DBA10 * m_v3Position.y,
        lbl_806DBA10 * m_v3Position.x);
    fn_801765C8(
        this, &GetJointPosition(m_nHeadJointIndex), lbl_806DB9D8);
    fn_80139D1C(4, GetGlobalPad());
}

void cFielder::fn_8004F974(float fDeltaT)
{
    m_fDesiredSpeed = 0.0f;

    if (m_pCurrentAnimController->TestFrameTrigger(lbl_806DB9CC))
    {
        fn_801BB0DC(this);
        m_fActualSpeed = 0.0f;
        m_fDesiredSpeed = 0.0f;
        SetVelocity(v3Zero);
        Unknown8(m_aActualFacingDirection, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::fn_8004FA34()
{
    fn_80038158(this, 0);

    if (m_pCurrentAnimController->m_fTime
            * (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys
        < lbl_806DBA04)
    {
        UnidentifiedAbilityEvent event;
        event.v3Position = m_v3Position;
        event.fParam1 = lbl_806DB9F4;
        event.fParam2 = lbl_806DB9F8;
        event.pFielder = this;
        fn_80060210(lbl_806E0C94, &event);
    }

    if (!lbl_806E0C94->IsGameplayOrOvertime())
    {
        fn_80040368(this);
    }
}

void cFielder::fn_8004FB04()
{
    fn_8002E3F8(this);
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x1D);
    muInvincibleStatus |= 1;

    if ((u16)abs_s16((s16)(m_aActualFacingDirection
            - (u16)(s32)(65536.0f * lbl_806DBA08)))
        < 0x4000)
    {
        SetAnimState(0x68, true, 0.2f, false, false);
    }
    else
    {
        SetAnimState(0x69, true, 0.2f, false, false);
    }

    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    Unknown8(m_aActualFacingDirection, false);
    m_aDesiredMovementDirection = m_aActualMovementDirection;
    m_fDesiredSpeed = 0.0f;

    UnidentifiedAbilityEvent event;
    event.v3Position = m_v3Position;
    event.fParam1 = lbl_806DB9F4;
    event.fParam2 = lbl_806DB9F8;
    event.pFielder = this;
    fn_80060014(lbl_806E0C94, &event);

    float fParam = FMAX(lbl_806DB9F4, lbl_806DB9F8);
    fParam += 0.5f;
    fn_801A0500(lbl_806DBA0C, this, fParam);
}

void cFielder::fn_8004FC90(float fDeltaT)
{
    float fFrame = m_pCurrentAnimController->m_fTime
        * (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys
        / lbl_806DB9FC;

    if (fFrame <= 1.0f)
    {
        u32 aFacing = m_aActualFacingDirection;
        float fBlend = fFrame * (lbl_806E3650 * fFrame + lbl_806E364C);
        fBlend = fFrame * fBlend;
        float fTurn = lbl_806DBA08 - (float)aFacing / 65536.0f;
        if (m_eAnimID == 0x69)
        {
            fTurn += 0.5f;
        }
        if (fTurn > 0.5f)
        {
            fTurn -= 1.0f;
        }
        else if (fTurn <= -0.5f)
        {
            fTurn += 1.0f;
        }
        fTurn = fTurn * fBlend;
        SetFacingDirection(
            (u16)(aFacing
                + (s16)(s32)(65536.0f * (fTurn * fBlend))),
            true);
    }

    if (m_pCurrentAnimController->TestFrameTrigger(lbl_806DB9FC))
    {
        UnidentifiedAbilityEvent event;
        event.v3Position = m_v3Position;
        event.fParam1 = lbl_806DB9F4;
        event.fParam2 = lbl_806DB9F8;
        event.pFielder = this;
        fn_8005FE18(lbl_806E0C94, &event);
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(lbl_806DBA00))
    {
        UnidentifiedAbilityEvent event;
        event.v3Position = m_v3Position;
        event.fParam1 = lbl_806DB9F4;
        event.fParam2 = lbl_806DB9F8;
        fn_8002D2C4(&event.v3Position, lbl_806DB9F4, 1);
        event.pFielder = this;
        fn_8005FC1C(lbl_806E0C94, &event);

        if (lbl_806E0C7C)
        {
            fn_801B897C(this);
        }

        if (m_pBall == 0)
        {
            fn_800EBBFC(mUnidentified318, 0x7997624D, 0, 0);
        }
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(
                 1.0f + lbl_806DBA00))
    {
        if (!fn_80332770())
        {
            fn_80111D7C(lbl_806DB9F0);
        }
        fn_80038158(this, 0);
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(lbl_806DBA04))
    {
        fn_801A0C58(0);

        UnidentifiedAbilityEvent event;
        event.v3Position = m_v3Position;
        event.fParam1 = lbl_806DB9F4;
        event.fParam2 = lbl_806DB9F8;
        event.pFielder = this;
        fn_80060210(lbl_806E0C94, &event);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::fn_8004FF40()
{
    if (!IsFallenDown())
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)5);
        SetAnimState(0x68, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        m_fDesiredSpeed = 0.0f;
    }
}

float cFielder::fn_8004FFF8()
{
    return mUnidentified3E0 / mUnidentified3E4;
}

void cFielder::fn_80050008()
{
    mUnidentified3E0 = 0.0f;
    mUnidentified3E4 = 0.0f;
    fn_8005001C(true);
}

void cFielder::fn_8005001C(bool bForce)
{
    if (m_eCharacterClass == DAISY)
    {
        if (mUnidentified3DC || bForce)
        {
            mUnidentified3DC = false;
            mUnidentified3E0 = mUnidentified3E0 - lbl_806DB9E8;
            bool bRunning = mUnidentified3E0 > 0.0f;
            if (bRunning)
            {
                if (mUnidentified3E0 < lbl_806E3658)
                {
                    mUnidentified3E0 = lbl_806E3658;
                }
            }
            mUnidentified3E8 = 0.0f;
            fn_8002FE54(this);
            fn_800EC12C(0x8A9FCF66, this);
        }
    }
    else if (m_eCharacterClass == MARIO)
    {
        if (mUnidentified3DC)
        {
            if (mUnidentified3F8 <= 0.0f || bForce)
            {
                mUnidentified3DC = false;
                mUnidentified3DD = false;
                mUnidentified3E0 = mUnidentified3E0 - lbl_806E0C78;
                bool bRunning = mUnidentified3E0 > 0.0f;
                if (bRunning)
                {
                    if (mUnidentified3E0 < mUnidentified3FC)
                    {
                        mUnidentified3E0 = mUnidentified3FC;
                    }
                }
                fn_8002FE54(this);
                if (fn_80319FEC(fn_8002E1A4(this), 0x23))
                {
                    fn_80319E58(fn_8002E1A4(this), 0x23);
                }
                fn_800EC12C(0x8A9FCF66, this);
            }
            else
            {
                mUnidentified3DD = true;
            }
        }
    }
    else if (m_eCharacterClass == PEACH)
    {
        if (mUnidentified3DC || bForce)
        {
            mUnidentified3DC = false;
            fn_800EC12C(0x8A9FCF66, this);
        }
    }
    else if (m_eCharacterClass == YOSHI)
    {
        if (mUnidentified3DC || bForce)
        {
            mUnidentified3DC = false;
            mUnidentified408 = 0.0f;
            fn_8002FE54(this);
        }
    }
}

void cFielder::fn_800501F0(bool bParam)
{
    switch (m_eCharacterClass)
    {
    case DAISY:
        mUnidentified3E4 = lbl_806DB9E4;
        mUnidentified3E8 = 0.0f;
        break;
    case MARIO:
        mUnidentified3E4 = lbl_806DB9DC;
        mUnidentified3F8 = mUnidentified3FC = lbl_806DB9E0;
        break;
    case PEACH:
        mUnidentified3E4 = lbl_806DB9EC;
        mUnidentified3F4 = 0.0f;
        break;
    case YOSHI:
        mUnidentified3E4 = 1.0f;
        break;
    }

    mUnidentified3E0 = mUnidentified3E4;

    if (bParam)
    {
        fn_80050284();
    }
}

bool cFielder::fn_80050284()
{
    if (!fn_8002EDC8(this, -1))
    {
        return false;
    }

    if (!mUnidentified3DC)
    {
        if (m_eCharacterClass == YOSHI)
        {
            fn_800EBBFC(mUnidentified318, 0x8A9FCF66, 0, 0);
        }
        else
        {
            fn_800EBBFC(mUnidentified318, 0x8A9FCF66, "TankOn", this);
        }
        mUnidentified3DC = true;
        mUnidentified3DD = false;
    }

    if (m_eCharacterClass == DAISY)
    {
        bool bRunning = mUnidentified3E0 > 0.0f;
        if (bRunning)
        {
            mUnidentified3EC = 0.0f;
            mUnidentified3F0 = 0;
            fn_800301E8(this);
        }
    }
    else if (m_eCharacterClass == MARIO)
    {
        m_pTweaks = mUnidentified328;
        if (fn_8002E060() != (eFielderDesireState)0xC)
        {
            EndDesire();
        }
        if (m_pBall == 0)
        {
            InitActionRunning();
        }
        else
        {
            InitActionRunningWB(false);
        }
        InitMovementCoast();
        m_fLeanAmount = 0.0f;
        if (!fn_80319FEC(fn_8002E1A4(this), 0x23))
        {
            fn_80319E84(fn_8002E1A4(this), 0x23, 0, 0);
        }
    }
    else if (m_eCharacterClass == PEACH)
    {
        if (m_eAnimID != 0x68 && fn_80034964(this)
            && fn_8002E060() != (eFielderDesireState)0x16)
        {
            SetAction((eFielderActionState)0x1D);
            SetAnimState(0x68, true, 0.2f, false, false);
            InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet,
                fvNotSet);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else if (m_eCharacterClass == YOSHI)
    {
        mUnidentified408 = 0.0f;
        fn_800301E8(this);
    }

    return true;
}

extern "C" void fn_800504A4(void)
{
}

extern "C" void fn_800504A8(UnidentifiedAbilityEffect* pParam)
{
    if (pParam->mUnidentified08 != 0)
    {
        fn_80178D0C(pParam->mUnidentified08);
    }
}

// The original template identities of these common weak pool statics are
// not yet known.
struct UnidentifiedPoolEntry14
{
    u8 mUnknown[0x14];
};

struct UnidentifiedPoolEntry40
{
    u8 mUnknown[0x40];
};

template <typename T>
struct UnidentifiedPoolStorage
{
    static SlotPool<UnidentifiedPoolEntry14> pool14;
    static SlotPool<UnidentifiedPoolEntry40> pool40;
};

template <typename T>
SlotPool<UnidentifiedPoolEntry14> UnidentifiedPoolStorage<T>::pool14(
    0x10, 0x10);
template <typename T>
SlotPool<UnidentifiedPoolEntry40> UnidentifiedPoolStorage<T>::pool40(
    0x10, 0x10);

// The original type identity of this common weak static is not yet known.
struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

struct UnidentifiedStaticTag;

template struct UnidentifiedPoolStorage<UnidentifiedStaticTag>;
template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
