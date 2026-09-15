#ifndef _DRAWABLEBIRDOEGG_H_
#define _DRAWABLEBIRDOEGG_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

struct BirdoEggObject;

class DrawableBirdoEgg
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableBirdoEgg();
    void Grab(const BirdoEggObject*);
    void Render(const BirdoEggObject*) const;
    void Blend(const float*, const DrawableBirdoEgg&, const DrawableBirdoEgg&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _021[3];
};

template <typename T>
inline void DrawableBirdoEgg::Replay(T& frame)
{
    Replayable<3>(frame, mVisible);
    if (mVisible)
    {
        Replayable<3>(frame, UnidentifiedQuaternionCompressor(mOrientation));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
        Replayable<3>(frame, FloatCompressor<0, 15, 12>(mScale));
    }
}

#endif // _DRAWABLEBIRDOEGG_H_
