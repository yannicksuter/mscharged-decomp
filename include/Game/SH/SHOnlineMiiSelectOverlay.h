#ifndef GAME_SH_ONLINE_MII_SELECT_OVERLAY_H
#define GAME_SH_ONLINE_MII_SELECT_OVERLAY_H

#include <RVLFaceLib/RFL_DataUtility.h>

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
struct FEPageControls;

class SHOnlineMiiSelectOverlay : public BaseSceneHandler
{
public:
    SHOnlineMiiSelectOverlay();
    virtual ~SHOnlineMiiSelectOverlay();
    void InitializeButtons();
    void SelectOption(unsigned int, void*);
    bool SelectMii();
    void DeleteSaveSlot();
    void CancelDeleteSaveSlot();
    void ReturnToMiiSelect();
    void ReturnToWiiMenu();
    void SanitizeMiiName(unsigned short* name);
    void OpenItem(unsigned int index, void* context);
    void CloseItem(unsigned int index, void* context);
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x1C */ unsigned char mPadding1C[4];
    /* 0x20 */ int mHoverCounts[4];
    /* 0x30 */ bool mInitialized;
    /* 0x31 */ bool mSavePending;
    /* 0x32 */ unsigned char mPadding32[2];
    /* 0x34 */ RFLAdditionalInfo mMiiInfo;
    /* 0x70 */ int mOfficialIndex;
    /* 0x74 */ int mIconIndex;
    /* 0x78 */ unsigned long long mCreateID;
    /* 0x80 */ int mSaveSlot;
    /* 0x84 */ unsigned short mDateText[0x40];
    /* 0x104 */ unsigned short mNameText[0x40];
    /* 0x184 */ unsigned short mRecordText[0x40];
    /* 0x204 */ FEPointerButton mButtons[2];
    /* 0x36C */ FEBackButton mBackButton;
    /* 0x444 */ FEPageControls* mPageControls;
    /* 0x448 */ TLComponentInstance* mButtonInstances[2];
}; // size 0x450

#endif // GAME_SH_ONLINE_MII_SELECT_OVERLAY_H
