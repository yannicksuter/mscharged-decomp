#ifndef GAME_PHYSICS_PHYSICS_NPC_H
#define GAME_PHYSICS_PHYSICS_NPC_H

#include "Game/Physics/PhysicsSphere.h"

class SkinAnimatedMovableNPC;

class PhysicsNPC : public PhysicsSphere
{
public:
    typedef void (*CallbackFn)(PhysicsObject*, PhysicsObject*, const nlVector3&);

    /* 0x38 */ CallbackFn mpTriggerCallbackFunc;
    /* 0x3C */ SkinAnimatedMovableNPC* mpAINPC;
}; // total size: 0x40

#endif // GAME_PHYSICS_PHYSICS_NPC_H
