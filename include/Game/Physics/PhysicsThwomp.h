#ifndef GAME_PHYSICS_PHYSICS_THWOMP_H
#define GAME_PHYSICS_PHYSICS_THWOMP_H

#include "Game/Physics/PhysicsBox.h"
#include "NL/nlArrayAllocator.h"

struct ThwompObject;

class PhysicsThwomp;
extern nlArrayAllocator<PhysicsThwomp> gPhysicsThwompAllocator;

class PhysicsThwomp : public PhysicsBox
{
public:
    PhysicsThwomp(
        ThwompObject* object, float lx, float ly, float lz);
    virtual ~PhysicsThwomp();

    virtual int GetObjectType() const { return 0x24; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PostUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    static void* operator new(unsigned long)
    {
        return gPhysicsThwompAllocator.Allocate();
    }

    static void operator delete(void* object)
    {
        gPhysicsThwompAllocator.Free((PhysicsThwomp*)object);
    }

    /* 0x38 */ float mHeight;
    /* 0x3C */ ThwompObject* mThwomp;
}; // size: 0x40

#endif // GAME_PHYSICS_PHYSICS_THWOMP_H
