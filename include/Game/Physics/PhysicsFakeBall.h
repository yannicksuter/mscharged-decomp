#ifndef _PHYSICSFAKEBALL_H_
#define _PHYSICSFAKEBALL_H_

#include "Game/Ball.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsGroundPlane.h"
#include "Game/Physics/PhysicsWall.h"
#include "Game/Physics/PhysicsWorld.h"
#include "Game/Physics/CollisionSpace.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlSlotPool.h"

class FakePhysicsBall;
class PhysicsGoaliePlane;

struct BallCacheInfo
{
    /* 0x00 */ float mfTime;
    /* 0x04 */ nlVector3 mv3Position;
    /* 0x10 */ nlVector3 mv3LinearVelocity;

    static SlotPool<BallCacheInfo> mBallCacheInfoSlotPool;
}; // total size: 0x1C

class FakeBallWorld
{
public:
    FakeBallWorld(cBall* pBall);
    ~FakeBallWorld();

    cBall* GetBall() const
    {
        return mpBall;
    }

    static BallCacheInfo* AddCacheEntry(
        float fTime, PhysicsBall* pPhysicsBall);
    static void GetNextBallPosVel(
        nlVector3& v3BallPos, nlVector3& v3BallVel);
    static bool FindBallIntercept(const nlVector3& v3PlayerPos,
        float fPlayerReach, float fPlayerSpeed, nlVector3& v3InterceptPos,
        nlVector3& v3InterceptVel, float& fInterceptTime,
        float& fClosestDist, float fMaxTime);
    static void GetNextBallPosition(nlVector3& v3BallPos);
    static void ResetBallIterator();
    static float GetPredictedPosAtDistance(float fDistance,
        nlVector3& v3Position, nlVector3& v3Velocity, bool bFreeBallOnly);
    static float GetPredictedHeightLimitTime(float fHeight, float fMinTime,
        nlVector3& v3ContactPoint, nlVector3& v3ContactVelocity,
        float& fTargetHeight, bool bDownOnly);
    static float GetPredictedPlaneIntersectTime(const nlVector4& v4Plane,
        nlVector3& v3ContactPoint, nlVector3& v3ContactVelocity);
    static void ClearBallCache();
    static bool GetPredictedBallPosition(float fDeltaTime,
        nlVector3& v3Position, nlVector3& v3Velocity);
    static void InvalidateBallCache();
    static void Destroy();
    static void Init(cBall* pBall);

    /* 0x00 */ cBall* mpBall;
    /* 0x04 */ FakePhysicsBall* mpPhysicsBall;
    /* 0x08 */ CollisionSpace* mpCollisionSpace;
    /* 0x0C */ PhysicsWorld* mpPhysicsWorld;
    /* 0x10 */ PhysicsGroundPlane* mpGroundPlane;
    /* 0x14 */ PhysicsGoaliePlane* mpGoaliePlane1;
    /* 0x18 */ PhysicsGoaliePlane* mpGoaliePlane2;
    /* 0x1C */ bool mbHitSuccess;
    /* 0x1D */ bool mUnidentified1D;
    /* 0x1E */ unsigned char mPadding01E[2];
    /* 0x20 */ dContactGeom mContactInfo;

    static nlDLListIterator<BallCacheInfo*>* mpCacheIterator;
    static nlDLListSlotPool<BallCacheInfo*> mBallCacheList;
    static float mfLastCacheTime;
    static FakeBallWorld* mpPredictWorld;
}; // total size: 0x4C

class PhysicsGoaliePlane : public PhysicsWall
{
public:
    PhysicsGoaliePlane(
        float a, float b, float c, FakeBallWorld& fakeBallWorld);
    virtual int GetObjectType() const { return 0x16; }

    /* 0x38 */ FakeBallWorld& mWorld;
}; // total size: 0x3C

class FakePhysicsBall : public PhysicsBall
{
public:
    FakePhysicsBall(float radius, FakeBallWorld& fakeBallWorld);
    virtual int GetObjectType() const { return 0x11; }
    virtual ContactType Contact(
        PhysicsObject* object, dContact* contact, int numContacts);

    /* 0x68 */ FakeBallWorld& mWorld;
}; // total size: 0x6C

extern "C" void fn_8016EEC8();
extern "C" void fn_8016F06C();

#endif // _PHYSICSFAKEBALL_H_
