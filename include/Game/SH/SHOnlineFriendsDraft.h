#ifndef GAME_SH_SHONLINEFRIENDSDRAFT_H
#define GAME_SH_SHONLINEFRIENDSDRAFT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feScrollBar.h"

struct NetworkDraftMachineInfo;

class SHOnlineFriendsDraft : public BaseSceneHandler
{
public:
    SHOnlineFriendsDraft();
    virtual ~SHOnlineFriendsDraft();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateDraftStatuses();
    void OnErrorDismissed();
    void InitializePlayerRows();

    struct DraftPlayerMapping
    {
        bool mIsGuest;
        s8 mTeamIndex;
        NetworkDraftMachineInfo* mMachineInfo;
    };

    /* 0x01C */ int mPlayerCount;
    /* 0x020 */ DraftPlayerMapping mDraftPlayers[4];
    /* 0x040 */ bool mIntroComplete;
    /* 0x044 */ int mCountdownSeconds;
    /* 0x048 */ FEScrollBar mScrollBar;
    /* 0x1FC */ bool mErrorPopupOpen;
    /* 0x200 */ TLComponentInstance* mPlayerRowInstances[4];
    /* 0x210 */ u16 mRankText[4][0x20];
    /* 0x310 */ u16 mRecordText[4][0x30];
    /* 0x490 */ u16 mCountdownText[8];
    /* 0x4A0 */ FEOnlinePlayerRow mPlayerRows[4];
private:
    void RefreshPlayerRows();
    void UpdateCountdown(int countdown);
    void ShowDisconnectedError();
}; // size 0x700

#endif // GAME_SH_SHONLINEFRIENDSDRAFT_H
