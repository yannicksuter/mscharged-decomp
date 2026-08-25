#ifndef GAME_MATH_HELPERS_H
#define GAME_MATH_HELPERS_H

#include "types.h"
#include "NL/nlMath.h"

static inline float nlMaxEquals(float a, float b)
{
    return (a >= b) ? a : b;
}

static inline float nlMinEquals(float a, float b)
{
    return (a <= b) ? a : b;
}

static inline s32 abs_s16(s16 x)
{
    if (x < 0)
    {
        return -x;
    }
    return x;
}

inline float CalculateDistanceSquared(
    const nlVector3& pos1, const nlVector3& pos2)
{
    nlVector3 delta;
    nlVec3Sub(delta, pos1, pos2);
    return nlGetLengthSquared3D(delta.x, delta.y, delta.z);
}

#endif // GAME_MATH_HELPERS_H
