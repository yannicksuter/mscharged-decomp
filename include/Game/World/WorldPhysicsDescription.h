#ifndef GAME_WORLD_WORLD_PHYSICS_DESCRIPTION_H
#define GAME_WORLD_WORLD_PHYSICS_DESCRIPTION_H

#include "NL/nlMath.h"

struct WorldPhysicsDescription
{
    /* 0x00 */ nlMatrix4 matLocalToParent;
    /* 0x40 */ unsigned long uPrimitiveType;
    /* 0x44 */ float fWidth;
    /* 0x48 */ float fLength;
    /* 0x4C */ float fHeight;
    /* 0x50 */ float fRadius;
};

#endif
