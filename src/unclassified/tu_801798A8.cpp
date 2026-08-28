#include "Game/Physics/PhysicsObject.h"

extern const float lbl_806E4AF0;
extern const float lbl_806E4AF4;
extern const float lbl_806E4AF8;

extern "C" bool fn_8017A018(PhysicsObject* object, dContact* contact,
    PhysicsObject*, bool setDefault)
{
    if (setDefault)
    {
        object->SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = lbl_806E4AF4;
    contact->surface.bounce_vel = lbl_806E4AF0;
    contact->surface.mu = lbl_806E4AF8;
    return true;
}

extern "C" void fn_8017A068(PhysicsObject*)
{
}

extern "C" int fn_8017A06C(const PhysicsObject*)
{
    return 0x20;
}

extern "C" PhysicsObject* fn_8017A074(
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
