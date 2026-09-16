#include "Game/AI/AiUtil.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/Terrain.h"
#include "Game/TerrainTweaks.h"
#include "Game/TerrainTweakValue.h"
#include "Game/TweakCallback.h"
#include "Game/TweakConfig.h"
#include "Game/GameTweaks.h"
#include "NL/nlPrint.h"

static const char* sTerrainConfigNames[6] = {
    "DryTerrain.ini",
    "NormalTerrain.ini",
    "SlowTerrain.ini",
    "WetTerrain.ini",
    "SlipperyTerrain.ini",
    "ThunderTerrain.ini",
};

static char sTerrainConfigPathFormat[] = "ini/Terrain/%s";
extern char sTerrainTweakCategory[];

static void LoadSelectedTerrain();

static TerrainTweakValue sCurrentTerrain(
    "CurrentTerrain", "Game/Terrain", 1, GetPowerupNamesArray());
static TweakCallback sLoadTerrain(
    "LoadTerrain", "Game/Terrain", LoadSelectedTerrain, true);
static u16 sFieldTerrainType = 0xFFFF;

static void LoadSelectedTerrain()
{
    g_pGame->LoadTerrain(sCurrentTerrain.mValue);
}

int GetTerrainConfigFilename(int index, char* buffer, unsigned long size)
{
    return nlSNPrintf(
        buffer, size, sTerrainConfigPathFormat, sTerrainConfigNames[index]);
}

Terrain::Terrain(int index)
{
    mIndex = index;
    nlSNPrintf(
        mName, 0x3F, sTerrainConfigPathFormat, sTerrainConfigNames[index]);
}

Terrain::~Terrain()
{
}

void Terrain::Load(int index)
{
    mIndex = index;
    nlSNPrintf(
        mName, 0x3F, sTerrainConfigPathFormat, sTerrainConfigNames[index]);
    LoadTweakConfigFile(mName, sTerrainTweakCategory, true);
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
    return Interpolate(
        0.008f, 2.5f * value,
        gGameTweaks.mTerrainTweaks->mfField_Friction);
}

float Terrain::GetRestitution(float value)
{
    return Interpolate(
        0.07f, 2.0f * value,
        gGameTweaks.mTerrainTweaks->mfField_Bounce);
}

void Terrain::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sFieldTerrainType == 0xFFFF)
    {
        sFieldTerrainType = cache->BeginType("FieldTerrain");
        cache->AddField(14, gDebugFieldTypes[14].size, 0, "meTerrain");
        cache->EndType();
    }

    cache->ChecksumData(sFieldTerrainType, &mIndex, context);
    cache->WriteData(sFieldTerrainType, &mIndex, sizeof(mIndex));
}

#include "Game/UnidentifiedStaticStorage.h"
