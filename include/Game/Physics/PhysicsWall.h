#ifndef GAME_PHYSICS_PHYSICS_WALL_H
#define GAME_PHYSICS_PHYSICS_WALL_H

#include "Game/Physics/PhysicsPlane.h"

class CollisionSpace;

class PhysicsWall : public PhysicsPlane
{
public:
    PhysicsWall(CollisionSpace*, float, float, float);
    virtual ~PhysicsWall() { }
    virtual int GetObjectType() const { return 0x19; }
};

#endif // GAME_PHYSICS_PHYSICS_WALL_H
