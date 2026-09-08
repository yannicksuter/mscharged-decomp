#ifndef GAME_SH_HALL_OF_FAME_HISTORY_H
#define GAME_SH_HALL_OF_FAME_HISTORY_H

#include "Game/BaseSceneHandler.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feScrollBar.h"

class AsyncImage;

class SHHallOfFameHistory : public BaseSceneHandler
{
public:
    SHHallOfFameHistory(int mode);
    virtual ~SHHallOfFameHistory();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateTitle();
    void UpdateCupRecordText();
    void UpdateGoalsRecordText();
    void UpdateDateText();
    void UpdateTeamDisplay();

    /* 0x01C */ int mMode;
    /* 0x020 */ FEBackButton mNavigation;
    /* 0x0F8 */ FEScrollBar mScrollWidget;
    /* 0x2AC */ CupHistoryRecord mHistory[12];
    /* 0x30C */ int mSelectedHistoryIndex;
    /* 0x310 */ int mHistoryCount;
    /* 0x314 */ int mUnidentified314[4];
    /* 0x324 */ bool mUnidentified324;
    /* 0x325 */ unsigned char mPadding325[3];
    /* 0x328 */ int mState;
    /* 0x32C */ unsigned short mTitleText[0x40];
    /* 0x3AC */ unsigned short mRecordText[0x80];
    /* 0x4AC */ unsigned short mDateText[0x20];
    /* 0x4EC */ unsigned short mTeamNameText[0x20];
    /* 0x52C */ AsyncImage* mImages[5];
    /* 0x540 */ bool mImageReady[5];
    /* 0x545 */ unsigned char mPadding545[3];
}; // size 0x548

#endif // GAME_SH_HALL_OF_FAME_HISTORY_H
