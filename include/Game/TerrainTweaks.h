#ifndef GAME_TERRAIN_TWEAKS_H
#define GAME_TERRAIN_TWEAKS_H

#include "Game/TweakValue.h"

struct TerrainTweaks
{
    TerrainTweaks(const char* fileName, const char* category);
    virtual ~TerrainTweaks();

    /* 0x04 */ TweakFloatBinding mfField_Speed;
    /* 0x14 */ TweakFloatBinding mfField_Slipperyness;
    /* 0x24 */ TweakFloatBinding mfField_Friction;
    /* 0x34 */ TweakFloatBinding mfField_Bounce;
}; // total size: 0x44

#endif // GAME_TERRAIN_TWEAKS_H
