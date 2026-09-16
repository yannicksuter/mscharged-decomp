#ifndef GAME_TERRAIN_H
#define GAME_TERRAIN_H

class DebugWriteCache;

class Terrain
{
public:
    Terrain(int index);
    ~Terrain();
    void Load(int index);
    float GetSpeedFactor();
    float GetSlideFactor();
    float GetRollingResistance(float value);
    float GetRestitution(float value);
    void SyncLog(void* context, DebugWriteCache* cache);

    /* 0x00 */ char mName[0x40];
    /* 0x40 */ int mIndex;
}; // size: 0x44

int GetTerrainConfigFilename(int index, char* buffer, unsigned long size);

#endif // GAME_TERRAIN_H
