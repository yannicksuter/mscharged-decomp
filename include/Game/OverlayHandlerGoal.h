#ifndef _OVERLAYHANDLERGOAL_H_
#define _OVERLAYHANDLERGOAL_H_

#include "Game/FE/BaseOverlayHandler.h"

struct GoalScoredData;
struct UnidentifiedEventData_80067308;

class GoalOverlay : public BaseOverlayHandler
{
public:
    GoalOverlay();
    virtual ~GoalOverlay();
    virtual void SceneCreated();
    virtual void Update(float fDeltaT);

    void fn_801F178C(GoalScoredData* data);
    void fn_801F17D0(int homeAway);
    void fn_801F17F4(UnidentifiedEventData_80067308* data);
    void Restart();
    void Reset();
    void UpdateGoalInfo(int homeAway, int playerIndex, bool isCaptainS2S, int numGoals);
    void SetHighlightNumber(int highlight);
    void DoMatchEndOverlay();
    void SetWinnerTitle();
    void DoCupWinOverlay(int cup);

    /* 0x028 */ int mCaptainGoals[2];
    /* 0x030 */ int mSidekickGoals[2];
    /* 0x038 */ u16 mClockBuffer[32];
    /* 0x078 */ u16 mDescriptionBuffer[128];
    /* 0x178 */ u16 mScoresBuffer[128];
    /* 0x278 */ bool mHasSniperCup;
    /* 0x279 */ bool mIsCreated;
    /* 0x27A */ bool mIsInOvertime;
}; // size 0x27C

#endif // _OVERLAYHANDLERGOAL_H_
