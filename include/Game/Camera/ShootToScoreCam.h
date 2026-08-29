#ifndef _SHOOTTOSCORECAM_H_
#define _SHOOTTOSCORECAM_H_

#include "Game/Camera/BaseCamera.h"

class cShootToScoreCamera : public cBaseCamera
{
public:
    cShootToScoreCamera();
    virtual ~cShootToScoreCamera();
    virtual void Update(float fDeltaT);
    virtual const nlMatrix4& GetViewMatrix() const { return m_matView; }
    virtual float GetFOV() const { return m_fFOV; }
    virtual eCameraType GetType() { return eCameraType_ShootToScore; }
    virtual const nlVector3& GetTargetPosition() const { return m_v3Target; }
    virtual const nlVector3& GetCameraPosition() const { return m_v3Camera; }

    void fn_800F93F0(float side);

    /* 0x20 */ nlMatrix4 m_matView;
    /* 0x60 */ nlVector3 m_v3Camera;
    /* 0x6C */ nlVector3 m_v3Target;
    /* 0x78 */ float m_fFOV;
}; // total size: 0x7C

#endif // _SHOOTTOSCORECAM_H_
