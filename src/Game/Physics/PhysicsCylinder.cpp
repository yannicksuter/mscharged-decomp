#include "Game/Physics/PhysicsCylinder.h"

#include "Game/Physics/CollisionSpace.h"

PhysicsCylinder::PhysicsCylinder(
    CollisionSpace* collisionSpace, PhysicsWorld* world, float radius, float length)
    : PhysicsObject(world)
{
    dMass mass;

    dSpaceID space = 0;
    if (collisionSpace != 0)
    {
        space = collisionSpace->m_spaceID;
    }

    m_geomID = dCreateCylinder(space, radius, length);

    if (m_bodyID != 0)
    {
        dGeomSetBody(m_geomID, m_bodyID);
        dMassSetCappedCylinderTotal(&mass, 1.0f, 3, radius, length);
        dBodySetMass(m_bodyID, &mass);
    }

    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();
}

void PhysicsCylinder::SetRadius(float radius)
{
    float length;
    float oldRadius;

    dGeomCylinderGetParams(m_geomID, &oldRadius, &length);
    SetParams(radius, length);
}

void PhysicsCylinder::SetLength(float length)
{
    float radius;
    float oldLength;

    dGeomCylinderGetParams(m_geomID, &radius, &oldLength);
    SetParams(radius, length);
}
