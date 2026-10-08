#include "NL/nlDLListContainer.inl"
#include "Game/FE/Overlay/OverlayHandlerMegaStrikeMeter.h"
#include "Game/Player.h"
#include "Game/CharacterTriggers.h"
#include "NL/nlIntersection.h"
#include "Game/Sys/audio.h"
#include "Game/DetInput.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/Audio/GameStreams.h"
#include "Game/RumbleActions.h"
#include "Game/Terrain.h"
#include "Game/Sys/debug.h"
#include "Game/AI/Fielder.h"
#include "NL/gl/glView.h"
#include "Game/AI/AIContext.h"
#include "Game/Render/BulletBill.h"

#include "Game/Render/RLView.h"

#include "Game/AI/FielderActions.h"
#include "Game/AI/Fuzzy.h"
#include "Game/AI/ScriptMachine.h"
#include "Game/AnimInventory.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/HeadTrack.h"
#include "Game/AI/ShotMeter.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Goalie.h"
#include "Game/PoseAccumulator.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Team.h"
#include "Game/Weather.h"
#include "Game/NetworkSession.h"
#include "Game/InputManager.h"
#include "Game/NetworkInputRecording.h"
#include "Game/CrowdRiot.h"
#include "Game/TweakValue.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/CharacterTweaks.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/NetworkMessages.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/nlString.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/globalpad.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/EventDataTypes.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/utility.h"
#include "Game/Render/HammerObject.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/ShootToScoreMeter.h"
#include "math.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

float lbl_806DB890 = 0.99f;
float lbl_806DB894 = 1.0f;
float lbl_806DB898 = 1.2f;
float lbl_806DB89C = 8.0f;
float lbl_806DB8A0 = 1.3f;
float lbl_806DB8A4 = 0.45f;
float lbl_806DB8A8 = 6.66f;
int lbl_806DB8AC = 300;
int lbl_806DB8B0 = 100;
float lbl_806DB8B4 = 5.0f;
float lbl_806DB8B8 = 7.5f;
float lbl_806DB8BC = 3.0f;
float lbl_806DB8C0 = 15.0f;
float lbl_806DB8C4 = 0.66f;
float lbl_806DB8C8 = 1.33f;
float lbl_806DB8CC = 1.5f;
float lbl_806DB8D0 = 6.0f;
float lbl_806DB8D4 = 16.0f;
float lbl_806DB8D8 = 1.7f;
float lbl_806DB8DC = 0.4f;
float lbl_806DB8E0 = 0.5f;
float lbl_806DB8E4 = 100.0f;
float lbl_806DB8E8 = 4.0f;
float lbl_806DB8EC = 2.0f;
float lbl_806DB8F0 = 0.75f;
float lbl_806DB8F4 = 0.66f;
float lbl_806DB8F8 = 3.0f;
float lbl_806DB8FC = 0.001f;
float lbl_806DB900 = 0.8f;
float lbl_806DB904 = 0.5f;
float lbl_806DB908 = 0.4f;
float lbl_806DB90C = 15.0f;
float lbl_806DB910 = 22.5f;
float lbl_806DB914 = 4.0f;
float lbl_806DB918 = 3300.0f;
float lbl_806DB91C = 0.021f;
float lbl_806DB920 = 0.33f;
float lbl_806DB924 = 1.35f;
float lbl_806DB928 = 0.1f;
float lbl_806DB92C = 0.27f;
float lbl_806DB930 = 0.41f;
float lbl_806DB934 = 0.22f;
float lbl_806DB938 = 0.12f;
float lbl_806DB93C = 0.15f;
float lbl_806DB940 = 0.05f;
float lbl_806DB944 = 0.03f;
float lbl_806DB948 = 3.0f;
float lbl_806DB94C = 3.0f;
float lbl_806DB950 = 6.0f;
float lbl_806DB954 = 6.0f;
float lbl_806DB958 = 0.22f;
float lbl_806DB95C = 0.5f;
float lbl_806DB960 = 0.12f;
float lbl_806DB964 = 0.24f;
float lbl_806DB968 = 0.06f;
float lbl_806DB96C = 0.22f;
float lbl_806DB970 = 0.3f;
float lbl_806DB974 = 0.2f;
float lbl_806DB978 = 0.5f;
bool gbUseTurboCharging = true;
float lbl_806DB980 = 2.0f;
float lbl_806DB984 = 2.0f;
float lbl_806DB988 = 1.15f;
float lbl_806DB98C = 1.6f;
float lbl_806DB990 = 0.8f;
float lbl_806DB994 = 1.25f;
float lbl_806DB998 = 0.25f;
float lbl_806DB99C = 1.5f;
float lbl_806DB9A0 = 1.4f;
float lbl_806DB9A4 = 0.5f;
float lbl_806DB9A8 = 0.8f;
float lbl_806DB9AC = 0.2f;
float lbl_806DB9B0 = 0.48f;
float lbl_806DB9B4 = 0.54f;
static unsigned short gHitReactFacingOffsets[4] = {
    0x8000,
    0x4000,
    0x0000,
    0xC000,
};

float lbl_806E0C68;
float lbl_806E0C6C;

extern const float lbl_806E3538[1] = { 0.1f };
extern const float lbl_806E35D4[1];

static const nlVector3 v3LaunchUp = { 0.0f, 0.0f, 5.0f };

extern "C" bool fn_8003E948(const cFielder* pFielder);

extern bool lbl_806DB5A8;

extern "C" bool fn_8003E8A0(const cFielder* pFielder);
extern "C" void fn_8005ED64(cGame* pGame, PlayerAttackData* pData);
extern "C" float fn_8002CE14(PlayerTweaks* pTweaks);



extern int gHitReactAnims[3][4];

extern int gShellAttackReactAnims[4];

extern unsigned short g_IdleTurnCompletionDelta;

void cFielder::asmRunning()
{
    fn_8002CE14(this->GetTweaks());

    s16 nAbsActualToDesiredFacingDirection = (s16)(u16)abs_s16(
        (s16)(mUnidentified024.m_aDesiredFacingDirection - mUnidentified024.m_aActualFacingDirection));
    s16 nAbsActualToDesiredMovementDirection = (s16)(u16)abs_s16(
        (s16)(mUnidentified024.m_aDesiredMovementDirection - mUnidentified024.m_aActualMovementDirection));
    float fSpeedFactor = InterpolateRangeClamped(
        0.96f, 0.6f, 0.0f, 0.5f, this->GetTweaks()->fMovementSpeed);
    bool bFirstTime;

    do
    {
        bFirstTime = false;

        switch (m_eAnimID)
        {
        default:
        {
            SetIdleAnimState();
            bFirstTime = false;
            break;
        }

        case 1:
        case 2:
        case 3:
        {
            if (ShouldStartCrossBlend(0))
            {
                if (mUnidentified024.m_fDesiredSpeed <= fn_8002CE14(this->GetTweaks()))
                {
                    SetIdleAnimState();
                }
                else
                {
                    mUnidentified024.m_fActualSpeed = GetRunSpeed(this->GetTweaks());
                    SetRunningAnimState(lbl_806E3538[0]);
                }
            }
            break;
        }

        case 0x1B:
        {
            switch (mActionRunningVars.eLastStrafeDirection)
            {
            case 1:
                SetStrafeRightAnimState();
                break;
            case 2:
                SetStrafeLeftAnimState();
                break;
            case 0:
            case 3:
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * GetRunSpeed(this->GetTweaks()))
                {
                    if (nAbsActualToDesiredFacingDirection >= 0x3A98)
                    {
                        SetBackRunningToRunAnimState();
                    }
                    else
                    {
                        SetBackRunningStopAnimState();
                    }
                }
                else
                {
                    SetIdleAnimState();
                }
                break;
            case 4:
                mUnidentified024.m_fDesiredSpeed = fn_8002CD2C(this->GetTweaks());
                break;
            }
            break;
        }

        case 0x22:
        {
            if (ShouldStartCrossBlend(4))
            {
                SetRunningAnimState(lbl_806E3538[0]);
            }
            break;
        }

        case 0x21:
        {
            if (ShouldStartCrossBlend(0x1B))
            {
                SetRunBackwardsAnimState();
            }
            break;
        }

        case 0x1E:
        {
            bool bAnimFinished = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
                              && m_pCurrentAnimController->m_fTime == 1.0f;

            if (bAnimFinished)
            {
                switch (mActionRunningVars.eLastStrafeDirection)
                {
                case 0:
                case 1:
                case 2:
                case 4:
                    SetBackRunningStopRecoverAnimState();
                    break;
                case 3:
                {
                    int nIndex = (u16)(mUnidentified024.m_aDesiredFacingDirection
                                       - mUnidentified024.m_aActualFacingDirection + 0x2000)
                              >> 14;
                    if (nIndex != 0)
                    {
                        SetStartAnimState(nIndex);
                    }
                    else
                    {
                        SetBackRunningStopStartAnimState();
                    }
                    break;
                }
                }
            }
            break;
        }

        case 0x1F:
        {
            if (ShouldStartCrossBlend(4))
            {
                switch (mActionRunningVars.eLastStrafeDirection)
                {
                case 1:
                case 2:
                case 4:
                    SetStopAnimState();
                    break;
                case 0:
                    SetIdleAnimState();
                    break;
                case 3:
                    SetRunningAnimState(lbl_806E3538[0]);
                    break;
                }
            }
            break;
        }

        case 0x20:
        {
            if (ShouldStartCrossBlend(4))
            {
                switch (mActionRunningVars.eLastStrafeDirection)
                {
                case 4:
                    SetRunBackwardsAnimState();
                    break;
                case 1:
                    SetStrafeRightAnimState();
                    break;
                case 2:
                    SetStrafeLeftAnimState();
                    break;
                case 0:
                    SetIdleAnimState();
                    break;
                case 3:
                    SetStartAnimState(-1);
                    break;
                }
            }
            break;
        }

        case 0x1C:
        {
            switch (mActionRunningVars.eLastStrafeDirection)
            {
            case 0:
            case 1:
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * GetRunSpeed(this->GetTweaks()))
                {
                    fn_8003B664();
                }
                else
                {
                    SetIdleAnimState();
                }
                break;
            case 2:
                mUnidentified024.m_fDesiredSpeed = fn_8002CC44(this->GetTweaks());
                break;
            case 3:
                SetRunningAnimState(lbl_806E3538[0]);
                break;
            case 4:
                SetRunBackwardsAnimState();
                break;
            }
            break;
        }

        case 0x1D:
        {
            switch (mActionRunningVars.eLastStrafeDirection)
            {
            case 0:
            case 2:
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * GetRunSpeed(this->GetTweaks()))
                {
                    fn_8003B5FC();
                }
                else
                {
                    SetIdleAnimState();
                }
                break;
            case 1:
                mUnidentified024.m_fDesiredSpeed = fn_8002CC44(this->GetTweaks());
                break;
            case 3:
                SetRunningAnimState(lbl_806E3538[0]);
                break;
            case 4:
                SetRunBackwardsAnimState();
                break;
            }
            break;
        }

        case 5:
        {
            mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;

            switch (mActionRunningVars.eLastStrafeDirection)
            {
            case 0:
                if (ShouldStartCrossBlend(0))
                {
                    SetIdleAnimState();
                }
                break;
            case 1:
                if (ShouldStartCrossBlend(0x1D))
                {
                    SetStrafeRightAnimState();
                }
                break;
            case 2:
                if (ShouldStartCrossBlend(0x1C))
                {
                    SetStrafeLeftAnimState();
                }
                break;
            case 3:
                if (nAbsActualToDesiredFacingDirection >= 0x639C)
                {
                    if (mUnidentified024.m_fActualSpeed
                        < fSpeedFactor * GetRunSpeed(this->GetTweaks()))
                    {
                        SetStartAnimState(-1);
                    }
                    else if (!mUnidentified1E4.m_tSwapFacingTimer.GetSeconds())
                    {
                        SetHardStopAnimState();
                    }
                }
                else if (ShouldStartCrossBlend(0))
                {
                    SetStartAnimState(-1);
                }
                break;
            case 4:
                if (ShouldStartCrossBlend(0))
                {
                    SetRunBackwardsAnimState();
                    mUnidentified024.m_fActualSpeed = 0.0f;
                }
                break;
            }
            break;
        }

        case 0x23:
        case 0x24:
        {
            if (ShouldStartCrossBlend(0))
            {
                SetIdleAnimState();
            }
            break;
        }

        case 0:
        {
            switch (mActionRunningVars.eLastStrafeDirection)
            {
            case 0:
                mUnidentified024.m_fDesiredSpeed = 0.0f;
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * GetRunSpeed(this->GetTweaks()))
                {
                    SetStopAnimState();
                }
                break;
            case 1:
                SetStrafeRightAnimState();
                break;
            case 2:
                SetStrafeLeftAnimState();
                break;
            case 3:
                if (mUnidentified024.m_fActualSpeed
                    < fSpeedFactor * GetRunSpeed(this->GetTweaks()))
                {
                    SetStartAnimState(-1);
                }
                else
                {
                    SetRunningAnimState(lbl_806E3538[0]);
                }
                break;
            case 4:
                SetRunBackwardsAnimState();
                break;
            }
            break;
        }

        case 9:
        {
            if (fn_8003E8A0(this) && mUnidentified3DC)
            {
                if (mUnidentified024.m_fDesiredSpeed
                    < fn_8002CE14(this->GetTweaks()) - 0.15f)
                {
                    if (mUnidentified024.m_fActualSpeed
                        > 0.6f * GetRunSpeed(this->GetTweaks()))
                    {
                        SetStopAnimState();
                    }
                    else
                    {
                        SetIdleAnimState();
                    }
                }
                return;
            }

            if (fn_8003E9F0() && mUnidentified3DC)
            {
                return;
            }
        }

        case 4:
        {
            switch (mActionRunningVars.eLastStrafeDirection)
            {
            case 0:
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * GetRunSpeed(this->GetTweaks()))
                {
                    SetStopAnimState();
                }
                else
                {
                    SetIdleAnimState();
                }
                break;
            case 3:
                if (nAbsActualToDesiredFacingDirection >= 0x639C)
                {
                    if (mUnidentified024.m_fActualSpeed
                        < fSpeedFactor * GetRunSpeed(this->GetTweaks()))
                    {
                        SetStartAnimState(-1);
                    }
                    else if (!mUnidentified1E4.m_tSwapFacingTimer.GetSeconds())
                    {
                        SetHardStopAnimState();
                    }
                }
                else if ((m_eAnimID != 9 && fn_8003E74C())
                         || (m_eAnimID == 9 && !fn_8003E74C()))
                {
                    SetRunningAnimState(lbl_806E3538[0]);
                }
                break;
            case 1:
                SetStrafeRightAnimState();
                break;
            case 2:
                SetStrafeLeftAnimState();
                break;
            case 4:
                if (mUnidentified024.m_fDesiredSpeed > fn_8002CE14(this->GetTweaks()))
                {
                    if (nAbsActualToDesiredMovementDirection < 0x4000)
                    {
                        SetRunToBackRunningAnimState();
                    }
                    else
                    {
                        SetStopAnimState();
                    }
                }
                else
                {
                    SetStopAnimState();
                }
                break;
            }
            break;
        }

        case 0xC:
        {
            bool bAnimFinished = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
                              && m_pCurrentAnimController->m_fTime == 1.0f;

            if (bAnimFinished)
            {
                if (mUnidentified024.m_fDesiredSpeed > fn_8002CE14(this->GetTweaks()))
                {
                    SetHardStopTurnAnimState();
                }
                else
                {
                    SetHardStopRecoverAnimState();
                }
            }
            break;
        }

        case 0xD:
        {
            if (ShouldStartCrossBlend(0))
            {
                if (mUnidentified024.m_fDesiredSpeed >= fn_8002CE14(this->GetTweaks()))
                {
                    mUnidentified024.m_fActualSpeed = GetRunSpeed(this->GetTweaks());
                    SetRunningAnimState(lbl_806E3538[0]);
                }
                else
                {
                    SetStopAnimState();
                }
            }
            break;
        }

        case 0xE:
        {
            if (ShouldStartCrossBlend(4))
            {
                SetIdleAnimState();
            }
            break;
        }
        }
    } while (bFirstTime);
}

void cFielder::asmRunningWB(float fDeltaT)
{
    float fIdleToRunWBDesiredSpeed = lbl_806E3538[0] + fn_8002CE14(this->GetTweaks());
    s16 nAbsActualToDesiredFacingDirection = (s16)(u16)abs_s16(
        (s16)(mUnidentified024.m_aDesiredFacingDirection - mUnidentified024.m_aActualFacingDirection));
    float fSpeedFactor = InterpolateRangeClamped(
        0.96f, 0.6f, 0.0f, 0.5f, this->GetTweaks()->fMovementSpeed);
    bool bFirstTime;

    do
    {
        bFirstTime = false;

        switch (m_eAnimID)
        {
        default:
        {
            if (mActionRunningWBVars.bWaitForAnimToFinish)
            {
                bool bAnimFinished = m_pCurrentAnimController->UnidentifiedAtEnd();
                if (bAnimFinished
                    || mUnidentified024.m_fDesiredSpeed >= fIdleToRunWBDesiredSpeed)
                {
                    mActionRunningWBVars.bWaitForAnimToFinish = false;
                }
            }

            if (mActionRunningWBVars.bWaitForAnimToFinish)
            {
                break;
            }

            SetIdleWBAnimState();
            bFirstTime = false;
            break;
        }

        case 0x10:
        case 0x11:
        case 0x12:
        {
            if (ShouldStartCrossBlend(0x17))
            {
                if (mUnidentified024.m_fDesiredSpeed <= fn_8002CE14(this->GetTweaks()))
                {
                    SetIdleWBAnimState();
                }
                else
                {
                    mUnidentified024.m_fActualSpeed = GetRunSpeed(this->GetTweaks());
                    SetRunningWBAnimState(lbl_806E3538[0]);
                }
            }
            break;
        }

        case 0x17:
        {
            mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;

            if (ShouldStartCrossBlend(0xF))
            {
                SetIdleWBAnimState();
                mUnidentified024.m_fActualSpeed = 0.0f;
                break;
            }

            if (nAbsActualToDesiredFacingDirection >= 0x639C)
            {
                if (mUnidentified024.m_fActualSpeed
                    < fSpeedFactor * this->GetTweaks()->GetRunningSpeed())
                {
                    SetStartWBAnimState();
                }
                else
                {
                    SetHardStopAnimState();
                }
                break;
            }

            if (mUnidentified024.m_fDesiredSpeed > fn_8002CE14(this->GetTweaks()))
            {
                if (mUnidentified024.m_fActualSpeed
                    < fSpeedFactor * this->GetTweaks()->GetRunningSpeed())
                {
                    SetStartWBAnimState();
                }
            }
            break;
        }

        case 0xF:
        {
            if (mUnidentified024.m_fDesiredSpeed > 1.0f)
            {
                if (mUnidentified024.m_fActualSpeed
                    < fSpeedFactor * this->GetTweaks()->GetRunningSpeed())
                {
                    SetStartWBAnimState();
                }
                else
                {
                    SetRunningWBAnimState(lbl_806E3538[0]);
                }
            }
            else if (mUnidentified024.m_fActualSpeed
                     > 0.6f * this->GetTweaks()->GetRunningSpeed())
            {
                SetStopAnimState();
            }
            else
            {
                mUnidentified024.m_fDesiredSpeed = 0.0f;
            }
            break;
        }

        case 9:
        {
            if (fn_8003E8A0(this) && mUnidentified3DC)
            {
                if (mUnidentified024.m_fDesiredSpeed
                    < fn_8002CE14(this->GetTweaks()) - 0.15f)
                {
                    if (mUnidentified024.m_fActualSpeed
                        > 0.6f * GetRunSpeed(this->GetTweaks()))
                    {
                        SetStopAnimState();
                    }
                    else
                    {
                        SetIdleWBAnimState();
                    }
                }
                return;
            }

            if (fn_8003E9F0() && mUnidentified3DC)
            {
                return;
            }
        }

        case 0x14:
        {
            if (nAbsActualToDesiredFacingDirection >= 0x639C)
            {
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * this->GetTweaks()->GetRunningSpeed())
                {
                    SetHardStopAnimState();
                }
                else
                {
                    SetStartWBAnimState();
                }
                break;
            }

            if (mUnidentified024.m_fDesiredSpeed < fn_8002CE14(this->GetTweaks()) - 0.15f)
            {
                if (mUnidentified024.m_fActualSpeed
                    > 0.6f * this->GetTweaks()->GetRunningSpeed())
                {
                    SetStopAnimState();
                }
                else
                {
                    SetIdleWBAnimState();
                }
                break;
            }

            if ((m_eAnimID != 9 && fn_8003E74C())
                || (m_eAnimID == 9 && !fn_8003E74C()))
            {
                SetRunningWBAnimState(lbl_806E3538[0]);
            }
            break;
        }

        case 0x18:
        {
            bool bAnimFinished = m_pCurrentAnimController->UnidentifiedAtEnd();

            if (bAnimFinished)
            {
                if (mUnidentified024.m_fDesiredSpeed < fn_8002CE14(this->GetTweaks()))
                {
                    if (mActionRunningWBVars.bCuePitch)
                    {
                        fn_8004B148();
                    }
                    else
                    {
                        SetHardStopRecoverAnimState();
                    }
                }
                else
                {
                    SetHardStopTurnAnimState();
                }
            }
            break;
        }

        case 0x19:
        {
            if (ShouldStartCrossBlend(0xF))
            {
                if (mActionRunningWBVars.bCuePitch)
                {
                    fn_8004B148();
                }
                else if (mUnidentified024.m_fDesiredSpeed
                         >= fn_8002CE14(this->GetTweaks()))
                {
                    mUnidentified024.m_fActualSpeed = GetRunSpeed(this->GetTweaks());
                    SetRunningWBAnimState(lbl_806E3538[0]);
                }
                else
                {
                    SetStopAnimState();
                }
            }
            break;
        }

        case 0x1A:
        {
            if (ShouldStartCrossBlend(0x14))
            {
                SetIdleWBAnimState();
                mUnidentified024.m_fActualSpeed = 0.0f;
            }
            break;
        }
        }
    } while (bFirstTime);
}

void cFielder::EndAction()
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0)
    {
        SetAction(ACTION_NEED_ACTION);
    }
    else
    {
        fn_8004E11C(lbl_806DB8F8);
    }
}

void cFielder::fn_80043ADC()
{
    if (m_eActionState == (eFielderActionState)0
        || m_eActionState == (eFielderActionState)0x23
        || m_eActionState == ACTION_ELECTROCUTION)
    {
        return;
    }

    if (m_pBall != 0)
    {
        ReleaseBall(0);
        nlVector3 v3Velocity = v3LaunchUp;
        ShootBallDueToContact(v3Velocity);
    }

    mUnidentified330 = UnidentifiedFielderPair330(false, -1.0f);
    mUnidentified330.mUnidentified04 = 0.75f + nlRandomf(0.25f);

    nlVector3 v3Position = GetJointPosition(m_nBip01JointIndex_0xA4);
    SetPosition(v3Position);

    InitDesire(
        FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0);
    SetAnimState(0x7C, true, 0.2f, false, false);
    InitMovementCoast();
}

void cFielder::fn_80043C18(float fDeltaT)
{
    switch (m_eAnimID)
    {
    case 0x7C:
    {
        float fSpin
            = 1.0f - this->GetTweaks()->mUnidentified064;
        int nSpinStep
            = (u16)(s32)(5000.0f * (2.0f * fSpin + 1.0f));
        Unknown8(
            mUnidentified024.m_aActualFacingDirection + nSpinStep, false);

        SetFacingDirection(
            SeekDirection(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection,
                GetShotWindupTurnSpeed(this->GetTweaks()),
                GetShotWindupTurnFalloff(this->GetTweaks()),
                fDeltaT),
            true);

        if (!mUnidentified330.mUnidentified00)
        {
            float fTime = mUnidentified330.mUnidentified04 - fDeltaT;
            mUnidentified330.mUnidentified04 = fTime;
            if (fTime < 0.0f)
            {
                float fRadius = (float)(s32)(6.0f * (0.33f * fSpin + 1.0f));
                float fUpVelocity
                    = (float)(s32)(15.0f * (0.2f * fSpin + 1.0f));
                nlVector3 v3Velocity;
                MakeRandomDirection2D(v3Velocity, fRadius);
                v3Velocity.z = fUpVelocity;
                SetVelocity(v3Velocity);
                mUnidentified330.mUnidentified00 = true;
                mUnidentified024.m_v3Position.z += fDeltaT * mUnidentified024.m_v3Velocity.z;
            }
            else
            {
                float fBlend = 1.0f - fTime;
                CrowdRiot* pTarget = g_pGame->mpCrowdRiot;
                nlVector3 v3Position;
                nlVec3WeightedSum(v3Position, fBlend, pTarget->mv3Position, fTime, mUnidentified024.m_v3Position);
                SetPosition(v3Position);
            }
        }
        else
        {
            nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;
            v3Velocity.x *= 0.99f;
            v3Velocity.y *= 0.99f;
            v3Velocity.z = -30.0f * fDeltaT + v3Velocity.z;
            SetVelocity(v3Velocity);

            float fNewZ = fDeltaT * mUnidentified024.m_v3Velocity.z + mUnidentified024.m_v3Position.z;
            mUnidentified024.m_v3Position.z = fNewZ;
            if (fNewZ < 0.0f)
            {
                mUnidentified024.m_v3Position.z = 0.0f;
                mUnidentified024.m_v3Velocity.z = 0.0f;

                nlPolar polar;
                nlCartesianToPolar(polar, mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y);
                s16 sFacingDelta = polar.a - mUnidentified024.m_aActualFacingDirection;
                Unknown8(mUnidentified024.m_aActualFacingDirection, false);
                SetVelocity(v3Zero);
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(sFacingDelta, v3Zero, 0.15f, false);
            }
        }
        break;
    }
    case 0x7D:
        if (ShouldStartCrossBlend(4))
        {
            EndAction();
        }
        break;
    default:
        EndAction();
        break;
    }
}

void cFielder::fn_80044148(const nlVector3& v3Velocity)
{
    if (m_eActionState == (eFielderActionState)0
        || m_eActionState == (eFielderActionState)0x23)
    {
        return;
    }
    if (m_pBall != 0)
    {
        ReleaseBall(0);
        g_pBall->SetVelocity(mUnidentified024.m_v3Velocity, SPINTYPE_NONE, 0);
    }

    InitDesire(
        FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x23);
    SetAnimState(0x7C, false, 0.0333333f, false, false);
    SetVelocity(v3Velocity);
    InitMovementCoast();
    SetTangible(1, 0);
    mUnidentified178 = 1.0f;

    if (GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = 0;
        m_pPhysicsCharacter->m_CanCollideWithWall = 0;
    }

    bool bUnidentified = IsCaptain();
    unsigned long soundID = 0x1CF82176;
    if (bUnidentified)
    {
        soundID = 0xFDEC8E0F;
    }
    PlaySound(m_uSoundSlotId, soundID, 0, 0);
}

void cFielder::fn_80044290(float fDeltaT)
{
    switch (m_eAnimID)
    {
    case 0x7C:
    {
        if (mUnidentified024.m_v3Velocity.z < 0.0f && mUnidentified024.m_v3Position.z <= lbl_806E3538[0])
        {
            nlVector3 v3Position = mUnidentified024.m_v3Position;
            float fGoalLineX = (float)fabs(cField::GetGoalLineX(0U));
            float fSidelineY
                = (float)fabs(0.5f * (2.0f * cField::mv3FieldPosition.y));

            if (GameInfoManager::Instance()->GetStadium() == 0x0B
                && (((float)fabs(v3Position.x) - fGoalLineX > 0.0f
                        && (float)fabs(v3Position.x) - (5.0f + fGoalLineX)
                               < 0.0f)
                    || ((float)fabs(v3Position.y) - fSidelineY > 0.0f
                        && (float)fabs(v3Position.y) - (1.0f + fSidelineY)
                               < 0.0f)))
            {
                nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;
                float fUpVelocity = v3Velocity.z;
                v3Velocity.z = 0.0f;
                float fSpeed = nlSqrt(v3Velocity.GetLengthSq3D(), true);
                if (fSpeed < 2.0f)
                {
                    if (fSpeed < 0.01f)
                    {
                        v3Velocity = mUnidentified024.m_v3Position;
                    }
                    float fRecipLength
                        = nlRecipSqrt(v3Velocity.GetLengthSq3D(), true);
                    nlVec3Scale(v3Velocity, v3Velocity, fRecipLength);
                    nlVec3Scale(v3Velocity, v3Velocity, 5.0f);
                }
                v3Velocity.z = (float)(-1.0 * fUpVelocity);
                if (v3Velocity.z < 1.0f)
                {
                    v3Velocity.z = -15.0f;
                }
                SetVelocity(v3Velocity);
            }
            else if (fabsf(v3Position.x) - (5.0f + fGoalLineX) > 0.0f
                     || fabsf(v3Position.y) - (1.0f + fSidelineY)
                            > 0.0f)
            {
                fn_80046244();
            }
            else
            {
                v3Position.z = 0.0f;
                SetPosition(v3Position);
                SetVelocity(v3Zero);
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
            }
        }
        else
        {
            nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;
            float fDamping = 1.0f - 0.5f * fDeltaT;
            v3Velocity.x *= fDamping;
            v3Velocity.y *= fDamping;
            v3Velocity.z = -30.0f * fDeltaT + v3Velocity.z;
            SetVelocity(v3Velocity);
            mUnidentified024.m_v3Position.z += v3Velocity.z * fDeltaT;

            Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
            if (pGoalie->mGoalieActionState == (eGoalieActionState)0x0D
                && pGoalie->mpTarget == this)
            {
                pGoalie->fn_80080BFC(fDeltaT);
            }

            float fSpin = 1.0f
                        - this->GetTweaks()->mUnidentified064;
            int nSpinStep
                = (u16)(s32)(5000.0f * (2.0f * fSpin + 1.0f));
            Unknown8(
                mUnidentified024.m_aActualFacingDirection + nSpinStep, false);

            SetFacingDirection(
                SeekDirection(mUnidentified024.m_aActualFacingDirection,
                    mUnidentified024.m_aDesiredFacingDirection,
                    GetShotWindupTurnSpeed(this->GetTweaks()),
                    GetShotWindupTurnFalloff(this->GetTweaks()),
                    fDeltaT),
                true);
        }
        break;
    }
    case 0x7D:
        if (ShouldStartCrossBlend(4))
        {
            EndAction();
        }
        break;
    }
}

bool cFielder::fn_800447C0(unsigned short aDirection)
{
    if (mtPostDekeTimer.m_uPackedTime != 0)
    {
        return false;
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)1);
    mUnidentified338 = 0;
    mUnidentified33A = false;
    mUnidentified33C = 2;

    if (IsPeachSuperPowerActive())
    {
        if (mUnidentified3DC)
        {
            fn_8005001C(false);
        }
    }
    else if (fn_8003E9F0())
    {
        if (mUnidentified3DC)
        {
            fn_8005001C(false);
        }
    }
    else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x13)
    {
        EmitShyGuyDeke(this);
    }

    s16 sFacingDelta;
    switch (mUnidentified024.m_eCharacterClass)
    {
    case (eCharacterClass)0x01:
    case (eCharacterClass)0x03:
    case (eCharacterClass)0x07:
    case (eCharacterClass)0x09:
    case (eCharacterClass)0x0B:
    case (eCharacterClass)0x0C:
    case (eCharacterClass)0x12:
        sFacingDelta = nlAngleDiff(aDirection, GetActualFacing());
        SetAnimState(0x50, true, 0.2f, false, false);
        break;
    case (eCharacterClass)0x02:
    case (eCharacterClass)0x06:
    case (eCharacterClass)0x11:
        if (m_pController != 0
            && m_pController->GetMovementStickMagnitude() > 0.001f)
        {
            aDirection = m_pController->GetMovementStickDirection();
        }
        sFacingDelta = nlAngleDiff(aDirection, GetActualFacing());
        SetAnimState(0x50, true, 0.2f, false, false);
        break;
    case (eCharacterClass)0x05:
    case (eCharacterClass)0x0A:
    case (eCharacterClass)0x0D:
    case (eCharacterClass)0x0F:
    case (eCharacterClass)0x10:
        sFacingDelta = nlAngleDiff(aDirection, GetActualFacing());
        SetAnimState(0x50, true, 0.2f, false, false);
        break;
    default:
    {
        sFacingDelta = nlAngleDiff(aDirection, GetActualFacing());
        if ((u16)abs_s16(sFacingDelta) < 0x2AAA)
        {
            SetAnimState(0x50, true, 0.2f, false, false);
        }
        else if ((u16)abs_s16(sFacingDelta) < 0x6AAA)
        {
            if (sFacingDelta < 0)
            {
                SetAnimState(0x4F, true, 0.2f, false, false);
                SetFacingDirection(aDirection + 0x4000, true);
            }
            else
            {
                SetAnimState(0x4E, true, 0.2f, false, false);
                SetFacingDirection(aDirection - 0x4000, true);
            }
            sFacingDelta = 0;
        }
        else
        {
            SetAnimState(0x51, true, 0.2f, false, false);
            SetFacingDirection(aDirection - 0x8000, true);
            sFacingDelta = 0;
        }
        break;
    }
    }

    if (fn_8003E70C())
    {
        Unknown8(mUnidentified024.m_aActualFacingDirection, false);
        SetFacingDirection(mUnidentified024.m_aDesiredFacingDirection, true);
    }

    mUnidentified024.m_fDesiredSpeed = 0.0f;
    InitMovementFromAnim((s16)sFacingDelta, v3Zero, lbl_806E3538[0], false);

    switch (mUnidentified024.m_eCharacterClass)
    {
    case (eCharacterClass)0x00:
    case (eCharacterClass)0x04:
    case (eCharacterClass)0x08:
    case (eCharacterClass)0x0E:
    case (eCharacterClass)0x13:
        m_pCurrentAnimController->m_fPlaybackSpeedScale
            = InterpolateRangeClamped(lbl_806DB988, lbl_806DB98C, 0.35f, 0.75f, g_pGame->mpTerrain->GetSpeedFactor());
        break;
    case (eCharacterClass)0x01:
    case (eCharacterClass)0x07:
    case (eCharacterClass)0x09:
    case (eCharacterClass)0x0D:
        SetFacingDirection(sFacingDelta + mUnidentified024.m_aActualFacingDirection, true);
        InitMovementDecelerateExponential(lbl_806DB8F4);
        break;
    }

    bool bUnidentified = IsCaptain();
    unsigned long soundID = 0xDEA5F49B;
    if (bUnidentified)
    {
        soundID = 0xA91D4914;
    }
    PlaySound(m_uSoundSlotId, soundID, 0, 0);

    bool bUnidentified2 = g_pGame->IsGameplayOrOvertime();
    if (bUnidentified2)
    {
        StatsTracker::Instance()->TrackStat(
            (ePlayerStats)0x15, m_pTeam->m_nSide, mUnidentified1E4.m_ID, 0, 0, 0, 0);
    }

    return true;
}

void cFielder::fn_80044BEC(float fDeltaT)
{
    if (mUnidentified024.m_eMovementState == MOVEMENT_DECELERATE_EXPONENTIAL)
    {
        mUnidentified024.m_fDesiredSpeed = 0.0f;
    }

    if (GetGlobalPad() != 0)
    {
        if (m_pCurrentAnimController->m_fTime > lbl_806DB8F0)
        {
            mUnidentified33A = !IsDekePadPressed();
        }
        else
        {
            mUnidentified33A = false;
        }

        if (m_pBall != 0)
        {
            TestButtonsToQueueActions(fDeltaT);
        }
    }
    else
    {
        switch (mUnidentified024.m_eCharacterClass)
        {
        case (eCharacterClass)0x05:
        case (eCharacterClass)0x0A:
        case (eCharacterClass)0x0F:
        {
            float fChance = Difficult(m_pTeam);
            if (nlRandomf(2.0f) < fChance)
            {
                if (m_pCurrentAnimController->TestFrameTrigger(5.0f))
                {
                    mUnidentified1E4.m_eLastPadAction = 0x1B;
                }
            }
            break;
        }
        }
    }

    if (ShouldStartCrossBlend(0x52))
    {
        EndAction();

        if (m_pBall == 0)
        {
            mUnidentified1E4.m_eLastPadAction = 0x32;
        }

        if (GetGlobalPad() != 0)
        {
            if (TestQueuedActions())
            {
                mUnidentified1E4.m_eLastPadAction = 0x32;
            }
        }
    }
}

void cFielder::InitActionElectrocution(const nlVector3& wallPosition,
    const nlVector3& wallNormal, bool bParam)
{
    if (m_eActionState == ACTION_ELECTROCUTION || IsShattered() == true)
    {
        return;
    }

    EndFrozenOrDazed();
    fn_8009750C();

    float fElectrocutionTime;
    m_pCurrentAnimController->m_pSAnim->UnidentifiedGetRemainingTime(
        m_pCurrentAnimController->m_fTime, fElectrocutionTime);
    fElectrocutionTime += lbl_806DB920;

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_ELECTROCUTION);

    if (m_pBall != 0)
    {
        ReleaseBall(0);

        nlVector3 v3BallVelocity;
        nlVec3Set(v3BallVelocity, 0.4f * mUnidentified024.m_v3Velocity.x, 0.5f * -mUnidentified024.m_v3Position.y, 6.0f);
        g_pBall->ShootRelease(v3BallVelocity, SPINTYPE_NONE);
        SetNoPickUpTime(0.5f);
    }

    SetFacingDirection(nlVector3ToAngle(wallNormal, 0x8000), true);
    SetAnimState(0x76, true, 0.2f, false, false);
    InitMovementNone(0.0f, 0.0f);

    nlVector3 newPosition = GetJointPosition(m_nBip01JointIndex_0xA4);

    nlVector3 safeBip01Position;
    GetJointPositionFuture(&safeBip01Position, 0, m_nBip01JointIndex_0xA4, 0.0f, false, false, false, true);

    safeBip01Position.z += 0.25f;
    newPosition.z = (newPosition.z >= safeBip01Position.z) ? newPosition.z : safeBip01Position.z;

    float fNetWidth = cNet::m_fNetWidth;
    if ((float)fabs(newPosition.y) < 0.5f * fNetWidth)
    {
        float fAdjust = mUnidentified024.m_fPlayerScale;
        float fMaxY
            = fn_8002BFA8(this->GetTweaks(), fAdjust) + 0.5f * fNetWidth;
        newPosition.y = nlMinEquals(nlMaxEquals(newPosition.y, -fMaxY), fMaxY);
    }

    SetPosition(newPosition);

    mUnidentified340 = fElectrocutionTime;
    mUnidentified348 = false;
    if (fElectrocutionTime < 0.3f)
    {
        mUnidentified340 = 0.3f;
    }

    if (bParam)
    {
        nlVector3 effectWallPosition;
        effectWallPosition.x = wallPosition.x;
        effectWallPosition.y = wallPosition.y;
        effectWallPosition.z = newPosition.z;
        CharacterElectrocutionEffect(this, effectWallPosition, wallNormal);
    }
    else
    {
        mUnidentified348 = true;
        EmitElectrocution(this);
    }

    bool bUnidentified = IsCaptain();
    unsigned long soundID = 0xBADF0EF9;
    if (bUnidentified)
    {
        soundID = 0x1602CA52;
    }
    PlaySound(m_uSoundSlotId, soundID, 0, 0);
}

void cFielder::fn_800451B0(const nlVector3& v3Position)
{
    if (m_eActionState == ACTION_ELECTROCUTION || IsShattered() == true)
    {
        return;
    }

    if (m_pBall != 0)
    {
        ReleaseBall(0);

        float fSpread = 4.0f;
        nlVector3 v3BallVelocity;
        v3BallVelocity.x = nlRandomf(fSpread) - 0.5f * fSpread;
        v3BallVelocity.y = nlRandomf(fSpread) - 0.5f * fSpread;
        v3BallVelocity.z = nlRandomf(fSpread);
        g_pBall->ShootRelease(v3BallVelocity, SPINTYPE_NONE);
    }

    EndFrozenOrDazed();
    fn_8009750C();

    if (m_pBall == 0)
    {
        nlVector3 v3Direction;
        nlVec3Set(v3Direction,
            v3Position.x - mUnidentified024.m_v3Position.x,
            v3Position.y - mUnidentified024.m_v3Position.y,
            v3Position.z - mUnidentified024.m_v3Position.z);
        SetFacingDirection(nlVector3ToAngle(v3Direction), true);
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_ELECTROCUTION);
    SetAnimState(0x79, true, 0.2f, false, false);
    InitMovementNone(0.0f, 0.0f);

    nlVector3 jointPos = GetJointPosition(m_nBip01JointIndex_0xA4);

    nlVector3 futureJointPos;
    GetJointPositionFuture(&futureJointPos, 0, m_nBip01JointIndex_0xA4, 0.0f, false, false, false, true);

    futureJointPos.z += 0.05f;
    jointPos.z = (jointPos.z >= futureJointPos.z) ? jointPos.z
                                                  : futureJointPos.z;

    SetPosition(jointPos);

    mUnidentified340 = lbl_806DB994 + nlRandomf(lbl_806DB998);
    mUnidentified344 = lbl_806DB99C;
    mUnidentified348 = true;
    EmitElectrocution(this);

    PlayRumbleAction(4, GetGlobalPad());

    bool bUnidentified = IsCaptain();
    unsigned long soundID = 0xBADF0EF9;
    if (bUnidentified)
    {
        soundID = 0x1602CA52;
    }
    PlaySound(m_uSoundSlotId, soundID, 0, 0);
}

void cFielder::ActionElectrocution(float dt)
{
    switch (m_eAnimID)
    {
    case 0x76:
    case 0x79:
    {
        mUnidentified340 -= dt;
        mUnidentified344 -= dt;

        if (m_eAnimID == 0x79 && mUnidentified344 > 0.0f)
        {
            nlVector3 v3Position = mUnidentified024.m_v3Position;
            float fShake
                = this->GetTweaks()->mUnidentified064;
            float fRise = lbl_806DB9A0
                        * ((1.0f - fShake) * nlRandomf(0.5f) + 0.5f);
            v3Position.z += fRise * dt;
            SetPosition(v3Position);
        }

        if (mUnidentified340 <= 0.0f)
        {
            if (m_eAnimID == 0x76)
            {
                SetAnimState(0x77, true, 0.2f, false, false);

                nlVector3 launchVelocity = { -5.0f, 0.0f, 5.0f };
                float cosAngle;
                float sinAngle;
                nlSinCos(&sinAngle, &cosAngle, mUnidentified024.m_aActualFacingDirection);

                nlVector3 velocity;
                velocity.x = launchVelocity.x * cosAngle
                           - launchVelocity.y * sinAngle;
                velocity.y = launchVelocity.y * cosAngle
                           + launchVelocity.x * sinAngle;
                velocity.z = launchVelocity.z;
                SetVelocity(velocity);

                if (!mUnidentified348)
                {
                    EmitElectrocutionExplosion("electrocution_explosion", this);
                }
            }
            else
            {
                SetAnimState(0x7A, true, 0.2f, false, false);
            }

            EndElectrocution(this);
            InitMovementCoast();
            PlayRumbleAction(1, GetGlobalPad());
        }
        break;
    }
    case 0x77:
    case 0x7A:
    {
        nlVector3 velocity = mUnidentified024.m_v3Velocity;
        velocity.x *= 0.99f;
        velocity.y *= 0.99f;
        velocity.z = -30.0f * dt + velocity.z;
        SetVelocity(velocity);

        mUnidentified024.m_v3Position.z += dt * mUnidentified024.m_v3Velocity.z;
        if (mUnidentified024.m_v3Position.z < 0.0f)
        {
            mUnidentified024.m_v3Position.z = 0.0f;
            mUnidentified024.m_v3Velocity.z = 0.0f;

            if (m_eAnimID == 0x77)
            {
                SetAnimState(0x78, true, 0.2f, false, false);
            }
            else
            {
                SetAnimState(0x7B, true, 0.2f, false, false);
            }
            InitMovementFromAnim(0, v3Zero, 0.0f, false);
            EmitElectrocution(this);
            PlayRumbleAction(1, GetGlobalPad());
        }
        else
        {
            if (m_pCurrentAnimController->TestTrigger(lbl_806DB9A4))
            {
                EmitElectrocution(this);
            }
            if (m_pCurrentAnimController->TestTrigger(lbl_806DB9A8))
            {
                EndElectrocution(this);
            }
        }
        break;
    }
    case 0x78:
    case 0x7B:
    {
        if (m_pCurrentAnimController->TestTrigger(lbl_806DB9AC))
        {
            EndElectrocution(this);
            PlayRumbleAction(1, GetGlobalPad());
        }
        if (m_pCurrentAnimController->TestTrigger(lbl_806DB9B0))
        {
            EmitElectrocution(this);
            PlayRumbleAction(1, GetGlobalPad());
        }
        if (m_pCurrentAnimController->TestTrigger(lbl_806DB9B4))
        {
            EndElectrocution(this);
            PlayRumbleAction(1, GetGlobalPad());
        }

        if (ShouldStartCrossBlend(4))
        {
            EndAction();
        }
        break;
    }
    }
}

void cFielder::fn_80045930()
{
    mUnidentified024.m_v3Position.z = 0.0f;
    mUnidentified024.m_v3Velocity.z = 0.0f;

    nlVector3 v3Direction;
    if (!g_pBall->m_pPhysicsBall->mbUseWindForce)
    {
        Weather* pObject = g_pGame->mpWeatherManager->GetWeather(2);
        if (pObject != 0 && !pObject->mbPaused)
        {
            pObject->Start();
            v3Direction = g_pBall->m_pPhysicsBall->mv3WindForce;
        }
        else
        {
            nlVec3Set(v3Direction, 0.0f, 1.0f, 0.0f);
        }
    }
    else
    {
        v3Direction = g_pBall->m_pPhysicsBall->mv3WindForce;
    }

    nlVector3 v3Position = { 0.0f, 0.0f, 0.0f };
    v3Position.x = nlRandomf(4.0f) - 2.0f;
    v3Position.y = nlRandomf(4.0f) - 2.0f;

    nlVec3Normalize(v3Direction, v3Direction);
    nlVec3Scale(v3Direction, v3Direction, 40.0f);
    nlVec3Sub(v3Position, v3Position, v3Direction);
    SetPosition(v3Position);

    SetAnimState(0x7E, true, 0.2f, false, false);
    InitMovementCoast();
}

void cFielder::fn_80045AEC(PhysicsObject* pObject)
{
    if (!fn_800344B0())
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
            g_pBall->ShootRelease(mUnidentified024.m_v3Velocity, SPINTYPE_NONE);
        }

        EndBirdoSuperPower();
        EndKoopaSuperPower();
        EndFrozenOrDazed();
        EndStar();
        EndMushroom();

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)0x18);
        SetAnimState(0x7C, false, 0.2f, false, false);
        InitMovementCoast();
        SetVelocity(v3Zero);

        mUnidentified34C = lbl_806DB90C;
        mUnidentified350 = pObject->GetPosition();

        KillSlideTackleTrail(this, 1);
        KillHitTrail(this, 1);
        KillWindups();

        bool bHasPad = GetGlobalPad() != 0;
        if (bHasPad)
        {
            SwapController(false);
        }

        bool bUnidentified = IsCaptain();
        unsigned long soundID = 0x1CF82176;
        if (bUnidentified)
        {
            soundID = 0xFDEC8E0F;
        }
        PlaySound(m_uSoundSlotId, soundID, 0, 0);
    }
}

extern const float lbl_806E35D4[1] = { 8.0f };

void cFielder::fn_80045C74(float fDeltaT)
{
    if (mUnidentified34C > 0.0f)
    {
        mUnidentified34C -= fDeltaT;

        int nSpinStep = (u16)(lbl_806DB918
                              * (lbl_806DB914
                                      * InterpolateRangeClamped(
                                          0.0f, 1.0f, 0.0f, lbl_806DB910, mUnidentified024.m_v3Velocity.z)
                                  + 1.0f));
        Unknown8(mUnidentified024.m_aActualFacingDirection + nSpinStep, false);

        SetFacingDirection(
            SeekDirection(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection,
                GetShotWindupTurnSpeed(this->GetTweaks()),
                GetShotWindupTurnFalloff(this->GetTweaks()),
                fDeltaT),
            true);

        float fT = FMIN((float)fabs(mUnidentified34C - lbl_806DB90C)
                            / (lbl_806DB90C * lbl_806DB908),
            1.0f);

        nlVector3 v3NewPosition;
        v3NewPosition.x
            = (1.0f - fT) * mUnidentified024.m_v3Position.x + fT * mUnidentified350.x;
        v3NewPosition.y
            = (1.0f - fT) * mUnidentified024.m_v3Position.y + fT * mUnidentified350.y;
        v3NewPosition.z
            = (1.0f - fT) * mUnidentified024.m_v3Position.z + fT * mUnidentified350.z;
        v3NewPosition.z = mUnidentified024.m_v3Position.z;
        SetPosition(v3NewPosition);

        mUnidentified024.m_v3Position.z += fDeltaT * mUnidentified024.m_v3Velocity.z;
        if (mUnidentified024.m_v3Position.z >= 50.0f)
        {
            mUnidentified024.m_v3Position.z = 50.0f;
        }

        float fNewVelocityZ = SeekSpeedExponential(
            mUnidentified024.m_v3Velocity.z, lbl_806DB910, lbl_806DB91C, fDeltaT);
        nlVector3 v3NewVelocity = v3Zero;
        v3NewVelocity.z = fNewVelocityZ;
        SetVelocity(v3NewVelocity);

        if (mUnidentified34C <= 0.0f)
        {
            fn_80045930();
        }
    }
    else if (m_eAnimID == 0x7E)
    {
        nlVector3 v3Delta;
        v3Delta.y = v3Zero.y - mUnidentified024.m_v3Position.y;
        v3Delta.x = v3Zero.x - mUnidentified024.m_v3Position.x;
        v3Delta.z = v3Zero.z - mUnidentified024.m_v3Position.z;
        Unknown8(nlVector3ToAngle(v3Delta), false);

        SetFacingDirection(
            SeekDirection(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection,
                fn_8002C0AC(this->GetTweaks()),
                fn_8002CF10(this->GetTweaks()),
                fDeltaT),
            true);

        nlPolarToCartesian(mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y, mUnidentified024.m_aActualFacingDirection, InterpolateRangeClamped(lbl_806DB8B4, lbl_806DB8B8, 0.0f, 4.0f, fn_800A6388(m_pTeam)));

        nlVector2 v3Distance = {
            mUnidentified024.m_v3Position.x - v3Zero.x,
            mUnidentified024.m_v3Position.y - v3Zero.y,
        };
        if (nlSqrt(v3Distance.x * v3Distance.x
                       + v3Distance.y * v3Distance.y,
                true)
            < lbl_806E35D4[0])
        {
            SetAnimState(0x7F, true, 0.2f, false, false);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else
    {
        if (ShouldStartCrossBlend(4))
        {
            bool bHasPad = GetGlobalPad() != 0;
            if (!bHasPad)
            {
                bool bGiven = false;
                for (int i = 0; i < 4; i++)
                {
                    cFielder* pOther = m_pTeam->GetFielder(i);
                    if (pOther != this)
                    {
                        bool bOtherHasPad = pOther->GetGlobalPad() != 0;
                        if (bOtherHasPad
                            && (pOther->fn_800344B0() || pOther->fn_80038918()))
                        {
                            SetAIPad(pOther->m_pController);
                            mUnidentified1E4.m_bCanTestController = false;
                            pOther->SetAIPad(0);
                            bGiven = true;
                        }
                    }
                }

                if (!bGiven)
                {
                    for (int i = 0; i < 0x10; i++)
                    {
                        cAIPad* pPad = GetAIPad(i);
                        if (pPad != 0)
                        {
                            int mySide = m_pTeam->m_nSide;
                            short playingSide
                                = GameInfoManager::Instance()->GetPlayingSide(
                                    (u16)i);
                            if (playingSide == mySide)
                            {
                                bool bTaken = false;
                                for (int j = 0; j < 5; j++)
                                {
                                    cPlayer* pPlayer = m_pTeam->GetPlayer(j);
                                    bool bPlayerHasPad
                                        = pPlayer->GetGlobalPad() != 0;
                                    if (bPlayerHasPad
                                        && pPlayer->m_pController == pPad)
                                    {
                                        bTaken = true;
                                    }
                                }
                                if (!bTaken)
                                {
                                    SetAIPad(pPad);
                                }
                            }
                        }
                    }
                }
            }

            EndAction();
        }
    }
}

void cFielder::fn_80046244()
{
    if (!fn_800344B0())
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
            g_pBall->ShootRelease(mUnidentified024.m_v3Velocity, SPINTYPE_NONE);
            SetBallFallState(g_pBall);
        }

        EndBirdoSuperPower();
        EndKoopaSuperPower();
        EndFrozenOrDazed();
        EndStar();
        EndMushroom();

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)3);
        SetAnimState(0x7C, false, 0.2f, false, false);
        InitMovementCoast();

        nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;
        v3Velocity.y += AIsgn(mUnidentified024.m_v3Position.y);
        v3Velocity.z += 3.5f;
        SetVelocity(v3Velocity);

        mUnidentified17C = false;

        mUnidentified34C = InterpolateRangeClamped(lbl_806DB8BC,
            lbl_806DB8C0,
            3.0f,
            0.0f,
            fn_800A6388(m_pTeam));

        KillSlideTackleTrail(this, 1);
        KillHitTrail(this, 1);
        KillWindups();

        bool bHasPad = GetGlobalPad() != 0;
        if (bHasPad)
        {
            SwapController(false);
        }

        if (GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = 0;
            m_pPhysicsCharacter->m_CanCollideWithWall = 0;
        }

        bool bUnidentified = IsCaptain();
        unsigned long soundID = 0x1CF82176;
        if (bUnidentified)
        {
            soundID = 0xFDEC8E0F;
        }
        PlaySound(m_uSoundSlotId, soundID, 0, 0);
    }
}

void cFielder::fn_8004643C(float fDeltaT)
{
    if (mUnidentified34C > 0.0f)
    {
        if (mUnidentified024.m_v3Position.z > -40.0f)
        {
            mUnidentified024.m_v3Position.z += fDeltaT * mUnidentified024.m_v3Velocity.z;
        }
        float fMinVelocity = -15.0f;
        mUnidentified34C -= fDeltaT;

        nlVector3 v3Velocity = GetVelocity();
        if (v3Velocity.z >= fMinVelocity)
        {
            v3Velocity.z += -16.0f * fDeltaT;
        }
        SetVelocity(v3Velocity);

        if (mUnidentified34C <= 0.0f)
        {
            fn_80045930();
        }
    }
    else if (m_eAnimID == 0x7E)
    {
        nlVector3 v3Delta;
        v3Delta.y = v3Zero.y - mUnidentified024.m_v3Position.y;
        v3Delta.x = v3Zero.x - mUnidentified024.m_v3Position.x;
        v3Delta.z = v3Zero.z - mUnidentified024.m_v3Position.z;
        Unknown8(nlVector3ToAngle(v3Delta), false);

        SetFacingDirection(
            SeekDirection(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection,
                fn_8002C0AC(this->GetTweaks()),
                fn_8002CF10(this->GetTweaks()),
                fDeltaT),
            true);

        nlPolarToCartesian(mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y, mUnidentified024.m_aActualFacingDirection, InterpolateRangeClamped(lbl_806DB8B4, lbl_806DB8B8, 0.0f, 4.0f, fn_800A6388(m_pTeam)));

        nlVector2 v3Distance = {
            mUnidentified024.m_v3Position.x - v3Zero.x,
            mUnidentified024.m_v3Position.y - v3Zero.y,
        };
        if (nlSqrt(v3Distance.x * v3Distance.x
                       + v3Distance.y * v3Distance.y,
                true)
            < lbl_806E35D4[0])
        {
            SetAnimState(0x7F, true, 0.2f, false, false);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else
    {
        if (ShouldStartCrossBlend(4))
        {
            bool bHasPad = GetGlobalPad() != 0;
            if (!bHasPad)
            {
                bool bGiven = false;
                for (int i = 0; i < 4; i++)
                {
                    cFielder* pOther = m_pTeam->GetFielder(i);
                    if (pOther != this)
                    {
                        bool bOtherHasPad = pOther->GetGlobalPad() != 0;
                        if (bOtherHasPad
                            && (pOther->fn_800344B0() || pOther->fn_80038918()))
                        {
                            SetAIPad(pOther->m_pController);
                            mUnidentified1E4.m_bCanTestController = false;
                            pOther->SetAIPad(0);
                            bGiven = true;
                        }
                    }
                }

                if (!bGiven)
                {
                    for (int i = 0; i < 0x10; i++)
                    {
                        cAIPad* pPad = GetAIPad(i);
                        if (pPad != 0)
                        {
                            int mySide = m_pTeam->m_nSide;
                            short playingSide
                                = GameInfoManager::Instance()->GetPlayingSide(
                                    (u16)i);
                            if (playingSide == mySide)
                            {
                                bool bTaken = false;
                                for (int j = 0; j < 5; j++)
                                {
                                    cPlayer* pPlayer = m_pTeam->GetPlayer(j);
                                    bool bPlayerHasPad
                                        = pPlayer->GetGlobalPad() != 0;
                                    if (bPlayerHasPad
                                        && pPlayer->m_pController == pPad)
                                    {
                                        bTaken = true;
                                    }
                                }
                                if (!bTaken)
                                {
                                    SetAIPad(pPad);
                                }
                            }
                        }
                    }
                }
            }

            EndAction();
        }
    }
}

void cFielder::InitActionHit(cFielder* pTarget, unsigned short aDirection)
{
    if (!IsStuck())
    {
        float fSpeedScale = InterpolateRangeClamped(lbl_806DB894,
            lbl_806DB898,
            0.35f,
            0.75f,
            g_pGame->mpTerrain->GetSpeedFactor());
        float fStartTime
            = fSpeedScale * (fn_8002D020(this->GetTweaks()) / 30.0f);
        float fEndTime
            = fSpeedScale * (fn_8002D050(this->GetTweaks()) / 30.0f);
        float fTimeRange = fEndTime - fStartTime;
        float fMoveDistance = fn_80030750(this);

        if (pTarget == 0)
        {
            pTarget = DoFindBestHitTarget();
        }

        if (pTarget != 0 && !fn_8003E70C())
        {
            float distance = fMoveDistance / fTimeRange;

            float fMidTime = fTimeRange / 2.0f + fStartTime;

            nlVector3 targetVelocity = pTarget->mUnidentified024.m_v3Velocity;

            if (pTarget->m_eActionState == 1
                && pTarget->m_pCurrentAnimController->m_fTime < 0.66f)
            {
                targetVelocity = v3Zero;
            }

            nlVector3 interceptPos;
            nlVec3ScaleAdd(interceptPos, fMidTime, targetVelocity, pTarget->mUnidentified024.m_v3Position);

            int nInterceptResult = 0;
            float fInterceptTimes[2];
            float combinedRadius
                = fn_8002BFA8(this->GetTweaks(), GetPlayerScale())
                + fn_8002BFA8(pTarget->GetTweaks(), pTarget->GetPlayerScale());
            CalcInterceptXY(mUnidentified024.m_v3Position, distance, combinedRadius, pTarget->mUnidentified024.m_v3Position, targetVelocity, nInterceptResult, fInterceptTimes);

            float fTime;

            if (nInterceptResult != 0)
            {
                if (nInterceptResult == 2)
                {
                    fTime = nlMinEquals(
                        fInterceptTimes[0], fInterceptTimes[1]);
                }
                else
                {
                    fTime = fInterceptTimes[0];
                }
            }
            else
            {
                fTime = 0.5f * fStartTime + 0.5f * fEndTime;
            }

            nlVec3ScaleAdd(interceptPos, fTime, targetVelocity, pTarget->mUnidentified024.m_v3Position);

            nlVector3 v3Delta;
            nlVec3Sub(v3Delta, interceptPos, mUnidentified024.m_v3Position);
            Unknown8(nlVector3ToAngle(v3Delta), false);
            SetFacingDirection(mUnidentified024.m_aDesiredFacingDirection, true);
        }
        else if (fn_8003E70C())
        {
            SetFacingDirection(mUnidentified024.m_aActualFacingDirection, true);
        }
        else
        {
            Unknown8(aDirection, false);
            SetFacingDirection(aDirection, true);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_HIT);
        SetAnimState(0x67, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        m_pCurrentAnimController->m_fPlaybackSpeedScale = fSpeedScale;

        PlayerAttackData* pData = g_PlayerAttackDataPool.Allocate();
        pData->pAttacker = this;
        bool bHasGlobalPad = GetGlobalPad() != 0;
        pData->nAttackerPadID
            = bHasGlobalPad ? GetGlobalPad()->GetPadID() : -1;
        pData->pTarget = pTarget;
        pData->mUnidentified10 = false;
        QueueAttackAttemptEvent(g_pGame, pData);

        if (mUnidentified024.m_eCharacterClass == TOAD)
        {
            lbl_806E12C8
                ->CreatePatch(6, this, mUnidentified024.m_v3Position, v3Zero, lbl_806DB8FC, lbl_806DB900, lbl_806DB904)
                ->SetEndRadiusTime(0.33f);
            PlaySound(m_uSoundSlotId, 0xA9AF871E, 0, 0);
        }
    }
}

void cFielder::ActionHit(float fDeltaT)
{
    if (mUnidentified024.m_eCharacterClass == TOAD)
    {
        for (int i = 0; i < 0x3C; i++)
        {
            PhysicsPatch* pEffect = lbl_806E12C8->fn_801745B8(i);
            if (pEffect != 0 && pEffect->m_Type == 6)
            {
                cSHierarchy* pHierarchy = m_pPoseAccumulator->GetBaseHierarchy();
                nlVector3 jointPos = GetJointPosition(
                    pHierarchy->GetNodeIndexByID(
                        nlStringLowerHash("bip01 Ponytail12")));
                pEffect->SetWorldPosition(jointPos);
            }
        }
    }

    if (mUnidentified024.m_eCharacterClass == MYSTERY)
    {
        if (m_pTeam->GetCaptain()->mUnidentified024.m_eCharacterClass == MARIO
            || m_pTeam->GetOtherTeam()->GetCaptain()->mUnidentified024.m_eCharacterClass
                   == MARIO)
        {
            cFielder* pCaptain = m_pTeam->GetCaptain();
            if (pCaptain->mUnidentified024.m_eCharacterClass != MARIO)
            {
                pCaptain = m_pTeam->GetOtherTeam()->GetCaptain();
            }

            const nlVector3& v2Position = GetPosition();

            float fOffsetY;
            float fOffsetX;
            nlPolarToCartesian(
                fOffsetX, fOffsetY, mUnidentified024.m_aActualFacingDirection, 1.2f);

            nlVector2 v2Target;
            nlVec2Set(v2Target, fOffsetX + v2Position.x, fOffsetY + v2Position.y);

            bool bBlocked = false;
            float fT1;
            float fT2;
            for (int i = 0; i < 0x14; i++)
            {
                PhysicsWaluigiWall* pObject
                    = pCaptain->mUnidentified3F8.mUnidentified08->GetWall(i);
                if (pObject != 0
                    && nlIntersectLineSegments2D((const nlVector2*)&v2Position, &v2Target, (const nlVector2*)&pObject->GetStartPoint(), (const nlVector2*)&pObject->GetEndPoint(), &fT1, &fT2))
                {
                    bBlocked = true;
                }
            }

            if (bBlocked)
            {
                if (mUnidentified024.m_eMovementState != MOVEMENT_NONE)
                {
                    SetVelocity(v3Zero);
                    InitMovementNone(0.0f, 0.0f);
                }
            }
            else if (mUnidentified024.m_eMovementState != MOVEMENT_FROM_ANIM)
            {
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
            }
        }
    }

    if (ShouldStartCrossBlend(4))
    {
        mUnidentified024.m_fActualSpeed = 0.0f;
        mUnidentified024.m_fDesiredSpeed = 0.0f;
        SetVelocity(v3Zero);
        InitMovementNone(0.0f, 0.0f);
        EndAction();
    }
}

int gHitReactAnims[3][4] = {
    { 0x6A, 0x6D, 0x6C, 0x6B },
    { 0x6E, 0x71, 0x70, 0x6F },
    { 0x72, 0x75, 0x74, 0x73 },
};

bool cFielder::fn_800470B4(cFielder* pFielder, cPlayer* pAttacker)
{
    nlVector3 v3Delta;
    s16 nFacingDelta;
    u16 aAngle;

    v3Delta.y = pFielder->mUnidentified024.m_v3Position.y - pAttacker->mUnidentified024.m_v3Position.y;
    v3Delta.x = pFielder->mUnidentified024.m_v3Position.x - pAttacker->mUnidentified024.m_v3Position.x;
    v3Delta.z = pFielder->mUnidentified024.m_v3Position.z - pAttacker->mUnidentified024.m_v3Position.z;
    aAngle = nlVector3ToAngle(v3Delta);
    nFacingDelta = aAngle - pAttacker->mUnidentified024.m_aActualFacingDirection;

    float fIntensityA
        = pFielder->GetTweaks()->mUnidentified064;
    float fIntensityB = 1.0f;
    if (pAttacker->m_eClassType == FIELDER)
    {
        fIntensityB
            = ((cFielder*)pAttacker)->GetTweaks()->mUnidentified064;
    }

    int nReact = 1;
    if (GameInfoManager::Instance()->IsRule0x8Equal1())
    {
        nReact = 2;
    }
    else if (fIntensityB < 0.0f && fIntensityA >= 0.0f)
    {
        nReact = 0;
    }
    else if (fIntensityA < 0.0f && fIntensityB >= 0.0f)
    {
        nReact = 2;
    }
    else if ((u16)abs_s16(nFacingDelta) < 0x4000)
    {
        nReact = 2;
    }

    if (pAttacker->mUnidentified024.m_eCharacterClass == MYSTERY
        && ((cFielder*)pAttacker)->m_eActionState == 1)
    {
        aAngle = pFielder->mUnidentified024.m_aActualFacingDirection;
    }

    return pFielder->fn_80047240(pAttacker, aAngle, nReact, false, false);
}

bool cFielder::fn_80047240(cPlayer* pAttacker, unsigned short aDirection,
    int nReact, bool bDoFrameLock, bool bBookPenalty)
{
    if (IsFallenDown() && mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0)
    {
        return false;
    }

    EndFrozenOrDazed();

    mUnidentified360 = bDoFrameLock;

    if (m_pBall != 0)
    {
        ReleaseBall(0);
        ShootBallDueToContact(pAttacker->mUnidentified024.m_v3Velocity);

        if (bBookPenalty)
        {
            static_cast<cFielder*>(pAttacker)->DoPenaltyCardBooking(this, PEN_TYPE_HIT_WITH_BALL);
        }
    }
    else if (bBookPenalty)
    {
        static_cast<cFielder*>(pAttacker)->DoPenaltyCardBooking(this, PEN_TYPE_HIT_NO_BALL);
    }

    if (!mUnidentified360)
    {
        SetPlayerAudioController(this);

        unsigned long soundID;
        if (pAttacker->IsCaptain())
        {
            bool bUnidentified = IsCaptain();
            soundID = 0xE606A2;
            if (bUnidentified)
            {
                soundID = 0x3642C41B;
            }
        }
        else
        {
            bool bUnidentified = IsCaptain();
            soundID = 0xBDD19FFF;
            if (bUnidentified)
            {
                soundID = 0xBD539FB8;
            }
        }
        PlaySound(m_uSoundSlotId, soundID, 0, 0);
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)5);

    s16 angleDiff
        = (s16)((u16)(aDirection + 0x8000) - mUnidentified024.m_aActualFacingDirection);
    u32 index = (u32)((s16)(angleDiff + 0x2000)) >> 14 & 3;

    SetAnimState(gHitReactAnims[nReact][index], true, 0.2f, false, false);
    SetFacingDirection(
        (u16)(aDirection + gHitReactFacingOffsets[index]), true);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    if (g_pGame->IsGameplayOrOvertime())
    {
        StatsTracker::Instance()->TrackStat((ePlayerStats)0x12,
            pAttacker->m_pTeam->m_nSide,
            pAttacker->mUnidentified1E4.m_ID,
            0,
            0,
            0,
            0);
    }

    switch (nReact)
    {
    case 0:
        PlayRumbleAction(2, GetGlobalPad());
        break;
    case 1:
        PlayRumbleAction(3, GetGlobalPad());
        break;
    case 2:
        PlayRumbleAction(4, GetGlobalPad());
        break;
    }

    mUnidentified024.m_fDesiredSpeed = 0.0f;
    return true;
}

void cFielder::fn_800474FC(float fDeltaT)
{
    if (m_pCurrentAnimController->TestFrameTrigger(lbl_806DB8EC)
        && mUnidentified360)
    {
        DeliverCaptainClashPresentationEvent(g_pGame);
    }

    if (ShouldStartCrossBlend(4))
    {
        mUnidentified024.m_fActualSpeed = 0.0f;
        mUnidentified024.m_fDesiredSpeed = 0.0f;
        SetVelocity(v3Zero);
        InitMovementNone(0.0f, 0.0f);
        EndAction();
    }
}

void cFielder::InitActionIdleTurn(unsigned short desiredFacingDirection)
{
    Unknown8(desiredFacingDirection, false);
    SetAnimState(0, true, 0.2f, false, false);
    InitMovementNone(75000.0f, 4000.0f);
    SetAction(ACTION_IDLE_TURN);
}

void cFielder::ActionIdleTurn(float fDeltaT)
{
    s16 angleDiff
        = (u16)abs_s16(mUnidentified024.m_aDesiredFacingDirection - mUnidentified024.m_aActualFacingDirection);

    if (angleDiff < g_IdleTurnCompletionDelta)
    {
        EndAction();
    }
}

void cFielder::InitActionLateOneTimerFromVolley()
{
    bIsModified = false;

    DoResetShotMeter(0.0f);

    ShotMeter* pShotMeter = m_pShotMeter;
    pShotMeter->CalcOneTimerValue(this, true);

    SetAction(ACTION_LATE_ONETIMER_FROM_VOLLEY);

    int LateOneTimerFromVolleyAnims[4] = {
        0x4A,
        0x4D,
        0x4C,
        0x4B,
    };

    s16 facingDelta = GetFacingDeltaToPosition(
        m_pTeam->GetOtherNet()->m_v3NetLocation);
    int index = (u16)(facingDelta + 0x2000) >> 14;

    int nAnimID;
    bool bMirror = false;
    if (index == 2)
    {
        bMirror = m_pCurrentAnimController->m_bMirror;
    }

    nAnimID = LateOneTimerFromVolleyAnims[index];

    s16 nTurnAdjust = 0;
    switch (nAnimID)
    {
    case 0x4C:
        nTurnAdjust = -0x8000;
        break;
    case 0x4D:
        nTurnAdjust = -0x4000;
        break;
    case 0x4B:
        nTurnAdjust = 0x4000;
        break;
    }

    s16 facingDelta2 = GetFacingDeltaToPosition(
        m_pTeam->GetOtherNet()->m_v3NetLocation);

    SetAnimState(nAnimID, true, 0.0f, false, bMirror);

    InitMovementFromAnim(
        (s16)(nTurnAdjust + facingDelta2), v3Zero, 0.6f, false);

    bool bShotNormally = true;
    if (ShouldIClearBall())
    {
        DoClearBall();
        bShotNormally = false;
    }
    else
    {
        DoRegularShooting(false);
        DeliverShotPresentationEndEvent(g_pGame);
    }

    EmitBallShot(this, BALL_EFFECT_PERFECT_SHOT, 0, 0, bShotNormally);

    bool bUnidentified = IsCaptain();
    unsigned long soundID = 0x1CEC5A02;
    if (bUnidentified)
    {
        soundID = 0xFDE0C69B;
    }
    PlaySound(m_uSoundSlotId, soundID, 0, 0);
}

void cFielder::ActionLateOneTimerFromVolley(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

// Preserve the full desired direction until subtracting; narrow only the delta.
static inline s16 ContactFacingDelta(u32 desired, u16 actual)
{
    return (s16)(desired - actual);
}

bool cFielder::DoCommonInitActionLooseBall(
    const nlVector3& rv3OneTimerTarget, bool bVolleyPass)
{
    nlVector3 v3SimulatedBallPos;
    nlVector3 v3IdleGroundContactOffset;
    nlVector3 v3IdleVolleyContactOffset;
    nlVector3 v3LeadGroundContactOffset;
    nlVector3 v3BallToSelf;
    nlVector3 v3ToTarget;
    nlVector3 v3ContactOffsetLocal;
    nlVector3 v3ContactOffsetWorld;
    nlVector3 v3TmpAdjustment;
    nlVector3 v3MoveAdjustment;
    float fBallDelta[2];
    float fGroundReachDelta[2];
    float fVolleyReachDelta[2];
    float fLeadReachDelta[2];
    float fPhysicsRadius;

    cSAnim* pIdleGroundContactAnim = m_pAnimInventory->GetAnim(
        GetOneTimerIdleGroundContactAnims()->nAnimID);
    float fContactFrame = GetOneTimerIdleGroundContactAnims()->fAnimContactFrame;
    float fMaxSimulatedTime
        = fContactFrame / (float)pIdleGroundContactAnim->m_nNumKeys;

    float fSimulatedTime = 0.0f;

    FakeBallWorld::ResetBallIterator();

    const LooseBallContactAnimInfo* pAnimInfoList
        = GetOneTimerIdleGroundContactAnims();
    int nNumAnims = GetNumOneTimerIdleGroundContactAnims();
    fContactFrame = pAnimInfoList->fAnimContactFrame;

    GetJointPositionFuture(&v3IdleGroundContactOffset,
        pAnimInfoList->nAnimID,
        m_nBallJointIndex,
        fContactFrame / (float)pIdleGroundContactAnim->m_nNumKeys,
        true,
        true,
        true,
        true);

    cSAnim* pIdleVolleyContactAnim
        = m_pAnimInventory->GetAnim(pAnimInfoList->nAnimID);
    fContactFrame = GetOneTimerIdleVolleyContactAnims()->fAnimContactFrame;
    float fIdleVolleyTime
        = fContactFrame / (float)pIdleVolleyContactAnim->m_nNumKeys;
    GetJointPositionFuture(&v3IdleVolleyContactOffset,
        GetOneTimerIdleVolleyContactAnims()->nAnimID,
        m_nBallJointIndex,
        fIdleVolleyTime,
        true,
        true,
        true,
        true);

    cSAnim* pLeadGroundContactAnim
        = m_pAnimInventory->GetAnim(pAnimInfoList->nAnimID);
    fContactFrame = GetOneTimerLeadGroundContactAnims()->fAnimContactFrame;
    float fLeadGroundTime
        = fContactFrame / (float)pLeadGroundContactAnim->m_nNumKeys;
    GetJointPositionFuture(&v3LeadGroundContactOffset,
        GetOneTimerLeadGroundContactAnims()->nAnimID,
        m_nBallJointIndex,
        fLeadGroundTime,
        true,
        true,
        true,
        true);

    float fMaxCatchupSpeed = GetRunningSpeed();
    float fMinBallZ = 100.0f;
    bool bBallState5 = g_pBall->meBallState == 5;
    bool bNoContactFound = true;

    while (fSimulatedTime < fMaxSimulatedTime)
    {
        FakeBallWorld::GetNextBallPosition(v3SimulatedBallPos);
        fSimulatedTime += FixedUpdateTask::GetPhysicsUpdateTick();

        if (v3SimulatedBallPos.z < fMinBallZ)
        {
            fMinBallZ = v3SimulatedBallPos.z;
        }

        fBallDelta[0] = v3SimulatedBallPos.x - mUnidentified024.m_v3Position.x;
        fBallDelta[1] = v3SimulatedBallPos.y - mUnidentified024.m_v3Position.y;
        float fBallDistance = nlSqrt(fBallDelta[0] * fBallDelta[0]
                                         + fBallDelta[1] * fBallDelta[1],
            true);

        if (!bBallState5)
        {
            if (v3SimulatedBallPos.z
                < g_pBall->fn_80014F38(GetPlayerScale()))
            {
                float fAnimTime
                    = (pAnimInfoList->fAnimContactFrame / 30.0f)
                    / fSimulatedTime;
                fGroundReachDelta[0]
                    = v3IdleGroundContactOffset.x - mUnidentified024.m_v3Position.x;
                fGroundReachDelta[1]
                    = v3IdleGroundContactOffset.y - mUnidentified024.m_v3Position.y;
                float fCatchup = fBallDistance
                               - nlSqrt(fGroundReachDelta[0] * fGroundReachDelta[0]
                                            + fGroundReachDelta[1] * fGroundReachDelta[1],
                                   true);
                if (fCatchup > 0.0f)
                {
                    fCatchup /= fSimulatedTime;
                }
                bool bCanReach = fCatchup <= fMaxCatchupSpeed;
                if (fAnimTime > lbl_806DB8C4 && fAnimTime < lbl_806DB8C8
                    && bCanReach)
                {
                    bNoContactFound = false;
                    break;
                }
            }

            float fVolleyZDelta = (float)fabs(
                v3SimulatedBallPos.z - v3IdleVolleyContactOffset.z);
            if (fVolleyZDelta < g_pBall->fn_80014F38(GetPlayerScale()))
            {
                float fAnimTime
                    = (GetOneTimerIdleVolleyContactAnims()->fAnimContactFrame
                          / 30.0f)
                    / fSimulatedTime;
                fVolleyReachDelta[0]
                    = v3IdleVolleyContactOffset.x - mUnidentified024.m_v3Position.x;
                fVolleyReachDelta[1]
                    = v3IdleVolleyContactOffset.y - mUnidentified024.m_v3Position.y;
                float fCatchup = fBallDistance
                               - nlSqrt(fVolleyReachDelta[0] * fVolleyReachDelta[0]
                                            + fVolleyReachDelta[1] * fVolleyReachDelta[1],
                                   true);
                if (fCatchup > 0.0f)
                {
                    fCatchup /= fSimulatedTime;
                }
                bool bCanReach = fCatchup <= fMaxCatchupSpeed;
                if (fAnimTime > lbl_806DB8C4 && fAnimTime < lbl_806DB8C8
                    && bCanReach)
                {
                    pAnimInfoList = GetOneTimerIdleVolleyContactAnims();
                    nNumAnims = GetNumOneTimerIdleVolleyContactAnims();
                    bNoContactFound = false;
                    break;
                }
            }
        }

        float fLeadZDelta = (float)fabs(
            v3SimulatedBallPos.z - v3LeadGroundContactOffset.z);
        if (fLeadZDelta < g_pBall->fn_80014F38(GetPlayerScale()))
        {
            float fAnimTime
                = (GetOneTimerLeadGroundContactAnims()->fAnimContactFrame
                      / 30.0f)
                / fSimulatedTime;
            fLeadReachDelta[0]
                = v3LeadGroundContactOffset.x - mUnidentified024.m_v3Position.x;
            fLeadReachDelta[1]
                = v3LeadGroundContactOffset.y - mUnidentified024.m_v3Position.y;
            float fCatchup = fBallDistance
                           - nlSqrt(fLeadReachDelta[0] * fLeadReachDelta[0]
                                        + fLeadReachDelta[1] * fLeadReachDelta[1],
                               true);
            if (fCatchup > 0.0f)
            {
                fCatchup /= fSimulatedTime;
            }
            bool bCanReach = fCatchup <= fMaxCatchupSpeed * lbl_806DB8CC;
            if (fAnimTime > lbl_806DB8C4 && fAnimTime < lbl_806DB8C8
                && bCanReach)
            {
                pAnimInfoList = GetOneTimerLeadGroundContactAnims();
                nNumAnims = GetNumOneTimerLeadGroundContactAnims();
                bNoContactFound = false;
                break;
            }
        }

        if (cField::FixOutOfBoundsPosition(v3SimulatedBallPos, 0.0f, false))
        {
            fSimulatedTime = fMaxSimulatedTime;
            break;
        }
    }

    if (bNoContactFound)
    {
        return false;
    }

    v3BallToSelf.y = mUnidentified024.m_v3Position.y - g_pBall->m_v3Position.y;
    v3BallToSelf.x = mUnidentified024.m_v3Position.x - g_pBall->m_v3Position.x;
    v3BallToSelf.z = mUnidentified024.m_v3Position.z - g_pBall->m_v3Position.z;

    const LooseBallContactAnimInfo* pBestBallContactAnimInfo
        = fn_80038230(pAnimInfoList, nNumAnims, mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_v3Position, rv3OneTimerTarget, nlATan2f(v3BallToSelf.y, v3BallToSelf.x));

    v3ToTarget.y = rv3OneTimerTarget.y - mUnidentified024.m_v3Position.y;
    v3ToTarget.x = rv3OneTimerTarget.x - mUnidentified024.m_v3Position.x;
    v3ToTarget.z = rv3OneTimerTarget.z - mUnidentified024.m_v3Position.z;
    u32 aDesiredFacingDirection = nlVector3ToAngle(v3ToTarget);

    switch (pBestBallContactAnimInfo->nAnimID)
    {
    case 0x35:
    case 0x39:
    case 0x45:
        aDesiredFacingDirection += 0x4000;
        break;
    case 0x36:
    case 0x3A:
    case 0x46:
        aDesiredFacingDirection -= 0x4000;
        break;
    case 0x3B:
    case 0x47:
    case 0x49:
        aDesiredFacingDirection += 0x8000;
        break;
    }

    int nFacingDelta
        = ContactFacingDelta(aDesiredFacingDirection, mUnidentified024.m_aActualFacingDirection);
    cSAnim* pBestContactAnim
        = m_pAnimInventory->GetAnim(pBestBallContactAnimInfo->nAnimID);

    fContactFrame = pBestBallContactAnimInfo->fAnimContactFrame;

    GetJointPositionFuture(&v3ContactOffsetLocal,
        pBestBallContactAnimInfo->nAnimID,
        m_nBallJointIndex,
        fContactFrame / (float)pBestContactAnim->m_nNumKeys,
        true,
        true,
        false,
        true);

    nlVec2Rotate(*(nlVector2*)&v3ContactOffsetWorld,
        *(const nlVector2*)&v3ContactOffsetLocal,
        aDesiredFacingDirection);
    v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;

    float fContactTime = pBestContactAnim->GetNormalizedTime(pBestBallContactAnimInfo->fAnimContactFrame);

    mUnidentified368 = fContactTime;

    SetAnimState(pBestBallContactAnimInfo->nAnimID, false, mUnidentified368 * lbl_806DB990, false, false);

    m_pCurrentAnimController->m_fPlaybackSpeedScale
        = (pBestBallContactAnimInfo->fAnimContactFrame / 30.0f)
        / fSimulatedTime;

    nlVec3Sub(v3TmpAdjustment, v3SimulatedBallPos, v3ContactOffsetWorld);
    nlVec3Sub(v3MoveAdjustment, v3TmpAdjustment, mUnidentified024.m_v3Position);

    InitMovementFromAnim(nFacingDelta, v3MoveAdjustment, mUnidentified368 * lbl_806DB990, false);

    m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&fPhysicsRadius);

    float fMaxGoalX = cField::GetGoalLineX(1U) - 0.5f;
    float fNetWidth = cNet::GetNetWidth();
    float fMaxGoalY = (0.5f * fNetWidth) + 1.5f;
    float fMinGoalY
        = ((0.5f * fNetWidth) - m_pTeam->m_pNet->GetPostRadius()) - fPhysicsRadius;

    float fAbsX = (float)fabs(v3SimulatedBallPos.x);
    if ((fAbsX < fMaxGoalX)
        || ((float)fabs(v3SimulatedBallPos.y) > fMaxGoalY)
        || ((float)fabs(v3SimulatedBallPos.y) < fMinGoalY))
    {
        m_pPhysicsCharacter->m_CanCollideWithWall = false;
    }

    return true;
}

void cFielder::InitActionLooseBallPass(cFielder* pPassTarget, bool bVolleyPass)
{
    cFielder* finalPassTarget;
    if (pPassTarget != 0)
    {
        finalPassTarget = pPassTarget;
    }
    else
    {
        finalPassTarget = static_cast<cFielder*>(fn_80096F54(this, bVolleyPass));
    }

    mActionLooseBallPassVars.passTarget = finalPassTarget;

    if (finalPassTarget == 0)
    {
        if (DoCommonInitActionLooseBall(
                m_pTeam->GetOtherNet()->m_v3NetLocation, false))
        {
            InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
            SetAction(ACTION_LOOSE_BALL_SHOT);
            bIsModified = false;
            SetNoPickUpTime(3.0f);

            bool bUnidentified = IsCaptain();
            unsigned long soundID = 0x1CEC5A02;
            if (bUnidentified)
            {
                soundID = 0xFDE0C69B;
            }
            PlaySound(m_uSoundSlotId, soundID, 0, 0);
        }
    }
    else if (DoCommonInitActionLooseBall(finalPassTarget->mUnidentified024.m_v3Position, true))
    {
        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_LOOSE_BALL_PASS);
        bIsModified = bVolleyPass;
        mUnidentified1E4.m_bCanTestController = false;
        SetNoPickUpTime(3.0f);
    }
}

void cFielder::fn_80048484(float fDeltaT)
{
    bool bIsChipShot = false;
    if (bIsModified || IsActionModifierPressed())
    {
        bIsChipShot = true;
    }
    bIsModified = bIsChipShot;

    if (m_pCurrentAnimController->TestTrigger(mUnidentified368))
    {
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionLooseBallShot(bool bIsChipShot)
{
    if (DoCommonInitActionLooseBall(
            m_pTeam->GetOtherNet()->m_v3NetLocation, false))
    {
        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_LOOSE_BALL_SHOT);
        bIsModified = bIsChipShot;
        SetNoPickUpTime(3.0f);

        bool bUnidentified = IsCaptain();
        unsigned long soundID = 0x1CEC5A02;
        if (bUnidentified)
        {
            soundID = 0xFDE0C69B;
        }
        PlaySound(m_uSoundSlotId, soundID, 0, 0);
    }
}

void cFielder::fn_800486DC(float fDeltaT)
{
    bool bIsChipShot = false;
    if (bIsModified || IsActionModifierPressed())
    {
        bIsChipShot = true;
    }
    bIsModified = bIsChipShot;

    if (m_pCurrentAnimController->TestTrigger(mUnidentified368))
    {
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void fn_80048870(cFielder* pFielder)
{
    lbl_806DB5A8 = false;

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 4; j++)
        {
            cFielder* pOther = pTeam->GetFielder(j);
            if (pFielder != pOther && !pOther->IsShattered())
            {
                pOther->ClearPowerupAnimState(false);
                fn_80031A30(pOther, 3, 999999.9f);
            }
        }
    }
}

void UnFreezeEveryoneButCaptain(cFielder* pCaptain)
{
    lbl_806DB5A8 = true;

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 4; j++)
        {
            cFielder* pFielder = pTeam->GetFielder(j);
            if (pCaptain != pFielder && pFielder->fn_8003881C()
                && g_pGame->m_uMegastrikeGoals == 0)
            {
                RequestStateMachineDeactivation(GetConcurrentState(fn_8002E1A4(pFielder), 0x1D));
            }
        }
    }
}

void cFielder::fn_800489C0(float)
{
}

float cFielder::fn_800489C4()
{
    float fShooting = this->GetTweaks()->fShooting;
    if (fShooting > 1.0f)
    {
        fShooting = 1.0f;
    }
    return InterpolateClamped(lbl_806DB948, lbl_806DB94C, fShooting);
}

float cFielder::fn_80048A08()
{
    float fShooting = this->GetTweaks()->fShooting;
    if (fShooting > 1.0f)
    {
        fShooting = 1.0f;
    }
    return InterpolateClamped(lbl_806DB950, lbl_806DB954, fShooting);
}

void cFielder::InitActionMegaStrikeMeter(bool bParam)
{
    tDebugPrintManager::Print(DC_NETWORK, "InitActionMegaStrikeMeter at frame %d\n", GetFixedUpdateTask()->GetFrame());

    mUnidentified390 = 0.0f;
    mUnidentified394 = 0.0f;
    mUnidentified398 = -1.0f;
    mUnidentified39C = -1.0f;
    mUnidentified3A0 = -1.0f;
    mUnidentified3A4 = -1.0f;
    mUnidentified3A8 = -1.0f;
    mUnidentified3AC = 0.0f;
    mUnidentified3B0 = 0.0f;
    mUnidentified3B4 = 0.0f;
    mUnidentified3B8 = false;
    mUnidentified3BC = 0.0f;
    mUnidentified3C0 = 0.0f;
    mUnidentified3C4 = 0.0f;
    mUnidentified3C8 = 0.0f;
    mUnidentified3CC = 0.0f;
    mUnidentified3D0 = 0.0f;
    mUnidentified3D4 = 0.0f;

    g_pGame->ResetMegaStrikeMeterQueues();

    mUnidentified478 = 0;

    if (m_pBall == 0)
    {
        m_pHeadTrack->m_bTrackOOI = true;

        if (m_eActionState != 0x12)
        {
            mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
            mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
            Unknown8(mUnidentified024.m_aActualFacingDirection, false);
            mActionRunningVars.bFirstCycleOfTurbo = false;
            mUnidentified024.m_fDesiredSpeed = mUnidentified024.m_fActualSpeed;
        }

        SetAction(ACTION_RUNNING);
        return;
    }

    if (mUnidentified3DC)
    {
        fn_8005001C(true);
    }

    bool bDidWindup = false;
    bool bNearGoal = false;
    float fAbsX = (float)fabs(mUnidentified024.m_v3Position.x);
    if (fAbsX > cField::GetGoalLineX(1U) - 2.0f)
    {
        float fNetWidth = cNet::m_fNetWidth;
        float fAbsY = (float)fabs(mUnidentified024.m_v3Position.y);
        if (fAbsY < 0.5f * fNetWidth + 1.0f)
        {
            bNearGoal = true;
        }
    }

    if (g_pGame->m_eGameState == 3 || bNearGoal)
    {
        KillWindup("ball_sts_windup");
        InitActionShot(false, false);
        bDidWindup = true;
    }

    if (!bDidWindup)
    {
        KillWindup("ball_sts_windup");
        SetAction(ACTION_SHOOT_TO_SCORE);

        if (m_eAnimID == 0x52)
        {
            SetAnimState(0x57, true, 0.2f, false, false);
        }
        else
        {
            SetAnimState(0x58, true, 0.2f, false, false);
        }

        nlVector3 v3NetLocation = m_pTeam->GetOtherNet()->m_v3NetLocation;

        float fDeltaX = v3NetLocation.x - mUnidentified024.m_v3Position.x;
        float fDeltaY = v3NetLocation.y - mUnidentified024.m_v3Position.y;
        float fAngleRad = nlATan2f(fDeltaY, fDeltaX);
        u16 nAngleUnits = (u16)(s32)(10430.378f * fAngleRad);
        s16 nTurnAdjust = CalcAnimTurnAdjust(
            mUnidentified024.m_aActualFacingDirection, nAngleUnits, m_eAnimID, 1.0f);
        InitMovementFromAnim(nTurnAdjust, v3Zero, 0.05f, false);

        if (bParam)
        {
            ShootToScoreMeter::instance.m_v3MeterPosition
                = ShootToScoreMeter::instance.m_v3OriginalMeterPosition
                = mUnidentified024.m_v3Position;
            ShootToScoreMeter::instance.TurnOnMeter();
            PlaySound(0, 0xC4534945, 0, 0);
        }

        MegaStrikeMeterData event;
        event.pFielder = this;
        event.fMeterValue = mUnidentified3BC;
        nlVector3 v3Column;
        glViewProjectPointToViewport(GetLayerView(eCLV_Unshadowed), &mUnidentified024.m_v3Position, &v3Column);
        event.v3Position = v3Column;
        DeliverMegaStrikeMeterStartEvent(g_pGame, &event);

        g_pGame->mpWeatherManager->Pause();

        ClearPowerupAnimState(false);

        float fMeterTime = lbl_806DB924;
        mUnidentified3B4 = fMeterTime;
        mUnidentified3AC = fMeterTime;
        mUnidentified3B8 = false;

        float fShooting = this->GetTweaks()->fShooting;
        if (fShooting > 1.0f)
        {
            fShooting = 1.0f;
        }
        else if (fShooting < 0.0f)
        {
            fShooting = 0.0f;
        }

        float fSegmentA
            = InterpolateClamped(lbl_806DB95C, lbl_806DB958, fShooting);
        float fSegmentB
            = InterpolateClamped(lbl_806DB960, lbl_806DB964, fShooting);
        float fSegmentC
            = InterpolateClamped(lbl_806DB968, lbl_806DB96C, fShooting);
        float fSegmentD
            = InterpolateClamped(lbl_806E0C68, lbl_806E0C6C, fShooting);

        float fHalfA = fSegmentA / 2.0f;
        float fHalfB = fSegmentB / 2.0f;
        float fHalfC = fSegmentC / 2.0f;
        float fHalfD = fSegmentD / 2.0f;

        mUnidentified3C4 = lbl_806DB970 + fHalfA;
        ShootToScoreMeter::instance.SetSegment1Position(mUnidentified3C4);
        ShootToScoreMeter::instance.SetSegment1Width(fSegmentA);

        mUnidentified3C8 = fHalfB + (mUnidentified3C4 + fHalfA);
        ShootToScoreMeter::instance.SetSegment2Position(mUnidentified3C8);
        ShootToScoreMeter::instance.SetSegment2Width(fSegmentB);

        mUnidentified3CC = fHalfC + (mUnidentified3C8 + fHalfB);
        ShootToScoreMeter::instance.SetSegment3Position(mUnidentified3CC);
        ShootToScoreMeter::instance.SetSegment3Width(fSegmentC);

        mUnidentified3D0 = fHalfD + (mUnidentified3CC + fHalfC);
        ShootToScoreMeter::instance.SetSegment4Position(mUnidentified3D0);
        ShootToScoreMeter::instance.SetSegment4Width(fSegmentD);

        mUnidentified3D4 = mUnidentified3D0;

        StopSound(0x5C8E379, this);
        PlaySound(0, 0x5C8E379, "Needle Left", this);
    }
}

static inline float GetShotMeterMidpointRatio()
{
    return 0.5f;
}

void cFielder::fn_80048FB0(float fDeltaT, bool bButtonPressed, int nParam)
{
    if (bButtonPressed)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Button pushed phase %d time %f\n", mUnidentified3B8, mUnidentified3AC);
    }

    if (bButtonPressed)
    {
        if (!mUnidentified3B8)
        {
            if (mUnidentified39C < 0.0f)
            {
                mUnidentified39C = mUnidentified3A8;
                DoMegaMeterFirstButtonPressEvent(nParam);
            }
        }
        else if (mUnidentified3A0 < 0.0f)
        {
            mUnidentified3A0 = mUnidentified3A8;
            DoMegaMeterSecondButtonPressEvent(nParam);
        }
    }

    mUnidentified3AC -= fDeltaT;
    if (mUnidentified3AC < 0.0f)
    {
        if (!mUnidentified3B8)
        {
            if (mUnidentified39C < 0.0f)
            {
                mUnidentified39C = 1.0f;
                DoMegaMeterFirstButtonPressEvent(nParam);
            }

            float fMidTime = GetShotMeterMidpointRatio() * lbl_806DB934 + lbl_806DB93C;
            if (mUnidentified3A8 <= fMidTime)
            {
                mUnidentified3B0 = InterpolateRangeClamped(lbl_806DB928,
                    0.0f,
                    fMidTime,
                    0.0f,
                    mUnidentified3A8);
            }
            else
            {
                mUnidentified3B0 = InterpolateRangeClamped(lbl_806DB930,
                    lbl_806DB92C,
                    1.0f,
                    fMidTime,
                    mUnidentified3A8);
            }

            mUnidentified3A4 = InterpolateClamped(lbl_806DB934,
                lbl_806DB938,
                this->GetTweaks()->fShooting);
            float fSecondPhaseTime = InterpolateClamped(lbl_806DB940,
                lbl_806DB944,
                this->GetTweaks()->fShooting);

            mUnidentified3B8 = true;
            mUnidentified3AC = mUnidentified3B0;
            ShootToScoreMeter::instance.SetGreenRegionWidth(
                mUnidentified3A4);
            ShootToScoreMeter::instance.SetYellowRegionWidth(fSecondPhaseTime);
            ShootToScoreMeter::instance.SetGreenBarPosition(lbl_806DB93C);
            ShootToScoreMeter::instance.mbShowGreenRegion = true;
        }
        else if (mUnidentified3A0 < 0.0f)
        {
            mUnidentified3A0 = 0.0f;
            DoMegaMeterSecondButtonPressEvent(nParam);
        }
    }

    if (mUnidentified3B8)
    {
        mUnidentified3A8
            = (mUnidentified39C * mUnidentified3AC) / mUnidentified3B0;
    }
    else
    {
        mUnidentified3A8 = 1.0f - mUnidentified3AC / mUnidentified3B4;
    }

    if (mUnidentified3A0 < 0.0f)
    {
        ShootToScoreMeter::instance.SetWhiteBarPosition(mUnidentified3A8);
    }
    else
    {
        ShootToScoreMeter::instance.SetWhiteBarPosition(mUnidentified3A0);
    }

    ShootToScoreMeter::instance.SetSavedWhiteBarPosition(mUnidentified39C);
}

void cFielder::fn_8004923C(float fDeltaT, bool bButtonPressed, int nParam)
{
    if (mUnidentified478 == 0)
    {
        if (nParam != 0)
        {
            NetMessageMegaStrikeMeter message;
            if (g_pNetworkSession->IsLiveNetworkGame())
            {
                g_pGame->SendMegaStrikeMeter(bButtonPressed);
            }
            fn_80048FB0(fDeltaT, bButtonPressed, nParam);
        }
        else if (g_pNetworkSession->IsLiveNetworkGame()
                 && g_pGame->mReceivedMegaStrikeMeter.mCount != 0)
        {
            fn_80048FB0(fDeltaT,
                g_pGame->mReceivedMegaStrikeMeter.Pop(),
                nParam);
        }
    }
    else if (g_pNetworkSession->IsLiveNetworkGame() && nParam == 0
             && g_pGame->mReceivedMegaStrikeMeter.mCount != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Have unprocessed m_ReceivedMegaMeterQ %d in state %d.  "
            "Processing All Now.\n",
            g_pGame->mReceivedMegaStrikeMeter.mCount,
            mUnidentified478);

        while (g_pGame->mReceivedMegaStrikeMeter.mCount != 0)
        {
            fn_80048FB0(fDeltaT,
                g_pGame->mReceivedMegaStrikeMeter.Pop(),
                nParam);
        }
    }

    if (mUnidentified478 == 2)
    {
        if (!IsNetworkOrRecordedGame())
        {
            mUnidentified478 = 3;
        }
        else
        {
            float fEndTime = mUnidentified398 + lbl_806DB974;
            if (GetFixedUpdateTask()->mSimulationTime >= fEndTime)
            {
                mUnidentified478 = 3;
            }
        }
    }
    else if (mUnidentified478 == 3)
    {
        MegaStrikeMeterOverlay* pScene
            = (MegaStrikeMeterOverlay*)g_pOverlayManager->GetScene(
                (SceneList)0x64);
        pScene->mMegaStrikeStarted = true;
        SetAction((eFielderActionState)0xB);
        fn_80048870(0);

        m_pTeam->GetGoalie()->fn_8008E2D0();
        m_pTeam->GetOtherTeam()->GetGoalie()->fn_8008E2D0();

        g_pBall->mbBallFrozen = true;
        muInvincibleStatus |= 0x1F;

        m_pTeam->GetOtherTeam()->GetGoalie()->fn_8008EF58();

        g_pGame->mpWeatherManager->Stop(false);
        g_pGame->fn_80058704();
        g_pGame->mpMegaStrikeShooter = this;
        DeliverMegaStrikeIntroEvent(g_pGame, this);
    }
}

void cFielder::DoMegaMeterFirstButtonPressEvent(int nParam)
{
    tDebugPrintManager::Print(DC_NETWORK, "DoMegaMeterFirstButtonPressEvent at time %f\n", mUnidentified3AC);

    ShootToScoreMeter::instance.mbShowSavedWhiteBar = true;

    mUnidentified3BC = (float)(s32)GetMegaStrikeShotCount(this, 0);

    PlayRumbleAction(1, GetGlobalPad());

    MegaStrikeMeterData event;
    event.pFielder = this;
    event.fMeterValue = mUnidentified3BC;
    nlVector3 v3Column;
    glViewProjectPointToViewport(GetLayerView(eCLV_Unshadowed), &mUnidentified024.m_v3Position, &v3Column);
    event.v3Position = v3Column;
    DeliverMegaStrikeMeterFirstEvent(g_pGame, &event);

    if (mUnidentified3AC >= 0.0f)
    {
        mUnidentified3AC = -1.0f;
    }

    StopSound(0x5C8E379, this);

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 5; j++)
        {
            cPlayer* pPlayer = pTeam->GetPlayer(j);
            if (pPlayer->GetGlobalPad() != 0)
            {
                SetPlayerAudioController(pPlayer);
                PlaySound(0, 0xCC32C1A8, 0, 0);
            }
        }
    }

    StopSound(0xBF541A4C, this);
    PlaySound(0, 0xBF541A4C, "Needle Right", this);
}

void cFielder::DoMegaMeterSecondButtonPressEvent(int nParam)
{
    tDebugPrintManager::Print(DC_NETWORK, "DoMegaMeterSecondButtonPressEvent at time %f\n", mUnidentified3AC);

    ShootToScoreMeter::instance.mbSecondButtonPressed = true;
    ShootToScoreMeter::instance.m_v3MeterPosition
        = ShootToScoreMeter::instance.m_v3OriginalMeterPosition;

    mUnidentified3C0 = GetMegaStrikeAccuracy(this, 0);

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 5; j++)
        {
            cPlayer* pPlayer = pTeam->GetPlayer(j);
            if (pPlayer->GetGlobalPad() != 0)
            {
                if (mUnidentified3C0 >= 1.0f)
                {
                    PlayRumbleAction(3, pPlayer->GetGlobalPad());
                    SetPlayerAudioController(pPlayer);
                    PlaySound(0, 0xD17A65BA, 0, 0);
                }
                else if (mUnidentified3C0 >= 0.0f)
                {
                    PlayRumbleAction(2, pPlayer->GetGlobalPad());
                    SetPlayerAudioController(pPlayer);
                    PlaySound(0, 0xCC2F680B, 0, 0);
                }
                else
                {
                    PlayRumbleAction(1, pPlayer->GetGlobalPad());
                    SetPlayerAudioController(pPlayer);
                    PlaySound(0, 0xCC36B742, 0, 0);
                }
            }
        }
    }

    MegaStrikeMeterData event;
    event.pFielder = this;
    event.fMeterValue = mUnidentified3C0;
    nlVector3 v3Column;
    glViewProjectPointToViewport(GetLayerView(eCLV_Unshadowed), &mUnidentified024.m_v3Position, &v3Column);
    event.v3Position = v3Column;
    DeliverMegaStrikeMeterSecondEvent(g_pGame, &event);

    if (nParam != 0)
    {
        if (g_pNetworkSession->IsLiveNetworkGame()
            && g_pGame->mPendingMegaStrikeMeter.mCount > 0)
        {
            g_pGame->SendRemainingMegaStrikeMeter();
        }

        if (!gNetworkInputRecording->mPlaybackReady)
        {
            g_pGame->SendMegaStrike(m_pTeam->m_nSide, mUnidentified1E4.m_ID, mUnidentified3BC, mUnidentified3C0);
        }

        mUnidentified478 = 1;
    }

    StopSound(0xBF541A4C, this);

    if (!IsNetworkOrRecordedGame())
    {
        FixedUpdateTask::SetFrameLock(lbl_806DB978);
    }
}

float GetMegaStrikeShotCount(cFielder* pFielder, int nParam)
{
    float fResult = pFielder->fn_800489C4();
    float fMeterMax = pFielder->fn_80048A08();
    float fMeterRange = fMeterMax - pFielder->fn_800489C4();

    float fShooting = pFielder->GetTweaks()->fShooting;
    if (fShooting > 1.0f)
    {
        fShooting = 1.0f;
    }
    else if (fShooting < 0.0f)
    {
        fShooting = 0.0f;
    }

    float fSegmentA
        = InterpolateClamped(lbl_806DB95C, lbl_806DB958, fShooting);
    float fSegmentB
        = InterpolateClamped(lbl_806DB960, lbl_806DB964, fShooting);
    float fSegmentC
        = InterpolateClamped(lbl_806DB968, lbl_806DB96C, fShooting);
    float fSegmentD
        = InterpolateClamped(lbl_806E0C68, lbl_806E0C6C, fShooting);

    float fHalfA = fSegmentA / 2.0f;
    float fHalfB = fSegmentB / 2.0f;
    float fHalfC = fSegmentC / 2.0f;
    float fHalfD = fSegmentD / 2.0f;

    float fTime = pFielder->mUnidentified39C;
    if (nParam != 0)
    {
        fTime = pFielder->mUnidentified3A8;
    }

    if (fTime >= pFielder->mUnidentified3C4 - fHalfA)
    {
        if (fTime < pFielder->mUnidentified3C4 + fHalfA)
        {
            fResult = 0.33f * fMeterRange + fResult;
        }
        else if (fTime < pFielder->mUnidentified3C8 + fHalfB)
        {
            fResult = 0.66f * fMeterRange + fResult;
        }
        else if (fTime < pFielder->mUnidentified3CC + fHalfC)
        {
            fResult = pFielder->fn_80048A08();
        }
        else if (fTime < pFielder->mUnidentified3D0 + fHalfD)
        {
            fResult = pFielder->fn_80048A08();
        }
    }

    float fBias;
    if (fResult < 0.0f)
    {
        fBias = -0.5f;
    }
    else
    {
        fBias = 0.5f;
    }
    return (float)(s32)(fResult + fBias);
}

float GetMegaStrikeAccuracy(cFielder* pFielder, int nParam)
{
    float fNeedle = -9999.0f;
    if (pFielder->mUnidentified3B8)
    {
        float fTime = pFielder->mUnidentified3A0;
        if (nParam != 0)
        {
            fTime = pFielder->mUnidentified3A8;
        }
        float fDelta = fabsf(lbl_806DB93C - fTime);
        float fHalfWidth = fabsf(pFielder->mUnidentified3A4 / 2.0f);
        if (fDelta
            < InterpolateClamped(lbl_806DB940, lbl_806DB944, pFielder->GetTweaks()->fShooting)
                  / 2.0f)
        {
            fDelta = 0.0f;
        }
        fNeedle = InterpolateRangeClamped(
            -1.0f, 1.0f, 2.0f * fHalfWidth, 0.0f, fDelta);
    }
    return fNeedle;
}

void cFielder::InitActionOneTimer(int animID, nlVector3& targetPos,
    float fAdjustEndTime, bool bIsChipShot, s16 nTurnAdjust)
{
    bIsModified = bIsChipShot;
    SetAction(ACTION_ONETIMER);
    mUnidentified368 = fAdjustEndTime;
    SetAnimState(animID, false, fAdjustEndTime * lbl_806DB990, false, false);

    nlVector3 v3MoveAdjustment;
    nlVec3Sub(v3MoveAdjustment, targetPos, mUnidentified024.m_v3Position);
    InitMovementFromAnim(nTurnAdjust, v3MoveAdjustment, fAdjustEndTime * lbl_806DB990, false);

    ClearPowerupAnimState(false);

    bool bUnidentified = IsCaptain();
    unsigned long soundID = 0x1CEC5A02;
    if (bUnidentified)
    {
        soundID = 0xFDE0C69B;
    }
    PlaySound(m_uSoundSlotId, soundID, 0, 0);
}

void cFielder::fn_80049EA0(float fDeltaT)
{
    bool bIsChipShot = false;
    if (bIsModified || IsActionModifierPressed())
    {
        bIsChipShot = true;
    }
    bIsModified = bIsChipShot;

    if (m_pCurrentAnimController->TestTrigger(mUnidentified368))
    {
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionOneTouchPassFromVolley(cPlayer* pPlayer, bool bParam)
{
    bool bIsChipShot = false;
    if (bIsModified || IsActionModifierPressed())
    {
        bIsChipShot = true;
    }
    bIsModified = bIsChipShot;

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_ONETOUCH_PASS_FROM_VOLLEY);

    int LateOneTimerFromVolleyAnims[4] = {
        0x4A,
        0x4D,
        0x4C,
        0x4B,
    };

    s16 facingDelta = GetFacingDeltaToPosition(pPlayer->mUnidentified024.m_v3Position);
    int index = (u16)(facingDelta + 0x2000) >> 14;

    int nAnimID;
    bool bMirror = false;
    if (index == 2)
    {
        bMirror = m_pCurrentAnimController->m_bMirror;
    }

    nAnimID = LateOneTimerFromVolleyAnims[index];

    s16 nTurnAdjust = 0;
    switch (nAnimID)
    {
    case 0x4C:
        nTurnAdjust = -0x8000;
        break;
    case 0x4D:
        nTurnAdjust = -0x4000;
        break;
    case 0x4B:
        nTurnAdjust = 0x4000;
        break;
    }

    s16 facingDelta2 = GetFacingDeltaToPosition(pPlayer->mUnidentified024.m_v3Position);

    SetAnimState(nAnimID, true, 0.0f, false, bMirror);

    InitMovementFromAnim(
        (s16)(nTurnAdjust + facingDelta2), v3Zero, 0.6f, false);

    DoRegularPassing(pPlayer, bParam, true, true, bParam, GetSlowestVolleyPassSpeed(this->GetTweaks()), GetFastestVolleyPassSpeed(this->GetTweaks()));

    mUnidentified371 = true;
}

void cFielder::ActionOneTouchPassFromVolley(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

bool cFielder::DoCalcCanDoPerfectPass(cFielder* pOther, const nlVector3& position)
{
    if (!pOther->ShouldIClearBall())
    {
        float fOtherScore = fn_800DBAB0(pOther);

        bool bResult = false;
        float fOtherOpen = CalcShotScoreValue(pOther, 0, 0);
        float fThisOpen = CalcShotScoreValue(this, 0, 0);

        nlVector2 v2Delta = {
            pOther->mUnidentified024.m_v3Position.x - mUnidentified024.m_v3Position.x,
            pOther->mUnidentified024.m_v3Position.y - mUnidentified024.m_v3Position.y,
        };
        if (nlVec2LengthSquared(v2Delta) > 25.0f
            && fOtherScore > lbl_806DB890 && fOtherOpen > fThisOpen)
        {
            bResult = true;
        }
        return bResult;
    }
    return false;
}

bool cFielder::InitActionPass(
    cPlayer* pPassTarget, bool bVolleyPass, int nParam, bool bIsOneTouchPass)
{
    if (pPassTarget == 0)
    {
        return false;
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_PASS);

    static int PassingAnims[4] = {
        0x25,
        0x28,
        0x27,
        0x26,
    };
    signed short nFacingDelta = GetFacingDeltaToPosition(pPassTarget->mUnidentified024.m_v3Position);
    int index = (u16)(nFacingDelta + 0x2000) >> 14;

    if (mUnidentified024.m_eCharacterClass == HAMMERBROS)
    {
        SetAnimState(PassingAnims[index], false, 0.05f, false, false);
    }
    else
    {
        SetAnimState(PassingAnims[index], true, 0.2f, false, false);
    }

    InitMovementCoast();

    if (bVolleyPass)
    {
        nlVector3 delta;
        delta.y = mUnidentified024.m_v3Position.y - pPassTarget->mUnidentified024.m_v3Position.y;
        delta.x = mUnidentified024.m_v3Position.x - pPassTarget->mUnidentified024.m_v3Position.x;
        delta.z = mUnidentified024.m_v3Position.z - pPassTarget->mUnidentified024.m_v3Position.z;
        float minDistSq = 4.0f;
        minDistSq *= minDistSq;
        float distSq = delta.GetLengthSq3D();

        if (distSq < minDistSq)
        {
            bVolleyPass = false;
        }
    }

    bIsModified = bVolleyPass;
    mUnidentified36C = pPassTarget;
    mUnidentified370 = nParam == 0;
    mUnidentified371 = bIsOneTouchPass;
    return true;
}

void cFielder::ActionPass(float fDeltaT)
{
    if (m_pBall != 0 && m_pCurrentAnimController->TestFrameTrigger(1.0f))
    {
        float fA = GetSlowestVolleyPassSpeed(this->GetTweaks());
        float fB = GetFastestVolleyPassSpeed(this->GetTweaks());
        if (!bIsModified)
        {
            fA = GetSlowestGroundPassSpeed(this->GetTweaks());
            fB = GetFastestGroundPassSpeed(this->GetTweaks());
        }
        DoRegularPassing(mUnidentified36C, bIsModified, mUnidentified370, false, false, fA, fB);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionPostWhistle()
{
    SetAction(ACTION_POST_WHISTLE);
    SetAnimState(0, false, 0.0f, false, false);
    InitMovementNone(0.0f, 0.0f);
    Unknown8(mUnidentified024.m_aActualFacingDirection, false);
    mUnidentified024.m_fActualSpeed = 0.0f;
    SetVelocity(v3Zero);
}

void cFielder::ActionPostWhistle(float fDeltaT)
{
}

void cFielder::InitActionBombReact(const nlVector3& v3BombPosition,
    float fRadius)
{
    EndFrozenOrDazed();
    fn_8009750C();

    if (g_pBall->m_pOwner == this)
    {
        ReleaseBall(0);
        ShootBallDueToContact((unsigned short)(s32)nlRandomf(65535.0f));
    }

    float fDistance
        = nlSqrt(CalculateDistanceSquared(mUnidentified024.m_v3Position, v3BombPosition), true)
        - fRadius;

    if (fDistance > 1.0f && !IsFallenDown())
    {
        InitActionBombHitReact(v3BombPosition);
    }
    else
    {
        if (!IsFallenDown())
        {
            bool bUnidentified = IsCaptain();
            unsigned long soundID = 0x00E606A2;
            if (bUnidentified)
            {
                soundID = 0x3642C41B;
            }
            PlaySound(m_uSoundSlotId, soundID, 0, 0);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_BOMB_REACT);

        s16 facingDelta = GetFacingDeltaToPosition(v3BombPosition);
        u16 absFacingDelta = (u16)abs_s16(facingDelta);

        if (absFacingDelta < 0x4000)
        {
            SetAnimState(0x65, true, 0.2f, false, false);
        }
        else
        {
            SetAnimState(0x66, true, 0.2f, false, false);
        }

        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        mUnidentified024.m_fDesiredSpeed = 0.0f;
    }
}

void cFielder::InitActionBombHitReact(const nlVector3& v3BombPosition)
{
    EndFrozenOrDazed();
    fn_8009750C();

    mUnidentified360 = false;

    if (!IsFallenDown())
    {
        bool bUnidentified = IsCaptain();
        unsigned long soundID = 0x00E606A2;
        if (bUnidentified)
        {
            soundID = 0x3642C41B;
        }
        PlaySound(m_uSoundSlotId, soundID, 0, 0);
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_HIT_REACT);

    u32 index = (((u16)((u16)GetFacingDeltaToPosition(v3BombPosition)) >> 14) & 3);
    SetAnimState(gHitReactAnims[2][index], true, 0.2f, false, false);

    float fDX = mUnidentified024.m_v3Position.x - v3BombPosition.x;
    float fDY = mUnidentified024.m_v3Position.y - v3BombPosition.y;
    float angleRad = nlATan2f(fDY, fDX);
    u16 targetAngle = (u16)(s32)(10430.378f * angleRad);
    SetFacingDirection(targetAngle + gHitReactFacingOffsets[index], true);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    mUnidentified024.m_fDesiredSpeed = 0.0f;
}

void cFielder::InitActionBananaReact(const nlVector3& fDeltaT)
{
    u16 angleDiff
        = (u16)abs_s16(mUnidentified024.m_aActualFacingDirection - mUnidentified024.m_aActualMovementDirection);
    if (angleDiff < 0x4000)
    {
        SetAnimState(0x63, true, 0.2f, false, false);
    }
    else
    {
        SetAnimState(0x64, true, 0.2f, false, false);
    }

    if (g_pBall->m_pOwner == this)
    {
        ReleaseBall(0);
        ShootBallDueToContact(mUnidentified024.m_aActualFacingDirection);
    }

    PlayRumbleAction(2, GetGlobalPad());

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_BANANA_REACT);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    mUnidentified024.m_fDesiredSpeed = 0.0f;
}

int gShellAttackReactAnims[4] = { 0x5F, 0x62, 0x61, 0x60 };

void cFielder::InitActionShellReact(const nlVector3& v3CollisionLocation,
    const nlVector3& v3CollisionVelocity)
{
    if (g_pBall->m_pOwner == this)
    {
        ReleaseBall(0);

        if (nlSqrt(v3CollisionVelocity.GetLengthSq3D(), true) > 0.05f)
        {
            ShootBallDueToContact(v3CollisionVelocity);
        }
        else
        {
            ShootBallDueToContact(mUnidentified024.m_aActualFacingDirection);
        }
    }

    EndDaze();
    fn_8009750C();

    PlayRumbleAction(2, GetGlobalPad());

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_SHELL_REACT);

    s16 facingDelta = GetFacingDeltaToPosition(v3CollisionLocation);
    facingDelta += 0x2000;
    SetAnimState(gShellAttackReactAnims[(u16)facingDelta >> 14], true, 0.2f, false, false);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    mUnidentified024.m_fDesiredSpeed = 0.0f;
}

void cFielder::InitActionRunning()
{
    m_pHeadTrack->m_bTrackOOI = true;

    if (m_eActionState != ACTION_RUNNING)
    {
        mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
        mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
        Unknown8(mUnidentified024.m_aActualFacingDirection, false);
        mUnidentified024.m_fDesiredSpeed = mUnidentified024.m_fActualSpeed;
        mActionRunningVars.bFirstCycleOfTurbo = false;
    }

    SetAction(ACTION_RUNNING);
}

void cFielder::ActionRunning(float dt)
{
    if (m_pBall != 0)
    {
        SetAction(ACTION_RUNNING_WB);
        mActionRunningWBVars.bWaitForAnimToFinish = false;
        mActionRunningWBVars.bCuePitch = false;
        mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
        mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
        bIsModified = false;
        mUnidentified374 = UnidentifiedFielderPair374();
    }
    else
    {
        if (fn_8003E948(this) && mUnidentified3DC)
        {
            if (m_eAnimID != 4)
            {
                SetRunningAnimState(lbl_806E3538[0]);
            }
        }
        else
        {
            asmRunning();
        }

        if (CanPickupBall(g_pBall, false))
        {
            PickupBall(g_pBall);
            SetAction(ACTION_RUNNING_WB);
            mActionRunningWBVars.bWaitForAnimToFinish = false;
            mActionRunningWBVars.bCuePitch = false;
            mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
            mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
            bIsModified = false;
            mUnidentified374 = UnidentifiedFielderPair374();
        }
    }
}

void cFielder::InitActionRunningWB(bool bWaitForAnimToFinish)
{
    SetAction(ACTION_RUNNING_WB);
    mActionRunningWBVars.bWaitForAnimToFinish = bWaitForAnimToFinish;
    mActionRunningWBVars.bCuePitch = false;
    mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
    bIsModified = false;
    mUnidentified374 = UnidentifiedFielderPair374();
}

void cFielder::ActionRunningWB(float dt)
{
    if (m_pBall == 0)
    {
        EndAction();
    }
    else
    {
        nlVector3 v3Target;
        nlPolarToCartesian(
            v3Target.x, v3Target.y, mUnidentified024.m_aActualFacingDirection, 2.0f);
        v3Target.z = 0.0f;
        nlVec3Add(v3Target, v3Target, mUnidentified024.m_v3Position);
        asmRunningWB(dt);
    }
}

void cFielder::fn_8004B148()
{
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x13);

    u16 aDirection = mUnidentified024.m_aActualFacingDirection;
    if (GetGlobalPad() != 0)
    {
        if (m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aDirection = m_pController->GetMovementStickDirection();
        }
    }

    if (mUnidentified024.m_fActualSpeed < GetTurboWBSpeed(this->GetTweaks()))
    {
        float fMinSpeed = GetTurboWBSpeed(this->GetTweaks());
        mUnidentified024.m_fActualSpeed = fMinSpeed;
        mUnidentified024.m_fDesiredSpeed = fMinSpeed;
    }

    nlVector3 v3Velocity;
    nlPolarToCartesian(v3Velocity.x, v3Velocity.y, aDirection, GetSpeedPowerupAdjusted(mUnidentified024.m_fActualSpeed));
    v3Velocity.z = 0.0f;
    SetVelocity(v3Velocity);
    InitMovementCoast();

    if (GetGlobalPad() != 0)
    {
        mUnidentified374.mUnidentified04
            = InterpolateRangeClamped(lbl_806DB8D8, lbl_806DB8DC, 0.2f, lbl_806DB8E0, (float)mUnidentified374.mUnidentified00 * FixedUpdateTask::GetPhysicsUpdateTick());
        GetGlobalPad()->ResetButtonStateTicks(0x17, 1);
    }
    else
    {
        mUnidentified374.mUnidentified04 = lbl_806DB8D8;
    }

    SetAnimState(0x25, true, 0.2f, false, false);
    PlayOwnedSound(0, 0x874F86F2, (XSoundOwner*)g_pBall->mUnidentifiedEC, 0, 0);
}

void cFielder::fn_8004B2E4(float fDeltaT)
{
    if (m_pBall != 0 && m_pCurrentAnimController->TestFrameTrigger(1.0f))
    {
        float fSpeed = mUnidentified024.m_fActualSpeed + mUnidentified374.mUnidentified04;
        if (fSpeed < lbl_806DB8E8)
        {
            fSpeed = lbl_806DB8E8;
        }

        nlVector3 v3BallVelocity;
        nlPolarToCartesian(v3BallVelocity.x, v3BallVelocity.y, mUnidentified024.m_aActualFacingDirection, fSpeed);
        v3BallVelocity.z = InterpolateRangeClamped(lbl_806DB8D0,
            lbl_806DB8D4,
            lbl_806DB8D8,
            lbl_806DB8DC,
            mUnidentified374.mUnidentified04);

        nlVector3 v3Spin;
        nlVector3 v3UpCopy = { 0.0f, 0.0f, 1.0f };
        nlVec3CrossProduct(v3Spin, v3BallVelocity, v3UpCopy);
        float fScale = lbl_806DB8E4 / nlSqrt(v3Spin.GetLengthSq3D(), true);
        nlVec3Scale(v3Spin, fScale);

        ReleaseBall(0);
        g_pBall->SetVelocity(v3BallVelocity, SPINTYPE_PARAMETER, &v3Spin);
        fn_80015C38(g_pBall, 0);
        g_pBall->SetVelocity(v3BallVelocity, SPINTYPE_PARAMETER, &v3Spin);
        SetNoPickUpTime(0.25f);

        if (gbUseTurboCharging != 0)
        {
            float fValue = this->GetTweaks()->fMovementSpeed;
            float fFraction
                = InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fValue);
            float fCharge = Interpolate(lbl_806DB980, lbl_806DB984, fFraction);
            fn_800154FC(g_pBall, fCharge + GetBallChargeValue(g_pBall, 0));
        }

        InitMovementRunning(fn_8002C0AC(this->GetTweaks()),
            fn_8002CF10(this->GetTweaks()),
            fn_8002C180(this->GetTweaks()),
            0.0f);
        InitDesire(
            (eFielderDesireState)0x14, 0.5f, -1.0f, fvNotSet, fvNotSet);
        EmitBallShot(this, BALL_EFFECT_S2S_SUPER_SHOT, 0, 0, 0);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::fn_8004B658()
{
    if (m_pBall == 0)
    {
        InitActionRunning();
    }
    else
    {
        SetAction(ACTION_UNKNOWN_30);
        DoResetShotMeter(0.0f);
        SetWindupWBAnimState();
        InitMovementRunningNoTurn(0.0f, GetShotWindupDecel(this->GetTweaks()));
        mUnidentified024.m_fDesiredSpeed = 0.0f;
        if (mUnidentified024.m_fActualSpeed > this->GetTweaks()->GetRunningSpeed())
        {
            mUnidentified024.m_fActualSpeed = this->GetTweaks()->GetRunningSpeed();
        }

        ShotAtGoalData event;
        event.pShooter = this;
        DeliverWindupShotEvent(g_pGame, &event);
        DeliverWindupPresentationEvent(g_pGame);

        PlaySound(0, 0x900862AC, "Windup", this);

        if (GetBallChargeValue(g_pBall, 0) < 1.0f)
        {
            EmitWindupAtBall("ball_shot_windup_0");
            EmitWindupAtBall("ball_shot_windup_ground_0");
        }
        else if (GetBallChargeValue(g_pBall, 0) < 2.0f)
        {
            EmitWindupAtBall("ball_shot_windup_1");
            EmitWindupAtBall("ball_shot_windup_ground_1");
        }
        else if (GetBallChargeValue(g_pBall, 0) < 3.0f)
        {
            EmitWindupAtBall("ball_shot_windup_2");
            EmitWindupAtBall("ball_shot_windup_ground_2");
        }
        else if (GetBallChargeValue(g_pBall, 0) < 4.0f)
        {
            EmitWindupAtBall("ball_shot_windup_3");
            EmitWindupAtBall("ball_shot_windup_ground_3");
        }
        else
        {
            EmitWindupAtBall("ball_shot_windup_max");
            EmitWindupAtBall("ball_shot_windup_ground_max");
        }
    }
}

bool cFielder::InitActionShot(bool bIsChipShot, bool bIsOneTimer)
{
    if (m_pBall == 0)
    {
        InitActionRunning();
        return false;
    }
    else
    {
        if (bIsOneTimer)
        {
            DoResetShotMeter(0.0f);
            m_pShotMeter->CalcOneTimerValue(this, false);
        }
        else
        {
            m_pShotMeter->ShotReleased(this);
        }

        if (m_pShotMeter->m_eShotMeterState == SHOT_METER_STS_RELEASED)
        {
            g_pBall->m_uGoalType = 2;
            if (GetCharacterClass() == (eCharacterClass)0x10)
            {
                BeginDekeIntangibility();
            }
            else
            {
                if (GetCharacterClass() == (eCharacterClass)0x0D
                    || GetCharacterClass() == (eCharacterClass)0x12
                    || GetCharacterClass() == (eCharacterClass)0x13)
                {
                    fn_8004E438();
                    return true;
                }
                if (GetCharacterClass() == (eCharacterClass)0x0E)
                {
                    if (gNPCManager->mpKoopaShell != 0)
                    {
                        gNPCManager->mpKoopaShell->Activate(this);
                    }
                }
                else if (GetCharacterClass() == (eCharacterClass)0x0C)
                {
                    if (gNPCManager->mpBirdoEgg != 0)
                    {
                        gNPCManager->mpBirdoEgg->Show(this);
                    }
                }
            }
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_UNKNOWN_15);
        bIsModified = bIsChipShot;

        switch (m_eAnimID)
        {
        case 0x52:
            SetAnimState(0x54, true, 0.2f, false, false);
            break;
        case 0x53:
            SetAnimState(0x55, true, 0.2f, false, false);
            break;
        default:
        {
            s16 facingDelta = GetFacingDeltaToPosition(
                m_pTeam->GetOtherNet()->m_v3NetLocation);
            if (facingDelta < 0)
            {
                SetAnimState(0x55, true, 0.2f, false, false);
            }
            else
            {
                SetAnimState(0x54, true, 0.2f, false, false);
            }
            break;
        }
        }

        float fSpeed = mUnidentified024.m_fActualSpeed;
        InitMovementRunningNoTurn(0.0f, fSpeed / GetShootingWindupTime(this->GetTweaks()));
        mUnidentified024.m_fDesiredSpeed = 0.0f;

        nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
        nlVector3 v3Delta;
        nlVec3Sub(v3Delta, v3NetPos, mUnidentified024.m_v3Position);
        Unknown8(nlVector3ToAngle(v3Delta), false);
        return true;
    }
}

void cFielder::fn_8004BB80(float fDeltaT)
{
    PlayRumbleAction(1, GetGlobalPad());

    nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
    nlVector3 v3Delta;
    nlVec3Sub(v3Delta, v3NetPos, mUnidentified024.m_v3Position);
    mUnidentified024.m_aDesiredFacingDirection = nlVector3ToAngle(v3Delta);

    SetFacingDirection(
        SeekDirection(mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_aDesiredFacingDirection, GetShotWindupTurnSpeed(this->GetTweaks()), GetShotWindupTurnFalloff(this->GetTweaks()), fDeltaT),
        false);

    float fChargeTime = m_pShotMeter->m_fTime;
    fChargeTime = GetShootingWindupTime(this->GetTweaks()) - fChargeTime;
    if (fChargeTime < 0.01f)
    {
        fChargeTime = 0.01f;
    }

    InitMovementRunningNoTurn(0.0f, mUnidentified024.m_fActualSpeed / fChargeTime);
    mUnidentified024.m_fDesiredSpeed = 0.0f;

    if (fChargeTime < 0.5f && lbl_806E0C74 == 0)
    {
        if (AIsgn(mUnidentified024.m_v3Position.x)
            == AIsgn(m_pTeam->GetOtherNet()->m_v3NetLocation.x))
        {
            nlVector3 v3Dir;
            nlVec3Sub(v3Dir, mUnidentified024.m_v3Position, m_pTeam->GetOtherNet()->m_v3NetLocation);
            nlVec3Normalize(v3Dir, v3Dir);
            nlVec3ScaleAdd(v3Dir, 2.0f, v3Dir, mUnidentified024.m_v3Position);

            lbl_806E0C74 = new (nlMalloc(
                sizeof(AvoidablePolygon), 8, false))
                AvoidablePolygon(2, v3Dir, m_pTeam->GetOtherNet()->m_v3NetLocation, 3.5f);
            lbl_806E0C74->mOwner = this;
        }
    }

    if (!IsStuck())
    {
        m_pShotMeter->Update(fDeltaT);
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10 && m_pBall != 0)
    {
        ShotMeter* pShotMeter = m_pShotMeter;
        float fWindow = lbl_806E3538[0];
        float fReleaseTime = pShotMeter->m_fTotalDuration;
        float fTime = pShotMeter->m_fTime;
        if (fTime > fReleaseTime - fWindow)
        {
            float fTimeLeft = fReleaseTime - fTime;
            mUnidentified178 = InterpolateRangeClamped(
                mUnidentified178, 0.0f, fWindow, 0.0f, fTimeLeft);
        }
    }

    if (m_pBall == 0)
    {
        EndAction();
    }
}

void cFielder::fn_8004BF58(eFielderActionState eNewAction)
{
    if (lbl_806E0C74 != 0)
    {
        delete lbl_806E0C74;
        lbl_806E0C74 = 0;
    }

    KillWindups();
    StopSound(0x900862AC, this);

    if (eNewAction != ACTION_UNKNOWN_15)
    {
        m_pShotMeter->Abort();
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10
            && eNewAction != (eFielderActionState)1)
        {
            mUnidentified178 = 1.0f;
        }
    }
    else
    {
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10
            && m_pShotMeter->m_eShotMeterState != SHOT_METER_STS_RELEASED)
        {
            mUnidentified178 = 1.0f;
        }
    }

    DeliverWindupPresentationEndEvent(g_pGame);
}

void cFielder::fn_8004C02C(float fDeltaT)
{
    if (m_pCurrentAnimController->m_fTime
        <= 1.825f / (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys)
    {
        nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
        nlVector3 v3Delta;
        nlVec3Sub(v3Delta, v3NetPos, mUnidentified024.m_v3Position);
        mUnidentified024.m_aDesiredFacingDirection = nlVector3ToAngle(v3Delta);

        SetFacingDirection(
            SeekDirection(mUnidentified024.m_aActualFacingDirection,
                mUnidentified024.m_aDesiredFacingDirection,
                200000.0f,
                GetShotWindupTurnFalloff(this->GetTweaks()),
                fDeltaT),
            false);
    }

    if (m_pBall != 0
        && m_pCurrentAnimController->TestFrameTrigger(1.825f))
    {
        if (!ShouldIClearBall())
        {
            DoRegularShooting(false);

            if (g_pBall->meBallState == 8)
            {
                if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x11)
                {
                    EmitBallShot(this, BALL_EFFECT_REGULAR_SHOT, 0, 0, 1);
                }
                else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x0F)
                {
                    EmitBallShot(this, BALL_EFFECT_PERFECT_PASS, 0, 0, 1);
                }
                else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10)
                {
                    EmitBallShot(this, BALL_EFFECT_ONETIMER_SHOT, 0, 0, 1);
                }
            }
            else
            {
                if (mUnidentified1E4.m_tBallPossessionTimer.GetSeconds() < lbl_806E3538[0]
                    || m_pShotMeter->GetSpeedValue() < lbl_806E3538[0])
                {
                    EmitBallShot(this, BALL_EFFECT_PERFECT_SHOT, 0, 0, 1);
                }
                else
                {
                    EmitBallShot(this, BALL_EFFECT_S2S_SHOT, 0, 0, 1);
                }
            }
        }
        else
        {
            DoClearBall();
            EmitBallShot(this, BALL_EFFECT_S2S_SHOT, 0, 0, 0);
        }

        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    if (ShouldStartCrossBlend(4))
    {
        m_pHeadTrack->m_bTrackOOI = false;
        EndAction();
    }
}

void cFielder::InitActionSlideAttack(
    cFielder* pTarget, float fTime, int nParam)
{
    if (!IsStuck())
    {
        nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;

        SetAction(ACTION_SLIDE_ATTACK);
        SetAnimState(0x5E, true, 0.2f, false, false);
        InitMovementRunning(0.0f, 0.0f, fn_8002C180(this->GetTweaks()), fn_8002CF24(this->GetTweaks()));
        mUnidentified1E4.m_tSlideAttackTimer.SetSeconds(GetSlideTime(this->GetTweaks()));

        mUnidentified388 = 0;
        bAttackSucceeded = false;
        mUnidentified38D = false;

        nlVector3 v3TargetPosition;
        nlVector3 v3TargetVelocity;
        nlVector3 v3Target;
        nlVector3 v3BallDelta;
        if (fTime < 0.0f)
        {
            fTime = CalcSlideAttackBallIntercept(v3Target, nParam);
        }
        else
        {
            if (pTarget != 0)
            {
                v3TargetPosition = pTarget->mUnidentified024.m_v3Position;
                v3TargetVelocity = pTarget->mUnidentified024.m_v3Velocity;
            }
            else
            {
                v3TargetPosition = g_pBall->m_v3Position;
                v3TargetVelocity = g_pBall->m_v3Velocity;
            }

            v3Target.x = v3TargetPosition.x + v3TargetVelocity.x * fTime;
            v3Target.y = v3TargetPosition.y + v3TargetVelocity.y * fTime;
            v3Target.z = 0.0f;
        }

        float fSpeed = GetSlideAttackSpeed(nParam);
        if (fn_8003E70C())
        {
            nlPolarToCartesian(
                v3Velocity.x, v3Velocity.y, mUnidentified024.m_aActualFacingDirection, fSpeed);
        }
        else
        {
            v3BallDelta.y = mUnidentified024.m_v3Position.y - g_pBall->m_v3Position.y;
            v3BallDelta.x = mUnidentified024.m_v3Position.x - g_pBall->m_v3Position.x;
            v3BallDelta.z = mUnidentified024.m_v3Position.z - g_pBall->m_v3Position.z;
            float fAdjust = mUnidentified024.m_fPlayerScale;
            float fBallDistance
                = nlSqrt(v3BallDelta.GetLengthSq3D(), true);

            if (fBallDistance
                < lbl_806E3538[0] + fn_8002BFA8(this->GetTweaks(), fAdjust))
            {
                float fLengthSq = v3Velocity.GetLengthSq3D();
                if (fLengthSq > 0.0001f)
                {
                    float fRecipLength = nlRecipSqrt(fLengthSq, true);
                    nlVec3Scale(v3Velocity, fRecipLength);
                    nlVec3Scale(v3Velocity, fSpeed);
                }
                else
                {
                    nlPolarToCartesian(v3Velocity.x, v3Velocity.y, mUnidentified024.m_aActualFacingDirection, fSpeed);
                }
            }
            else
            {
                if (0.0f == fTime)
                {
                    nlPolarToCartesian(v3Velocity.x, v3Velocity.y, mUnidentified024.m_aActualFacingDirection, fSpeed);
                }
                else
                {
                    v3Velocity.x = v3Target.x - mUnidentified024.m_v3Position.x;
                    v3Velocity.y = v3Target.y - mUnidentified024.m_v3Position.y;
                    v3Velocity.z = 0.0f;
                    float fLengthSq = v3Velocity.GetLengthSq3D();
                    if (fLengthSq > 0.0001f)
                    {
                        float fRecipLength = nlRecipSqrt(fLengthSq, true);
                        nlVec3Scale(v3Velocity, fRecipLength);
                        nlVec3Scale(v3Velocity, fSpeed);
                    }
                    else
                    {
                        nlPolarToCartesian(v3Velocity.x, v3Velocity.y, mUnidentified024.m_aActualFacingDirection, fSpeed);
                    }
                }

                nlPolar polar;
                nlCartesianToPolar(polar, v3Velocity.x, v3Velocity.y);
                Unknown8(polar.a, false);
                SetFacingDirection(mUnidentified024.m_aDesiredFacingDirection, true);
            }
        }

        v3Velocity.z = 0.0f;
        SetVelocity(v3Velocity);

        nlPolar polar;
        nlCartesianToPolar(polar, v3Velocity.x, v3Velocity.y);
        float fFinalSpeed = polar.r;
        mUnidentified024.m_fDesiredSpeed = fFinalSpeed;
        mUnidentified024.m_fActualSpeed = fFinalSpeed;

        StopSound(0x2AE03886, this);
        PlaySound(0, 0x2AE03886, "SlideAttack", this);

        PlayerAttackData* pNode = g_PlayerAttackDataPool.Allocate();
        pNode->pAttacker = this;
        bool bHasPad = GetGlobalPad() != 0;
        pNode->nAttackerPadID = bHasPad ? GetGlobalPad()->GetPadID() : -1;
        pNode->pTarget = 0;
        pNode->mUnidentified10 = true;
        QueueAttackAttemptEvent(g_pGame, pNode);
    }
}

void cFielder::fn_8004C88C(float fDeltaT)
{
    if (!bAttackSucceeded && mUnidentified388 == 0 && fn_8003E6FC())
    {
        float fCurrSpeed;
        nlVector3 v3NewVelocity;
        nlVector2 v2Delta;
        v2Delta.x = g_pBall->m_v3Position.x - mUnidentified024.m_v3Position.x;
        v2Delta.y = g_pBall->m_v3Position.y - mUnidentified024.m_v3Position.y;
        nlVec2Length(v2Delta);

        nlVector2 v2Direction;
        nlVec2Scale(v2Direction, v2Delta, 1.0f / nlVec2Length(v2Delta));
        v2Delta.y = 8.5f * v2Direction.y;
        v2Delta.x = 8.5f * v2Direction.x;

        fCurrSpeed = nlSqrt(nlGetLengthSquared1D(mUnidentified024.m_v3Velocity.x)
                                + nlGetLengthSquared1D(mUnidentified024.m_v3Velocity.y),
            true);

        nlVector2 v2NewVelocity;
        v2NewVelocity.x = v2Delta.x + mUnidentified024.m_v3Velocity.x;
        v2NewVelocity.y = v2Delta.y + mUnidentified024.m_v3Velocity.y;
        nlVector2 v2NormalizedVelocity;
        nlVec2Scale(v2NormalizedVelocity, v2NewVelocity, 1.0f / nlVec2Length(v2NewVelocity));

        if (v2NormalizedVelocity.x * v2Direction.x
                + v2NormalizedVelocity.y * v2Direction.y
            >= 0.0f)
        {
            v2NewVelocity.y = fCurrSpeed * v2NormalizedVelocity.y;
            v2NewVelocity.x = fCurrSpeed * v2NormalizedVelocity.x;
            nlVec3Set(v3NewVelocity,
                v2NewVelocity.x,
                v2NewVelocity.y,
                0.0f);

            nlPolar polar;
            nlCartesianToPolar(polar, v3NewVelocity.x, v3NewVelocity.y);
            Unknown8(polar.a, false);
            SetFacingDirection(polar.a, true);
            SetVelocity(v3NewVelocity);
        }
    }

    if (g_pBall->m_pOwner == 0 && !fn_80014E20(g_pBall)
        && g_pBall->m_tShotTimer.m_uPackedTime == 0
        && !(g_pBall->m_tNoPickupTimer.m_uPackedTime != 0
             && g_pBall->m_pPrevOwner != 0
             && g_pBall->m_pPrevOwner->m_eClassType == GOALIE))
    {
        bool bCanPickup = CanPickupBall(g_pBall, true);
        bool bTouched = false;
        if (bAttackSucceeded || bCanPickup)
        {
            bTouched = true;
        }
        bAttackSucceeded = bTouched;

        if (bTouched && bCanPickup)
        {
            if (g_pBall->UnidentifiedHasPassTarget()
                && g_pBall->m_pPrevOwner != 0
                && g_pBall->m_pPrevOwner->m_eClassType == FIELDER
                && !IsOnSameTeam(g_pBall->m_pPrevOwner))
            {
                PlayerAttackData* pNode = g_PlayerAttackDataPool.Allocate();
                pNode->pAttacker = this;
                bool bHasPad = GetGlobalPad() != 0;
                pNode->nAttackerPadID
                    = bHasPad ? GetGlobalPad()->GetPadID() : -1;
                pNode->pTarget = 0;
                pNode->mUnidentified10 = true;
                fn_8005ED64(g_pGame, pNode);

                if (m_pBall != 0)
                {
                    if (GetStadiumUnknown0x10(
                            GameInfoManager::Instance()->GetStadium()))
                    {
                        unsigned long soundID = 0x0DCA472D;
                        if (m_pTeam->m_nSide == 0)
                        {
                            soundID = 0x902DA0E4;
                        }
                        PlayCrowdReaction(soundID);
                    }
                }
            }

            PickupBall(g_pBall);
        }
    }

    if (bAttackSucceeded)
    {
        if (GetGlobalPad() != 0 && !mUnidentified38D)
        {
            mUnidentified38D = IsDekePadPressed() == 0;
        }

        if (m_pBall != 0)
        {
            TestButtonsToQueueActions(fDeltaT);
        }
    }

    switch (mUnidentified388)
    {
    case 0:
    {
        nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;
        float fSpeed = GetActualSpeed();
        nlPolarToCartesian(v3Velocity.x, v3Velocity.y, GetActualFacing(), fSpeed);
        v3Velocity.z = 0.0f;
        SetVelocity(v3Velocity);

        bool bUnidentified = false;
        if (GetGlobalPad() != 0 && mUnidentified38D && m_pBall != 0)
        {
            if (!IsDekePadPressed())
            {
                bUnidentified = true;
            }
        }

        if (mUnidentified1E4.m_tSlideAttackTimer.m_uPackedTime == 0 || bUnidentified)
        {
            PlayRumbleAction(1, GetGlobalPad());
            mUnidentified388 = 1;
            mUnidentified1E4.m_tSlideAttackTimer.SetSeconds(GetSlideDecelTime(this->GetTweaks()));
        }
        break;
    }
    case 1:
    {
        float fDecelTime = mUnidentified1E4.m_tSlideAttackTimer.GetSeconds();
        if (fDecelTime < 0.01f)
        {
            fDecelTime = 0.01f;
        }

        float fTargetSpeed = 1.0f;
        if (m_pBall != 0)
        {
            fTargetSpeed = fn_8002CE14(this->GetTweaks());
        }

        InitMovementRunningNoTurn(
            0.0f, (mUnidentified024.m_fActualSpeed - fTargetSpeed) / fDecelTime);
        mUnidentified024.m_fDesiredSpeed = fTargetSpeed;

        if (mUnidentified1E4.m_tSlideAttackTimer.m_uPackedTime == 0)
        {
            fn_8004D238();
        }
        break;
    }
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        fn_8004D238();
    }
}

void cFielder::fn_8004D238()
{
    if (m_pBall != 0)
    {
        if (!TestQueuedActions())
        {
            EndDesire();
            EndAction();
        }
    }
    else
    {
        EndDesire();
        EndAction();
    }

    mUnidentified1E4.m_eLastPadAction = 0x32;
}

void cFielder::fn_8004D480(const nlVector3& v3CollisionVelocity)
{
    EndFrozenOrDazed();
    fn_8009750C();

    if (IsFallenDown() && m_eActionState == (eFielderActionState)0x1C)
    {
        SetAnimState(0x56, true, 0.2f, false, false);
        cPN_SAnimController* pController = m_pCurrentAnimController;
        pController->m_fPrevTime = pController->m_fTime;
        pController->m_fTime = 0.05f;
        pController->m_bLooped = 0;
    }
    else
    {
        if (g_pBall->m_pOwner == this)
        {
            ReleaseBall(0);
            ShootBallDueToContact(v3CollisionVelocity);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)0x1C);
        SetAnimState(0x56, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);

        nlPolar polar;
        nlCartesianToPolar(
            polar, v3CollisionVelocity.x, v3CollisionVelocity.y);
        SetFacingDirection(polar.a, true);
        mUnidentified024.m_aActualMovementDirection = polar.a;
        mUnidentified024.m_fDesiredSpeed = 0.0f;

        PlayRumbleAction(2, GetGlobalPad());

        if (fn_8003E8A0(this))
        {
            EndBowserSmoke(this);
        }
    }
}

void cFielder::InitActionSlideAttackReact(cPlayer* pAttacker, bool bSkipEvent)
{
    if (!IsFallenDown())
    {
        EndFrozenOrDazed();

        bool bHadBall = false;
        if (m_pBall != 0)
        {
            bHadBall = true;
            ReleaseBall(0);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_SLIDE_ATTACK_REACT);

        s16 facingDelta;
        if (pAttacker != this)
        {
            facingDelta = GetFacingDeltaToPosition(pAttacker->mUnidentified024.m_v3Position);
        }
        else
        {
            facingDelta = (s16)(int)mUnidentified024.m_aActualFacingDirection;
        }

        static int SlideAttackReactAnims[4] = {
            0x5F,
            0x62,
            0x61,
            0x60,
        };

        SetAnimState(SlideAttackReactAnims[(u16)(facingDelta + 0x2000) >> 14],
            true,
            0.2f,
            false,
            false);

        InitMovementFromAnim(0, v3Zero, 1.0f, false);

        PlayRumbleAction(2, GetGlobalPad());

        if (pAttacker->m_eClassType == FIELDER && !bSkipEvent
            && pAttacker != this && bHadBall)
        {
            PlayerAttackData* pNode = g_PlayerAttackDataPool.Allocate();
            pNode->pAttacker = pAttacker;
            bool bHasPad = pAttacker->GetGlobalPad() != 0;
            pNode->nAttackerPadID
                = bHasPad ? pAttacker->GetGlobalPad()->GetPadID() : -1;
            pNode->pTarget = 0;
            pNode->mUnidentified10 = true;
            fn_8005ED64(g_pGame, pNode);

            if (pAttacker->m_pBall != 0
                && GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
            {
                unsigned long soundID = 0xDCA472D;
                if (pAttacker->m_pTeam->m_nSide == 0)
                {
                    soundID = 0x902DA0E4;
                }
                PlayCrowdReaction(soundID);
            }
        }

        PlayRumbleAction(1, pAttacker->GetGlobalPad());
        PlaySound(0, 0x57208DA, 0, 0);
        mUnidentified024.m_fDesiredSpeed = 0.0f;
    }
}

void cFielder::ActionSlideAttackReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionBombReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionSTSHitReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionShellReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionBananaReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionReceivePass(int animID, nlVector3& v3TargetPos,
    short sDirectionSeekSpeed, float fAdjustEndTime)
{
    mUnidentified368 = fAdjustEndTime;
    SetAction(ACTION_RECEIVE_PASS);
    SetAnimState(animID, false, fAdjustEndTime * lbl_806DB990, false, false);

    nlVector3 v3Direction;
    nlVec3Set(v3Direction,
        v3TargetPos.x - mUnidentified024.m_v3Position.x,
        v3TargetPos.y - mUnidentified024.m_v3Position.y,
        v3TargetPos.z - mUnidentified024.m_v3Position.z);

    InitMovementFromAnim(sDirectionSeekSpeed, v3Direction, fAdjustEndTime * lbl_806DB990, false);
    ClearPowerupAnimState(false);
}

void cFielder::ActionSquishReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(0x52))
    {
        EndAction();
    }
}

void cFielder::InitActionWait()
{
    SetAction(ACTION_WAIT);
    SetAnimState(0, true, 0.2f, false, false);
    InitMovementNone(0.0f, 0.0f);
    Unknown8(mUnidentified024.m_aActualFacingDirection, false);
}

void cFielder::ActionWait(float fDeltaT)
{
}

void cFielder::fn_8004E11C(float fParam)
{
    Unknown8(mUnidentified024.m_aActualFacingDirection, false);
    fn_80097358(this, fParam);
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_UNKNOWN_31);
    SetAnimState(0x80, false, 0.3f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.5f;
    mUnidentified3D8 = 0;
    mUnidentified3DA = 0;

    bool bHasPad = GetGlobalPad() != 0;
    if (bHasPad == true)
    {
        SwapController(false);
    }
}

void cFielder::fn_8004E228()
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0)
    {
        mUnidentified3D8 = 0;
        mUnidentified3DA = 0;
        EndAction();
    }
    else
    {
        int nTarget = 0x32;
        int nLimit = lbl_806DB8AC;
        if (mUnidentified3DA < -nLimit)
        {
            nTarget = 0x4B;
        }
        else if (mUnidentified3DA > nLimit)
        {
            nTarget = 0x19;
        }
        else if (mUnidentified3DA * mUnidentified3D8 > 0)
        {
            int nSign = -0x19;
            if (mUnidentified3DA > 0)
            {
                nSign = 0x19;
            }
            nTarget = nSign + 0x32;
        }

        s16 nRandom = (s16)nlRandom(0x64);
        mUnidentified3D8 = mUnidentified3D8 + (nTarget - nRandom) / 15;
        if (mUnidentified3D8 > lbl_806DB8B0)
        {
            mUnidentified3D8 = lbl_806DB8B0;
        }
        else if (mUnidentified3D8 < -lbl_806DB8B0)
        {
            mUnidentified3D8 = -lbl_806DB8B0;
        }
        mUnidentified3DA = mUnidentified3DA + mUnidentified3D8;
        SetFacingDirection(mUnidentified3DA + mUnidentified024.m_aActualFacingDirection, true);
    }
}

void cFielder::fn_8004E438()
{
    EndShrink();
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_UNKNOWN_32);

    nlVector3 v3Delta;
    nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
    nlVec3Sub(v3Delta, v3NetPos, mUnidentified024.m_v3Position);
    Unknown8(nlVector3ToAngle(v3Delta), false);
    SetFacingDirection(mUnidentified024.m_aDesiredFacingDirection, true);

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x0D)
    {
        SetAnimState(0x81, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 0.0f, false);
        PlayOwnedSound(m_uSoundSlotId, 0x3D267BDF, (XSoundOwner*)g_pBall->mUnidentifiedEC, "Skillshot", this);
    }
    else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x12)
    {
        SetAnimState(0x81, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 0.0f, false);
        PlayOwnedSound(m_uSoundSlotId, 0x3D267BDF, (XSoundOwner*)g_pBall->mUnidentifiedEC, "Skillshot", this);
    }
    else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x13)
    {
        SetAnimState(0x81, true, 0.2f, false, false);
        m_pBulletBill->Show(this);
        InitMovementFromAnim(0, v3Zero, 0.0f, false);
        EmitShyGuyBulletStart(this);
        PlaySound(m_uSoundSlotId, 0x1D6C8D56, 0, 0);
    }

    bool bUnidentified = g_pGame->IsGameplayOrOvertime();
    if (bUnidentified)
    {
        StatsTracker::Instance()->TrackStat(
            (ePlayerStats)4, m_pTeam->m_nSide, mUnidentified1E4.m_ID, 1, 0, 0, 0);
    }
}

void cFielder::ReleaseHammerProjectile()
{
    if (m_eActionState == ACTION_UNKNOWN_32)
    {
        for (int i = 0; i < 1; i++)
        {
            HammerObject* pProjectile = gNPCManager->GetHammer(-1);
            if (pProjectile != 0)
            {
                pProjectile->Activate(this);
                pProjectile->SetPosition(GetJointPosition(m_nLeftHandJointIndex));

                float fDistance = lbl_806DB89C
                                + lbl_806DB8A0
                                      * (float)(int)(pProjectile->mIndex % 5);

                nlVector3 v3Target;
                nlVec3Set(v3Target, m_m4WorldMatrix.m11, m_m4WorldMatrix.m12, m_m4WorldMatrix.m13);
                nlVec3ScaleAdd(v3Target, fDistance, v3Target, mUnidentified024.m_v3Position);

                nlVector3 v3Delta;
                nlVec3Sub(v3Delta, v3Target, mUnidentified024.m_v3Position);

                float fGravity = pProjectile->mPhysics->m_gravity;
                float fStartHeight = pProjectile->GetPosition()->z;

                float fVerticalSpeed
                    = lbl_806DB8A4
                    * nlSqrt(v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y,
                        true);

                int nNumRoots;
                float fX1, fX2;
                SolveQuadratic(
                    0.5f * fGravity, fVerticalSpeed, fStartHeight, nNumRoots, fX1, fX2);

                float fFlightTime;
                if (fX1 > 0.0f)
                {
                    fFlightTime = fX1;
                }
                else if (fX2 > 0.0f)
                {
                    fFlightTime = fX2;
                }
                float fInverseFlightTime = 1.0f / fFlightTime;
                nlVec3Scale(v3Delta, fInverseFlightTime);
                v3Delta.z = fVerticalSpeed;
                pProjectile->SetVelocity(v3Delta);
            }
        }
    }
}

void cFielder::fn_8004E8B8()
{
    if (m_eActionState == ACTION_UNKNOWN_32)
    {
        muInvincibleStatus |= 1;
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x13)
        {
            EmitShyGuyBulletShoot(this);
            PlayOwnedSound(m_uSoundSlotId, 0x3D267BDF, (XSoundOwner*)g_pBall->mUnidentifiedEC, "Skillshot", this);
        }
    }
}

void cFielder::fn_8004E92C()
{
    bool bUnidentified = g_pGame->IsGameplayOrOvertime();
    if (bUnidentified)
    {
        if (m_pBall != 0 && m_eActionState == ACTION_UNKNOWN_32)
        {
            DoResetShotMeter(0.0f);
            m_pShotMeter->CalcOneTimerValue(this, false);

            if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x12)
            {
                g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;

                float fBallX = (float)fabs(g_pBall->m_v3Position.x);
                float fRadius = g_pBall->m_pPhysicsBall->GetRadius();
                float fGoalLineX = cField::GetGoalLineX(1U);
                if (fBallX < fGoalLineX + fRadius)
                {
                    DoRegularShooting(false);
                }
                else
                {
                    ReleaseBall(0);
                }
                EmitBallShot(this, BALL_EFFECT_S2S_SHOT, 0, 0, 0);
            }
            else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x0D)
            {
                DoRegularShooting(false);
                EmitBallShot(this, BALL_EFFECT_S2S_SHOT, 0, 0, 0);
            }
        }
    }
    else
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
        }
    }
}

void cFielder::fn_8004EA9C()
{
    if (m_eActionState == ACTION_UNKNOWN_32)
    {
        ClearInvincibility(0);
    }
}

void cFielder::fn_8004EAB4(float fDeltaT)
{
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x13)
    {
        m_pBulletBill->position = GetJointPosition(m_nBallJointIndex);
        m_pBulletBill->velocity = mUnidentified024.m_v3Velocity;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::fn_8004EC40()
{
    if (!mbTangible)
    {
        RestoreTangibility(false);
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x12)
    {
        ClearInvincibility(0);
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
    }
    else if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x13)
    {
        ClearInvincibility(0);

        if (m_pBulletBill->active)
        {
            CollisionBulletBillData* pNode = 0;
            g_CollisionBulletBillDataPool.Allocate(pNode);
            pNode->player = this;
            pNode->bulletBill = m_pBulletBill;
            QueueExplosionBulletBill(pNode);
        }

        SetVelocity(v3Zero);
        mUnidentified024.m_v3PrevVelocity = v3Zero;
    }

    StopSound(0x3D267BDF, this);
}

void cFielder::fn_8004ED64()
{
    EndShrink();
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x21);
    InitMovementCoast();

    mUnidentified410.mUnidentified00 = mUnidentified024.m_v3Position;
    mUnidentified410.mUnidentified0C = true;

    g_pBall->m_tNoPickupTimer.SetSeconds(0.5f);
    SetNoPickUpTime(0.5f);

    Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
    pGoalie->m_pPhysicsCharacter->m_CanCollideWithBall = 0;
    SetTangible(0, 1);

    g_pBall->m_pPhysicsBall->mbCanCollidePlayer = false;
    g_pBall->m_pPhysicsBall->mbCanCollideGoalie = false;
}

void cFielder::fn_8004EE48(float fDeltaT)
{
    if (mUnidentified410.mUnidentified0C)
    {
        float fAbsX;
        float fDistSq;
        float fGoalLineX;

        fGoalLineX = cField::GetGoalLineX(1U) - 0.5f;

        nlVector3 v3BallPos = g_pBall->m_v3Position;
        fAbsX = fabsf(v3BallPos.x);

        fDistSq = nlVec3DistanceSquared2D(
            mUnidentified410.mUnidentified00, v3BallPos);

        if (fn_800167A8(g_pBall)
            && !(fDistSq > lbl_806DB8A8 * lbl_806DB8A8)
            && !(fAbsX > fGoalLineX))
        {
            return;
        }

        mUnidentified410.mUnidentified0C = false;
        SetAnimState(0x81, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        EmitBallShot(this, BALL_EFFECT_S2S_SHOT, 0, 0, 0);
        SetNoPickUpTime(0.2f);

        Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
        pGoalie->m_pPhysicsCharacter->m_CanCollideWithBall = 1;

        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
    }
    else
    {
        nlVector3 v3Position = mUnidentified024.m_v3Position;
        float fAnimTime = m_pCurrentAnimController->m_fTime;
        v3Position.z
            *= 1.0f - fAnimTime;
        if (v3Position.z < 0.0f)
        {
            v3Position.z = 0.0f;
        }
        SetPosition(v3Position);

        if (m_pCurrentAnimController->TestTrigger(0.05f))
        {
            RestoreTangibility(false);
        }

        if (ShouldStartCrossBlend(0))
        {
            if (0.0f != v3Position.z)
            {
                v3Position.z = 0.0f;
                SetPosition(v3Position);
            }
            EndAction();
        }
    }
}

void cFielder::fn_8004F180()
{
    nlVector3 v3Position = mUnidentified024.m_v3Position;
    v3Position.z = 0.0f;
    SetPosition(v3Position);

    mUnidentified17C = true;
    RestoreTangibility(false);
    ClearInvincibility(0);
    mUnidentified424 = false;
}

void cFielder::fn_8004F204()
{
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_UNKNOWN_34);
    SetAnimState(0, false, 0.0f, false, false);
    InitMovementNone(0.0f, 0.0f);

    nlVector3 v3Position = mUnidentified024.m_v3Position;
    v3Position.z = -2.0f;
    SetPosition(v3Position);
    SetVelocity(v3Zero);

    SetTangible(1, 0);

    mUnidentified17C = false;
    mUnidentified178 = 1.0f;
    SetNoPickUpTime(1.0f);
    mUnidentified424 = false;
}

void cFielder::fn_8004F2FC(float fDeltaT)
{
    if (mUnidentified424)
    {
        switch (m_eAnimID)
        {
        case 0x7C:
        {
            if (mUnidentified024.m_v3Velocity.z < 0.0f && mUnidentified024.m_v3Position.z <= lbl_806E3538[0])
            {
                nlVector3 v3Position = mUnidentified024.m_v3Position;
                v3Position.z = 0.0f;
                SetPosition(v3Position);
                SetVelocity(v3Zero);
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
            }
            else
            {
                nlVector3 v3Velocity = mUnidentified024.m_v3Velocity;
                float fDamping = 1.0f - 0.5f * fDeltaT;
                v3Velocity.x *= fDamping;
                v3Velocity.y *= fDamping;
                v3Velocity.z = -30.0f * fDeltaT + v3Velocity.z;
                SetVelocity(v3Velocity);
                mUnidentified024.m_v3Position.z += v3Velocity.z * fDeltaT;
            }
            break;
        }
        case 0x50:
        case 0x7D:
            if (ShouldStartCrossBlend(4))
            {
                SetTangible(1, 0);
                EndAction();
            }
            break;
        }
    }
}

static TweakBoolBinding s_UseTurboChargingTweak(
    "gbUseTurboCharging", "Game/Gameplay/Charging/Turbo",
    &gbUseTurboCharging, true);

u16 g_IdleTurnCompletionDelta = (u16)DegreesToAngle(10.0f);

AvoidablePolygon* lbl_806E0C74;
