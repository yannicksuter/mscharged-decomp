#ifndef GAME_SH_SH_CHOOSE_CAPTAINS_H
#define GAME_SH_SH_CHOOSE_CAPTAINS_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feCharacterPDAComponent.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;
class TLInstance;
class TLImageInstance;
class FETextureResource;

class ChooseCaptainsSceneV2 : public BaseSceneHandler
{
public:
    enum SceneType
    {
        ST_DOMINATION = 0,
        ST_STRIKER_CUP = 1,
    };

    enum ScenePhase
    {
        PHASE_ENTERING = 0,
        PHASE_CHOOSING = 1,
        PHASE_EXITING_FORWARD = 2,
        PHASE_EXITING_BACK = 3,
    };

    ChooseCaptainsSceneV2(SceneType sceneType, ScreenMovement movement);
    virtual ~ChooseCaptainsSceneV2();
    virtual void Update(float dt);
    virtual void SceneCreated();

    int GetSide(unsigned long pad);
    void LoadCaptainTextures();
    void RefreshCaptainImages();
    void OnCaptainPointerPress(int index, void* context);
    void UpdateDraftTimer(int countdown);
    void OnCaptainPointerInside(int index, void* context);
    void OnCaptainPointerEnter(int index, void* context);
    void OnCaptainPointerLeave(int index, void* context);
    void OnSelectPointerEnter(int index, void* context);
    void OnSelectPointerInside(int index, void* context);
    void OnReadyPointerPress(int index, void* context);
    void OnReadyPointerEnter(int index, void* context);
    void OnReadyPointerLeave(int index, void* context);
    void OnReadyPointerInside(int index, void* context);
    void OnDonePointerPress(int index, void* context);
    void UpdateSelectText(int which);
    void OnDisconnectDismissed();
    void OnDonePointerInside(int index, void* context);
    void OnSelectPointerLeave(int index, void* context);
    void OnDonePointerEnter(int index, void* context);
    void OnDonePointerLeave(int index, void* context);
    void InitializeCaptainButtons();
    void InitializePointerButtons();
    void OnSelectPointerPress(int index, void* context);
    void UpdatePointerCursors();
    void UpdateDoneButton();
    void ReleaseController(int index);
    void SetSelectButtonBounds(int side);
    void ShowDisconnectedError();

    /* 0x001C */ bool mPopupActive;
    /* 0x001D */ u8 mPadding1D[3];
    /* 0x0020 */ SceneType mSceneType;
    /* 0x0024 */ ScreenMovement mMovement;
    /* 0x0028 */ int mSidePads[2];
    /* 0x0030 */ int mSelectedCaptains[2];
    /* 0x0038 */ bool mConfirmed[2];
    /* 0x003A */ bool mReadyPressed[2];
    /* 0x003C */ bool mChangeTextShown[2];
    /* 0x003E */ u8 mPadding3E[2];
    /* 0x0040 */ int mCaptainIds[2];
    /* 0x0048 */ bool mSideJoined[2];
    /* 0x004A */ bool mCaptainButtonsInitialized;
    /* 0x004B */ bool mPointerButtonsInitialized;
    /* 0x004C */ bool mCaptainsShown;
    /* 0x004D */ bool mBothConfirmed;
    /* 0x004E */ bool mInputSuppressed;
    /* 0x004F */ u8 mPadding4F;
    /* 0x0050 */ FETextureResource* mCaptainTextures[12][2];
    /* 0x00B0 */ FEPointerButton mCaptainButtons[12];
    /* 0x0920 */ FEPointerButton mSelectButtons[2];
    /* 0x0A88 */ FEPointerButton mReadyButtons[2];
    /* 0x0BF0 */ FEPointerButton mDoneButton;
    /* 0x0CA4 */ FEBackButton mBackButton;
    /* 0x0D7C */ FECharacterPDAComponent mCaptainComponents[2];
    /* 0x12D4 */ TLComponentInstance* mCaptainInstances[12];
    /* 0x1304 */ TLComponentInstance* mSelectButtonInstances[2];
    /* 0x130C */ TLComponentInstance* mOkButtonInstances[2];
    /* 0x1314 */ TLComponentInstance* mDoneButtonInstance;
    /* 0x1318 */ TLInstance* mGreenArrows[2];
    /* 0x1320 */ TLComponentInstance* mCaptainsLayer;
    /* 0x1324 */ TLComponentInstance* mPDALayers[2];
    /* 0x132C */ TLComponentInstance* mSelectDisplays[2];
    /* 0x1334 */ TLComponentInstance* mCupPDALayer;
    /* 0x1338 */ TLImageInstance* mCaptainImages[12];
    /* 0x1368 */ int mDraftCountdown;
    /* 0x136C */ unsigned short mTimerText[8];
    /* 0x137C */ bool mDraftExitDone;
    /* 0x137D */ u8 mPadding137D[3];
    /* 0x1380 */ int mScenePhase;
}; // size 0x1384

bool IsLocalDraftPad(int pad);

#endif // GAME_SH_SH_CHOOSE_CAPTAINS_H
