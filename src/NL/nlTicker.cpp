#include "NL/nlTicker.h"

#include "decomp.h"

extern "C" u32 OSGetTick();

u32 __OSBusClock AT_ADDRESS(0x800000F8);

void nlInitTicker()
{
}

u32 nlGetTicker()
{
    return OSGetTick();
}

f32 fn_802AAA28(u32 delta)
{
    return 0.001f * (f32)(u32)((delta << 3) / ((__OSBusClock >> 2) / 125000));
}

f32 nlGetTickerDifference(unsigned int startTick, unsigned int endTick)
{
    u32 delta = endTick - startTick;

    return 0.001f * (f32)(u32)((delta << 3) / ((__OSBusClock >> 2) / 125000));
}
