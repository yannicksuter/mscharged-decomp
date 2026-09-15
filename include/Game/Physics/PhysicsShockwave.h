#ifndef GAME_PHYSICS_PHYSICS_SHOCKWAVE_H
#define GAME_PHYSICS_PHYSICS_SHOCKWAVE_H

#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"

class cCharacter;
class cFielder;
struct BulletBillObject;

enum eShockwaveType
{
    SHOCKWAVE_EXPLOSION = 0,
    SHOCKWAVE_HIT = 1,
    SHOCKWAVE_LIGHTNING = 2,
    SHOCKWAVE_FREEZE = 3,
    SHOCKWAVE_BULLET_BILL = 4,
    SHOCKWAVE_DAISY_FIST = 5,
};

class PhysicsShockwave : public PhysicsSphere
{
public:
    PhysicsShockwave(void* owner, const nlVector3& position,
        int type, float maximumRadius, float expansionRate);

    virtual ~PhysicsShockwave();
    virtual int GetObjectType() const { return 0x23; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    static void* operator new(unsigned long)
    {
        PhysicsShockwave* object = 0;
        pool.Allocate(object);
        return object;
    }

    static void operator delete(void* object)
    {
        pool.Free((PhysicsShockwave*)object);
    }

    static SlotPool<PhysicsShockwave> pool;

    /* 0x38 */ void* mOwner;
    /* 0x3C */ nlVector3 mPosition;
    /* 0x48 */ int mType;
    /* 0x4C */ float mRadius;
    /* 0x50 */ float mMaximumRadius;
    /* 0x54 */ float mExpansionRate;
    /* 0x58 */ int mSourceIndex;
    /* 0x5C */ bool mFinished;
}; // size: 0x60

extern "C" void InitializeShockwaves();
extern "C" void ShutdownShockwaves();
extern "C" void UpdateShockwaves(float dt);

extern "C" PhysicsShockwave* CreatePowerupShockwave(
    const nlVector3* position, cFielder* owner, bool frozen,
    int sourceIndex, float maximumRadius);
extern "C" PhysicsShockwave* CreateHitShockwave(
    cFielder* owner, const nlVector3* position, float maximumRadius);
extern "C" PhysicsShockwave* CreateBulletBillShockwave(
    BulletBillObject* bulletBill);
extern "C" PhysicsShockwave* CreateLightningShockwave(
    const nlVector3* position, float maximumRadius);
extern "C" PhysicsShockwave* CreateExplosionShockwave(
    const nlVector3* position);

PhysicsShockwave* CreateDaisyFistImpact(
    const nlVector3* position, cCharacter* owner);

#endif // GAME_PHYSICS_PHYSICS_SHOCKWAVE_H
