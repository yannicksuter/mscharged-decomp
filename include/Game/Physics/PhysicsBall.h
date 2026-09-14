#ifndef GAME_PHYSICS_PHYSICS_BALL_H
#define GAME_PHYSICS_PHYSICS_BALL_H

#include "Game/DebugWriteCache.h"
#include "Game/Physics/PhysicsSphere.h"

class PhysicsBall : public PhysicsSphere
{
public:
    PhysicsBall(CollisionSpace* space, PhysicsWorld* world, float radius);

    virtual void Unknown0();
    virtual bool SetContactInfo(
        dContact* contact, PhysicsObject* other, bool param);
    virtual void PreUpdate();
    virtual void PostUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(
        PhysicsObject* other, dContact* contact, int numContacts);
    virtual void RegisterDebugFields(
        unsigned short* type, DebugWriteCache* cache);

    void CalcAngularFromLinearVelocity(nlVector3& v3AngularVel);
    void CalcSurfaceVelocity(nlVector3& v3VelocityOut);
    void fn_8013FE00();
    void fn_8013FE14();
    void fn_80140C30();
    float fn_80140C3C();
    void SetUseAngularVelocity(bool param_1);
    void ScaleAngularVelocity(float scale);
    void AddResistanceForces();
    void CloneBall(const PhysicsBall& other);

    static float GetBallMaxVelocity();

    /* 0x38 */ nlVector3 mv3TiltForce;
    /* 0x44 */ nlVector3 mv3WindForce;
    /* 0x50 */ bool mbUseTiltForce;
    /* 0x51 */ bool mbUseWindForce;
    /* 0x52 */ bool mbIsSupportedByGround;
    /* 0x53 */ bool mbUseAngularVel;
    /* 0x54 */ bool mbUseMagnusEffect;
    /* 0x55 */ bool mbIgnoreForces;
    /* 0x56 */ bool mbCanFreeFall;
    /* 0x57 */ bool mbCanGoThroughGround;
    /* 0x58 */ bool mbPassLockedIn;
    /* 0x59 */ unsigned char mPadding059[3];
    /* 0x5C */ float mfSpinTimer;
    /* 0x60 */ float mfBallAirResistance;
    /* 0x64 */ float mfChargeBonus;
}; // total size: 0x68

inline void PhysicsBall::RegisterDebugFields(
    unsigned short* type, DebugWriteCache* cache)
{
#define REGISTER_FIELD(kind, field) \
    cache->AddField(kind, gDebugFieldTypes[kind].size, \
        (unsigned char*)&field - (unsigned char*)&mv3TiltForce, #field)

    REGISTER_FIELD(22, mv3TiltForce);
    REGISTER_FIELD(22, mv3WindForce);
    REGISTER_FIELD(16, mbUseTiltForce);
    REGISTER_FIELD(16, mbUseWindForce);
    REGISTER_FIELD(16, mbIsSupportedByGround);
    REGISTER_FIELD(16, mbUseAngularVel);
    REGISTER_FIELD(16, mbUseMagnusEffect);
    REGISTER_FIELD(16, mbIgnoreForces);
    REGISTER_FIELD(16, mbCanFreeFall);
    REGISTER_FIELD(16, mbCanGoThroughGround);
    REGISTER_FIELD(16, mbPassLockedIn);
    REGISTER_FIELD(17, mfSpinTimer);
    REGISTER_FIELD(17, mfBallAirResistance);
    REGISTER_FIELD(17, mfChargeBonus);

#undef REGISTER_FIELD
}

#endif // GAME_PHYSICS_PHYSICS_BALL_H
