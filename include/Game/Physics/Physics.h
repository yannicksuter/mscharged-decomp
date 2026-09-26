#ifndef GAME_PHYSICS_PHYSICS_H
#define GAME_PHYSICS_PHYSICS_H

#include "Game/Physics/PhysicsWorld.h"
#include "NL/nlListContainer.h"

class CollisionSpace;
class PhysicsMesh;
class PhysicsObject;

void PhysicsUpdate(PhysicsWorld*, float);
void ODEFree(void*, unsigned long);
void* ODERealloc(void*, unsigned long, unsigned long);
void* ODEAlloc(unsigned long);

extern PhysicsMesh* g_TerrainMesh;
extern PhysicsWorld* g_PhysicsWorld;
extern CollisionSpace* g_CollisionSpace;
extern nlListContainer<PhysicsObject*> g_StaticPhysicsPrimitives;
extern nlListContainer<PhysicsObject*> g_NetPhysicsObjects;

#endif // GAME_PHYSICS_PHYSICS_H
