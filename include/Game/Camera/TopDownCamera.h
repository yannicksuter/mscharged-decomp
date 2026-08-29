#ifndef _TOPDOWNCAMERA_H_
#define _TOPDOWNCAMERA_H_

#include "Game/Camera/BaseCamera.h"

class TopDownCamera : public cBaseCamera
{
public:
    TopDownCamera();
    virtual ~TopDownCamera();
    virtual const nlMatrix4& GetViewMatrix() const;
    virtual void Update(float dt);

    virtual const nlVector3& GetCameraPosition() const
    {
        return m_Position;
    }

    virtual const nlVector3& GetTargetPosition() const
    {
        return m_Target;
    }

    virtual eCameraType GetType()
    {
        return eCameraType_TopDown;
    }

    virtual float GetFOV() const;

    /* 0x20 */ nlMatrix4 m_View;
    /* 0x60 */ nlVector3 m_Position;
    /* 0x6C */ nlVector3 m_Target;
}; // total size: 0x78

#endif // _TOPDOWNCAMERA_H_
