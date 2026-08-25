#ifndef GAME_PHYSICS_PHYSICS_CYLINDER_H
#define GAME_PHYSICS_PHYSICS_CYLINDER_H

#include "Game/Physics/PhysicsObject.h"
#include "ode/ext/dCylinder.h"

class CollisionSpace;
class PhysicsWorld;

class PhysicsCylinder : public PhysicsObject
{
public:
    PhysicsCylinder(CollisionSpace*, PhysicsWorld*, float, float);

    void SetRadius(float);
    void SetLength(float);

    virtual int GetObjectType() const { return 3; }

private:
    void SetParams(float radius, float length)
    {
        dGeomCylinderSetParams(m_geomID, radius, length);
    }
};

#endif
