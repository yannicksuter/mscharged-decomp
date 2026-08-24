#include "Game/Physics/PhysicsCapsule.h"

#include "Game/Physics/CollisionSpace.h"

static inline void SetCapsuleParams(float radius, float length, dGeomID geom)
{
    dGeomCCylinderSetParams(geom, radius, length);
}

PhysicsCapsule::PhysicsCapsule(CollisionSpace* collisionSpace, PhysicsWorld* world, float radius, float length)
    : PhysicsObject(world)
{
    dMass mass;

    dSpaceID space = 0;
    if (collisionSpace != 0)
    {
        space = collisionSpace->m_spaceID;
    }

    m_geomID = dCreateCCylinder(space, radius, length);

    if (m_bodyID != 0)
    {
        dGeomSetBody(m_geomID, m_bodyID);
        dMassSetCappedCylinderTotal(&mass, 1.0f, 3, radius, length);
        dBodySetMass(m_bodyID, &mass);
    }

    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();
}

void PhysicsCapsule::SetRadius(float radius)
{
    float length;
    float oldRadius;

    dGeomCCylinderGetParams(m_geomID, &oldRadius, &length);
    SetCapsuleParams(radius, length, m_geomID);
}

void PhysicsCapsule::SetLength(float length)
{
    float radius;
    float oldLength;

    dGeomCCylinderGetParams(m_geomID, &radius, &oldLength);
    SetCapsuleParams(radius, length, m_geomID);
}
