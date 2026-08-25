#ifndef GAME_PHYSICS_PHYSICS_FINITE_PLANE_H
#define GAME_PHYSICS_PHYSICS_FINITE_PLANE_H

#include "Game/Physics/PhysicsObject.h"

class CollisionSpace;

class PhysicsFinitePlane : public PhysicsObject
{
public:
    PhysicsFinitePlane(CollisionSpace*, nlVector3&, nlVector3&, nlVector3&, bool, float);
    virtual ~PhysicsFinitePlane() { }

    virtual int GetObjectType() const { return 7; }

    /* 0x38 */ float xMin;
    /* 0x3C */ float xMax;
    /* 0x40 */ float yMin;
    /* 0x44 */ float yMax;
    /* 0x48 */ float mErrorCorrectionDepth;
    /* 0x4C */ struct dxJoint* m_CharMoveJoint;
}; // size: 0x50

#endif
