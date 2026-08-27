#ifndef GAME_PHYSICS_PHYSICS_H
#define GAME_PHYSICS_PHYSICS_H

#include "Game/Physics/PhysicsWorld.h"

class CollisionSpace;
class PhysicsMesh;

void PhysicsUpdate(PhysicsWorld*, float);
void ODEFree(void*, unsigned long);
void* ODERealloc(void*, unsigned long, unsigned long);
void* ODEAlloc(unsigned long);

extern PhysicsMesh* g_TerrainMesh;
extern PhysicsWorld* g_PhysicsWorld;
extern CollisionSpace* g_CollisionSpace;

#endif // GAME_PHYSICS_PHYSICS_H
