#ifndef GAME_CAMERA_FOLLOW_CAM_H
#define GAME_CAMERA_FOLLOW_CAM_H

#include "Game/Camera/BaseCamera.h"

class cFollowCamera : public cBaseCamera
{
public:
    enum FollowTarget
    {
        FOLLOW_BALL = 0,
        FOLLOW_CHARACTER = 1,
        FOLLOW_SELECTABLE = 2,
        FOLLOW_ANIM_VIEWER_CHARACTER = 3,
    };

    cFollowCamera();
    cFollowCamera(cFollowCamera::FollowTarget followTarget);
    virtual ~cFollowCamera() { }

    virtual eCameraType GetType();
    virtual void Update(float fDeltaT);
    virtual const nlMatrix4& GetViewMatrix() const;
    virtual const nlVector3& GetTargetPosition() const;
    virtual const nlVector3& GetCameraPosition() const;

    /* 0x20 */ FollowTarget m_FollowTarget;
    /* 0x24 */ nlMatrix4 m_matView;
    /* 0x64 */ nlVector3 m_v3CameraPosition;
    /* 0x70 */ nlVector3 m_v3OOI;
    /* 0x7C */ nlVector3 m_v3OOIDampened;
    /* 0x88 */ nlVector3 m_v3OOIDampenedPrev;
    /* 0x94 */ bool m_bOOISet;
    /* 0x96 */ u16 m_aFacingDirection;
    /* 0x98 */ u16 m_aPitch;
    /* 0x9C */ float m_fOOIDistance;
    /* 0xA0 */ bool m_bPitchLimits;
    /* 0xA1 */ bool m_bControlsLocked;
};

#endif // GAME_CAMERA_FOLLOW_CAM_H
