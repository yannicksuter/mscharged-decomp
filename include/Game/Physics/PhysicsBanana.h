#ifndef GAME_PHYSICS_PHYSICS_BANANA_H
#define GAME_PHYSICS_PHYSICS_BANANA_H

#include "Game/Physics/PhysicsSphere.h"

class PowerupBase;

class PhysicsBanana : public PhysicsSphere
{
public:
    PhysicsBanana(float radius);
    virtual ~PhysicsBanana();
    virtual int GetObjectType() const { return 0x15; }
    virtual bool SetContactInfo(dContact* contact, PhysicsObject* other, bool first);
    virtual void PreUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject* other, dContact* contact, int numContacts);

    /* 0x38 */ void (*m_pTriggerCallbackFunc)(PhysicsObject*, PhysicsObject*, nlVector3&, void*);
    /* 0x3C */ void* m_pCallbackParam;
    /* 0x40 */ PowerupBase* m_pPowerupObject;
    /* 0x44 */ bool m_bIsSupportedByGround;
    /* 0x45 */ u8 mUnidentified045[0x03];
}; // total size: 0x48

#endif // GAME_PHYSICS_PHYSICS_BANANA_H
