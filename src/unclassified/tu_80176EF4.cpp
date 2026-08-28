#include "Game/Physics/PhysicsObject.h"

extern const float lbl_806DCB08;
extern float lbl_806E12D8;
extern const float lbl_806E4A80;

extern "C" void fn_80177280(PhysicsObject*)
{
}

extern "C" void fn_80177284(PhysicsObject* object)
{
    object->PhysicsObject::PostUpdate();
}

extern "C" bool fn_80177288(PhysicsObject* object, dContact* contact,
    PhysicsObject*, bool setDefault)
{
    if (setDefault)
    {
        object->SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = lbl_806DCB08;
    contact->surface.mu = lbl_806E12D8;
    contact->surface.bounce_vel = lbl_806E4A80;
    return true;
}

extern "C" int fn_80177450(const PhysicsObject*)
{
    return 0x24;
}

extern "C" void* fn_80177458(void* object, int shouldDelete)
{
    if (object != 0 && shouldDelete > 0)
    {
        ::operator delete(object);
    }
    return object;
}
