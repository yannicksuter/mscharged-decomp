#ifndef _DRAWABLEYOSHIEGG_H_
#define _DRAWABLEYOSHIEGG_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

class YoshiEggObject;

class DrawableYoshiEgg
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableYoshiEgg();
    void Grab(const YoshiEggObject*);
    void Render(const YoshiEggObject*) const;
    void Blend(const float*, const DrawableYoshiEgg&, const DrawableYoshiEgg&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _021[3];
};

template <typename T>
inline void DrawableYoshiEgg::Replay(T& frame)
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

#endif // _DRAWABLEYOSHIEGG_H_
