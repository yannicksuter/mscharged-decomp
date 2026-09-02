#ifndef GAME_BULLET_BILL_H
#define GAME_BULLET_BILL_H

#include "NL/nlMath.h"
#include "types.h"

class PhysicsObject;
class DrawableObject;
class cFielder;

typedef DrawableObject RenderObject;

struct BulletBillObject
{
    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ nlVector3 position;
    /* 0x1C */ nlVector3 velocity;
    /* 0x28 */ u32 unknown_28;
    /* 0x2C */ float scale;
    /* 0x30 */ float unknown_30;
    /* 0x34 */ float unknown_34;
    /* 0x38 */ bool active;
    /* 0x39 */ u8 padding_39[3];
    /* 0x3C */ PhysicsObject* physics;
    /* 0x40 */ RenderObject* drawable;
    /* 0x44 */ cFielder* target;
}; // total size: 0x48

#endif // GAME_BULLET_BILL_H
