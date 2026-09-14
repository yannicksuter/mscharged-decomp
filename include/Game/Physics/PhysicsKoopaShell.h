#ifndef GAME_PHYSICS_PHYSICS_KOOPA_SHELL_H
#define GAME_PHYSICS_PHYSICS_KOOPA_SHELL_H

#include "Game/Physics/PhysicsSphere.h"

struct KoopaShellObject;

class PhysicsKoopaShell : public PhysicsSphere
{
public:
    PhysicsKoopaShell(KoopaShellObject*, float);
    virtual ~PhysicsKoopaShell();
    virtual int GetObjectType() const;
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    /* 0x38 */ void* mUnidentified38;
    /* 0x3C */ KoopaShellObject* mKoopaShell;
}; // total size: 0x40

#endif // GAME_PHYSICS_PHYSICS_KOOPA_SHELL_H
