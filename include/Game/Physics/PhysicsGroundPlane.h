#ifndef GAME_PHYSICS_PHYSICS_GROUND_PLANE_H
#define GAME_PHYSICS_PHYSICS_GROUND_PLANE_H

#include "Game/Physics/PhysicsPlane.h"

class CollisionSpace;

class PhysicsGroundPlane : public PhysicsPlane
{
public:
    PhysicsGroundPlane(CollisionSpace* collision_space);
    virtual int GetObjectType() const { return PHYSOBJ_GROUND_PLANE; }
};

#endif // GAME_PHYSICS_PHYSICS_GROUND_PLANE_H
