#ifndef _BASECAMERA_H_
#define _BASECAMERA_H_

#include "Game/Camera/rumblefilter.h"
#include "NL/nlMath.h"

#include <string.h>

enum eCameraType
{
    eCameraType_Debug = 0,
    eCameraType_Gameplay = 1,
    eCameraType_TopDown = 2,
    eCameraType_KickOff = 3,
    eCameraType_FollowCharacter = 4,
    eCameraType_FollowBall = 5,
    eCameraType_Goal = 6,
    eCameraType_ShootToScore = 7,
    eCameraType_Replay = 8,
    eCameraType_Animated = 9,
    eCameraType_MatrixEffect = 10,
    eCameraType_AnimViewer = 11,
    eCameraType_FaceCloseup = 12,
};

class cBaseCamera
{
public:
    cBaseCamera()
    {
        mUpVector.x = 0.0f;
        mUpVector.y = 0.0f;
        mUpVector.z = 1.0f;
        memset(m_pFilter, 0, sizeof(m_pFilter));
    }

    virtual ~cBaseCamera() { }
    virtual eCameraType GetType() = 0;
    virtual void Update(float) = 0;
    virtual const nlMatrix4& GetViewMatrix() const = 0;
    virtual float GetFOV() const;
    virtual void Reactivate();
    virtual const nlVector3& GetTargetPosition() const = 0;
    virtual const nlVector3& GetCameraPosition() const = 0;

    /* 0x04 */ cBaseCamera* m_next;
    /* 0x08 */ cBaseCamera* m_prev;
    /* 0x0C */ cCameraFilter* m_pFilter[2];
    /* 0x14 */ nlVector3 mUpVector;
}; // total size: 0x20

#endif // _BASECAMERA_H_
