#ifndef GAME_PHYSICS_PHYSICS_YOSHI_EGG_H
#define GAME_PHYSICS_PHYSICS_YOSHI_EGG_H

#include "Game/Physics/PhysicsSphere.h"

class UnidentifiedObject_801B535C;

class PhysicsYoshiEgg : public PhysicsSphere
{
public:
    PhysicsYoshiEgg(UnidentifiedObject_801B535C*, float);
    virtual ~PhysicsYoshiEgg();
    virtual int GetObjectType() const;
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    /* 0x38 */ void* mUnidentified38;
    /* 0x3C */ UnidentifiedObject_801B535C* mYoshiEgg;
}; // total size: 0x40

#endif // GAME_PHYSICS_PHYSICS_YOSHI_EGG_H
