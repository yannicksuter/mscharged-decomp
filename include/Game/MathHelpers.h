#ifndef GAME_MATH_HELPERS_H
#define GAME_MATH_HELPERS_H

#include "types.h"

static inline s32 abs_s16(s16 x)
{
    if (x < 0)
    {
        return -x;
    }
    return x;
}

#endif // GAME_MATH_HELPERS_H
