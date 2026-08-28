#ifndef GAME_PHYSICS_PHYSICS_WALL_H
#define GAME_PHYSICS_PHYSICS_WALL_H

#include "Game/Physics/PhysicsPlane.h"

class CollisionSpace;

class PhysicsWall : public PhysicsPlane
{
public:
    PhysicsWall(CollisionSpace* collision_space, float a, float b, float c);
    virtual ~PhysicsWall() { }
    virtual int GetObjectType() const { return 0x17; }
};

#endif // GAME_PHYSICS_PHYSICS_WALL_H
