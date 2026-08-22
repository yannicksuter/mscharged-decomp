#ifndef GAME_PHYSICS_PHYSICS_PLANE_H
#define GAME_PHYSICS_PHYSICS_PLANE_H

#include "Game/Physics/PhysicsObject.h"

class CollisionSpace;

class PhysicsPlane : public PhysicsObject
{
public:
    PhysicsPlane(CollisionSpace*, float, float, float, float);
    virtual int GetObjectType() const { return 6; }
};

#endif
