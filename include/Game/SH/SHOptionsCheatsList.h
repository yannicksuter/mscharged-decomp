#ifndef GAME_SH_SHOPTIONSCHEATSLIST_H
#define GAME_SH_SHOPTIONSCHEATSLIST_H

#include "Game/BaseSceneHandler.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class SHOptionsCheatsList : public BaseSceneHandler
{
public:
    SHOptionsCheatsList();
    virtual ~SHOptionsCheatsList();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnCheatPointerInside(int index, void* context);
    void OnCheatPointerLeave(int index, void* context);
    void UpdateCheatText(int item);
    void InitializeButtons();
    void OnCheatPointerEnter(int index, void* context);
    void OnCheatPointerPress(int index, void* context);

    /* 0x01C */ FEPointerButton mComponents[5];
    /* 0x3A0 */ FEScrollBar mScrollWidget;
    /* 0x554 */ TLComponentInstance* mCheatInstances[5];
    /* 0x568 */ bool mButtonsInitialized;
    /* 0x569 */ unsigned char mPadding569[3];
    /* 0x56C */ FEBackButton mNavigation;
    /* 0x644 */ int mPointerInsideCounts[4];
    /* 0x654 */ unsigned char mSavedNavigationButtons;
    /* 0x655 */ unsigned char mPadding655[3];
    /* 0x658 */ int mState;
    /* 0x65C */ int mFirstVisibleCheat;
    /* 0x660 */ PowerupSettings* mSettings;
    /* 0x664 */ int mCheatCategory;
}; // size 0x668

#endif // GAME_SH_SHOPTIONSCHEATSLIST_H
