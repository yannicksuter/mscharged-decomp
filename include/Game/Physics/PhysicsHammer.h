#ifndef GAME_PHYSICS_PHYSICS_HAMMER_H
#define GAME_PHYSICS_PHYSICS_HAMMER_H

#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlArrayAllocator.h"

struct HammerObject;

class PhysicsHammer;
extern nlArrayAllocator<PhysicsHammer> gPhysicsHammerAllocator;

class PhysicsHammer : public PhysicsSphere
{
public:
    PhysicsHammer(float radius);
    virtual ~PhysicsHammer();

    virtual int GetObjectType() const { return 0x1F; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PostUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    void EnableGravity();
    void Freeze();

    static void* operator new(unsigned long)
    {
        return gPhysicsHammerAllocator.Allocate();
    }

    static void operator delete(void* object)
    {
        gPhysicsHammerAllocator.Free((PhysicsHammer*)object);
    }

    /* 0x38 */ HammerObject* mHammer;
}; // size: 0x3C

#endif // GAME_PHYSICS_PHYSICS_HAMMER_H
