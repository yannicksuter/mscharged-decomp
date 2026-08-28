#include "Game/Physics/PhysicsObject.h"

extern const float lbl_806E49B0;
extern const float lbl_806E49B4;
extern const float lbl_806E49B8;

extern "C" bool fn_801711C0(PhysicsObject* object, dContact* contact,
    PhysicsObject*, bool setDefault)
{
    if (setDefault)
    {
        object->SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = lbl_806E49B4;
    contact->surface.bounce_vel = lbl_806E49B0;
    contact->surface.mu = lbl_806E49B8;
    return true;
}

extern "C" void fn_80171210(PhysicsObject*)
{
}

extern "C" int fn_80171214(const PhysicsObject*)
{
    return 0x22;
}

extern "C" PhysicsObject* fn_8017121C(
    PhysicsObject* object, int shouldDelete)
{
    if (object != 0)
    {
        object->PhysicsObject::~PhysicsObject();
        if (shouldDelete > 0)
        {
            ::operator delete(object);
        }
    }
    return object;
}
