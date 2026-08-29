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
class DrawableObject;
class PhysicsBall;

class cBall
{
public:
    void SetPosition(const nlVector3& pos);
    void SetVelocity(const nlVector3& velocity, eSpinType spin,
        const nlVector3* pAngularVelocity);
    nlVector3* GetAIVelocity() const;
    nlVector3* GetDrawablePosition() const;
    float fn_80014F38(float fScale) const;
    cFielder* GetOwnerFielder();
    cPlayer* GetOwnerGoalie();
    cFielder* GetPassTargetFielder() const;
    bool GetInNet(int& nSide);

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
    /* 0x2C */ u8 mUnidentified02C[0x1E];

public:
    /* 0x4A */ bool mUnidentified4A;

private:
    /* 0x4B */ u8 mUnidentified04B[0x09];

public:
    /* 0x54 */ nlVector3 m_v3Position;
    /* 0x60 */ nlVector3 m_v3PrevPosition;

public:
    /* 0x6C */ nlVector3 m_v3Velocity;

public:
    /* 0x78 */ nlVector3 m_v3PassIntercept;

private:
    /* 0x84 */ u8 mUnidentified084[0x10];

public:
    /* 0x94 */ nlVector3 m_v3ShotTarget;

private:
    /* 0xA0 */ u8 mUnidentified0A0[0x0C];

public:
    /* 0xAC */ unsigned int m_uGoalType;

private:
    /* 0xB0 */ u8 mUnidentified0B0[0x08];

public:
    /* 0xB8 */ int m_unk_0xA4;

private:
    /* 0xBC */ u8 mUnidentified0BC[0x0C];

public:
    /* 0xC8 */ cPlayer* m_pOwner;

public:
    /* 0xCC */ cPlayer* m_pPrevOwner;

private:
    /* 0xD0 */ cPlayer* m_pLastTouch;

public:
    /* 0xD4 */ cPlayer* m_pPassTarget;
    /* 0xD8 */ cPlayer* m_pShooter;

private:
    /* 0xDC */ u8 mUnidentified0DC[0x08];

public:
    /* 0xE4 */ DrawableObject* m_pDrawableBall;

public:
    /* 0xE8 */ PhysicsBall* m_pPhysicsBall;
    /* 0xEC */ void* mUnidentifiedEC;
};

extern cBall* g_pBall;

#endif // GAME_BALL_H
