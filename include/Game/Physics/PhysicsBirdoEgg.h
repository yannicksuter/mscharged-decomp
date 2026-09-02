#ifndef GAME_PHYSICS_PHYSICS_BIRDO_EGG_H
#define GAME_PHYSICS_PHYSICS_BIRDO_EGG_H

#include "Game/Physics/PhysicsSphere.h"

struct BirdoEggObject;

class PhysicsBirdoEgg : public PhysicsSphere
{
public:
    PhysicsBirdoEgg(BirdoEggObject*, float);
    virtual ~PhysicsBirdoEgg();
    virtual int GetObjectType() const { return 0x21; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    /* 0x38 */ void* mUnidentified38;
    /* 0x3C */ BirdoEggObject* mBirdoEgg;
}; // total size: 0x40

#endif // GAME_PHYSICS_PHYSICS_BIRDO_EGG_H
