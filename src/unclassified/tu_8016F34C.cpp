#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/FixedUpdateTask.h"
#include "NL/utility.h"

#include "types.h"

static const f32 CANT_COLLIDE = 3.402823466e+38F;

class PhysicsGoalie : public PhysicsCharacter
{
public:
    virtual ~PhysicsGoalie() { }
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

static float ballMaxMotionPerTick =
    PhysicsBall::GetBallMaxVelocity()
    * FixedUpdateTask::GetPhysicsUpdateTick();

void PhysicsGoalie::PostUpdate()
{
    PhysicsCharacter::PostUpdate();
    CollideGoalieWithPost();
}

bool PhysicsGoalie::SweepTestForBallContact(
    const nlVector3& ballPrevPosition,
    const nlVector3& ballCurrentPosition, const nlVector3& velocity,
    float ballRadius, nlVector3& positionWhenHit,
    nlVector3& contactNormal) const
{
    int testsPassed = 0;
    float goalieRadius = 4.0f * m_CentreOfMassHeight;

    nlVector3 goaliePos;
    GetPosition(&goaliePos);
    goaliePos.z = (2.0 * m_CentreOfMassHeight) + goaliePos.z;

    if (IsBallNearGoalie(
            goalieRadius, ballRadius, goaliePos, ballPrevPosition))
    {
        testsPassed = 1;
        if (BigBallSweepTest(goalieRadius, ballRadius, goaliePos,
                ballPrevPosition, ballCurrentPosition))
        {
            testsPassed = 2;
            if (SweepTestEveryBone(ballRadius, ballPrevPosition,
                    ballCurrentPosition, contactNormal,
                    positionWhenHit))
            {
                testsPassed = 3;
            }
        }
    }

    return testsPassed == 3;
}

bool PhysicsGoalie::IsBallNearGoalie(float goalieRadius,
    float ballRadius, const nlVector3& goaliePos,
    const nlVector3& ballPrevPosition)
{
    return (nlSqrt(
                nlGetLengthSquared3D(ballPrevPosition.x - goaliePos.x,
                    ballPrevPosition.y - goaliePos.y,
                    ballPrevPosition.z - goaliePos.z),
                true)
        - (goalieRadius + (ballRadius + ballMaxMotionPerTick)))
        <= 0.0f;
}

bool PhysicsGoalie::BigBallSweepTest(float goalieRadius,
    float ballRadius, const nlVector3& goaliePos,
    const nlVector3& ballPrevPosition,
    const nlVector3& ballCurrentPosition)
{
    float time = SweepSpheres(ballRadius, ballPrevPosition,
        ballCurrentPosition, goalieRadius, goaliePos, goaliePos);

    if ((time == CANT_COLLIDE) || (time < 0.0f) || (time > 1.0f))
    {
        return false;
    }
    return true;
}
