#ifndef GAME_AI_FIELDER_H
#define GAME_AI_FIELDER_H

#include "Game/Player.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/FielderAbility.h"

enum eTurboRequest
{
    TR_FAR_DISTANCE = 0,
    TR_MOVING_TARGET = 1,
    TR_FORCED_ON = 2,
    TR_FORCED_OFF = 3,
};

enum eFielderActionState
{
    ACTION_NEED_ACTION = -1,
    ACTION_ELECTROCUTION = 0x02,
    ACTION_HIT = 0x04,
    ACTION_HIT_REACT = 0x06,
    ACTION_IDLE_TURN = 0x07,
    ACTION_LATE_ONETIMER_FROM_VOLLEY = 0x08,
    ACTION_LOOSE_BALL_PASS = 0x09,
    ACTION_LOOSE_BALL_SHOT = 0x0A,
    ACTION_ONETIMER = 0x0D,
    ACTION_ONETOUCH_PASS_FROM_VOLLEY = 0x0E,
    ACTION_PASS = 0x0F,
    ACTION_SHOT = 0x0B,
    ACTION_SHOOT_TO_SCORE = 0x0C,
    ACTION_POST_WHISTLE = 0x10,
    ACTION_RECEIVE_PASS = 0x11,
    ACTION_RUNNING = 0x12,
    ACTION_RUNNING_WB = 0x14,
    ACTION_UNKNOWN_15 = 0x15,
    ACTION_SLIDE_ATTACK = 0x16,
    ACTION_SLIDE_ATTACK_REACT = 0x17,
    ACTION_BOMB_REACT = 0x19,
    ACTION_SHELL_REACT = 0x1A,
    ACTION_BANANA_REACT = 0x1B,
    ACTION_UNKNOWN_30 = 0x1E,
    ACTION_UNKNOWN_31 = 0x1F,
    ACTION_UNKNOWN_32 = 0x20,
    ACTION_UNKNOWN_34 = 0x22,
    ACTION_WAIT = 0x24,
};

enum eFielderDesireState
{
    FIELDERDESIRE_FINISH_ACTION = 0x15,
};

enum eStrafeDirection
{
    STRAFE_IDLE = 0,
    STRAFE_RIGHT = 1,
    STRAFE_LEFT = 2,
    STRAFE_FORWARD = 3,
    STRAFE_BACK = 4,
};

enum eRole
{
    ROLE_STRIKER = 0,
    ROLE_WINGER = 1,
    ROLE_MIDFIELD = 2,
    ROLE_DEFENCE = 3,
    NUM_ROLES = 4,
};

struct LooseBallContactAnimInfo
{
    /* 0x0 */ s32 nAnimID;
    /* 0x4 */ float fAnimContactFrame;
    /* 0x8 */ u16 aIncomingAngleMin;
    /* 0xA */ u16 aIncomingAngleMax;
}; // total size: 0xC

class cFielder;
class ChainChomp;
class UnidentifiedNPC_801B43F8;
struct CollisionThwompPlayerData;
extern "C" bool fn_800344DC(cFielder*, const nlVector3*);
class cSHierarchy;
class AnimRetargetList;
class CharacterPhysicsData;
class WaluigiWallManager;
void fn_80048870(cFielder* pFielder);
const LooseBallContactAnimInfo* GetOneTimerIdleGroundContactAnims();
int GetNumOneTimerIdleGroundContactAnims();
const LooseBallContactAnimInfo* GetOneTimerIdleVolleyContactAnims();
int GetNumOneTimerIdleVolleyContactAnims();
const LooseBallContactAnimInfo* GetOneTimerLeadGroundContactAnims();
int GetNumOneTimerLeadGroundContactAnims();

struct UnidentifiedFielderPair330
{
    UnidentifiedFielderPair330(bool bParam, float fParam)
        : mUnidentified00(bParam)
        , mUnidentified04(fParam)
    {
    }
    /* 0x0 */ bool mUnidentified00;
    /* 0x4 */ float mUnidentified04;
}; // total size: 0x8

struct UnidentifiedFielderPair374
{
    UnidentifiedFielderPair374()
        : mUnidentified00(0)
        , mUnidentified04(0.0f)
    {
    }
    /* 0x0 */ int mUnidentified00;
    /* 0x4 */ float mUnidentified04;
}; // total size: 0x8

struct UnidentifiedFielderAction364
{
    UnidentifiedFielderAction364()
        : passTarget(0)
    {
    }

    cFielder* passTarget;
};

struct UnidentifiedFielderAction37C
{
    UnidentifiedFielderAction37C()
        : eLastStrafeDirection(STRAFE_IDLE)
        , bFirstCycleOfTurbo(false)
    {
    }

    eStrafeDirection eLastStrafeDirection;
    bool bFirstCycleOfTurbo;
};

struct UnidentifiedFielderAction384
{
    UnidentifiedFielderAction384()
    {
        bWaitForAnimToFinish = false;
        bCuePitch = false;
    }

    bool bCuePitch;
    bool bWaitForAnimToFinish;
};

struct UnidentifiedFielderAction410
{
    UnidentifiedFielderAction410()
    {
        nlVec3Set(mUnidentified00, 0.0f, 0.0f, 0.0f);
        mUnidentified0C = false;
    }

    nlVector3 mUnidentified00;
    bool mUnidentified0C;
};

class FuzzyVariant;
class DesireSteering;
class DesireUserControlled;
class UnidentifiedDesire35;
class cFielder;
class DesireSuperPower;
extern "C" void fn_800C5DBC(DesireSteering*, float);
extern "C" void fn_800C6FDC(DesireSteering*, float);
extern "C" bool fn_800D1C34(const cFielder*);
extern "C" bool fn_800D0DB0(DesireSuperPower*, void*);
class PhysicsObject;
class ShotMeter;
class UnidentifiedFielderInput;
class UnidentifiedFuzzyRuntimeBase;
struct BulletBillObject;

class cFielder : public cPlayer
{
    friend void fn_800C5DBC(DesireSteering*, float);
    friend void fn_800C6FDC(DesireSteering*, float);
    friend class UnidentifiedDesire35;
    friend class DesireUserControlled;
    friend class DesireSuperPower;
    friend bool fn_800D1C34(const cFielder*);
    friend bool fn_800D0DB0(class DesireSuperPower*, void*);

public:
    PlayerTweaks* GetTweaks() const;
    UnidentifiedFuzzyRuntimeBase* fn_8002E198() const;
    float GetSpeedPowerupAdjusted(float fSpeed);

    unsigned int IsFrozen();
    unsigned int IsShattered();

    cFielder(int nPlayerID, int nTeamID, eCharacterClass cc,
        const int* nModelID, cSHierarchy* pHierarchy,
        cAnimInventory* pAnimInventory,
        const CharacterPhysicsData* pCharacterPhysicsData, PlayerTweaks* pCharTweaks,
        PlayerTweaks* pUnidentifiedTweaks,
        AnimRetargetList* pAnimRetargetList, int nIndex);
    virtual ~cFielder();
    virtual void PrePhysicsUpdate();
    virtual void PreUpdate(float fTime);
    virtual void UnidentifiedVirtual1C();
    virtual void ResetEffects();
    virtual void Unknown8(unsigned short aParam, bool bParam);
    virtual void SetPosition(const nlVector3& position);
    virtual void Update(float fDeltaT);
    virtual void Unknown10(
        const nlVector3& v3Position, unsigned short aDirection);
    virtual void Unknown11(void* context, DebugWriteCache* cache);
    virtual void Unknown12(RunningChecksum* pChecksum);
    virtual bool CanPickupBall(cBall* pBall, bool bParam);
    virtual void CollideWithCharacterCallback(
        CollisionPlayerPlayerData* pData);
    virtual void CollideWithWallCallback(
        const CollisionPlayerWallData* pData);
    virtual void InitActionPostWhistle();
    virtual void fn_80099074(const UnidentifiedEventData24*);

    void ClearPassTargetIfAmThePassTarget();
    void CleanUpAction(eFielderActionState actionState);
    ePowerUpType GetPowerupType() const;
    void UseTeamPowerup(cFielder* pTarget);
    void UpdateActionState(float dt);
    void UpdateHeadTracking(float fDeltaT);
    cFielder* DoFindBestHitTarget();
    void UpdateController(float fDeltaT);
    bool IsRunning() const;
    bool fn_800345EC(cFielder* pOtherFielder) const;
    bool fn_80034894(cFielder* pOtherFielder) const;
    bool fn_800344B0() const;
    bool IsRunningWithBall() const;
    void StartRunning();
    bool CanGetElectrocuted(
        const CollisionPlayerWallData* eventData);
    eFielderActionState GetActionState() const { return m_eActionState; }
    bool CanDoCaptainShootToScore();
    bool CanReceivePass();
    void SetSlideAttackSuccessFlag();
    void fn_80036A38(int nParam, float fAmount);
    bool fn_8003E8F4() const;
    bool fn_8003E74C() const;
    bool CollideWithFreezeCallback();
    bool CollideWithBananaCallback(const nlVector3& rv3BananaPosition);
    bool CollideWithShellCallback(ePowerupSize eSize, bool bUnknown, const nlVector3& rv3Pos1, const nlVector3& rv3Pos2);
    bool CollideWithBobombCallback(const nlVector3& v3CollisionLocation, float fBombRadius);
    void CollideWithChainCallback(ChainChomp* pChainChomp);
    void fn_8003295C(UnidentifiedNPC_801B43F8* pDebris);
    void fn_80032CB8(CollisionThwompPlayerData*);
    bool fn_8003E7F8() const;
    bool fn_8003E84C() const;
    bool fn_8003E9F0() const;
    bool fn_8003EA44() const;
    bool fn_8003EA6C() const;
    cFielder* GetMark() const { return m_pMark[0]; }
    cFielder* GetMark(int index) const { return m_pMark[index]; }
    void fn_8003057C(int nParam);
    void fn_800305DC(float fParam);
    void fn_8003063C(PlayerTweaks* pParam);
    void fn_800306A0(cFielder* pParam);
    void fn_800306DC();
    bool fn_800306F4(cFielder* pParam);
    void DoResetShotMeter(float fTime);
    bool IsActionDone() const;
    bool IsFallenDown() const;
    void fn_800D0534(float amount)
    {
        mUnidentified3E0 -= amount;
    }
    bool IsStuck() const;
    bool UnidentifiedClass17Or2() const
    {
        return (int)mUnidentified024.m_eCharacterClass == 0x11
            || (int)mUnidentified024.m_eCharacterClass == 2;
    }
    bool UnidentifiedClass17Or2Or6() const
    {
        return UnidentifiedClass17Or2() || (int)mUnidentified024.m_eCharacterClass == 6;
    }
    bool IsInvincible() const
    {
        return !IsStuck() && (muInvincibleStatus & 0x1F) == 0x1F;
    }
    bool UnidentifiedInvinciblePowerups() const
    {
        bool result = !IsStuck() && (muInvincibleStatus & 8);
        return result;
    }
    bool IsInvincibleHammers() const
    {
        bool result = false;
        if (!IsStuck() && (muInvincibleStatus & 0x10))
            result = true;
        return result;
    }
    bool IsInvincibleChars() const
    {
        bool result = false;
        if (!IsStuck() && (muInvincibleStatus & 1))
            result = true;
        return result;
    }
    void TestCollisionForInvicibility(cFielder* pOpponent);
    const LooseBallContactAnimInfo* fn_80038230(
        const LooseBallContactAnimInfo* pBallContactAnimInfo,
        int nNumContactAnims, unsigned short aFutureFacingDirection,
        const nlVector3& v3FuturePosition, const nlVector3& v3OneTimerTarget,
        float fAngle);
    bool IsHitting() const;
    bool fn_80038660() const;
    bool fn_80038918() const;
    float fn_8002E058();
    eFielderDesireState fn_8002E060();
    void fn_8002E0FC();
    int fn_8002E9D0() const;
    bool fn_8003499C() const;
    bool fn_8003E6EC() const;
    bool fn_8003E6FC() const;
    bool fn_8003E70C() const;
    bool fn_8003E71C() const;
    bool fn_8003E72C() const;
    bool fn_8003E73C() const;
    bool fn_800470B4(cFielder* pFielder, cPlayer* pAttacker);
    bool fn_80047240(cPlayer* pParam0, unsigned short aParam1,
        int nParam2, bool bParam3, bool bParam4);
    void SetAction(eFielderActionState actionState);
    bool InitDesire(eFielderDesireState eDesireType, float fConfidence,
        float fDuration, const FuzzyVariant& opt1, const FuzzyVariant& opt2);
    void EndDesire();
    void EndAction();
    void fn_8004E11C(float fParam);
    void InitActionSlideAttackReact(cPlayer* pAttacker, bool bSkipEvent);
    void InitActionElectrocution(const nlVector3& wallPosition,
        const nlVector3& wallNormal, bool bParam);
    void fn_800451B0(const nlVector3& v3Position);
    void ActionElectrocution(float dt);
    void fn_80045930();
    void fn_8004643C(float fDeltaT);
    void fn_80045C74(float fDeltaT);
    void InitActionHit(cFielder* pTarget, unsigned short aDirection);
    void ActionHit(float fDeltaT);
    void fn_800474FC(float fDeltaT);
    void InitActionIdleTurn(unsigned short desiredFacingDirection);
    void ActionIdleTurn(float fDeltaT);
    void InitActionLateOneTimerFromVolley();
    void ActionLateOneTimerFromVolley(float fDeltaT);
    bool DoCommonInitActionLooseBall(
        const nlVector3& rv3OneTimerTarget, bool bVolleyPass);
    void InitActionLooseBallPass(cFielder* pPassTarget, bool bVolleyPass);
    void fn_80048484(float fDeltaT);
    void InitActionLooseBallShot(bool bIsChipShot);
    void fn_800486DC(float fDeltaT);
    void fn_800489C0();
    float fn_800489C4();
    float fn_80048A08();
    void InitActionMegaStrikeMeter(bool bParam);
    void fn_80048FB0(float fDeltaT, bool bButtonPressed, int nParam);
    void fn_8004923C(float fDeltaT, bool bButtonPressed, int nParam);
    void DoMegaMeterFirstButtonPressEvent(int nParam);
    void DoMegaMeterSecondButtonPressEvent(int nParam);
    void InitActionOneTimer(int animID, nlVector3& targetPos,
        float fAdjustEndTime, bool bIsChipShot, s16 nTurnAdjust);
    void fn_80049EA0(float fDeltaT);
    void InitActionOneTouchPassFromVolley(cPlayer* pPlayer, bool bParam);
    void ActionOneTouchPassFromVolley(float fDeltaT);
    bool fn_8004A330(cFielder* pOther);
    bool InitActionPass(
        cPlayer* pPassTarget, bool bVolleyPass, int nParam, bool bIsOneTouchPass);
    void ActionPass(float fDeltaT);
    void fn_8004F828();
    void fn_8004F8E8();
    void fn_8004F974(float fDeltaT);
    void fn_8004FA34();
    void fn_8004FB04();
    void fn_8004FC90(float fDeltaT);
    void fn_8004FF40();
    float fn_8004FFF8();
    void fn_80050008();
    void fn_8005001C(bool bForce);
    void fn_800501F0(bool bParam);
    bool fn_80050284();
    void fn_80045AEC(PhysicsObject* pObject);
    void ActionPostWhistle(float fDeltaT);
    void ShootBallDueToContact(const nlVector3& v3IncomingVelocity);
    void ShootBallDueToContact(unsigned short aShootDirection);
    void DoClearBall();
    void DoFindBestShotTarget(nlVector3& v3PositionOut, float& fShotSpeed, int nParam);
    void DoRegularShooting(bool bParam);
    void InitActionBombReact(const nlVector3& v3BombPosition, float fRadius);
    void InitActionBombHitReact(const nlVector3& v3BombPosition);
    void InitActionBananaReact(const nlVector3& fDeltaT);
    void InitActionShellReact(const nlVector3& v3CollisionLocation,
        const nlVector3& v3CollisionVelocity);
    void InitActionRunning();
    void ActionRunning(float dt);
    void asmRunning();
    void asmRunningWB(float fDeltaT);
    void InitActionRunningWB(bool bWaitForAnimToFinish);
    void ActionRunningWB(float dt);
    void fn_8004B658();
    void fn_80043ADC();
    void fn_80044148(const nlVector3& v3Velocity);
    void fn_80044BEC(float fDeltaT);
    bool fn_800447C0(unsigned short aDirection);
    void fn_80043C18(float fDeltaT);
    void fn_80044290(float fDeltaT);
    void fn_80046244();

    void fn_8004B148();
    void fn_8004B2E4(float fDeltaT);
    bool fn_8004B86C(bool bIsChipShot, bool bParam);
    void fn_8004BF58(eFielderActionState eNewAction);
    void InitActionSlideAttack(cFielder* pTarget, int nParam, float fTime);
    void fn_8004BB80(float fDeltaT);
    void fn_8004C02C(float fDeltaT);
    void fn_8004C88C(float fDeltaT);
    void fn_8004D238();
    void fn_8004D480(const nlVector3& v3CollisionVelocity);
    void fn_8004E228();
    void fn_8004E8B8();
    void fn_8004EA9C();
    void fn_8004E6B4();
    void fn_8004E92C();
    void fn_8004EAB4(float fDeltaT);
    void fn_8004EC40();
    void fn_8004ED64();
    void fn_8004EE48(float fDeltaT);
    void fn_8004F180();
    void fn_8004F204();
    void fn_8004F2FC(float fDeltaT);
    void fn_8004E438();
    void ActionSlideAttackReact(float fDeltaT);
    void ActionBombReact(float fDeltaT);
    void ActionSTSHitReact(float fDeltaT);
    void ActionShellReact(float fDeltaT);
    void ActionBananaReact(float fDeltaT);
    void ActionSquishReact(float fDeltaT);
    void InitActionReceivePass(int animID, nlVector3& v3TargetPos,
        short sDirectionSeekSpeed, float fAdjustEndTime);
    void InitActionWait();
    void ActionWait(float fDeltaT);
    bool IsStriker() const;
    bool IsWinger() const;
    bool IsMidField() const;
    bool IsDefense() const;
    PlayerTweaks* UnidentifiedGetTweaks() const { return mUnidentified32C; }

public:
    /* 0x324 */ PlayerTweaks* m_pTweaks;

private:
    /* 0x328 */ PlayerTweaks* mUnidentified328;
    /* 0x32C */ PlayerTweaks* mUnidentified32C;
    /* 0x330 */ UnidentifiedFielderPair330 mUnidentified330;
    /* 0x338 */ u16 mUnidentified338;

public:
    /* 0x33A */ bool mUnidentified33A;

private:
    /* 0x33B */ u8 mUnknown33B[0x01];

public:
    /* 0x33C */ int mUnidentified33C;

public:
    /* 0x340 */ float mUnidentified340;
    /* 0x344 */ float mUnidentified344;
    /* 0x348 */ bool mUnidentified348;

private:
    /* 0x349 */ u8 mUnknown349[0x03];

public:
    /* 0x34C */ float mUnidentified34C;
    /* 0x350 */ nlVector3 mUnidentified350;

private:
    /* 0x35C */ float mUnidentified35C;

public:
    /* 0x360 */ bool mUnidentified360;

public:
    /* 0x361 */ bool bYoshiInWindup;
    /* 0x362 */ bool bIsModified;

private:
    /* 0x363 */ u8 mUnknown363[0x01];

public:
    /* 0x364 */ UnidentifiedFielderAction364 mActionLooseBallPassVars;

public:
    /* 0x368 */ float mUnidentified368;

private:
public:
    /* 0x36C */ cPlayer* mUnidentified36C;
    /* 0x370 */ bool mUnidentified370;
    /* 0x371 */ bool mUnidentified371;

private:
    /* 0x372 */ u8 mUnknown372[0x02];

public:
    /* 0x374 */ UnidentifiedFielderPair374 mUnidentified374;
    /* 0x37C */ UnidentifiedFielderAction37C mActionRunningVars;

public:
    /* 0x384 */ UnidentifiedFielderAction384 mActionRunningWBVars;

private:
    /* 0x386 */ u8 mUnknown386[0x02];

public:
    /* 0x388 */ int mUnidentified388;
    /* 0x38C */ bool bAttackSucceeded;
    /* 0x38D */ bool mUnidentified38D;

private:
    /* 0x38E */ u8 mUnknown38E[0x02];

public:
    /* 0x390 */ float mUnidentified390;
    /* 0x394 */ float mUnidentified394;
    /* 0x398 */ float mUnidentified398;
    /* 0x39C */ float mUnidentified39C;
    /* 0x3A0 */ float mUnidentified3A0;
    /* 0x3A4 */ float mUnidentified3A4;
    /* 0x3A8 */ float mUnidentified3A8;
    /* 0x3AC */ float mUnidentified3AC;
    /* 0x3B0 */ float mUnidentified3B0;
    /* 0x3B4 */ float mUnidentified3B4;
    /* 0x3B8 */ bool mUnidentified3B8;

private:
    /* 0x3B9 */ u8 mUnknown3B9[0x03];

public:
    /* 0x3BC */ float mUnidentified3BC;
    /* 0x3C0 */ float mUnidentified3C0;
    /* 0x3C4 */ float mUnidentified3C4;
    /* 0x3C8 */ float mUnidentified3C8;
    /* 0x3CC */ float mUnidentified3CC;
    /* 0x3D0 */ float mUnidentified3D0;
    /* 0x3D4 */ float mUnidentified3D4;

public:
    /* 0x3D8 */ s16 mUnidentified3D8;
    /* 0x3DA */ s16 mUnidentified3DA;

public:
    /* 0x3DC */ bool mUnidentified3DC;

private:
    /* 0x3DD */ bool mUnidentified3DD;

private:
    /* 0x3DE */ u8 mUnknown3DE[0x02];

public:
    /* 0x3E0 */ float mUnidentified3E0;
    /* 0x3E4 */ float mUnidentified3E4;
    /* 0x3E8 */ UnidentifiedFielderAbility3E8 mUnidentified3E8;
    /* 0x3F4 */ float mUnidentified3F4;
    /* 0x3F8 */ UnidentifiedAbilityEffect mUnidentified3F8;

private:
    /* 0x404 */ float mUnidentified404;

public:
    /* 0x408 */ float mUnidentified408;

private:
    /* 0x40C */ float mUnidentified40C;

public:
    /* 0x410 */ UnidentifiedFielderAction410 mUnidentified410;

public:
    /* 0x420 */ BulletBillObject* mUnidentified420;

public:
    /* 0x424 */ bool mUnidentified424;

private:
    /* 0x425 */ u8 mUnknown425[0x03];

public:
    /* 0x428 */ UnidentifiedFielderInput* mUnidentified428;

private:
    /* 0x42C */ bool m_bHasBeenUpdated;
    /* 0x42D */ u8 mUnknown42D[3];

public:
    /* 0x430 */ eFielderActionState m_eActionState;

private:
    /* 0x434 */ bool m_bInPosition;
    /* 0x435 */ u8 mUnknown435[3];
    /* 0x438 */ Timer m_tMoveToTurboTimer;

public:
    /* 0x440 */ Timer mtPostDekeTimer;

public:
    /* 0x448 */ int m_nPowerupAnimID;

private:
    /* 0x44C */ Timer mtPowerupThrowTime;

public:
    /* 0x454 */ u32 muInvincibleStatus;

private:
    /* 0x458 */ float mfAirInterceptHeight[2];

public:
    /* 0x460 */ eRole m_eRole;

private:
    /* 0x464 */ cFielder* m_pMark[4];

public:
    /* 0x474 */ bool mbWasHitByPowerupThisFrame;
    /* 0x475 */ bool mbTangible;

private:
    /* 0x476 */ bool mbIgnorePadSwitchRelease;
    /* 0x477 */ u8 mUnknown477;

public:
    /* 0x478 */ int mUnidentified478;

public:
    /* 0x47C */ ShotMeter* m_pShotMeter;
}; // total size: 0x480

#endif // GAME_AI_FIELDER_H
