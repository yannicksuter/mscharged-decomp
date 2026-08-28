#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/Character.h"
#include "NL/nlTimer.h"

class cBall;
class cAIPad;
class cGlobalPad;
class cPN_Feather;
class cPN_SAnimController;
class cPN_SingleAxisBlender;
class cTeam;
class PlayerTweaks;
struct CollisionPlayerPlayerData;
struct CollisionPlayerWallData;
struct UnidentifiedPlayerEventData;

class cPlayer : public cCharacter
{
public:
    virtual ~cPlayer();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate(float dt);
    virtual void PreUpdate(float dt);
    virtual bool CanPickupBall(cBall* pBall, bool bParam);
    virtual void CollideWithBallCallback(cBall* pBall);
    virtual void CollideWithCharacterCallback(
        CollisionPlayerPlayerData* pData);
    virtual void CollideWithWallCallback(
        const CollisionPlayerWallData* pData);
    virtual void InitActionPostWhistle();
    virtual void fn_80099074(UnidentifiedPlayerEventData*);

    void ReleaseBall(int nParam);
    void PickupBall(cBall* pBall);
    void SetNoPickUpTime(float NewNoPickUpTime);
    cGlobalPad* GetGlobalPad();
    int GetBallJointIndex() const
    {
        return m_nBallJointIndex;
    }
    bool IsOnSameTeam(cPlayer* other);
    float DoFlashLight(const nlVector3& Position, unsigned short aDirection,
        float fAngleWeighting, float fIgnoreObjectCloserThanThis,
        float fIgnoreObjectFartherThanThis);
    nlVector3 GetAIOffNetLocation(const nlVector3* v3ReferencePos);
    void fn_8009750C();
    void fn_80097648(float fParam);
    cPN_SingleAxisBlender* CreateSingleAxisBlender(
        const int* pSABAnims, int nNumSABAnims, int nPrimaryAnim,
        void (*fWeightCB)(unsigned int, cPN_SingleAxisBlender*),
        float fWeightSeek, cPN_SAnimController* pSynchingController,
        float fInitialWeight);

public:
    /* 0x1E4 */ int m_ID;

protected:
    /* 0x1E8 */ u8 mUnknown1E8[0x08];
    /* 0x1F0 */ float mUnidentified1F0;
    /* 0x1F4 */ u8 mUnknown1F4;
    /* 0x1F5 */ bool mUnidentified1F5;
    /* 0x1F6 */ bool mUnidentified1F6;
    /* 0x1F7 */ u8 mUnknown1F7;
    /* 0x1F8 */ u8 mUnknown1F8[0x34];

public:
    /* 0x22C */ Timer m_tNoPickupTimer;

protected:
    /* 0x234 */ u8 mUnknown234[0x1C];

public:
    /* 0x250 */ u32 mUnidentified250;
    /* 0x254 */ void* mUnidentified254;
    /* 0x258 */ u8 mUnknown258[0x04];
    /* 0x25C */ int m_eLastPadAction;
    /* 0x260 */ u8 mUnknown260[0x90];
    /* 0x2F0 */ cPN_Feather* mUnidentified2F0;
    /* 0x2F4 */ u8 mUnknown2F4[0x04];

public:
    /* 0x2F8 */ int m_nBallJointIndex;

protected:
    /* 0x2FC */ int m_nRightFootJointIndex;
    /* 0x300 */ int m_nLeftFootJointIndex;

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
