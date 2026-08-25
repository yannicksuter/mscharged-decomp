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

class cFielder : public cPlayer
{
public:
    virtual ~cFielder();
    virtual void PreUpdate(float fTime);

    void CleanUpAction();
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
    bool IsStriker() const;
    bool IsWinger() const;
    bool IsMidField() const;
    bool IsDefense() const;

private:
    /* 0x328 */ u8 mUnknown328[0x08];
    /* 0x330 */ u8 mUnknown330[0xFC];
    /* 0x42C */ bool m_bHasBeenUpdated;
    /* 0x42D */ u8 mUnknown42D[3];

public:
    /* 0x430 */ eFielderActionState m_eActionState;

private:
    /* 0x434 */ u8 mUnknown434[0x2C];
    /* 0x460 */ eRole m_eRole;
    /* 0x464 */ u8 mUnknown464[0x10];
    /* 0x474 */ bool mbWasHitByPowerupThisFrame;

public:
    /* 0x475 */ bool mUnidentified475;

private:
    /* 0x476 */ u8 mUnknown476[6];
    /* 0x47C */ ShotMeter* m_pShotMeter;
}; // total size: 0x480

#endif // GAME_AI_FIELDER_H
