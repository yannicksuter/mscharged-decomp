#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H

#include "Game/EventDataTypes.h"
#include "NL/nlSlotPool.h"

extern SlotPool<UnidentifiedEventData26> lbl_80570110;
extern SlotPool<UnidentifiedEventData24> lbl_80570138;
extern SlotPool<UnidentifiedEventData34> lbl_80570188;

void QueueCollisionBallChain(CollisionBallChainData* data);
void QueueCollisionChainPowerup(CollisionChainPowerupData* data);
void QueueCollisionKoopaShotBallPlayer(CollisionKoopaShotBallPlayerData* data);
void QueueCollisionKoopaShellGoalie(CollisionKoopaShellGoalieData* data);
void QueueCollisionKoopaShellEnd(CollisionKoopaShellEndData* data);
void QueueCollisionBirdoShotBallPlayer(CollisionBirdoShotBallPlayerData* data);
void QueueCollisionBirdoEggGoalie(CollisionBirdoEggGoalieData* data);
void QueueCollisionBirdoEggEnd(CollisionBirdoEggEndData* data);
void QueueCollisionPatchPlayer(UnidentifiedEventData24* data);
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

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
