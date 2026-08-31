#ifndef GAME_CAMERA_REPLAY_CAMERA_H
#define GAME_CAMERA_REPLAY_CAMERA_H

#include "Game/Camera/BaseCamera.h"

class ReplayCamera : public cBaseCamera
{
public:
    ReplayCamera();

    virtual eCameraType GetType() { return eCameraType_Replay; }
    virtual void Update(float deltaTime);
    virtual const nlMatrix4& GetViewMatrix() const;
    virtual float GetFOV() const;
    virtual const nlVector3& GetTargetPosition() const;
    virtual const nlVector3& GetCameraPosition() const;

    void ManualUpdate(float deltaTime);
    void SetSideOfInterest(int sideOfInterest);

    /* 0x020 */ u8 mUnidentified020[0xE0];
}; // total size: 0x100

#endif // GAME_CAMERA_REPLAY_CAMERA_H
