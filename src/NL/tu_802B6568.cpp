#include "NL/nlMath.h"

#include "Game/Core/mtRandom.h"

unsigned int nlDefaultSeed = 0x12345678;

float nlRandomf(float fMin, float fMax, unsigned int* pSeed)
{
    return fMin + nlRandomf(fMax - fMin, pSeed);
}

float nlRandomf(float fMax, unsigned int* pSeed)
{
    return (1.0f / 2147483647.0f) * fMax * (float)nlRandom(0x7FFFFFFF, pSeed);
}

unsigned int nlRandom(unsigned int range, unsigned int* seed)
{
    if (range == 0)
    {
        return 0;
    }

    unsigned int next;
    unsigned int temp;

    unsigned int result = *seed % range;
    next = *seed ^ 0x1d872b41;
    temp = next ^ (next >> 5);
    *seed = temp ^ (next ^ (temp << 0x1b));
    return result;
}

void nlSetRandomSeed(unsigned int value, unsigned int* seed)
{
    unsigned int temp;
    unsigned int next;

    *seed = value;

    next = *seed ^ 0x1d872b41;
    temp = next ^ (next >> 5);

    *seed = temp ^ (next ^ (temp << 0x1b));
}

void nlInitRandom()
{
    seedMT(0x1105);
}
