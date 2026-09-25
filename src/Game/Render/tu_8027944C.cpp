#include "Game/Render/tu_8027944C.h"

#include "Game/BasicStadium.h"
#include "Game/FE/feModelManager.h"
#include "Game/Physics/PhysicsFinitePlane.h"
#include "NL/nlList.h"
#include "NL/nlListContainer.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"

extern nlListContainer<PhysicsObject*> g_StaticPhysicsPrimitives;
extern nlListContainer<PhysicsObject*> g_NetPhysicsObjects;
extern "C" PhysicsObject* fn_80341EEC(WorldPhysicsDescription_80341EEC*, CollisionSpace*);
extern "C" void fn_8034417C(void*);
void fn_802788BC(BasicStadium*, float);

float lbl_806DEE78 = 0.75f;
float lbl_806DEE7C = 10.0f;

extern "C" PhysicsObject* fn_8027944C(StadiumPhysicsObject_8027944C* object)
{
    WorldPhysicsDescription_80341EEC* pDescription = &object->m_Description;
    PhysicsObject* pPhysicsObject;
    switch (pDescription->m_uType)
    {
    case 4:
    {
        const nlMatrix4& transform = pDescription->m_transform;
        bool inside = false;
        nlVector3 position;
        nlVector3 axis0;
        nlVector3 axis1;
        nlVector3 axis2;
        transform.GetRow_(3, position);
        transform.GetRow_(0, axis0);
        transform.GetRow_(1, axis1);
        transform.GetRow_(2, axis2);
        if ((position.x > 0.0f && axis2.x > 0.01f)
            || (position.x < 0.0f && axis2.x < -0.01f))
            inside = true;
        nlVec3Scale(axis0, 0.5f * pDescription->m_f44);
        nlVec3Scale(axis1, 0.5f * pDescription->m_f48);
        pPhysicsObject = new (8, false) PhysicsFinitePlane(
            0, position, axis0, axis1, true, inside ? lbl_806DEE78 : lbl_806DEE7C);
        g_StaticPhysicsPrimitives.AddEnd(pPhysicsObject);
        break;
    }
    default:
        pPhysicsObject = fn_80341EEC(pDescription, 0);
        g_StaticPhysicsPrimitives.AddEnd(pPhysicsObject);
        break;
    }
    pPhysicsObject->SetCategory(0x800);
    pPhysicsObject->SetCollide(0x20);
    g_NetPhysicsObjects.AddEnd(pPhysicsObject);
    return pPhysicsObject;
}

void StadiumPhysicsObject_8027944C::UnidentifiedVirtual1C(WorldObjectLoadContext*)
{
    m_pPhysicsObject = fn_8027944C(this);
}

void StadiumPhysicsObject_8027944C::ReleaseResources()
{
    g_StaticPhysicsPrimitives.RemoveEntry(m_pPhysicsObject);
    g_NetPhysicsObjects.RemoveEntry(m_pPhysicsObject);
    fn_8034417C(this);
}

void WorldPhysicsDrawable_80534448::SetWorldMatrix(const nlMatrix4& transform)
{
    m_Description.m_transform = transform;
}

nlMatrix4* WorldPhysicsDrawable_80534448::GetWorldMatrix()
{
    return &m_Description.m_transform;
}

StadiumPhysicsObject_8027944C::~StadiumPhysicsObject_8027944C()
{
}

void StadiumMarker_8027999C::UnidentifiedVirtual1C(WorldObjectLoadContext*)
{
    nlSingleton<FEModelManager>::Instance()->RegisterObject(this);
}

void StadiumMarker_8027999C::ReleaseResources()
{
}

void StadiumMarker_802799AC::UnidentifiedVirtual1C(WorldObjectLoadContext*)
{
    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    if (stadium != 0)
    {
        nlMatrix4* transform = GetWorldMatrix();
        nlVector3 position = *(nlVector3*)&transform->m41;
        fn_802788BC(stadium, position.z);
    }
}

void StadiumMarker_802799AC::ReleaseResources()
{
}

StadiumMarker_8027999C::~StadiumMarker_8027999C()
{
}

StadiumMarker_802799AC::~StadiumMarker_802799AC()
{
}
