#ifndef GAME_WEATHERDATA_H
#define GAME_WEATHERDATA_H

#include "NL/nlMath.h"

struct SandTombWeather;

void InitChainLightningPaths();
int GetNumChainLightningPaths();
nlVector3* GetLeftChainLightningPath(int index);
int GetLeftChainLightningPathLength(int index);
nlVector3* GetRightChainLightningPath(int index);
int GetRightChainLightningPathLength(int index);
int GetNumSandPatches(SandTombWeather* weather);
nlVector4 GetSandPatch(SandTombWeather* weather, int index, bool side);

#endif // GAME_WEATHERDATA_H
