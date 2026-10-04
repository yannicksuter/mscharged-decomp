#ifndef GAME_FE_ONLINE_RANKING_H
#define GAME_FE_ONLINE_RANKING_H

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feTimer.h"

class OnlineRankingOverlay : public BaseOverlayHandler
{
public:
    OnlineRankingOverlay();
    virtual ~OnlineRankingOverlay();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void OnCountdownTick(FETimer* timer);
    void UpdateResultsDisplay();

    /* 0x026 */ unsigned short mTimerText[16];
    /* 0x046 */ unsigned short mScoreText[16];
    /* 0x066 */ unsigned short mScorePointsText[32];
    /* 0x0A6 */ unsigned short mGoalsText[16];
    /* 0x0C6 */ unsigned short mGoalPointsText[32];
    /* 0x106 */ u8 mUnidentified106[0x40];
    /* 0x146 */ unsigned short mTotalPointsText[32];
    /* 0x188 */ FETimer mCountdownTimer;
    /* 0x1A4 */ bool mCountdownTicked;
    /* 0x1A5 */ bool mPlayingOutSlide;
    /* 0x1A6 */ bool mResultsDisplayed;
    /* 0x1A7 */ u8 mPadding1A7;
    /* 0x1A8 */ int mCountdownSeconds;
    /* 0x1AC */ int mTotalPoints;
    /* 0x1B0 */ bool mWon;
    /* 0x1B1 */ bool mTied;
    /* 0x1B4 */ int mScorePoints;
    /* 0x1B8 */ int mGoalPoints;
    /* 0x1BC */ int mBonusPoints;
}; // size 0x1C0

#endif // GAME_FE_ONLINE_RANKING_H
