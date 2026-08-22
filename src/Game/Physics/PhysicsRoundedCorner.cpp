#include "Game/Physics/PhysicsRoundedCorner.h"

#include "Game/Physics/CollisionSpace.h"
#include "ode/ext/dRoundedCorner.h"

PhysicsRoundedCorner::PhysicsRoundedCorner(
    CollisionSpace* collisionSpace, const nlVector2& position, float radius, bool isTrigger, bool isStatic)
    : PhysicsObject(0)
{
    dMass mass;

    dSpaceID space = 0;
    if (collisionSpace != 0)
    {
        space = collisionSpace->m_spaceID;
    }

    m_geomID = dCreateRoundedCorner(space, radius, isTrigger, isStatic);
    m_bodyID = 0;

    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();
    dGeomSetPosition(m_geomID, position.x, position.y, 0.0f);
}
