#ifndef GAME_PHYSICS_PHYSICS_AI_BALL_H
#define GAME_PHYSICS_PHYSICS_AI_BALL_H

#include "Game/Physics/PhysicsBall.h"

class cBall;
class DebugWriteCache;

class PhysicsAIBall : public PhysicsBall
{
public:
    PhysicsAIBall(float radius);

    virtual void Unknown0();
    virtual int GetObjectType() const { return 0x10; }
    virtual void PreUpdate();
    virtual void PostUpdate();
    virtual ContactType Contact(
        PhysicsObject* obj, dContact* info, int numContacts);
    virtual void SyncLog(void* context, DebugWriteCache* cache);
    virtual void RegisterDebugFields(
        unsigned short* type, DebugWriteCache* cache);

    static bool IsBallOutsideNet(const nlVector3& ballPosition);
    static bool DidBallJustEnterNet(
        const nlVector3& oldPosition, nlVector3 newPosition);

    void CheckIfBallWentThroughGoalPost();
    void CheckIfBallWentThroughGoalie();

    /* 0x68 */ cBall* m_pAIBall;
    /* 0x6C */ nlVector3 m_prevPosition;
    /* 0x78 */ unsigned int m_goalieContactFramesAgo;
    /* 0x7C */ bool mbIsInsideNet;
    /* 0x7D */ bool mbGoalPlaneContact;
    /* 0x7E */ bool mbBallSpeedBelowSweepTestThreshold;
    /* 0x7F */ bool mbHasHitPlayer;
    /* 0x80 */ bool mbCanCollidePlayer;
    /* 0x81 */ bool mbCanCollideGoalie;
    /* 0x82 */ unsigned char mPadding082[2];
}; // total size: 0x84

#endif // GAME_PHYSICS_PHYSICS_AI_BALL_H
