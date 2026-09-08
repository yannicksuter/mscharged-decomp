#ifndef GAME_PHYSICS_PHYSICS_SPHERE_801798A8_H
#define GAME_PHYSICS_PHYSICS_SPHERE_801798A8_H

#include "Game/Physics/PhysicsSphere.h"

class UnidentifiedObject_801B535C;

class PhysicsSphere_801798A8 : public PhysicsSphere
{
public:
    PhysicsSphere_801798A8(UnidentifiedObject_801B535C*, float);
    virtual ~PhysicsSphere_801798A8();
    virtual int GetObjectType() const;
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    /* 0x38 */ void* mUnidentified38;
    /* 0x3C */ UnidentifiedObject_801B535C* mUnidentified3C;
}; // size: 0x40

#endif
