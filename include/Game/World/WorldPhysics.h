#ifndef GAME_WORLD_WORLD_PHYSICS_H
#define GAME_WORLD_WORLD_PHYSICS_H

struct WorldPhysicsDescription;
class CollisionSpace;
class PhysicsObject;
class WorldPhysicsDrawable;

PhysicsObject* CreatePhysicsPrimitive(
    const WorldPhysicsDescription* pDescription, CollisionSpace* pCollisionSpace);
void ReleaseWorldPhysicsObject(WorldPhysicsDrawable* pOwner);

#endif // GAME_WORLD_WORLD_PHYSICS_H
