#ifndef GAME_DEBUG_PROFILERDISPLAY_802B9A3C_H
#define GAME_DEBUG_PROFILERDISPLAY_802B9A3C_H

#include "Game/Debug/TimeRegions.h"

struct ProfilerDisplay_802B9A3C
{
    ProfilerDisplay_802B9A3C()
        : count(0)
    {
    }

    float fn_802B9B60(int index, float bin);

    UnidentifiedTimeRegionData_802B9570* data[8];
    int count;
};

extern "C" void fn_802B9670(
    UnidentifiedTimeRegionData_802B9570* data, float value);
extern "C" int fn_802B974C(
    UnidentifiedTimeRegionData_802B9570* data, int index);
extern "C" float fn_802B98C8(
    UnidentifiedTimeRegionData_802B9570* data, int index);
extern "C" ProfilerDisplay_802B9A3C* fn_802B9A3C();
extern "C" void fn_802B9A6C(ProfilerDisplay_802B9A3C* display,
    UnidentifiedTimeRegionData_802B9570* data);
extern "C" void fn_802B9A88(ProfilerDisplay_802B9A3C* display);

#endif // GAME_DEBUG_PROFILERDISPLAY_802B9A3C_H
