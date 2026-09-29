#include "Game/Camera/ReplayCamera.h"
#include "Game/Camera/CameraDamping.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/tu_800F9460.h"
#include "Game/Render/RLViewLayers.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/CharacterTemplate.h"
#include "Game/Field.h"
#include "Game/MathHelpers.h"
#include "Game/Player.h"
#include "Game/ReplayManager.h"
#include "Game/Team.h"
#include "Game/Render/depthoffield.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "NL/nlPrint.h"
#include "NL/nlTask.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glPlat.h"
#include "Game/UnidentifiedStaticStorage.h"
#include <math.h>

static const nlVector3 sZeroVector = { 0.0f, 0.0f, 0.0f };

float gMatrixEffectCameraDistance[2] = { 18.0f, 0.0f };
float gReplayCameraPositionDampingTime = 0.25f;
float gReplayCameraLookAtDampingTime = 0.25f;
float gReplayCameraLookAtOffsetDampingTime = 0.5f;
float gReplayCameraDepthOfFieldOffset = 4.0f;
float gReplayCameraDepthOfFieldReferenceFov = 45.0f;
float gReplayCameraDepthOfFieldFovBlend = 1.0f;
float gReplayCameraVerticalFovMarginDegrees = 12.0f;
float gReplayCameraAspectRatio = 1.25f;
float gReplayCameraWidescreenAspectRatio[2] = { 1.666f, 0.0f };

u8 gMatrixEffectCameraFrozen[8];

static inline float GetSideDirection(int side)
{
    return side == 0 ? -1.0f : 1.0f;
}

static inline float LimitMagnitude(float value, float limit)
{
    if (nlAbs(value) > limit)
    {
        float sign;
        if (value == 0.0f)
            sign = 0.0f;
        else
            sign = value < 0.0f ? -1.0f : 1.0f;
        float magnitude = nlMinEquals(nlAbs(value), limit);
        value = magnitude * sign;
    }
    return value;
}

/**
 * Offset/Address/Size: 0x38A8 | 0x800F5970 | size: 0x4
 */
void ReplayCamera::UpdateTweakMode()
{
}
/**
 * Offset/Address/Size: 0x38A4 | 0x800F5974 | size: 0x12C
 */
ReplayCamera::ReplayCamera()
{
    mDeltaFov = 0.0f;
    mTargetFov = 0.0f;
    mFov = 50.0f;
    mSideOfInterest = 0;
    mNoDampenForOneUpdate = false;
    mNoDampenLookAtForOneUpdate = false;
    mFrozen = false;
    mPositionFrozen = false;
    mFocus = 0;
    mSecondaryFocus = 0;
    mCamPos = REPLAY_CAMERA_POSITION_SIDELINE;
    mAutoFov = false;
    mAutoFovMin = -1.0f;
    mAutoFovMax = -1.0f;
    mAutoFovMinDistance = -1.0f;
    mAutoFovMaxDistance = -1.0f;
    mAutoFovMaxChangeRate = 0.0f;
    mViewMatrix.SetIdentity();
    nlVec3Set(mPosition, 0.0f, 0.0f, 2.0f);
    nlVec3Set(mLookAt, 0.0f, 0.0f, 1.0f);
    nlVec3Set(mPositionVelocity, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mLookAtVelocity, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mLookAtOffsetVelocity, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mLookAtOffset, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mPositionOffset, 0.0f, 0.0f, 0.0f);
    mBallToGoalRotationDegrees = 0.0f;
}

/**
 * Offset/Address/Size: 0x3778 | 0x800F5AA0 | size: 0x40
 */
const nlMatrix4& ReplayCamera::GetViewMatrix() const
{
    glMatrixLookAt(mViewMatrix, mPosition, mLookAt, mUpVector);
    return mViewMatrix;
}

/**
 * Offset/Address/Size: 0x3738 | 0x800F5AE0 | size: 0x58
 */
float ReplayCamera::GetFOV() const
{
    float fov = mFov;
    if (IsWidescreen())
    {
        fov = fn_800F2410(mFov);
    }
    return fov;
}

/**
 * Offset/Address/Size: 0x36E0 | 0x800F5B38 | size: 0x4
 */
void ReplayCamera::Update(float fDeltaT)
{
}

/**
 * Offset/Address/Size: 0x36DC | 0x800F5B3C | size: 0x8BC
 */
void ReplayCamera::ManualUpdate(float deltaT)
{
    ReplayManager* replayManager = ReplayManager::Instance();
    if (replayManager->mRender == NULL)
    {
        return;
    }

    if (!mFrozen)
    {
        nlVector3 lookAt = GetFocusPosition(mFocus);
        nlVector3 position = GetPosition(mCamPos, GetSideDirection(mSideOfInterest));

        if (mPositionFrozen)
        {
            position = mPosition;
        }

        if (mNoDampenForOneUpdate)
        {
            mLookAt = lookAt;
            mPosition = position;
            mNoDampenForOneUpdate = false;
            mNoDampenLookAtForOneUpdate = false;
        }
        else if (mNoDampenLookAtForOneUpdate)
        {
            mLookAt = lookAt;
            mNoDampenLookAtForOneUpdate = false;
        }
        else
        {
            nlVec3Sub(mLookAt, mLookAt, mLookAtOffset);
            mPosition.x = Dampen(mPosition.x, position.x, mPositionVelocity.x, gReplayCameraPositionDampingTime, deltaT);
            mPosition.y = Dampen(mPosition.y, position.y, mPositionVelocity.y, gReplayCameraPositionDampingTime, deltaT);
            mPosition.z = Dampen(mPosition.z, position.z, mPositionVelocity.z, gReplayCameraPositionDampingTime, deltaT);
            mLookAt.x = Dampen(mLookAt.x, lookAt.x, mLookAtVelocity.x, gReplayCameraLookAtDampingTime, deltaT);
            mLookAt.y = Dampen(mLookAt.y, lookAt.y, mLookAtVelocity.y, gReplayCameraLookAtDampingTime, deltaT);
            mLookAt.z = Dampen(mLookAt.z, lookAt.z, mLookAtVelocity.z, gReplayCameraLookAtDampingTime, deltaT);
        }

        if (mAutoFov == true)
        {
            nlVector3 difference;
            nlVec3Sub(difference, lookAt, position);
            float distance = nlSqrt(difference.GetLengthSq3D(), true);
            float maxChange = mAutoFovMaxChangeRate * deltaT;
            float fov = InterpolateRangeClamped(mAutoFovMin, mAutoFovMax,
                mAutoFovMinDistance, mAutoFovMaxDistance, distance);
            if (nlAbs(mFov - fov) > maxChange)
            {
                if (fov < mFov)
                    fov = nlMaxEquals(fov, mFov - maxChange);
                else
                    fov = nlMinEquals(fov, mFov + maxChange);
            }
            mFov = fov;
        }
        else if (mDeltaFov != 0.0f)
        {
            if (mFov < mTargetFov)
                mFov += deltaT * mDeltaFov;
            else if (mFov > mTargetFov)
                mFov -= deltaT * mDeltaFov;

            if (fabsf(mFov - mTargetFov) < 2.0f * (deltaT * mDeltaFov))
                mDeltaFov = 0.0f;
        }

        nlVector3 targetOffset = { 0.0f, 0.0f, 0.0f };
        if (mSecondaryFocus != mFocus)
        {
            nlVector3 secondaryLookAt = GetFocusPosition(mSecondaryFocus);
            targetOffset = GetClampedFocusPosition(position, lookAt, secondaryLookAt,
                glplatGetDefaultTargetWidth(), glplatGetDefaultTargetHeight(),
                DegreesToRadians(mFov));
            nlVec3Sub(targetOffset, targetOffset, lookAt);
        }

        if (mLookAtOffset.x != targetOffset.x
            && mLookAtOffset.y != targetOffset.y
            && mLookAtOffset.z != targetOffset.z)
        {
            nlVec3Scale(targetOffset, targetOffset, 10.0f);
            nlVec3Scale(mLookAtOffset, mLookAtOffset, 10.0f);
            mLookAtOffset.x = Dampen(mLookAtOffset.x, targetOffset.x,
                mLookAtOffsetVelocity.x, gReplayCameraLookAtOffsetDampingTime, deltaT);
            mLookAtOffset.y = Dampen(mLookAtOffset.y, targetOffset.y,
                mLookAtOffsetVelocity.y, gReplayCameraLookAtOffsetDampingTime, deltaT);
            mLookAtOffset.z = Dampen(mLookAtOffset.z, targetOffset.z,
                mLookAtOffsetVelocity.z, gReplayCameraLookAtOffsetDampingTime, deltaT);
            nlVec3Scale(mLookAtOffset, mLookAtOffset, 0.1f);
        }

        nlVec3Add(mLookAt, mLookAt, mLookAtOffset);

        if (mFov < 1.0f)
            mFov = 1.0f;
        if (mFov > 120.0f)
            mFov = 120.0f;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 8)
    {
        float fovScale = BlendCameraValue(1.0f, gReplayCameraDepthOfFieldReferenceFov / mFov, gReplayCameraDepthOfFieldFovBlend);
        fovScale *= fovScale;
        lbl_806E0F20[0] = fovScale;

        DepthOfFieldManager::instance.m_fDistanceFromCamera
            = gReplayCameraDepthOfFieldOffset * lbl_806E0F20[0]
            + nlSqrt(CalculateDistanceSquared(mPosition, mLookAt), true);
    }
}

/**
 * Offset/Address/Size: 0x2E20 | 0x800F63F8 | size: 0x748
 */
nlVector3 ReplayCamera::GetClampedFocusPosition(const nlVector3& position,
    const nlVector3& lookAt, const nlVector3& secondaryLookAt,
    unsigned int width, unsigned int height, float fov) const
{
    float maximumAngle = DegreesToRadians(110.0f);
    float divisor = 4.0f;
    float angleScale = 1.0 - 1.0 / divisor;
    float horizontalLimit = fov * angleScale;
    horizontalLimit *= 0.5f;
    float aspectRatio = IsWidescreen() ? gReplayCameraWidescreenAspectRatio[0] : gReplayCameraAspectRatio;
    float verticalLimit = fov * (1.0 / aspectRatio) * angleScale;
    verticalLimit *= 0.5f;
    verticalLimit -= DegreesToRadians(gReplayCameraVerticalFovMarginDegrees);
    verticalLimit = nlMaxEquals(0.005f, verticalLimit);

    nlVector3 result = lookAt;
    if (!nlNear(secondaryLookAt, lookAt))
    {
        nlVector3 direction;
        nlVec3Sub(direction, lookAt, position);
        float distance = nlSqrt(direction.GetLengthSq3D(), true);
        nlVec3Normalize(direction, direction);

        nlVector3 secondaryDirection;
        nlVec3Sub(secondaryDirection, secondaryLookAt, position);
        nlVec3Normalize(secondaryDirection, secondaryDirection);

        nlVector3 side;
        nlVec3CrossProduct(side, direction, mUpVector);
        nlVec3Normalize(side, side);

        nlVector3 cameraUp;
        nlVec3CrossProduct(cameraUp, direction, side);
        nlVec3Normalize(cameraUp, cameraUp);
        if (cameraUp.GetLengthSq3D() < 0.0f)
            nlVec3Scale(cameraUp, -1.0f);

        nlVector3 v3Unidentified;
        nlVec3Set(v3Unidentified, 1.0f, 0.0f, 0.0f);

        nlMatrix4 cameraMatrix;
        nlMatrix4 inverseCameraMatrix;
        nlMakeRotTransMatrix(cameraMatrix, direction, cameraUp, mUpVector, sZeroVector);
        nlInvertRotTransMatrix(inverseCameraMatrix, cameraMatrix);

        nlVector3 localDirection;
        nlMultDirVectorMatrix(localDirection, secondaryDirection, inverseCameraMatrix);

        float horizontalAngle = AngUnitsToRad_fromUnsignedShort(
            nlVector3ToAngle(localDirection));
        if (localDirection.y < 0.0f && nlAbs(horizontalAngle) > horizontalLimit)
            horizontalAngle = -1.0f * (DegreesToRadians(360.0f) - horizontalAngle);
        if (nlAbs(horizontalAngle) > maximumAngle)
            horizontalAngle = 0.0f;

        float verticalAngle = AngUnitsToRad_fromUnsignedShort(
            (unsigned short)(int)(10430.378f * nlATan2f(localDirection.z, 1.0f)));
        if (nlAbs(verticalAngle) > DegreesToRadians(180.0f))
            verticalAngle = DegreesToRadians(360.0f) - verticalAngle;
        if (localDirection.z > 0.0f)
            verticalAngle *= -1.0f;
        if (nlAbs(verticalAngle) > maximumAngle)
            verticalAngle = 0.0f;

        horizontalAngle = LimitMagnitude(horizontalAngle, horizontalLimit);
        verticalAngle = LimitMagnitude(verticalAngle, verticalLimit);

        if (horizontalAngle != 0.0f)
        {
            nlVector3 unrotatedDirection = direction;
            nlQuaternion horizontalRotation;
            nlMakeQuat(horizontalRotation, cameraUp, horizontalAngle);
            RotateVector(direction, unrotatedDirection, horizontalRotation);
        }

        if (verticalAngle != 0.0f)
        {
            nlVec3CrossProduct(side, direction, mUpVector);
            nlVector3 unrotatedDirection = direction;
            nlQuaternion verticalRotation;
            nlMakeQuat(verticalRotation, side, verticalAngle);
            RotateVector(direction, unrotatedDirection, verticalRotation);
        }

        nlVector3 scaledDirection;
        nlVec3Scale(scaledDirection, direction, distance);
        nlVec3Add(result, scaledDirection, position);
    }

    return result;
}

/**
 * Offset/Address/Size: 0x26D8 | 0x800F6B40 | size: 0x310
 */
nlVector3 ReplayCamera::GetFocusPosition(int focus) const
{
    RenderSnapshot* render = ReplayManager::Instance()->mRender;
    nlVector3 result = { 0.0f, 0.0f, 0.0f };

    switch (focus)
    {
    case 0:
        result = render->mBall.mPosition;
        result.z += 0.35f;
        break;
    case 3:
    {
        cCharacter* goalie = mSideOfInterest == 0 ? g_pCharacters[8] : g_pCharacters[9];
        result = goalie->mUnidentified024.m_v3Position;
        result.z = 1.0f;
        break;
    }
    case 1:
    {
        DrawableCharacter* player = render->mBall.IndexToPlayer(render->mBall.mFlags.bits.ownerIndex);
        if (player != NULL && player->character != NULL
            && player->character->m_eClassType == FIELDER)
        {
            nlVector3 bip01Pos = player->position;
            float height = player->height;
            bip01Pos.z += height + 1.0f;
            result = bip01Pos;
        }
        else
        {
            player = render->mBall.IndexToPlayer(render->mBall.mFlags.bits.previousOwnerIndex);
            if (player != NULL && player->character != NULL
                && player->character->m_eClassType == FIELDER)
            {
                nlVector3 bip01Pos = player->position;
                float height = player->height;
                bip01Pos.z += height + 1.0f;
                result = bip01Pos;
            }
            else
            {
                result = render->mBall.mPosition;
            }
        }
        break;
    }
    case 2:
    {
        nlVector3 netPos = { 0.0f, 0.0f, 1.0f };
        netPos.x = cField::GetGoalLineX(GetSideDirection(mSideOfInterest));
        if (netPos.x > 0.0f)
            netPos.x += 3.0f;
        else
            netPos.x -= 3.0f;
        result = netPos;
        break;
    }
    case 4:
    {
        cPlayer* goalie = (cPlayer*)(mSideOfInterest == 0 ? g_pCharacters[8] : g_pCharacters[9]);
        cFielder* captain = goalie->m_pTeam->GetCaptain();
        result = captain->mUnidentified024.m_v3Position;
        result.z = 1.0f;
        break;
    }
    case 5:
        nlVec3Set(result, 0.0f, 0.0f, 0.0f);
        break;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x23C8 | 0x800F6E50 | size: 0x8
 */
void ReplayCamera::SetSideOfInterest(int side)
{
    mSideOfInterest = side;
}

/**
 * Offset/Address/Size: 0x23C0 | 0x800F6E58 | size: 0xA0
 */
void ReplayCamera::CutTo(ReplayCameraPosition position)
{
    mCamPos = position;
    mFrozen = false;
    mPositionFrozen = false;
    mPosition = GetPosition(mCamPos, -1.0f);
    mFov = GetFov(mCamPos);
    mNoDampenLookAtForOneUpdate = true;
    mNoDampenForOneUpdate = false;
    mUsePositionLimits = false;
    nlVec3Set(mPositionOffset, 0.0f, 0.0f, 0.0f);
    mBallToGoalRotationDegrees = 0.0f;
}

/**
 * Offset/Address/Size: 0x2320 | 0x800F6EF8 | size: 0x2C
 */
void ReplayCamera::MoveTo(ReplayCameraPosition position)
{
    mFrozen = false;
    mPositionFrozen = false;
    mCamPos = position;
    mUsePositionLimits = false;
    nlVec3Set(mPositionOffset, 0.0f, 0.0f, 0.0f);
    mBallToGoalRotationDegrees = 0.0f;
}

/**
 * Offset/Address/Size: 0x22F4 | 0x800F6F24 | size: 0xEB8
 */
float ReplayCamera::GetFov(ReplayCameraPosition position) const
{
    switch (position)
    {
    case REPLAY_CAMERA_POSITION_INSIDE_NET:
        return GetConfigFloat(Config::Global(), "replay/camera_inside_net_fov", 50.0f);
    case REPLAY_CAMERA_POSITION_HIGH_UP:
        return GetConfigFloat(Config::Global(), "replay/camera_high_up_fov", 50.0f);
    default:
        if (position >= REPLAY_CAMERA_POSITION_GENERIC_0 && position <= REPLAY_CAMERA_POSITION_GENERIC_LAST)
        {
            BasicString<char, Detail::TempStringAllocator> prefix("replay/camera_");
            {
                BasicString<char, Detail::TempStringAllocator> formatStr("generic_{0}_fov");
                int idx = position - REPLAY_CAMERA_POSITION_GENERIC_0;
                prefix.AppendInPlace(Format(formatStr, idx));
            }
            float fov = GetConfigFloat(Config::Global(), prefix.c_str(), 50.0f);
            return fov;
        }
        return 27.0f;
    }
}

/**
 * Offset/Address/Size: 0x143C | 0x800F7DDC | size: 0x1144
 */
nlVector3 ReplayCamera::GetPosition(ReplayCameraPosition position, float direction) const
{
    nlVector3 ret = { 0.0f, 0.0f, 0.0f };
    float goalLineX = cField::GetGoalLineX(direction);
    float sideLineY = cField::GetSidelineY(1);

    switch (position)
    {
    case REPLAY_CAMERA_POSITION_INSIDE_NET:
    {
        float x = GetConfigFloat(Config::Global(), "replay/camera_inside_net_x", 7.0f);
        float y = GetConfigFloat(Config::Global(), "replay/camera_inside_net_y", 8.0f);
        float z = GetConfigFloat(Config::Global(), "replay/camera_inside_net_z", 2.0f);
        ret.x = cField::GetGoalLineX(direction) + direction * x;
        ret.y = y;
        ret.z = z;
        break;
    }
    case REPLAY_CAMERA_POSITION_SIDELINE:
        ret = ReplayManager::Instance()->mRender->mBall.mPosition;
        ret.x *= 0.0f;
        ret.y = cField::GetSidelineY(0);
        ret.z = 0.0f;
        break;
    case REPLAY_CAMERA_POSITION_BALL_TO_GOAL:
    {
        nlVector3 ballPos = ReplayManager::Instance()->mRender->mBall.mPosition;
        nlVector3 goalPos = { 0.0f, 0.0f, 0.0f };
        goalPos.x = 30.0f * direction + goalLineX;
        nlVector3 ballToGoal;
        nlVec3Sub(ballToGoal, goalPos, ballPos);
        nlVec3Scale(ballToGoal, nlRecipSqrt(ballToGoal.GetLengthSq3D(), false));

        nlQuaternion rotation;
        nlVector3 rotationAxis = { 0.0f, 0.0f, 1.0f };
        fn_802B5370(rotation, rotationAxis,
            DegreesToAngle(mBallToGoalRotationDegrees));
        RotateVector(ballToGoal, ballToGoal, rotation);

        nlVec3Scale(ballToGoal, -GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_behind_dist", 16.0f));
        nlVec3Add(ret, ballPos, ballToGoal);
        float minHeight = GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_min_height", 3.0f);
        if (ret.z < minHeight)
            ret.z = minHeight;

        float minDistToGoal = GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_min_dist_to_goal", 8.0f);
        if (nlAbs(goalPos.x - ret.x) < minDistToGoal)
            ret.x = goalPos.x - direction * minDistToGoal;

        ret.y += GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_y_offset", 0.0f);
        break;
    }
    case REPLAY_CAMERA_POSITION_HIGH_UP:
    {
        float highX = GetConfigFloat(Config::Global(), "replay/camera_high_up_x", -6.0f);
        float highY = GetConfigFloat(Config::Global(), "replay/camera_high_up_y", -5.0f);
        float highZ = GetConfigFloat(Config::Global(), "replay/camera_high_up_z", 8.0f);
        float minDistBehind = GetConfigFloat(Config::Global(), "replay/camera_high_up_min_dist_behind", 8.0f);

        ret.x = highX * GetSideDirection(mSideOfInterest);
        ret.y = highY;
        ret.z = highZ;
        if (nlAbs(ret.x - mLookAt.x) < minDistBehind)
            ret.x = mLookAt.x - minDistBehind * GetSideDirection(mSideOfInterest);
        break;
    }
    default:
        if (position >= REPLAY_CAMERA_POSITION_GENERIC_0 && position <= REPLAY_CAMERA_POSITION_GENERIC_LAST)
        {
            char key[256];
            nlSNPrintf(key, sizeof(key), "replay/camera_generic_%d_x", position - REPLAY_CAMERA_POSITION_GENERIC_0);
            float xVal = GetConfigFloat(Config::Global(), key, 0.0f) * GetSideDirection(mSideOfInterest);
            nlSNPrintf(key, sizeof(key), "replay/camera_generic_%d_y", position - REPLAY_CAMERA_POSITION_GENERIC_0);
            float yVal = GetConfigFloat(Config::Global(), key, 0.0f);
            nlSNPrintf(key, sizeof(key), "replay/camera_generic_%d_z", position - REPLAY_CAMERA_POSITION_GENERIC_0);
            float zVal = GetConfigFloat(Config::Global(), key, 0.0f);
            ret.x = xVal;
            ret.y = yVal;
            ret.z = zVal;
        }
        break;
    }

    ret.x += mPositionOffset.x;
    ret.y += mPositionOffset.y;
    ret.z += mPositionOffset.z;

    nlVector3 max;
    max.x = GetConfigFloat(Config::Global(), "replay/camera_max_behind_goal_line", 2.0f);
    max.y = GetConfigFloat(Config::Global(), "replay/camera_max_beyond_side_line", 2.0f);
    max.z = GetConfigFloat(Config::Global(), "replay/camera_max_height", 20.0f);
    if (mUsePositionLimits == true)
    {
        max.x = mMaxBehindGoalLine;
        max.y = mMaxBeyondSideLine;
        max.z = mMaxHeight;
    }

    float minZ = GetConfigFloat(Config::Global(), "replay/camera_min_height", 0.5f);

    if (ret.z > max.z)
        ret.z = max.z;
    if (ret.z < minZ)
        ret.z = minZ;
    if (ret.x < -fabsf(goalLineX) - max.x)
        ret.x = -fabsf(goalLineX) - max.x;
    if (ret.x > max.x + fabsf(goalLineX))
        ret.x = max.x + fabsf(goalLineX);
    if (ret.y < -sideLineY - max.y)
        ret.y = -sideLineY - max.y;
    if (ret.y > sideLineY + max.y)
        ret.y = sideLineY + max.y;

    return ret;
}

/**
 * Offset/Address/Size: 0x2F8 | 0x800F8F20 | size: 0xC
 */
void ReplayCamera::SetLookAtDampingTime(const float& dampingTime)
{
    gReplayCameraLookAtDampingTime = dampingTime;
}

/**
 * Offset/Address/Size: 0x2EC | 0x800F8F2C | size: 0xC
 */
void ReplayCamera::SetPositionDampingTime(const float& dampingTime)
{
    gReplayCameraPositionDampingTime = dampingTime;
}

/**
 * Offset/Address/Size: 0x2E0 | 0x800F8F38 | size: 0x1C
 */
void ReplayCamera::SetPositionOffset(const nlVector3& offset)
{
    mPositionOffset = offset;
}

/**
 * Offset/Address/Size: 0x2C4 | 0x800F8F54 | size: 0x28
 */
void ReplayCamera::SetAutoFov(float minFov, float maxFov, float minDistance, float maxDistance, float maxChangeRate)
{
    mAutoFov = true;
    mDeltaFov = 0.0f;
    mAutoFovMin = minFov;
    mAutoFovMax = maxFov;
    mAutoFovMinDistance = minDistance;
    mAutoFovMaxDistance = maxDistance;
    mAutoFovMaxChangeRate = maxChangeRate;
}

/**
 * Offset/Address/Size: 0x29C | 0x800F8F7C | size: 0x18
 */
void ReplayCamera::SetPositionLimits(float maxBehindGoalLine, float maxBeyondSideLine, float maxHeight)
{
    mUsePositionLimits = true;
    mMaxBehindGoalLine = maxBehindGoalLine;
    mMaxBeyondSideLine = maxBeyondSideLine;
    mMaxHeight = maxHeight;
}
