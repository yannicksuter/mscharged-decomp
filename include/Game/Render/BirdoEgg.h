#ifndef GAME_RENDER_BIRDOEGG_H
#define GAME_RENDER_BIRDOEGG_H

#include "Game/Drawable/RenderObject.h"
#include "NL/nlMath.h"
#include "types.h"

class cFielder;
class PhysicsSphere;

struct BirdoEggObject
{
    BirdoEggObject(RenderObject* drawable);
    ~BirdoEggObject();

    void Update(float deltaTime);
    void SetPosition(const nlVector3& position);
    void Show(cFielder* shooter);
    void Hide(bool destroyEffect);
    float GetScale() const;
    void Reset();

    /* 0x00 */ nlQuaternion mOrientation;
    /* 0x10 */ float mSpinSpeed;
    /* 0x14 */ nlVector3 mPosition;
    /* 0x20 */ float mRadius;
    /* 0x24 */ float mTargetRadius;
    /* 0x28 */ float mRadiusTimer;
    /* 0x2C */ float mActiveTimer;
    /* 0x30 */ bool mVisible;
    /* 0x31 */ u8 mPadding031[3];
    /* 0x34 */ PhysicsSphere* mPhysics;
    /* 0x38 */ RenderObject* mDrawable;
    /* 0x3C */ cFielder* mShooter;
    /* 0x40 */ nlVector3 mVelocity;
}; // total size: 0x4C

#endif // GAME_RENDER_BIRDOEGG_H
