#ifndef _DRAWABLEHAMMER_H_
#define _DRAWABLEHAMMER_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

struct HammerObject;

class DrawableHammer
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableHammer();
    void Grab(const HammerObject*);
    void Render(const HammerObject*) const;
    void Blend(const float*, const DrawableHammer&, const DrawableHammer&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _021[3];
};

template <typename T>
inline void DrawableHammer::Replay(T& frame)
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

#endif // _DRAWABLEHAMMER_H_
