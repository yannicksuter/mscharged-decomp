#include "Game/Sys/audio.h"
#include "Game/CharacterTriggers.h"
#include "Game/AI/Fielder.h"
#include "Game/RumbleActions.h"
#include "Game/Render/FlyingCamera.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/Physics/PhysicsShockwave.h"

#include "Game/AI/Fuzzy.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/CharacterTweaks.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/InputManager.h"
#include "Game/MathHelpers.h"
#include "Game/SAnim.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Task/FixedUpdateTask.h"
#include "NL/globalpad.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "types.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Camera/CameraMan.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static float sDKSuperHitNoiseAmplitude[4] = { 0.15f, 0.4f, 0.0f, 0.0f };

float gDKSuperDeceleration = 0.25f;
float gDKSuperChargeEndFrame = 15.0f;
float gDKSuperHitNoiseFrequency = 30.0f;
float gDKSuperHitNoiseDuration = 1.0f;
float gDKSuperShockwaveRadius = 9.0f;
float gWaluigiTankCapacity = 2.5f;
float gWaluigiWallMinSegmentTime = 0.3f;
float gBowserTankCapacity = 3.5f;
float gBowserTankOffCost = 0.3f;
float gWarioTankCapacity = 1.5f;
float gPeachFlashFrameLockTime = 0.5f;
float gPeachPhotoHalfWidth = 5.4f;
float gPeachPhotoHalfHeight = 5.15f;
float gPeachCameraFlashFrame = 1.0f;
float gPeachFlashFrame = 10.0f;
float gPeachCamerasAwayFrame = 15.0f;
float gPeachSuperFacing = 0.5f;
int gPeachFlyingCameraCount = 4;
float gDKSuperHitTiltScale = 0.4f;
float gWaluigiTankOffCost;
bool gPeachPhotoEmitEnabled;

extern "C" void fn_8002E3F8(cFielder* pFielder);
extern "C" bool fn_8002EDC8(cFielder* pFielder, int nParam);


void cFielder::InitActionDKSuper()
{
    fn_8002E3F8(this);
    SetAction((eFielderActionState)0x1D);
    SetAnimState(0x68, true, 0.2f, false, false);
    muInvincibleStatus |= 1;
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    InitMovementDecelerateExponential(gDKSuperDeceleration);
    Unknown8(mUnidentified024.m_aActualFacingDirection, false);
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aActualMovementDirection;
    mUnidentified024.m_fDesiredSpeed = 0.0f;
    EmitDKSuperCharge(this);
}

void cFielder::DoDKSuperHit()
{
    EmitDKSuperHit(this);
    FireCameraNoiseFilter(*(nlVector3*)sDKSuperHitNoiseAmplitude, gDKSuperHitNoiseFrequency, gDKSuperHitNoiseDuration);
    fn_80061B1C(1, gDKSuperHitTiltScale * mUnidentified024.m_v3Position.y,
        gDKSuperHitTiltScale * mUnidentified024.m_v3Position.x);
    CreateHitShockwave(
        this, &GetJointPosition(m_nHeadJointIndex), gDKSuperShockwaveRadius);
    PlayRumbleAction(4, GetGlobalPad());
}

void cFielder::ActionDKSuper(float fDeltaT)
{
    mUnidentified024.m_fDesiredSpeed = 0.0f;

    if (m_pCurrentAnimController->TestFrameTrigger(gDKSuperChargeEndFrame))
    {
        KillDKSuperCharge(this);
        mUnidentified024.m_fActualSpeed = 0.0f;
        mUnidentified024.m_fDesiredSpeed = 0.0f;
        SetVelocity(v3Zero);
        Unknown8(mUnidentified024.m_aActualFacingDirection, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::CleanUpPeachSuper()
{
    fn_80038158(this, 0);

    if (m_pCurrentAnimController->m_fTime
            * (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys
        < gPeachCamerasAwayFrame)
    {
        UnidentifiedEventData_800673FC event;
        event.mUnidentified04 = mUnidentified024.m_v3Position;
        event.mUnidentified10 = gPeachPhotoHalfWidth;
        event.mUnidentified14 = gPeachPhotoHalfHeight;
        event.mUnidentified00 = this;
        fn_80060210(g_pGame, &event);
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        StartRunning();
    }
}

void cFielder::InitActionPeachSuper()
{
    fn_8002E3F8(this);
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x1D);
    muInvincibleStatus |= 1;

    if ((u16)abs_s16((s16)(mUnidentified024.m_aActualFacingDirection
            - (u16)(s32)(65536.0f * gPeachSuperFacing)))
        < 0x4000)
    {
        SetAnimState(0x68, true, 0.2f, false, false);
    }
    else
    {
        SetAnimState(0x69, true, 0.2f, false, false);
    }

    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    Unknown8(mUnidentified024.m_aActualFacingDirection, false);
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aActualMovementDirection;
    mUnidentified024.m_fDesiredSpeed = 0.0f;

    UnidentifiedEventData_800673FC event;
    event.mUnidentified04 = mUnidentified024.m_v3Position;
    event.mUnidentified10 = gPeachPhotoHalfWidth;
    event.mUnidentified14 = gPeachPhotoHalfHeight;
    event.mUnidentified00 = this;
    fn_80060014(g_pGame, &event);

    float fParam = FMAX(gPeachPhotoHalfWidth, gPeachPhotoHalfHeight);
    fParam += 0.5f;
    SetFlyingCameraCount(gPeachFlyingCameraCount, this, fParam);
}

void cFielder::ActionPeachSuper(float fDeltaT)
{
    float fFrame = m_pCurrentAnimController->get_fTime()
        * (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys
        / gPeachCameraFlashFrame;

    if (fFrame <= 1.0f)
    {
        u32 aFacing = mUnidentified024.m_aActualFacingDirection;
        float fBlend = fFrame * (-2.0f * fFrame + 3.0f);
        fBlend = fFrame * fBlend;
        float fTurn = (float)aFacing / 65536.0f;
        fTurn = gPeachSuperFacing - fTurn;
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
        s16 adjustedDelta
            = (s16)(s32)(65536.0f * (fTurn * fBlend));
        u16 newFacing = adjustedDelta + aFacing;
        SetFacingDirection(newFacing, true);
    }

    if (m_pCurrentAnimController->TestFrameTrigger(gPeachCameraFlashFrame))
    {
        UnidentifiedEventData_800673FC event;
        event.mUnidentified04 = mUnidentified024.m_v3Position;
        event.mUnidentified10 = gPeachPhotoHalfWidth;
        event.mUnidentified14 = gPeachPhotoHalfHeight;
        event.mUnidentified00 = this;
        fn_8005FE18(g_pGame, &event);
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(gPeachFlashFrame))
    {
        UnidentifiedEventData_800673FC event;
        event.mUnidentified04 = mUnidentified024.m_v3Position;
        event.mUnidentified10 = gPeachPhotoHalfWidth;
        event.mUnidentified14 = gPeachPhotoHalfHeight;
        cField::FixOutOfBoundsPosition(event.mUnidentified04, gPeachPhotoHalfWidth, true);
        event.mUnidentified00 = this;
        fn_8005FC1C(g_pGame, &event);

        if (gPeachPhotoEmitEnabled)
        {
            EmitPeachPhoto(this);
        }

        if (m_pBall == 0)
        {
            PlaySound(m_uSoundSlotId, 0x7997624D, 0, 0);
        }
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(
                 1.0f + gPeachFlashFrame))
    {
        if (!IsNetworkOrRecordedGame())
        {
            FixedUpdateTask::SetFrameLock(gPeachFlashFrameLockTime);
        }
        fn_80038158(this, 0);
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(gPeachCamerasAwayFrame))
    {
        SetFlyingCameraTarget(0);

        UnidentifiedEventData_800673FC event;
        event.mUnidentified04 = mUnidentified024.m_v3Position;
        event.mUnidentified10 = gPeachPhotoHalfWidth;
        event.mUnidentified14 = gPeachPhotoHalfHeight;
        event.mUnidentified00 = this;
        fn_80060210(g_pGame, &event);
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
        mUnidentified024.m_fDesiredSpeed = 0.0f;
    }
}

float cFielder::GetSuperPowerTankFraction()
{
    return mUnidentified3E0 / mUnidentified3E4;
}

void cFielder::ClearSuperPowerTank()
{
    mUnidentified3E0 = 0.0f;
    mUnidentified3E4 = 0.0f;
    fn_8005001C(true);
}

void cFielder::fn_8005001C(bool bForce)
{
    if (mUnidentified024.m_eCharacterClass == DAISY)
    {
        if (mUnidentified3DC || bForce)
        {
            mUnidentified3DC = false;
            float fDecay = gBowserTankOffCost;
            mUnidentified3E0 = mUnidentified3E0 - fDecay;
            bool bRunning = mUnidentified3E0 > 0.0f;
            if (bRunning)
            {
                if (mUnidentified3E0 < 0.03f)
                {
                    mUnidentified3E0 = 0.03f;
                }
            }
            mUnidentified3E8.nextFireballTime = 0.0f;
            fn_8002FE54(this);
            StopSound(0x8A9FCF66, this);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == MARIO)
    {
        if (mUnidentified3DC)
        {
            if (mUnidentified3F8.mUnidentified00 <= 0.0f || bForce)
            {
                mUnidentified3DC = false;
                mUnidentified3DD = false;
                float fDecay = gWaluigiTankOffCost;
                mUnidentified3E0 = mUnidentified3E0 - fDecay;
                bool bRunning = mUnidentified3E0 > 0.0f;
                if (bRunning)
                {
                    if (mUnidentified3E0 < mUnidentified3F8.mUnidentified04)
                    {
                        mUnidentified3E0 = mUnidentified3F8.mUnidentified04;
                    }
                }
                fn_8002FE54(this);
                if (fn_80319FEC(fn_8002E1A4(this), 0x23))
                {
                    fn_80319E58(fn_8002E1A4(this), 0x23);
                }
                StopSound(0x8A9FCF66, this);
            }
            else
            {
                mUnidentified3DD = true;
            }
        }
    }
    else if (mUnidentified024.m_eCharacterClass == PEACH)
    {
        if (mUnidentified3DC || bForce)
        {
            mUnidentified3DC = false;
            StopSound(0x8A9FCF66, this);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == YOSHI)
    {
        if (mUnidentified3DC || bForce)
        {
            mUnidentified3DC = false;
            mUnidentified408 = 0.0f;
            fn_8002FE54(this);
        }
    }
}

void cFielder::InitSuperPowerTank(bool bTurnOn)
{
    switch (mUnidentified024.m_eCharacterClass)
    {
    case DAISY:
        mUnidentified3E4 = gBowserTankCapacity;
        mUnidentified3E8.nextFireballTime = 0.0f;
        break;
    case MARIO:
        mUnidentified3E4 = gWaluigiTankCapacity;
        mUnidentified3F8.mUnidentified00
            = mUnidentified3F8.mUnidentified04 = gWaluigiWallMinSegmentTime;
        break;
    case PEACH:
        mUnidentified3E4 = gWarioTankCapacity;
        mUnidentified3F4 = 0.0f;
        break;
    case YOSHI:
        mUnidentified3E4 = 1.0f;
        break;
    }

    mUnidentified3E0 = mUnidentified3E4;

    if (bTurnOn)
    {
        TurnOnSuperPowerTank();
    }
}

bool cFielder::TurnOnSuperPowerTank()
{
    if (!fn_8002EDC8(this, -1))
    {
        return false;
    }

    if (!mUnidentified3DC)
    {
        if (mUnidentified024.m_eCharacterClass == YOSHI)
        {
            PlaySound(m_uSoundSlotId, 0x8A9FCF66, 0, 0);
        }
        else
        {
            PlaySound(m_uSoundSlotId, 0x8A9FCF66, "TankOn", this);
        }
        mUnidentified3DC = true;
        mUnidentified3DD = false;
    }

    if (mUnidentified024.m_eCharacterClass == DAISY)
    {
        bool bRunning = mUnidentified3E0 > 0.0f;
        if (bRunning)
        {
            mUnidentified3E8.fireballStageTime = 0.0f;
            mUnidentified3E8.fireballStageNum = 0;
            fn_800301E8(this);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == MARIO)
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
        mUnidentified024.m_fLeanAmount = 0.0f;
        if (!fn_80319FEC(fn_8002E1A4(this), 0x23))
        {
            fn_80319E84(fn_8002E1A4(this), 0x23, 0, 0);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == PEACH)
    {
        if (m_eAnimID != 0x68 && IsRunning()
            && fn_8002E060() != (eFielderDesireState)0x16)
        {
            SetAction((eFielderActionState)0x1D);
            SetAnimState(0x68, true, 0.2f, false, false);
            InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet,
                fvNotSet);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == YOSHI)
    {
        mUnidentified408 = 0.0f;
        fn_800301E8(this);
    }

    return true;
}

void ActBowserSuper::fn_800504A4()
{
}

void WaluigiWallState::fn_800504A8()
{
    if (mUnidentified08 != 0)
    {
        mUnidentified08->ClearWalls();
    }
}
