#ifndef GAME_SH_ONLINE_FRIEND_CODE_ENTRY_H
#define GAME_SH_ONLINE_FRIEND_CODE_ENTRY_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

enum eFriendCodeEntryPhase
{
    FRIEND_CODE_ENTRY_ENTERING = 0,
    FRIEND_CODE_ENTRY_ACTIVE = 1,
    FRIEND_CODE_ENTRY_TRANSITIONING = 2,
    FRIEND_CODE_ENTRY_EXITING_BACK = 3,
};

class SHOnlineFriendCodeEntry : public BaseSceneHandler
{
public:
    SHOnlineFriendCodeEntry();
    virtual ~SHOnlineFriendCodeEntry();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void InitializeButtons();
    void OnKeypadPointerEnter(int index, void* context);
    void OnKeypadPointerLeave(int index, void* context);
    void OnDigitPointerPress(int index, void* context);
    void OnDigitPointerEnter(int index, void* context);
    void OnDigitPointerLeave(int index, void* context);
    void RestoreFriendCodeInput();
    void UpdateConfirmButton();
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    void OnAddFriendErrorDismissed();
#endif
    void SetDigit(int index, const wchar_t* text);
    void ClearDigit(int index);
    void SetDigitText(int index, const wchar_t* text);
    void ClearDigitTexts();
    void SetSelectedDigit(int index);
    unsigned long long ParseFriendKey();
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    void ShowAddFriendError(int error);
#endif
    void OnKeypadPointerPress(int index, void* context);

    /* 0x001C */ int mHoverCount;
    /* 0x0020 */ bool mButtonsInitialized;
    /* 0x0021 */ unsigned char mPadding0021[3];
    /* 0x0024 */ int mSelectedDigit;
    /* 0x0028 */ unsigned short mDigits[12][2];
    /* 0x0058 */ FEPointerButton mKeypadButtons[12];
    /* 0x08C8 */ FEPointerButton mDigitButtons[12];
    /* 0x1138 */ FEBackButton mBackButton;
    /* 0x1210 */ TLComponentInstance* mKeypadInstances[12];
    /* 0x1240 */ TLComponentInstance* mDigitInstances[12];
    /* 0x1270 */ TLComponentInstance* mOutConfirmButtonInstance;
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    /* 0x1274 */ bool mPopupActive;
    /* 0x1275 */ unsigned char mPadding1275[3];
#endif
    /* 0x1278 (USA/Japan/Korea), 0x1274 (Europe) */ int mState;
}; // size 0x127C (USA/Japan/Korea), 0x1278 (Europe)

#endif // GAME_SH_ONLINE_FRIEND_CODE_ENTRY_H
