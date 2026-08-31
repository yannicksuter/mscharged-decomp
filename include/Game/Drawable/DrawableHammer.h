#ifndef _DRAWABLEHAMMER_H_
#define _DRAWABLEHAMMER_H_

#include "types.h"
#include "NL/nlMath.h"

struct HammerObject;

class DrawableHammer
{
public:
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

#endif // _DRAWABLEHAMMER_H_
