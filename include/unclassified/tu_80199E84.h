#ifndef UNCLASSIFIED_TU_80199E84_H
#define UNCLASSIFIED_TU_80199E84_H

#include "NL/nlMath.h"
#include "types.h"

class PhysicsSphere;

struct State_80199E84
{
    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ float unknown_10;
    /* 0x14 */ nlVector3 position;
    /* 0x20 */ float unknown_20;
    /* 0x24 */ float unknown_24;
    /* 0x28 */ float unknown_28;
    /* 0x2C */ float unknown_2C;
    /* 0x30 */ bool visible;
    /* 0x31 */ u8 pad_31[3];
    /* 0x34 */ PhysicsSphere* physics;
    /* 0x38 */ void* drawable;
    /* 0x3C */ u32 unknown_3C;
    /* 0x40 */ nlVector3 unknown_40;
}; // total size: 0x4C

extern "C" void fn_8019A248(
    State_80199E84*, const nlVector3& position);

#endif // UNCLASSIFIED_TU_80199E84_H
