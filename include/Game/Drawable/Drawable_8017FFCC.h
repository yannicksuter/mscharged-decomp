#ifndef _DRAWABLE_8017FFCC_H_
#define _DRAWABLE_8017FFCC_H_

#include "types.h"
#include "NL/nlMath.h"

class cCharacter;
class DrawableObject;
typedef DrawableObject RenderObject;

struct Object_8017FFF4
{
    Object_8017FFF4(int index);
    ~Object_8017FFF4();

    void fn_8019D778(float dt);
    void fn_8019D940(cCharacter* owner, u16 orientation);
    float fn_8019DA04() const;
    void fn_8019DA14();

    /* 0x00 */ u16 orientation;
    /* 0x02 */ u16 _002;
    /* 0x04 */ nlVector3 position;
    /* 0x10 */ float _010;
    /* 0x14 */ float _014;
    /* 0x18 */ float _018;
    /* 0x1C */ float _01C;
    /* 0x20 */ bool visible;
    /* 0x21 */ u8 _021[3];
    /* 0x24 */ RenderObject* _024;
    /* 0x28 */ cCharacter* _028;
    /* 0x2C */ float _02C;
}; // total size: 0x30

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
