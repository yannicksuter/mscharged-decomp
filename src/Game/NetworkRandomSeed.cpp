#include "Game/InputRouter.h"

#include "NL/nlMath.h"

u32 gNetworkRandomSeed = 0x12345678;

u32 GetNetworkRandomSeed()
{
    return gNetworkRandomSeed;
}

void SetNetworkRandomSeed(u32 seed)
{
    gNetworkRandomSeed = seed;
}

void OnInputSessionReset()
{
}

u32 nlRandom(u32 range)
{
    return nlRandom(range, &gNetworkRandomSeed);
}

float nlRandomf(float maximum)
{
    return nlRandomf(maximum, &gNetworkRandomSeed);
}
