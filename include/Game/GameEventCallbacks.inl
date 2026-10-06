extern "C" void fn_80072134(LightningStrikeData* node)
{
    g_LightningStrikeDataPool.Free(node);
}

extern "C" void fn_8007214C(ShotAtGoalData* node)
{
    g_ShotAtGoalDataPool.Free(node);
}

extern "C" void fn_80072164(UnidentifiedRegistrationNode* node)
{
    node->mNext = (UnidentifiedRegistrationNode*)g_NISDataPool.m_FreeList;
    g_NISDataPool.m_FreeList = (SlotPoolEntry*)node;
}

extern "C" void fn_8007217C(UnidentifiedRegistrationNode* node)
{
    node->mNext = (UnidentifiedRegistrationNode*)g_CollisionCrowdDataPool.m_FreeList;
    g_CollisionCrowdDataPool.m_FreeList = (SlotPoolEntry*)node;
}

extern "C" void fn_80072194(PlayerAttackData* node)
{
    g_PlayerAttackDataPool.Free(node);
}

void FreeCollisionPlayerWallData(CollisionPlayerWallData* node)
{
    g_CollisionPlayerWallDataPool.Free(node);
}

extern "C" EventDispatcher* fn_800721C4()
{
    return &gDispatchEventsTask->dispatcher;
}
