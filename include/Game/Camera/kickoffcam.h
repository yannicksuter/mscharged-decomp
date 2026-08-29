#ifndef _KICKOFFCAM_H_
#define _KICKOFFCAM_H_

#include "Game/Camera/BaseCamera.h"

class cKickOffCamera : public cBaseCamera
{
public:
    cKickOffCamera();
    virtual ~cKickOffCamera();
    virtual void Update(float dt);
    virtual const nlMatrix4& GetViewMatrix() const { return m_matView; }
    virtual const nlVector3& GetCameraPosition() const { return m_v3Camera; }
    virtual const nlVector3& GetTargetPosition() const { return m_v3Target; }
    virtual eCameraType GetType() { return eCameraType_KickOff; }

    /* 0x20 */ nlMatrix4 m_matView;
    /* 0x60 */ nlVector3 m_v3Camera;
    /* 0x6C */ nlVector3 m_v3Target;
}; // total size: 0x78

#endif // _KICKOFFCAM_H_
