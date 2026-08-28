#ifndef GAME_CAMERA_ANIMCAM_H
#define GAME_CAMERA_ANIMCAM_H

#include "Game/Camera/BaseCamera.h"
#include "Game/SAnim.h"

class cCameraData
{
public:
    cCameraData();
    ~cCameraData();

    /* 0x00 */ cCameraData* next;
    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ unsigned long m_uKeyCount;
    /* 0x0C */ char* field_0x0C;
    /* 0x10 */ nlVector3* cameraPos;
    /* 0x14 */ nlVector3* targetPos;
    /* 0x18 */ nlQuaternion* cameraRot;
    /* 0x1C */ float* fFOV;
    /* 0x20 */ float* fFocalLength;
    /* 0x24 */ bool ownsKeyData;
}; // total size: 0x28

class cAnimCamera : public cBaseCamera
{
public:
    cAnimCamera();
    virtual ~cAnimCamera();

    virtual eCameraType GetType();
    virtual void Update(float dt);
    virtual const nlMatrix4& GetViewMatrix() const;
    virtual float GetFOV() const;
    virtual const nlVector3& GetTargetPosition() const;
    virtual const nlVector3& GetCameraPosition() const;
    virtual float GetFocalLength() const;

    static bool LoadCameraAnimation(nlChunk* begin, unsigned long, const char* cameraName, bool ownsKeyData);
    static void FreeCameraAnimations();

    void SetAnimationTime(float fTime, bool bUpdateView);
    void BuildAnimViewMatrix(nlMatrix4& mView);
    void UnselectCameraAnimation();
    void SelectCameraAnimation(const char* name);
    float ManualUpdate(float dt);

    static cCameraData* m_cameraDataList;

    /* 0x20 */ bool m_bCyclic;
    /* 0x21 */ bool m_bUseSimulationTime;
    /* 0x22 */ bool m_LetManagerDoUpdate;
    /* 0x23 */ bool m_bUnusedPad;
    /* 0x24 */ nlMatrix4 m_matView;
    /* 0x64 */ nlVector3 m_vecCamera;
    /* 0x70 */ nlVector3 m_vecTarget;
    /* 0x7C */ float m_fAnimationTime;
    /* 0x80 */ float m_fAnimationSpeed;
    /* 0x84 */ float m_fLastSimulationTime;
    /* 0x88 */ cCameraData* m_pActiveCameraData;
    /* 0x8C */ nlVector3 m_OffsetPos;
    /* 0x98 */ nlVector3 m_Mirror;
    /* 0xA4 */ unsigned short mFacingAngle;
    /* 0xA8 */ float m_Fov;
    /* 0xAC */ float m_FocalLength;
    /* 0xB0 */ void (*m_EndOfAnimationCallback)();
}; // total size: 0xB4

inline eCameraType cAnimCamera::GetType()
{
    return eCameraType_Animated;
}

inline const nlVector3& cAnimCamera::GetTargetPosition() const
{
    return m_vecTarget;
}

inline const nlVector3& cAnimCamera::GetCameraPosition() const
{
    return m_vecCamera;
}

inline float cAnimCamera::GetFOV() const
{
    return m_Fov;
}

inline float cAnimCamera::GetFocalLength() const
{
    return m_FocalLength;
}

inline const nlMatrix4& cAnimCamera::GetViewMatrix() const
{
    return m_matView;
}

#endif // GAME_CAMERA_ANIMCAM_H
