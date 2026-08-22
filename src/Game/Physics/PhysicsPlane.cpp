#include "Game/Physics/PhysicsPlane.h"

#include "Game/Physics/CollisionSpace.h"

PhysicsPlane::PhysicsPlane(CollisionSpace* collisionSpace, float a, float b, float c, float d)
    : PhysicsObject(0)
{
    dSpaceID space = 0;
    if (collisionSpace != 0)
    {
        space = collisionSpace->m_spaceID;
    }

    m_geomID = dCreatePlane(space, a, b, c, d);
    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();
}
