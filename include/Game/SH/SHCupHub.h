#ifndef GAME_SH_SH_CUP_HUB_H
#define GAME_SH_SH_CUP_HUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class TLTextInstance;
class FEPageControls;

class CupHubScene : public BaseSceneHandler
{
public:
    CupHubScene();
    virtual ~CupHubScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateRows();
    void UpdateRoundText(int index, int value);
    void UpdateRow(int index);
    void UpdateRoundGameText(TLTextInstance* roundText, int round, TLTextInstance* gameText, int game, int index);
    void InitializePointerButtons();
    void OnMatchupPointerPress(unsigned int index, void* context);
    void OnMatchupPointerEnter(unsigned int index, void* context);
    void OnMatchupPointerLeave(unsigned int index, void* context);
    void OnMatchupPointerInside(unsigned int index, void* context);
    void OnRulesPointerEnter(unsigned int index, void* context);
    void OnRulesPointerLeave(unsigned int index, void* context);
    void OnRulesPointerPress(unsigned int index, void* context);
    void BuildMatchupStates();

    /* 0x01C */ TLComponentInstance* mMovingHighlight;
    /* 0x020 */ TLComponentInstance* mRowInstances[4];
    /* 0x030 */ FEPointerButton mMatchupComponents[4];
    /* 0x300 */ bool mInitialized;
    /* 0x304 */ int mScrollOffset;
    /* 0x308 */ FEScrollBar mScrollWidget;
    /* 0x4BC */ u16 mTextBuffers[4][16];
    /* 0x53C */ u16 mGameText[4][16];
    /* 0x5BC */ u16 mTitleBuffer[64];
    /* 0x63C */ u16 mScoreText[4][2][4];
    /* 0x67C */ bool mSuppressInput;
    /* 0x67D */ bool mPagingEnabled;
    /* 0x67E */ bool mPreviousPagePressed;
    /* 0x67F */ bool mNextPagePressed;
    /* 0x680 */ int mHoverCounts[4];
    /* 0x690 */ FEBackButton mNavigationComponent;
    /* 0x768 */ FEPageControls* mPageControls;
    /* 0x76C */ FEPointerButton mRulesComponent;
    /* 0x820 */ TLComponentInstance* mRulesButton;
    /* 0x824 */ s8 mMatchupStates[54][2];
    /* 0x890 */ u8 mEntryCount;
    /* 0x894 */ int mState;
}; // size 0x898

#endif // GAME_SH_SH_CUP_HUB_H
