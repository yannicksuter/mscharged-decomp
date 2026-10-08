#include "NL/nlTicker.h"

#include <revolution/os/OSHardware_fwd.h>
#include <revolution/os/OSTime_fwd.h>

void nlInitTicker()
{
}

u32 nlGetTicker()
{
    return OSGetTick();
}

f32 nlTicksToMilliseconds(u32 delta)
{
    return 0.001f * (f32)(u32)((delta << 3) / ((OS_BUS_CLOCK_SPEED >> 2) / 125000));
}

f32 nlGetTickerDifference(unsigned int startTick, unsigned int endTick)
{
    return nlTicksToMilliseconds(endTick - startTick);
}
