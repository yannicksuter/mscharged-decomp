#include "Game/Physics/PhysicsSphere.h"

#include "Game/Physics/CollisionSpace.h"

PhysicsSphere::PhysicsSphere(CollisionSpace* collisionSpace, PhysicsWorld* world, float radius)
    : PhysicsObject(world)
{
    dMass mass;

    dSpaceID space = 0;
    if (collisionSpace != 0)
    {
        space = collisionSpace->m_spaceID;
    }

    m_geomID = dCreateSphere(space, radius);

    if (m_bodyID != 0)
    {
        dGeomSetBody(m_geomID, m_bodyID);
        dMassSetSphereTotal(&mass, 1.0f, radius);
        dBodySetMass(m_bodyID, &mass);
    }

    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();
}

float PhysicsSphere::GetRadius()
{
    return dGeomSphereGetRadius(m_geomID);
}

void PhysicsSphere::SetRadius(float radius)
{
    dGeomSphereSetRadius(m_geomID, radius);
}
