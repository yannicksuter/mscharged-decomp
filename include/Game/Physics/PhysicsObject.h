#ifndef GAME_PHYSICS_PHYSICS_OBJECT_H
#define GAME_PHYSICS_PHYSICS_OBJECT_H

#include "ode/collision.h"
#include "ode/contact.h"
#include "ode/objects.h"

class DebugWriteCache;
class nlMatrix4;
class nlVector3;
class PhysicsWorld;

enum ContactType
{
    NO_CONTACT = 0,
    ONE_WAY_CONTACT_THIS = 1,
    ONE_WAY_CONTACT_OTHER = 2,
    TWO_WAY_CONTACT = 3,
};

class PhysicsObject
{
public:
    enum CoordinateType
    {
        WORLD_COORDINATES = 0,
        RELATIVE_TO_PARENT = 1,
    };

    PhysicsObject(PhysicsWorld*);
    virtual ~PhysicsObject();

    virtual void Unknown0();
    virtual int GetObjectType() const = 0;
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreUpdate();
    virtual void PostUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);
    virtual ContactType Contact(PhysicsObject*, dContact*, int, PhysicsObject*);
    virtual void SyncLog(void*, DebugWriteCache*);

    void Reconnect(dSpaceID);
    dSpaceID Disconnect();
    void EnableCollisions();
    void DisableCollisions();
    void ZeroForceAccumulators();
    void SetAngularVelocity(const nlVector3&);
    void SetLinearVelocity(const nlVector3&);
    void GetRotation(nlMatrix4*) const;
    void SetRotation(const nlMatrix4&, CoordinateType = WORLD_COORDINATES);
    void GetPosition(nlVector3*) const;
    void SetPosition(const nlVector3&, CoordinateType);
    void SetDefaultCollideBits();
    void SetDefaultContactInfo(dContact*);
    void SetCategory(unsigned int);
    void SetCollide(unsigned int);
    nlVector3& GetPosition() const;

    /* 0x04 */ dBodyID m_bodyID;
    /* 0x08 */ dGeomID m_geomID;
    /* 0x0C */ PhysicsObject* m_parentObject;
    /* 0x10 */ float m_gravity;
    /* 0x14 */ unsigned char m_unknown[0x24];
}; // size: 0x38

#endif
