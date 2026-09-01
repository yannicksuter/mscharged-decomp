#ifndef UNCLASSIFIED_TU_801A5F10_H
#define UNCLASSIFIED_TU_801A5F10_H

#include "NL/nlMath.h"
#include "types.h"

class cFielder;
class DrawableObject;
class PhysicsSphere;
typedef DrawableObject RenderObject;

struct KoopaShellObject
{
    /* 0x00 */ u16 mSpin;
    /* 0x02 */ u8 mPadding002[2];
    /* 0x04 */ nlVector3 mPosition;
    /* 0x10 */ float mRadius;
    /* 0x14 */ float mTargetRadius;
    /* 0x18 */ float mRadiusTimer;
    /* 0x1C */ float mActiveTimer;
    /* 0x20 */ bool mVisible;
    /* 0x21 */ u8 mPadding021[3];
    /* 0x24 */ PhysicsSphere* mPhysics;
    /* 0x28 */ RenderObject* mDrawable;
    /* 0x2C */ cFielder* mOwner;
    /* 0x30 */ nlVector3 mVelocity;
}; // total size: 0x3C

extern "C"
{
    KoopaShellObject* fn_801A5F30(
        KoopaShellObject*, RenderObject*);
    KoopaShellObject* fn_801A6004(KoopaShellObject*, int);
    void fn_801A6074(KoopaShellObject*, float);
    void fn_801A6344(KoopaShellObject*, cFielder*);
    void fn_801A64A4(KoopaShellObject*, bool);
    float fn_801A65C0(const KoopaShellObject*);
    void fn_801A65D0(KoopaShellObject*, const nlVector3&);
    void fn_801A65F8(KoopaShellObject*);
}

#endif // UNCLASSIFIED_TU_801A5F10_H
