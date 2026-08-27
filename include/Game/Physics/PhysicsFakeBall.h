#ifndef _PHYSICSFAKEBALL_H_
#define _PHYSICSFAKEBALL_H_

#include "NL/nlMath.h"

class FakeBallWorld
{
public:
    static void InvalidateBallCache();
    static bool GetPredictedBallPosition(
        float fDeltaTime, nlVector3& v3Position, nlVector3& v3Velocity);
};

#endif // _PHYSICSFAKEBALL_H_
