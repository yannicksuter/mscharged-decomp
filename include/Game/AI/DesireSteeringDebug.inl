inline void DesireSteering::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireSteering");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&m_ePositionSeekState - (u8*)&mvDesiredPosition,
        "m_ePositionSeekState");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3DesiredPos - (u8*)&mvDesiredPosition,
        "m_v3DesiredPos");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fDesiredFacingDirection - (u8*)&mvDesiredPosition,
        "m_fDesiredFacingDirection");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fFacingTotalWeight - (u8*)&mvDesiredPosition,
        "m_fFacingTotalWeight");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3LastDesiredPos - (u8*)&mvDesiredPosition,
        "m_v3LastDesiredPos");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3DesiredVel - (u8*)&mvDesiredPosition,
        "m_v3DesiredVel");
    cache->AddField(22, gDebugFieldTypes[22].size,
        (u8*)&m_v3TempDesiredPos - (u8*)&mvDesiredPosition,
        "m_v3TempDesiredPos");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fTotalWeight - (u8*)&mvDesiredPosition,
        "m_fTotalWeight");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fUrgency - (u8*)&mvDesiredPosition,
        "m_fUrgency");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fDesiredArrivalTime - (u8*)&mvDesiredPosition,
        "m_fDesiredArrivalTime");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fForcedArrivalRadius - (u8*)&mvDesiredPosition,
        "m_fForcedArrivalRadius");
    cache->AddField(17, gDebugFieldTypes[17].size,
        (u8*)&m_fAvoidanceMult - (u8*)&mvDesiredPosition,
        "m_fAvoidanceMult");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&m_ThingsToAvoid - (u8*)&mvDesiredPosition,
        "m_ThingsToAvoid");
    cache->EndType();
}

inline void DesireSteering::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSteeringType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSteeringType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireSteeringType, data, context);
    cache->WriteData(sDesireSteeringType, data,
        sizeof(DesireSteering) - offset);
}

