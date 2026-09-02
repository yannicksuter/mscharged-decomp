#ifndef GAME_PHYSICS_PHYSICS_NPC_H
#define GAME_PHYSICS_PHYSICS_NPC_H

#include "Game/Physics/PhysicsSphere.h"

class SkinAnimatedMovableNPC;

class PhysicsNPC : public PhysicsSphere
{
public:
    typedef void (*CallbackFn)(PhysicsObject*, PhysicsObject*, const nlVector3&);

    PhysicsNPC(float radius);
    virtual int GetObjectType() const { return 0x18; }
    virtual bool SetContactInfo(
        dContact* contact, PhysicsObject* other, bool first);
    virtual void PreUpdate();
    virtual void PostUpdate();
    virtual ContactType Contact(
        PhysicsObject* object, dContact* contact, int numContacts);

    void SetCallbackFunction(CallbackFn callback);

    /* 0x38 */ CallbackFn mpTriggerCallbackFunc;
    /* 0x3C */ SkinAnimatedMovableNPC* mpAINPC;
    /* 0x40 */ bool mUnidentified040;
    /* 0x41 */ unsigned char mPadding041[3];
    /* 0x44 */ int mUnidentified044;
    /* 0x48 */ float mUnidentified048;
}; // total size: 0x4C

#endif // GAME_PHYSICS_PHYSICS_NPC_H
