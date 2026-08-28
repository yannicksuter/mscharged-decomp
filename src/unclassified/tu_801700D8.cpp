#include "Game/Physics/PhysicsObject.h"

extern const nlVector3 lbl_804DCC90;
extern const float lbl_806E49A0;
extern const float lbl_806E49A4;
extern const float lbl_806E49A8;
extern const float lbl_806E49AC;

extern "C" void fn_80170708(PhysicsObject*)
{
}

extern "C" void fn_8017070C(PhysicsObject* object)
{
    object->PhysicsObject::PostUpdate();
}

extern "C" bool fn_80170710(PhysicsObject* object, dContact* contact,
    PhysicsObject*, bool setDefault)
{
    if (setDefault)
    {
        object->SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = lbl_806E49A8;
    contact->surface.mu = lbl_806E49AC;
    contact->surface.bounce_vel = lbl_806E49A4;
    return true;
}

extern "C" void fn_80170760(PhysicsObject* object)
{
    object->m_gravity = lbl_806E49A0;
}

extern "C" void fn_8017076C(PhysicsObject* object)
{
    object->SetLinearVelocity(lbl_804DCC90);
    object->SetAngularVelocity(lbl_804DCC90);
    object->m_gravity = lbl_806E49A4;
}

extern "C" int fn_80170988(const PhysicsObject*)
{
    return 0x1F;
}

extern "C" void* fn_80170990(void* object, int shouldDelete)
{
    if (object != 0 && shouldDelete > 0)
    {
        ::operator delete(object);
    }
    return object;
}
