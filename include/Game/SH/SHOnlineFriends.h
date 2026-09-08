#ifndef GAME_SH_SHONLINEFRIENDS_H
#define GAME_SH_SHONLINEFRIENDS_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feBackButton.h"

class TLTextInstance;

class SHOnlineFriends : public BaseSceneHandler
{
public:
    SHOnlineFriends();
    virtual ~SHOnlineFriends();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void UpdateScrollRange();
    void UpdateFriend(int index);
    static int CompareFriendStatus(const void* a, const void* b);
    void InitializeButtons();
    void OnPointerPress(int index, void* context);
    void OnPointerEnter(int index, void* context);
    void OnPointerLeave(int index, void* context);
    void DeleteFriend(int index);
    void CancelDeleteFriend();
    void UpdateFriendCode();
    void OnDialogDismissed();
    void OnErrorDismissed();
    void UpdateAddFriendRow();
    void UpdateVisibleRows();

    /* 0x001C */ int mUnidentified001C;
    /* 0x0020 */ int mUnidentified0020;
    /* 0x0024 */ int mUnidentified0024;
    /* 0x0028 */ int mUnidentified0028;
    /* 0x002C */ int mUnidentified002C;
    /* 0x0030 */ bool mUnidentified0030;
    /* 0x0031 */ bool mUnidentified0031;
    /* 0x0034 */ float mUnidentified0034;
    /* 0x0038 */ FEPointerButton mUnidentified0038[4];
    /* 0x0308 */ FEScrollBar mUnidentified0308;
    /* 0x04BC */ FEBackButton mUnidentified04BC;
    /* 0x0594 */ TLTextInstance* mUnidentified0594;
    /* 0x0598 */ u16 mUnidentified0598[64];
    /* 0x0618 */ TLComponentInstance* mUnidentified0618[4];
    /* 0x0628 */ u16 mUnidentified0628[4][32];
    /* 0x0728 */ u16 mUnidentified0728[4][48];
    /* 0x08A8 */ FEOnlinePlayerRow mUnidentified08A8[64];
    /* 0x2EA8 */ FEOnlinePlayerRow* mUnidentified2EA8[64];
    /* 0x2FA8 */ bool mUnidentified2FA8;
    /* 0x2FAC */ int mUnidentified2FAC;
}; // size 0x2FB0

#endif // GAME_SH_SHONLINEFRIENDS_H
