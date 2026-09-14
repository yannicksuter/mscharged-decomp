#ifndef GAME_SH_ONLINE_INVITE_RESPONSE_H
#define GAME_SH_ONLINE_INVITE_RESPONSE_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

class SHOnlineInviteResponse : public BaseSceneHandler
{
public:
    SHOnlineInviteResponse();
    virtual ~SHOnlineInviteResponse();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    void InitializeButtons();
    void OnPointerEnter(unsigned int index, void* context);
    void OnPointerLeave(unsigned int index, void* context);
    void OnPointerPress(unsigned int index, void* context);
    void ApplySelectedAction();

    /* 0x01C */ FEPointerButton mButtons[4];
    /* 0x2EC */ TLComponentInstance* mButtonInstances[4];
    /* 0x2FC */ unsigned short mInvitationText[0x80];
    /* 0x3FC */ int mHoverCounts[4];
    /* 0x40C */ int mSelectedAction;
    /* 0x410 */ bool mButtonsInitialized;
    /* 0x414 */ int mState;
}; // size 0x418

void JoinOnlineFriendInvitation();

#endif // GAME_SH_ONLINE_INVITE_RESPONSE_H
