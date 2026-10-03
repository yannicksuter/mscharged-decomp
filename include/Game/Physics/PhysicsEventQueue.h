#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H

#include "Game/EventDataTypes.h"
#include "NL/nlSlotPool.h"

class PhysicsEventQueue;
extern PhysicsEventQueue* gPhysicsEventQueue;

extern SlotPool<UnidentifiedEventData26> g_UnidentifiedEventData26Pool;
extern SlotPool<UnidentifiedEventData24> g_UnidentifiedEventData24Pool;
extern SlotPool<UnidentifiedEventData30> g_UnidentifiedEventData30Pool;
extern SlotPool<UnidentifiedEventData34> g_UnidentifiedEventData34Pool;
extern SlotPool<CollisionShockwaveData> gCollisionShockwaveDataPool;

void QueueCollisionBallChain(CollisionBallChainData* data);
void QueueCollisionChainCrowd(UnidentifiedEventData28* data);
void QueueCollisionChainPowerup(CollisionChainPowerupData* data);
void QueueCollisionKoopaShotBallPlayer(CollisionKoopaShotBallPlayerData* data);
void QueueCollisionKoopaShellGoalie(CollisionKoopaShellGoalieData* data);
void QueueCollisionKoopaShellEnd(CollisionKoopaShellEndData* data);
void QueueCollisionBirdoShotBallPlayer(CollisionBirdoShotBallPlayerData* data);
void QueueCollisionBirdoEggGoalie(CollisionBirdoEggGoalieData* data);
void QueueCollisionBirdoEggEnd(CollisionBirdoEggEndData* data);
void QueueCollisionPatchPlayer(UnidentifiedEventData24* data);
void QueueCollisionPatchGround(UnidentifiedEventData24* data);
void QueueCollisionPatchPowerup(UnidentifiedEventData30* data);
void QueueCollisionPatchChain(UnidentifiedEventData28* data);
void QueueCollisionPatchPatch(UnidentifiedEventData24* data);
void QueueCollisionPatchBall(UnidentifiedEventData31* data);
void QueueCollisionPatchWall(UnidentifiedEventData31* data);
void QueueCollisionHammerPlayer(UnidentifiedEventData26* data);
void QueueCollisionHammerGround(UnidentifiedEventData26* data);
void QueueCollisionHammerPowerup(UnidentifiedEventData27* data);
void QueueCollisionHammerChain(UnidentifiedEventData28* data);
void QueueBirdoEggDestroyPowerup(UnidentifiedEventData27* data);
void QueueBirdoEggDestroyHammer(UnidentifiedEventData35* data);
void QueueBirdoEggKnockYoshiTongue(cFielder* data);
void QueueKoopaShellDestroyPowerup(UnidentifiedEventData27* data);
void QueueKoopaShellDestroyHammer(UnidentifiedEventData35* data);
void QueueKoopaShellKnockYoshiTongue(cFielder* data);
void QueueCollisionEggBall(UnidentifiedEventData34* data);
void QueueCollisionEggPlayer(UnidentifiedEventData34* data);
void QueueCollisionCrackEgg(UnidentifiedEventData34* data);
void QueueCollisionDebrisBall(UnidentifiedNPC_801B43F8* data);


// Shared functions and data from Game/Physics/PhysicsEventQueue.cpp.
void FreePhysicsEventDataPools();
void CreatePhysicsEventQueue();
void DestroyPhysicsEventQueue();
void DispatchPhysicsEvents(PhysicsEventQueue* queue);
void RegisterPhysicsEventHandlers();
void QueueBallFall();
void QueueCollisionPlayerPlayer(CollisionPlayerPlayerData*);
void QueueCollisionPlayerWall(CollisionPlayerWallData*);
void QueueCollisionTongue(UnidentifiedEventData24*);
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
void QueueCollisionThwompPlayer(void* source, cCharacter* target);
void QueueCollisionThwompBall(UnidentifiedEventData33* data);
void QueueCollisionWaluigiWall(cFielder*);
void QueueCollisionShockwave(CollisionShockwaveData* data);
void FreeCollisionPlayerPlayerData(void*);
void FreeUnidentifiedEventData24(void*);
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
void FreeUnidentifiedEventData30(void*);
void FreeUnidentifiedEventData26(UnidentifiedEventData26*);
void FreeCollisionThwompPlayerData(CollisionThwompPlayerData*);
void FreeUnidentifiedEventData34(UnidentifiedEventData34*);

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
