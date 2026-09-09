#ifndef GAME_SH_ONLINE_DRAFT_H
#define GAME_SH_ONLINE_DRAFT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feScrollBar.h"

class SHOnlineMatchmakingDraft : public BaseSceneHandler
{
public:
    SHOnlineMatchmakingDraft();
    virtual ~SHOnlineMatchmakingDraft();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateDraftTeams();
    void UpdateDraftStatuses();
    void OnErrorDismissed();

    /* 0x01C */ int mPlayerCount;
    /* 0x020 */ bool mScrollingEnabled;
    /* 0x024 */ int mScrollOffset;
    /* 0x028 */ int mScrollRange;
    /* 0x02C */ bool mDraftStarted;
    /* 0x030 */ int mConnectionCount;
    /* 0x034 */ bool mCanCancel;
    /* 0x038 */ int mReturnScene;
    /* 0x03C */ bool mIntroFinished;
    /* 0x040 */ int mCountdown;
    /* 0x044 */ bool mErrorPopupOpen;
    /* 0x048 */ FEScrollBar mScrollWidget;
    /* 0x1FC */ FEBackButton mBackButton;
    /* 0x2D4 */ TLComponentInstance* mPlayerInstances[4];
    /* 0x2E4 */ u16 mPlayerNameBuffers[4][0x20];
    /* 0x3E4 */ u16 mPlayerDescriptionBuffers[4][0x30];
    /* 0x564 */ u16 mCountdownBuffer[8];
    /* 0x574 */ FEOnlinePlayerRow mPlayers[8];
}; // size 0xA34

#endif // GAME_SH_ONLINE_DRAFT_H
