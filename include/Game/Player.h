#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/Character.h"
#include "NL/nlTimer.h"

class cBall;
class cAIPad;
class cFielder;
class cGlobalPad;
class cPN_Feather;
class cPN_SAnimController;
class cPN_SingleAxisBlender;
class cTeam;
class PlayerTweaks;
class SpaceSearch;
class UnidentifiedPlayerRadius;
struct CollisionPlayerPlayerData;
struct CollisionPlayerWallData;
struct UnidentifiedPlayerEventData;

enum eBallRotationMode
{
    BRM_ANIMATED = 0,
    BRM_MATCH_VELOCITY = 1,
};

class cPlayer : public cCharacter
{
public:
    virtual ~cPlayer();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate();
    virtual void PreUpdate(float dt);
    virtual void UnidentifiedVirtual1C();
    virtual void SetAnimID(int animID);
    virtual void Update(float fDeltaT);
    virtual bool CanPickupBall(cBall* pBall, bool bParam);
    virtual void CollideWithBallCallback(cBall* pBall);
    virtual void CollideWithCharacterCallback(
        CollisionPlayerPlayerData* pData);
    virtual void CollideWithWallCallback(
        const CollisionPlayerWallData* pData);
    virtual void InitActionPostWhistle();
    virtual void fn_80099074(UnidentifiedPlayerEventData*);

    void ReleaseBall(int nParam);
    void GetAnimatedBallOrientation(nlQuaternion& qRetval);
    void PickupBall(cBall* pBall);
    void SetNoPickUpTime(float NewNoPickUpTime);
    cGlobalPad* GetGlobalPad();
    void* fn_800972CC();
    void SetAIPad(cAIPad* pPad);
    void fn_80096CDC(cBall* pBall);
    void PlayAttackReactionSounds(float fScale);
    void ClearPowerupAnimState(bool bIsEndGame);
    cPlayer* fn_8009670C(nlVector3* pPosition, bool bParam);
    cFielder* GetClosestOpponentFielder(
        nlVector3* pPosition, bool bParam);
    cPlayer* fn_800966AC(nlVector3* pPosition, bool bParam);
    void SetSpaceSearch(SpaceSearch* pSpaceSearch);
    int GetBallJointIndex() const
    {
        return m_nBallJointIndex;
    }
    bool IsOnSameTeam(cPlayer* other);
    float DoFlashLight(const nlVector3& Position, unsigned short aDirection,
        float fAngleWeighting, float fIgnoreObjectCloserThanThis,
        float fIgnoreObjectFartherThanThis);
    nlVector3 GetAIDefNetLocation(const nlVector3* v3ReferencePos);
    nlVector3 GetAIOffNetLocation(const nlVector3* v3ReferencePos);
    void fn_800974B0();
    void fn_8009750C();
    void fn_80097648(float fParam);
    bool fn_800976C4();
    cPN_SingleAxisBlender* CreateSingleAxisBlender(
        const int* pSABAnims, int nNumSABAnims, int nPrimaryAnim,
        void (*fWeightCB)(unsigned int, cPN_SingleAxisBlender*),
        float fWeightSeek, cPN_SAnimController* pSynchingController,
        float fInitialWeight);

public:
    /* 0x1E4 */ int m_ID;

protected:
    /* 0x1E8 */ int m_nFeatherAnimID;
    /* 0x1EC */ bool m_bIsContactingWall;
    /* 0x1ED */ u8 mPadding1ED[0x03];
    /* 0x1F0 */ float m_fSkipTimer;
    /* 0x1F4 */ bool m_bSkipActionUpdate;
    /* 0x1F5 */ bool m_bSkipAnimUpdate;
    /* 0x1F6 */ bool m_bForceFeatherUpdate;
    /* 0x1F7 */ u8 mPadding1F7;

public:
    /* 0x1F8 */ nlVector3 m_v3AIPosition;

public:
    /* 0x204 */ eBallRotationMode m_eBallRotationMode;
    /* 0x208 */ bool m_ResetBaseBallOrientation;
    /* 0x209 */ u8 mPadding209[0x03];
    /* 0x20C */ nlQuaternion m_BaseBallOrientation;

public:
    /* 0x21C */ Timer m_tBallPossessionTimer;
    /* 0x224 */ Timer m_tBallUnPossessionTimer;
    /* 0x22C */ Timer m_tNoPickupTimer;
    /* 0x234 */ float m_fShotStrengthTime;
    /* 0x238 */ Timer m_tSlideAttackTimer;
    /* 0x240 */ Timer m_tLooseBallPassTimer;
    /* 0x248 */ Timer m_tInactivityTimer;
    /* 0x250 */ Timer m_tFireTimer;
    /* 0x258 */ bool m_bCanTestController;
    /* 0x259 */ u8 mUnknown259[0x03];
    /* 0x25C */ int m_eLastPadAction;
    /* 0x260 */ u16 m_aSwapFacingDirection;
    /* 0x262 */ u8 mPadding262[0x02];
    /* 0x264 */ Timer m_tSwapFacingTimer;
    /* 0x26C */ float m_UserControlledTime;
    /* 0x270 */ Timer m_tSwapControllerTimer[16];

protected:
    /* 0x2F0 */ cPN_Feather* m_pPowerupLayer;
    /* 0x2F4 */ u8 mUnknown2F4[0x04];

public:
    /* 0x2F8 */ int m_nBallJointIndex;

public:
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

public:
    /* 0x31C */ SpaceSearch* m_pSpaceSearch;
    /* 0x320 */ UnidentifiedPlayerRadius* mUnidentified320;

public:
    /* 0x324 */ PlayerTweaks* m_pTweaks;
}; // total size: 0x328

#endif // GAME_PLAYER_H
