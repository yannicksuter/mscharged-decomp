#include "Game/Physics/PhysicsColumn.h"

#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsWorld.h"
#include "NL/nlMath.h"
#include "ode/NLGAdditions.h"
#include "ode/ext/dColumn.h"

PhysicsColumn::PhysicsColumn(CollisionSpace* collisionSpace, PhysicsWorld* world, float radius)
    : PhysicsObject(world)
{
    dMass m;

    dSpaceID spaceID = 0;
    if (collisionSpace != 0)
    {
        spaceID = collisionSpace->m_spaceID;
    }

    m_geomID = dCreateColumn(spaceID, radius);

    if (m_bodyID != 0)
    {
        dGeomSetBody(m_geomID, m_bodyID);
        dMassSetCappedCylinderTotal(&m, 1.0f, 3, radius, 1.0f);
        dBodySetMass(m_bodyID, &m);
    }

    dGeomSetData(m_geomID, this);
    SetDefaultCollideBits();

    m_jointID = dJointCreateCharacter(world->m_World, 0);
    dJointAttach(m_jointID, m_bodyID, 0);

    nlVector4 v;
    nlVec3Set(*(nlVector3*)&v, 0.0f, 0.0f, 1.0f);
    dJointSetCharacterNoMotionDirection(m_jointID, (float*)&v);

    dBodySetAutoDisableFlag(m_bodyID, 0);
}

void PhysicsColumn::GetRadius(float* radius)
{
    dGeomColumnGetParams(m_geomID, radius);
}

void PhysicsColumn::SetRadius(float radius)
{
    dGeomColumnSetParams(m_geomID, radius);
}

PhysicsColumn::~PhysicsColumn()
{
}
