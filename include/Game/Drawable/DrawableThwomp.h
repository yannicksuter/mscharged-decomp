#ifndef _DRAWABLETHWOMP_H_
#define _DRAWABLETHWOMP_H_

#include "types.h"
#include "NL/nlMath.h"

struct ThwompObject;

class DrawableThwomp
{
public:
    DrawableThwomp();
    void Grab(const ThwompObject*);
    void Render(ThwompObject*) const;
    void Blend(const float*, const DrawableThwomp&, const DrawableThwomp&);

    nlVector3 mPosition;
    nlQuaternion mOrientation;
    bool mVisible;
    char _01D[3];
};

#endif // _DRAWABLETHWOMP_H_
