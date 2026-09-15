#ifndef _DRAWABLEDAISYFIST_H_
#define _DRAWABLEDAISYFIST_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

struct DaisyFistObject;

class DrawableDaisyFist
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableDaisyFist();
    void Grab(const DaisyFistObject* object);
    void Render(const DaisyFistObject* object) const;
    void Blend(const float* factors, const DrawableDaisyFist& lhs,
        const DrawableDaisyFist& rhs);

    u16 mOrientation;
    char _002[2];
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _015[3];
};

template <typename T>
inline void DrawableDaisyFist::Replay(T& frame)
{
    Replayable<3>(frame, mVisible);
    if (mVisible)
    {
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.x));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.y));
        Replayable<3>(frame, FloatCompressor<-127, 127, 7>(mPosition.z));
        Replayable<3>(frame, mOrientation);
        Replayable<3>(frame, FloatCompressor<0, 15, 12>(mScale));
    }
}

#endif // _DRAWABLEDAISYFIST_H_
