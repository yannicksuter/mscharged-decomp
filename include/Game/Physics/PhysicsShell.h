#ifndef GAME_PHYSICS_PHYSICS_SHELL_H
#define GAME_PHYSICS_PHYSICS_SHELL_H

#include "Game/Physics/PhysicsSphere.h"

class PowerupBase;

class PhysicsShell : public PhysicsSphere
{
public:
    virtual int GetObjectType() const { return 0x14; }

    /* 0x38 */ void (*m_pTriggerCallbackFunc)(PhysicsObject*, PhysicsObject*, nlVector3&, void*);
    /* 0x3C */ void* m_pCallbackParam;
    /* 0x40 */ PowerupBase* m_pPowerupObject;
    /* 0x44 */ u8 mUnidentified044[0x0C];
}; // total size: 0x50

#endif // GAME_PHYSICS_PHYSICS_SHELL_H
