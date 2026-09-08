#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H

#include "Game/EventDataTypes.h"

void QueueCollisionBallChain(CollisionBallChainData* data);
void QueueCollisionChainPowerup(CollisionChainPowerupData* data);
extern "C" void fn_8014A044(UnidentifiedNPC_801B43F8* data);

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
