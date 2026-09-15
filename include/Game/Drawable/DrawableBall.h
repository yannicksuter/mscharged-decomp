#ifndef _DRAWABLEBALL_H_
#define _DRAWABLEBALL_H_

#include "types.h"
#include "NL/nlMath.h"
#include "Game/Replay.h"

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
    const nlVector3& fn_801925BC() const;
    template <typename T>
    void Replay(T& frame);
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


template <typename T>
void DrawableBall::Replay(T& frame)
{
    Replayable<1>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
    Replayable<1>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
    Replayable<1>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
    Replayable<1>(frame, UnidentifiedQuaternionCompressor(mOrientation));
    Replayable<1>(frame, FloatCompressor<-127, 127, 5>(mVelocity.x));
    Replayable<1>(frame, FloatCompressor<-127, 127, 5>(mVelocity.y));
    Replayable<1>(frame, FloatCompressor<-127, 127, 5>(mVelocity.z));
    Replayable<1>(frame, mFlags.value);
    Replayable<1>(frame, FloatCompressor<0, 8, 12>(mScale));
    Replayable<1>(frame, mTrailCount);
    for (unsigned int i = 0; i < mTrailCount; i++)
    {
        Replayable<1>(frame, mTrail[i].visible);
        if (mTrail[i].visible)
        {
            nlVector3& position = mTrail[i].position;
            Replayable<1>(frame, FloatCompressor<-127, 127, 7>(position.x));
            Replayable<1>(frame, FloatCompressor<-127, 127, 7>(position.y));
            Replayable<1>(frame, FloatCompressor<-127, 127, 7>(position.z));
            Replayable<1>(frame, UnidentifiedQuaternionCompressor(mTrail[i].orientation));
        }
    }
}

#endif // _DRAWABLEBALL_H_
