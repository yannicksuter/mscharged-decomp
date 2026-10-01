#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H

#include "Game/EventDataTypes.h"
#include "NL/nlSlotPool.h"

class PhysicsEventQueue;
extern "C" PhysicsEventQueue* lbl_806E11F0;

extern SlotPool<UnidentifiedEventData26> lbl_80570110;
extern SlotPool<UnidentifiedEventData24> lbl_80570138;
extern SlotPool<UnidentifiedEventData30> lbl_80570160;
extern SlotPool<UnidentifiedEventData34> lbl_80570188;
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
extern "C" void fn_80148588(UnidentifiedEventData24* data);
extern "C" void fn_801486D0(UnidentifiedEventData30* data);
extern "C" void fn_80148818(UnidentifiedEventData28* data);
extern "C" void fn_80148954(UnidentifiedEventData24* data);
extern "C" void fn_80148A9C(UnidentifiedEventData31* data);
extern "C" void fn_80148BD8(UnidentifiedEventData31* data);
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
extern "C" void fn_8014A044(UnidentifiedNPC_801B43F8* data);


// Shared functions and data from Game/Physics/PhysicsEventQueue.cpp.
extern "C" void fn_80143FD4();
extern "C" void fn_80144070();
extern "C" void fn_801440BC();
extern "C" void fn_80144130(PhysicsEventQueue*);
extern "C" void fn_80144AB8();
extern "C" void fn_80145C9C();
extern "C" void fn_80145DD0(CollisionPlayerPlayerData*);
extern "C" void fn_80145F18(CollisionPlayerWallData*);
extern "C" void fn_80146060(UnidentifiedEventData24*);
extern "C" void fn_801461A8();
extern "C" void fn_801462DC(CollisionPlayerBallData*);
extern "C" void fn_80146424(BallNetmeshEventData*, bool);
extern "C" void fn_8014658C(CollisionBallGroundData*);
extern "C" void fn_801466D4(CollisionBallWallData*);
extern "C" void fn_8014681C(CollisionBallGoalpostData*);
extern "C" void fn_80146964(CollisionBallShellData*);
extern "C" void fn_801473A4(CollisionPowerupGroundData*);
extern "C" void fn_801474EC(CollisionPowerupGroundData*);
extern "C" void fn_80147634(CollisionPowerupWallData*);
extern "C" void fn_8014777C(PowerupHitPlayerEventData*);
extern "C" void fn_801478C4(CollisionPlayerBananaData*);
extern "C" void fn_80147A0C(CollisionPlayerShellData*);
extern "C" void fn_80147B54(CollisionPlayerFreezeData*);
extern "C" void fn_80147C9C(CollisionBulletBillData*);
extern "C" void fn_80147DE4(CollisionBulletBillData*);
extern "C" void fn_80148074(PowerupUsedEventData*);
extern "C" void fn_80149984(void* source, cCharacter* target);
extern "C" void fn_80149B30(UnidentifiedEventData33* data);
extern "C" void fn_8014A180(cFielder*);
extern "C" void fn_8016A658(void*);
extern "C" void fn_8016A670(void*);
extern "C" void fn_8016A688(CollisionPlayerBallData*);
extern "C" void fn_8016A6A0(void*);
extern "C" void fn_8016A6B8(CollisionBallGroundData*);
extern "C" void fn_8016A6D0(CollisionBallWallData*);
extern "C" void fn_8016A6E8(CollisionBallGoalpostData*);
extern "C" void fn_8016A700(CollisionBallShellData*);
extern "C" void fn_8016A730(CollisionKoopaShotBallPlayerData*);
extern "C" void fn_8016A748(CollisionKoopaShellGoalieData*);
extern "C" void fn_8016A760(void*);
extern "C" void fn_8016A778(CollisionBirdoShotBallPlayerData*);
extern "C" void fn_8016A790(CollisionBirdoEggGoalieData*);
extern "C" void fn_8016A7A8(void*);
extern "C" void fn_8016A7C0(void*);
extern "C" void fn_8016A7D8(CollisionPowerupWallData*);
extern "C" void fn_8016A7F0(void*);
extern "C" void fn_8016A808(CollisionPlayerBananaData*);
extern "C" void fn_8016A820(CollisionPlayerShellData*);
extern "C" void fn_8016A838(CollisionPlayerFreezeData*);
extern "C" void fn_8016A850(CollisionBulletBillData*);
extern "C" void fn_8016A868(void*);
extern "C" void fn_8016A898(void*);
extern "C" void fn_8016A8B0(UnidentifiedEventData26*);
extern "C" void fn_8016A8C8(CollisionThwompPlayerData*);
extern "C" void fn_8016A8E0(UnidentifiedEventData34*);

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
