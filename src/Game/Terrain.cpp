#include "Game/AI/AiUtil.h"
#include "Game/Terrain.h"
#include "Game/GameTweaks.h"
#include "NL/nlPrint.h"

extern const char* gTerrainNames[6];
extern const char sTerrainConfigPathFormat[];

int GetTerrainConfigFilename(int index, char* buffer, unsigned long size)
{
    return nlSNPrintf(buffer, size, sTerrainConfigPathFormat, gTerrainNames[index]);
}

Terrain::Terrain(int index)
{
    mIndex = index;
    nlSNPrintf(mName, 0x3F, sTerrainConfigPathFormat, gTerrainNames[index]);
}

Terrain::~Terrain()
{
}

float Terrain::GetSpeedFactor()
{
    return gGameTweaks.mTerrainTweaks->mfField_Speed;
}

float Terrain::GetSlideFactor()
{
    return gGameTweaks.mTerrainTweaks->mfField_Slipperyness;
}

float Terrain::GetRollingResistance(float value)
{
    return Interpolate(0.25f, 0.008f * value, gGameTweaks.mTerrainTweaks->mfField_Friction);
}

float Terrain::GetRestitution(float value)
{
    return Interpolate(0.07f, 2.0f * value, gGameTweaks.mTerrainTweaks->mfField_Bounce);
}
