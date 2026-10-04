#ifndef GAME_EXCITEMENT_SYSTEM_H
#define GAME_EXCITEMENT_SYSTEM_H

#include "Game/InterpreterCore.h"

struct CollisionBallGoalpostData;
struct PlayerAttackData;
struct LightningStrikeData;

class ExcitementSystem : public InterpreterCore
{
public:
    static ExcitementSystem& Instance();

    void ClearExcitementValues();
    void RegisterEventHandlers();
    void OnAttackSuccess(PlayerAttackData*);
    void OnLightningStrike(LightningStrikeData*);
    void OnCollisionBallGoalpost(CollisionBallGoalpostData*);
    virtual void DoFunctionCall(unsigned int);

    /* 0x028 */ float mMaxBallDistanceSq;
    /* 0x02C */ u16 mExcitement;
    /* 0x02E */ u16 mExcitementCount;
    /* 0x030 */ u8 mFielderAnimExcitement[130];
    /* 0x0B2 */ u8 mGoalieAnimExcitement[178];
    /* 0x164 */ u8 mEventExcitement[4];
    /* 0x168 */ void* mByteCode;

private:
    ExcitementSystem();
    void LoadScript();
}; // total size: 0x16C

#endif // GAME_EXCITEMENT_SYSTEM_H
