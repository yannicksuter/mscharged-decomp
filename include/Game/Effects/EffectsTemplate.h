#ifndef GAME_EFFECTS_EFFECTS_TEMPLATE_H
#define GAME_EFFECTS_EFFECTS_TEMPLATE_H

#include "NL/nlMath.h"

extern unsigned int uSeed;

inline float RandomizedValue(float base, float range)
{
    float randomOffset = nlRandomf(0.5f * range, &uSeed);
    unsigned int randomSign = nlRandom(0x7FFFFFFF, &uSeed);

    if (randomSign & 1)
    {
        return base + randomOffset;
    }
    else
    {
        return base - randomOffset;
    }
}

#endif // GAME_EFFECTS_EFFECTS_TEMPLATE_H
