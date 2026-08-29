#ifndef _MATRIXEFFECTCAM_H_
#define _MATRIXEFFECTCAM_H_

#include "Game/Camera/BaseCamera.h"

class MatrixEffectCam : public cBaseCamera
{
public:
    MatrixEffectCam();
    virtual ~MatrixEffectCam();
    virtual void Update(float dt);
    virtual const nlMatrix4& GetViewMatrix() const { return mViewMatrix; }
    virtual const nlVector3& GetCameraPosition() const { return mCameraPosition; }
    virtual const nlVector3& GetTargetPosition() const { return mTargetPosition; }
    virtual eCameraType GetType() { return eCameraType_MatrixEffect; }

    /* 0x20 */ nlMatrix4 mViewMatrix;
    /* 0x60 */ nlVector3 mCameraPosition;
    /* 0x6C */ nlVector3 mTargetPosition;
    /* 0x78 */ nlVector3 mUnidentified078;
}; // total size: 0x84

#endif // _MATRIXEFFECTCAM_H_
