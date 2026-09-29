#ifndef _SHPAUSEPOSTGAME_H_
#define _SHPAUSEPOSTGAME_H_

#include "Game/FE/feInput.h"
#include "Game/FE/feTimer.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "Game/FE/MatchSummary.h"

class PausePostGameScene : public SHStrikerTimesBase
{
public:
    PausePostGameScene(int);
    virtual ~PausePostGameScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void OnDoneTransitionComplete();

    void OnCountdownTick(FETimer* timer);
    void UpdateSummaryDisplay();
    void BuildStoryArticle();
    static void OnSelectRematch();
    static void OnSelectQuit();
    static void OnSelectChangeTeams();

    /* 0x5D4 */ int mMode;
    /* 0x5D8 */ eFEINPUT_PAD mControllingInput;
    /* 0x5DC */ bool mIsMultiplayer;
    /* 0x5DE */ u16 mCountdownText[8];
    /* 0x5F0 */ FETimer mTimer;
    /* 0x60C */ bool mTimerTicked;
    /* 0x60D */ bool mSummaryDisplayed;
    /* 0x60E */ u8 mPadding60E[2];
    /* 0x610 */ int mCountdownSeconds;
    /* 0x614 */ MatchSummary mSummary;
}; // size 0xA48

void ContinuePostGame(bool online);

#endif // _SHPAUSEPOSTGAME_H_
