#include "Game/Physics/PhysicsWall.h"

PhysicsWall::PhysicsWall(
    CollisionSpace* collision_space, float a, float b, float c)
    : PhysicsPlane(collision_space, a, b, 0.0f, -c)
{
}
