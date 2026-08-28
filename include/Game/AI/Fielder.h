#ifndef GAME_AI_FIELDER_H
#define GAME_AI_FIELDER_H

#include "Game/Player.h"

enum eFielderActionState
{
    ACTION_NEED_ACTION = -1,
    ACTION_LOOSE_BALL_SHOT = 0x0A,
    ACTION_SHOT = 0x0B,
    ACTION_SHOOT_TO_SCORE = 0x0C,
    ACTION_UNKNOWN_15 = 0x15,
    ACTION_UNKNOWN_32 = 0x20,
    ACTION_UNKNOWN_34 = 0x22,
};

enum eFielderDesireState
{
    FIELDERDESIRE_FINISH_ACTION = 0x15,
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

const LooseBallContactAnimInfo* GetOneTimerIdleGroundContactAnims();
int GetNumOneTimerIdleGroundContactAnims();
const LooseBallContactAnimInfo* GetOneTimerIdleVolleyContactAnims();
int GetNumOneTimerIdleVolleyContactAnims();
const LooseBallContactAnimInfo* GetOneTimerLeadGroundContactAnims();
int GetNumOneTimerLeadGroundContactAnims();

class ShotMeter;
struct UnidentifiedFielderInput;

class cFielder : public cPlayer
{
public:
    virtual ~cFielder();
    virtual void PreUpdate(float fTime);
    virtual bool CanPickupBall(cBall* pBall, bool bParam);
    virtual void CollideWithCharacterCallback(
        CollisionPlayerPlayerData* pData);
    virtual void CollideWithWallCallback(
        const CollisionPlayerWallData* pData);
    virtual void InitActionPostWhistle();
    virtual void fn_80099074(UnidentifiedPlayerEventData*);

    void CleanUpAction();
    bool CanGetElectrocuted(
        const CollisionPlayerWallData* eventData);
    bool CanDoCaptainShootToScore();
    bool CanReceivePass();
    void fn_8002FDC4(unsigned short aParam, bool bParam);
    void fn_8003057C(void* pParam);
    void fn_800305DC(float fParam);
    void fn_8003063C(PlayerTweaks* pParam);
    void fn_800306A0(cFielder* pParam);
    void fn_800306DC();
    bool fn_800306F4(cFielder* pParam);
    void DoResetShotMeter(float fTime);
    bool IsActionDone() const;
    bool IsFallenDown() const;
    eFielderDesireState fn_8002E060();
    bool fn_8003E6FC() const;
    void fn_80047240(cPlayer* pParam0, unsigned short aParam1,
        int nParam2, bool bParam3, bool bParam4);
    void SetAction(eFielderActionState actionState);
    void EndDesire();
    void EndAction();
    void InitActionSlideAttackReact(cPlayer* pAttacker, bool bSkipEvent);
    void InitActionElectrocution(const nlVector3& wallPosition,
        const nlVector3& wallNormal, bool bParam);
    bool IsStriker() const;
    bool IsWinger() const;
    bool IsMidField() const;
    bool IsDefense() const;
    cFielder* GetMark() const { return mUnidentified464[0]; }

private:
    /* 0x328 */ u8 mUnknown328[0x04];
    /* 0x32C */ PlayerTweaks* mUnidentified32C;
    /* 0x330 */ u8 mUnknown330[0xAC];
    /* 0x3DC */ bool mUnidentified3DC;
    /* 0x3DD */ u8 mUnknown3DD[0x23];

public:
    /* 0x400 */ void* mUnidentified400;

private:
    /* 0x404 */ u8 mUnknown404[0x20];

public:
    /* 0x424 */ bool mUnidentified424;

private:
    /* 0x425 */ u8 mUnknown425[0x03];
    /* 0x428 */ UnidentifiedFielderInput* mUnidentified428;
    /* 0x42C */ bool m_bHasBeenUpdated;
    /* 0x42D */ u8 mUnknown42D[3];

public:
    /* 0x430 */ eFielderActionState m_eActionState;

private:
    /* 0x434 */ u8 mUnknown434[0x14];

public:
    /* 0x448 */ int m_nPowerupAnimID;

private:
    /* 0x44C */ u8 mUnknown44C[0x08];

public:
    /* 0x454 */ u32 muInvincibleStatus;

private:
    /* 0x458 */ u8 mUnknown458[0x08];

public:
    /* 0x460 */ eRole m_eRole;

private:
    /* 0x464 */ cFielder* mUnidentified464[4];

public:
    /* 0x474 */ bool mbWasHitByPowerupThisFrame;
    /* 0x475 */ bool mbTangible;

private:
    /* 0x476 */ u8 mUnknown476[6];

public:
    /* 0x47C */ ShotMeter* m_pShotMeter;
}; // total size: 0x480

#endif // GAME_AI_FIELDER_H
