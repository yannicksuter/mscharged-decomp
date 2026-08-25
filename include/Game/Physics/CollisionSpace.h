#ifndef GAME_PHYSICS_COLLISION_SPACE_H
#define GAME_PHYSICS_COLLISION_SPACE_H

#include "Game/Physics/PhysicsWorld.h"
#include "ode/collision.h"

class PhysicsObject;
class PhysicsWorld;

class CollisionSpace
{
public:
    CollisionSpace(PhysicsWorld* physicsWorld, bool addToWorld)
    {
        m_physicsWorld = physicsWorld;
        if (addToWorld)
        {
            physicsWorld->AddCollisionSpace(this);
        }
    }
    virtual ~CollisionSpace();

    void DoCollide(void*, dNearCallback*);
    void CallPreCollide(PhysicsObject*);
    void PreCollide();
    void PreUpdate();
    void CallLogGeoms(PhysicsObject*);
    void SyncLogSpace();

    /* 0x04 */ dSpaceID m_spaceID;
    /* 0x08 */ CollisionSpace* m_nextCollisionSpace;
    /* 0x0C */ PhysicsWorld* m_physicsWorld;
}; // size: 0x10

extern CollisionSpace* g_CollisionSpace;

#endif
