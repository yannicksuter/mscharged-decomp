#ifndef GAME_TERRAIN_TWEAK_VALUE_INL
#define GAME_TERRAIN_TWEAK_VALUE_INL

inline void TerrainTweakValueBase::CopyValueFrom(TweakValueBase*)
{
}

inline int TerrainTweakValueBase::GetStorageKind()
{
    return 0;
}

inline int TerrainTweakValueBase::GetValueType()
{
    return 0;
}

inline void* TerrainTweakValueBase::GetValueAddress()
{
    return 0;
}

inline void TerrainTweakValueBase::FormatValue(
    char* buffer, unsigned long size)
{
    buffer[0] = '\0';
}

inline void TerrainTweakValueBase::ParseValue(const char*)
{
}

#endif // GAME_TERRAIN_TWEAK_VALUE_INL
