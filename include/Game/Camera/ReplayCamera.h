#ifndef GAME_CAMERA_REPLAY_CAMERA_H
#define GAME_CAMERA_REPLAY_CAMERA_H

#include "Game/Camera/BaseCamera.h"

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
    REPLAY_CAMERA_POSITION_GENERIC_LAST = 20,
    REPLAY_CAMERA_POSITION_NUM_POSITIONS = 21,
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
    void ManualUpdate(float deltaTime);
    void SetSideOfInterest(int sideOfInterest);
    void CutTo(ReplayCameraPosition camPos);
    void fn_800F6EF8(ReplayCameraPosition camPos);
    float GetFov(ReplayCameraPosition position) const;
    nlVector3 GetPosition(ReplayCameraPosition position, float direction) const;

    void fn_800F8F20(const float& value);
    void fn_800F8F2C(const float& value);
    void fn_800F8F38(const nlVector3& value);
    void fn_800F8F54(float value0, float value1, float value2, float value3, float value4);
    void fn_800F8F7C(float value0, float value1, float value2);

private:
    nlVector3 fn_800F63F8(const nlVector3& position, const nlVector3& lookAt,
        const nlVector3& previousLookAt, unsigned int width, unsigned int height,
        float fov) const;
    nlVector3 fn_800F6B40(int focus) const;

public:
    /* 0x020 */ float mDeltaFov;
    /* 0x024 */ float mUnidentified024;
    /* 0x028 */ float mFov;
    /* 0x02C */ int mSideOfInterest;
    /* 0x030 */ nlVector3 mUnidentified030;
    /* 0x03C */ nlVector3 mUnidentified03C;
    /* 0x048 */ nlVector3 mUnidentified048;
    /* 0x054 */ nlVector3 mUnidentified054;
    /* 0x060 */ bool mNoDampenForOneUpdate;
    /* 0x061 */ bool mUnidentified061;
    /* 0x062 */ bool mFrozen;
    /* 0x063 */ bool mUnidentified063;
    /* 0x064 */ int mFocus;
    /* 0x068 */ int mUnidentified068;
    /* 0x06C */ ReplayCameraPosition mCamPos;
    /* 0x070 */ nlVector3 mPosition;
    /* 0x07C */ nlVector3 mLookAt;
    /* 0x088 */ nlMatrix4 mViewMatrix;
    /* 0x0C8 */ nlVector3 mUnidentified0C8;
    /* 0x0D4 */ bool mUnidentified0D4;
    /* 0x0D5 */ u8 mPadding0D5[3];
    /* 0x0D8 */ float mUnidentified0D8;
    /* 0x0DC */ float mUnidentified0DC;
    /* 0x0E0 */ float mUnidentified0E0;
    /* 0x0E4 */ float mUnidentified0E4;
    /* 0x0E8 */ float mUnidentified0E8;
    /* 0x0EC */ bool mUnidentified0EC;
    /* 0x0ED */ u8 mPadding0ED[3];
    /* 0x0F0 */ float mUnidentified0F0;
    /* 0x0F4 */ float mUnidentified0F4;
    /* 0x0F8 */ float mUnidentified0F8;
    /* 0x0FC */ float mUnidentified0FC;
}; // total size: 0x100

#endif // GAME_CAMERA_REPLAY_CAMERA_H
