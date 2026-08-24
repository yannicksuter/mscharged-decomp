#include "Game/Physics/PhysicsTransform.h"

PhysicsTransform::PhysicsTransform()
    : PhysicsObject(0)
{
    m_geomID = dCreateGeomTransform(0);
    dGeomTransformSetInfo(m_geomID, 1);
    m_SubObject = 0;
}

void PhysicsTransform::Unknown0()
{
    PhysicsObject::Unknown0();
    if (m_SubObject != 0)
    {
        m_SubObject->Unknown0();
    }
}

PhysicsTransform::~PhysicsTransform()
{
    delete m_SubObject;
}

void PhysicsTransform::Attach(PhysicsObject* object, PhysicsObject* parent)
{
    if (m_SubObject == 0)
    {
        if (parent != 0)
        {
            m_bodyID = parent->m_bodyID;
        }

        dGeomSetData(m_geomID, object);
        dGeomSetCategoryBits(m_geomID, dGeomGetCategoryBits(object->m_geomID));
        dGeomSetCollideBits(m_geomID, dGeomGetCollideBits(object->m_geomID));

        dSpaceID space = object->Disconnect();
        dGeomTransformSetGeom(m_geomID, object->m_geomID);
        EnableCollisions();
        dSpaceAdd(space, m_geomID);
        dGeomSetBody(m_geomID, m_bodyID);

        object->m_parentObject = parent;
        m_SubObject = object;
    }
}

void PhysicsTransform::Release()
{
    nlMatrix4 matrix;
    nlVector3 position;
    nlVector3 subObjectPosition;
    nlVector3 transformedPosition;

    if (m_SubObject != 0)
    {
        m_SubObject->m_parentObject = 0;

        GetPosition(&position);
        m_SubObject->GetPosition(&subObjectPosition);

        dSpaceID space = dGeomGetSpace(m_geomID);
        if (space != 0)
        {
            dSpaceRemove(space, m_geomID);
        }
        m_bodyID = 0;

        dGeomTransformSetGeom(m_geomID, 0);
        m_SubObject->Reconnect(space);
        GetRotation(&matrix);

        nlMultPosVectorMatrix(transformedPosition, subObjectPosition, matrix);
        position.z = position.z + transformedPosition.z;
        position.y = position.y + transformedPosition.y;
        position.x = position.x + transformedPosition.x;
        m_SubObject->SetPosition(position, WORLD_COORDINATES);

        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;
        m_SubObject->SetLinearVelocity(position);
        m_SubObject->SetAngularVelocity(position);
        m_SubObject->ZeroForceAccumulators();

        m_SubObject = 0;
        DisableCollisions();
    }
}

void PhysicsTransform::SetSubObjectPosition(
    const nlVector3& position, PhysicsObject::CoordinateType coordinateType)
{
    m_SubObject->SetPosition(position, coordinateType);
}

void PhysicsTransform::SetSubObjectTransform(
    const nlMatrix4& transform, PhysicsObject::CoordinateType coordinateType)
{
    nlVector3 position;
    m_SubObject->SetRotation(transform, coordinateType);
    float z = transform.e2[3][2];
    float y = transform.e2[3][1];
    float x = transform.e2[3][0];
    nlVec3Set(position, x, y, z);
    m_SubObject->SetPosition(position, coordinateType);
}
