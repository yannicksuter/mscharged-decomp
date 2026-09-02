#include "Game/Physics/Physics.h"
#include "unclassified/tu_80176EF4.h"

PhysicsBox_80176EF4::PhysicsBox_80176EF4(
    ThwompObject* object, float lx, float ly, float lz)
    : PhysicsBox(g_CollisionSpace, g_PhysicsWorld, lx, ly, lz)
    , mUnidentified038(lz)
    , mUnidentified03C(object)
{
    SetCollide(0x1F062);
    SetCategory(0x18000);
    m_gravity = 0.0f;
}

PhysicsBox_80176EF4::~PhysicsBox_80176EF4()
{
}

extern const float lbl_806DCB08;
extern float lbl_806E12D8;
extern const float lbl_806E4A80;

void PhysicsBox_80176EF4::PreCollide()
{
}

void PhysicsBox_80176EF4::PostUpdate()
{
    PhysicsObject::PostUpdate();
}

bool PhysicsBox_80176EF4::SetContactInfo(dContact* contact,
    PhysicsObject* otherObject, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = lbl_806DCB08;
    contact->surface.mu = lbl_806E12D8;
    contact->surface.bounce_vel = lbl_806E4A80;
    return true;
}

extern "C" void* fn_80177458(void* object, int shouldDelete)
{
    if (object != 0 && shouldDelete > 0)
    {
        ::operator delete(object);
    }
    return object;
}
