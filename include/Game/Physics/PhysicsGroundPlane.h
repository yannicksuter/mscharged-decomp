#ifndef GAME_PHYSICS_PHYSICS_GROUND_PLANE_H
#define GAME_PHYSICS_PHYSICS_GROUND_PLANE_H

#include "Game/Physics/PhysicsPlane.h"

class CollisionSpace;

class PhysicsGroundPlane : public PhysicsPlane
{
public:
    PhysicsGroundPlane(CollisionSpace*);
    virtual ~PhysicsGroundPlane() { }
    virtual int GetObjectType() const { return 0x11; }
};

#endif // GAME_PHYSICS_PHYSICS_GROUND_PLANE_H
