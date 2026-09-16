#ifndef GAME_FE_OVERLAY_HANDLER_CHALLENGE_PREVIEW_H
#define GAME_FE_OVERLAY_HANDLER_CHALLENGE_PREVIEW_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

class ChallengePreviewOverlay : public BaseOverlayHandler
{
public:
    ChallengePreviewOverlay(ScreenMovement movement);
    virtual ~ChallengePreviewOverlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnContinuePointerEnter(int index, void* context);
    void OnContinuePointerLeave(int index, void* context);
    void OnContinuePointerPress(int index, void* context);
    inline void InitializeContinueButton();

    /* 0x028 */ FEPointerButton mContinueButton;
    /* 0x0DC */ TLComponentInstance* mContinueButtonInstance;
    /* 0x0E0 */ ScreenMovement mMovement;
    /* 0x0E4 */ bool mButtonInitialized;
    /* 0x0E5 */ bool mContinuePressed;
    /* 0x0E6 */ u16 mTextBuffers[4][0x80];
    /* 0x4E6 */ u8 mPadding4E6[2];
    /* 0x4E8 */ int mState;
}; // size 0x4EC

#endif // GAME_FE_OVERLAY_HANDLER_CHALLENGE_PREVIEW_H
