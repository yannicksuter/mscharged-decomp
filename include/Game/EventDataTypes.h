#ifndef GAME_EVENT_DATA_TYPES_H
#define GAME_EVENT_DATA_TYPES_H

#include "NL/nlMath.h"

class cPlayer;

struct CollisionPlayerWallData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ nlVector3 contactPoint;
    /* 0x10 */ nlVector3 wallNormal;
}; // total size: 0x1C

#endif // GAME_EVENT_DATA_TYPES_H
