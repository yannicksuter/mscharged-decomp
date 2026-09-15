#ifndef GAME_RENDER_KOOPA_SHELL_OBJECT_H
#define GAME_RENDER_KOOPA_SHELL_OBJECT_H

#include "NL/nlMath.h"
#include "types.h"

class cFielder;
class DrawableObject;
class PhysicsKoopaShell;
class EmissionController;
typedef DrawableObject RenderObject;

struct KoopaShellObject
{
    KoopaShellObject(RenderObject* drawable);
    ~KoopaShellObject();
    void Update(float deltaTime);
    static void UpdateTrailEffect(EmissionController&);
    void Activate(cFielder* owner);
    void Deactivate(bool destroyEffect);
    float GetRadiusScale() const;
    void SetPosition(const nlVector3& position);
    void Reset();

    /* 0x00 */ u16 mSpin;
    /* 0x02 */ u8 mPadding002[2];
    /* 0x04 */ nlVector3 mPosition;
    /* 0x10 */ float mRadius;
    /* 0x14 */ float mTargetRadius;
    /* 0x18 */ float mRadiusTimer;
    /* 0x1C */ float mActiveTimer;
    /* 0x20 */ bool mVisible;
    /* 0x21 */ u8 mPadding021[3];
    /* 0x24 */ PhysicsKoopaShell* mPhysics;
    /* 0x28 */ RenderObject* mDrawable;
    /* 0x2C */ cFielder* mOwner;
    /* 0x30 */ nlVector3 mVelocity;
}; // total size: 0x3C

#endif // GAME_RENDER_KOOPA_SHELL_OBJECT_H
