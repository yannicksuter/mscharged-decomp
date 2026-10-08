#ifndef GAME_PHYSICS_PHYSICS_OBJECT_H
#define GAME_PHYSICS_PHYSICS_OBJECT_H

#include "NL/nlMath.h"
#include "ode/collision.h"
#include "ode/contact.h"
#include "ode/objects.h"

class DebugWriteCache;
class PhysicsObject;
class PhysicsWorld;

enum ePhysicsPrimitiveType
{
    PHYS_PRIMITIVE_BOX = 0,
    PHYS_PRIMITIVE_SPHERE = 1,
    PHYS_PRIMITIVE_CAPSULE = 2,
    PHYS_PRIMITIVE_CYLINDER = 3,
    PHYS_PRIMITIVE_FINITE_PLANE = 4,
    PHYS_PRIMITIVE_PLANE = 6,
};

enum ePhysicsObjectType
{
    PHYSOBJ_BOX = 0x01,
    PHYSOBJ_CAPSULE = 0x02,
    PHYSOBJ_CYLINDER = 0x03,
    PHYSOBJ_COLUMN = 0x04,
    PHYSOBJ_ROUNDED_CORNER = 0x05,
    PHYSOBJ_PLANE = 0x06,
    PHYSOBJ_FINITEPLANE = 0x07,
    PHYSOBJ_CHARACTER = 0x08,
    PHYSOBJ_COMPOSITE = 0x09,
    PHYSOBJ_SPHERE = 0x0A,
    PHYSOBJ_SPHERE_BONE = 0x0D,
    PHYSOBJ_CAPSULE_BONE = 0x0E,
    PHYSOBJ_CYLINDER_BONE = 0x0F,
    PHYSOBJ_AI_BALL = 0x10,
    PHYSOBJ_FAKE_BALL = 0x11,
    PHYSOBJ_GROUND_PLANE = 0x12,
    PHYSOBJ_TRIGGER_VOLUME = 0x13,
    PHYSOBJ_SHELL = 0x14,
    PHYSOBJ_BANANA = 0x15,
    PHYSOBJ_GOALIE_PLANE = 0x16,
    PHYSOBJ_WALL = 0x17,
    PHYSOBJ_NPC = 0x18,
    PHYSOBJ_NET = 0x19,
    PHYSOBJ_PATCH = 0x1C,
    PHYSOBJ_WALUIGI_WALL = 0x1D,
    PHYSOBJ_BULLET_BILL = 0x1E,
    PHYSOBJ_HAMMER = 0x1F,
    PHYSOBJ_YOSHI_EGG = 0x20,
    PHYSOBJ_BIRDO_EGG = 0x21,
    PHYSOBJ_KOOPA_SHELL = 0x22,
    PHYSOBJ_SHOCKWAVE = 0x23,
    PHYSOBJ_THWOMP = 0x24,
};

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
    virtual void PreCollide() { }
    virtual ContactType Contact(PhysicsObject*, dContact*, int);
    virtual ContactType Contact(PhysicsObject*, dContact*, int, PhysicsObject*);
    virtual void SyncLog(void*, DebugWriteCache*) { }

    void CloneObject(const PhysicsObject&);
    void MakeStatic();
    void SetMass(float);
    float GetGravity() const { return m_gravity; }
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
