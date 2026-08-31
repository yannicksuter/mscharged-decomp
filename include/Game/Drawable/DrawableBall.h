#ifndef _DRAWABLEBALL_H_
#define _DRAWABLEBALL_H_

#include "types.h"
#include "NL/nlMath.h"

class DrawableCharacter;
class RenderSnapshot;

struct BallTrailState
{
    BallTrailState()
    {
        visible = false;
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;
        orientation.z = 0.0f;
        orientation.y = 0.0f;
        orientation.x = 0.0f;
        orientation.w = 1.0f;
    }

    nlQuaternion orientation;
    nlVector3 position;
    bool visible;
    char _1D[3];
};

union DrawableBallFlags
{
    DrawableBallFlags(u32 initial)
        : value(initial)
    {
    }

    u32 value;
    struct
    {
        u32 visible : 1;
        u32 transient : 4;
        u32 lastTouchIndex : 5;
        u32 ownerIndex : 5;
        u32 previousOwnerIndex : 5;
        u32 passTargetIndex : 5;
        u32 unused : 7;
    } bits;
};

class DrawableBall
{
public:
    DrawableBall(RenderSnapshot*);
    DrawableCharacter* IndexToPlayer(int) const;
    void Grab();
    void Render() const;
    void Blend(const float*, const DrawableBall&, const DrawableBall&);
    void EvaluateFrom(DrawableCharacter&);

    RenderSnapshot* mRenderSnapshot;
    DrawableBallFlags mFlags;
    float mScale;
    nlVector3 mVelocity;
    nlVector3 mPosition;
    nlQuaternion mOrientation;
    nlQuaternion mPrevOrientation;
    BallTrailState mTrail[10];
    u32 mTrailCount;
};

#endif // _DRAWABLEBALL_H_
