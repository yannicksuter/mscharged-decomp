#ifndef _DRAWABLEDAISYFIST_H_
#define _DRAWABLEDAISYFIST_H_

#include "types.h"
#include "NL/nlMath.h"

struct DaisyFistObject;

class DrawableDaisyFist
{
public:
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

#endif // _DRAWABLEDAISYFIST_H_
