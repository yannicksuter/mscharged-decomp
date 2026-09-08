#ifndef _DRAWABLEYOSHIEGG_H_
#define _DRAWABLEYOSHIEGG_H_

#include "types.h"
#include "NL/nlMath.h"

class UnidentifiedObject_801B535C;

class DrawableYoshiEgg
{
public:
    DrawableYoshiEgg();
    void Grab(const UnidentifiedObject_801B535C*);
    void Render(const UnidentifiedObject_801B535C*) const;
    void Blend(const float*, const DrawableYoshiEgg&, const DrawableYoshiEgg&);

    nlQuaternion mOrientation;
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
    char _021[3];
};

#endif // _DRAWABLEYOSHIEGG_H_
