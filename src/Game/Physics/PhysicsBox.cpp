#include "Game/Physics/PhysicsBox.h"

#include "Game/Physics/CollisionSpace.h"

PhysicsBox::PhysicsBox(CollisionSpace* collisionSpace, PhysicsWorld* world, float lx, float ly, float lz)
    : PhysicsObject(world)
{
    dMass mass;

    dSpaceID space = 0;
    if (collisionSpace != 0)
    {
        space = collisionSpace->m_spaceID;
    }

    m_geomID = dCreateBox(space, lx, ly, lz);

    if (m_bodyID != 0)
    {
        dGeomSetBody(m_geomID, m_bodyID);
        dMassSetBoxTotal(&mass, lx * ly * lz, lx, ly, lz);
        dBodySetMass(m_bodyID, &mass);
    }

    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();
}

void PhysicsBox::GetSides(float* sides)
{
    dVector3 dimensions;

    dGeomBoxGetLengths(m_geomID, dimensions);
    sides[0] = dimensions[0];
    sides[1] = dimensions[1];
    sides[2] = dimensions[2];
}

void PhysicsBox::SetSides(float lx, float ly, float lz)
{
    dGeomBoxSetLengths(m_geomID, lx, ly, lz);
}
