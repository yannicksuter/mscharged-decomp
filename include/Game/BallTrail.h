#ifndef GAME_BALL_TRAIL_H
#define GAME_BALL_TRAIL_H

#include "NL/nlMath.h"
#include "types.h"

class BlurHandler;
class DrawableModel;

struct LiveBallTrail
{
    LiveBallTrail();
    ~LiveBallTrail();

    /* 0x00 */ nlQuaternion orientation;
    /* 0x10 */ nlVector3 position;
    /* 0x1C */ nlVector3 velocity;
    /* 0x28 */ nlVector3 mUnidentified028;
    /* 0x34 */ DrawableModel* drawable;
    /* 0x38 */ BlurHandler* mUnidentified038;
    /* 0x3C */ bool visible;
};

#endif // GAME_BALL_TRAIL_H
