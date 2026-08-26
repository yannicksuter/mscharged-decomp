#ifndef GAME_PHYSICS_PHYSICS_AI_BALL_H
#define GAME_PHYSICS_PHYSICS_AI_BALL_H

#include "NL/nlMath.h"

class PhysicsAIBall
{
public:
    static bool IsBallOutsideNet(const nlVector3& ballPosition);
    static bool DidBallJustEnterNet(
        const nlVector3& oldPosition, nlVector3 newPosition);
};

#endif // GAME_PHYSICS_PHYSICS_AI_BALL_H
