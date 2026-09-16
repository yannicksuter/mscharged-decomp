#ifndef GAME_RENDER_HAMMER_OBJECT_H
#define GAME_RENDER_HAMMER_OBJECT_H

#include "Game/Physics/PhysicsHammer.h"
#include "NL/nlMath.h"
#include "types.h"

class AvoidableObject;
class cFielder;
class DrawableObject;
typedef DrawableObject RenderObject;

struct HammerObject
{
    HammerObject(int index, float radius);
    ~HammerObject();

    const nlVector3* GetPosition() const;
    void SetPosition(const nlVector3& position);
    void SetVelocity(const nlVector3& velocity);
    const nlQuaternion* GetOrientation();
    void OnLanding();
    void Freeze(float duration);
    void Update(float deltaTime);
    void Activate(cFielder* owner);
    void Reset(bool);
    void Deactivate(bool emitEffect);

    /* 0x00 */ nlQuaternion mOrientation;
    /* 0x10 */ float mBaseRadius;
    /* 0x14 */ float mRadiusScale;
    /* 0x18 */ float mTargetRadiusScale;
    /* 0x1C */ float mRadiusTimer;
    /* 0x20 */ u32 mIndex;
    /* 0x24 */ bool mActive;
    /* 0x25 */ bool mPendingReset;
    /* 0x26 */ u8 mPadding026[2];
    /* 0x28 */ PhysicsHammer* mPhysics;
    /* 0x2C */ RenderObject* mDrawable;
    /* 0x30 */ AvoidableObject* mAvoidable;
    /* 0x34 */ cFielder* mOwner;
    /* 0x38 */ nlVector3 mStoredVelocity;
    /* 0x44 */ float mFreezeTimer;
    /* 0x48 */ float mLandedTimer;
}; // size: 0x4C

#endif // GAME_RENDER_HAMMER_OBJECT_H
