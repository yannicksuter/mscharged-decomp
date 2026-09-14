#ifndef UNCLASSIFIED_TU_80174ED4_H
#define UNCLASSIFIED_TU_80174ED4_H

#include "Game/Physics/PhysicsSphere.h"

class PhysicsSphere_80174F04 : public PhysicsSphere
{
public:
    PhysicsSphere_80174F04(float radius);
    virtual ~PhysicsSphere_80174F04();
    virtual ContactType Contact(
        PhysicsObject* other, dContact* info, int numContacts);

    /* 0x38 */ void (*m_pTriggerCallbackFunc)(PhysicsObject*, PhysicsObject*,
        nlVector3&, void*);
    /* 0x3C */ void* m_pCallbackParam;
}; // total size: 0x40

#endif // UNCLASSIFIED_TU_80174ED4_H
