#ifndef GAME_PHYSICS_PHYSICS_SHELL_H
#define GAME_PHYSICS_PHYSICS_SHELL_H

#include "Game/Physics/PhysicsSphere.h"

class PowerupBase;

class PhysicsSphere_80174F04 : public PhysicsSphere
{
public:
    PhysicsSphere_80174F04(float radius);
    virtual ContactType Contact(
        PhysicsObject* other, dContact* info, int numContacts);

    /* 0x38 */ void (*m_pTriggerCallbackFunc)(PhysicsObject*, PhysicsObject*,
        nlVector3&, void*);
    /* 0x3C */ void* m_pCallbackParam;
}; // total size: 0x40

class PhysicsShell : public PhysicsSphere
{
public:
    PhysicsShell(float radius);
    virtual int GetObjectType() const { return 0x14; }
    virtual bool SetContactInfo(
        dContact* contact, PhysicsObject* other, bool first);
    virtual void PreUpdate();
    virtual void PostUpdate();
    virtual ContactType Contact(
        PhysicsObject* obj, dContact* info, int numContacts);

    /* 0x38 */ void (*m_pTriggerCallbackFunc)(PhysicsObject*, PhysicsObject*, nlVector3&, void*);
    /* 0x3C */ void* m_pCallbackParam;
    /* 0x40 */ PowerupBase* m_pPowerupObject;
    /* 0x44 */ bool mbIsInNet;
    /* 0x45 */ bool m_bIsSupportedByGround;
    /* 0x46 */ bool mUnidentified046;
    /* 0x47 */ u8 mPadding047;
    /* 0x48 */ int mUnidentified048;
    /* 0x4C */ float mUnidentified04C;
}; // total size: 0x50

#endif // GAME_PHYSICS_PHYSICS_SHELL_H
