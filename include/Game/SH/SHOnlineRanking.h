#ifndef GAME_SH_SHONLINE_RANKING_H
#define GAME_SH_SHONLINE_RANKING_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
struct FEPageControls;

class SHOnlineRanking : public BaseSceneHandler
{
public:
    SHOnlineRanking();
    virtual ~SHOnlineRanking();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateRows();
    void SelectLeaderboardCategory();
    bool PopulateRow(int row, int leaderboardIndex);
    void OnRowPointerEnter(int index, void* context);
    void OnRowPointerLeave(int index, void* context);
    void OnRowPointerPress(int index, void* context);
    void UpdateHeader();
    void OnErrorDismissed();

    /* 0x001C */ TLComponentInstance* mUnidentified1C;
    /* 0x0020 */ int mPointerHoverCounts[4];
    /* 0x0030 */ bool mUnidentified30;
    /* 0x0031 */ u8 mPadding31[3];
    /* 0x0034 */ int mMyRank;
    /* 0x0038 */ int mFirstVisibleRank;
    /* 0x003C */ u16 mRowText[10][5][40];
    /* 0x0FDC */ u8 mUnidentifiedFDC[0x80];
    /* 0x105C */ int mLeaderboardCategory;
    /* 0x1060 */ int mLeaderboardSelection;
    /* 0x1064 */ FEPointerButton mRowButtons[10];
    /* 0x176C */ FEScrollBar mScrollBar;
    /* 0x1920 */ FEBackButton mBackButton;
    /* 0x19F8 */ FEPageControls* mPageControls;
    /* 0x19FC */ int mTransitionState;
    /* 0x1A00 */ TLComponentInstance* mRowInstances[10];
    /* 0x1A28 */ TLComponentInstance* mHighlight;
    /* 0x1A2C */ u8 mUnidentified1A2C[0x18];
    /* 0x1A44 */ bool mErrorPopupOpen;
}; // size 0x1A48

#endif // GAME_SH_SHONLINE_RANKING_H
