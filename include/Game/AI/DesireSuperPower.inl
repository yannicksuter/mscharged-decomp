#ifndef GAME_AI_DESIRESUPERPOWER_INL
#define GAME_AI_DESIRESUPERPOWER_INL

/**
 * Offset/Address/Size: 0x9658 | 0x800D1D54 | size: 0x110
 */
inline void DesireSuperPower::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireSuperPower");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpDKShockAvoidable - (u8*)&mvDesiredPosition,
        "mpDKShockAvoidable");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x9768 | 0x800D1E64 | size: 0xC4
 */
inline void DesireSuperPower::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSuperPowerType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSuperPowerType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireSuperPowerType,
        (u8*)this + offset, sizeof(DesireSuperPower) - offset);
    if (data != NULL)
    {
        DesireSuperPower* copy
            = (DesireSuperPower*)((u8*)data - offset);
        *(int*)&copy->mpDKShockAvoidable = -1;
        cFielder* target = mpTarget;
        *(int*)&copy->mpTarget
            = target == NULL ? -1 : target->mUnidentified120;
        cache->ChecksumData(sDesireSuperPowerType, data, context);
    }
}

#endif // GAME_AI_DESIRESUPERPOWER_INL
