#ifndef GAME_PHYSICS_PHYSICS_BALL_H
#define GAME_PHYSICS_PHYSICS_BALL_H

#include "Game/Physics/PhysicsSphere.h"
#include "types.h"

class cBall;

class PhysicsBall : public PhysicsSphere
{
public:
    PhysicsBall(float radius);
    static float GetBallMaxVelocity();

public:
    /* 0x38 */ nlVector3 mUnidentified038;
    /* 0x44 */ nlVector3 mUnidentified044;
    /* 0x50 */ bool mUnidentified050;
    /* 0x51 */ bool mUnidentified051;

private:
    /* 0x52 */ u8 mUnidentified052[0x02];

public:
    /* 0x54 */ bool mUnidentified054;

private:
    /* 0x55 */ u8 mUnidentified055[0x0B];

public:
    /* 0x60 */ float mUnidentified060;
    /* 0x64 */ float mUnidentified064;

    /* 0x68 */ cBall* m_pBall;

private:
    /* 0x6C */ u8 mUnidentified06C[0x10];

public:
    /* 0x7C */ bool m_bInsideNet;

private:
    /* 0x7D */ u8 mUnidentified07D[0x03];

public:
    /* 0x80 */ bool m_bCollideWithFielders;
    /* 0x81 */ bool m_bCollideWithGoalies;
};

#endif // GAME_PHYSICS_PHYSICS_BALL_H
