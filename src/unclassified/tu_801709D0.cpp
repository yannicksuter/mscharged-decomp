#include "Game/Physics/PhysicsSphere.h"

struct KoopaShellObject;

class PhysicsSphere_801709D0 : public PhysicsSphere
{
public:
    virtual ~PhysicsSphere_801709D0();
    virtual int GetObjectType() const;
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    /* 0x38 */ void* mUnidentified038;
    /* 0x3C */ KoopaShellObject* mObject;
};

extern const float lbl_806E49B0;
extern const float lbl_806E49B4;
extern const float lbl_806E49B8;

bool PhysicsSphere_801709D0::SetContactInfo(dContact* contact,
    PhysicsObject*, bool setDefault)
{
    if (setDefault)
    {
        SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = lbl_806E49B4;
    contact->surface.bounce_vel = lbl_806E49B0;
    contact->surface.mu = lbl_806E49B8;
    return true;
}

void PhysicsSphere_801709D0::PreCollide()
{
}

int PhysicsSphere_801709D0::GetObjectType() const
{
    return 0x22;
}

PhysicsSphere_801709D0::~PhysicsSphere_801709D0()
{
}
