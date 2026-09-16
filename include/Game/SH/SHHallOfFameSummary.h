#ifndef GAME_SH_SHHALLOFFAMESUMMARY_H
#define GAME_SH_SHHALLOFFAMESUMMARY_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feBackButton.h"

struct FEPageControls;

class SHHallOfFameSummary : public BaseSceneHandler
{
public:
    SHHallOfFameSummary(int mode);
    virtual ~SHHallOfFameSummary();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateTitle();
    void UpdateRows();
    void UpdateRow(int index, const char* stringId, bool unlocked);

    /* 0x01C */ int mMode;
    /* 0x020 */ FEBackButton mBackButton;
    /* 0x0F8 */ FEPageControls* mPageControls;
    /* 0x0FC */ FEScrollBar mScrollBar;
    /* 0x2B0 */ unsigned short mTitleBuffer[0x40];
    /* 0x330 */ unsigned short mItemTextBuffers[7][0x20];
    /* 0x4F0 */ unsigned char mUnidentified4F0[0x1C0];
    /* 0x6B0 */ int mPointerInsideCount[4];
    /* 0x6C0 */ int mItemCount;
    /* 0x6C4 */ int mFirstVisibleItem;
    /* 0x6C8 */ bool mInitialized;
    /* 0x6C9 */ bool mNextPageRequested;
    /* 0x6CA */ bool mPreviousPageRequested;
    /* 0x6CB */ unsigned char mPadding6CB;
    /* 0x6CC */ int mState;
}; // size 0x6D0


#endif // GAME_SH_SHHALLOFFAMESUMMARY_H
