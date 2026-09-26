#ifndef GAME_WEATHERDATA_H
#define GAME_WEATHERDATA_H

#include "NL/nlMath.h"

struct SandTombWeather;

extern "C" void fn_800B0358();
extern "C" int fn_800B045C();
extern "C" nlVector3* fn_800B0464(int index);
extern "C" int fn_800B0478(int index);
extern "C" nlVector3* fn_800B048C(int index);
extern "C" int fn_800B04A0(int index);
extern "C" int fn_800B04B4(SandTombWeather* weather);
extern "C" nlVector4 fn_800B04BC(SandTombWeather* weather, int index, bool side);

#endif // GAME_WEATHERDATA_H
