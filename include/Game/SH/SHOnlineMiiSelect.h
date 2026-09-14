#ifndef GAME_SH_ONLINE_MII_SELECT_H
#define GAME_SH_ONLINE_MII_SELECT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

struct FEPageControls;

class SHOnlineMiiSelect : public BaseSceneHandler
{
public:
    SHOnlineMiiSelect();
    virtual ~SHOnlineMiiSelect();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    void ClearMissingMiiSaveSlots();
    void BuildMiiList();
    void UpdatePage();
    void InitializeButtons();
    void OpenItem(unsigned int index, void* context);
    void CloseItem(unsigned int index, void* context);
    void SelectMii(unsigned int, void* context);
    void ReturnToWiiMenu();

    /* 0x01C */ int mMiiCount;
    /* 0x020 */ int mHoverCounts[4];
    /* 0x030 */ bool mInitialized;
    /* 0x031 */ unsigned char mPadding31[3];
    /* 0x034 */ int mPageCount;
    /* 0x038 */ int mCurrentPage;
    /* 0x03C */ bool mSuppressPageInput;
    /* 0x03D */ unsigned char mPadding3D[3];
    /* 0x040 */ int mOfficialIndices[100];
    /* 0x1D0 */ unsigned short mPageText[24];
    /* 0x200 */ FEPointerButton mMiiButtons[10];
    /* 0x908 */ FEBackButton mBackButton;
    /* 0x9E0 */ FEPageControls* mPageControls;
    /* 0x9E4 */ TLComponentInstance* mMiiInstances[10];
}; // size 0xA0C

#endif // GAME_SH_ONLINE_MII_SELECT_H
