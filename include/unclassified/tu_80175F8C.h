#ifndef UNCLASSIFIED_TU_80175F8C_H
#define UNCLASSIFIED_TU_80175F8C_H

#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"

class PhysicsSphere_80175F8C : public PhysicsSphere
{
public:
    PhysicsSphere_80175F8C(void* owner, const nlVector3& position,
        int effectType, float maximumRadius, float growthRate);

    virtual ~PhysicsSphere_80175F8C();
    virtual int GetObjectType() const { return 0x23; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    static void* operator new(unsigned long)
    {
        PhysicsSphere_80175F8C* object = 0;
        pool.Allocate(object);
        return object;
    }

    static void operator delete(void* object)
    {
        pool.Free((PhysicsSphere_80175F8C*)object);
    }

    static SlotPool<PhysicsSphere_80175F8C> pool;

    /* 0x38 */ void* owner;
    /* 0x3C */ nlVector3 position;
    /* 0x48 */ int effectType;
    /* 0x4C */ float currentRadius;
    /* 0x50 */ float maximumRadius;
    /* 0x54 */ float growthRate;
    /* 0x58 */ int sourceIndex;
    /* 0x5C */ bool finished;
}; // size: 0x60

#endif // UNCLASSIFIED_TU_80175F8C_H
