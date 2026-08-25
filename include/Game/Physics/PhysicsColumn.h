#ifndef GAME_PHYSICS_PHYSICS_COLUMN_H
#define GAME_PHYSICS_PHYSICS_COLUMN_H

#include "Game/Physics/PhysicsObject.h"

class CollisionSpace;
class PhysicsWorld;

class PhysicsColumn : public PhysicsObject
{
public:
    PhysicsColumn(CollisionSpace*, PhysicsWorld*, float);
    virtual ~PhysicsColumn();

    void GetRadius(float*);
    void SetRadius(float);

    virtual int GetObjectType() const { return 4; }

    /* 0x38 */ dJointID m_jointID;
}; // size: 0x3C

#endif
