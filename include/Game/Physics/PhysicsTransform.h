#ifndef GAME_PHYSICS_PHYSICS_TRANSFORM_H
#define GAME_PHYSICS_PHYSICS_TRANSFORM_H

#include "Game/Physics/PhysicsObject.h"
#include "NL/nlMath.h"

class PhysicsTransform : public PhysicsObject
{
public:
    PhysicsTransform();
    virtual ~PhysicsTransform();

    virtual void Unknown0();
    virtual int GetObjectType() const { return m_SubObject->GetObjectType(); }

    void Attach(PhysicsObject*, PhysicsObject*);
    void Release();
    void SetSubObjectPosition(const nlVector3&, PhysicsObject::CoordinateType);
    void SetSubObjectTransform(const nlMatrix4&, PhysicsObject::CoordinateType);

    /* 0x38 */ PhysicsObject* m_SubObject;
}; // size: 0x3C

#endif
