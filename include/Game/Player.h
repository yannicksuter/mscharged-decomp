#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/Character.h"
#include "NL/nlTimer.h"
#include "Game/DetInput.h"

class cBall;
class cAIPad;
class cFielder;
class cPN_Feather;
class cPN_SAnimController;
class cPN_SingleAxisBlender;
class cTeam;
class PlayerTweaks;
class cSHierarchy;
class AnimRetargetList;
class CharacterPhysicsData;
class SpaceSearch;
class AvoidableObject;
struct CollisionPlayerPlayerData;
struct CollisionPlayerWallData;
struct UnidentifiedEventData24;

enum eBallRotationMode
{
    BRM_ANIMATED = 0,
    BRM_MATCH_VELOCITY = 1,
};

enum ePadActions
{
    PAD_SWITCH = 26,
};

struct UnidentifiedPlayerState_1E4
{

    UnidentifiedPlayerState_1E4() { UnidentifiedReset(); }
    void UnidentifiedReset()
    {
        m_nFeatherAnimID = 0;
        m_bIsContactingWall = false;
        m_fSkipTimer = 0.0f;
        m_bSkipActionUpdate = false;
        m_bSkipAnimUpdate = false;
        m_bForceFeatherUpdate = false;
        m_eBallRotationMode = BRM_MATCH_VELOCITY;
        m_ResetBaseBallOrientation = false;
        m_fShotStrengthTime = 0.0f;
        m_bCanTestController = true;
        m_eLastPadAction = 50;
        m_aSwapFacingDirection = 0;
        m_UserControlledTime = 0.0f;
        nlVec3Set(m_v3AIPosition, 0.0f, 0.0f, 0.0f);
        m_BaseBallOrientation.x = m_BaseBallOrientation.y = m_BaseBallOrientation.z = 0.0f;
        m_BaseBallOrientation.w = 1.0f;
        m_tBallPossessionTimer.UnidentifiedClear();
        m_tBallUnPossessionTimer.UnidentifiedClear();
        m_tNoPickupTimer.UnidentifiedClear();
        m_tSlideAttackTimer.UnidentifiedClear();
        m_tLooseBallPassTimer.UnidentifiedClear();
        m_tInactivityTimer.UnidentifiedClear();
        m_tFireTimer.UnidentifiedClear();
        m_tSwapFacingTimer.UnidentifiedClear();
    }
    /* 0x00 */ int m_ID;

    /* 0x04 */ int m_nFeatherAnimID;
    /* 0x08 */ bool m_bIsContactingWall;
    /* 0x09 */ u8 mPadding1ED[0x03];
    /* 0x0C */ float m_fSkipTimer;
    /* 0x10 */ bool m_bSkipActionUpdate;
    /* 0x11 */ bool m_bSkipAnimUpdate;
    /* 0x12 */ bool m_bForceFeatherUpdate;
    /* 0x13 */ u8 mPadding1F7;

    /* 0x14 */ nlVector3 m_v3AIPosition;

    /* 0x20 */ eBallRotationMode m_eBallRotationMode;
    /* 0x24 */ bool m_ResetBaseBallOrientation;
    /* 0x25 */ u8 mPadding209[0x03];
    /* 0x28 */ nlQuaternion m_BaseBallOrientation;

    /* 0x38 */ Timer m_tBallPossessionTimer;
    /* 0x40 */ Timer m_tBallUnPossessionTimer;
    /* 0x48 */ Timer m_tNoPickupTimer;
    /* 0x50 */ float m_fShotStrengthTime;
    /* 0x54 */ Timer m_tSlideAttackTimer;
    /* 0x5C */ Timer m_tLooseBallPassTimer;
    /* 0x64 */ Timer m_tInactivityTimer;
    /* 0x6C */ Timer m_tFireTimer;
    /* 0x74 */ bool m_bCanTestController;
    /* 0x75 */ u8 mUnknown259[0x03];
    /* 0x78 */ int m_eLastPadAction;
    /* 0x7C */ u16 m_aSwapFacingDirection;
    /* 0x7E */ u8 mPadding262[0x02];
    /* 0x80 */ Timer m_tSwapFacingTimer;
    /* 0x88 */ float m_UserControlledTime;
};

class cPlayer : public cCharacter
{
public:
    cPlayer(int nPlayerID, eCharacterClass characterClass,
            const int* nModelID, cSHierarchy* hierarchy,
            cAnimInventory* animInventory, const CharacterPhysicsData* physData,
            float fPhysCapsuleHeight, float fPhysCapsuleRadius,
            AnimRetargetList* animRetargetList, int nIndex, eClassTypes classType);
    virtual ~cPlayer();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate();
    virtual void Unknown7(float dt);
    virtual void PreUpdate(float dt);
    virtual void UnidentifiedVirtual1C();
    virtual void SetAnimID(int animID);
    virtual void Update(float fDeltaT);
    virtual void Unknown10(
        const nlVector3& v3Position, unsigned short aDirection);
    virtual void Unknown11(void* context, DebugWriteCache* cache);
    virtual void Unknown12(RunningChecksum* pChecksum);
    virtual bool CanPickupBall(cBall* pBall, bool bParam);
    virtual void CollideWithBallCallback(cBall* pBall);
    virtual void CollideWithCharacterCallback(
        CollisionPlayerPlayerData* pData);
    virtual void CollideWithWallCallback(
        const CollisionPlayerWallData* pData);
    virtual void InitActionPostWhistle();
    virtual void fn_80099074(const UnidentifiedEventData24*);

    void ClearSwapControllerTimer()
    {
        for (int i = 0; i < 16; i++)
            m_tSwapControllerTimer[i].UnidentifiedClear();
    }
    void ReleaseBall(int nParam);
    void DoRegularPassing(cPlayer* pTeammate, bool bVolleyPass,
        bool bFindPosition, bool bPerfectPass, bool bParam4,
        float fMinPassSpeed, float fMaxPassSpeed);
    void fn_80095DF4(float fDeltaT);
    void GetAnimatedBallOrientation(nlQuaternion& qRetval);
    bool UnidentifiedPowerupPredicate();
    bool UnidentifiedIsPressed(int button);
    void PickupBall(cBall* pBall);
    void SetNoPickUpTime(float NewNoPickUpTime);
    DetInput* GetGlobalPad();
    bool HasBall() const { return m_pBall != 0; }
    u8 SwapController(bool bParam);
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
    cTeam* GetTeam() const
    {
        return m_pTeam;
    }
    float DoFlashLight(const nlVector3& Position, unsigned short aDirection,
                       float fAngleWeighting, float fIgnoreObjectCloserThanThis,
                       float fIgnoreObjectFartherThanThis);
    nlVector3 GetAIDefNetLocation(const nlVector3* v3ReferencePos);
    nlVector3 GetAIOffNetLocation(const nlVector3* v3ReferencePos);
    void fn_800974B0();
    void fn_8009750C();
    void fn_80097648(float fParam);
    bool fn_800976C4() const;
    void SetPowerupAnimState(int nodeIndex, int animID, float blendTime);
    void SetPowerupAnimState(int animID);
    static void PlayerHeadTrackCallback(unsigned int nSelf, unsigned int nParam2,
                                        cPoseAccumulator* pPoseAccumulator, unsigned int nJointIndex, int nParentIndex);
    bool IsCharacterInAir(float fParam) const;
    cPN_SingleAxisBlender* CreateSingleAxisBlender(
        const int* pSABAnims, int nNumSABAnims, int nPrimaryAnim,
        void (*fWeightCB)(unsigned int, cPN_SingleAxisBlender*),
        float fWeightSeek, cPN_SAnimController* pSynchingController,
        float fInitialWeight);

public:
    /* 0x1E4 */ UnidentifiedPlayerState_1E4 mUnidentified1E4;
    /* 0x270 */ Timer m_tSwapControllerTimer[16];

protected:
    /* 0x2F0 */ cPN_Feather* m_pPowerupLayer;
    /* 0x2F4 */ cPN_Feather* m_pReceivePassLayer;

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
    /* 0x320 */ AvoidableObject* mUnidentified320;

}; // total size: 0x324

#endif // GAME_PLAYER_H
