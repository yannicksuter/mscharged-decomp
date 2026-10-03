#include "Game/Render/StadiumPhysicsObject.h"

#include "Game/BasicStadium.h"
#include "Game/FE/feModelManager.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsFinitePlane.h"
#include "Game/World/worldanim.h"
#include "NL/nlList.h"
#include "NL/nlListContainer.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"

float sfStaticFinitePlaneThinDepth = 0.75f;
float sfStaticFinitePlaneThickDepth = 10.0f;

PhysicsObject* ConstructStaticPhysicsPrimitive(StadiumPhysicsObject* object)
{
    WorldPhysicsDescription* physElement = &object->m_Description;
    PhysicsObject* obj;
    switch (physElement->uPrimitiveType)
    {
    case 4:
    {
        const nlMatrix4& transform = physElement->matLocalToParent;
        bool normalPointsAwayFromField = false;
        nlVector3 centre;
        nlVector3 v1;
        nlVector3 v2;
        nlVector3 normal;
        transform.GetRow_(3, centre);
        transform.GetRow_(0, v1);
        transform.GetRow_(1, v2);
        transform.GetRow_(2, normal);
        if ((centre.x > 0.0f && normal.x > 0.01f)
            || (centre.x < 0.0f && normal.x < -0.01f))
            normalPointsAwayFromField = true;
        nlVec3Scale(v1, 0.5f * physElement->fWidth);
        nlVec3Scale(v2, 0.5f * physElement->fLength);
        obj = new (8, false) PhysicsFinitePlane(0, centre, v1, v2, true,
            normalPointsAwayFromField ? sfStaticFinitePlaneThinDepth : sfStaticFinitePlaneThickDepth);
        g_StaticPhysicsPrimitives.AddEnd(obj);
        break;
    }
    default:
        obj = CreatePhysicsPrimitive(physElement, 0);
        g_StaticPhysicsPrimitives.AddEnd(obj);
        break;
    }
    obj->SetCategory(0x800);
    obj->SetCollide(0x20);
    g_NetPhysicsObjects.AddEnd(obj);
    return obj;
}

void StadiumPhysicsObject::Initialize(WorldObjectLoadContext*)
{
    m_pPhysicsObject = ConstructStaticPhysicsPrimitive(this);
}

void StadiumPhysicsObject::ReleaseResources()
{
    g_StaticPhysicsPrimitives.RemoveEntry(m_pPhysicsObject);
    g_NetPhysicsObjects.RemoveEntry(m_pPhysicsObject);
    WorldPhysicsDrawable::ReleaseResources();
}

void WorldPhysicsDrawable::SetWorldMatrix(const nlMatrix4& transform)
{
    m_Description.matLocalToParent = transform;
}

nlMatrix4* WorldPhysicsDrawable::GetWorldMatrix()
{
    return &m_Description.matLocalToParent;
}

StadiumPhysicsObject::~StadiumPhysicsObject()
{
}

void StadiumFEModelMarker::Initialize(WorldObjectLoadContext*)
{
    nlSingleton<FEModelManager>::Instance()->RegisterObject(this);
}

void StadiumFEModelMarker::ReleaseResources()
{
}

void StadiumShadowHeightMarker::Initialize(WorldObjectLoadContext*)
{
    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    if (stadium != 0)
    {
        nlMatrix4* transform = GetWorldMatrix();
        nlVector3 position = *(nlVector3*)&transform->m41;
        SetStadiumShadowHeight(stadium, position.z);
    }
}

void StadiumShadowHeightMarker::ReleaseResources()
{
}

StadiumFEModelMarker::~StadiumFEModelMarker()
{
}

StadiumShadowHeightMarker::~StadiumShadowHeightMarker()
{
}
