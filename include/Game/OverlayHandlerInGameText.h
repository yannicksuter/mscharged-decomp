#ifndef GAME_OVERLAY_HANDLER_IN_GAME_TEXT_H
#define GAME_OVERLAY_HANDLER_IN_GAME_TEXT_H

#include "Game/FE/BaseOverlayHandler.h"
#include "types.h"

enum OverlaySlideName
{
    SLIDE_NAME_INVALID = -1,
    SLIDE_NAME_TEXT_GOAL = 0,
    SLIDE_NAME_TEXT_KICKOFF = 1,
    SLIDE_NAME_TEXT_WINNER = 2,
    SLIDE_NAME_TEXT_PAUSE = 3,
    SLIDE_NAME_TEXT_TIE = 4,
    SLIDE_NAME_TEXT_LOADING = 5,
    SLIDE_NAME_TEXT_SHOOT = 6,
    SLIDE_NAME_TEXT_REPLAY = 7,
    NUM_SLIDE_NAMES = 8,
};

struct InGameTextEntry
{
    /* 0x0 */ OverlaySlideName mOverlayEnum;
    /* 0x4 */ const char* mSlideName;
    /* 0x8 */ u32 mTaskVisibility;
}; // size 0xC

class InGameTextOverlay : public BaseOverlayHandler
{
public:
    InGameTextOverlay();
    virtual ~InGameTextOverlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetSlide(OverlaySlideName slideName);
    void DisplayFinalScore();

    /* 0x28 */ OverlaySlideName mCurrentSlideName;
    /* 0x2C */ OverlaySlideName mPendingSlideName;
    /* 0x30 */ u16 mScoresBuffer[32];
    /* 0x70 */ u16 mWinnerBuffer[32];
}; // size 0xB0

#endif // GAME_OVERLAY_HANDLER_IN_GAME_TEXT_H
