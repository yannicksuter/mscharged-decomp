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
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    void SetPendingDisplacement(const nlVector3&, float);
#endif

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
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    /* 0x3C */ nlVector3 mDisplacementDirection;
    /* 0x48 */ float mDisplacementDistance;
#endif
}; // size: 0x4C (USA/Japan/Korea), 0x3C (Europe)

#endif
