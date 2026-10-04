#ifndef GAME_SH_SH_CHOOSE_SIDEKICKS_H
#define GAME_SH_SH_CHOOSE_SIDEKICKS_H

#include "Game/SH/SHChooseCaptains.h"
#include "Game/FE/feCaptainComponent.h"
#include "Game/DB/StatsTracker.h"

class AsyncImage;
class TLComponentInstance;
class FETextureResource;

class ChooseSidekicksSceneV2 : public BaseSceneHandler
{
public:
    ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::SceneType sceneType, ScreenMovement movement);
    virtual ~ChooseSidekicksSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    int GetSide(unsigned long pad);
    eTeamID GetTeam(int side) const
    {
        eTeamID team = (eTeamID)mTeams[side];
        return team;
    }
    void UpdateDraftTimer(int value);
    void OnSidekickPointerPress(int index, void* context);
    void OnSidekickPointerEnter(int index, void* context);
    void OnSidekickPointerLeave(int index, void* context);
    void OnSlotPointerPress(int index, void* context);
    void OnSlotPointerEnter(int index, void* context);
    void OnSlotPointerLeave(int index, void* context);
    void OnSlotPointerInside(int index, void* context);
    void OnRandomPointerPress(int index, void* context);
    void OnRandomPointerEnter(int index, void* context);
    void OnRandomPointerLeave(int index, void* context);
    void OnSelectPointerPress(int index, void* context);
    void OnSelectPointerLeave(int index, void* context);
    void OnDonePointerPress(int index, void* context);
    void OnDonePointerEnter(int index, void* context);
    void OnSelectPointerEnter(int index, void* context);
    void OnDonePointerLeave(int index, void* context);
    void OnDonePointerInside(int index, void* context);
    void InitializeSidekickButtons();
    void InitializePointerButtons();
    void CommitSidekickChoices();
    void UpdatePointerCursors();
    void LoadSidekickTextures();
    void ApplySidekickTextures();
    void SubmitSidekickChoice();
    void UpdateSlotButtons();
    void LoadSidekickImages(int firstCaptain, int secondCaptain);
    void UpdateAsyncImages();
    bool AreImagesLoaded();
    void OnDisconnectDismissed();
    void ShowDisconnectedError();
    void ReleaseController(int index);

    /* 0x001C */ bool mPopupActive;
    /* 0x001D */ u8 mPadding1D[3];
    /* 0x0020 */ int mSidePads[2];
    /* 0x0028 */ int mHoveredSidekicks[2];
    /* 0x0030 */ int mSelectedSlots[2];
    /* 0x0038 */ ScreenMovement mMovement;
    /* 0x003C */ ChooseCaptainsSceneV2::SceneType mSceneType;
    /* 0x0040 */ int mTeams[2];
    /* 0x0048 */ bool mReadyPressed[2];
    /* 0x004A */ bool mPointerButtonsInitialized;
    /* 0x004B */ bool mSidekickButtonsInitialized;
    /* 0x004C */ bool mSidekicksShown;
    /* 0x004D */ bool mSelectionMade;
    /* 0x004E */ u8 mPadding4E[2];
    /* 0x0050 */ FECaptainComponent mSidekickComponents[2];
    /* 0x00A0 */ bool mSlotClickEnabled[2][3];
    /* 0x00A6 */ u8 mPaddingA6[2];
    /* 0x00A8 */ FETextureResource* mSidekickTextures[8][2];
    /* 0x00E8 */ FEPointerButton mSidekickButtons[8];
    /* 0x0688 */ FEPointerButton mSlotButtons[2][3];
    /* 0x0AC0 */ FEPointerButton mSlotClickButtons[2][3];
    /* 0x0EF8 */ FEPointerButton mRandomButtons[2];
    /* 0x1060 */ FEPointerButton mSelectButtons[2];
    /* 0x11C8 */ FEPointerButton mDoneButton;
    /* 0x127C */ FEBackButton mBackButton;
    /* 0x1354 */ FECharacterPDAComponent mCaptainComponents[2];
    /* 0x18AC */ TLComponentInstance* mSidekickInstances[8];
    /* 0x18CC */ TLComponentInstance* mSlotInstances[2][3];
    /* 0x18E4 */ TLComponentInstance* mSlotClickInstances[2][3];
    /* 0x18FC */ TLComponentInstance* mRandomButtonInstances[2];
    /* 0x1904 */ TLComponentInstance* mSelectButtonInstances[2];
    /* 0x190C */ TLInstance* mGreenArrows[2];
    /* 0x1914 */ TLComponentInstance* mSidekicksLayer;
    /* 0x1918 */ TLComponentInstance* mDoneButtonInstance;
    /* 0x191C */ u8 mUnidentified191C[0x20];
    /* 0x193C */ int mDraftCountdown;
    /* 0x1940 */ unsigned short mTimerText[8];
    /* 0x1950 */ bool mDraftExitDone;
    /* 0x1951 */ u8 mPadding1951[3];
    /* 0x1954 */ int mState;
    /* 0x1958 */ AsyncImage* mAttributeImages[2][8];
    /* 0x1998 */ AsyncImage* mPositionImages[2][8];
    /* 0x19D8 */ bool mAttributesLoaded[2][8];
    /* 0x19E8 */ bool mPositionsLoaded[2][8];
    /* 0x19F8 */ bool mImagesLoaded;
    /* 0x19F9 */ u8 mPadding19F9[3];
}; // size 0x19FC

#endif // GAME_SH_SH_CHOOSE_SIDEKICKS_H
