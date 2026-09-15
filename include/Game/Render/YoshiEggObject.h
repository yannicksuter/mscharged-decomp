#ifndef GAME_RENDER_YOSHI_EGG_OBJECT_H
#define GAME_RENDER_YOSHI_EGG_OBJECT_H

#include "NL/nlMath.h"
#include "Game/Drawable/RenderObject.h"

class cFielder;
class EmissionController;
class PhysicsYoshiEgg;

class YoshiEggObject
{
public:
    YoshiEggObject(RenderObject*);
    ~YoshiEggObject();

    void Suspend(bool, float);
    void Update(float);
    static void UpdateTrailEffect(EmissionController&);
    void Activate(cFielder*);
    void Deactivate(bool);
    float GetRadius() const;
    void UpdateTransform(float);
    void Reset();
    void Break();
    void SetPendingDisplacement(const nlVector3&, float);

    /* 0x00 */ nlQuaternion mOrientation;
    /* 0x10 */ nlVector3 mPosition;
    /* 0x1C */ float mRadius;
    /* 0x20 */ float mTargetRadius;
    /* 0x24 */ float mRadiusTime;
    /* 0x28 */ bool mActive;
    /* 0x2C */ PhysicsYoshiEgg* mPhysics;
    /* 0x30 */ RenderObject* mDrawable;
    /* 0x34 */ cFielder* mFielder;
    /* 0x38 */ float mDelay;
    /* 0x3C */ nlVector3 mDisplacementDirection;
    /* 0x48 */ float mDisplacementDistance;
}; // size: 0x4C

#endif
