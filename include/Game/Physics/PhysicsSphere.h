#ifndef GAME_PHYSICS_PHYSICS_SPHERE_H
#define GAME_PHYSICS_PHYSICS_SPHERE_H

#include "Game/Physics/PhysicsObject.h"

class CollisionSpace;
class PhysicsWorld;

class PhysicsSphere : public PhysicsObject
{
public:
    PhysicsSphere(CollisionSpace*, PhysicsWorld*, float);
    virtual int GetObjectType() const { return 0xA; }

    float GetRadius();
    void SetRadius(float);
};

#endif
