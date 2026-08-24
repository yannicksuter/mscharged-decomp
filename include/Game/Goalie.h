#ifndef GAME_GOALIE_H
#define GAME_GOALIE_H

#include "Game/AI/GoalieSave.h"
#include "Game/GoalieFatigue.h"
#include "Game/Player.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"

class PhysicsGoalie;
class cPoseNode;
class cPN_SAnimController;
class cPN_SingleAxisBlender;

enum eGoalieActionState
{
    GOALIEACTION_MOVE = 0,
};

enum eGoalieMoveDirection
{
    GOALIEDIR_IDLE = 0,
};

enum eGoalieCrouchType
{
    GOALIECROUCH_SHOT = 0,
};

enum eGoalieOffplayType
{
    GOALIE_OFFPLAY_NONE = 0,
};

enum eUrgency
{
    URGENCY_LOW = 0,
};

class Goalie : public cPlayer
{
public:
    void SetGoalieAction(eGoalieActionState newGoalieState, int newSubstate);
    static void SaveBlendCallback(
        unsigned int nParam, cPN_SAnimController* pAnimCtrl);
    cPoseNode* SetupBlender(bool bPrimary, const float* fStartPercent,
        int nMainAnimID, int nMilestone);
    void PlayNewAnim(int nAnimID);
    void CleanGoalieAction();
    float CheckForDelflectAwayFromNet();
    void InitActionMove(bool bParam);
    void InitActionSaveSetup(bool bCanReposition);
    void InitActionSave();
    void CleanupStun();
    void ChooseSwatAnim(int nParam);
    PhysicsGoalie* GetPhysicsGoalie();
    static void MoveDirectionCB(
        unsigned int nParam, cPN_SingleAxisBlender* blender);
    static void MoveWeightCB(
        unsigned int nParam, cPN_SingleAxisBlender* blender);
    static void StrafeSynchronizedSpeedCallback(
        unsigned int nParam, cPN_SAnimController* controller);
    static void RunWeightCB(
        unsigned int nParam, cPN_SingleAxisBlender* blender);
    void ActionSaveSetup(float deltaTime);

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
    /* 0x3E0 */ unsigned int muBallChangeCount;
    /* 0x3E4 */ unsigned int muBallDeflectCount;
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
    /* 0x414 */ cPlayer* mpShooter;
    /* 0x418 */ cPlayer* mpTarget;
    /* 0x41C */ cPlayer* mpMonty;
    /* 0x420 */ cPlayer* mpSkillShooter;
    /* 0x424 */ SaveData* mpSaveData;
    /* 0x428 */ SaveBlendInfo mBlendInfo;
    /* 0x4B8 */ GoalieFatigue mFatigue;
    /* 0x4C8 */ u8 mUnidentified4C8[8];
}; // total size: 0x4D0

#endif // GAME_GOALIE_H
