#include "Game/TerrainTweaks.h"

#include "Game/TweakFileLoader.h"

TerrainTweaks::~TerrainTweaks()
{
}

TerrainTweaks::TerrainTweaks(const char* fileName, const char* category)
{
    mfField_Speed.BindWithDefault(
        "mfField_Speed", 0.5f, category, true, 0.0f, 1.0f, 0.01f);
    mfField_Slipperyness.BindWithDefault(
        "mfField_Slipperyness", 0.5f, category, true, 0.0f, 1.0f, 0.01f);
    mfField_Friction.BindWithDefault(
        "mfField_Friction", 0.5f, category, true, 0.0f, 1.0f, 0.01f);
    mfField_Bounce.BindWithDefault(
        "mfField_Bounce", 0.5f, category, true, 0.0f, 1.0f, 0.01f);
    gTweakFileLoader.LoadFileAsync(fileName, category);
}

#include "Game/UnidentifiedStaticStorage.h"
