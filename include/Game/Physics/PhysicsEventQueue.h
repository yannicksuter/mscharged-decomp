#ifndef GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
#define GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H

#include "Game/EventDataTypes.h"

void QueueCollisionBallChain(CollisionBallChainData* data);
void QueueCollisionChainPowerup(CollisionChainPowerupData* data);

#endif // GAME_PHYSICS_PHYSICS_EVENT_QUEUE_H
