static inline void FreeCollisionWindDebrisPlayerData(CollisionWindDebrisPlayerData* pData)
{
    g_CollisionWindDebrisPlayerDataPool.Free(pData);
}
