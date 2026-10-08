#include "Game/World/WorldPhysics.h"
#include "Game/World/WorldPhysicsDescription.h"
#include "Game/World/WorldAnimObjects.h"
#include "Game/World/WorldHelperObject.h"

#include "Game/Physics/PhysicsBox.h"
#include "Game/Physics/PhysicsCapsule.h"
#include "Game/Physics/PhysicsFinitePlane.h"
#include "Game/Physics/PhysicsPlane.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlMemory.h"

void WorldHelperObject::SetWorldMatrix(const nlMatrix4& transform)
{
}

PhysicsObject* CreatePhysicsPrimitive(
    const WorldPhysicsDescription* pDescription,
    CollisionSpace* pCollisionSpace)
{
    PhysicsObject* pPhysicsObject = 0;
    switch (pDescription->uPrimitiveType)
    {
    case PHYS_PRIMITIVE_BOX:
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsBox), 8, false)) PhysicsBox(
                pCollisionSpace, 0, pDescription->fLength,
                pDescription->fWidth, pDescription->fHeight);
        pPhysicsObject->SetWorldMatrix(pDescription->matLocalToParent);
        break;
    case PHYS_PRIMITIVE_SPHERE:
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsSphere), 8, false)) PhysicsSphere(
                pCollisionSpace, 0, pDescription->fRadius);
        pPhysicsObject->SetWorldMatrix(pDescription->matLocalToParent);
        break;
    case PHYS_PRIMITIVE_CAPSULE:
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsCapsule), 8, false))
                PhysicsCapsule(pCollisionSpace, 0,
                    pDescription->fRadius, pDescription->fHeight);
        pPhysicsObject->SetWorldMatrix(pDescription->matLocalToParent);
        break;
    case PHYS_PRIMITIVE_FINITE_PLANE:
    {
        const nlMatrix4& transform = pDescription->matLocalToParent;
        nlVector3 position;
        nlVector3 axis0;
        nlVector3 axis1;
        nlVector3 normal;
        transform.GetRow_(3, position);
        transform.GetRow_(0, axis0);
        transform.GetRow_(1, axis1);
        transform.GetRow_(2, normal);
        nlVec3Scale(axis0, 0.5f * pDescription->fWidth);
        nlVec3Scale(axis1, 0.5f * pDescription->fLength);
        pPhysicsObject = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false))
            PhysicsFinitePlane(pCollisionSpace, position, axis0, axis1, true, -1.0f);
        break;
    }
    case PHYS_PRIMITIVE_PLANE:
    {
        const nlMatrix4& transform = pDescription->matLocalToParent;
        nlVector3 normal;
        transform.GetRow_(2, normal);
        float distance = nlVec3DotProduct(normal, transform.GetTranslation());
        pPhysicsObject = new (nlMalloc(sizeof(PhysicsPlane), 8, false))
            PhysicsPlane(pCollisionSpace, normal.x, normal.y, normal.z, distance);
        break;
    }
    }

    pPhysicsObject->SetCategory(0xFF);
    pPhysicsObject->SetCollide(0xFF);
    return pPhysicsObject;
}

void ReleaseWorldPhysicsObject(WorldPhysicsDrawable* pOwner)
{
    if (pOwner->m_pPhysicsObject != 0)
    {
        delete pOwner->m_pPhysicsObject;
        pOwner->m_pPhysicsObject = 0;
    }
}
