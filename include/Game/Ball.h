#ifndef GAME_BALL_H
#define GAME_BALL_H

#include "NL/nlMath.h"
#include "NL/nlTimer.h"

enum eSpinType
{
    SPINTYPE_NONE = 0,
    SPINTYPE_BACK = 1,
    SPINTYPE_FORWARD = 2,
    SPINTYPE_ROLLING = 3,
    SPINTYPE_PARAMETER = 4,
};

class cFielder;
class cPlayer;
class PhysicsBall;

class cBall
{
public:
    void SetPosition(const nlVector3& pos);
    void SetVelocity(const nlVector3& velocity, eSpinType spin,
        const nlVector3* pAngularVelocity);
    cFielder* GetOwnerFielder();
    cFielder* GetPassTargetFielder() const;

    /* 0x00 */ u8 m_bBallPathChangeCount;

private:
    /* 0x01 */ u8 mUnidentified001[0x03];

public:
    /* 0x04 */ u32 m_bBallDeflectCount;

public:
    /* 0x08 */ u32 mUnidentified008;

public:
    /* 0x0C */ Timer m_tShotTimer;

private:
    /* 0x14 */ u8 mUnidentified014[0x08];

public:
    /* 0x1C */ Timer m_tNoPickupTimer;
    /* 0x24 */ Timer m_tPassTargetTimer;

private:
    /* 0x2C */ u8 mUnidentified02C[0x28];

public:
    /* 0x54 */ nlVector3 m_v3Position;

private:
    /* 0x60 */ u8 mUnidentified060[0x0C];

public:
    /* 0x6C */ nlVector3 m_v3Velocity;

private:
    /* 0x78 */ u8 mUnidentified078[0x34];

public:
    /* 0xAC */ unsigned int m_uGoalType;

private:
    /* 0xB0 */ u8 mUnidentified0B0[0x18];

public:
    /* 0xC8 */ cPlayer* m_pOwner;

private:
    /* 0xCC */ u8 mUnidentified0CC[0x08];

public:
    /* 0xD4 */ cPlayer* m_pPassTarget;

private:
    /* 0xD8 */ u8 mUnidentified0D8[0x10];

public:
    /* 0xE8 */ PhysicsBall* m_pPhysicsBall;
};

extern cBall* g_pBall;

#endif // GAME_BALL_H
