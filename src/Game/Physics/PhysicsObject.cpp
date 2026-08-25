#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsWorld.h"
#include "NL/platvmath.h"
#include "ode/mass.h"
#include "NL/nlMath.h"

float PhysicsObject::DefaultGravity = -9.8f;

void ConvertDMat3ToNLMat4(const float* src, nlMatrix4* dest);
void ConvertNLMat4ToDMat3(const nlMatrix4& src, float* dest);

PhysicsObject::PhysicsObject(PhysicsWorld* world)
{
    m_bodyID = 0;
    m_geomID = 0;
    m_parentObject = 0;
    m_gravity = DefaultGravity;
    m_contactHandler = 0;
    m_unknown30 = 0;
    m_unknown34 = 0;

    if (world != 0)
    {
        m_bodyID = dBodyCreate(world->m_World);
        dBodySetData(m_bodyID, this);
        dBodySetGravityMode(m_bodyID, 0);
    }
}

PhysicsObject::~PhysicsObject()
{
    if (m_bodyID != 0)
    {
        dBodyDestroy(m_bodyID);
        m_bodyID = 0;
    }

    if (m_geomID != 0)
    {
        dGeomDestroy(m_geomID);
        m_geomID = 0;
    }
}

void PhysicsObject::SetDefaultCollideBits()
{
    dGeomSetCollideBits(m_geomID, 2);
    dGeomSetCategoryBits(m_geomID, 2);
}

void PhysicsObject::SetCollide(unsigned int collideBits)
{
    dGeomSetCollideBits(m_geomID, collideBits);
}

void PhysicsObject::SetCategory(unsigned int categoryBits)
{
    dGeomSetCategoryBits(m_geomID, categoryBits);
}

void PhysicsObject::CheckForNaN()
{
    dBodyGetForce(m_bodyID);
    dBodyGetTorque(m_bodyID);
    GetPosition();
    GetLinearVelocity();
}

void PhysicsObject::PreUpdate()
{
    bool enabled = (bool)dBodyIsEnabled(m_bodyID);
    if (enabled && m_bodyID != 0 && m_parentObject == 0)
    {
        dMass m;
        dBodyGetMass(m_bodyID, &m);
        dBodyAddForce(m_bodyID, 0.f, 0.f, m_gravity * m.mass);
    }

    dBodyGetForce(m_bodyID);
    dBodyGetTorque(m_bodyID);

    {
        nlVector3 position1;
        nlVector3 transformedPosition;
        nlVector3 position2;
        nlVector3 parentPosition;
        nlMatrix4 rotation1;
        nlMatrix4 rotation2;
        nlMatrix4 outerResult;
        nlMatrix4 rotation3;
        nlMatrix4 middleResult;
        nlMatrix4 deepResult;
        nlMatrix4 ancestorRotation;
        nlMatrix4 parentRotation;
        const float* values;
        dGeomID geomID = m_geomID;

        if (geomID == 0 && m_bodyID != 0)
        {
            values = dBodyGetPosition(m_bodyID);
        }
        else
        {
            values = dGeomGetPosition(geomID);
        }

        PhysicsObject* parent = m_parentObject;
        float positionZ;
        float positionY;
        float positionX;
        positionZ = values[2];
        positionY = values[1];
        positionX = values[0];
        m_position.x = positionX;
        m_position.y = positionY;
        m_position.z = positionZ;

        if (parent != 0)
        {
            if (parent->m_geomID == 0 && parent->m_bodyID != 0)
            {
                values = dBodyGetPosition(parent->m_bodyID);
            }
            else
            {
                values = dGeomGetPosition(parent->m_geomID);
            }
            float parentZ;
            float parentY;
            float parentX;
            parentZ = values[2];
            parentY = values[1];
            parentX = values[0];
            parentPosition.x = parentX;
            parentPosition.y = parentY;
            parentPosition.z = parentZ;

            if (parent->m_parentObject != 0)
            {
                parent->m_parentObject->GetPosition(&position2);
                parent->m_parentObject->GetRotation(&ancestorRotation);
                nlMultPosVectorMatrix(
                    position1, parentPosition, ancestorRotation);
                parentPosition = position1;
                nlVec3Add(parentPosition, position2, parentPosition);
            }

            parent = m_parentObject;
            if (parent->m_geomID == 0 && parent->m_bodyID != 0)
            {
                values = dBodyGetRotation(parent->m_bodyID);
            }
            else
            {
                values = dGeomGetRotation(parent->m_geomID);
            }

            parentRotation.e2[0][0] = values[0];
            parentRotation.e2[1][0] = values[1];
            parentRotation.e2[2][0] = values[2];
            parentRotation.e2[0][1] = values[4];
            parentRotation.e2[1][1] = values[5];
            parentRotation.e2[2][1] = values[6];
            parentRotation.e2[0][2] = values[8];
            parentRotation.e2[1][2] = values[9];
            parentRotation.e2[2][2] = values[10];
            parentRotation.e2[3][0] = 0.0f;
            parentRotation.e2[3][1] = 0.0f;
            parentRotation.e2[3][2] = 0.0f;
            parentRotation.e2[3][3] = 1.0f;
            parentRotation.e2[0][3] = 0.0f;
            parentRotation.e2[1][3] = 0.0f;
            parentRotation.e2[2][3] = 0.0f;

            PhysicsObject* grandParent = parent->m_parentObject;
            if (grandParent != 0)
            {
                if (grandParent->m_geomID == 0
                    && grandParent->m_bodyID != 0)
                {
                    values = dBodyGetRotation(grandParent->m_bodyID);
                }
                else
                {
                    values = dGeomGetRotation(grandParent->m_geomID);
                }
                ConvertDMat3ToNLMat4(values, &rotation1);

                PhysicsObject* greatGrandParent =
                    grandParent->m_parentObject;
                if (greatGrandParent != 0)
                {
                    if (greatGrandParent->m_geomID == 0
                        && greatGrandParent->m_bodyID != 0)
                    {
                        values =
                            dBodyGetRotation(greatGrandParent->m_bodyID);
                    }
                    else
                    {
                        values =
                            dGeomGetRotation(greatGrandParent->m_geomID);
                    }
                    ConvertDMat3ToNLMat4(values, &rotation2);

                    if (greatGrandParent->m_parentObject != 0)
                    {
                        greatGrandParent->m_parentObject->GetRotation(
                            &rotation3);
                        nlMultMatrices(deepResult, rotation2, rotation3);
                        rotation2 = deepResult;
                    }
                    nlMultMatrices(middleResult, rotation1, rotation2);
                    rotation1 = middleResult;
                }
                nlMultMatrices(outerResult, parentRotation, rotation1);
                parentRotation = outerResult;
            }

            nlMultPosVectorMatrix(
                transformedPosition, m_position, parentRotation);
            m_position = transformedPosition;
            nlVec3Add(m_position, parentPosition, m_position);
        }
    }

    {
        PhysicsObject* parent1;
        PhysicsObject* parent2;
        PhysicsObject* parent3;
        PhysicsObject* parent4;

        parent1 = m_parentObject;
        if (parent1 != 0)
        {
            parent2 = parent1->m_parentObject;
            if (parent2 != 0)
            {
                parent3 = parent2->m_parentObject;
                if (parent3 != 0)
                {
                    parent4 = parent3->m_parentObject;
                    if (parent4 != 0)
                    {
                        parent4->GetLinearVelocity(&m_linearVelocity);
                        return;
                    }
                    const float* values =
                        dBodyGetLinearVel(parent3->m_bodyID);
                    nlVec3Set(m_linearVelocity,
                        values[0],
                        values[1],
                        values[2]);
                    return;
                }
                const float* values = dBodyGetLinearVel(parent2->m_bodyID);
                float z;
                float y;
                float x;
                z = values[2];
                y = values[1];
                x = values[0];
                m_linearVelocity.x = x;
                m_linearVelocity.y = y;
                m_linearVelocity.z = z;
                return;
            }
            const float* values = dBodyGetLinearVel(parent1->m_bodyID);
            float z;
            float y;
            float x;
            z = values[2];
            y = values[1];
            x = values[0];
            m_linearVelocity.x = x;
            m_linearVelocity.y = y;
            m_linearVelocity.z = z;
            return;
        }
        const float* values = dBodyGetLinearVel(m_bodyID);
        float z;
        float y;
        float x;
        z = values[2];
        y = values[1];
        x = values[0];
        m_linearVelocity.x = x;
        m_linearVelocity.y = y;
        m_linearVelocity.z = z;
    }
}


void PhysicsObject::PostUpdate()
{
    dBodyGetForce(m_bodyID);
    dBodyGetTorque(m_bodyID);

    {
        nlVector3 position1;
        nlVector3 transformedPosition;
        nlVector3 position2;
        nlVector3 parentPosition;
        nlMatrix4 rotation1;
        nlMatrix4 rotation2;
        nlMatrix4 outerResult;
        nlMatrix4 rotation3;
        nlMatrix4 middleResult;
        nlMatrix4 deepResult;
        nlMatrix4 ancestorRotation;
        nlMatrix4 parentRotation;
        const float* values;
        dGeomID geomID = m_geomID;

        if (geomID == 0 && m_bodyID != 0)
        {
            values = dBodyGetPosition(m_bodyID);
        }
        else
        {
            values = dGeomGetPosition(geomID);
        }

        PhysicsObject* parent = m_parentObject;
        float positionZ;
        float positionY;
        float positionX;
        positionZ = values[2];
        positionY = values[1];
        positionX = values[0];
        m_position.x = positionX;
        m_position.y = positionY;
        m_position.z = positionZ;

        if (parent != 0)
        {
            if (parent->m_geomID == 0 && parent->m_bodyID != 0)
            {
                values = dBodyGetPosition(parent->m_bodyID);
            }
            else
            {
                values = dGeomGetPosition(parent->m_geomID);
            }
            float parentZ;
            float parentY;
            float parentX;
            parentZ = values[2];
            parentY = values[1];
            parentX = values[0];
            parentPosition.x = parentX;
            parentPosition.y = parentY;
            parentPosition.z = parentZ;

            if (parent->m_parentObject != 0)
            {
                parent->m_parentObject->GetPosition(&position2);
                parent->m_parentObject->GetRotation(&ancestorRotation);
                nlMultPosVectorMatrix(
                    position1, parentPosition, ancestorRotation);
                parentPosition = position1;
                nlVec3Add(parentPosition, position2, parentPosition);
            }

            parent = m_parentObject;
            if (parent->m_geomID == 0 && parent->m_bodyID != 0)
            {
                values = dBodyGetRotation(parent->m_bodyID);
            }
            else
            {
                values = dGeomGetRotation(parent->m_geomID);
            }

            parentRotation.e2[0][0] = values[0];
            parentRotation.e2[1][0] = values[1];
            parentRotation.e2[2][0] = values[2];
            parentRotation.e2[0][1] = values[4];
            parentRotation.e2[1][1] = values[5];
            parentRotation.e2[2][1] = values[6];
            parentRotation.e2[0][2] = values[8];
            parentRotation.e2[1][2] = values[9];
            parentRotation.e2[2][2] = values[10];
            parentRotation.e2[3][0] = 0.0f;
            parentRotation.e2[3][1] = 0.0f;
            parentRotation.e2[3][2] = 0.0f;
            parentRotation.e2[3][3] = 1.0f;
            parentRotation.e2[0][3] = 0.0f;
            parentRotation.e2[1][3] = 0.0f;
            parentRotation.e2[2][3] = 0.0f;

            PhysicsObject* grandParent = parent->m_parentObject;
            if (grandParent != 0)
            {
                if (grandParent->m_geomID == 0
                    && grandParent->m_bodyID != 0)
                {
                    values = dBodyGetRotation(grandParent->m_bodyID);
                }
                else
                {
                    values = dGeomGetRotation(grandParent->m_geomID);
                }
                ConvertDMat3ToNLMat4(values, &rotation1);

                PhysicsObject* greatGrandParent =
                    grandParent->m_parentObject;
                if (greatGrandParent != 0)
                {
                    if (greatGrandParent->m_geomID == 0
                        && greatGrandParent->m_bodyID != 0)
                    {
                        values =
                            dBodyGetRotation(greatGrandParent->m_bodyID);
                    }
                    else
                    {
                        values =
                            dGeomGetRotation(greatGrandParent->m_geomID);
                    }
                    ConvertDMat3ToNLMat4(values, &rotation2);

                    if (greatGrandParent->m_parentObject != 0)
                    {
                        greatGrandParent->m_parentObject->GetRotation(
                            &rotation3);
                        nlMultMatrices(deepResult, rotation2, rotation3);
                        rotation2 = deepResult;
                    }
                    nlMultMatrices(middleResult, rotation1, rotation2);
                    rotation1 = middleResult;
                }
                nlMultMatrices(outerResult, parentRotation, rotation1);
                parentRotation = outerResult;
            }

            nlMultPosVectorMatrix(
                transformedPosition, m_position, parentRotation);
            m_position = transformedPosition;
            nlVec3Add(m_position, parentPosition, m_position);
        }
    }

    {
        PhysicsObject* parent1;
        PhysicsObject* parent2;
        PhysicsObject* parent3;
        PhysicsObject* parent4;

        parent1 = m_parentObject;
        if (parent1 != 0)
        {
            parent2 = parent1->m_parentObject;
            if (parent2 != 0)
            {
                parent3 = parent2->m_parentObject;
                if (parent3 != 0)
                {
                    parent4 = parent3->m_parentObject;
                    if (parent4 != 0)
                    {
                        parent4->GetLinearVelocity(&m_linearVelocity);
                        return;
                    }
                    const float* values =
                        dBodyGetLinearVel(parent3->m_bodyID);
                    nlVec3Set(m_linearVelocity,
                        values[0],
                        values[1],
                        values[2]);
                    return;
                }
                const float* values = dBodyGetLinearVel(parent2->m_bodyID);
                float z;
                float y;
                float x;
                z = values[2];
                y = values[1];
                x = values[0];
                m_linearVelocity.x = x;
                m_linearVelocity.y = y;
                m_linearVelocity.z = z;
                return;
            }
            const float* values = dBodyGetLinearVel(parent1->m_bodyID);
            float z;
            float y;
            float x;
            z = values[2];
            y = values[1];
            x = values[0];
            m_linearVelocity.x = x;
            m_linearVelocity.y = y;
            m_linearVelocity.z = z;
            return;
        }
        const float* values = dBodyGetLinearVel(m_bodyID);
        float z;
        float y;
        float x;
        z = values[2];
        y = values[1];
        x = values[0];
        m_linearVelocity.x = x;
        m_linearVelocity.y = y;
        m_linearVelocity.z = z;
    }
}


void PhysicsObject::SetPosition(const nlVector3& pos, PhysicsObject::CoordinateType type)
{
    nlMatrix4 rot;
    nlMatrix4 inv_rot;
    nlVector3 _pos;
    PhysicsObject* parentObj;
    PhysicsObject* parent_parentObj = m_parentObject;

    if ((parent_parentObj != 0) && (type == 0))
    {
        {
            nlMatrix4 deepResult;
            nlMatrix4 middleResult;
            nlMatrix4 rot_tmp2;
            nlMatrix4 finalResult;
            nlMatrix4 rot_1;
            nlMatrix4 parentRot;
            const float* dRot;
            if (parent_parentObj->m_geomID == 0
                && parent_parentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(parent_parentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(parent_parentObj->m_geomID);
            }

            rot.e2[0][0] = dRot[0];
            rot.e2[1][0] = dRot[1];
            rot.e2[2][0] = dRot[2];
            rot.e2[0][1] = dRot[4];
            rot.e2[1][1] = dRot[5];
            rot.e2[2][1] = dRot[6];
            rot.e2[0][2] = dRot[8];
            rot.e2[1][2] = dRot[9];
            rot.e2[2][2] = dRot[10];
            rot.e2[3][0] = 0.0f;
            rot.e2[3][1] = 0.0f;
            rot.e2[3][2] = 0.0f;
            rot.e2[3][3] = 1.0f;
            rot.e2[0][3] = 0.0f;
            rot.e2[1][3] = 0.0f;
            rot.e2[2][3] = 0.0f;

            if ((parent_parentObj = parent_parentObj->m_parentObject) != 0)
            {
                if (parent_parentObj->m_geomID == 0
                    && parent_parentObj->m_bodyID != 0)
                {
                    dRot = dBodyGetRotation(parent_parentObj->m_bodyID);
                }
                else
                {
                    dRot = dGeomGetRotation(parent_parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(dRot, &parentRot);

                if ((parent_parentObj = parent_parentObj->m_parentObject) != 0)
                {
                    if (parent_parentObj->m_geomID == 0
                        && parent_parentObj->m_bodyID != 0)
                    {
                        dRot = dBodyGetRotation(parent_parentObj->m_bodyID);
                    }
                    else
                    {
                        dRot = dGeomGetRotation(parent_parentObj->m_geomID);
                    }
                    ConvertDMat3ToNLMat4(dRot, &rot_1);

                    if (parent_parentObj->m_parentObject != 0)
                    {
                        parent_parentObj->m_parentObject->GetRotation(
                            &rot_tmp2);
                        nlMultMatrices(deepResult, rot_1, rot_tmp2);
                        rot_1 = deepResult;
                    }
                    nlMultMatrices(middleResult, parentRot, rot_1);
                    parentRot = middleResult;
                }

                nlMultMatrices(finalResult, rot, parentRot);
                rot = finalResult;
            }
        }

        parentObj = m_parentObject;
        {
            nlVector3 parentPos;
            nlVector3 pos_2;
            nlVector3 transformedPosition;
            nlVector3 pos_1;
            nlMatrix4 parentRot;
            nlMatrix4 ancestorRotation;
            nlMatrix4 deepResult;
            nlMatrix4 middleResult;
            nlMatrix4 rotation3;
            nlMatrix4 outerResult;
            nlMatrix4 rot_2;
            nlMatrix4 rot_1;
            const float* _retFloats;
            dGeomID geomID = parentObj->m_geomID;

            if (geomID == 0 && parentObj->m_bodyID != 0)
            {
                _retFloats = dBodyGetPosition(parentObj->m_bodyID);
            }
            else
            {
                _retFloats = dGeomGetPosition(geomID);
            }

            nlVec3Set(parentObj->m_position,
                _retFloats[0],
                _retFloats[1],
                _retFloats[2]);
            parent_parentObj = parentObj->m_parentObject;

            if (parent_parentObj != 0)
            {
                if (parent_parentObj->m_geomID == 0
                    && parent_parentObj->m_bodyID != 0)
                {
                    _retFloats =
                        dBodyGetPosition(parent_parentObj->m_bodyID);
                }
                else
                {
                    _retFloats =
                        dGeomGetPosition(parent_parentObj->m_geomID);
                }
                nlVec3Set(parentPos,
                    _retFloats[0],
                    _retFloats[1],
                    _retFloats[2]);

                if (parent_parentObj->m_parentObject != 0)
                {
                    parent_parentObj->m_parentObject->GetPosition(&pos_2);
                    parent_parentObj->m_parentObject->GetRotation(
                        &ancestorRotation);
                    nlMultPosVectorMatrix(
                        pos_1, parentPos, ancestorRotation);
                    parentPos = pos_1;
                    nlVec3Add(parentPos, pos_2, parentPos);
                }

                parent_parentObj = parentObj->m_parentObject;
                if (parent_parentObj->m_geomID == 0
                    && parent_parentObj->m_bodyID != 0)
                {
                    _retFloats =
                        dBodyGetRotation(parent_parentObj->m_bodyID);
                }
                else
                {
                    _retFloats =
                        dGeomGetRotation(parent_parentObj->m_geomID);
                }

                parentRot.e2[0][0] = _retFloats[0];
                parentRot.e2[1][0] = _retFloats[1];
                parentRot.e2[2][0] = _retFloats[2];
                parentRot.e2[0][1] = _retFloats[4];
                parentRot.e2[1][1] = _retFloats[5];
                parentRot.e2[2][1] = _retFloats[6];
                parentRot.e2[0][2] = _retFloats[8];
                parentRot.e2[1][2] = _retFloats[9];
                parentRot.e2[2][2] = _retFloats[10];
                parentRot.e2[3][0] = 0.0f;
                parentRot.e2[3][1] = 0.0f;
                parentRot.e2[3][2] = 0.0f;
                parentRot.e2[3][3] = 1.0f;
                parentRot.e2[0][3] = 0.0f;
                parentRot.e2[1][3] = 0.0f;
                parentRot.e2[2][3] = 0.0f;

                PhysicsObject* temp_r28_2 =
                    parent_parentObj->m_parentObject;
                if (temp_r28_2 != 0)
                {
                    if (temp_r28_2->m_geomID == 0
                        && temp_r28_2->m_bodyID != 0)
                    {
                        _retFloats =
                            dBodyGetRotation(temp_r28_2->m_bodyID);
                    }
                    else
                    {
                        _retFloats =
                            dGeomGetRotation(temp_r28_2->m_geomID);
                    }
                    ConvertDMat3ToNLMat4(_retFloats, &rot_1);

                    PhysicsObject* greatGrandParent =
                        temp_r28_2->m_parentObject;
                    if (greatGrandParent != 0)
                    {
                        if (greatGrandParent->m_geomID == 0
                            && greatGrandParent->m_bodyID != 0)
                        {
                            _retFloats =
                                dBodyGetRotation(greatGrandParent->m_bodyID);
                        }
                        else
                        {
                            _retFloats =
                                dGeomGetRotation(greatGrandParent->m_geomID);
                        }
                        ConvertDMat3ToNLMat4(_retFloats, &rot_2);

                        if (greatGrandParent->m_parentObject != 0)
                        {
                            greatGrandParent->m_parentObject->GetRotation(
                                &rotation3);
                            nlMultMatrices(deepResult, rot_2, rotation3);
                            rot_2 = deepResult;
                        }
                        nlMultMatrices(middleResult, rot_1, rot_2);
                        rot_1 = middleResult;
                    }
                    nlMultMatrices(outerResult, parentRot, rot_1);
                    parentRot = outerResult;
                }

                nlMultPosVectorMatrix(transformedPosition,
                    parentObj->m_position,
                    parentRot);
                parentObj->m_position = transformedPosition;
                nlVec3Add(
                    parentObj->m_position, parentPos, parentObj->m_position);
            }
        }

        nlVector3& p = parentObj->m_position;

        rot.e2[3][0] = p.x;
        rot.e2[3][1] = p.y;
        rot.e2[3][2] = p.z;
        rot.e2[3][3] = 1.0f;

        nlInvertRotTransMatrix(inv_rot, rot);
        nlMultPosVectorMatrix(_pos, pos, inv_rot);
        if ((m_geomID == 0) && (m_bodyID != 0))
        {
            dBodySetPosition(m_bodyID, _pos.x, _pos.y, _pos.z);
            return;
        }
        dGeomSetPosition(m_geomID, _pos.x, _pos.y, _pos.z);
        return;
    }

    if ((m_geomID == 0) && (m_bodyID != 0))
    {
        dBodySetPosition(m_bodyID, pos.x, pos.y, pos.z);
        return;
    }
    dGeomSetPosition(m_geomID, pos.x, pos.y, pos.z);
}

void PhysicsObject::GetPosition(nlVector3* position) const
{
    const float* values;
    if (m_geomID == 0 && m_bodyID != 0)
    {
        values = dBodyGetPosition(m_bodyID);
    }
    else
    {
        values = dGeomGetPosition(m_geomID);
    }

    nlMatrix4 parentRotation;
    nlMatrix4 ancestorRotation;
    nlMatrix4 deepResult;
    nlMatrix4 middleResult;
    nlMatrix4 rotation3;
    nlMatrix4 outerResult;
    nlMatrix4 rotation2;
    nlMatrix4 rotation1;
    nlVector3 parentPosition;
    nlVector3 position2;
    nlVector3 transformedPosition;
    nlVector3 position1;

    PhysicsObject* parent = m_parentObject;
    nlVec3Set(*position, values[0], values[1], values[2]);

    if (parent != 0)
    {
        if (parent->m_geomID == 0 && parent->m_bodyID != 0)
        {
            values = dBodyGetPosition(parent->m_bodyID);
        }
        else
        {
            values = dGeomGetPosition(parent->m_geomID);
        }
        nlVec3Set(parentPosition, values[0], values[1], values[2]);

        if (parent->m_parentObject != 0)
        {
            parent->m_parentObject->GetPosition(&position2);
            parent->m_parentObject->GetRotation(&ancestorRotation);
            nlMultPosVectorMatrix(position1, parentPosition, ancestorRotation);
            parentPosition = position1;
            nlVec3Add(parentPosition, position2, parentPosition);
        }

        parent = m_parentObject;
        if (parent->m_geomID == 0 && parent->m_bodyID != 0)
        {
            values = dBodyGetRotation(parent->m_bodyID);
        }
        else
        {
            values = dGeomGetRotation(parent->m_geomID);
        }

        parentRotation.e2[0][0] = values[0];
        parentRotation.e2[1][0] = values[1];
        parentRotation.e2[2][0] = values[2];
        parentRotation.e2[0][1] = values[4];
        parentRotation.e2[1][1] = values[5];
        parentRotation.e2[2][1] = values[6];
        parentRotation.e2[0][2] = values[8];
        parentRotation.e2[1][2] = values[9];
        parentRotation.e2[2][2] = values[10];
        parentRotation.e2[3][0] = 0.0f;
        parentRotation.e2[3][1] = 0.0f;
        parentRotation.e2[3][2] = 0.0f;
        parentRotation.e2[3][3] = 1.0f;
        parentRotation.e2[0][3] = 0.0f;
        parentRotation.e2[1][3] = 0.0f;
        parentRotation.e2[2][3] = 0.0f;

        PhysicsObject* grandParent = parent->m_parentObject;
        if (grandParent != 0)
        {
            if (grandParent->m_geomID == 0 && grandParent->m_bodyID != 0)
            {
                values = dBodyGetRotation(grandParent->m_bodyID);
            }
            else
            {
                values = dGeomGetRotation(grandParent->m_geomID);
            }
            ConvertDMat3ToNLMat4(values, &rotation1);

            PhysicsObject* greatGrandParent = grandParent->m_parentObject;
            if (greatGrandParent != 0)
            {
                if (greatGrandParent->m_geomID == 0
                    && greatGrandParent->m_bodyID != 0)
                {
                    values = dBodyGetRotation(greatGrandParent->m_bodyID);
                }
                else
                {
                    values = dGeomGetRotation(greatGrandParent->m_geomID);
                }
                ConvertDMat3ToNLMat4(values, &rotation2);

                if (greatGrandParent->m_parentObject != 0)
                {
                    greatGrandParent->m_parentObject->GetRotation(&rotation3);
                    nlMultMatrices(deepResult, rotation2, rotation3);
                    rotation2 = deepResult;
                }
                nlMultMatrices(middleResult, rotation1, rotation2);
                rotation1 = middleResult;
            }
            nlMultMatrices(outerResult, parentRotation, rotation1);
            parentRotation = outerResult;
        }

        nlMultPosVectorMatrix(transformedPosition, *position, parentRotation);
        *position = transformedPosition;
        float z = parentPosition.z + position->z;
        float y = parentPosition.y + position->y;
        float x = parentPosition.x + position->x;
        position->x = x;
        position->y = y;
        position->z = z;
    }
}

nlVector3& PhysicsObject::GetPosition()
{
    nlVector3 pos_1;
    nlVector3 transformedPosition;
    nlVector3 pos_2;
    nlVector3 parentPos;
    nlMatrix4 rot_1;
    nlMatrix4 rot_2;
    nlMatrix4 outerResult;
    nlMatrix4 rotation3;
    nlMatrix4 middleResult;
    nlMatrix4 deepResult;
    nlMatrix4 ancestorRotation;
    nlMatrix4 parentRot;
    const float* _retFloats;
    dGeomID geomID = m_geomID;

    if (geomID == 0 && m_bodyID != 0)
    {
        _retFloats = dBodyGetPosition(m_bodyID);
    }
    else
    {
        _retFloats = dGeomGetPosition(geomID);
    }

    PhysicsObject* parentObj = m_parentObject;
    float positionZ;
    float positionY;
    float positionX;
    positionZ = _retFloats[2];
    positionY = _retFloats[1];
    positionX = _retFloats[0];
    m_position.x = positionX;
    m_position.y = positionY;
    m_position.z = positionZ;

    if (parentObj != 0)
    {
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            _retFloats = dBodyGetPosition(parentObj->m_bodyID);
        }
        else
        {
            _retFloats = dGeomGetPosition(parentObj->m_geomID);
        }
        float parentZ;
        float parentY;
        float parentX;
        parentZ = _retFloats[2];
        parentY = _retFloats[1];
        parentX = _retFloats[0];
        parentPos.x = parentX;
        parentPos.y = parentY;
        parentPos.z = parentZ;

        if (parentObj->m_parentObject != 0)
        {
            parentObj->m_parentObject->GetPosition(&pos_2);
            parentObj->m_parentObject->GetRotation(&ancestorRotation);
            nlMultPosVectorMatrix(pos_1, parentPos, ancestorRotation);
            parentPos = pos_1;
            nlVec3Add(parentPos, pos_2, parentPos);
        }

        parentObj = m_parentObject;
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            _retFloats = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            _retFloats = dGeomGetRotation(parentObj->m_geomID);
        }

        parentRot.e2[0][0] = _retFloats[0];
        parentRot.e2[1][0] = _retFloats[1];
        parentRot.e2[2][0] = _retFloats[2];
        parentRot.e2[0][1] = _retFloats[4];
        parentRot.e2[1][1] = _retFloats[5];
        parentRot.e2[2][1] = _retFloats[6];
        parentRot.e2[0][2] = _retFloats[8];
        parentRot.e2[1][2] = _retFloats[9];
        parentRot.e2[2][2] = _retFloats[10];
        parentRot.e2[3][0] = 0.0f;
        parentRot.e2[3][1] = 0.0f;
        parentRot.e2[3][2] = 0.0f;
        parentRot.e2[3][3] = 1.0f;
        parentRot.e2[0][3] = 0.0f;
        parentRot.e2[1][3] = 0.0f;
        parentRot.e2[2][3] = 0.0f;

        PhysicsObject* parent_parentObj = parentObj->m_parentObject;
        if (parent_parentObj != 0)
        {
            if (parent_parentObj->m_geomID == 0
                && parent_parentObj->m_bodyID != 0)
            {
                _retFloats = dBodyGetRotation(parent_parentObj->m_bodyID);
            }
            else
            {
                _retFloats = dGeomGetRotation(parent_parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(_retFloats, &rot_1);

            PhysicsObject* parent_parent_parentObj =
                parent_parentObj->m_parentObject;
            if (parent_parent_parentObj != 0)
            {
                if (parent_parent_parentObj->m_geomID == 0
                    && parent_parent_parentObj->m_bodyID != 0)
                {
                    _retFloats =
                        dBodyGetRotation(parent_parent_parentObj->m_bodyID);
                }
                else
                {
                    _retFloats =
                        dGeomGetRotation(parent_parent_parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(_retFloats, &rot_2);

                if (parent_parent_parentObj->m_parentObject != 0)
                {
                    parent_parent_parentObj->m_parentObject->GetRotation(
                        &rotation3);
                    nlMultMatrices(deepResult, rot_2, rotation3);
                    rot_2 = deepResult;
                }
                nlMultMatrices(middleResult, rot_1, rot_2);
                rot_1 = middleResult;
            }
            nlMultMatrices(outerResult, parentRot, rot_1);
            parentRot = outerResult;
        }

        nlMultPosVectorMatrix(
            transformedPosition, m_position, parentRot);
        m_position = transformedPosition;
        nlVec3Add(m_position, parentPos, m_position);
    }
    return m_position;
}


void PhysicsObject::SetRotation(const nlMatrix3& m3_in, PhysicsObject::CoordinateType arg1)
{
    nlMatrix4 matrix;
    dMatrix3 parentOdeRotation;
    nlMatrix4 transposeResult;
    nlMatrix4 inheritedParentRotation;
    nlMatrix4 inheritedRotation1;
    nlMatrix4 inheritedFinalResult;
    nlMatrix4 inheritedRotation2;
    nlMatrix4 inheritedMiddleResult;
    nlMatrix4 inheritedDeepResult;
    nlMatrix4 localRotation;
    nlMatrix4 parentRotation;
    dMatrix3 directOdeRotation;

    matrix.SetIdentity();
    matrix.e2[0][0] = m3_in.e2[0][0];
    matrix.e2[0][1] = m3_in.e2[0][1];
    matrix.e2[0][2] = m3_in.e2[0][2];
    matrix.e2[1][0] = m3_in.e2[1][0];
    matrix.e2[1][1] = m3_in.e2[1][1];
    matrix.e2[1][2] = m3_in.e2[1][2];
    matrix.e2[2][0] = m3_in.e2[2][0];
    matrix.e2[2][1] = m3_in.e2[2][1];
    matrix.e2[2][2] = m3_in.e2[2][2];

    PhysicsObject* parentObj = m_parentObject;
    if (parentObj != 0 && arg1 == WORLD_COORDINATES)
    {
        const float* dRot;
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(parentObj->m_geomID);
        }

        parentRotation.e2[0][0] = dRot[0];
        parentRotation.e2[1][0] = dRot[1];
        parentRotation.e2[2][0] = dRot[2];
        parentRotation.e2[0][1] = dRot[4];
        parentRotation.e2[1][1] = dRot[5];
        parentRotation.e2[2][1] = dRot[6];
        parentRotation.e2[0][2] = dRot[8];
        parentRotation.e2[1][2] = dRot[9];
        parentRotation.e2[2][2] = dRot[10];
        parentRotation.e2[3][0] = 0.0f;
        parentRotation.e2[3][1] = 0.0f;
        parentRotation.e2[3][2] = 0.0f;
        parentRotation.e2[3][3] = 1.0f;
        parentRotation.e2[0][3] = 0.0f;
        parentRotation.e2[1][3] = 0.0f;
        parentRotation.e2[2][3] = 0.0f;

        parentObj = parentObj->m_parentObject;
        if (parentObj != 0)
        {
            if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(parentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(dRot, &inheritedParentRotation);

            parentObj = parentObj->m_parentObject;
            if (parentObj != 0)
            {
                if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
                {
                    dRot = dBodyGetRotation(parentObj->m_bodyID);
                }
                else
                {
                    dRot = dGeomGetRotation(parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(dRot, &inheritedRotation1);

                if (parentObj->m_parentObject != 0)
                {
                    parentObj->m_parentObject->GetRotation(
                        &inheritedRotation2);
                    nlMultMatrices(inheritedDeepResult,
                        inheritedRotation1,
                        inheritedRotation2);
                    inheritedRotation1 = inheritedDeepResult;
                }
                nlMultMatrices(inheritedMiddleResult,
                    inheritedParentRotation,
                    inheritedRotation1);
                inheritedParentRotation = inheritedMiddleResult;
            }

            nlMultMatrices(inheritedFinalResult,
                parentRotation,
                inheritedParentRotation);
            parentRotation = inheritedFinalResult;
        }

        nlTransposeMatrix(transposeResult, parentRotation);
        parentRotation = transposeResult;
        nlMultMatrices(localRotation, matrix, parentRotation);
        ConvertNLMat4ToDMat3(localRotation, parentOdeRotation);

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, parentOdeRotation);
            return;
        }
        dGeomSetRotation(m_geomID, parentOdeRotation);
        return;
    }

    float w0 = matrix.e[12];
    float z0 = matrix.e[8];
    float y0 = matrix.e[4];
    float x0 = matrix.e[0];
    float w1 = matrix.e[13];
    float z1 = matrix.e[9];
    float y1 = matrix.e[5];
    float x1 = matrix.e[1];
    float w2 = matrix.e[14];
    float z2 = matrix.e[10];
    float y2 = matrix.e[6];
    float x2 = matrix.e[2];

    directOdeRotation[0] = x0;
    directOdeRotation[1] = y0;
    directOdeRotation[2] = z0;
    directOdeRotation[3] = w0;
    directOdeRotation[4] = x1;
    directOdeRotation[5] = y1;
    directOdeRotation[6] = z1;
    directOdeRotation[7] = w1;
    directOdeRotation[8] = x2;
    directOdeRotation[9] = y2;
    directOdeRotation[10] = z2;
    directOdeRotation[11] = w2;

    if (m_geomID == 0 && m_bodyID != 0)
    {
        dBodySetRotation(m_bodyID, directOdeRotation);
        return;
    }
    dGeomSetRotation(m_geomID, directOdeRotation);
}

void PhysicsObject::SetRotation(const nlMatrix4& m4_in, PhysicsObject::CoordinateType arg1)
{
    dMatrix3 directOdeRotation;
    nlMatrix4 parentRot;
    nlMatrix4 localRotation;
    nlMatrix4 deepResult;
    nlMatrix4 middleResult;
    nlMatrix4 rot_tmp2;
    nlMatrix4 outerResult;
    nlMatrix4 rot_2;
    nlMatrix4 rot_1;
    nlMatrix4 transposeResult;
    dMatrix3 parentOdeRotation;

    PhysicsObject* parentObj = m_parentObject;
    if (parentObj != 0 && arg1 == WORLD_COORDINATES)
    {
        const float* dRot;
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(parentObj->m_geomID);
        }

        parentRot.e2[0][0] = dRot[0];
        parentRot.e2[1][0] = dRot[1];
        parentRot.e2[2][0] = dRot[2];
        parentRot.e2[0][1] = dRot[4];
        parentRot.e2[1][1] = dRot[5];
        parentRot.e2[2][1] = dRot[6];
        parentRot.e2[0][2] = dRot[8];
        parentRot.e2[1][2] = dRot[9];
        parentRot.e2[2][2] = dRot[10];
        parentRot.e2[3][0] = 0.0f;
        parentRot.e2[3][1] = 0.0f;
        parentRot.e2[3][2] = 0.0f;
        parentRot.e2[3][3] = 1.0f;
        parentRot.e2[0][3] = 0.0f;
        parentRot.e2[1][3] = 0.0f;
        parentRot.e2[2][3] = 0.0f;

        parentObj = parentObj->m_parentObject;
        if (parentObj != 0)
        {
            if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(parentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(dRot, &rot_1);

            parentObj = parentObj->m_parentObject;
            if (parentObj != 0)
            {
                if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
                {
                    dRot = dBodyGetRotation(parentObj->m_bodyID);
                }
                else
                {
                    dRot = dGeomGetRotation(parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(dRot, &rot_2);

                if (parentObj->m_parentObject != 0)
                {
                    parentObj->m_parentObject->GetRotation(&rot_tmp2);
                    nlMultMatrices(deepResult, rot_2, rot_tmp2);
                    rot_2 = deepResult;
                }
                nlMultMatrices(middleResult, rot_1, rot_2);
                rot_1 = middleResult;
            }

            nlMultMatrices(outerResult, parentRot, rot_1);
            parentRot = outerResult;
        }

        nlTransposeMatrix(transposeResult, parentRot);
        parentRot = transposeResult;
        nlMultMatrices(localRotation, m4_in, parentRot);
        ConvertNLMat4ToDMat3(localRotation, parentOdeRotation);

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, parentOdeRotation);
            return;
        }
        dGeomSetRotation(m_geomID, parentOdeRotation);
        return;
    }

    float x2;
    float y2;
    float z2;
    float w2;
    float x1;
    float y1;
    float z1;
    float w1;
    float x0;
    float y0;
    float z0;
    float w0;
    w0 = m4_in.e[12];
    z0 = m4_in.e[8];
    y0 = m4_in.e[4];
    x0 = m4_in.e[0];
    w1 = m4_in.e[13];
    z1 = m4_in.e[9];
    y1 = m4_in.e[5];
    x1 = m4_in.e[1];
    w2 = m4_in.e[14];
    z2 = m4_in.e[10];
    y2 = m4_in.e[6];
    x2 = m4_in.e[2];

    directOdeRotation[0] = x0;
    directOdeRotation[1] = y0;
    directOdeRotation[2] = z0;
    directOdeRotation[3] = w0;
    directOdeRotation[4] = x1;
    directOdeRotation[5] = y1;
    directOdeRotation[6] = z1;
    directOdeRotation[7] = w1;
    directOdeRotation[8] = x2;
    directOdeRotation[9] = y2;
    directOdeRotation[10] = z2;
    directOdeRotation[11] = w2;

    if (m_geomID == 0 && m_bodyID != 0)
    {
        dBodySetRotation(m_bodyID, directOdeRotation);
        return;
    }
    dGeomSetRotation(m_geomID, directOdeRotation);
}

void PhysicsObject::GetRotation(nlMatrix4* m_out) const
{
    const float* dRot;
    PhysicsObject* parentObj;
    if (m_geomID == 0 && m_bodyID != 0)
    {
        dRot = dBodyGetRotation(m_bodyID);
    }
    else
    {
        dRot = dGeomGetRotation(m_geomID);
    }

    nlMatrix4 parentRot;
    nlMatrix4 rot_1;
    nlMatrix4 finalResult;
    nlMatrix4 rot_2;
    nlMatrix4 middleResult;
    nlMatrix4 deepResult;

    parentObj = m_parentObject;

    m_out->e2[0][0] = dRot[0];
    m_out->e2[1][0] = dRot[1];
    m_out->e2[2][0] = dRot[2];
    m_out->e2[0][1] = dRot[4];
    m_out->e2[1][1] = dRot[5];
    m_out->e2[2][1] = dRot[6];
    m_out->e2[0][2] = dRot[8];
    m_out->e2[1][2] = dRot[9];
    m_out->e2[2][2] = dRot[10];
    m_out->e2[3][0] = 0.0f;
    m_out->e2[3][1] = 0.0f;
    m_out->e2[3][2] = 0.0f;
    m_out->e2[3][3] = 1.0f;
    m_out->e2[0][3] = 0.0f;
    m_out->e2[1][3] = 0.0f;
    m_out->e2[2][3] = 0.0f;

    if (parentObj == 0)
    {
        return;
    }

    if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
    {
        dRot = dBodyGetRotation(parentObj->m_bodyID);
    }
    else
    {
        dRot = dGeomGetRotation(parentObj->m_geomID);
    }
    ConvertDMat3ToNLMat4(dRot, &parentRot);

    parentObj = parentObj->m_parentObject;
    if (parentObj != 0)
    {
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(parentObj->m_geomID);
        }
        ConvertDMat3ToNLMat4(dRot, &rot_1);

        if (parentObj->m_parentObject != 0)
        {
            parentObj->m_parentObject->GetRotation(&rot_2);
            nlMultMatrices(deepResult, rot_1, rot_2);
            rot_1 = deepResult;
        }
        nlMultMatrices(middleResult, parentRot, rot_1);
        parentRot = middleResult;
    }

    nlMultMatrices(finalResult, *m_out, parentRot);
    *m_out = finalResult;
}

void PhysicsObject::SetLinearVelocity(const nlVector3& velocity)
{
    dBodySetLinearVel(m_bodyID, velocity.x, velocity.y, velocity.z);
}

void PhysicsObject::GetLinearVelocity(nlVector3* vel) const
{
    PhysicsObject* parent2;
    PhysicsObject* parent4;
    PhysicsObject* parent;
    PhysicsObject* parent3;

    parent = m_parentObject;
    if (parent != 0)
    {
        parent2 = parent->m_parentObject;
        if (parent2 != 0)
        {
            parent3 = parent2->m_parentObject;
            if (parent3 != 0)
            {
                parent4 = parent3->m_parentObject;
                if (parent4 != 0)
                {
                    parent4->GetLinearVelocity(vel);
                    return;
                }
                float* velFloats =
                    (float*)dBodyGetLinearVel(parent3->m_bodyID);
                vel->Set(velFloats[0], velFloats[1], velFloats[2]);
                return;
            }
            float* velFloats =
                (float*)dBodyGetLinearVel(parent2->m_bodyID);
            nlVec3Set(*vel, velFloats[0], velFloats[1], velFloats[2]);
            return;
        }
        float* velFloats = (float*)dBodyGetLinearVel(parent->m_bodyID);
        nlVec3Set(*vel, velFloats[0], velFloats[1], velFloats[2]);
        return;
    }
    float* velFloats = (float*)dBodyGetLinearVel(m_bodyID);
    nlVec3Set(*vel, velFloats[0], velFloats[1], velFloats[2]);
}

nlVector3& PhysicsObject::GetLinearVelocity()
{
    PhysicsObject* temp_r5;
    PhysicsObject* temp_r3;
    PhysicsObject* temp_r5_2;
    PhysicsObject* temp_r3_2;

    temp_r5 = m_parentObject;
    if (temp_r5 != 0)
    {
        temp_r3 = temp_r5->m_parentObject;
        if (temp_r3 != 0)
        {
            temp_r5_2 = temp_r3->m_parentObject;
            if (temp_r5_2 != 0)
            {
                temp_r3_2 = temp_r5_2->m_parentObject;
                if (temp_r3_2 != 0)
                {
                    temp_r3_2->GetLinearVelocity(&m_linearVelocity);
                }
                else
                {
                    const float* v =
                        dBodyGetLinearVel(temp_r5_2->m_bodyID);
                    nlVec3Set(m_linearVelocity, v[0], v[1], v[2]);
                }
            }
            else
            {
                const float* v = dBodyGetLinearVel(temp_r3->m_bodyID);
                float z;
                float y;
                float x;
                z = v[2];
                y = v[1];
                x = v[0];
                m_linearVelocity.x = x;
                m_linearVelocity.y = y;
                m_linearVelocity.z = z;
            }
        }
        else
        {
            const float* v = dBodyGetLinearVel(temp_r5->m_bodyID);
            float z;
            float y;
            float x;
            z = v[2];
            y = v[1];
            x = v[0];
            m_linearVelocity.x = x;
            m_linearVelocity.y = y;
            m_linearVelocity.z = z;
        }
    }
    else
    {
        const float* v = dBodyGetLinearVel(m_bodyID);
        float z;
        float y;
        float x;
        z = v[2];
        y = v[1];
        x = v[0];
        m_linearVelocity.x = x;
        m_linearVelocity.y = y;
        m_linearVelocity.z = z;
    }
    return m_linearVelocity;
}


void PhysicsObject::SetAngularVelocity(const nlVector3& velocity)
{
    dBodySetAngularVel(m_bodyID, (float)velocity.x, (float)velocity.y, (float)velocity.z);
}

void PhysicsObject::GetAngularVelocity(nlVector3* vel) const
{
    PhysicsObject* parent2;
    PhysicsObject* parent4;
    PhysicsObject* parent;
    PhysicsObject* parent3;

    parent = m_parentObject;
    if (parent != 0)
    {
        parent2 = parent->m_parentObject;
        if (parent2 != 0)
        {
            parent3 = parent2->m_parentObject;
            if (parent3 != 0)
            {
                parent4 = parent3->m_parentObject;
                if (parent4 != 0)
                {
                    parent4->GetAngularVelocity(vel);
                    return;
                }
                const float* velFloats =
                    dBodyGetAngularVel(parent3->m_bodyID);
                vel->Set(velFloats[0], velFloats[1], velFloats[2]);
                return;
            }
            const float* velFloats = dBodyGetAngularVel(parent2->m_bodyID);
            nlVec3Set(*vel, velFloats[0], velFloats[1], velFloats[2]);
            return;
        }
        const float* velFloats = dBodyGetAngularVel(parent->m_bodyID);
        nlVec3Set(*vel, velFloats[0], velFloats[1], velFloats[2]);
        return;
    }
    const float* velFloats = dBodyGetAngularVel(this->m_bodyID);
    nlVec3Set(*vel, velFloats[0], velFloats[1], velFloats[2]);
}

void PhysicsObject::AddForceAtCentreOfMass(const nlVector3& force)
{
    dBodyAddForce(m_bodyID, force.x, force.y, force.z);
}

void PhysicsObject::ZeroForceAccumulators()
{
    dBodySetForce(m_bodyID, 0.f, 0.f, 0.f);
    dBodySetTorque(m_bodyID, 0.f, 0.f, 0.f);
}

void PhysicsObject::SetDefaultContactInfo(dContact* contact)
{
    (contact->surface).mode = 0x14;
    (contact->surface).soft_cfm = 0.0001f;
    (contact->surface).mu = 0.9f;
    (contact->surface).mu2 = 0.0f;
    (contact->surface).bounce = .2f;
    (contact->surface).bounce_vel = .1f;
}

bool PhysicsObject::SetContactInfo(dContact* contact, PhysicsObject* otherObject, bool first)
{
    if (m_parentObject != 0)
    {
        return m_parentObject->SetContactInfo(contact, otherObject, first);
    }

    if (first != 0)
    {
        (contact->surface).mode = 0x14;
        (contact->surface).soft_cfm = 0.0001f;
        (contact->surface).mu = 0.9f;
        (contact->surface).mu2 = 0.0f;
        (contact->surface).bounce = 0.2f;
        (contact->surface).bounce_vel = 0.1f;
    }

    return true;
}

void PhysicsObject::SetWorldMatrix(const nlMatrix4& in)
{
    dMatrix3 parentOdeRotation;
    nlMatrix4 transposeResult;
    nlMatrix4 inheritedParentRotation;
    nlMatrix4 inheritedRotation1;
    nlMatrix4 inheritedFinalResult;
    nlMatrix4 inheritedRotation2;
    nlMatrix4 inheritedMiddleResult;
    nlMatrix4 inheritedDeepResult;
    nlMatrix4 localRotation;
    nlMatrix4 parentRotation;
    dMatrix3 mat;

    PhysicsObject* parentObj = m_parentObject;
    if (parentObj != 0)
    {
        const float* dRot;
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(parentObj->m_geomID);
        }

        parentRotation.e2[0][0] = dRot[0];
        parentRotation.e2[1][0] = dRot[1];
        parentRotation.e2[2][0] = dRot[2];
        parentRotation.e2[0][1] = dRot[4];
        parentRotation.e2[1][1] = dRot[5];
        parentRotation.e2[2][1] = dRot[6];
        parentRotation.e2[0][2] = dRot[8];
        parentRotation.e2[1][2] = dRot[9];
        parentRotation.e2[2][2] = dRot[10];
        parentRotation.e2[3][0] = 0.0f;
        parentRotation.e2[3][1] = 0.0f;
        parentRotation.e2[3][2] = 0.0f;
        parentRotation.e2[3][3] = 1.0f;
        parentRotation.e2[0][3] = 0.0f;
        parentRotation.e2[1][3] = 0.0f;
        parentRotation.e2[2][3] = 0.0f;

        parentObj = parentObj->m_parentObject;
        if (parentObj != 0)
        {
            if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(parentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(dRot, &inheritedParentRotation);

            parentObj = parentObj->m_parentObject;
            if (parentObj != 0)
            {
                if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
                {
                    dRot = dBodyGetRotation(parentObj->m_bodyID);
                }
                else
                {
                    dRot = dGeomGetRotation(parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(dRot, &inheritedRotation1);

                if (parentObj->m_parentObject != 0)
                {
                    parentObj->m_parentObject->GetRotation(
                        &inheritedRotation2);
                    nlMultMatrices(inheritedDeepResult,
                        inheritedRotation1,
                        inheritedRotation2);
                    inheritedRotation1 = inheritedDeepResult;
                }
                nlMultMatrices(inheritedMiddleResult,
                    inheritedParentRotation,
                    inheritedRotation1);
                inheritedParentRotation = inheritedMiddleResult;
            }

            nlMultMatrices(inheritedFinalResult,
                parentRotation,
                inheritedParentRotation);
            parentRotation = inheritedFinalResult;
        }

        nlTransposeMatrix(transposeResult, parentRotation);
        parentRotation = transposeResult;
        nlMultMatrices(localRotation, in, parentRotation);
        ConvertNLMat4ToDMat3(localRotation, parentOdeRotation);

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, parentOdeRotation);
        }
        else
        {
            dGeomSetRotation(m_geomID, parentOdeRotation);
        }
    }
    else
    {
        float w0 = in.e[12];
        float z0 = in.e[8];
        float y0 = in.e[4];
        float x0 = in.e[0];
        float w1 = in.e[13];
        float z1 = in.e[9];
        float y1 = in.e[5];
        float x1 = in.e[1];
        float w2 = in.e[14];
        float z2 = in.e[10];
        float y2 = in.e[6];
        float x2 = in.e[2];

        mat[0] = x0;
        mat[1] = y0;
        mat[2] = z0;
        mat[3] = w0;
        mat[4] = x1;
        mat[5] = y1;
        mat[6] = z1;
        mat[7] = w1;
        mat[8] = x2;
        mat[9] = y2;
        mat[10] = z2;
        mat[11] = w2;

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, mat);
        }
        else
        {
            dGeomSetRotation(m_geomID, mat);
        }
    }

    nlVector3* pos = (nlVector3*)&in.e2[3][0];
    SetPosition(*pos, WORLD_COORDINATES);
}

void PhysicsObject::DisableCollisions()
{
    dGeomDisable(m_geomID);
}

void PhysicsObject::EnableCollisions()
{
    dGeomEnable(m_geomID);
}

bool PhysicsObject::AreCollisionsEnabled()
{
    return (bool)dGeomIsEnabled(m_geomID);
}

dSpaceID PhysicsObject::Disconnect()
{
    dSpaceID space;
    space = dGeomGetSpace(m_geomID);
    if (space != 0)
    {
        dSpaceRemove(space, m_geomID);
    }
    dGeomSetBody(m_geomID, 0);
    if (m_bodyID != 0)
    {
        dBodyDisable(m_bodyID);
    }
    return space;
}
void PhysicsObject::Reconnect(dSpaceID space)
{
    dSpaceAdd(space, m_geomID);
    dGeomSetBody(m_geomID, m_bodyID);
    if (m_bodyID != 0)
    {
        dBodyEnable(m_bodyID);
    }
}

void PhysicsObject::SetMass(float mass)
{
    if (m_bodyID != 0)
    {
        dMass m;
        dBodyGetMass(m_bodyID, &m);
        dMassAdjust(&m, mass);
        dBodySetMass(m_bodyID, &m);
    }
}

void PhysicsObject::MakeStatic()
{
    if (m_bodyID != 0)
    {
        dBodyDestroy(m_bodyID);
        m_bodyID = 0;
    }
    dGeomSetBody(m_geomID, 0);
}

ContactType PhysicsObject::Contact(PhysicsObject* obj1, dContact* contact, int param)
{
    return Contact(obj1, contact, param, 0);
}

ContactType PhysicsObject::Contact(PhysicsObject* obj1, dContact* contact, int param, PhysicsObject* obj2)
{
    if (m_parentObject != 0)
    {
        return m_parentObject->Contact(obj1, contact, param, this);
    }
    if (m_contactHandler != 0)
    {
        return m_contactHandler->Contact(this, obj1, contact, param);
    }
    return TWO_WAY_CONTACT;
}

void PhysicsObject::CloneObject(const PhysicsObject& obj)
{
    nlMatrix4 m4Rot;
    nlMatrix4 positionRotation1;
    nlMatrix4 positionRotation2;
    nlMatrix4 positionOuterResult;
    nlMatrix4 positionRotation3;
    nlMatrix4 positionMiddleResult;
    nlMatrix4 positionDeepResult;
    nlMatrix4 positionAncestorRotation;
    nlMatrix4 positionParentRotation;
    nlMatrix4 objectDeepResult;
    nlMatrix4 objectMiddleResult;
    nlMatrix4 objectRotation2;
    nlMatrix4 objectFinalResult;
    nlMatrix4 objectRotation1;
    nlMatrix4 objectParentRotation;
    dMatrix3 parentOdeRotation;
    nlMatrix4 transposeResult;
    nlMatrix4 inheritedParentRotation;
    nlMatrix4 inheritedRotation1;
    nlMatrix4 inheritedFinalResult;
    nlMatrix4 inheritedRotation2;
    nlMatrix4 inheritedMiddleResult;
    nlMatrix4 inheritedDeepResult;
    nlMatrix4 localRotation;
    nlMatrix4 parentRotation;
    dMatrix3 _rot;
    nlVector3 v3Pos;
    nlVector3 v3LinearVel;
    nlVector3 v3AngularVelocity;
    nlVector3 position1;
    nlVector3 transformedPosition;
    nlVector3 position2;
    nlVector3 parentPosition;

    const float* values;
    if (obj.m_geomID == 0 && obj.m_bodyID != 0)
    {
        values = dBodyGetPosition(obj.m_bodyID);
    }
    else
    {
        values = dGeomGetPosition(obj.m_geomID);
    }

    PhysicsObject* parentObj = obj.m_parentObject;
    float positionZ;
    float positionY;
    float positionX;
    positionZ = values[2];
    positionY = values[1];
    positionX = values[0];
    v3Pos.x = positionX;
    v3Pos.y = positionY;
    v3Pos.z = positionZ;

    if (parentObj != 0)
    {
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            values = dBodyGetPosition(parentObj->m_bodyID);
        }
        else
        {
            values = dGeomGetPosition(parentObj->m_geomID);
        }
        float parentZ;
        float parentY;
        float parentX;
        parentZ = values[2];
        parentY = values[1];
        parentX = values[0];
        parentPosition.x = parentX;
        parentPosition.y = parentY;
        parentPosition.z = parentZ;

        if (parentObj->m_parentObject != 0)
        {
            parentObj->m_parentObject->GetPosition(&position2);
            parentObj->m_parentObject->GetRotation(
                &positionAncestorRotation);
            nlMultPosVectorMatrix(
                position1, parentPosition, positionAncestorRotation);
            parentPosition = position1;
            nlVec3Add(parentPosition, position2, parentPosition);
        }

        parentObj = obj.m_parentObject;
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            values = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            values = dGeomGetRotation(parentObj->m_geomID);
        }

        positionParentRotation.e2[0][0] = values[0];
        positionParentRotation.e2[1][0] = values[1];
        positionParentRotation.e2[2][0] = values[2];
        positionParentRotation.e2[0][1] = values[4];
        positionParentRotation.e2[1][1] = values[5];
        positionParentRotation.e2[2][1] = values[6];
        positionParentRotation.e2[0][2] = values[8];
        positionParentRotation.e2[1][2] = values[9];
        positionParentRotation.e2[2][2] = values[10];
        positionParentRotation.e2[3][0] = 0.0f;
        positionParentRotation.e2[3][1] = 0.0f;
        positionParentRotation.e2[3][2] = 0.0f;
        positionParentRotation.e2[3][3] = 1.0f;
        positionParentRotation.e2[0][3] = 0.0f;
        positionParentRotation.e2[1][3] = 0.0f;
        positionParentRotation.e2[2][3] = 0.0f;

        PhysicsObject* parent_parentObj = parentObj->m_parentObject;
        if (parent_parentObj != 0)
        {
            if (parent_parentObj->m_geomID == 0
                && parent_parentObj->m_bodyID != 0)
            {
                values = dBodyGetRotation(parent_parentObj->m_bodyID);
            }
            else
            {
                values = dGeomGetRotation(parent_parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(values, &positionRotation1);

            PhysicsObject* parent_parent_parentObj =
                parent_parentObj->m_parentObject;
            if (parent_parent_parentObj != 0)
            {
                if (parent_parent_parentObj->m_geomID == 0
                    && parent_parent_parentObj->m_bodyID != 0)
                {
                    values =
                        dBodyGetRotation(parent_parent_parentObj->m_bodyID);
                }
                else
                {
                    values =
                        dGeomGetRotation(parent_parent_parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(values, &positionRotation2);

                if (parent_parent_parentObj->m_parentObject != 0)
                {
                    parent_parent_parentObj->m_parentObject->GetRotation(
                        &positionRotation3);
                    nlMultMatrices(positionDeepResult,
                        positionRotation2,
                        positionRotation3);
                    positionRotation2 = positionDeepResult;
                }
                nlMultMatrices(positionMiddleResult,
                    positionRotation1,
                    positionRotation2);
                positionRotation1 = positionMiddleResult;
            }
            nlMultMatrices(positionOuterResult,
                positionParentRotation,
                positionRotation1);
            positionParentRotation = positionOuterResult;
        }

        nlMultPosVectorMatrix(
            transformedPosition, v3Pos, positionParentRotation);
        v3Pos = transformedPosition;
        nlVec3Add(v3Pos, parentPosition, v3Pos);
    }

    SetPosition(v3Pos, WORLD_COORDINATES);

    const float* dRot;
    if (obj.m_geomID == 0 && obj.m_bodyID != 0)
    {
        dRot = dBodyGetRotation(obj.m_bodyID);
    }
    else
    {
        dRot = dGeomGetRotation(obj.m_geomID);
    }

    PhysicsObject* rotationParentObj = obj.m_parentObject;
    m4Rot.e2[0][0] = dRot[0];
    m4Rot.e2[1][0] = dRot[1];
    m4Rot.e2[2][0] = dRot[2];
    m4Rot.e2[0][1] = dRot[4];
    m4Rot.e2[1][1] = dRot[5];
    m4Rot.e2[2][1] = dRot[6];
    m4Rot.e2[0][2] = dRot[8];
    m4Rot.e2[1][2] = dRot[9];
    m4Rot.e2[2][2] = dRot[10];
    m4Rot.e2[3][0] = 0.0f;
    m4Rot.e2[3][1] = 0.0f;
    m4Rot.e2[3][2] = 0.0f;
    m4Rot.e2[3][3] = 1.0f;
    m4Rot.e2[0][3] = 0.0f;
    m4Rot.e2[1][3] = 0.0f;
    m4Rot.e2[2][3] = 0.0f;

    if (rotationParentObj != 0)
    {
        if (rotationParentObj->m_geomID == 0
            && rotationParentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(rotationParentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(rotationParentObj->m_geomID);
        }
        ConvertDMat3ToNLMat4(dRot, &objectParentRotation);

        rotationParentObj = rotationParentObj->m_parentObject;
        if (rotationParentObj != 0)
        {
            if (rotationParentObj->m_geomID == 0
                && rotationParentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(rotationParentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(rotationParentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(dRot, &objectRotation1);

            if (rotationParentObj->m_parentObject != 0)
            {
                rotationParentObj->m_parentObject->GetRotation(
                    &objectRotation2);
                nlMultMatrices(objectDeepResult,
                    objectRotation1,
                    objectRotation2);
                objectRotation1 = objectDeepResult;
            }
            nlMultMatrices(objectMiddleResult,
                objectParentRotation,
                objectRotation1);
            objectParentRotation = objectMiddleResult;
        }

        nlMultMatrices(objectFinalResult, m4Rot, objectParentRotation);
        m4Rot = objectFinalResult;
    }

    parentObj = m_parentObject;
    if (parentObj != 0)
    {
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(parentObj->m_geomID);
        }

        parentRotation.e2[0][0] = dRot[0];
        parentRotation.e2[1][0] = dRot[1];
        parentRotation.e2[2][0] = dRot[2];
        parentRotation.e2[0][1] = dRot[4];
        parentRotation.e2[1][1] = dRot[5];
        parentRotation.e2[2][1] = dRot[6];
        parentRotation.e2[0][2] = dRot[8];
        parentRotation.e2[1][2] = dRot[9];
        parentRotation.e2[2][2] = dRot[10];
        parentRotation.e2[3][0] = 0.0f;
        parentRotation.e2[3][1] = 0.0f;
        parentRotation.e2[3][2] = 0.0f;
        parentRotation.e2[3][3] = 1.0f;
        parentRotation.e2[0][3] = 0.0f;
        parentRotation.e2[1][3] = 0.0f;
        parentRotation.e2[2][3] = 0.0f;

        parentObj = parentObj->m_parentObject;
        if (parentObj != 0)
        {
            if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(parentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(dRot, &inheritedParentRotation);

            parentObj = parentObj->m_parentObject;
            if (parentObj != 0)
            {
                if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
                {
                    dRot = dBodyGetRotation(parentObj->m_bodyID);
                }
                else
                {
                    dRot = dGeomGetRotation(parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(dRot, &inheritedRotation1);

                if (parentObj->m_parentObject != 0)
                {
                    parentObj->m_parentObject->GetRotation(
                        &inheritedRotation2);
                    nlMultMatrices(inheritedDeepResult,
                        inheritedRotation1,
                        inheritedRotation2);
                    inheritedRotation1 = inheritedDeepResult;
                }
                nlMultMatrices(inheritedMiddleResult,
                    inheritedParentRotation,
                    inheritedRotation1);
                inheritedParentRotation = inheritedMiddleResult;
            }

            nlMultMatrices(inheritedFinalResult,
                parentRotation,
                inheritedParentRotation);
            parentRotation = inheritedFinalResult;
        }

        nlTransposeMatrix(transposeResult, parentRotation);
        parentRotation = transposeResult;
        nlMultMatrices(localRotation, m4Rot, parentRotation);
        ConvertNLMat4ToDMat3(localRotation, parentOdeRotation);

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, parentOdeRotation);
        }
        else
        {
            dGeomSetRotation(m_geomID, parentOdeRotation);
        }
    }
    else
    {
        float w0 = m4Rot.e[12];
        float z0 = m4Rot.e[8];
        float y0 = m4Rot.e[4];
        float x0 = m4Rot.e[0];
        float w1 = m4Rot.e[13];
        float z1 = m4Rot.e[9];
        float y1 = m4Rot.e[5];
        float x1 = m4Rot.e[1];
        float w2 = m4Rot.e[14];
        float z2 = m4Rot.e[10];
        float y2 = m4Rot.e[6];
        float x2 = m4Rot.e[2];

        _rot[0] = x0;
        _rot[1] = y0;
        _rot[2] = z0;
        _rot[3] = w0;
        _rot[4] = x1;
        _rot[5] = y1;
        _rot[6] = z1;
        _rot[7] = w1;
        _rot[8] = x2;
        _rot[9] = y2;
        _rot[10] = z2;
        _rot[11] = w2;

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, _rot);
        }
        else
        {
            dGeomSetRotation(m_geomID, _rot);
        }
    }

    PhysicsObject* temp_r3;
    PhysicsObject* temp_r5;
    PhysicsObject* temp_r5_2;
    PhysicsObject* temp_r3_2;
    temp_r3 = obj.m_parentObject;
    if (temp_r3 != 0)
    {
        temp_r5 = temp_r3->m_parentObject;
        if (temp_r5 != 0)
        {
            temp_r5_2 = temp_r5->m_parentObject;
            if (temp_r5_2 != 0)
            {
                temp_r3_2 = temp_r5_2->m_parentObject;
                if (temp_r3_2 != 0)
                {
                    temp_r3_2->GetLinearVelocity(&v3LinearVel);
                }
                else
                {
                    const float* v =
                        dBodyGetLinearVel(temp_r5_2->m_bodyID);
                    nlVec3Set(v3LinearVel, v[0], v[1], v[2]);
                }
            }
            else
            {
                const float* v = dBodyGetLinearVel(temp_r5->m_bodyID);
                float z;
                float y;
                float x;
                z = v[2];
                y = v[1];
                x = v[0];
                v3LinearVel.x = x;
                v3LinearVel.y = y;
                v3LinearVel.z = z;
            }
        }
        else
        {
            const float* v = dBodyGetLinearVel(temp_r3->m_bodyID);
            float z;
            float y;
            float x;
            z = v[2];
            y = v[1];
            x = v[0];
            v3LinearVel.x = x;
            v3LinearVel.y = y;
            v3LinearVel.z = z;
        }
    }
    else
    {
        const float* v = dBodyGetLinearVel(obj.m_bodyID);
        float z;
        float y;
        float x;
        z = v[2];
        y = v[1];
        x = v[0];
        v3LinearVel.x = x;
        v3LinearVel.y = y;
        v3LinearVel.z = z;
    }

    dBodySetLinearVel(m_bodyID, v3LinearVel.x, v3LinearVel.y, v3LinearVel.z);

    temp_r3 = obj.m_parentObject;
    if (temp_r3 != 0)
    {
        temp_r5 = temp_r3->m_parentObject;
        if (temp_r5 != 0)
        {
            temp_r5_2 = temp_r5->m_parentObject;
            if (temp_r5_2 != 0)
            {
                temp_r3_2 = temp_r5_2->m_parentObject;
                if (temp_r3_2 != 0)
                {
                    temp_r3_2->GetAngularVelocity(&v3AngularVelocity);
                }
                else
                {
                    const float* v =
                        dBodyGetAngularVel(temp_r5_2->m_bodyID);
                    nlVec3Set(v3AngularVelocity, v[0], v[1], v[2]);
                }
            }
            else
            {
                const float* v = dBodyGetAngularVel(temp_r5->m_bodyID);
                float z;
                float y;
                float x;
                z = v[2];
                y = v[1];
                x = v[0];
                v3AngularVelocity.x = x;
                v3AngularVelocity.y = y;
                v3AngularVelocity.z = z;
            }
        }
        else
        {
            const float* v = dBodyGetAngularVel(temp_r3->m_bodyID);
            float z;
            float y;
            float x;
            z = v[2];
            y = v[1];
            x = v[0];
            v3AngularVelocity.x = x;
            v3AngularVelocity.y = y;
            v3AngularVelocity.z = z;
        }
    }
    else
    {
        const float* v = dBodyGetAngularVel(obj.m_bodyID);
        float z;
        float y;
        float x;
        z = v[2];
        y = v[1];
        x = v[0];
        v3AngularVelocity.x = x;
        v3AngularVelocity.y = y;
        v3AngularVelocity.z = z;
    }

    dBodySetAngularVel(m_bodyID, v3AngularVelocity.x, v3AngularVelocity.y, v3AngularVelocity.z);
    dBodyID sourceBody = obj.m_bodyID;
    m_gravity = obj.m_gravity;

    f32 mass;
    if (sourceBody != 0)
    {
        dMass m1;
        dBodyGetMass(obj.m_bodyID, &m1);
        mass = m1.mass;
    }
    else
    {
        mass = -1.f;
    }

    if (m_bodyID != 0)
    {
        dMass m2;
        dBodyGetMass(m_bodyID, &m2);
        dMassAdjust(&m2, mass);
        dBodySetMass(m_bodyID, &m2);
    }
}


void ConvertNLMat4ToDMat3(const nlMatrix4& src, float* dest)
{
    float w0 = src.e[12];
    float z0 = src.e[8];
    float y0 = src.e[4];
    float x0 = src.e[0];
    float w1 = src.e[13];
    float z1 = src.e[9];
    float y1 = src.e[5];
    float x1 = src.e[1];
    float w2 = src.e[14];
    float z2 = src.e[10];
    float y2 = src.e[6];
    float x2 = src.e[2];

    dest[0] = x0;
    dest[1] = y0;
    dest[2] = z0;
    dest[3] = w0;
    dest[4] = x1;
    dest[5] = y1;
    dest[6] = z1;
    dest[7] = w1;
    dest[8] = x2;
    dest[9] = y2;
    dest[10] = z2;
    dest[11] = w2;
}

void ConvertDMat3ToNLMat4(const float* src, nlMatrix4* dest)
{
    dest->e2[0][0] = src[0];
    dest->e2[1][0] = src[1];
    dest->e2[2][0] = src[2];
    dest->e2[0][1] = src[4];
    dest->e2[1][1] = src[5];
    dest->e2[2][1] = src[6];
    dest->e2[0][2] = src[8];
    dest->e2[1][2] = src[9];
    dest->e2[2][2] = src[10];
    dest->e2[3][0] = 0.0f;
    dest->e2[3][1] = 0.0f;
    dest->e2[3][2] = 0.0f;
    dest->e2[3][3] = 1.0f;
    dest->e2[0][3] = 0.0f;
    dest->e2[1][3] = 0.0f;
    dest->e2[2][3] = 0.0f;
}

void PhysicsObject::Unknown0()
{
    nlMatrix4 identity;
    dMatrix3 parentOdeRotation;
    nlMatrix4 transposeResult;
    nlMatrix4 inheritedParentRotation;
    nlMatrix4 inheritedRotation1;
    nlMatrix4 inheritedFinalResult;
    nlMatrix4 inheritedRotation2;
    nlMatrix4 inheritedMiddleResult;
    nlMatrix4 inheritedDeepResult;
    nlMatrix4 localRotation;
    nlMatrix4 parentRotation;
    dMatrix3 directOdeRotation;
    nlVector3 zero = { 0.0f, 0.0f, 0.0f };

    SetPosition(zero, WORLD_COORDINATES);

    identity.SetIdentity();

    PhysicsObject* parentObj = m_parentObject;
    if (parentObj != 0)
    {
        const float* dRot;
        if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
        {
            dRot = dBodyGetRotation(parentObj->m_bodyID);
        }
        else
        {
            dRot = dGeomGetRotation(parentObj->m_geomID);
        }

        parentRotation.e2[0][0] = dRot[0];
        parentRotation.e2[1][0] = dRot[1];
        parentRotation.e2[2][0] = dRot[2];
        parentRotation.e2[0][1] = dRot[4];
        parentRotation.e2[1][1] = dRot[5];
        parentRotation.e2[2][1] = dRot[6];
        parentRotation.e2[0][2] = dRot[8];
        parentRotation.e2[1][2] = dRot[9];
        parentRotation.e2[2][2] = dRot[10];
        parentRotation.e2[3][0] = 0.0f;
        parentRotation.e2[3][1] = 0.0f;
        parentRotation.e2[3][2] = 0.0f;
        parentRotation.e2[3][3] = 1.0f;
        parentRotation.e2[0][3] = 0.0f;
        parentRotation.e2[1][3] = 0.0f;
        parentRotation.e2[2][3] = 0.0f;

        parentObj = parentObj->m_parentObject;
        if (parentObj != 0)
        {
            if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
            {
                dRot = dBodyGetRotation(parentObj->m_bodyID);
            }
            else
            {
                dRot = dGeomGetRotation(parentObj->m_geomID);
            }
            ConvertDMat3ToNLMat4(dRot, &inheritedParentRotation);

            parentObj = parentObj->m_parentObject;
            if (parentObj != 0)
            {
                if (parentObj->m_geomID == 0 && parentObj->m_bodyID != 0)
                {
                    dRot = dBodyGetRotation(parentObj->m_bodyID);
                }
                else
                {
                    dRot = dGeomGetRotation(parentObj->m_geomID);
                }
                ConvertDMat3ToNLMat4(dRot, &inheritedRotation1);

                if (parentObj->m_parentObject != 0)
                {
                    parentObj->m_parentObject->GetRotation(
                        &inheritedRotation2);
                    nlMultMatrices(inheritedDeepResult,
                        inheritedRotation1,
                        inheritedRotation2);
                    inheritedRotation1 = inheritedDeepResult;
                }
                nlMultMatrices(inheritedMiddleResult,
                    inheritedParentRotation,
                    inheritedRotation1);
                inheritedParentRotation = inheritedMiddleResult;
            }

            nlMultMatrices(inheritedFinalResult,
                parentRotation,
                inheritedParentRotation);
            parentRotation = inheritedFinalResult;
        }

        nlTransposeMatrix(transposeResult, parentRotation);
        parentRotation = transposeResult;
        nlMultMatrices(localRotation, identity, parentRotation);
        ConvertNLMat4ToDMat3(localRotation, parentOdeRotation);

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, parentOdeRotation);
        }
        else
        {
            dGeomSetRotation(m_geomID, parentOdeRotation);
        }
    }
    else
    {
        float w0 = identity.e[12];
        float z0 = identity.e[8];
        float y0 = identity.e[4];
        float x0 = identity.e[0];
        float w1 = identity.e[13];
        float z1 = identity.e[9];
        float y1 = identity.e[5];
        float x1 = identity.e[1];
        float w2 = identity.e[14];
        float z2 = identity.e[10];
        float y2 = identity.e[6];
        float x2 = identity.e[2];

        directOdeRotation[0] = x0;
        directOdeRotation[1] = y0;
        directOdeRotation[2] = z0;
        directOdeRotation[3] = w0;
        directOdeRotation[4] = x1;
        directOdeRotation[5] = y1;
        directOdeRotation[6] = z1;
        directOdeRotation[7] = w1;
        directOdeRotation[8] = x2;
        directOdeRotation[9] = y2;
        directOdeRotation[10] = z2;
        directOdeRotation[11] = w2;

        if (m_geomID == 0 && m_bodyID != 0)
        {
            dBodySetRotation(m_bodyID, directOdeRotation);
        }
        else
        {
            dGeomSetRotation(m_geomID, directOdeRotation);
        }
    }

    if (m_bodyID != 0)
    {
        SetLinearVelocity(zero);
        SetAngularVelocity(zero);
        ZeroForceAccumulators();
    }
}
