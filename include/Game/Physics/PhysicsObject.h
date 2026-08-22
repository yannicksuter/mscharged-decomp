#ifndef GAME_PHYSICS_PHYSICS_OBJECT_H
#define GAME_PHYSICS_PHYSICS_OBJECT_H

#include "ode/collision.h"
#include "ode/contact.h"
#include "ode/objects.h"

class DebugWriteCache;
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

    void SetDefaultCollideBits();

    /* 0x04 */ dBodyID m_bodyID;
    /* 0x08 */ dGeomID m_geomID;
    /* 0x0C */ PhysicsObject* m_parentObject;
    /* 0x10 */ float m_gravity;
    /* 0x14 */ unsigned char m_unknown[0x24];
}; // size: 0x38

#endif
