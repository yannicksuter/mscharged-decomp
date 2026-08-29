#include "Game/Core/mtRandom.h"

static unsigned long mt[624];
static int mti = 625;

void seedMT(unsigned long seed)
{
    mt[0] = seed;

    for (mti = 1; mti < 624; ++mti)
    {
        mt[mti] = 1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti;
    }
}
