#ifndef GAME_PHYSICS_PHYSICS_NET_H
#define GAME_PHYSICS_PHYSICS_NET_H

#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsCapsule.h"
#include "Game/Physics/PhysicsFinitePlane.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Net.h"
#include "Game/Render/NetMesh.h"

class PhysicsNet
{
public:
    static PhysicsNet* spPhysNetNegativeX;
    static PhysicsNet* spPhysNetPositiveX;
    static float sfPhysicsNetWidth;
    static float sfPhysicsNetHeight;
    static float sfPhysicsNetDepth;
    static bool sbSweepTestEnabled;
    static float sfWallSoftness;

    PhysicsNet(CollisionSpace* space, bool positive_x);
    virtual ~PhysicsNet();
    virtual int GetObjectType() const { return 25; }

    static bool IsAGoalPost(PhysicsObject*);
    static bool IsAGoalWall(PhysicsObject*);
    static bool IsGoalWallSetA(PhysicsObject*);
    static bool IsGoalWallSetB(PhysicsObject*);
    bool SweepTestForBallContact(const nlVector3& startPos,
        const nlVector3& endPos, const nlVector3& ballVelocity,
        float ballRadius, nlVector3& contactPos,
        nlVector3& contactNormal, PhysicsObject** hitObject) const;
    static void StaticInit(CollisionSpace* pCollisionSpace);
    static void StaticDestroy();

    /* 0x04 */ PhysicsFinitePlane* mpBackWall;
    /* 0x08 */ PhysicsFinitePlane* mpTopWall;
    /* 0x0C */ PhysicsFinitePlane* mpAngledWall;
    /* 0x10 */ PhysicsFinitePlane* mpGoalWallA1;
    /* 0x14 */ PhysicsFinitePlane* mpGoalWallA2;
    /* 0x18 */ PhysicsFinitePlane* mpGoalWallB1;
    /* 0x1C */ PhysicsFinitePlane* mpGoalWallB2;
    /* 0x20 */ PhysicsCapsule* mpSideGoalPost1;
    /* 0x24 */ PhysicsCapsule* mpSideGoalPost2;
    /* 0x28 */ PhysicsCapsule* mpTopGoalPost;
    /* 0x2C */ NetMesh* mpNetMesh;
    /* 0x30 */ cNet* mpNet;
    /* 0x34 */ float errorCorrectionDepth;
}; // size: 0x38

#endif // GAME_PHYSICS_PHYSICS_NET_H
