#ifndef _DRAWABLEBULLETBILL_H_
#define _DRAWABLEBULLETBILL_H_

#include "types.h"
#include "Game/Replay.h"
#include "NL/nlMath.h"

struct BulletBillObject;

class DrawableBulletBill
{
public:
    template <typename T>
    void Replay(T& frame);
    DrawableBulletBill();
    void Grab(const BulletBillObject*);
    void Render(const BulletBillObject*) const;
    void Blend(const float*, const DrawableBulletBill&, const DrawableBulletBill&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _021[3];
};

template <typename T>
inline void DrawableBulletBill::Replay(T& frame)
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

#endif // _DRAWABLEBULLETBILL_H_
