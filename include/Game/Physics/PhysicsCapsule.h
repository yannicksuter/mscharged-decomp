#ifndef GAME_PHYSICS_PHYSICS_CAPSULE_H
#define GAME_PHYSICS_PHYSICS_CAPSULE_H

#include "Game/Physics/PhysicsObject.h"

class CollisionSpace;
class PhysicsWorld;

class PhysicsCapsule : public PhysicsObject
{
public:
    PhysicsCapsule(CollisionSpace*, PhysicsWorld*, float, float);

    void SetRadius(float);
    void SetLength(float);

    virtual int GetObjectType() const { return 2; }
};

#endif
