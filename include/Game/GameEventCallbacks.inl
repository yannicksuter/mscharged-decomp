void FreeLightningStrikeData(LightningStrikeData* node)
{
    g_LightningStrikeDataPool.Free(node);
}

void FreeShotAtGoalData(ShotAtGoalData* node)
{
    g_ShotAtGoalDataPool.Free(node);
}

void FreeNISData(NISData* node)
{
    g_NISDataPool.Free(node);
}

void FreeCollisionCrowdData(CollisionCrowdData* node)
{
    g_CollisionCrowdDataPool.Free(node);
}

void FreePlayerAttackData(PlayerAttackData* node)
{
    g_PlayerAttackDataPool.Free(node);
}

void FreeCollisionPlayerWallData(CollisionPlayerWallData* node)
{
    g_CollisionPlayerWallDataPool.Free(node);
}

EventDispatcher* GetGameEventDispatcher()
{
    return &gDispatchEventsTask->dispatcher;
}
