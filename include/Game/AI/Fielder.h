#ifndef GAME_AI_FIELDER_H
#define GAME_AI_FIELDER_H

#include "Game/Player.h"

enum eFielderActionState
{
    ACTION_NEED_ACTION = -1,
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
    void SetAction(eFielderActionState actionState);
    bool IsStriker() const;
    bool IsWinger() const;
    bool IsMidField() const;
    bool IsDefense() const;

private:
    /* 0x328 */ u8 mUnknown328[0x08];
    /* 0x330 */ u8 mUnknown330[0xFC];
    /* 0x42C */ bool m_bHasBeenUpdated;
    /* 0x42D */ u8 mUnknown42D[3];
    /* 0x430 */ eFielderActionState m_eActionState;
    /* 0x434 */ u8 mUnknown434[0x2C];
    /* 0x460 */ eRole m_eRole;
    /* 0x464 */ u8 mUnknown464[0x10];
    /* 0x474 */ bool mbWasHitByPowerupThisFrame;
    /* 0x475 */ u8 mUnknown475[7];
    /* 0x47C */ ShotMeter* m_pShotMeter;
}; // total size: 0x480

#endif // GAME_AI_FIELDER_H
