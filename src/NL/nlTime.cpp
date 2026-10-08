#include <revolution/os/OSTime.h>

#include "NL/nlTime.h"

void nlInitTime()
{
}

unsigned long long nlGetTime()
{
    return OSGetTime();
}

static f32 nlTimeToMilliseconds(unsigned long long delta)
{
    return 0.001f * (f32)(unsigned long long)((delta << 3) / ((OS_BUS_CLOCK_SPEED >> 2) / 125000));
}

f32 nlGetTimeDifference(unsigned long long startTime, unsigned long long endTime)
{
    return nlTimeToMilliseconds(endTime - startTime);
}
