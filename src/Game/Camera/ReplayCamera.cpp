#include "Game/Camera/ReplayCamera.h"

#include "Game/AI/AiUtil.h"
#include "Game/CharacterTemplate.h"
#include "Game/Field.h"
#include "Game/ReplayManager.h"
#include "Game/Render/depthoffield.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "NL/nlPrint.h"
#include "NL/nlTask.h"
#include "NL/gl/glMatrix.h"

extern "C" bool fn_80273B00();
extern "C" float fn_800F2410(float fov);

float lbl_806DC510[2] = { 18.0f, 0.0f };
float lbl_806DC518 = 0.25f;
float lbl_806DC51C = 0.25f;
float lbl_806DC520 = 0.5f;
float lbl_806DC524 = 4.0f;
float lbl_806DC528 = 45.0f;
float lbl_806DC52C = 1.0f;
float lbl_806DC530 = 12.0f;
float lbl_806DC534 = 1.25f;
float lbl_806DC538[2] = { 1.666f, 0.0f };

u8 lbl_806E0F18[8];

static inline float GetSideDirection(int side)
{
    return side == 0 ? -1.0f : 1.0f;
}

/**
 * Offset/Address/Size: 0x29C | 0x800F8F7C | size: 0x18
 */
void ReplayCamera::fn_800F8F7C(float value0, float value1, float value2)
{
    mUnidentified0EC = true;
    mUnidentified0F0 = value0;
    mUnidentified0F4 = value1;
    mUnidentified0F8 = value2;
}

/**
 * Offset/Address/Size: 0x2C4 | 0x800F8F54 | size: 0x28
 */
void ReplayCamera::fn_800F8F54(float value0, float value1, float value2, float value3, float value4)
{
    mUnidentified0D4 = true;
    mDeltaFov = 0.0f;
    mUnidentified0D8 = value0;
    mUnidentified0DC = value1;
    mUnidentified0E0 = value2;
    mUnidentified0E4 = value3;
    mUnidentified0E8 = value4;
}

/**
 * Offset/Address/Size: 0x2E0 | 0x800F8F38 | size: 0x1C
 */
void ReplayCamera::fn_800F8F38(const nlVector3& value)
{
    mUnidentified0C8 = value;
}

/**
 * Offset/Address/Size: 0x2EC | 0x800F8F2C | size: 0xC
 */
void ReplayCamera::fn_800F8F2C(const float& value)
{
    lbl_806DC518 = value;
}

/**
 * Offset/Address/Size: 0x2F8 | 0x800F8F20 | size: 0xC
 */
void ReplayCamera::fn_800F8F20(const float& value)
{
    lbl_806DC51C = value;
}

/**
 * Offset/Address/Size: 0x143C | 0x800F7DDC | size: 0x1144
 */
nlVector3 ReplayCamera::GetPosition(ReplayCameraPosition position, float direction) const
{
    nlVector3 result = { 0.0f, 0.0f, 0.0f };
    float goalLineX = cField::GetGoalLineX(direction);
    float sidelineY = cField::GetSidelineY(1);

    switch (position)
    {
    case REPLAY_CAMERA_POSITION_INSIDE_NET:
    {
        float x = GetConfigFloat(Config::Global(), "replay/camera_inside_net_x", 7.0f);
        float y = GetConfigFloat(Config::Global(), "replay/camera_inside_net_y", 8.0f);
        float z = GetConfigFloat(Config::Global(), "replay/camera_inside_net_z", 2.0f);
        result.x = cField::GetGoalLineX(direction) + direction * x;
        result.y = y;
        result.z = z;
        break;
    }
    case REPLAY_CAMERA_POSITION_SIDELINE:
        result = ReplayManager::Instance()->mRender->mBall.mPosition;
        result.x *= 0.8f;
        result.y = cField::GetSidelineY(0) - 5.0f;
        result.z = 2.0f;
        break;
    case REPLAY_CAMERA_POSITION_BALL_TO_GOAL:
    {
        nlVector3 ballPos = ReplayManager::Instance()->mRender->mBall.mPosition;
        nlVector3 goalPos = { 30.0f * direction + goalLineX, 0.0f, 0.0f };
        nlVector3 ballToGoal;
        nlVec3Sub(ballToGoal, goalPos, ballPos);
        nlVec3Scale(ballToGoal, nlRecipSqrt(ballToGoal.GetLengthSq3D(), false));

        float behindDist = GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_behind_dist", 16.0f);
        result.x = ballPos.x - behindDist * ballToGoal.x;
        result.y = ballPos.y - behindDist * ballToGoal.y;
        result.z = ballPos.z - behindDist * ballToGoal.z;
        float minHeight = GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_min_height", 3.0f);
        if (result.z < minHeight)
            result.z = minHeight;

        float minDistToGoal = GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_min_dist_to_goal", 8.0f);
        if (nlAbs(goalPos.x - result.x) < minDistToGoal)
            result.x = goalPos.x - direction * minDistToGoal;

        result.y += GetConfigFloat(Config::Global(), "replay/camera_ball_to_goal_y_offset", 0.0f);
        break;
    }
    case REPLAY_CAMERA_POSITION_HIGH_UP:
    {
        float highX = GetConfigFloat(Config::Global(), "replay/camera_high_up_x", -6.0f);
        float highY = GetConfigFloat(Config::Global(), "replay/camera_high_up_y", 0.0f);
        float highZ = GetConfigFloat(Config::Global(), "replay/camera_high_up_z", 8.0f);
        float minDistBehind = GetConfigFloat(Config::Global(), "replay/camera_high_up_min_dist_behind", 8.0f);

        result.x = highX * GetSideDirection(mSideOfInterest);
        result.y = highY;
        result.z = highZ;
        if (nlAbs(result.x - mLookAt.x) < minDistBehind)
            result.x = mLookAt.x - minDistBehind * GetSideDirection(mSideOfInterest);
        break;
    }
    default:
        if (position >= REPLAY_CAMERA_POSITION_GENERIC_0 && position <= REPLAY_CAMERA_POSITION_GENERIC_LAST)
        {
            int index = position - REPLAY_CAMERA_POSITION_GENERIC_0;
            char key[64];
            nlSNPrintf(key, sizeof(key), "replay/camera_generic_%d_x", index);
            result.x = GetConfigFloat(Config::Global(), key, 0.0f) * GetSideDirection(mSideOfInterest);
            nlSNPrintf(key, sizeof(key), "replay/camera_generic_%d_y", index);
            result.y = GetConfigFloat(Config::Global(), key, 0.0f);
            nlSNPrintf(key, sizeof(key), "replay/camera_generic_%d_z", index);
            result.z = GetConfigFloat(Config::Global(), key, 0.0f);
        }
        break;
    }

    result.x += mUnidentified0C8.x;
    result.y += mUnidentified0C8.y;
    result.z += mUnidentified0C8.z;

    nlVector3 limits;
    limits.x = GetConfigFloat(Config::Global(), "replay/camera_max_behind_goal_line", 2.0f);
    limits.y = GetConfigFloat(Config::Global(), "replay/camera_max_beyond_side_line", 2.0f);
    limits.z = GetConfigFloat(Config::Global(), "replay/camera_max_height", 20.0f);
    float minZ = GetConfigFloat(Config::Global(), "replay/camera_min_height", 0.5f);

    if (mUnidentified0EC)
    {
        limits.x = mUnidentified0F0;
        limits.y = mUnidentified0F4;
        limits.z = mUnidentified0F8;
    }

    if (result.z > limits.z)
        result.z = limits.z;
    if (result.z < minZ)
        result.z = minZ;
    if (result.x < -nlAbs(goalLineX) - limits.x)
        result.x = -nlAbs(goalLineX) - limits.x;
    if (result.x > limits.x + nlAbs(goalLineX))
        result.x = limits.x + nlAbs(goalLineX);
    if (result.y < -sidelineY - limits.y)
        result.y = -sidelineY - limits.y;
    if (result.y > sidelineY + limits.y)
        result.y = sidelineY + limits.y;

    return result;
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
            BasicString<char, Detail::TempStringAllocator> formatString("generic_{0}_fov");
            int index = position - REPLAY_CAMERA_POSITION_GENERIC_0;
            prefix.AppendInPlace(Format(formatString, index));
            return GetConfigFloat(Config::Global(), prefix.c_str(), 50.0f);
        }
        return 27.0f;
    }
}

/**
 * Offset/Address/Size: 0x2320 | 0x800F6EF8 | size: 0x2C
 */
void ReplayCamera::fn_800F6EF8(ReplayCameraPosition camPos)
{
    mFrozen = false;
    mUnidentified063 = false;
    mCamPos = camPos;
    mUnidentified0EC = false;
    nlVec3Set(mUnidentified0C8, 0.0f, 0.0f, 0.0f);
    mUnidentified0FC = 0.0f;
}

/**
 * Offset/Address/Size: 0x23C0 | 0x800F6E58 | size: 0xA0
 */
void ReplayCamera::CutTo(ReplayCameraPosition camPos)
{
    mCamPos = camPos;
    mFrozen = false;
    mUnidentified063 = false;
    mPosition = GetPosition(mCamPos, -1.0f);
    mFov = GetFov(mCamPos);
    mUnidentified061 = true;
    mNoDampenForOneUpdate = false;
    mUnidentified0EC = false;
    nlVec3Set(mUnidentified0C8, 0.0f, 0.0f, 0.0f);
    mUnidentified0FC = 0.0f;
}

/**
 * Offset/Address/Size: 0x23C8 | 0x800F6E50 | size: 0x8
 */
void ReplayCamera::SetSideOfInterest(int sideOfInterest)
{
    mSideOfInterest = sideOfInterest;
}

/**
 * Offset/Address/Size: 0x26D8 | 0x800F6B40 | size: 0x310
 */
nlVector3 ReplayCamera::fn_800F6B40(int focus) const
{
    RenderSnapshot* render = ReplayManager::Instance()->mRender;
    nlVector3 result = { 0.0f, 0.0f, 0.0f };

    switch (focus)
    {
    case 0:
        result = render->mBall.mPosition;
        result.z += 0.48f;
        break;
    case 1:
    {
        DrawableCharacter* player = render->mBall.IndexToPlayer(render->mBall.mFlags.bits.ownerIndex);
        if (player == NULL)
        {
            player = render->mBall.IndexToPlayer(render->mBall.mFlags.bits.previousOwnerIndex);
        }
        if (player != NULL)
        {
            result = player->position;
            result.z += player->height;
        }
        else
        {
            result = render->mBall.mPosition;
        }
        break;
    }
    case 2:
        result.x = cField::GetGoalLineX(GetSideDirection(mSideOfInterest));
        result.x += result.x > 0.0f ? 10.0f : -10.0f;
        break;
    case 3:
    {
        cCharacter* goalie = g_pCharacters[mSideOfInterest == 0 ? 8 : 9];
        if (goalie != NULL)
        {
            result = goalie->m_v3Position;
            result.z = 1.0f;
        }
        break;
    }
    case 4:
    {
        cCharacter* captain = g_pCharacters[mSideOfInterest == 0 ? 0 : 5];
        if (captain != NULL)
        {
            result = captain->m_v3Position;
            result.z = 1.0f;
        }
        break;
    }
    case 5:
        break;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x2E20 | 0x800F63F8 | size: 0x748
 */
nlVector3 ReplayCamera::fn_800F63F8(const nlVector3& position,
    const nlVector3& lookAt, const nlVector3& previousLookAt,
    unsigned int width, unsigned int height, float fov) const
{
    nlVector3 result = position;
    nlVector3 previousDirection;
    nlVector3 direction;
    nlVec3Sub(previousDirection, previousLookAt, lookAt);
    nlVec3Sub(direction, position, lookAt);

    float distance = nlSqrt(direction.GetLengthSq3D(), true);
    if (distance != 0.0f && width != 0 && height != 0)
    {
        float aspect = (float)width / (float)height;
        float framing = nlSqrt(previousDirection.GetLengthSq3D(), true);
        float scale = framing / (fov * aspect);
        result.x += scale * direction.x / distance;
        result.y += scale * direction.y / distance;
        result.z += scale * direction.z / distance;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x36DC | 0x800F5B3C | size: 0x8BC
 */
void ReplayCamera::ManualUpdate(float deltaTime)
{
    ReplayManager* replayManager = ReplayManager::Instance();
    if (replayManager->mRender == NULL)
    {
        return;
    }

    if (!mFrozen)
    {
        nlVector3 lookAt = fn_800F6B40(mFocus);
        nlVector3 position = GetPosition(mCamPos, GetSideDirection(mSideOfInterest));

        if (mUnidentified063)
        {
            position = mPosition;
        }

        if (mNoDampenForOneUpdate)
        {
            mLookAt = lookAt;
            mPosition = position;
            mNoDampenForOneUpdate = false;
            mUnidentified061 = false;
        }
        else if (mUnidentified061)
        {
            mLookAt = lookAt;
            mUnidentified061 = false;
        }
        else
        {
            mPosition.x = (1.0f - lbl_806DC518) * mPosition.x + lbl_806DC518 * position.x;
            mPosition.y = (1.0f - lbl_806DC518) * mPosition.y + lbl_806DC518 * position.y;
            mPosition.z = (1.0f - lbl_806DC518) * mPosition.z + lbl_806DC518 * position.z;
            mLookAt.x = (1.0f - lbl_806DC51C) * mLookAt.x + lbl_806DC51C * lookAt.x;
            mLookAt.y = (1.0f - lbl_806DC51C) * mLookAt.y + lbl_806DC51C * lookAt.y;
            mLookAt.z = (1.0f - lbl_806DC51C) * mLookAt.z + lbl_806DC51C * lookAt.z;
        }

        if (mUnidentified0D4)
        {
            nlVector3 difference;
            nlVec3Sub(difference, lookAt, position);
            float distance = nlSqrt(difference.GetLengthSq3D(), true);
            float fov = InterpolateRangeClamped(mUnidentified0D8, mUnidentified0DC,
                mUnidentified0E0, mUnidentified0E4, distance);
            float maxChange = mUnidentified0E8 * deltaTime;
            if (nlAbs(mFov - fov) > maxChange)
            {
                fov = fov < mFov ? mFov - maxChange : mFov + maxChange;
            }
            mFov = fov;
        }
        else if (mDeltaFov != 0.0f)
        {
            if (mFov < mUnidentified024)
                mFov += deltaTime * mDeltaFov;
            else if (mFov > mUnidentified024)
                mFov -= deltaTime * mDeltaFov;

            if (nlAbs(mFov - mUnidentified024) < 2.0f * deltaTime * mDeltaFov)
                mDeltaFov = 0.0f;
        }

        mLookAt.x += mUnidentified054.x;
        mLookAt.y += mUnidentified054.y;
        mLookAt.z += mUnidentified054.z;

        if (mFov < 10.0f)
            mFov = 10.0f;
        if (mFov > 120.0f)
            mFov = 120.0f;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 8)
    {
        nlVector3 direction;
        nlVec3Sub(direction, mPosition, mLookAt);
        DepthOfFieldManager::instance.m_fDistanceFromCamera = 4.0f + nlSqrt(direction.GetLengthSq3D(), true);
    }
}

/**
 * Offset/Address/Size: 0x36E0 | 0x800F5B38 | size: 0x4
 */
void ReplayCamera::Update(float fDeltaT)
{
}

/**
 * Offset/Address/Size: 0x3738 | 0x800F5AE0 | size: 0x58
 */
float ReplayCamera::GetFOV() const
{
    float fov = mFov;
    if (fn_80273B00())
    {
        fov = fn_800F2410(mFov);
    }
    return fov;
}

/**
 * Offset/Address/Size: 0x3778 | 0x800F5AA0 | size: 0x40
 */
const nlMatrix4& ReplayCamera::GetViewMatrix() const
{
    glMatrixLookAt(*(nlMatrix4*)&mViewMatrix, mPosition, mLookAt, mUpVector);
    return mViewMatrix;
}

/**
 * Offset/Address/Size: 0x38A4 | 0x800F5974 | size: 0x12C
 */
ReplayCamera::ReplayCamera()
{
    mDeltaFov = 0.0f;
    mUnidentified024 = 0.0f;
    mFov = 50.0f;
    mSideOfInterest = 0;
    mNoDampenForOneUpdate = false;
    mUnidentified061 = false;
    mFrozen = false;
    mUnidentified063 = false;
    mFocus = 0;
    mUnidentified068 = 0;
    mCamPos = REPLAY_CAMERA_POSITION_SIDELINE;
    mUnidentified0D4 = false;
    mUnidentified0D8 = -1.0f;
    mUnidentified0DC = -1.0f;
    mUnidentified0E0 = -1.0f;
    mUnidentified0E4 = -1.0f;
    mUnidentified0E8 = 0.0f;
    mViewMatrix.SetIdentity();
    nlVec3Set(mPosition, 0.0f, 0.0f, 2.0f);
    nlVec3Set(mLookAt, 0.0f, 0.0f, 1.0f);
    nlVec3Set(mUnidentified030, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mUnidentified03C, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mUnidentified048, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mUnidentified054, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mUnidentified0C8, 0.0f, 0.0f, 0.0f);
    mUnidentified0FC = 0.0f;
}

/**
 * Offset/Address/Size: 0x38A8 | 0x800F5970 | size: 0x4
 */
void ReplayCamera::UpdateTweakMode()
{
}
