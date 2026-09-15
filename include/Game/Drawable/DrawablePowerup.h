#ifndef _DRAWABLEPOWERUP_H_
#define _DRAWABLEPOWERUP_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

class DrawablePowerup
{
public:
    template <typename T>
    void Replay(T& frame);
    void Grab(int);
    void Render(int) const;
    void Blend(const float*, const DrawablePowerup&, const DrawablePowerup&);

    void SetUnidentifiedVisible(bool visible)
    {
        mVisible = visible;
    }

    s8 mType;
    char _01[3];
    float mScale;
    float mRadius;
    bool mVisible;
    char _0D[3];
    nlVector3 mPosition;
    u16 mOrientation;
};

template <typename T>
inline void DrawablePowerup::Replay(T& frame)
{
    Replayable<3>(frame, mVisible);
    if (mVisible)
    {
        Replayable<3>(frame, (char&)mType);
        Replayable<3>(frame, mOrientation);
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
        Replayable<3>(frame, FloatCompressor<0, 15, 12>(mRadius));
        Replayable<3>(frame, FloatCompressor<0, 15, 12>(mScale));
    }
}

#endif // _DRAWABLEPOWERUP_H_
