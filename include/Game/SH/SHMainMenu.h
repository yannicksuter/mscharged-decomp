#ifndef GAME_SH_SH_MAIN_MENU_H
#define GAME_SH_SH_MAIN_MENU_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class TLInstance;
class TLImageInstance;
class TLTextInstance;

class SHMainMenu : public BaseSceneHandler
{
public:
    SHMainMenu();
    virtual ~SHMainMenu();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void InitializeMenuItems();
    void CloseItem(unsigned int index, void* context);
    void ReturnToWiiMenu();
    void OpenItem(unsigned int index, void* context);
    void SelectItem(unsigned int index, void* context);
    void ApplyItem(unsigned int item);

    static const int NUM_ITEMS = 7;

    /* 0x01C */ TLComponentInstance* mMenuItemInstances[NUM_ITEMS];
    /* 0x038 */ TLComponentInstance* mMenuHighlights[NUM_ITEMS];
    /* 0x054 */ TLComponentInstance* mMenuArrows[NUM_ITEMS];
    /* 0x070 */ TLInstance* mMenuGroups[NUM_ITEMS];
    /* 0x08C */ TLImageInstance* mMenuHitImages[NUM_ITEMS];
    /* 0x0A8 */ TLTextInstance* mItemDescription;
    /* 0x0AC */ FEPointerButton mMenuItems[NUM_ITEMS];
    /* 0x598 */ FEBackButton mBackButton;
    /* 0x670 */ bool mMenuInitialized;
    /* 0x671 */ bool mItemSelected;
    /* 0x674 */ int mHighlightedItemCounts[4];
    /* 0x684 */ bool mWaitingForLoad;
}; // size 0x688

#endif // GAME_SH_SH_MAIN_MENU_H
