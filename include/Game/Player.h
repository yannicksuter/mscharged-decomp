#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/Character.h"
#include "NL/nlTimer.h"

class cBall;
class cAIPad;
class cGlobalPad;
class cTeam;
class PlayerTweaks;

class cPlayer : public cCharacter
{
public:
    virtual ~cPlayer();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate(float dt);
    virtual void PreUpdate(float dt);

    void ReleaseBall(bool bParam);
    void PickupBall(cBall* pBall);
    cGlobalPad* GetGlobalPad();
    bool IsOnSameTeam(cPlayer* other);
    void fn_8009750C();
    void fn_80097648(float fParam);

protected:
    /* 0x1E4 */ u8 mUnknown1E4[0x48];

public:
    /* 0x22C */ Timer m_tNoPickupTimer;

protected:
    /* 0x234 */ u8 mUnknown234[0x1C];
    /* 0x250 */ u32 mUnidentified250;
    /* 0x254 */ void* mUnidentified254;
    /* 0x258 */ u8 mUnknown258[0xA0];

public:
    /* 0x2F8 */ int m_nBallJointIndex;

protected:
    /* 0x2FC */ u8 mUnknown2FC[0x08];

public:
    /* 0x304 */ int m_nLeftHandJointIndex;
    /* 0x308 */ int m_nRightHandJointIndex;

public:
    /* 0x30C */ cAIPad* m_pController;

    /* 0x310 */ cBall* m_pBall;
    /* 0x314 */ cTeam* m_pTeam;
    /* 0x318 */ u32 mUnidentified318;

protected:
    /* 0x31C */ u8 mUnknown31C[0x08];

public:
    /* 0x324 */ PlayerTweaks* m_pTweaks;
}; // total size: 0x328

#endif // GAME_PLAYER_H
