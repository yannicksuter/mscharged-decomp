#ifndef GAME_PHYSICS_PHYSICS_WORLD_H
#define GAME_PHYSICS_PHYSICS_WORLD_H

#include "NL/nlList.h"
#include "ode/collision.h"
#include "ode/objects.h"

class CollisionSpace;
class DebugWriteCache;
class PhysicsObject;

class PhysicsWorld
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void SetCFM(float);
    void SetERP(float);
    void AddCollisionSpace(CollisionSpace*);
    void PreCollide(CollisionSpace*);
    void PreCollide(PhysicsObject*);
    void PreUpdate(CollisionSpace*);
    void PreUpdate(PhysicsObject*);
    void PostUpdate(PhysicsObject*);
    void DoCollide(CollisionSpace*);
    void Collide();
    void PreUpdate();
    void PostUpdate();
    void Update(float, bool);
    static void SpaceCollideCallback(void*, dxGeom*, dxGeom*);
    void LogBody(PhysicsObject*);
    void LogSpace(CollisionSpace*);
    void LogGeom(PhysicsObject*);
    void SyncLog(void*, DebugWriteCache*);
    void DoCollisions(PhysicsObject*, nlListContainer<PhysicsObject*>&);

    typedef void (PhysicsWorld::*PhysicsObjectCallback)(PhysicsObject*);
    typedef void (PhysicsWorld::*CollisionSpaceCallback)(CollisionSpace*);

    /* 0x00 */ dWorldID m_World;
    /* 0x04 */ dJointGroupID m_ContactGroup;
    /* 0x08 */ CollisionSpace* m_SpaceList;
    /* 0x0C */ void* m_SyncLogContext;
    /* 0x10 */ DebugWriteCache* m_SyncLogCache;
}; // size: 0x14

#endif
