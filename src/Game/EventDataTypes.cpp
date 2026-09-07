#include "Game/AI/FielderActions.h"
#include "Game/EventDataTypes.h"

struct UnusedEventData24A
{
    unsigned char data[0x18];
};

struct UnusedEventData16A
{
    unsigned char data[0x10];
};

struct UnusedEventData20A
{
    unsigned char data[0x14];
};

struct UnusedEventData8A
{
    unsigned char data[0x08];
};

struct UnusedEventData28A
{
    unsigned char data[0x1C];
};

struct UnusedEventData28B
{
    unsigned char data[0x1C];
};

struct UnusedEventData8B
{
    unsigned char data[0x08];
};

struct UnusedEventData16B
{
    unsigned char data[0x10];
};

struct UnusedEventData32A
{
    unsigned char data[0x20];
};

struct UnusedEventData20B
{
    unsigned char data[0x14];
};

struct UnusedEventData8C
{
    unsigned char data[0x08];
};

struct UnusedEventData4A
{
    unsigned char data[0x04];
};

struct UnusedEventData4B
{
    unsigned char data[0x04];
};

struct UnusedEventData8D
{
    unsigned char data[0x08];
};

struct UnusedEventData36A
{
    unsigned char data[0x24];
};

struct UnusedEventData4C
{
    unsigned char data[0x04];
};

SlotPool<CollisionPlayerPlayerData> g_CollisionPlayerPlayerDataPool(16, 16);
SlotPool<CollisionChainPlayerData> g_CollisionChainPlayerDataPool(16, 16);
SlotPool<CollisionWindDebrisPlayerData> g_CollisionWindDebrisPlayerDataPool(16, 16);
SlotPool<CollisionThwompPlayerData> g_CollisionThwompPlayerDataPool(16, 16);
SlotPool<CollisionBulletBillData> g_CollisionBulletBillDataPool(16, 16);
SlotPool<CollisionChainPowerupData> g_CollisionChainPowerupDataPool(16, 16);
SlotPool<CollisionPlayerWallData> g_CollisionPlayerWallDataPool(16, 16);
SlotPool<UnusedEventData24A> g_UnusedEventData24APool(16, 16);
SlotPool<UnusedEventData16A> g_UnusedEventData16APool(16, 16);
SlotPool<UnusedEventData20A> g_UnusedEventData20APool(16, 16);
SlotPool<LightningStrikeData> g_LightningStrikeDataPool(16, 16);
SlotPool<CollisionBallWallData> g_CollisionBallWallDataPool(16, 16);
SlotPool<CollisionCrowdData> g_CollisionCrowdDataPool(16, 16);
SlotPool<CollisionPowerupGroundData> g_CollisionPowerupGroundDataPool(16, 16);
SlotPool<CollisionBallGroundData> g_CollisionBallGroundDataPool(16, 16);
SlotPool<CollisionPowerupWallData> g_CollisionPowerupWallDataPool(16, 16);
SlotPool<CollisionPlayerBallData> g_CollisionPlayerBallDataPool(16, 16);
SlotPool<UnusedEventData8A> g_UnusedEventData8APool(16, 16);
SlotPool<CollisionKoopaShellGoalieData> g_CollisionKoopaShellGoalieDataPool(16, 16);
SlotPool<CollisionKoopaShotBallPlayerData> g_CollisionKoopaShotBallPlayerDataPool(16, 16);
SlotPool<CollisionBirdoShotBallPlayerData> g_CollisionBirdoShotBallPlayerDataPool(16, 16);
SlotPool<CollisionBirdoEggGoalieData> g_CollisionBirdoEggGoalieDataPool(16, 16);
SlotPool<CollisionKoopaShellEndData> g_CollisionKoopaShellEndDataPool(16, 16);
SlotPool<CollisionBirdoEggEndData> g_CollisionBirdoEggEndDataPool(16, 16);
SlotPool<CollisionBallShellData> g_CollisionBallShellDataPool(16, 16);
SlotPool<CollisionBallChainData> g_CollisionBallChainDataPool(16, 16);
SlotPool<CollisionPlayerShellData> g_CollisionPlayerShellDataPool(16, 16);
SlotPool<CollisionPlayerFreezeData> g_CollisionPlayerFreezeDataPool(16, 16);
SlotPool<CollisionPlayerBananaData> g_CollisionPlayerBananaDataPool(16, 16);
SlotPool<UnusedEventData28A> g_UnusedEventData28APool(16, 16);
SlotPool<UnusedEventData28B> g_UnusedEventData28BPool(16, 16);
SlotPool<CollisionPowerupStatsData> g_CollisionPowerupStatsDataPool(16, 16);
SlotPool<CollisionBallGoalpostData> g_CollisionBallGoalpostDataPool(16, 16);
SlotPool<BallNetmeshEventData> g_BallNetmeshEventDataPool(16, 16);
SlotPool<UnusedEventData8B> g_UnusedEventData8BPool(16, 16);
SlotPool<UnusedEventData16B> g_UnusedEventData16BPool(16, 16);
SlotPool<UnusedEventData32A> g_UnusedEventData32APool(16, 16);
SlotPool<ShotAtGoalData> g_ShotAtGoalDataPool(16, 16);
SlotPool<UnusedEventData20B> g_UnusedEventData20BPool(16, 16);
SlotPool<PenaltyData> g_PenaltyDataPool(16, 16);
SlotPool<UnusedEventData8C> g_UnusedEventData8CPool(16, 16);
SlotPool<UnusedEventData4A> g_UnusedEventData4APool(16, 16);
SlotPool<UnusedEventData4B> g_UnusedEventData4BPool(16, 16);
SlotPool<UnusedEventData8D> g_UnusedEventData8DPool(16, 16);
SlotPool<UnusedEventData36A> g_UnusedEventData36APool(16, 16);
SlotPool<PlayerAttackData> g_PlayerAttackDataPool(16, 16);
SlotPool<NISData> g_NISDataPool(16, 16);
SlotPool<PowerupUsedEventData> g_PowerupUsedEventDataPool(16, 16);
SlotPool<PowerupHitPlayerEventData> g_PowerupHitPlayerEventDataPool(16, 16);
SlotPool<UnusedEventData4C> g_UnusedEventData4CPool(16, 16);

void FreeEventDataPools()
{
    g_CollisionPlayerPlayerDataPool.FreeBlocks();
    g_CollisionChainPlayerDataPool.FreeBlocks();
    g_CollisionWindDebrisPlayerDataPool.FreeBlocks();
    g_CollisionThwompPlayerDataPool.FreeBlocks();
    g_CollisionBulletBillDataPool.FreeBlocks();
    g_CollisionChainPowerupDataPool.FreeBlocks();
    g_CollisionPlayerWallDataPool.FreeBlocks();
    g_UnusedEventData24APool.FreeBlocks();
    g_UnusedEventData16APool.FreeBlocks();
    g_UnusedEventData20APool.FreeBlocks();
    g_LightningStrikeDataPool.FreeBlocks();
    g_CollisionBallWallDataPool.FreeBlocks();
    g_CollisionCrowdDataPool.FreeBlocks();
    g_CollisionPowerupGroundDataPool.FreeBlocks();
    g_CollisionBallGroundDataPool.FreeBlocks();
    g_CollisionPowerupWallDataPool.FreeBlocks();
    g_CollisionPlayerBallDataPool.FreeBlocks();
    g_UnusedEventData8APool.FreeBlocks();
    g_CollisionKoopaShellGoalieDataPool.FreeBlocks();
    g_CollisionKoopaShotBallPlayerDataPool.FreeBlocks();
    g_CollisionBirdoShotBallPlayerDataPool.FreeBlocks();
    g_CollisionBirdoEggGoalieDataPool.FreeBlocks();
    g_CollisionKoopaShellEndDataPool.FreeBlocks();
    g_CollisionBirdoEggEndDataPool.FreeBlocks();
    g_CollisionBallShellDataPool.FreeBlocks();
    g_CollisionBallChainDataPool.FreeBlocks();
    g_CollisionPlayerShellDataPool.FreeBlocks();
    g_CollisionPlayerFreezeDataPool.FreeBlocks();
    g_CollisionPlayerBananaDataPool.FreeBlocks();
    g_UnusedEventData28APool.FreeBlocks();
    g_UnusedEventData28BPool.FreeBlocks();
    g_CollisionPowerupStatsDataPool.FreeBlocks();
    g_CollisionBallGoalpostDataPool.FreeBlocks();
    g_BallNetmeshEventDataPool.FreeBlocks();
    g_UnusedEventData8BPool.FreeBlocks();
    g_UnusedEventData16BPool.FreeBlocks();
    g_UnusedEventData32APool.FreeBlocks();
    g_ShotAtGoalDataPool.FreeBlocks();
    g_UnusedEventData20BPool.FreeBlocks();
    g_PenaltyDataPool.FreeBlocks();
    g_UnusedEventData8CPool.FreeBlocks();
    g_UnusedEventData4APool.FreeBlocks();
    g_UnusedEventData4BPool.FreeBlocks();
    g_UnusedEventData8DPool.FreeBlocks();
    g_UnusedEventData36APool.FreeBlocks();
    g_PlayerAttackDataPool.FreeBlocks();
    g_NISDataPool.FreeBlocks();
    g_PowerupUsedEventDataPool.FreeBlocks();
    g_PowerupHitPlayerEventDataPool.FreeBlocks();
    g_UnusedEventData4CPool.FreeBlocks();
}
