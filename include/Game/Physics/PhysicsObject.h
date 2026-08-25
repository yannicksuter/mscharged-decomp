#ifndef GAME_PHYSICS_PHYSICS_OBJECT_H
#define GAME_PHYSICS_PHYSICS_OBJECT_H

#include "NL/nlMath.h"
#include "ode/collision.h"
#include "ode/contact.h"
#include "ode/objects.h"

class DebugWriteCache;
class PhysicsObject;
class PhysicsWorld;

enum ContactType
{
    NO_CONTACT = 0,
    ONE_WAY_CONTACT_THIS = 1,
    ONE_WAY_CONTACT_OTHER = 2,
    TWO_WAY_CONTACT = 3,
};

class PhysicsContactHandler
{
public:
    virtual ContactType Contact(
        PhysicsObject*, PhysicsObject*, dContact*, int) = 0;
};

class PhysicsObject
{
public:
    static float DefaultGravity;

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

    void CloneObject(const PhysicsObject&);
    void MakeStatic();
    void SetMass(float);
    void Reconnect(dSpaceID);
    dSpaceID Disconnect();
    bool AreCollisionsEnabled();
    void EnableCollisions();
    void DisableCollisions();
    void SetWorldMatrix(const nlMatrix4&);
    void ZeroForceAccumulators();
    void AddForceAtCentreOfMass(const nlVector3&);
    void GetAngularVelocity(nlVector3*) const;
    void SetAngularVelocity(const nlVector3&);
    nlVector3& GetLinearVelocity();
    void GetLinearVelocity(nlVector3*) const;
    void SetLinearVelocity(const nlVector3&);
    void GetRotation(nlMatrix4*) const;
    void SetRotation(const nlMatrix4&, CoordinateType = WORLD_COORDINATES);
    void SetRotation(const nlMatrix3&, CoordinateType = WORLD_COORDINATES);
    void GetPosition(nlVector3*) const;
    void SetPosition(const nlVector3&, CoordinateType);
    void SetDefaultCollideBits();
    void SetDefaultContactInfo(dContact*);
    void SetCategory(unsigned int);
    void SetCollide(unsigned int);
    nlVector3& GetPosition();
    inline void CheckForNaN();

    inline bool IsObjectType(int type) const { return GetObjectType() == type; }

    /* 0x04 */ dBodyID m_bodyID;
    /* 0x08 */ dGeomID m_geomID;
    /* 0x0C */ PhysicsObject* m_parentObject;
    /* 0x10 */ float m_gravity;
    /* 0x14 */ nlVector3 m_position;
    /* 0x20 */ nlVector3 m_linearVelocity;
    /* 0x2C */ PhysicsContactHandler* m_contactHandler;
    /* 0x30 */ void* m_unknown30;
    /* 0x34 */ void* m_unknown34;
}; // size: 0x38

#endif
