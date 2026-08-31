#ifndef _TU_8019FE24_H_
#define _TU_8019FE24_H_

#include "NL/nlMath.h"

class cFielder;

struct Object_8019FE24
{
    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ nlVector3 position;
    /* 0x1C */ nlVector3 previousPosition;
    /* 0x28 */ nlVector3 _028;
    /* 0x34 */ nlVector3 _034;
    /* 0x40 */ float _040;
    /* 0x44 */ float _044;
    /* 0x48 */ float _048;
    /* 0x4C */ float _04C;
    /* 0x50 */ float _050;
    /* 0x54 */ int index;
    /* 0x58 */ u16 angle;
    /* 0x5A */ bool visible;
    /* 0x5B */ u8 _05B;
}; // total size: 0x5C

extern "C"
{
    void fn_8019FE24(Object_8019FE24* object, float dt);
    void fn_801A01F8();
    void fn_801A0208(float dt);
    void fn_801A0500(int count, cFielder* fielder, float duration);
    Object_8019FE24* fn_801A0C44(int index);
    void fn_801A0C58(cFielder* fielder);
}

#endif // _TU_8019FE24_H_
