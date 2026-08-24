#ifndef GAME_PHYSICS_PHYSICS_BOX_H
#define GAME_PHYSICS_PHYSICS_BOX_H

#include "Game/Physics/PhysicsObject.h"

class CollisionSpace;
class PhysicsWorld;

class PhysicsBox : public PhysicsObject
{
public:
    PhysicsBox(CollisionSpace*, PhysicsWorld*, float, float, float);

    void GetSides(float*);
    void SetSides(float, float, float);

    virtual int GetObjectType() const { return 1; }
};

#endif
