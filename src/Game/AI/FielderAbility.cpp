#include "NL/nlDLListContainer.inl"
#include "Game/Sys/audio.h"
#include "Game/CharacterTriggers.h"
#include "Game/AI/Fielder.h"
#include "Game/RumbleActions.h"
#include "Game/Render/FlyingCamera.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/Physics/PhysicsShockwave.h"

#include "Game/AI/Fuzzy.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/ScriptMachine.h"
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



void cFielder::InitActionDKSuper()
{
    EndFrozenOrDazed();
    SetAction((eFielderActionState)0x1D);
    SetAnimState(0x68, true, 0.2f, false, false);
    muInvincibleStatus |= 1;
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    InitMovementDecelerateExponential(gDKSuperDeceleration);
    SetDesiredFacingDirection(mUnidentified024.m_aActualFacingDirection, false);
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aActualMovementDirection;
    mUnidentified024.m_fDesiredSpeed = 0.0f;
    EmitDKSuperCharge(this);
}

void cFielder::DoDKSuperHit()
{
    EmitDKSuperHit(this);
    FireCameraNoiseFilter(*(nlVector3*)sDKSuperHitNoiseAmplitude, gDKSuperHitNoiseFrequency, gDKSuperHitNoiseDuration);
    SetFieldTilt(1, gDKSuperHitTiltScale * mUnidentified024.m_v3Position.y,
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
        SetDesiredFacingDirection(mUnidentified024.m_aActualFacingDirection, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::CleanUpPeachSuper()
{
    ClearInvincibility(0);

    if (m_pCurrentAnimController->m_fTime
            * (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys
        < gPeachCamerasAwayFrame)
    {
        PeachPhotoData event;
        event.v3Position = mUnidentified024.m_v3Position;
        event.fHalfWidth = gPeachPhotoHalfWidth;
        event.fHalfHeight = gPeachPhotoHalfHeight;
        event.pPlayer = this;
        DeliverPeachCamerasAwayEvent(g_pGame, &event);
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        StartRunning();
    }
}

void cFielder::InitActionPeachSuper()
{
    EndFrozenOrDazed();
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
    SetDesiredFacingDirection(mUnidentified024.m_aActualFacingDirection, false);
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aActualMovementDirection;
    mUnidentified024.m_fDesiredSpeed = 0.0f;

    PeachPhotoData event;
    event.v3Position = mUnidentified024.m_v3Position;
    event.fHalfWidth = gPeachPhotoHalfWidth;
    event.fHalfHeight = gPeachPhotoHalfHeight;
    event.pPlayer = this;
    DeliverPeachCamerasDownEvent(g_pGame, &event);

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
        PeachPhotoData event;
        event.v3Position = mUnidentified024.m_v3Position;
        event.fHalfWidth = gPeachPhotoHalfWidth;
        event.fHalfHeight = gPeachPhotoHalfHeight;
        event.pPlayer = this;
        DeliverPeachCameraFlashEvent(g_pGame, &event);
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(gPeachFlashFrame))
    {
        PeachPhotoData event;
        event.v3Position = mUnidentified024.m_v3Position;
        event.fHalfWidth = gPeachPhotoHalfWidth;
        event.fHalfHeight = gPeachPhotoHalfHeight;
        cField::FixOutOfBoundsPosition(event.v3Position, gPeachPhotoHalfWidth, true);
        event.pPlayer = this;
        DeliverPeachFlashEvent(g_pGame, &event);

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
        ClearInvincibility(0);
    }
    else if (m_pCurrentAnimController->TestFrameTrigger(gPeachCamerasAwayFrame))
    {
        SetFlyingCameraTarget(0);

        PeachPhotoData event;
        event.v3Position = mUnidentified024.m_v3Position;
        event.fHalfWidth = gPeachPhotoHalfWidth;
        event.fHalfHeight = gPeachPhotoHalfHeight;
        event.pPlayer = this;
        DeliverPeachCamerasAwayEvent(g_pGame, &event);
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
    return m_fSuperPowerTankLevel / m_fSuperPowerTankCapacity;
}

void cFielder::ClearSuperPowerTank()
{
    m_fSuperPowerTankLevel = 0.0f;
    m_fSuperPowerTankCapacity = 0.0f;
    TurnOffSuperPowerTank(true);
}

void cFielder::TurnOffSuperPowerTank(bool bForce)
{
    if (mUnidentified024.m_eCharacterClass == BOWSER)
    {
        if (m_bSuperPowerTankOn || bForce)
        {
            m_bSuperPowerTankOn = false;
            float fDecay = gBowserTankOffCost;
            m_fSuperPowerTankLevel = m_fSuperPowerTankLevel - fDecay;
            bool bRunning = m_fSuperPowerTankLevel > 0.0f;
            if (bRunning)
            {
                if (m_fSuperPowerTankLevel < 0.03f)
                {
                    m_fSuperPowerTankLevel = 0.03f;
                }
            }
            mActionBowserSuper.nextFireballTime = 0.0f;
            SetNormalTweaks();
            StopSound(0x8A9FCF66, this);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == WALUIGI)
    {
        if (m_bSuperPowerTankOn)
        {
            if (mWaluigiWallState.mUnidentified00 <= 0.0f || bForce)
            {
                m_bSuperPowerTankOn = false;
                m_bSuperPowerTankShutdownPending = false;
                float fDecay = gWaluigiTankOffCost;
                m_fSuperPowerTankLevel = m_fSuperPowerTankLevel - fDecay;
                bool bRunning = m_fSuperPowerTankLevel > 0.0f;
                if (bRunning)
                {
                    if (m_fSuperPowerTankLevel < mWaluigiWallState.mUnidentified04)
                    {
                        m_fSuperPowerTankLevel = mWaluigiWallState.mUnidentified04;
                    }
                }
                SetNormalTweaks();
                if (IsConcurrentStateActive(GetFielderScriptMachine(this), 0x23))
                {
                    DeactivateConcurrentState(GetFielderScriptMachine(this), 0x23);
                }
                StopSound(0x8A9FCF66, this);
            }
            else
            {
                m_bSuperPowerTankShutdownPending = true;
            }
        }
    }
    else if (mUnidentified024.m_eCharacterClass == WARIO)
    {
        if (m_bSuperPowerTankOn || bForce)
        {
            m_bSuperPowerTankOn = false;
            StopSound(0x8A9FCF66, this);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == PETEY)
    {
        if (m_bSuperPowerTankOn || bForce)
        {
            m_bSuperPowerTankOn = false;
            m_fPeteyMuckBallSpeed = 0.0f;
            SetNormalTweaks();
        }
    }
}

void cFielder::InitSuperPowerTank(bool bTurnOn)
{
    switch (mUnidentified024.m_eCharacterClass)
    {
    case BOWSER:
        m_fSuperPowerTankCapacity = gBowserTankCapacity;
        mActionBowserSuper.nextFireballTime = 0.0f;
        break;
    case WALUIGI:
        m_fSuperPowerTankCapacity = gWaluigiTankCapacity;
        mWaluigiWallState.mUnidentified00
            = mWaluigiWallState.mUnidentified04 = gWaluigiWallMinSegmentTime;
        break;
    case WARIO:
        m_fSuperPowerTankCapacity = gWarioTankCapacity;
        m_fNextGasTime = 0.0f;
        break;
    case PETEY:
        m_fSuperPowerTankCapacity = 1.0f;
        break;
    }

    m_fSuperPowerTankLevel = m_fSuperPowerTankCapacity;

    if (bTurnOn)
    {
        TurnOnSuperPowerTank();
    }
}

bool cFielder::TurnOnSuperPowerTank()
{
    if (!CanUsePowerup(this, -1))
    {
        return false;
    }

    if (!m_bSuperPowerTankOn)
    {
        if (mUnidentified024.m_eCharacterClass == PETEY)
        {
            PlaySound(m_uSoundSlotId, 0x8A9FCF66, 0, 0);
        }
        else
        {
            PlaySound(m_uSoundSlotId, 0x8A9FCF66, "TankOn", this);
        }
        m_bSuperPowerTankOn = true;
        m_bSuperPowerTankShutdownPending = false;
    }

    if (mUnidentified024.m_eCharacterClass == BOWSER)
    {
        bool bRunning = m_fSuperPowerTankLevel > 0.0f;
        if (bRunning)
        {
            mActionBowserSuper.fireballStageTime = 0.0f;
            mActionBowserSuper.fireballStageNum = 0;
            SetSuperPowerTweaks();
        }
    }
    else if (mUnidentified024.m_eCharacterClass == WALUIGI)
    {
        m_pTweaks = m_pSuperPowerTweaks;
        if (GetDesireState() != (eFielderDesireState)0xC)
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
        if (!IsConcurrentStateActive(GetFielderScriptMachine(this), 0x23))
        {
            ActivateConcurrentState(GetFielderScriptMachine(this), 0x23, 0, 0);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == WARIO)
    {
        if (m_eAnimID != 0x68 && IsRunning()
            && GetDesireState() != (eFielderDesireState)0x16)
        {
            SetAction((eFielderActionState)0x1D);
            SetAnimState(0x68, true, 0.2f, false, false);
            InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet,
                fvNotSet);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == PETEY)
    {
        m_fPeteyMuckBallSpeed = 0.0f;
        SetSuperPowerTweaks();
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
