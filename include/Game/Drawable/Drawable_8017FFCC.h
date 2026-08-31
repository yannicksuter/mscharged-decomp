#ifndef _DRAWABLE_8017FFCC_H_
#define _DRAWABLE_8017FFCC_H_

#include "types.h"
#include "NL/nlMath.h"

struct Object_8017FFF4;
struct RenderEntry_8018006C;

class Drawable_8017FFCC
{
public:
    Drawable_8017FFCC();
    void Grab(const Object_8017FFF4* object);
    void Render(const RenderEntry_8018006C* entry) const;
    void Blend(const float* factors, const Drawable_8017FFCC& lhs,
        const Drawable_8017FFCC& rhs);

    u16 mOrientation;
    u16 _002;
    nlVector3 mPosition;
    float mScale;
    bool mVisible;
};

#endif // _DRAWABLE_8017FFCC_H_
