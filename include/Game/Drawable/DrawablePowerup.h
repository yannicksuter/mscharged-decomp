#ifndef _DRAWABLEPOWERUP_H_
#define _DRAWABLEPOWERUP_H_

#include "types.h"
#include "NL/nlMath.h"

class DrawablePowerup
{
public:
    void Grab(int);
    void Render(int) const;
    void Blend(const float*, const DrawablePowerup&, const DrawablePowerup&);

    s8 mType;
    char _01[3];
    float mScale;
    float mRadius;
    bool mVisible;
    char _0D[3];
    nlVector3 mPosition;
    u16 mOrientation;
};

#endif // _DRAWABLEPOWERUP_H_
