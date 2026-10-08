#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H

#include "Game/EventDataTypes.h"
#include "NL/nlSlotPool.h"

class PhysicsEventQueue;
extern PhysicsEventQueue* gPhysicsEventQueue;

extern SlotPool<CollisionProjectileData> g_CollisionProjectileDataPool;
extern SlotPool<CollisionPatchData> g_CollisionPatchDataPool;
extern SlotPool<CollisionPatchPowerupData> g_CollisionPatchPowerupDataPool;
extern SlotPool<CollisionEggData> g_CollisionEggDataPool;
extern SlotPool<CollisionShockwaveData> gCollisionShockwaveDataPool;

void QueueCollisionBallChain(CollisionBallChainData* data);
void QueueCollisionChainCrowd(ChainChomp* data);
void QueueCollisionChainPowerup(CollisionChainPowerupData* data);
void QueueCollisionKoopaShotBallPlayer(CollisionKoopaShotBallPlayerData* data);
void QueueCollisionKoopaShellGoalie(CollisionKoopaShellGoalieData* data);
void QueueCollisionKoopaShellEnd(CollisionKoopaShellEndData* data);
void QueueCollisionBirdoShotBallPlayer(CollisionBirdoShotBallPlayerData* data);
void QueueCollisionBirdoEggGoalie(CollisionBirdoEggGoalieData* data);
void QueueCollisionBirdoEggEnd(CollisionBirdoEggEndData* data);
void QueueCollisionPatchPlayer(CollisionPatchData* data);
void QueueCollisionPatchGround(CollisionPatchData* data);
void QueueCollisionPatchPowerup(CollisionPatchPowerupData* data);
void QueueCollisionPatchChain(ChainChomp* data);
void QueueCollisionPatchPatch(CollisionPatchData* data);
void QueueCollisionPatchBall(PhysicsPatch* data);
void QueueCollisionPatchWall(PhysicsPatch* data);
void QueueCollisionHammerPlayer(CollisionProjectileData* data);
void QueueCollisionHammerGround(CollisionProjectileData* data);
void QueueCollisionHammerPowerup(PowerupBase* data);
void QueueCollisionHammerChain(ChainChomp* data);
void QueueBirdoEggDestroyPowerup(PowerupBase* data);
void QueueBirdoEggDestroyHammer(HammerObject* data);
void QueueBirdoEggKnockYoshiTongue(cFielder* data);
void QueueKoopaShellDestroyPowerup(PowerupBase* data);
void QueueKoopaShellDestroyHammer(HammerObject* data);
void QueueKoopaShellKnockYoshiTongue(cFielder* data);
void QueueCollisionEggBall(CollisionEggData* data);
void QueueCollisionEggPlayer(CollisionEggData* data);
void QueueCollisionCrackEgg(CollisionEggData* data);
void QueueCollisionDebrisBall(WindDebris* data);


// Shared functions and data from Game/Physics/PhysicsEventQueue.cpp.
void FreePhysicsEventDataPools();
void CreatePhysicsEventQueue();
void DestroyPhysicsEventQueue();
void DispatchPhysicsEvents(PhysicsEventQueue* queue);
void RegisterPhysicsEventHandlers();
void QueueBallFall();
void QueueCollisionPlayerPlayer(CollisionPlayerPlayerData*);
void QueueCollisionPlayerWall(CollisionPlayerWallData*);
void QueueCollisionTongue(CollisionPatchData*);
void QueueCollisionBallTronWall();
void QueueCollisionPlayerBall(CollisionPlayerBallData*);
void QueueCollisionBallNetmesh(BallNetmeshEventData*, bool);
void QueueCollisionBallGround(CollisionBallGroundData*);
void QueueCollisionBallWall(CollisionBallWallData*);
void QueueCollisionBallGoalpost(CollisionBallGoalpostData*);
void QueueCollisionBallShell(CollisionBallShellData*);
void QueueCollisionPowerupGround(CollisionPowerupGroundData*);
void QueueCollisionPowerupGoalie(CollisionPowerupGroundData*);
void QueueCollisionPowerupWall(CollisionPowerupWallData*);
void QueuePowerupHit(PowerupHitPlayerEventData*);
void QueueCollisionPlayerBanana(CollisionPlayerBananaData*);
void QueueCollisionPlayerShell(CollisionPlayerShellData*);
void QueueCollisionPlayerFreeze(CollisionPlayerFreezeData*);
void QueueCollisionBulletBillPlayer(CollisionBulletBillData*);
void QueueCollisionBulletBillFreeze(CollisionBulletBillData*);
void QueueExplosionBulletBill(CollisionBulletBillData*);
void QueuePowerupUsed(PowerupUsedEventData*);
void QueueCollisionThwompPlayer(ThwompObject* thwomp, cCharacter* target);
void QueueCollisionThwompBall(ThwompObject* data);
void QueueCollisionWaluigiWall(cFielder*);
void QueueCollisionShockwave(CollisionShockwaveData* data);
void FreeCollisionPlayerPlayerData(void*);
void FreeCollisionPatchData(void*);
void FreeCollisionPlayerBallData(CollisionPlayerBallData*);
void FreeBallNetmeshEventData(void*);
void FreeCollisionBallGroundData(CollisionBallGroundData*);
void FreeCollisionBallWallData(CollisionBallWallData*);
void FreeCollisionBallGoalpostData(CollisionBallGoalpostData*);
void FreeCollisionBallShellData(CollisionBallShellData*);
void FreeCollisionKoopaShotBallPlayerData(CollisionKoopaShotBallPlayerData*);
void FreeCollisionKoopaShellGoalieData(CollisionKoopaShellGoalieData*);
void FreeCollisionKoopaShellEndData(void*);
void FreeCollisionBirdoShotBallPlayerData(CollisionBirdoShotBallPlayerData*);
void FreeCollisionBirdoEggGoalieData(CollisionBirdoEggGoalieData*);
void FreeCollisionBirdoEggEndData(void*);
void FreeCollisionPowerupGroundData(void*);
void FreeCollisionPowerupWallData(CollisionPowerupWallData*);
void FreePowerupHitPlayerEventData(void*);
void FreeCollisionPlayerBananaData(CollisionPlayerBananaData*);
void FreeCollisionPlayerShellData(CollisionPlayerShellData*);
void FreeCollisionPlayerFreezeData(CollisionPlayerFreezeData*);
void FreeCollisionBulletBillData(CollisionBulletBillData*);
void FreePowerupUsedEventData(void*);
void FreeCollisionPatchPowerupData(void*);
void FreeCollisionProjectileData(CollisionProjectileData*);
void FreeCollisionThwompPlayerData(CollisionThwompPlayerData*);
void FreeCollisionEggData(CollisionEggData*);

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
