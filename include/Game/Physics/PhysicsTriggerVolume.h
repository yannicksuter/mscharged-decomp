#ifndef GAME_PHYSICS_PHYSICS_TRIGGER_VOLUME_H
#define GAME_PHYSICS_PHYSICS_TRIGGER_VOLUME_H

#include "Game/Physics/PhysicsSphere.h"

class PhysicsTriggerVolume : public PhysicsSphere
{
public:
    PhysicsTriggerVolume(float radius);
    virtual ~PhysicsTriggerVolume();
    virtual ContactType Contact(
        PhysicsObject* other, dContact* info, int numContacts);

    /* 0x38 */ void (*m_pTriggerCallbackFunc)(PhysicsObject*, PhysicsObject*,
        nlVector3&, void*);
    /* 0x3C */ void* m_pCallbackParam;
}; // total size: 0x40

#endif // GAME_PHYSICS_PHYSICS_TRIGGER_VOLUME_H
