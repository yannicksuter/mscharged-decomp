#ifndef _SHPAUSEPOSTGAME_H_
#define _SHPAUSEPOSTGAME_H_

#include "Game/FE/feInput.h"
#include "Game/FE/feTimer.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "unclassified/tu_80209584.h"

class PausePostGameScene : public SHStrikerTimesBase
{
public:
    PausePostGameScene(int);
    virtual ~PausePostGameScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void OnDoneTransitionComplete();

    void fn_801EDBF8(FETimer* timer);
    void fn_801EE180();
    void fn_801EE6A8();
    static void OnSelectRematch();
    static void OnSelectQuit();
    static void OnSelectChangeTeams();

    /* 0x5D4 */ int mUnidentified5D4;
    /* 0x5D8 */ eFEINPUT_PAD mUnidentified5D8;
    /* 0x5DC */ bool mUnidentified5DC;
    /* 0x5DE */ u16 mUnidentified5DE[8];
    /* 0x5F0 */ FETimer mTimer;
    /* 0x60C */ bool mUnidentified60C;
    /* 0x60D */ bool mUnidentified60D;
    /* 0x60E */ u8 mPadding60E[2];
    /* 0x610 */ int mUnidentified610;
    /* 0x614 */ TU80209584Summary mSummary;
}; // size 0xA48

void fn_801EDC90(bool online);

#endif // _SHPAUSEPOSTGAME_H_
