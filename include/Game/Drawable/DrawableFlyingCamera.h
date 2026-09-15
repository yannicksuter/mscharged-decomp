#ifndef _DRAWABLEFLYINGCAMERA_H_
#define _DRAWABLEFLYINGCAMERA_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

class DrawableFlyingCamera
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableFlyingCamera();
    void Grab();
    void Render() const;
    void Blend(const float*, const DrawableFlyingCamera&, const DrawableFlyingCamera&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    int mIndex;
    bool mVisible;
    char _025[3];
};

template <typename T>
inline void DrawableFlyingCamera::Replay(T& frame)
{
    Replayable<3>(frame, mVisible);
    if (mVisible)
    {
        Replayable<3>(frame, UnidentifiedQuaternionCompressor(mOrientation));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
        Replayable<3>(frame, FloatCompressor<0, 2, 6>(mScale));
    }
}

#endif // _DRAWABLEFLYINGCAMERA_H_
