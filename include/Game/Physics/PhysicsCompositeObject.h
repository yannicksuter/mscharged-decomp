#ifndef GAME_PHYSICS_PHYSICS_COMPOSITE_OBJECT_H
#define GAME_PHYSICS_PHYSICS_COMPOSITE_OBJECT_H

#include "Game/Physics/PhysicsObject.h"
#include "NL/nlMath.h"

class PhysicsCompositeObject : public PhysicsObject
{
public:
    PhysicsCompositeObject(PhysicsWorld*);
    virtual ~PhysicsCompositeObject();

    virtual void Unknown0();
    virtual int GetObjectType() const { return 9; }

    int AddObject(PhysicsObject*);
    void AdjustTransform(int, nlMatrix4&, bool);

    /* 0x38 */ unsigned char m_Components[8];
    /* 0x40 */ int m_NumComponents;
}; // size: 0x44

#endif
