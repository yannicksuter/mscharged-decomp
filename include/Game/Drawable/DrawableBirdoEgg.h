#ifndef _DRAWABLEBIRDOEGG_H_
#define _DRAWABLEBIRDOEGG_H_

#include "types.h"
#include "NL/nlMath.h"

struct BirdoEggObject;

class DrawableBirdoEgg
{
public:
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

#endif // _DRAWABLEBIRDOEGG_H_
