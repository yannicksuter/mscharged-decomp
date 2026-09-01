#include "Game/Camera/tu_800F9460.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/Camera/GameplayCam.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlTask.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C" UnidentifiedEventRegistry* lbl_806E1D90;
extern "C" unsigned char* lbl_806E2164;
extern "C" float fn_800155A0(cBall*, int);
extern "C" bool fn_800344B0(cFielder*);
extern "C" bool fn_8003C180(cPlayer*);
extern "C" bool fn_8003E74C(cFielder*);
extern "C" float fn_80111D3C();
extern "C" float fn_80111D6C();
extern "C" void* fn_8011166C();

extern "C" UnidentifiedTypedEvent<UnidentifiedEventData_80066748>*
    fn_80023350(const char*, int);
extern "C" UnidentifiedTypedEvent<UnidentifiedEventData_80065F14>*
    fn_8002356C(const char*, int);
extern "C" UnidentifiedTypedEvent<UnidentifiedEventData_8006701C>*
    fn_80023F8C(const char*, int);
extern "C" UnidentifiedTypedEvent<UnidentifiedEventData00>*
    fn_80023FF8(const char*, int);
extern "C" UnidentifiedTypedEvent<UnidentifiedEventData32>*
    fn_800240D0(const char*, int);

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

static UnidentifiedTypedEvent<UnidentifiedEventData_8006649C>*
GetGoalieSaveEvent(const char* name, int length)
{
    unsigned int hash = fn_802B289C(name, length);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    return (UnidentifiedTypedEvent<UnidentifiedEventData_8006649C>*)event;
}

void UnidentifiedCameraEffects::OnGoalieSlamAttackSuccess(
    UnidentifiedEventData_80066748*)
{
    if (g_pGame->m_eGameState == 3 || fn_80111D3C() != 1.0f)
    {
        return;
    }

    Reset();
    g_pGame->fn_80058528(lbl_806DC5D8, 0.0f);
    g_pGame->fn_80058528(lbl_806DC5DC, lbl_806DC5D0);
    mOwnsTimeScale = true;
    mRestoreTimeScale = true;
    mTransitionTime = lbl_806DC5D0;
    mTransitionInTime = lbl_806DC5D0;
    mTransitionOutTime = lbl_806DC5D4;
    mZoomStart = lbl_806DC5C8;
    mRotationDegrees = lbl_806DC5CC;
    mRotateCamera = lbl_806DC5C4;

    if (g_pBall->m_pLastTouch != 0
        && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
               ->m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnGoalieSlamAttackAttempt(
    UnidentifiedEventData_80066748*)
{
}

void UnidentifiedCameraEffects::OnGoalieDekeAttackSuccess(
    UnidentifiedEventData_80066748*)
{
    if (g_pGame->m_eGameState == 3 || fn_80111D3C() != 1.0f)
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
    mRotateCamera = lbl_806DC5A8;

    if (g_pBall->m_pLastTouch != 0
        && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
               ->m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnGoalieDekeAttackAttempt(
    UnidentifiedEventData_80066748*)
{
}

void UnidentifiedCameraEffects::OnCollisionThwompPlayer(
    UnidentifiedEventData32* eventData)
{
    if (g_pGame->m_eGameState == 3 || eventData == 0
        || eventData->source == 0 || eventData->sourceValue != (void*)3
        || eventData->target == 0)
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
    UnidentifiedEventData_8006649C*)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (g_pBall->m_v3Velocity.GetLengthSq3D()
            < lbl_806DC588 * lbl_806DC588
        || fn_80111D3C() != 1.0f
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

void UnidentifiedCameraEffects::OnMegaStrikeMeterEnd(
    UnidentifiedEventData00*)
{
    mCameraFlags &= ~8;
}

void UnidentifiedCameraEffects::OnMegaStrikeMeterStart(
    UnidentifiedEventData_8006701C*)
{
    mCameraFlags |= 8;
}

void UnidentifiedCameraEffects::OnWindupPresentationEnd(
    UnidentifiedEventData00*)
{
    mCameraFlags &= ~6;
    Reset();
}

void UnidentifiedCameraEffects::OnWindupPresentation(
    UnidentifiedEventData00*)
{
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }

    if (fn_8003C180(g_pBall->m_pOwner))
    {
        mCameraFlags |= 4;
    }
    else
    {
        mCameraFlags |= 2;
    }
}

void UnidentifiedCameraEffects::OnCaptainClashPresentationEnd(
    UnidentifiedEventData00*)
{
}

void UnidentifiedCameraEffects::OnCaptainClashPresentation(
    UnidentifiedEventData00*)
{
    if (g_pGame->m_eGameState == 3 || fn_80111D3C() != 1.0f)
    {
        return;
    }

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
    mRotateCamera = lbl_806DC560;

    if (g_pBall->m_pLastTouch != 0
        && g_pBall->m_pLastTouch->m_pTeam->GetOtherTeam()->GetCaptain()
               ->m_v3Velocity.x < 0.0f)
    {
        mRotationDegrees *= -1.0f;
    }
}

void UnidentifiedCameraEffects::OnShotPresentationEnd(
    UnidentifiedEventData00*)
{
    if (g_pGame->m_eGameState != 3 && IsTransitionActive()
        && mPrimaryPlayer != 0
        && fn_800155A0(g_pBall, 0) >= 4.0f)
    {
        FireCameraRumbleFilter(
            lbl_806DC5C8, lbl_806DC5CC, lbl_806DC5D0, lbl_806DC5D4);
    }
}

void UnidentifiedCameraEffects::OnShotPresentation(
    UnidentifiedEventData00*)
{
    if (g_pGame->m_eGameState == 3 || fn_80111D3C() != 1.0f
        || !IsPassTargetClear())
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
    UnidentifiedEventData_80065F14*)
{
    Reset();
}

nlVector3 UnidentifiedCameraEffects::CalculateTargetOffset(
    const GameplayCamera* camera) const
{
    nlVector3 result = { 0.0f, 0.0f, 0.0f };
    nlVector3 target = result;
    bool hasTarget = false;

    if (mTransitionBlend != 0.0f && mRotateCamera)
    {
        target = camera->m_v3Target;
        hasTarget = true;
    }
    if (mTransitionBlend != 0.0f && mTrackSecondaryPlayer
        && mSecondaryPlayer != 0)
    {
        target = mSecondaryPlayer->m_v3Position;
        hasTarget = true;
    }

    if (hasTarget)
    {
        nlVector3 offset;
        offset.x = target.x - camera->m_v3Target.x;
        offset.y = target.y - camera->m_v3Target.y;
        offset.z = target.z - camera->m_v3Target.z;
        ReplayManager::Instance();
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
        float gameY = g_pGame->mUnidentified084;
        float clampedX = gameX < lbl_806DC5F4 ? gameX : lbl_806DC5F4;
        float clampedY = gameY < lbl_806DC5F4 ? gameY : lbl_806DC5F4;
        float amount = nlAbs(clampedX) >= nlAbs(clampedY)
                         ? nlAbs(clampedX)
                         : nlAbs(clampedY);
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
        && mTransitionTime >= -mTransitionOutTime;
}

void UnidentifiedCameraEffects::UpdateTransition(float deltaTime)
{
    if (mUseRealTime)
    {
        fn_8011166C();
        mTransitionTime -= deltaTime * fn_80111D6C();
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

    float inverseBlend = 1.0f
                       - (mTransitionBlend < 0.0f
                              ? 0.0f
                              : (mTransitionBlend > 1.0f
                                     ? 1.0f
                                     : mTransitionBlend));
    mTransitionBlend = nlAbs(inverseBlend);
    if (mTransitionBlend != 0.0f)
    {
        mTransitionScale = Interpolate(
            0.0f, mZoomStart, mTransitionBlend);
    }

    if (mRestoreTimeScale && mOwnsTimeScale && mTransitionTime <= 0.0f)
    {
        if (fn_80338C20(lbl_806E20D8) == 0
            && (lbl_806E2164 == 0 || lbl_806E2164[4] == 0))
        {
            g_pGame->fn_80059FC4();
        }
        mOwnsTimeScale = false;
    }

    if (mZoomStart < 0.0f)
    {
        float minimum = 0.6f * mZoomStart;
        mTransitionScale = mTransitionScale >= mZoomStart
                         ? mTransitionScale
                         : mZoomStart;
        mTransitionScale = mTransitionScale <= minimum
                         ? mTransitionScale
                         : minimum;
    }
    else
    {
        float maximum = 0.6f * mZoomStart;
        mTransitionScale = mTransitionScale >= maximum
                         ? mTransitionScale
                         : maximum;
        mTransitionScale = mTransitionScale <= mZoomStart
                         ? mTransitionScale
                         : mZoomStart;
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
        float gameY = g_pGame->mUnidentified084;
        float clampedX = gameX < lbl_806DC5F4 ? gameX : lbl_806DC5F4;
        float clampedY = gameY < lbl_806DC5F4 ? gameY : lbl_806DC5F4;
        float amount = nlAbs(clampedX) >= nlAbs(clampedY)
                         ? nlAbs(clampedX)
                         : nlAbs(clampedY);
        result -= (amount / lbl_806DC5F4) * lbl_806E0F44;
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
    if (owner != 0 || passTarget == 0
        || fn_800155A0(g_pBall, 0) < lbl_806DC554
        || g_pBall->m_tPassTargetTimer.GetSeconds() < lbl_806DC55C)
    {
        return false;
    }

    cTeam* otherTeam = passTarget->m_pTeam->GetOtherTeam();
    float minimumDistanceSq = lbl_806DC558 * lbl_806DC558;
    for (int i = 0; i < 4; ++i)
    {
        cFielder* fielder = g_pTeams[otherTeam->m_nSide]->GetFielder(i);
        if (fielder->mUnidentified120 == passTarget->mUnidentified120)
            continue;
        nlVector3 delta;
        delta.x = fielder->m_v3Position.x - passTarget->m_v3Position.x;
        delta.y = fielder->m_v3Position.y - passTarget->m_v3Position.y;
        delta.z = fielder->m_v3Position.z - passTarget->m_v3Position.z;
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
                         ? fielder->m_v3Position.x > owner->m_v3Position.x
                         : fielder->m_v3Position.x < owner->m_v3Position.x;
        if (beyondOwner && !fn_800344B0(fielder)
            && !fielder->IsFallenDown()
            && fielder->m_eActionState != (eFielderActionState)0x23)
        {
            return false;
        }

        nlVector3 delta;
        delta.x = fielder->m_v3Position.x - owner->m_v3Position.x;
        delta.y = fielder->m_v3Position.y - owner->m_v3Position.y;
        delta.z = fielder->m_v3Position.z - owner->m_v3Position.z;
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
        facingGoal = goalLineX > 0.0f
                   ? owner->m_v3Position.x > 0.0f
                   : owner->m_v3Position.x < 0.0f;
    }
    if (facingGoal)
        mCameraFlags |= 1;
    else
        mCameraFlags &= ~1;

    if (fn_8003E74C(g_pTeams[0]->GetCaptain())
        || fn_8003E74C(g_pTeams[1]->GetCaptain()))
        mCameraFlags |= 0x20;
    else
        mCameraFlags &= ~0x20;

    if (g_pGame->mUnidentified080 != 0.0f
        || g_pGame->mUnidentified084 != 0.0f)
        mCameraFlags |= 0x40;
}

void UnidentifiedCameraEffects::Reset()
{
    if (mOwnsTimeScale && fn_80338C20(lbl_806E20D8) == 0
        && (lbl_806E2164 == 0 || lbl_806E2164[4] == 0))
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

    mFlagUpdateTimer += deltaTime;
    mZoomScale = 0.0f;
    mTransitionScale = 0.0f;
    if (mFlagUpdateTimer <= lbl_806DC5D8)
    {
        mFlagUpdateTimer = 0.0f;
        UpdateCameraFlags();
    }

    if (lbl_806DC540)
    {
        if (IsTransitionActive())
        {
            if (nlTaskManager::m_pInstance->mCurrentState == 2)
                UpdateTransition(deltaTime);
        }
        else if (mOwnsTimeScale)
        {
            Reset();
        }
        mZoomScale = CalculateZoomScale(deltaTime);
    }
}

void UnidentifiedCameraEffects::RegisterEventListeners()
{
    {
        Function<UnidentifiedEventData_80065F14*> callback(
            Bind<void>(MemFun(&UnidentifiedCameraEffects::OnGoalScored),
                this, placeholder0));
        fn_8002356C("GoalScored", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(
            Bind<void>(MemFun(&UnidentifiedCameraEffects::OnShotPresentation),
                this, placeholder0));
        fn_80023FF8("ShotPresentation", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnShotPresentationEnd),
            this, placeholder0));
        fn_80023FF8("ShotPresentationEnd", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnCaptainClashPresentation),
            this, placeholder0));
        fn_80023FF8("CaptainClashPresentation", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(Bind<void>(
            MemFun(
                &UnidentifiedCameraEffects::OnCaptainClashPresentationEnd),
            this, placeholder0));
        fn_80023FF8("CaptainClashPresentationEnd", -1)
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnWindupPresentation),
            this, placeholder0));
        fn_80023FF8("WindupPresentation", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnWindupPresentationEnd),
            this, placeholder0));
        fn_80023FF8("WindupPresentationEnd", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_8006701C*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnMegaStrikeMeterStart),
            this, placeholder0));
        fn_80023F8C("MegaStrikeMeterStart", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData00*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnMegaStrikeMeterEnd),
            this, placeholder0));
        fn_80023FF8("MegaStrikeMeterEnd", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_8006649C*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnGoalieSave),
            this, placeholder0));
        GetGoalieSaveEvent("GoalieSave", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData32*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnCollisionThwompPlayer),
            this, placeholder0));
        fn_800240D0("CollisionThwompPlayer", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_80066748*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnGoalieDekeAttackAttempt),
            this, placeholder0));
        fn_80023350("GoalieDekeAttackAttempt", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_80066748*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnGoalieDekeAttackSuccess),
            this, placeholder0));
        fn_80023350("GoalieDekeAttackSuccess", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_80066748*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnGoalieSlamAttackAttempt),
            this, placeholder0));
        fn_80023350("GoalieSlamAttackAttempt", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_80066748*> callback(Bind<void>(
            MemFun(&UnidentifiedCameraEffects::OnGoalieSlamAttackSuccess),
            this, placeholder0));
        fn_80023350("GoalieSlamAttackSuccess", -1)->Add(callback, 0, -1);
    }
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
