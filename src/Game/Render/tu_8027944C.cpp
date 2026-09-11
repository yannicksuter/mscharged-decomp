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
        nlVector3 position = { transform.m41, transform.m42, transform.m43 };
        nlVector3 axis0 = { transform.m11, transform.m12, transform.m13 };
        nlVector3 axis1 = { transform.m21, transform.m22, transform.m23 };
        nlVector3 axis2 = { transform.m31, transform.m32, transform.m33 };
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

void StadiumPhysicsObject_8027944C::fn_80279700(void*)
{
    m_pPhysicsObject = fn_8027944C(this);
}

void StadiumPhysicsObject_8027944C::fn_80279730()
{
    g_StaticPhysicsPrimitives.RemoveEntry(m_pPhysicsObject);
    g_NetPhysicsObjects.RemoveEntry(m_pPhysicsObject);
    fn_8034417C(this);
}

void StadiumPhysicsObject_8027944C::SetWorldMatrix(const nlMatrix4* transform)
{
    m_Description.m_transform = *transform;
}

nlMatrix4* StadiumPhysicsObject_8027944C::GetWorldMatrix()
{
    return &m_Description.m_transform;
}

StadiumPhysicsObject_8027944C::~StadiumPhysicsObject_8027944C()
{
}

void StadiumMarker_8027999C::fn_8027999C(void*)
{
    nlSingleton<FEModelManager>::Instance()->RegisterObject(this);
}

void StadiumMarker_8027999C::fn_802799A8()
{
}

void StadiumMarker_802799AC::fn_802799AC(void*)
{
    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    if (stadium != 0)
    {
        nlMatrix4* transform = GetWorldMatrix();
        nlVector3 position = *(nlVector3*)&transform->m41;
        fn_802788BC(stadium, position.z);
    }
}

void StadiumMarker_802799AC::fn_80279A24()
{
}

StadiumMarker_8027999C::~StadiumMarker_8027999C()
{
}

StadiumMarker_802799AC::~StadiumMarker_802799AC()
{
}
