#ifndef _FACECAM_H_
#define _FACECAM_H_

#include "Game/Camera/BaseCamera.h"
#include "Game/Character.h"

class FaceCam : public cBaseCamera
{
public:
    FaceCam(float distance);
    virtual void Update(float dt);
    virtual const nlMatrix4& GetViewMatrix() const { return mViewMatrix; }
    virtual eCameraType GetType() { return eCameraType_FaceCloseup; }
    virtual ~FaceCam() { }
    virtual const nlVector3& GetCameraPosition() const;
    virtual const nlVector3& GetTargetPosition() const;

    void SetToUserCharacter();

    /* 0x20 */ cCharacter* mpCharacter;
    /* 0x24 */ float mDistance;
    /* 0x28 */ nlVector3 mTargetPosition;
    /* 0x34 */ nlVector3 mCameraPosition;
    /* 0x40 */ nlMatrix4 mViewMatrix;
    /* 0x80 */ float mfFOV;
}; // total size: 0x84

#endif // _FACECAM_H_
