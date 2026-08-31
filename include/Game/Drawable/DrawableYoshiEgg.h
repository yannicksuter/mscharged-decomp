#ifndef _DRAWABLEYOSHIEGG_H_
#define _DRAWABLEYOSHIEGG_H_

#include "types.h"
#include "NL/nlMath.h"

struct YoshiEggObject;

class DrawableYoshiEgg
{
public:
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

#endif // _DRAWABLEYOSHIEGG_H_
