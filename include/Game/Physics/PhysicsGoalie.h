#ifndef GAME_PHYSICS_PHYSICS_GOALIE_H
#define GAME_PHYSICS_PHYSICS_GOALIE_H

#include "Game/Physics/PhysicsCharacter.h"

class PhysicsGoalie : public PhysicsCharacter
{
public:
    virtual void PostUpdate();

    bool SweepTestForBallContact(const nlVector3& ballPrevPosition,
        const nlVector3& ballCurrentPosition, const nlVector3& velocity,
        float ballRadius, nlVector3& positionWhenHit,
        nlVector3& contactNormal) const;
    bool SweepTestEveryBone(float ballRadius,
        const nlVector3& ballPrevPosition,
        const nlVector3& ballCurrentPosition, nlVector3& contactNormal,
        nlVector3& positionWhenHit) const;
    void CollideGoalieWithPost();

    static bool IsBallNearGoalie(float goalieRadius, float ballRadius,
        const nlVector3& goaliePos,
        const nlVector3& ballPrevPosition);
    static bool BigBallSweepTest(float goalieRadius, float ballRadius,
        const nlVector3& goaliePos,
        const nlVector3& ballPrevPosition,
        const nlVector3& ballCurrentPosition);
};

#endif // GAME_PHYSICS_PHYSICS_GOALIE_H
