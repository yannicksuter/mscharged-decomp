#ifndef GAME_GOALIE_H
#define GAME_GOALIE_H

#include "Game/AI/GoalieSave.h"
#include "Game/GoalieFatigue.h"
#include "Game/Player.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"

class PhysicsGoalie;
class LooseBallInfo;
class cFielder;
class cPoseNode;
class cPN_SAnimController;
class cPN_SingleAxisBlender;

enum eGoalieActionState
{
    GOALIEACTION_MOVE = 0,
    GOALIEACTION_SAVE_SETUP = 2,
    GOALIEACTION_SAVE_REPOSITION = 3,
    GOALIEACTION_SAVE = 4,
    GOALIEACTION_DIVE_RECOVER = 6,
    GOALIEACTION_STS_RECOVER = 7,
    GOALIEACTION_PASS = 8,
    GOALIEACTION_PASS_INTERCEPT = 9,
    GOALIEACTION_PRE_CROUCH = 10,
    GOALIEACTION_PURSUE_BALL_CARRIER = 11,
    GOALIEACTION_PURSUE_BALL_POUNCE = 12,
    GOALIEACTION_UNIDENTIFIED_13 = 13,
    GOALIEACTION_LOOSEBALL_SETUP = 14,
    GOALIEACTION_LOOSEBALL_CATCH = 15,
    GOALIEACTION_LOOSEBALL_PICKUP = 16,
    GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING = 17,
    GOALIEACTION_LOOSEBALL_PURSUE_ROLLING = 18,
    GOALIEACTION_LOOSEBALL_DESPERATE = 19,
    GOALIEACTION_OFFPLAY = 22,
    GOALIEACTION_SNAP_BALL = 23,
    GOALIEACTION_GRAB_BALL = 24,
};

enum eGoalieMoveDirection
{
    GOALIEDIR_IDLE = 0,
};

enum eGoalieCrouchType
{
    GOALIECROUCH_SHOT = 0,
    GOALIECROUCH_PASS = 1,
    GOALIECROUCH_LOOSEBALL = 2,
};

enum eGoalieOffplayType
{
    GOALIE_OFFPLAY_NONE = 0,
};

enum eUrgency
{
    URGENCY_LOW = 0,
    URGENCY_MED = 1,
    URGENCY_HIGH = 2,
};

class Goalie : public cPlayer
{
public:
    void SetGoalieAction(eGoalieActionState newGoalieState, int newSubstate);
    static void SaveBlendCallback(
        unsigned int nParam, cPN_SAnimController* pAnimCtrl);
    cPoseNode* SetupBlender(bool bPrimary, const float* fStartPercent,
        int nMainAnimID, int nMilestone);
    void PlayBlendedAnims(
        float fStartTime, float fParam2, int nMilestone);
    void PlayNewAnim(int nAnimID);
    void CleanGoalieAction();
    float CheckForDelflectAwayFromNet();
    void CheckForLimbEndZoneCollision();
    void InitActionMove(bool bParam);
    void InitActionMoveWB();
    void InitActionPass(bool useTarget);
    void InitActionPreCrouch(eGoalieCrouchType crouchType);
    void fn_8008BBB0(cFielder* pTarget, int nPursueDekeType);
    void InitActionLooseBallPickup(float fDistance, bool bStartPickup);
    void InitActionSaveSetup(bool bCanReposition);
    void InitActionSave();
    inline void InitActionPassInterceptSave();
    inline void InitActionPursueBallCarrier();
    inline void InitActionPursueBallPounce();
    inline void InitActionPursueRecover();
    void CleanupStun();
    void ChooseSwatAnim(int nParam);
    void ExecutePounce(cPlayer* pPlayer, bool bCheckHitDistance);
    PhysicsGoalie* GetPhysicsGoalie();
    void SetDesiredSaveFacing(const nlVector3& v3BallPosition);
    bool IsCloseToPlane(const nlVector3& rPos1,
        const nlVector3& rPos2, float fThreshold);
    void MakeExertEvent();
    bool CanInterceptPass();
    bool CheckForSTSAttack();
    bool IsWithinPounceRange();
    bool IsOpponentBallCarrierInRange();
    cPlayer* FindOpenPassTarget();
    bool IsTargetViable(cPlayer* pTarget);
    bool fn_8007BC40();
    bool fn_8007C73C();
    bool fn_8007D740();
    void FindDesiredGoaliePosition(nlVector3& pos, nlVector3& dir,
        nlVector3& focus, unsigned short& ang,
        const nlVector3* pThreatPos);
    int ChooseRunAnim(short nAngle, const nlVector3& rTargetPos,
        float fThreshold);
    void TrackTarget(
        const nlVector3& v3Target, float fRatio, float fParam3);
    void TacklePlayer(cPlayer* pPlayer);
    void StealBall(cPlayer* pPlayer);
    void WhackSTSPlayer(cFielder* pFielder);
    bool IsLooseBallClose(float fDistFromBox);
    bool IsPassThreat();
    void InitActionSaveReposition();
    void InitActionLooseBallPursueRolling();
    void InitActionLooseBallSetup();
    void InitActionLooseBallCatch();
    static void MoveDirectionCB(
        unsigned int nParam, cPN_SingleAxisBlender* blender);
    static void MoveWeightCB(
        unsigned int nParam, cPN_SingleAxisBlender* blender);
    static void StrafeSynchronizedSpeedCallback(
        unsigned int nParam, cPN_SAnimController* controller);
    static void RunWeightCB(
        unsigned int nParam, cPN_SingleAxisBlender* blender);
    void ActionSaveSetup(float deltaTime);
    void ActionDiveRecover(float fDeltaT);
    void ActionPass(float deltaTime);
    void ActionPassIntercept(float deltaTime);
    void ActionPreCrouch(float deltaTime);
    void ActionPursueBallCarrier(float fDeltaT);
    void ActionPursueBallPounce(float fDeltaT);
    void fn_8008A610(float fDeltaT);
    void ActionOffplay(float fDeltaT);
    void ActionLooseBallPursueBouncing(float deltaTime);
    void ActionSnapBall(float fDeltaT);
    void ActionGrabBall(float fDeltaT);
    void fn_8008B718(float fDeltaT);

private:
    /* 0x328 */ eGoalieActionState mGoalieActionState;
    /* 0x32C */ eGoalieActionState mPrevGoalieActionState;
    /* 0x330 */ eUrgency mUrgency;
    /* 0x334 */ int mnSubstate;
    /* 0x338 */ eGoalieMoveDirection mMoveDirection;
    /* 0x33C */ eGoalieCrouchType mCrouchType;
    /* 0x340 */ int mPursueDekeType;
    /* 0x344 */ int mPursueDekeState;
    /* 0x348 */ float mfSwitchTime;
    /* 0x34C */ unsigned int muSaveType;
    /* 0x350 */ float mfWaitTime;
    /* 0x354 */ float mfTimeTilSave;
    /* 0x358 */ float mfDelayTime;
    /* 0x35C */ float mfWallBlock;
    /* 0x360 */ unsigned int muWallID;
    /* 0x364 */ bool mbPlayMiss;
    /* 0x365 */ bool mbShouldMiss;
    /* 0x366 */ bool mbStunEffectActive;
    /* 0x367 */ bool mbDoIntercept;
    /* 0x368 */ bool mbDoNavigate;
    /* 0x369 */ bool mbDoHeadTrack;
    /* 0x36A */ bool mbBallImpacted;
    /* 0x36B */ bool mbNoUserControl;
    /* 0x36C */ bool mbIsPosed;
    /* 0x36D */ bool mbIsDown;
    /* 0x36E */ bool mbPickedUp;
    /* 0x36F */ bool mbRecalcSave;
    /* 0x370 */ bool mbCheckForMegaGoal;
    /* 0x371 */ bool mbMegaUserSave;
    /* 0x372 */ bool mbGrabMonty;
    /* 0x373 */ bool mbTryLobSave;
    /* 0x374 */ nlVector3 mv3LocalContactPosition;
    /* 0x380 */ nlVector3 mv3LocalContactVelocity;
    /* 0x38C */ nlVector3 mv3TargetPosition;
    /* 0x398 */ nlVector3 mv3TargetVelocity;
    /* 0x3A4 */ nlVector3 mv3NavTarget;
    /* 0x3B0 */ nlVector3 mv3LocalNavTarget;
    /* 0x3BC */ unsigned short maLocalAngle;
    /* 0x3BE */ u8 mUnidentified3BE[2];
    /* 0x3C0 */ unsigned short maInitialAngle;
    /* 0x3C2 */ unsigned short maSaveAngle;
    /* 0x3C4 */ float mfTargetTime;
    /* 0x3C8 */ float mfTargetDist;
    /* 0x3CC */ float mfSpeedScale;
    /* 0x3D0 */ float mfBallCharge;
    /* 0x3D4 */ float mfNextBallTime;
    /* 0x3D8 */ float mfMegaAccuracy;
    /* 0x3DC */ float mfMegaTargetTime;
    /* 0x3E0 */ unsigned int muBallDeflectCount;
    /* 0x3E4 */ unsigned int mUnidentified3E4;
    /* 0x3E8 */ eGoalieOffplayType mnOffplayPending;
    /* 0x3EC */ unsigned int muMegaAnimState;
    /* 0x3F0 */ unsigned int muMegaStoreTexID;
    /* 0x3F4 */ unsigned int muMegaNextTarget;
    /* 0x3F8 */ unsigned int muMegaReadyToSave;
    /* 0x3FC */ int mBallsLaunched;
    /* 0x400 */ int mLowLobAnim;
    /* 0x404 */ Timer mFreezeTimer;
    /* 0x40C */ void* mMegaMachine;
    /* 0x410 */ cPlayer* mpPassTarget;
    /* 0x414 */ cFielder* mpShooter;
    /* 0x418 */ cFielder* mpTarget;
    /* 0x41C */ cPlayer* mpMonty;
    /* 0x420 */ cPlayer* mpSkillShooter;
    /* 0x424 */ SaveData* mpSaveData;
    /* 0x428 */ SaveBlendInfo mBlendInfo;
    /* 0x4B8 */ GoalieFatigue mFatigue;
    /* 0x4C8 */ u8 mUnidentified4C8[4];
    /* 0x4CC */ const LooseBallInfo* mpLooseBallInfo;
}; // total size: 0x4D0

#endif // GAME_GOALIE_H
