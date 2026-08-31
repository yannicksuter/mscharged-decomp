#ifndef _DRAWABLEBULLETBILL_H_
#define _DRAWABLEBULLETBILL_H_

#include "types.h"
#include "NL/nlMath.h"

struct BulletBillObject;

class DrawableBulletBill
{
public:
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

#endif // _DRAWABLEBULLETBILL_H_
