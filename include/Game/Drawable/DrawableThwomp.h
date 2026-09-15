#ifndef _DRAWABLETHWOMP_H_
#define _DRAWABLETHWOMP_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

struct ThwompObject;

class DrawableThwomp
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableThwomp();
    void Grab(const ThwompObject*);
    void Render(ThwompObject*) const;
    void Blend(const float*, const DrawableThwomp&, const DrawableThwomp&);

    nlVector3 mPosition;
    nlQuaternion mOrientation;
    bool mVisible;
    char _01D[3];
};

template <typename T>
inline void DrawableThwomp::Replay(T& frame)
{
    Replayable<3>(frame, mVisible);
    if (mVisible)
    {
        Replayable<3>(frame, UnidentifiedQuaternionCompressor(mOrientation));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
    }
}

#endif // _DRAWABLETHWOMP_H_
