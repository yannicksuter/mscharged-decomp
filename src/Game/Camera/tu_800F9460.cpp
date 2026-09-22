#include "Game/Camera/tu_800F9460.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/Camera/GameplayCam.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/MathHelpers.h"
#include "Game/NetworkSession.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlTask.h"
#include "Game/UnidentifiedStaticStorage.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C" UnidentifiedEventRegistry* g_pEventRegistry;
extern "C" unsigned char* gNetworkInputRecording;
extern "C" bool fn_8003C180(cPlayer*);

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

bool lbl_806DC540 = true;
float lbl_806DC544 = 0.6f;
float lbl_806DC548 = -6.0f;
float lbl_806DC54C = 0.65f;
float lbl_806DC550 = 0.1f;
float lbl_806DC554 = 3.0f;
float lbl_806DC558 = 2.0f;
float lbl_806DC55C = 0.2f;
bool lbl_806DC560 = true;
float lbl_806DC564 = 0.15f;
float lbl_806DC568 = -3.0f;
float lbl_806DC56C = -1.0f;
float lbl_806DC570 = 0.5f;
float lbl_806DC574 = 0.6f;
float lbl_806DC578 = 0.075f;
float lbl_806DC57C = 0.99f;
bool lbl_806DC580 = true;
float lbl_806DC584 = 1.0f;
float lbl_806DC588 = 40.0f;
float lbl_806DC58C = 3.7f;
float lbl_806DC590 = -3.0f;
float lbl_806DC594 = -1.0f;
float lbl_806DC598 = 0.5f;
float lbl_806DC59C = 0.6f;
float lbl_806DC5A0 = 0.075f;
float lbl_806DC5A4 = 0.99f;
bool lbl_806DC5A8 = true;
float lbl_806DC5AC = -3.0f;
float lbl_806DC5B0 = -1.0f;
float lbl_806DC5B4 = 0.5f;
float lbl_806DC5B8 = 0.6f;
float lbl_806DC5BC = 0.075f;
float lbl_806DC5C0 = 0.99f;
bool lbl_806DC5C4 = true;
float lbl_806DC5C8 = 0.2f;
float lbl_806DC5CC = 0.225f;
float lbl_806DC5D0 = 4300.0f;
float lbl_806DC5D4 = 6.0f;
float lbl_806DC5D8 = 0.1f;
float lbl_806DC5DC = 0.4f;
float lbl_806DC5E0 = 0.6f;
float lbl_806DC5E4 = -0.8f;
float lbl_806DC5E8 = 0.65f;
float lbl_806DC5EC = 0.4f;
float lbl_806DC5F0 = 9.0f;
float lbl_806DC5F4 = 8.0f;
float lbl_806DC5F8 = -3.0f;
float lbl_806DC5FC = -1.0f;

float lbl_806E0F20[2];
float lbl_806E0F28;
float lbl_806E0F2C;
float lbl_806E0F30;
float lbl_806E0F34;
bool lbl_806E0F38;
bool lbl_806E0F39;
float lbl_806E0F3C;
float lbl_806E0F40;
float lbl_806E0F44;

template <>
UnidentifiedCameraEffects*
    nlSingleton<UnidentifiedCameraEffects>::s_pInstance = 0;

static UnidentifiedTypedEvent<GoalieSaveData>*
GetGoalieSaveEvent(const char* name, int length)
{
    unsigned int hash = HashEventName(name, length);
    UnidentifiedEventBase** foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    return (UnidentifiedTypedEvent<GoalieSaveData>*)event;
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
    g_pGame->fn_80058528(lbl_806DC5A0, 0.0f);
    g_pGame->fn_80058528(lbl_806DC5A4, lbl_806DC598);
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mTransitionTime = lbl_806DC598;
    mTransitionInTime = lbl_806DC598;
    mTransitionOutTime = lbl_806DC59C;
    mZoomStart = lbl_806DC590;
    mRotationDegrees = lbl_806DC594;
    mRotateCamera = lbl_806DC5A8;

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
        lbl_806DC5C8, lbl_806DC5CC, lbl_806DC5D0, lbl_806DC5D4);
    Reset();
    g_pGame->fn_80058528(lbl_806DC5BC, 0.0f);
    g_pGame->fn_80058528(lbl_806DC5C0, lbl_806DC5B4);
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mTransitionTime = lbl_806DC5B4;
    mTransitionInTime = lbl_806DC5B4;
    mTransitionOutTime = lbl_806DC5B8;
    mZoomStart = lbl_806DC5AC;
    mRotationDegrees = lbl_806DC5B0;
    mRotateCamera = lbl_806DC5C4;

    if (g_pBall->m_pLastTouch != 0
        && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
               ->mUnidentified024.m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnGoalieDekeAttackAttempt(
    PlayerAttackData*)
{
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
    if (eventData->source == 0)
    {
        return;
    }
    if ((int)eventData->sourceValue != 3)
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
            lbl_806DC5C8, lbl_806DC5CC, lbl_806DC5D0, lbl_806DC5D4);
    }
}

void UnidentifiedCameraEffects::OnGoalieSave(
    GoalieSaveData*)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (g_pBall->m_v3Velocity.GetLengthSq3D()
            < lbl_806DC588 * lbl_806DC588
        || FixedUpdateTask::GetTargetTimeScale() != 1.0f
        || fn_800155A0(g_pBall, 0) <= lbl_806DC58C)
    {
        return;
    }

    Reset();
    if (lbl_806DC584 != 1.0f)
    {
        g_pGame->fn_80058528(lbl_806DC584, 0.0f);
        mOwnsTimeScale = true;
        mRestoreTimeScale = true;
    }
    mTransitionTime = lbl_806E0F34;
    mTransitionInTime = lbl_806E0F34;
    mTransitionOutTime = lbl_806E0F34;
    mZoomStart = lbl_806E0F2C;
    mRotationDegrees = lbl_806E0F30;
    mRotateCamera = lbl_806E0F38;
    FireCameraRumbleFilter(
        lbl_806DC5C8, lbl_806DC5CC, lbl_806DC5D0, lbl_806DC5D4);
    if (g_pBall->m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnMegaStrikeMeterEnd()
{
    mCameraFlags &= ~8;
}

void UnidentifiedCameraEffects::OnMegaStrikeMeterStart(
    UnidentifiedEventData_8006701C*)
{
    mCameraFlags |= 8;
}

void UnidentifiedCameraEffects::OnWindupPresentationEnd()
{
    mCameraFlags &= ~6;
    Reset();
}

void UnidentifiedCameraEffects::OnWindupPresentation()
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (fn_8003C180(g_pBall->m_pOwner) == true)
    {
        mCameraFlags |= 4;
    }
    else
    {
        mCameraFlags |= 2;
    }
}

void UnidentifiedCameraEffects::OnCaptainClashPresentationEnd()
{
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
            lbl_806DC5C8, lbl_806DC5CC, lbl_806DC5D0, lbl_806DC5D4);
        g_pGame->fn_80058528(lbl_806DC578, 0.0f);
        g_pGame->fn_80058528(lbl_806DC57C, lbl_806DC570);
        mOwnsTimeScale = true;
        mRestoreTimeScale = true;
        mTransitionTime = lbl_806DC570;
        mTransitionInTime = lbl_806DC570;
        mTransitionOutTime = lbl_806DC574;
        mZoomStart = lbl_806DC568;
        mRotationDegrees = lbl_806DC56C;
        mRotateCamera = lbl_806DC580;

        if (g_pBall->m_pLastTouch != 0
            && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
                   ->mUnidentified024.m_v3Velocity.x < 0.0f)
        {
            mRotationDegrees *= -1.0f;
        }
    }
}

void UnidentifiedCameraEffects::OnShotPresentationEnd()
{
    if (g_pGame->m_eGameState != 3 && IsTransitionActive()
        && mPrimaryPlayer != 0
        && fn_800155A0(g_pBall, 0) >= 4.0f)
    {
        FireCameraRumbleFilter(
            lbl_806DC5C8, lbl_806DC5CC, lbl_806DC5D0, lbl_806DC5D4);
    }
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
                  + lbl_806E0F28;
    g_pGame->fn_80058528(lbl_806DC54C, 0.0f);
    g_pGame->fn_80058528(lbl_806DC550, endTime);
    mTransitionTime = endTime;
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mUseRealTime = true;
    mTransitionInTime = endTime;
    mTransitionOutTime = lbl_806DC564;
    mZoomStart = lbl_806DC548;
    mRotationDegrees = 0.0f;
    mRotateCamera = lbl_806DC560;
    mPrimaryPlayer = (cFielder*)g_pBall->m_pPassTarget;
}

void UnidentifiedCameraEffects::ResetForPresentation(void*)
{
    Reset();
}

void UnidentifiedCameraEffects::OnGoalScored(
    GoalScoredData*)
{
    Reset();
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
        result.y = lbl_806DC544
                 * Interpolate(0.0f, offset.y, mTransitionBlend);
    }

    return result;
}

void UnidentifiedCameraEffects::AdjustCameraVectors(float deltaTime,
    nlVector3* camera, nlVector3* target) const
{
    if ((mCameraFlags & 0x40) != 0)
    {
        float gameX = g_pGame->mUnidentified080;
        float clampedX = nlMinEquals(lbl_806DC5F4, gameX);
        float gameY = g_pGame->mUnidentified084;
        float clampedY = nlMinEquals(lbl_806DC5F4, gameY);
        float amount = nlAbs(clampedX);
        amount = nlMaxEquals(amount, nlAbs(clampedY));
        target->y += lbl_806DC5F8 * (amount / lbl_806DC5F4);
    }
    else if (GameInfoManager::Instance()->GetStadium() == 10)
    {
        camera->z += lbl_806DC5FC * deltaTime;
    }
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

bool UnidentifiedCameraEffects::IsTransitionActive() const
{
    return (mTransitionInTime != 0.0f || mTransitionOutTime != 0.0f)
        && mTransitionTime >= -1.0f * mTransitionOutTime;
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
            && gNetworkInputRecording[4] == 0)
        {
            g_pGame->fn_80059FC4();
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

float UnidentifiedCameraEffects::CalculateZoomScale(float) const
{
    float result = 1.0f;
    if ((mCameraFlags & 1) != 0)
        result -= lbl_806DC5DC;
    if ((mCameraFlags & 2) != 0)
        result -= lbl_806DC5E0;
    if ((mCameraFlags & 4) != 0)
        result -= lbl_806DC5E4;
    if ((mCameraFlags & 8) != 0)
        result -= lbl_806DC5E8;
    if ((mCameraFlags & 0x10) != 0)
        result -= lbl_806DC5EC;
    if ((mCameraFlags & 0x20) != 0)
        result -= lbl_806E0F40;
    if ((mCameraFlags & 0x40) != 0)
    {
        float gameX = g_pGame->mUnidentified080;
        float clampedX = nlMinEquals(lbl_806DC5F4, gameX);
        float gameY = g_pGame->mUnidentified084;
        float clampedY = nlMinEquals(lbl_806DC5F4, gameY);
        float amount = nlAbs(clampedX);
        amount = nlMaxEquals(amount, nlAbs(clampedY));
        float fraction = amount / lbl_806DC5F4;
        result -= fraction * lbl_806E0F44;
    }
    if (mCameraFlags == 0)
        result -= lbl_806E0F3C;
    if (lbl_806E0F39)
        result = 1.0f;
    return result;
}

bool UnidentifiedCameraEffects::IsPassTargetClear() const
{
    cPlayer* owner = g_pBall->m_pOwner;
    cPlayer* passTarget = g_pBall->m_pPassTarget;
    float minimumDistanceSq = lbl_806DC558 * lbl_806DC558;
    if (owner != 0 || passTarget == 0
        || fn_800155A0(g_pBall, 0) < lbl_806DC554)
    {
        return false;
    }
    if (g_pBall->m_tPassTargetTimer.GetSeconds() < lbl_806DC55C)
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

bool UnidentifiedCameraEffects::AreFieldersClear() const
{
    cFielder* owner = (cFielder*)g_pBall->m_pOwner;
    if (owner == 0 || owner->m_eClassType != FIELDER)
        return false;

    cTeam* otherTeam = owner->m_pTeam->GetOtherTeam();
    float goalLineX = cField::GetGoalLineX(
        (unsigned int)otherTeam->m_nSide);
    float minimumDistanceSq = lbl_806DC5F0 * lbl_806DC5F0;
    for (int i = 0; i < 4; ++i)
    {
        cFielder* fielder = g_pTeams[otherTeam->m_nSide]->GetFielder(i);
        if (fielder->mUnidentified120 == owner->mUnidentified120)
            continue;

        bool beyondOwner = goalLineX > 0.0f
                         ? fielder->mUnidentified024.m_v3Position.x > owner->mUnidentified024.m_v3Position.x
                         : fielder->mUnidentified024.m_v3Position.x < owner->mUnidentified024.m_v3Position.x;
        if (beyondOwner && !fielder->fn_800344B0()
            && !fielder->IsFallenDown()
            && fielder->m_eActionState != (eFielderActionState)0x23)
        {
            return false;
        }

        float dy = fielder->mUnidentified024.m_v3Position.y - owner->mUnidentified024.m_v3Position.y;
        float dx = fielder->mUnidentified024.m_v3Position.x - owner->mUnidentified024.m_v3Position.x;
        float dz = fielder->mUnidentified024.m_v3Position.z - owner->mUnidentified024.m_v3Position.z;
        nlVector3 delta;
        delta.x = dx;
        delta.y = dy;
        delta.z = dz;
        if (delta.GetLengthSq3D() < minimumDistanceSq)
            return false;
    }
    return true;
}

void UnidentifiedCameraEffects::UpdateCameraFlags()
{
    if (AreFieldersClear())
        mCameraFlags |= 0x10;
    else
        mCameraFlags &= ~0x10;

    cFielder* owner = (cFielder*)g_pBall->m_pOwner;
    bool facingGoal = false;
    if (owner != 0 && owner->m_eClassType == FIELDER)
    {
        float goalLineX = cField::GetGoalLineX((unsigned int)
            owner->m_pTeam->GetOtherTeam()->m_nSide);
        facingGoal = (goalLineX > 0.0
                         && owner->mUnidentified024.m_v3Position.x > 0.0f)
                  || (goalLineX < 0.0
                         && owner->mUnidentified024.m_v3Position.x < 0.0f);
    }
    if (facingGoal)
        mCameraFlags |= 1;
    else
        mCameraFlags &= ~1;

    if (g_pTeams[0]->GetCaptain()->fn_8003E74C()
        || g_pTeams[1]->GetCaptain()->fn_8003E74C())
        mCameraFlags |= 0x20;
    else
        mCameraFlags &= ~0x20;

    if (g_pGame->mUnidentified080 != 0.0f
        || g_pGame->mUnidentified084 != 0.0f)
        mCameraFlags |= 0x40;
}

void UnidentifiedCameraEffects::Reset()
{
    if (mOwnsTimeScale && g_pNetworkSessionBase->GetLocalMachineId() == 0
        && gNetworkInputRecording[4] == 0)
    {
        g_pGame->fn_80059FC4();
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

void UnidentifiedCameraEffects::UnidentifiedNoOp()
{
}

void UnidentifiedCameraEffects::Update(float deltaTime)
{
    if (g_pGame->m_eGameState == 3)
    {
        Reset();
        return;
    }

    if (mPrimaryPlayer != 0
        && mPrimaryPlayer->fn_8002E060() != (eFielderDesireState)0x16
        && mPrimaryPlayer->m_eActionState != ACTION_ONETIMER
        && mPrimaryPlayer->m_eActionState != ACTION_PASS
        && mPrimaryPlayer->m_eActionState != ACTION_ONETOUCH_PASS_FROM_VOLLEY)
    {
        Reset();
    }

    mZoomScale = 0.0f;
    mTransitionScale = 0.0f;
    mFlagUpdateTimer += deltaTime;
    if (mFlagUpdateTimer <= lbl_806DC5D8)
    {
        mFlagUpdateTimer = 0.0f;
        UpdateCameraFlags();
    }

    if (lbl_806DC540 == true)
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

void UnidentifiedCameraEffects::RegisterEventListeners()
{
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalScored), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("ShotPresentation", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnShotPresentation), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("ShotPresentationEnd", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnShotPresentationEnd), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("CaptainClashPresentation", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnCaptainClashPresentation), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("CaptainClashPresentationEnd", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnCaptainClashPresentationEnd), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("WindupPresentation", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnWindupPresentation), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("WindupPresentationEnd", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnWindupPresentationEnd), this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterStart", -1)->Add(Function<UnidentifiedEventData_8006701C*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnMegaStrikeMeterStart), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegaStrikeMeterEnd", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnMegaStrikeMeterEnd), this)), 0, -1);
    GetGoalieSaveEvent("GoalieSave", -1)->Add(Function<GoalieSaveData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalieSave), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<CollisionThwompPlayerData>("CollisionThwompPlayer", -1)->Add(Function<CollisionThwompPlayerData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnCollisionThwompPlayer), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieDekeAttackAttempt", -1)->Add(Function<PlayerAttackData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalieDekeAttackAttempt), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieDekeAttackSuccess", -1)->Add(Function<PlayerAttackData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalieDekeAttackSuccess), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieSlamAttackAttempt", -1)->Add(Function<PlayerAttackData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalieSlamAttackAttempt), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("GoalieSlamAttackSuccess", -1)->Add(Function<PlayerAttackData*>(Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalieSlamAttackSuccess), this, placeholder0)), 0, -1);
}

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
