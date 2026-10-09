#ifndef GAME_SH_SHPAUSE_H
#define GAME_SH_SHPAUSE_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feButtonComponent.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePointerButton.h"

enum ePauseMenuAction
{
    PAUSE_RESUME = 0,
    PAUSE_CHOOSE_SIDES = 1,
    PAUSE_AUDIO_OPTIONS = 2,
    PAUSE_VISUAL_OPTIONS = 3,
    PAUSE_MATCH_INFO = 4,
    PAUSE_QUIT = 5,
    PAUSE_CONTROLLER_MAP = 6,
};

class PauseMenuScene : public BaseSceneHandler
{
public:
    enum TransitionType
    {
        TT_INVALID = -1,
        TT_IN = 0,
        TT_OUT = 1,
        TT_CHOOSE_SIDES = 2,
        TT_AUDIO_OPTIONS = 3,
        TT_VISUAL_OPTIONS = 4,
        TT_CHALLENGE_PREVIEW = 5,
        TT_STATISTICS = 6,
        TT_CONTROLLER_MAP = 7,
    };

    PauseMenuScene();
    virtual ~PauseMenuScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    void OnSelectQUIT();
    void OnSelectPopupNOFORFEIT();
    void OnSelectPopupYESFORFEIT();
    void OnSelectRESUME(TLComponentInstance* instance);
    void TransitionOut(TransitionType newtype);
    void InitializePointerButtons();
    void OnOptionPointerEnter(unsigned int index, void* context);
    void OnOptionPointerLeave(unsigned int index, void* context);
    void OnOptionPointerPress(unsigned int index, void* context);

    /* 0x01C */ bool mGameIsOver;
    /* 0x020 */ float mQuitDelay;
    /* 0x024 */ eFEINPUT_PAD mQuittingController;
    /* 0x028 */ TLComponentInstance* mOptionInstances[7];
    /* 0x044 */ FEPointerButton mOptionButtons[7];
    /* 0x530 */ bool mInitialized;
    /* 0x534 */ int mHoverCounts[4];
    /* 0x544 */ TransitionType mTransitionTo;
    /* 0x548 */ bool mIsInTransition;
    /* 0x549 */ bool mStartAnimAtEnd;
    /* 0x54A */ bool mSelectionMade;
    /* 0x54B */ bool mIntroSoundPlayed;
    /* 0x54C */ ButtonComponent mButtons;
    /* 0x570 */ ButtonComponent mButtons2;

    static eFEINPUT_PAD mControllingInput;
    static float mDelayBeforeUnpause;
    static s32 mLastSelectedIndex;
}; // size 0x594

#endif // GAME_SH_SHPAUSE_H
