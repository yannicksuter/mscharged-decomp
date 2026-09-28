#ifndef GAME_SH_SH_CHOOSE_SIDES_H
#define GAME_SH_SH_CHOOSE_SIDES_H

#include "Game/BaseGameSceneManager.h"
#include "NL/nlColour.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class TLImageInstance;

class SHChooseSides2 : public BaseSceneHandler
{
public:
    enum eCSContext
    {
        FRIENDLY = 0,
        CUP = 1,
        SUPERCUP = 2,
        TOURNAMENT = 3,
        PAUSE = 4,
    };

    SHChooseSides2(eCSContext context, ScreenMovement movement);
    virtual ~SHChooseSides2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void LeaveScene();
    void BindChooseSideInstances();
    void OnControllerPointerEnter(unsigned int index, void* context);
    void OnControllerPointerLeave(unsigned int index, void* context);
    void OnControllerPointerPress(unsigned int index, void* context);
    void OnHomeAwayPointerEnter(unsigned int index, void* context);
    void OnHomeAwayPointerInside(unsigned int index, void* context);
    void OnHomeAwayPointerLeave(unsigned int index, void* context);
    void OnHomeAwayPointerPress(unsigned int index, void* context);
    void Proceed();
    void OnHelpPointerEnter(unsigned int index, void* context);
    void OnHelpPointerLeave(unsigned int index, void* context);
    void OnHelpPointerPress(unsigned int index, void* context);
    void ReleaseController(int index);
    void UpdateHomeAwayVisibility();
    void SetSidekickImage(TLImageInstance* image, int sidekick, int team);
    bool RemoveDisconnectedControllers(bool playSound);

    /* 0x01C */ bool mUnidentified1C;
    /* 0x01D */ bool mUnidentified1D;
    /* 0x01E */ bool mUnidentified1E;
    /* 0x01F */ bool mUnidentified1F;
    /* 0x020 */ FEPointerButton mControllerComponents[2];
    /* 0x188 */ FEPointerButton mHomeAwayComponent;
    /* 0x23C */ FEPointerButton mHelpComponent;
    /* 0x2F0 */ FEBackButton mUnidentified2F0;
    /* 0x3C8 */ ScreenMovement mMovement;
    /* 0x3CC */ TLComponentInstance* mSideGroups[2];
    /* 0x3D4 */ TLComponentInstance* mHomeAwayBox;
    /* 0x3D8 */ TLComponentInstance* mHelpButton;
    /* 0x3DC */ eCSContext mContext;
    /* 0x3E0 */ int mPlayingSides[4];
    /* 0x3F0 */ nlColour mUnidentified3F0[2];
    /* 0x3F8 */ int mControllerCounts[4];
    /* 0x408 */ int mUnidentified408;
    /* 0x40C */ int mState;
}; // size 0x410

#endif // GAME_SH_SH_CHOOSE_SIDES_H
