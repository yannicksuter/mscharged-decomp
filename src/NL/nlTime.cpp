#include "NL/nlTime.h"

#include "decomp.h"

typedef long long OSTime;

extern "C" OSTime OSGetTime();

u32 __OSBusClock AT_ADDRESS(0x800000F8);

void nlInitTime()
{
}

unsigned long long nlGetTime()
{
    return OSGetTime();
}

static f32 nlTimeToMilliseconds(unsigned long long delta)
{
    return 0.001f * (f32)(unsigned long long)((delta << 3) / ((__OSBusClock >> 2) / 125000));
}

f32 nlGetTimeDifference(unsigned long long startTime, unsigned long long endTime)
{
    return nlTimeToMilliseconds(endTime - startTime);
}
