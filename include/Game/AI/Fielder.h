#ifndef GAME_AI_FIELDER_H
#define GAME_AI_FIELDER_H

#include "Game/Player.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/FielderAbility.h"

enum ePenaltyType
{
    PEN_TYPE_HIT_WITH_BALL = 0,
    PEN_TYPE_HIT_NO_BALL = 1,
    PEN_TYPE_SLIDE_WITH_BALL = 2,
    PEN_TYPE_SLIDE_NO_BALL = 3,
    NUM_PEN_TYPES = 4,
};

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
    FIELDERDESIRE_PASS = 14,
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
void SetFielderFrozenState(cFielder* pFielder, int nParam, float fParam);
class ChainChomp;
class WindDebris;
struct CollisionThwompPlayerData;
bool IsFielderFrontInvincible(cFielder*, const nlVector3*);
class cSHierarchy;
class AnimRetargetList;
class CharacterPhysicsData;
class WaluigiWallManager;
void FreezeEveryoneButCaptain(cFielder* pFielder);
const LooseBallContactAnimInfo* GetOneTimerIdleGroundContactAnims();
int GetNumOneTimerIdleGroundContactAnims();
const LooseBallContactAnimInfo* GetOneTimerIdleVolleyContactAnims();
int GetNumOneTimerIdleVolleyContactAnims();
const LooseBallContactAnimInfo* GetOneTimerLeadGroundContactAnims();
int GetNumOneTimerLeadGroundContactAnims();

struct ActCrowdVars
{
    ActCrowdVars(bool bParam, float fParam)
        : bHasBeenSuckedToMiddle(bParam)
        , fStuckInRiotTime(fParam)
    {
    }
    /* 0x0 */ bool bHasBeenSuckedToMiddle;
    /* 0x4 */ float fStuckInRiotTime;
}; // total size: 0x8

struct ActRunPassVars
{
    ActRunPassVars()
        : nHeldTicks(0)
        , fSpeed(0.0f)
    {
    }
    /* 0x0 */ int nHeldTicks;
    /* 0x4 */ float fSpeed;
}; // total size: 0x8

struct ActLooseBallPass
{
    ActLooseBallPass()
        : passTarget(0)
    {
    }

    cFielder* passTarget;
};

struct ActRunningVars
{
    ActRunningVars()
        : eLastStrafeDirection(STRAFE_IDLE)
        , bFirstCycleOfTurbo(false)
    {
    }

    eStrafeDirection eLastStrafeDirection;
    bool bFirstCycleOfTurbo;
};

struct ActRunningWBVars
{
    ActRunningWBVars()
    {
        bWaitForAnimToFinish = false;
        bCuePitch = false;
    }

    bool bCuePitch;
    bool bWaitForAnimToFinish;
};

struct ActBooSkillshot
{
    ActBooSkillshot()
    {
        nlVec3Set(v3StartPosition, 0.0f, 0.0f, 0.0f);
        bFollowingBall = false;
    }

    nlVector3 v3StartPosition;
    bool bFollowingBall;
};

class FuzzyVariant;
class UnidentifiedVariantCollection;
class AvoidController;
class cPN_SingleAxisBlender;
class DesireSteering;
class DesireUserControlled;
class UnidentifiedDesire35;
class DesireSuperPower;
extern "C" void fn_800C5DBC(DesireSteering*, float);
extern "C" void fn_800C6FDC(DesireSteering*, float);
bool IsFielderSuperPowerTankOn(const cFielder*);
extern "C" bool fn_800D0DB0(DesireSuperPower*, void*);
float GetFielderHitReach(cFielder* pFielder);
class PhysicsObject;
class ShotMeter;
class AIContext;
class ScriptMachine;
class FuzzyRuntimeBase;
struct BulletBillObject;

class cFielder : public cPlayer
{
    static float GetHeadTrackingFinalPhaseTime();
    static float GetModifiedShotTargetDistance(float shotDistance);
    void RegisterDebugFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActCrowdVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActDekeVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActElectVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActFallVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActHitVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActHitReactVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActSuperVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActShootPassCommonFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActLooseBallPassFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActOneTimerVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActPassingVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActRunPassVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActRunningVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActRunningWBVarsFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActSlideAttackFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActMegaStrikeMeterFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActStunnedFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActBowserSuperFields(unsigned short* type, DebugWriteCache* cache);
    void RegisterActWarioSuperFields(unsigned short* type, DebugWriteCache* cache);
    friend void fn_800C5DBC(DesireSteering*, float);
    friend void fn_800C6FDC(DesireSteering*, float);
    friend class UnidentifiedDesire35;
    friend class DesireUserControlled;
    friend class DesireSuperPower;
    friend class DesireUsePowerup;
    friend class DesireShrink;
    friend bool IsFielderSuperPowerTankOn(const cFielder*);
    friend bool fn_800D0DB0(class DesireSuperPower*, void*);
    friend float GetFielderHitReach(cFielder*);

public:
    bool CanReactToGroundEffects() const;
    bool ActivateDesire(int state, UnidentifiedVariantCollection* parameters);
    const nlVector3& GetDesiredPosition();
    const nlVector3& GetDesiredVelocity();

    void SetDesiredSpeed(float minSpeed, float maxSpeed);
    float GetSlideAttackSpeed(int direction);
    void SetTangible(bool tangible, bool affectGoalLine);
    bool GetDekePadDirection(unsigned short* direction);
    bool IsDekeRequested(unsigned short* direction);
    bool IsReceivePassDekeRequested(unsigned short* direction);
    bool IsDekePadPressed();
    void TestButtonsToQueueActions(float deltaTime);
    bool TestQueuedActions();

    void GetApproachPosition(nlVector3* position, const nlVector3* from, float predictionTime);
    float GetDekeDistance();
    void EmitMegaStrikeWindup();
    void CleanActionShootToScore();
    void CleanActionShot(eFielderActionState newAction);
    void SetWindupWBAnimState();

    void TestButtonsWindup();
    void TestButtonsRunningWB(float deltaTime);
    void UpdateTimers(float deltaTime);
    void UseCaptainPowerup();
    bool IsReceivePassHitRequested(unsigned short* direction);
    void UpdateFacingToLooseBall();

    float GetRunningSpeed();
    void ClearInvincibility(bool force);
    void SetMegaStrikeResult(float numBalls, float accuracy);
    void EjectMonty(bool finished, unsigned short direction);
    void EndMontyDeke();
    float GetAirInterceptHeight(int type);
    bool CalculateFormationPosition(nlVector3& position);
    bool IsWarioSuperPowerActive() const;

    bool CanBeHitBySkillshot();
    bool CanGetElectrocuted() const;
    void PlayImpactCameraRumble();
    void ShouldIWave();

    void CollideWithShockwaveCallback(const nlVector3& position);

    void SetNormalTweaks();
    void SetSuperPowerTweaks();
    void SetRunningAnimState(float blendTime);
    void SetRunningWBAnimState(float blendTime);
    float GetShotProbability(float fValue);
    void CalcRegularShot(nlVector3& velocity, nlVector3& target, int ballState);
    bool DoLooseBallContactFromIdle(nlVector3& animStart, float& animStartTime, nlVector3& ballContact, float& ballContactTime, unsigned short facing, const LooseBallContactAnimInfo* info);
    bool DoLooseBallContactFromRun(nlVector3& animStart, float& animStartTime, nlVector3& ballContact, float& ballContactTime, const LooseBallContactAnimInfo* info, const nlVector3& passIntercept, unsigned int facing);
    void DoPenaltyCardBooking(cFielder* foulee, ePenaltyType type);
    void RestoreTangibility(bool fadeIn);
    void BeginDekeIntangibility();
    void CleanActionDeke();
    void SetStartAnimState(int animState);
    void SetStartWBAnimState();
    void SetHardStopAnimState();
    bool ShouldIClearBall();
    void TestAnimBallContact();
    void TestLooseBallControls(bool forceContact);
    float CalcSlideAttackBallIntercept(nlVector3& target, int direction);
    AvoidController* GetAvoidController();
    bool EndMushroom();
    bool EndShrink();
    bool EndStar();
    void EndFrozenOrDazed();
    void EndConfusion();
    void EndDaze();
    bool EndSuperPower(int);
    bool EndBowserSuperPower(bool);
    bool EndMarioSuperPower();
    bool EndLuigiSuperPower();
    bool EndWaluigiSuperPower();
    bool EndWarioSuperPower(bool);
    bool EndPeteySuperPower(bool);

    void SetHardStopRecoverAnimState();
    void SetHardStopTurnAnimState();
    void SetRunBackwardsAnimState();
    void SetStrafeLeftAnimState();
    void SetStrafeRightAnimState();
    void SetRunToBackRunningAnimState();
    void SetBackRunningToRunAnimState();
    void SetBackRunningStopAnimState();
    void SetBackRunningStopStartAnimState();
    void SetBackRunningStopRecoverAnimState();
    void SetStopAnimState();
    void SetStrafeRightStopAnimState();
    void SetStrafeLeftStopAnimState();
    void SetIdleAnimState();
    void SetIdleWBAnimState();
    static void RunningSABcallback(unsigned int parameter, cPN_SingleAxisBlender* blender);

    void GetReceivePassBallContactOffset(nlVector3&, unsigned short, const LooseBallContactAnimInfo*);
    PlayerTweaks* GetTweaks() const;
    void AddDesiredPosition(const nlVector3& position, float urgency, float weight);
    float GetDistanceToDesiredPos();
    bool CanContactLooseBall(bool requireBestInterceptor);
    bool IsActionModifierPressed();
    FuzzyRuntimeBase* GetFuzzyRuntime() const;
    float GetSpeedPowerupAdjusted(float fSpeed);

    bool FreezeWithPeachPhoto(float duration);
    bool CanBeFrozen() const;
    bool CanBeCaughtInPhoto() const;
    bool CanBeAffectedByPhoto() const;
    bool IsDaisySuperPowerActive() const;
    unsigned int IsFrozen() const;
    unsigned int IsShattered() const;
    bool IsMegaStrikeFrozen() const;

    cFielder(int nPlayerID, int nTeamID, eCharacterClass cc,
        const int* nModelID, cSHierarchy* pHierarchy,
        cAnimInventory* pAnimInventory,
        const CharacterPhysicsData* pCharacterPhysicsData, PlayerTweaks* pCharTweaks,
        PlayerTweaks* pUnidentifiedTweaks,
        AnimRetargetList* pAnimRetargetList, int nIndex);
    virtual ~cFielder();
    virtual void PrePhysicsUpdate();
    virtual void PreUpdate(float fTime);
    virtual void ResetAnimState();
    virtual void ResetEffects();
    virtual void SetDesiredFacingDirection(unsigned short aParam, bool bParam);
    virtual void SetPosition(const nlVector3& position);
    virtual void Update(float fDeltaT);
    virtual void Reset(const nlVector3& v3Position, unsigned short aDirection);
    virtual void SyncLog(void* context, DebugWriteCache* cache);
    virtual void ChecksumState(RunningChecksum* pChecksum);
    virtual bool CanPickupBall(cBall* pBall, bool bParam);
    virtual void CollideWithCharacterCallback(
        CollisionPlayerPlayerData* pData);
    virtual void CollideWithWallCallback(
        const CollisionPlayerWallData* pData);
    virtual void InitActionPostWhistle();
    virtual void CollideWithPatchCallback(const UnidentifiedEventData24*);

    void ClearPassTargetIfAmThePassTarget();
    void CleanUpAction(eFielderActionState actionState);
    ePowerUpType GetPowerupType();
    void UseTeamPowerup(cFielder* pTarget);
    void UpdateActionState(float dt);
    void UpdateHeadTracking(float fDeltaT);
    cFielder* DoFindBestHitTarget();
    void UpdateController(float fDeltaT);
    bool IsRunning() const;
    bool IsAboveFielder(cFielder* pOtherFielder) const;
    bool HasLooseBallContactPriority(cFielder* pOtherFielder) const;
    bool IsInFallAction() const;
    bool IsRunningWithBall() const
    {
        bool bRunningWithBall = false;
        if (m_eActionState == ACTION_RUNNING_WB)
        {
            bRunningWithBall = true;
        }
        return bRunningWithBall;
    }
    void StartRunning();
    bool CanGetElectrocuted(
        const CollisionPlayerWallData* eventData);
    eFielderActionState GetActionState() const { return m_eActionState; }
    bool IsInActionState(eFielderActionState actionState) const
    {
        return m_eActionState == actionState;
    }
    bool CanDoCaptainShootToScore();
    bool CanDoSidekickShootToScore();
    bool CanReceivePass();
    void SetSlideAttackSuccessFlag();
    void IncrementPowerupMeter(int nParam, float fAmount);
    bool IsPeachSuperPowerActive() const;
    bool IsSuperGrowActive() const;
    bool CollideWithFreezeCallback();
    bool CollideWithBananaCallback(const nlVector3& rv3BananaPosition);
    bool CollideWithShellCallback(ePowerupSize eSize, bool bUnknown, const nlVector3& rv3Pos1, const nlVector3& rv3Pos2);
    bool CollideWithBobombCallback(const nlVector3& v3CollisionLocation, float fBombRadius);
    void CollideWithChainCallback(ChainChomp* pChainChomp);
    void CollideWithWindDebrisCallback(WindDebris* pDebris);
    void CollideWithThwompCallback(CollisionThwompPlayerData*);
    bool IsMarioSuperPowerActive() const;
    bool IsLuigiSuperPowerActive() const;
    bool IsPeteySuperPowerActive() const;
    bool IsDKSuperPowerActive() const;
    bool IsYoshiSuperPowerActive() const;
    cFielder* GetMark() const { return m_pMark[0]; }
    cFielder* GetMark(int index) const { return m_pMark[index]; }
    void SetThingsToAvoid(int nParam);
    void SetAvoidanceMultiplier(float fParam);
    void SetTweaks(PlayerTweaks* pParam);
    void AddMark(cFielder* pParam);
    void ClearMarks();
    bool IsMarking(cFielder* pParam);
    void DoResetShotMeter(float fTime);
    bool IsActionDone() const;
    bool IsFallenDown() const;
    void DrainSuperPowerTank(float amount)
    {
        m_fSuperPowerTankLevel -= amount;
    }
    bool IsStuck() const;
    bool IsDaisyOrDryBones() const
    {
        return (int)mUnidentified024.m_eCharacterClass == 0x11
            || (int)mUnidentified024.m_eCharacterClass == 2;
    }
    bool HasTeleportDeke() const
    {
        return IsDaisyOrDryBones() || (int)mUnidentified024.m_eCharacterClass == 6;
    }
    bool IsInvincible() const
    {
        return !IsStuck() && (muInvincibleStatus & 0x1F) == 0x1F;
    }
    bool IsInvinciblePowerups() const
    {
        bool result = !IsStuck() && (muInvincibleStatus & 8);
        return result;
    }
    bool IsInvincibleHammers() const
    {
        bool result = !IsStuck() && (muInvincibleStatus & 0x10);
        return result;
    }
    bool IsInvincibleFront() const
    {
        bool result = !IsStuck() && (muInvincibleStatus & 2);
        return result;
    }
    bool IsInvincibleChars() const
    {
        bool result = !IsStuck() && (muInvincibleStatus & 1);
        return result;
    }
    void TestCollisionForInvincibility(cFielder* pOpponent);
    const LooseBallContactAnimInfo* FindLooseBallContactAnim(
        const LooseBallContactAnimInfo* pBallContactAnimInfo,
        int nNumContactAnims, unsigned short aFutureFacingDirection,
        const nlVector3& v3FuturePosition, const nlVector3& v3OneTimerTarget,
        float fAngle);
    bool IsHitting() const;
    bool IsSlideAttacking() const;
    bool IsFrozenStateActive() const;
    float GetMegaStrikeMeterPosition();
    eFielderDesireState GetDesireState();
    void EndAllDesires();
    int GetPreviousDesireState() const;
    bool IsReceivingVolleyPass() const;
    bool IsSuperPowerActive() const;
    bool IsStarActive() const;
    bool IsConfused() const;
    bool IsMushroomActive() const;
    bool IsSlippery() const;
    bool IsShrunk() const;
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
    void fn_800489C0(float);
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
    bool DoCalcCanDoPerfectPass(cFielder* pOther, const nlVector3& position);
    bool InitActionPass(
        cPlayer* pPassTarget, bool bVolleyPass, int nParam, bool bIsOneTouchPass);
    void ActionPass(float fDeltaT);
    void InitActionDKSuper();
    void DoDKSuperHit();
    void ActionDKSuper(float fDeltaT);
    void CleanUpPeachSuper();
    void InitActionPeachSuper();
    void ActionPeachSuper(float fDeltaT);
    void fn_8004FF40();
    float GetSuperPowerTankFraction();
    void ClearSuperPowerTank();
    void TurnOffSuperPowerTank(bool bForce);
    void InitSuperPowerTank(bool bTurnOn);
    bool TurnOnSuperPowerTank();
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
    bool InitActionShot(bool bIsChipShot, bool bIsOneTimer);
    void fn_8004BF58(eFielderActionState eNewAction);
    void InitActionSlideAttack(cFielder* pTarget, float fTime, int nParam);
    void fn_8004BB80(float fDeltaT);
    void fn_8004C02C(float fDeltaT);
    void fn_8004C88C(float fDeltaT);
    void fn_8004D238();
    void InitActionKnockdownReact(const nlVector3& v3CollisionVelocity);
    void fn_8004E228();
    void fn_8004E8B8();
    void fn_8004EA9C();
    void ReleaseHammerProjectile();
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
    void ActionShellReact(float fDeltaT);
    void ActionBananaReact(float fDeltaT);
    void ActionKnockdownReact(float fDeltaT);
    void ActionReceivePass(float fDeltaT);
    void InitActionReceivePass(int animID, nlVector3& v3TargetPos,
        short sDirectionSeekSpeed, float fAdjustEndTime);
    void InitActionWait();
    void ActionWait(float fDeltaT);
    bool IsStriker() const;
    bool IsWinger() const;
    bool IsMidField() const;
    bool IsDefense() const;
    PlayerTweaks* GetNormalTweaks() const { return m_pNormalTweaks; }

    /* 0x324 */ PlayerTweaks* m_pTweaks;

private:
    /* 0x328 */ PlayerTweaks* m_pSuperPowerTweaks;
    /* 0x32C */ PlayerTweaks* m_pNormalTweaks;
    /* 0x330 */ ActCrowdVars mActionCrowdVars;
    /* 0x338 */ u16 m_aDekeDirection;

public:
    /* 0x33A */ bool m_bDekeReset;

private:
    /* 0x33B */ u8 mUnknown33B[0x01];

public:
    /* 0x33C */ int m_nDPadDownCounter;

    /* 0x340 */ float m_fElectrocutionTime;
    /* 0x344 */ float m_fElectrocutionLiftTime;
    /* 0x348 */ bool m_bGroundElectrocution;

private:
    /* 0x349 */ u8 mUnknown349[0x03];

public:
    /* 0x34C */ float m_fFallingTime;
    /* 0x350 */ nlVector3 m_v3SuckToSpot;

private:
    /* 0x35C */ float m_fHitDistance;

public:
    /* 0x360 */ bool m_bHitReactFrameLock;

    /* 0x361 */ bool bYoshiInWindup;
    /* 0x362 */ bool bIsModified;

private:
    /* 0x363 */ u8 mUnknown363[0x01];

public:
    /* 0x364 */ ActLooseBallPass mActionLooseBallPassVars;

    /* 0x368 */ float m_fOneTimerAnimTime;

    /* 0x36C */ cPlayer* m_pPassTarget;
    /* 0x370 */ bool m_bAllowLeadPass;
    /* 0x371 */ bool m_bOneTouchPass;

private:
    /* 0x372 */ u8 mUnknown372[0x02];

public:
    /* 0x374 */ ActRunPassVars mActionRunPassVars;
    /* 0x37C */ ActRunningVars mActionRunningVars;

    /* 0x384 */ ActRunningWBVars mActionRunningWBVars;

private:
    /* 0x386 */ u8 mUnknown386[0x02];

public:
    /* 0x388 */ int m_eSlideAttackState;
    /* 0x38C */ bool bAttackSucceeded;
    /* 0x38D */ bool m_bSlideAttackReset;

private:
    /* 0x38E */ u8 mUnknown38E[0x02];

public:
    /* 0x390 */ float m_fMegaStrikeNumBalls;
    /* 0x394 */ float m_fMegaStrikeAccuracy;
    /* 0x398 */ float m_fMegaStrikeReceivedTimestamp;
    /* 0x39C */ float m_fMegaStrikeFirstPressPosition;
    /* 0x3A0 */ float m_fMegaStrikeSecondPressPosition;
    /* 0x3A4 */ float m_fMegaStrikeGreenRegionWidth;
    /* 0x3A8 */ float m_fMegaStrikeMeterPosition;
    /* 0x3AC */ float m_fMegaStrikePhaseTimeRemaining;
    /* 0x3B0 */ float m_fMegaStrikeSecondPhaseDuration;
    /* 0x3B4 */ float m_fMegaStrikeFirstPhaseDuration;
    /* 0x3B8 */ bool m_bMegaStrikeSecondPhase;

private:
    /* 0x3B9 */ u8 mUnknown3B9[0x03];

public:
    /* 0x3BC */ float m_fLocalMegaStrikeNumBalls;
    /* 0x3C0 */ float m_fLocalMegaStrikeAccuracy;
    /* 0x3C4 */ float m_fMegaStrikeSegment1Position;
    /* 0x3C8 */ float m_fMegaStrikeSegment2Position;
    /* 0x3CC */ float m_fMegaStrikeSegment3Position;
    /* 0x3D0 */ float m_fMegaStrikeSegment4Position;
    /* 0x3D4 */ float mUnidentified3D4;

    /* 0x3D8 */ s16 m_nStunnedAngularAcceleration;
    /* 0x3DA */ s16 m_nStunnedAngularVelocity;

    /* 0x3DC */ bool m_bSuperPowerTankOn;

private:
    /* 0x3DD */ bool m_bSuperPowerTankShutdownPending;

    /* 0x3DE */ u8 mUnknown3DE[0x02];

public:
    /* 0x3E0 */ float m_fSuperPowerTankLevel;
    /* 0x3E4 */ float m_fSuperPowerTankCapacity;
    /* 0x3E8 */ ActBowserSuper mActionBowserSuper;
    /* 0x3F4 */ float m_fNextGasTime;
    /* 0x3F8 */ WaluigiWallState mWaluigiWallState;

private:
    /* 0x404 */ float m_fPeteyLastMuckBallTime;

public:
    /* 0x408 */ float m_fPeteyMuckBallSpeed;

private:
    /* 0x40C */ float m_fPeteySuperPowerTime;

public:
    /* 0x410 */ ActBooSkillshot mActionBooSkillshot;

    /* 0x420 */ BulletBillObject* m_pBulletBill;

    /* 0x424 */ bool m_bMontyDekeFinished;

private:
    /* 0x425 */ u8 mUnknown425[0x03];

public:
    /* 0x428 */ AIContext* m_pAIContext;

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
    /* 0x478 */ int m_nMegaStrikeResultState;

    /* 0x47C */ ShotMeter* m_pShotMeter;
private:
    bool ShouldSkipHardStopAnim();
    float CalculateShotProbability(float fValue);
    float EvaluateShotProbability(float fValue);
    void SetRunLeanSAB(const int* anims, int count, int primary);
    bool CheckReceivePassState();
    bool IsAvailableToReceivePass();
}; // total size: 0x480

// Shared fielder functions and data.
class Desire;
Desire* GetFielderDesire(cFielder*, int);
bool CanUsePowerup(cFielder*, int);
bool IsBowserSuperPowerActive(const cFielder*);
bool IsWaluigiSuperPowerActive(const cFielder*);
int GetFielderHitReaction(cFielder*, cFielder*, float);
void TryFielderQueuedPass(cFielder*);
extern bool gForceUserControl;
extern bool gForceHomeUserControl;
extern bool gForceAwayUserControl;
extern unsigned char gEnableStandingWallElectrocution;
extern unsigned char gForceHeavyHitReaction;

bool IsFielderDazed(const cFielder* pFielder);

ScriptMachine* GetFielderScriptMachine(cFielder* pFielder);

#endif // GAME_AI_FIELDER_H
