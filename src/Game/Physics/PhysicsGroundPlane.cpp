#include "Game/Physics/PhysicsGroundPlane.h"

PhysicsGroundPlane::PhysicsGroundPlane(CollisionSpace* collision_space)
    : PhysicsPlane(collision_space, 0.0f, 0.0f, 1.0f, 0.0f)
{
}
