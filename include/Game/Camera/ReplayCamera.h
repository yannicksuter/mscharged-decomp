#ifndef GAME_CAMERA_REPLAY_CAMERA_H
#define GAME_CAMERA_REPLAY_CAMERA_H

#include "Game/Camera/BaseCam.h"

extern float gMatrixEffectCameraDistance[2];
extern u8 gMatrixEffectCameraFrozen[8];

enum ReplayCameraPosition
{
    REPLAY_CAMERA_POSITION_INSIDE_NET = 0,
    REPLAY_CAMERA_POSITION_SIDELINE = 1,
    REPLAY_CAMERA_POSITION_BALL_TO_GOAL = 2,
    REPLAY_CAMERA_POSITION_HIGH_UP = 3,
    REPLAY_CAMERA_POSITION_GENERIC_0 = 4,
    REPLAY_CAMERA_POSITION_GENERIC_1 = 5,
    REPLAY_CAMERA_POSITION_GENERIC_2 = 6,
    REPLAY_CAMERA_POSITION_GENERIC_3 = 7,
    REPLAY_CAMERA_POSITION_GENERIC_4 = 8,
    REPLAY_CAMERA_POSITION_GENERIC_5 = 9,
    REPLAY_CAMERA_POSITION_GENERIC_6 = 10,
    REPLAY_CAMERA_POSITION_GENERIC_7 = 11,
    REPLAY_CAMERA_POSITION_GENERIC_8 = 12,
    REPLAY_CAMERA_POSITION_GENERIC_9 = 13,
    REPLAY_CAMERA_POSITION_GENERIC_10 = 14,
    REPLAY_CAMERA_POSITION_GENERIC_11 = 15,
    REPLAY_CAMERA_POSITION_GENERIC_12 = 16,
    REPLAY_CAMERA_POSITION_GENERIC_13 = 17,
    REPLAY_CAMERA_POSITION_GENERIC_14 = 18,
    REPLAY_CAMERA_POSITION_GENERIC_15 = 19,
    REPLAY_CAMERA_POSITION_GENERIC_LAST = 55,
    REPLAY_CAMERA_POSITION_NUM_POSITIONS = 56,
};

class ReplayCamera : public cBaseCamera
{
public:
    ReplayCamera();
    virtual ~ReplayCamera() { }

    virtual eCameraType GetType() { return eCameraType_Replay; }
    virtual void Update(float fDeltaT);
    virtual const nlMatrix4& GetViewMatrix() const;
    virtual float GetFOV() const;
    virtual const nlVector3& GetTargetPosition() const { return mLookAt; }
    virtual const nlVector3& GetCameraPosition() const { return mPosition; }

    static void UpdateTweakMode();
    void ManualUpdate(float deltaT);
    void SetSideOfInterest(int side);
    void CutTo(ReplayCameraPosition position);
    void MoveTo(ReplayCameraPosition position);
    float GetFov(ReplayCameraPosition position) const;
    nlVector3 GetPosition(ReplayCameraPosition position, float direction) const;

    void SetLookAtDampingTime(const float& dampingTime);
    void SetPositionDampingTime(const float& dampingTime);
    void SetPositionOffset(const nlVector3& offset);
    void SetAutoFov(float minFov, float maxFov, float minDistance, float maxDistance, float maxChangeRate);
    void SetPositionLimits(float maxBehindGoalLine, float maxBeyondSideLine, float maxHeight);

private:
    nlVector3 GetClampedFocusPosition(const nlVector3& position, const nlVector3& lookAt,
        const nlVector3& secondaryLookAt, unsigned int width, unsigned int height,
        float fov) const;
    nlVector3 GetFocusPosition(int focus) const;

public:
    /* 0x020 */ float mDeltaFov;
    /* 0x024 */ float mTargetFov;
    /* 0x028 */ float mFov;
    /* 0x02C */ int mSideOfInterest;
    /* 0x030 */ nlVector3 mPositionVelocity;
    /* 0x03C */ nlVector3 mLookAtVelocity;
    /* 0x048 */ nlVector3 mLookAtOffsetVelocity;
    /* 0x054 */ nlVector3 mLookAtOffset;
    /* 0x060 */ bool mNoDampenForOneUpdate;
    /* 0x061 */ bool mNoDampenLookAtForOneUpdate;
    /* 0x062 */ bool mFrozen;
    /* 0x063 */ bool mPositionFrozen;
    /* 0x064 */ int mFocus;
    /* 0x068 */ int mSecondaryFocus;
    /* 0x06C */ ReplayCameraPosition mCamPos;
    /* 0x070 */ nlVector3 mPosition;
    /* 0x07C */ nlVector3 mLookAt;
    /* 0x088 */ mutable nlMatrix4 mViewMatrix;
    /* 0x0C8 */ nlVector3 mPositionOffset;
    /* 0x0D4 */ bool mAutoFov;
    /* 0x0D5 */ u8 mPadding0D5[3];
    /* 0x0D8 */ float mAutoFovMin;
    /* 0x0DC */ float mAutoFovMax;
    /* 0x0E0 */ float mAutoFovMinDistance;
    /* 0x0E4 */ float mAutoFovMaxDistance;
    /* 0x0E8 */ float mAutoFovMaxChangeRate;
    /* 0x0EC */ bool mUsePositionLimits;
    /* 0x0ED */ u8 mPadding0ED[3];
    /* 0x0F0 */ float mMaxBehindGoalLine;
    /* 0x0F4 */ float mMaxBeyondSideLine;
    /* 0x0F8 */ float mMaxHeight;
    /* 0x0FC */ float mBallToGoalRotationDegrees;
}; // total size: 0x100

#endif // GAME_CAMERA_REPLAY_CAMERA_H
