#ifndef UNCLASSIFIED_TU_8019D6B4_H
#define UNCLASSIFIED_TU_8019D6B4_H

#include "NL/nlMath.h"
#include "types.h"

class cCharacter;
class DrawableObject;
typedef DrawableObject RenderObject;

// Live object for one crystal fist of Daisy's super ability. NPCManager keeps
// eight of them; DesireSuperPower spawns a ring of fists around Daisy, and each
// fist rises out of the ground in front of its owner, creates the impact sphere
// when it breaks the surface, then sinks back below the field.
struct DaisyFistObject
{
    DaisyFistObject(int index);
    ~DaisyFistObject();

    void Update(float dt);
    void Spawn(cCharacter* owner, u16 orientation);
    float GetScale() const;
    void Reset();

    /* 0x00 */ u16 mOrientation;
    /* 0x02 */ u8 mPadding002[2];
    /* 0x04 */ nlVector3 mPosition;
    /* 0x10 */ float mScale;
    /* 0x14 */ float mTargetScale;
    /* 0x18 */ float mScaleTimer;
    /* 0x1C */ float mRiseTimer;
    /* 0x20 */ bool mVisible;
    /* 0x21 */ u8 mPadding021[3];
    /* 0x24 */ RenderObject* mDrawable;
    /* 0x28 */ cCharacter* mOwner;
    /* 0x2C */ float mDelayTimer;
}; // total size: 0x30

#endif // UNCLASSIFIED_TU_8019D6B4_H
