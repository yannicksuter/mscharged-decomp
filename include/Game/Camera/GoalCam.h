#ifndef _GOALCAM_H_
#define _GOALCAM_H_

#include "Game/Camera/BaseCamera.h"

class GoalCamera : public cBaseCamera
{
public:
    GoalCamera();
    virtual ~GoalCamera();
    virtual void Update(float dt);
    virtual const nlMatrix4& GetViewMatrix() const { return m_matView; }
    virtual eCameraType GetType() { return eCameraType_Goal; }
    virtual const nlVector3& GetTargetPosition() const { return m_vecTarget; }
    virtual const nlVector3& GetCameraPosition() const { return m_vecCamera; }

    /* 0x20 */ nlMatrix4 m_matView;
    /* 0x60 */ nlVector3 m_vecCamera;
    /* 0x6C */ nlVector3 m_vecTarget;
}; // total size: 0x78

#endif // _GOALCAM_H_
