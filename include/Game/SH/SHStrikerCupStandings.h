#ifndef GAME_SH_SHSTRIKERCUPSTANDINGS_H
#define GAME_SH_SHSTRIKERCUPSTANDINGS_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class FEPageControls;

class StrikerCupStandingsScene : public BaseSceneHandler
{
public:
    StrikerCupStandingsScene();
    virtual ~StrikerCupStandingsScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnButtonPointerEnter(unsigned int index, void* context);
    void OnButtonPointerLeave(unsigned int index, void* context);
    void OnButtonPointerPress(unsigned int index, void* context);
    void OnTeamPointerEnter(unsigned int index, void* context);
    void OnTeamPointerLeave(unsigned int index, void* context);
    void UpdateStandings();
    bool PopulateTeamRow(int row, int teamPosition);
    void InitializePointerButtons();

    /* 0x01C */ TLComponentInstance* mTeamRows[5];
    /* 0x030 */ TLComponentInstance* mWhiteLine;
    /* 0x034 */ TLComponentInstance* mHighlight;
    /* 0x038 */ TLComponentInstance* mMovingHighlight;
    /* 0x03C */ int mScrollOffset;
    /* 0x040 */ int mUserTeamPosition;
    /* 0x044 */ int mStandingsIndices[8];
    /* 0x064 */ unsigned char mPadding64[8];
    /* 0x06C */ unsigned short mStatText[5][7][4];
    /* 0x184 */ unsigned short mTitleText[64];
    /* 0x204 */ bool mPointerButtonsInitialized;
    /* 0x205 */ bool mBracketPressed;
    /* 0x206 */ bool mPreviousPagePressed;
    /* 0x207 */ bool mNextPagePressed;
    /* 0x208 */ int mPointerHoverCounts[4];
    /* 0x218 */ FEScrollBar mScrollBar;
    /* 0x3CC */ FEBackButton mBackButton;
    /* 0x4A4 */ FEPageControls* mPageControls;
    /* 0x4A8 */ FEPointerButton mHelpButton;
    /* 0x55C */ FEPointerButton mBracketButton;
    /* 0x610 */ FEPointerButton mTeamButtons[5];
    /* 0x994 */ unsigned char mPadding994[0x14];
    /* 0x9A8 */ TLComponentInstance* mBracketButtonInstance;
    /* 0x9AC */ TLComponentInstance* mHelpButtonInstance;
    /* 0x9B0 */ int mTransitionState;
}; // size 0x9B4

#endif // GAME_SH_SHSTRIKERCUPSTANDINGS_H
