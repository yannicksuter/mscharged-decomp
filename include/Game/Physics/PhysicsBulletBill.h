#ifndef GAME_PHYSICS_PHYSICS_BULLET_BILL_H
#define GAME_PHYSICS_PHYSICS_BULLET_BILL_H

#include "Game/Physics/PhysicsSphere.h"

struct BulletBillObject;

class PhysicsBulletBill : public PhysicsSphere
{
public:
    PhysicsBulletBill(BulletBillObject*, float, float);
    virtual ~PhysicsBulletBill();
    virtual int GetObjectType() const { return 0x1E; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    /* 0x38 */ void* mUnidentified38;
    /* 0x3C */ BulletBillObject* mBulletBill;
}; // total size: 0x40

#endif // GAME_PHYSICS_PHYSICS_BULLET_BILL_H
