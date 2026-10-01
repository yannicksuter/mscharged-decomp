#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_INL
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_INL

#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsShockwave.h"

extern "C" inline void fn_8016A658(void* data)
{
    g_CollisionPlayerPlayerDataPool.Free((CollisionPlayerPlayerData*)data);
}

extern "C" inline void fn_8016A670(void* data)
{
    lbl_80570138.Free((UnidentifiedEventData24*)data);
}

extern "C" inline void fn_8016A688(CollisionPlayerBallData* data)
{
    g_CollisionPlayerBallDataPool.Free(data);
}

extern "C" inline void fn_8016A6A0(void* data)
{
    g_BallNetmeshEventDataPool.Free((BallNetmeshEventData*)data);
}

extern "C" inline void fn_8016A6B8(CollisionBallGroundData* data)
{
    g_CollisionBallGroundDataPool.Free(data);
}

extern "C" inline void fn_8016A6D0(CollisionBallWallData* data)
{
    g_CollisionBallWallDataPool.Free(data);
}

extern "C" inline void fn_8016A6E8(CollisionBallGoalpostData* data)
{
    g_CollisionBallGoalpostDataPool.Free(data);
}

extern "C" inline void fn_8016A700(CollisionBallShellData* data)
{
    g_CollisionBallShellDataPool.Free(data);
}

inline void FreeCollisionBallChainData(CollisionBallChainData* data)
{
    g_CollisionBallChainDataPool.Free(data);
}

extern "C" inline void fn_8016A730(CollisionKoopaShotBallPlayerData* data)
{
    g_CollisionKoopaShotBallPlayerDataPool.Free(data);
}

extern "C" inline void fn_8016A748(CollisionKoopaShellGoalieData* data)
{
    g_CollisionKoopaShellGoalieDataPool.Free(data);
}

extern "C" inline void fn_8016A760(void* data)
{
    g_CollisionKoopaShellEndDataPool.Free((CollisionKoopaShellEndData*)data);
}

extern "C" inline void fn_8016A778(CollisionBirdoShotBallPlayerData* data)
{
    g_CollisionBirdoShotBallPlayerDataPool.Free(data);
}

extern "C" inline void fn_8016A790(CollisionBirdoEggGoalieData* data)
{
    g_CollisionBirdoEggGoalieDataPool.Free(data);
}

extern "C" inline void fn_8016A7A8(void* data)
{
    g_CollisionBirdoEggEndDataPool.Free((CollisionBirdoEggEndData*)data);
}

extern "C" inline void fn_8016A7C0(void* data)
{
    g_CollisionPowerupGroundDataPool.Free((CollisionPowerupGroundData*)data);
}

extern "C" inline void fn_8016A7D8(CollisionPowerupWallData* data)
{
    g_CollisionPowerupWallDataPool.Free(data);
}

extern "C" inline void fn_8016A7F0(void* data)
{
    g_PowerupHitPlayerEventDataPool.Free((PowerupHitPlayerEventData*)data);
}

extern "C" inline void fn_8016A808(CollisionPlayerBananaData* data)
{
    g_CollisionPlayerBananaDataPool.Free(data);
}

extern "C" inline void fn_8016A820(CollisionPlayerShellData* data)
{
    g_CollisionPlayerShellDataPool.Free(data);
}

extern "C" inline void fn_8016A838(CollisionPlayerFreezeData* data)
{
    g_CollisionPlayerFreezeDataPool.Free(data);
}

extern "C" inline void fn_8016A850(CollisionBulletBillData* data)
{
    g_CollisionBulletBillDataPool.Free(data);
}

extern "C" inline void fn_8016A868(void* data)
{
    g_PowerupUsedEventDataPool.Free((PowerupUsedEventData*)data);
}

inline void FreeCollisionChainPowerupData(CollisionChainPowerupData* data)
{
    g_CollisionChainPowerupDataPool.Free(data);
}

extern "C" inline void fn_8016A898(void* data)
{
    lbl_80570160.Free((UnidentifiedEventData30*)data);
}

extern "C" inline void fn_8016A8B0(UnidentifiedEventData26* data)
{
    lbl_80570110.Free(data);
}

extern "C" inline void fn_8016A8C8(CollisionThwompPlayerData* data)
{
    g_CollisionThwompPlayerDataPool.Free(data);
}

extern "C" inline void fn_8016A8E0(UnidentifiedEventData34* data)
{
    lbl_80570188.Free(data);
}

extern "C" inline void FreeCollisionShockwaveData(void* data)
{
    gCollisionShockwaveDataPool.Free((CollisionShockwaveData*)data);
}

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_INL
