#ifndef GAME_RENDER_THWOMP_OBJECT_H
#define GAME_RENDER_THWOMP_OBJECT_H

#include "NL/nlMath.h"
#include "types.h"

class PhysicsObject;
class DrawableObject;
struct glModelPacket;

typedef DrawableObject RenderObject;

class AvoidablePolygon;

enum eThwompState
{
    THWOMP_STATE_HIDDEN = -1,
    THWOMP_STATE_APPEARING = 0,
    THWOMP_STATE_IDLE = 1,
    THWOMP_STATE_WARNING = 2,
    THWOMP_STATE_FALLING = 3,
    THWOMP_STATE_LANDED = 4,
    THWOMP_STATE_RISING = 5,
    THWOMP_STATE_DISAPPEARING = 6,
    THWOMP_STATE_FORCED_RISING = 7,
    THWOMP_STATE_8 = 8,
};

struct ThwompObject
{
    ThwompObject(int index);
    ~ThwompObject();

    void Freeze(float duration);
    void Update(float dt);
    void Spawn(float x, float y);
    void Stop(bool immediate);
    void SetState(eThwompState state);
    const nlVector3* GetPosition() const;
    void OnLanding();
    float GetScale() const;
    void UpdateTexture();

    /* 0x00 */ eThwompState mState;
    /* 0x04 */ int mIndex;
    /* 0x08 */ bool mVisible;
    /* 0x09 */ u8 mPadding009[3];
    /* 0x0C */ PhysicsObject* mPhysics;
    /* 0x10 */ RenderObject* mDrawable;
    /* 0x14 */ AvoidablePolygon* mAvoidable;
    /* 0x18 */ float mDelayTimer;
    /* 0x1C */ float mLandingTimer;
    /* 0x20 */ float mWarningTimer;
    /* 0x24 */ float mScaleTimer;
    /* 0x28 */ int mUnidentified028;
    /* 0x2C */ u8 mPadding02C[0x1C];
    /* 0x48 */ glModelPacket* mDiffusePacket;
    /* 0x4C */ glModelPacket* mGlossPacket;
    /* 0x50 */ unsigned long mTexture3;
    /* 0x54 */ unsigned long mTexture3Index;
    /* 0x58 */ unsigned long mTexture2;
    /* 0x5C */ unsigned long mTexture2Index;
    /* 0x60 */ unsigned long mTexture1;
    /* 0x64 */ unsigned long mTexture1Index;
}; // size: 0x68

#endif // GAME_RENDER_THWOMP_OBJECT_H
