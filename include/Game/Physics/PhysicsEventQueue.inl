#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_INL
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_INL

#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsShockwave.h"

inline void FreeCollisionPlayerPlayerData(void* data)
{
    g_CollisionPlayerPlayerDataPool.Free((CollisionPlayerPlayerData*)data);
}

inline void FreeUnidentifiedEventData24(void* data)
{
    g_UnidentifiedEventData24Pool.Free((UnidentifiedEventData24*)data);
}

inline void FreeCollisionPlayerBallData(CollisionPlayerBallData* data)
{
    g_CollisionPlayerBallDataPool.Free(data);
}

inline void FreeBallNetmeshEventData(void* data)
{
    g_BallNetmeshEventDataPool.Free((BallNetmeshEventData*)data);
}

inline void FreeCollisionBallGroundData(CollisionBallGroundData* data)
{
    g_CollisionBallGroundDataPool.Free(data);
}

inline void FreeCollisionBallWallData(CollisionBallWallData* data)
{
    g_CollisionBallWallDataPool.Free(data);
}

inline void FreeCollisionBallGoalpostData(CollisionBallGoalpostData* data)
{
    g_CollisionBallGoalpostDataPool.Free(data);
}

inline void FreeCollisionBallShellData(CollisionBallShellData* data)
{
    g_CollisionBallShellDataPool.Free(data);
}

inline void FreeCollisionBallChainData(CollisionBallChainData* data)
{
    g_CollisionBallChainDataPool.Free(data);
}

inline void FreeCollisionKoopaShotBallPlayerData(CollisionKoopaShotBallPlayerData* data)
{
    g_CollisionKoopaShotBallPlayerDataPool.Free(data);
}

inline void FreeCollisionKoopaShellGoalieData(CollisionKoopaShellGoalieData* data)
{
    g_CollisionKoopaShellGoalieDataPool.Free(data);
}

inline void FreeCollisionKoopaShellEndData(void* data)
{
    g_CollisionKoopaShellEndDataPool.Free((CollisionKoopaShellEndData*)data);
}

inline void FreeCollisionBirdoShotBallPlayerData(CollisionBirdoShotBallPlayerData* data)
{
    g_CollisionBirdoShotBallPlayerDataPool.Free(data);
}

inline void FreeCollisionBirdoEggGoalieData(CollisionBirdoEggGoalieData* data)
{
    g_CollisionBirdoEggGoalieDataPool.Free(data);
}

inline void FreeCollisionBirdoEggEndData(void* data)
{
    g_CollisionBirdoEggEndDataPool.Free((CollisionBirdoEggEndData*)data);
}

inline void FreeCollisionPowerupGroundData(void* data)
{
    g_CollisionPowerupGroundDataPool.Free((CollisionPowerupGroundData*)data);
}

inline void FreeCollisionPowerupWallData(CollisionPowerupWallData* data)
{
    g_CollisionPowerupWallDataPool.Free(data);
}

inline void FreePowerupHitPlayerEventData(void* data)
{
    g_PowerupHitPlayerEventDataPool.Free((PowerupHitPlayerEventData*)data);
}

inline void FreeCollisionPlayerBananaData(CollisionPlayerBananaData* data)
{
    g_CollisionPlayerBananaDataPool.Free(data);
}

inline void FreeCollisionPlayerShellData(CollisionPlayerShellData* data)
{
    g_CollisionPlayerShellDataPool.Free(data);
}

inline void FreeCollisionPlayerFreezeData(CollisionPlayerFreezeData* data)
{
    g_CollisionPlayerFreezeDataPool.Free(data);
}

inline void FreeCollisionBulletBillData(CollisionBulletBillData* data)
{
    g_CollisionBulletBillDataPool.Free(data);
}

inline void FreePowerupUsedEventData(void* data)
{
    g_PowerupUsedEventDataPool.Free((PowerupUsedEventData*)data);
}

inline void FreeCollisionChainPowerupData(CollisionChainPowerupData* data)
{
    g_CollisionChainPowerupDataPool.Free(data);
}

inline void FreeUnidentifiedEventData30(void* data)
{
    g_UnidentifiedEventData30Pool.Free((UnidentifiedEventData30*)data);
}

inline void FreeUnidentifiedEventData26(UnidentifiedEventData26* data)
{
    g_UnidentifiedEventData26Pool.Free(data);
}

inline void FreeCollisionThwompPlayerData(CollisionThwompPlayerData* data)
{
    g_CollisionThwompPlayerDataPool.Free(data);
}

inline void FreeUnidentifiedEventData34(UnidentifiedEventData34* data)
{
    g_UnidentifiedEventData34Pool.Free(data);
}

inline void FreeCollisionShockwaveData(void* data)
{
    gCollisionShockwaveDataPool.Free((CollisionShockwaveData*)data);
}

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_INL
