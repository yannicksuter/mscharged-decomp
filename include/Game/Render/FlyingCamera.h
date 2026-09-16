#ifndef GAME_RENDER_FLYING_CAMERA_H
#define GAME_RENDER_FLYING_CAMERA_H

#include "NL/nlMath.h"

class cFielder;

struct FlyingCamera
{
    /* 0x00 */ nlQuaternion mOrientation;
    /* 0x10 */ nlVector3 mPosition;
    /* 0x1C */ nlVector3 mPreviousPosition;
    /* 0x28 */ nlVector3 mPositionIntegral;
    /* 0x34 */ nlVector3 mTargetPosition;
    /* 0x40 */ float mPositionGain;
    /* 0x44 */ float mPositionDamping;
    /* 0x48 */ float mIntegralGain;
    /* 0x4C */ float mOrbitRadius;
    /* 0x50 */ float mHeightOffset;
    /* 0x54 */ int mIndex;
    /* 0x58 */ u16 mAngle;
    /* 0x5A */ bool mVisible;
    /* 0x5B */ u8 mPad5B;
}; // total size: 0x5C

void UpdateFlyingCamera(FlyingCamera* camera, float dt);
void ResetFlyingCameras();
void UpdateFlyingCameras(float dt);
void SetFlyingCameraCount(int count, cFielder* fielder, float orbitRadius);
FlyingCamera* GetFlyingCamera(int index);
void SetFlyingCameraTarget(cFielder* fielder);

#endif // GAME_RENDER_FLYING_CAMERA_H
