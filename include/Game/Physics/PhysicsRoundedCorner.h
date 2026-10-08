#ifndef GAME_PHYSICS_PHYSICS_ROUNDED_CORNER_H
#define GAME_PHYSICS_PHYSICS_ROUNDED_CORNER_H

#include "Game/Physics/PhysicsObject.h"
#include "NL/nlMath.h"

class CollisionSpace;

class PhysicsRoundedCorner : public PhysicsObject
{
public:
    PhysicsRoundedCorner(CollisionSpace* collisionSpace, const nlVector2& position, float radius, bool extendPositiveX, bool extendPositiveY);
    virtual int GetObjectType() const { return 5; }
};

#endif
