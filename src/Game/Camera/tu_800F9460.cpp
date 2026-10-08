#include "NL/nlDLListContainer.inl"
#include "Game/Camera/tu_800F9460.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/Camera/GameplayCam.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/MathHelpers.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkInputRecording.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlTask.h"
#include "Game/UnidentifiedStaticStorage.h"
#include <math.h>

// Instantiate the Function classes RegisterEventListeners uses, in its order,
// before BindMember is declared. MWCC emits late template copies grouped by
// declaration point in reverse, which gives retail's tail: the BindMember
// copies, then the Function constructors from the last event to the first.
typedef char UnidentifiedFunctionSize0[sizeof(Function<GoalScoredData*>)];
typedef char UnidentifiedFunctionSize1[sizeof(Function<FnVoidVoid>)];
typedef char UnidentifiedFunctionSize2[sizeof(Function<MegaStrikeMeterData*>)];
typedef char UnidentifiedFunctionSize3[sizeof(Function<GoalieSaveData*>)];
typedef char UnidentifiedFunctionSize4[sizeof(Function<CollisionThwompPlayerData*>)];
typedef char UnidentifiedFunctionSize5[sizeof(Function<PlayerAttackData*>)];

#include "NL/nlBindMember.h"


static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

bool gCameraEffectsEnabled = true;
float gCameraTargetYOffsetScale = 0.6f;
float gShotPresentationZoom = -6.0f;
float gShotPresentationInitialTimeScale = 0.65f;
float gShotPresentationEndTimeScale = 0.1f;
float gShotPresentationMinBallCharge = 3.0f;
float gShotPresentationPlayerClearRadius = 2.0f;
float gShotPresentationMinPassTime = 0.2f;
bool gShotPresentationRotateCamera = true;
float gShotPresentationOutTime = 0.15f;
float gCaptainClashZoom = -3.0f;
float gCaptainClashRotationDegrees = -1.0f;
float gCaptainClashInTime = 0.5f;
float gCaptainClashOutTime = 0.6f;
float gCaptainClashInitialTimeScale = 0.075f;
float gCaptainClashEndTimeScale = 0.99f;
bool gCaptainClashRotateCamera = true;
float gGoalieSaveTimeScale = 1.0f;
float gGoalieSaveMinBallSpeed = 40.0f;
float gGoalieSaveMinBallCharge = 3.7f;
float gGoalieSlamZoom = -3.0f;
float gGoalieSlamRotationDegrees = -1.0f;
float gGoalieSlamInTime = 0.5f;
float gGoalieSlamOutTime = 0.6f;
float gGoalieSlamInitialTimeScale = 0.075f;
float gGoalieSlamEndTimeScale = 0.99f;
bool gGoalieSlamRotateCamera = true;
float gGoalieDekeZoom = -3.0f;
float gGoalieDekeRotationDegrees = -1.0f;
float gGoalieDekeInTime = 0.5f;
float gGoalieDekeOutTime = 0.6f;
float gGoalieDekeInitialTimeScale = 0.075f;
float gGoalieDekeEndTimeScale = 0.99f;
bool gGoalieDekeRotateCamera = true;
float gCameraRumbleX = 0.2f;
float gCameraRumbleY = 0.225f;
float gCameraRumbleSpring = 4300.0f;
float gCameraRumbleDamping = 6.0f;
float gCameraFlagUpdateInterval = 0.1f;
float gCameraOffensiveZoneZoomAdjustment = 0.4f;
float gCameraWindupZoomAdjustment = 0.6f;
float gCameraAlternateWindupZoomAdjustment = -0.8f;
float gCameraMegaStrikeMeterZoomAdjustment = 0.65f;
float gCameraClearFieldersZoomAdjustment = 0.4f;
float gCameraFielderClearRadius = 9.0f;
float gCameraMaxStadiumTilt = 8.0f;
float gCameraTiltTargetOffset = -3.0f;
float gStadium10CameraHeightZoomAdjustment = -1.0f;

float lbl_806E0F20[2];
float gShotPresentationPassTimeOffset;
float gGoalieSaveZoom;
float gGoalieSaveRotationDegrees;
float gGoalieSaveTransitionTime;
bool gGoalieSaveRotateCamera;
bool gCameraZoomOverrideEnabled;
float gCameraNoFlagsZoomAdjustment;
float gCameraCaptainFlagZoomAdjustment;
float gCameraStadiumTiltZoomAdjustment;

template <>
UnidentifiedCameraEffects*
    nlSingleton<UnidentifiedCameraEffects>::s_pInstance = 0;

static UnidentifiedTypedEvent<GoalieSaveData>*
GetGoalieSaveEvent(const char* name, int length);

UnidentifiedCameraEffects::UnidentifiedCameraEffects()
{
    mCameraFlags = 0;
    mFlagUpdateTimer = 0.0f;
    mTransitionBlend = 0.0f;
    mZoomStart = 0.0f;
    mRotateCamera = false;
    mTrackSecondaryPlayer = false;
    mRotationDegrees = 0.0f;
    mTransitionTime = 0.0f;
    mTransitionInTime = 0.0f;
    mTransitionOutTime = 0.0f;
    mTransitionHoldTime = 0.0f;
    mOwnsTimeScale = false;
    mPrimaryPlayer = 0;
    mSecondaryPlayer = 0;
    mZoomScale = 0.0f;
    mTransitionScale = 0.0f;
}

void UnidentifiedCameraEffects::RegisterEventListeners()
{
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(BindMember(this, &UnidentifiedCameraEffects::OnGoalScored)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("ShotPresentation", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnShotPresentation)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("ShotPresentationEnd", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnShotPresentationEnd)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("CaptainClashPresentation", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnCaptainClashPresentation)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("CaptainClashPresentationEnd", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnCaptainClashPresentationEnd)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("WindupPresentation", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnWindupPresentation)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("WindupPresentationEnd", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnWindupPresentationEnd)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeMeterData>("MegaStrikeMeterStart", -1)->Add(Function<MegaStrikeMeterData*>(BindMember(this, &UnidentifiedCameraEffects::OnMegaStrikeMeterStart)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegaStrikeMeterEnd", -1)->Add(Function<FnVoidVoid>(BindMember(this, &UnidentifiedCameraEffects::OnMegaStrikeMeterEnd)), 0, -1);
    GetGoalieSaveEvent("GoalieSave", -1)->Add(Function<GoalieSaveData*>(BindMember(this, &UnidentifiedCameraEffects::OnGoalieSave)), 0, -1);
    UnidentifiedFindEvent<CollisionThwompPlayerData>("CollisionThwompPlayer", -1)->Add(Function<CollisionThwompPlayerData*>(BindMember(this, &UnidentifiedCameraEffects::OnCollisionThwompPlayer)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieDekeAttackAttempt", -1)->Add(Function<PlayerAttackData*>(BindMember(this, &UnidentifiedCameraEffects::OnGoalieDekeAttackAttempt)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieDekeAttackSuccess", -1)->Add(Function<PlayerAttackData*>(BindMember(this, &UnidentifiedCameraEffects::OnGoalieDekeAttackSuccess)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieSlamAttackAttempt", -1)->Add(Function<PlayerAttackData*>(BindMember(this, &UnidentifiedCameraEffects::OnGoalieSlamAttackAttempt)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieSlamAttackSuccess", -1)->Add(Function<PlayerAttackData*>(BindMember(this, &UnidentifiedCameraEffects::OnGoalieSlamAttackSuccess)), 0, -1);
}

void UnidentifiedCameraEffects::Update(float deltaTime)
{
    if (g_pGame->m_eGameState == 3)
    {
        Reset();
        return;
    }

    if (mPrimaryPlayer != 0
        && mPrimaryPlayer->GetDesireState() != (eFielderDesireState)0x16
        && mPrimaryPlayer->m_eActionState != ACTION_ONETIMER
        && mPrimaryPlayer->m_eActionState != ACTION_PASS
        && mPrimaryPlayer->m_eActionState != ACTION_ONETOUCH_PASS_FROM_VOLLEY)
    {
        Reset();
    }

    mZoomScale = 0.0f;
    mTransitionScale = 0.0f;
    mFlagUpdateTimer += deltaTime;
    if (mFlagUpdateTimer <= gCameraFlagUpdateInterval)
    {
        mFlagUpdateTimer = 0.0f;
        UpdateCameraFlags();
    }

    if (gCameraEffectsEnabled == true)
    {
        if (IsTransitionActive())
        {
            if (nlTaskManager::m_pInstance->mCurrentState == 2)
                UpdateTransition(deltaTime);
        }
        else if (mOwnsTimeScale == true)
        {
            Reset();
        }
        mZoomScale = CalculateZoomScale(deltaTime);
    }
}

void UnidentifiedCameraEffects::UnidentifiedNoOp()
{
}

void UnidentifiedCameraEffects::Reset()
{
    if (mOwnsTimeScale && g_pNetworkSessionBase->GetLocalMachineId() == 0
        && !gNetworkInputRecording->mPlaybackReady)
    {
        g_pGame->SendSlowDownEnd();
    }

    mOwnsTimeScale = false;
    mRestoreTimeScale = false;
    mTransitionTime = 0.0f;
    mTransitionInTime = 0.0f;
    mTransitionOutTime = 0.0f;
    mTransitionHoldTime = 0.0f;
    mZoomStart = 0.0f;
    mRotationDegrees = 0.0f;
    mRotateCamera = false;
    mTrackSecondaryPlayer = false;
    mUseRealTime = false;
    mPrimaryPlayer = 0;
    mSecondaryPlayer = 0;
}

void UnidentifiedCameraEffects::UpdateCameraFlags()
{
    if (AreFieldersClear() == true)
        mCameraFlags |= 0x10;
    else
        mCameraFlags &= ~0x10;

    cFielder* owner = (cFielder*)g_pBall->m_pOwner;
    bool facingGoal;
    if (owner != 0 && owner->m_eClassType == FIELDER)
    {
        float goalLineX = cField::GetGoalLineX((unsigned int)
            owner->m_pTeam->GetOtherTeam()->m_nSide);
        if (goalLineX > 0.0
            && owner->mUnidentified024.m_v3Position.x > 0.0f)
        {
            facingGoal = true;
            goto facingGoalKnown;
        }
        if (goalLineX < 0.0
            && owner->mUnidentified024.m_v3Position.x < 0.0f)
        {
            facingGoal = true;
            goto facingGoalKnown;
        }
    }
    facingGoal = false;
facingGoalKnown:
    if (facingGoal == true)
        mCameraFlags |= 1;
    else
        mCameraFlags &= ~1;

    if (g_pTeams[0]->GetCaptain()->IsSuperGrowActive()
        || g_pTeams[1]->GetCaptain()->IsSuperGrowActive())
        mCameraFlags |= 0x20;
    else
        mCameraFlags &= ~0x20;

    if (g_pGame->GetXAxisTilt() != 0.0f || g_pGame->GetYAxisTilt() != 0.0f)
        mCameraFlags |= 0x40;
}

bool UnidentifiedCameraEffects::AreFieldersClear() const
{
    cFielder* owner = (cFielder*)g_pBall->m_pOwner;
    bool clear = false;
    float dy;
    float dx;
    float dz;
    float minimumDistanceSq = gCameraFielderClearRadius * gCameraFielderClearRadius;
    if (owner != 0 && owner->m_eClassType == FIELDER)
    {
        int otherTeam = owner->m_pTeam->GetOtherTeam()->m_nSide;
        float goalLineX = cField::GetGoalLineX((unsigned int)otherTeam);
        for (int i = 0; i < 4; ++i)
        {
            cFielder* fielder = g_pTeams[otherTeam]->GetFielder(i);
            if (fielder->mUnidentified120 == owner->mUnidentified120)
                continue;

            if (goalLineX > 0.0f)
            {
                if (fielder->mUnidentified024.m_v3Position.x > owner->mUnidentified024.m_v3Position.x
                    && !fielder->IsInFallAction()
                    && !fielder->IsFallenDown()
                    && fielder->m_eActionState != (eFielderActionState)0x23)
                {
                    return false;
                }
            }
            if (goalLineX < 0.0f)
            {
                if (fielder->mUnidentified024.m_v3Position.x < owner->mUnidentified024.m_v3Position.x
                    && !fielder->IsInFallAction()
                    && !fielder->IsFallenDown()
                    && fielder->m_eActionState != (eFielderActionState)0x23)
                {
                    return false;
                }
            }

            dy = fielder->mUnidentified024.m_v3Position.y - owner->mUnidentified024.m_v3Position.y;
            dx = fielder->mUnidentified024.m_v3Position.x - owner->mUnidentified024.m_v3Position.x;
            dz = fielder->mUnidentified024.m_v3Position.z - owner->mUnidentified024.m_v3Position.z;
            nlVector3 delta;
            delta.x = dx;
            delta.y = dy;
            delta.z = dz;
            if (delta.GetLengthSq3D() < minimumDistanceSq)
                return false;
        }
        clear = true;
    }
    return clear;
}

bool UnidentifiedCameraEffects::IsPassTargetClear() const
{
    cPlayer* owner = g_pBall->m_pOwner;
    cPlayer* passTarget = g_pBall->m_pPassTarget;
    float minimumDistanceSq = gShotPresentationPlayerClearRadius * gShotPresentationPlayerClearRadius;
    if (owner != 0 || passTarget == 0
        || GetBallChargeValue(g_pBall, 0) < gShotPresentationMinBallCharge)
    {
        return false;
    }
    if (g_pBall->m_tPassTargetTimer.GetSeconds() < gShotPresentationMinPassTime)
    {
        return false;
    }

    int otherTeam = passTarget->m_pTeam->GetOtherTeam()->m_nSide;
    for (int i = 0; i < 4; ++i)
    {
        cFielder* fielder = g_pTeams[otherTeam]->GetFielder(i);
        if (fielder->mUnidentified120 == passTarget->mUnidentified120)
            continue;
        float dy = fielder->mUnidentified024.m_v3Position.y - passTarget->mUnidentified024.m_v3Position.y;
        float dx = fielder->mUnidentified024.m_v3Position.x - passTarget->mUnidentified024.m_v3Position.x;
        float dz = fielder->mUnidentified024.m_v3Position.z - passTarget->mUnidentified024.m_v3Position.z;
        nlVector3 delta;
        delta.x = dx;
        delta.y = dy;
        delta.z = dz;
        if (delta.GetLengthSq3D() < minimumDistanceSq)
            return false;
    }
    return true;
}

float UnidentifiedCameraEffects::CalculateZoomScale(float) const
{
    float result = 1.0f;
    if ((mCameraFlags & 1) != 0)
        result -= gCameraOffensiveZoneZoomAdjustment;
    if ((mCameraFlags & 2) != 0)
        result -= gCameraWindupZoomAdjustment;
    if ((mCameraFlags & 4) != 0)
        result -= gCameraAlternateWindupZoomAdjustment;
    if ((mCameraFlags & 8) != 0)
        result -= gCameraMegaStrikeMeterZoomAdjustment;
    if ((mCameraFlags & 0x10) != 0)
        result -= gCameraClearFieldersZoomAdjustment;
    if ((mCameraFlags & 0x20) != 0)
        result -= gCameraCaptainFlagZoomAdjustment;
    if ((mCameraFlags & 0x40) != 0)
    {
        float gameX = g_pGame->mfXTilt;
        float clampedX = nlMinEquals(gCameraMaxStadiumTilt, gameX);
        float gameY = g_pGame->mfYTilt;
        float clampedY = nlMinEquals(gCameraMaxStadiumTilt, gameY);
        float amount = nlMaxEquals(fabsf(clampedX), fabsf(clampedY));
        float fraction = amount / gCameraMaxStadiumTilt;
        result -= fraction * gCameraStadiumTiltZoomAdjustment;
    }
    if (mCameraFlags == 0)
        result -= gCameraNoFlagsZoomAdjustment;
    if (gCameraZoomOverrideEnabled)
        result = 1.0f;
    return result;
}

void UnidentifiedCameraEffects::UpdateTransition(float deltaTime)
{
    if (mUseRealTime == true)
    {
        mTransitionTime -= deltaTime * GetFixedUpdateTask()->GetTimeScale();
    }
    else
    {
        mTransitionTime -= deltaTime;
    }

    mTransitionBlend = 0.0f;
    if (mTransitionTime >= 0.0f)
    {
        mTransitionBlend = nlAbs(mTransitionTime / mTransitionInTime);
    }
    else if (mTransitionHoldTime > 0.0f)
    {
        mTransitionTime = 0.0f;
        mTransitionBlend = 0.0f;
        mTransitionHoldTime -= deltaTime;
    }
    else
    {
        mTransitionBlend = nlAbs(mTransitionTime / mTransitionOutTime);
    }

    if (!IsTransitionActive())
    {
        Reset();
        return;
    }

    mTransitionBlend
        = nlMinEquals(nlMaxEquals(mTransitionBlend, 0.0f), 1.0f);
    mTransitionBlend = (float)__fabs(1.0f - mTransitionBlend);
    if (mTransitionBlend != 0.0f)
    {
        mTransitionScale = Interpolate(
            0.0f, mZoomStart, mTransitionBlend);
    }

    if (mRestoreTimeScale == true && mOwnsTimeScale == true
        && mTransitionTime <= 0.0f)
    {
        if (g_pNetworkSessionBase->GetLocalMachineId() == 0
            && !gNetworkInputRecording->mPlaybackReady)
        {
            g_pGame->SendSlowDownEnd();
        }
        mOwnsTimeScale = false;
    }

    float zoomStart = mZoomStart;
    if (zoomStart < 0.0f)
    {
        float limit = -1.0f * zoomStart;
        mTransitionScale
            = nlMinEquals(nlMaxEquals(mTransitionScale, zoomStart), limit);
    }
    else
    {
        float limit = -1.0f * zoomStart;
        mTransitionScale
            = nlMinEquals(nlMaxEquals(mTransitionScale, limit), zoomStart);
    }
}

bool UnidentifiedCameraEffects::IsTransitionActive() const
{
    return (mTransitionInTime != 0.0f || mTransitionOutTime != 0.0f)
        && mTransitionTime >= -1.0f * mTransitionOutTime;
}

nlVector3 UnidentifiedCameraEffects::RotateCameraVector(
    const nlVector3& vector) const
{
    nlVector3 result = vector;
    if (mTransitionBlend != 0.0f && mRotationDegrees != 0.0f)
    {
        float rotation = Interpolate(
            0.0f, mRotationDegrees, mTransitionBlend);
        nlMatrix4 matrix;
        matrix.SetIdentity();
        nlMakeRotationMatrixY(matrix, rotation * 3.1415927f / 180.0f);
        nlMultDirVectorMatrix(result, vector, matrix);
    }
    return result;
}

void UnidentifiedCameraEffects::AdjustCameraVectors(float zoom,
    nlVector3* camera, nlVector3* target) const
{
    if ((mCameraFlags & 0x40) != 0)
    {
        float gameX = g_pGame->mfXTilt;
        float clampedX = nlMinEquals(gCameraMaxStadiumTilt, gameX);
        float gameY = g_pGame->mfYTilt;
        float clampedY = nlMinEquals(gCameraMaxStadiumTilt, gameY);
        float amount = nlMaxEquals(fabsf(clampedX), fabsf(clampedY));
        target->y += gCameraTiltTargetOffset * (amount / gCameraMaxStadiumTilt);
    }
    else if (GameInfoManager::Instance()->GetStadium() == 10)
    {
        camera->z += gStadium10CameraHeightZoomAdjustment * zoom;
    }
}

nlVector3 UnidentifiedCameraEffects::CalculateTargetOffset(
    const GameplayCamera* camera) const
{
    nlVector3 result = v3Zero;
    nlVector3 target = v3Zero;
    nlVector3 offset;
    nlVector3 finalCameraTarget;
    bool hasTarget = false;

    if (mTransitionBlend != 0.0f && mRotateCamera)
    {
        nlVector3 cameraTarget = camera->m_v3Target;
        target = cameraTarget;
        hasTarget = true;
    }
    if (mTransitionBlend != 0.0f && mTrackSecondaryPlayer
        && mSecondaryPlayer != 0)
    {
        target = mSecondaryPlayer->mUnidentified024.m_v3Position;
        hasTarget = true;
    }

    if (hasTarget == true)
    {
        finalCameraTarget = camera->m_v3Target;
        ReplayManager::Instance();
        nlVec3Sub(offset, target, finalCameraTarget);
        result.x = Interpolate(0.0f, offset.x, mTransitionBlend);
        result.y = gCameraTargetYOffsetScale
                 * Interpolate(0.0f, offset.y, mTransitionBlend);
    }

    return result;
}

void UnidentifiedCameraEffects::OnGoalScored(
    GoalScoredData*)
{
    Reset();
}

void UnidentifiedCameraEffects::ResetForPresentation(void*)
{
    Reset();
}

void UnidentifiedCameraEffects::OnShotPresentation()
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }
    if (FixedUpdateTask::GetTargetTimeScale() != 1.0f)
    {
        return;
    }
    if (!IsPassTargetClear())
    {
        return;
    }

    Reset();
    float endTime = g_pBall->m_tPassTargetTimer.GetSeconds()
                  + gShotPresentationPassTimeOffset;
    g_pGame->StartSlowDown(gShotPresentationInitialTimeScale, 0.0f);
    g_pGame->StartSlowDown(gShotPresentationEndTimeScale, endTime);
    mTransitionTime = endTime;
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mUseRealTime = true;
    mTransitionInTime = endTime;
    mTransitionOutTime = gShotPresentationOutTime;
    mZoomStart = gShotPresentationZoom;
    mRotationDegrees = 0.0f;
    mRotateCamera = gShotPresentationRotateCamera;
    mPrimaryPlayer = (cFielder*)g_pBall->m_pPassTarget;
}

void UnidentifiedCameraEffects::OnShotPresentationEnd()
{
    if (g_pGame->m_eGameState != 3 && IsTransitionActive()
        && mPrimaryPlayer != 0
        && GetBallChargeValue(g_pBall, 0) >= 4.0f)
    {
        FireCameraRumbleFilter(
            gCameraRumbleX, gCameraRumbleY, gCameraRumbleSpring, gCameraRumbleDamping);
    }
}

void UnidentifiedCameraEffects::OnCaptainClashPresentation()
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (FixedUpdateTask::GetTargetTimeScale() == 1.0f)
    {
        Reset();
        FireCameraRumbleFilter(
            gCameraRumbleX, gCameraRumbleY, gCameraRumbleSpring, gCameraRumbleDamping);
        g_pGame->StartSlowDown(gCaptainClashInitialTimeScale, 0.0f);
        g_pGame->StartSlowDown(gCaptainClashEndTimeScale, gCaptainClashInTime);
        mOwnsTimeScale = true;
        mRestoreTimeScale = true;
        mTransitionTime = gCaptainClashInTime;
        mTransitionInTime = gCaptainClashInTime;
        mTransitionOutTime = gCaptainClashOutTime;
        mZoomStart = gCaptainClashZoom;
        mRotationDegrees = gCaptainClashRotationDegrees;
        mRotateCamera = gCaptainClashRotateCamera;

        if (g_pBall->m_pLastTouch != 0
            && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
                   ->mUnidentified024.m_v3Velocity.x < 0.0f)
        {
            mRotationDegrees *= -1.0f;
        }
    }
}

void UnidentifiedCameraEffects::OnCaptainClashPresentationEnd()
{
}

void UnidentifiedCameraEffects::OnWindupPresentation()
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (static_cast<cFielder*>(g_pBall->m_pOwner)->ShouldIClearBall() == true)
    {
        mCameraFlags |= 4;
    }
    else
    {
        mCameraFlags |= 2;
    }
}

void UnidentifiedCameraEffects::OnWindupPresentationEnd()
{
    mCameraFlags &= ~6;
    Reset();
}

void UnidentifiedCameraEffects::OnMegaStrikeMeterStart(
    MegaStrikeMeterData*)
{
    mCameraFlags |= 8;
}

void UnidentifiedCameraEffects::OnMegaStrikeMeterEnd()
{
    mCameraFlags &= ~8;
}

void UnidentifiedCameraEffects::OnGoalieSave(
    GoalieSaveData*)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    float ballSpeedSq = g_pBall->m_v3Velocity.GetLengthSq3D();
    if (FixedUpdateTask::GetTargetTimeScale() != 1.0f)
    {
        return;
    }
    if (!(ballSpeedSq >= gGoalieSaveMinBallSpeed * gGoalieSaveMinBallSpeed))
    {
        return;
    }
    if (!(GetBallChargeValue(g_pBall, 0) > gGoalieSaveMinBallCharge))
    {
        return;
    }

    Reset();
    if (gGoalieSaveTimeScale != 1.0f)
    {
        g_pGame->StartSlowDown(gGoalieSaveTimeScale, 0.0f);
        mOwnsTimeScale = true;
        mRestoreTimeScale = true;
    }
    mTransitionTime = gGoalieSaveTransitionTime;
    mTransitionInTime = gGoalieSaveTransitionTime;
    mTransitionOutTime = gGoalieSaveTransitionTime;
    mZoomStart = gGoalieSaveZoom;
    mRotationDegrees = gGoalieSaveRotationDegrees;
    mRotateCamera = gGoalieSaveRotateCamera;
    FireCameraRumbleFilter(
        gCameraRumbleX, gCameraRumbleY, gCameraRumbleSpring, gCameraRumbleDamping);
    if (g_pBall->m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnCollisionThwompPlayer(
    CollisionThwompPlayerData* eventData)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }
    if (eventData == 0)
    {
        return;
    }
    if (eventData->thwomp == 0)
    {
        return;
    }
    if (eventData->state != THWOMP_STATE_FALLING)
    {
        return;
    }
    if (eventData->target == 0)
    {
        return;
    }

    cPlayer* player = (cPlayer*)eventData->target;
    if (player->m_eClassType == FIELDER && g_pBall->m_pOwner == player)
    {
        FireCameraRumbleFilter(
            gCameraRumbleX, gCameraRumbleY, gCameraRumbleSpring, gCameraRumbleDamping);
    }
}

void UnidentifiedCameraEffects::OnGoalieDekeAttackAttempt(
    PlayerAttackData*)
{
}

void UnidentifiedCameraEffects::OnGoalieDekeAttackSuccess(
    PlayerAttackData*)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (FixedUpdateTask::GetTargetTimeScale() != 1.0f)
    {
        return;
    }

    FireCameraRumbleFilter(
        gCameraRumbleX, gCameraRumbleY, gCameraRumbleSpring, gCameraRumbleDamping);
    Reset();
    g_pGame->StartSlowDown(gGoalieDekeInitialTimeScale, 0.0f);
    g_pGame->StartSlowDown(gGoalieDekeEndTimeScale, gGoalieDekeInTime);
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mTransitionTime = gGoalieDekeInTime;
    mTransitionInTime = gGoalieDekeInTime;
    mTransitionOutTime = gGoalieDekeOutTime;
    mZoomStart = gGoalieDekeZoom;
    mRotationDegrees = gGoalieDekeRotationDegrees;
    mRotateCamera = gGoalieDekeRotateCamera;

    if (g_pBall->m_pLastTouch != 0
        && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
               ->mUnidentified024.m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnGoalieSlamAttackAttempt(
    PlayerAttackData*)
{
}

void UnidentifiedCameraEffects::OnGoalieSlamAttackSuccess(
    PlayerAttackData*)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (FixedUpdateTask::GetTargetTimeScale() != 1.0f)
    {
        return;
    }

    Reset();
    g_pGame->StartSlowDown(gGoalieSlamInitialTimeScale, 0.0f);
    g_pGame->StartSlowDown(gGoalieSlamEndTimeScale, gGoalieSlamInTime);
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mTransitionTime = gGoalieSlamInTime;
    mTransitionInTime = gGoalieSlamInTime;
    mTransitionOutTime = gGoalieSlamOutTime;
    mZoomStart = gGoalieSlamZoom;
    mRotationDegrees = gGoalieSlamRotationDegrees;
    mRotateCamera = gGoalieSlamRotateCamera;

    if (g_pBall->m_pLastTouch != 0
        && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
               ->mUnidentified024.m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

#include "NL/nlBindMember.inl"
#include "NL/nlFunction.inl"

static UnidentifiedTypedEvent<GoalieSaveData>*
GetGoalieSaveEvent(const char* name, int length)
{
    unsigned int hash = HashEventName(name, length);
    EventRegistryValue* foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    EventBase* event = foundEvent != 0 ? foundEvent->event : 0;
    return (UnidentifiedTypedEvent<GoalieSaveData>*)event;
}
