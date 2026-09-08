#ifndef GAME_SH_SHONLINEMATCHMAKINGDRAFT_H
#define GAME_SH_SHONLINEMATCHMAKINGDRAFT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feBackButton.h"

class SHOnlineMatchmakingDraft : public BaseSceneHandler
{
public:
    SHOnlineMatchmakingDraft();
    virtual ~SHOnlineMatchmakingDraft();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void UpdateDraftTeams();
    void UpdateDraftStatuses();
    void OnErrorDismissed();

    /* 0x01C */ int mUnidentified01C;
    /* 0x020 */ bool mUnidentified020;
    /* 0x024 */ int mUnidentified024;
    /* 0x028 */ int mUnidentified028;
    /* 0x02C */ bool mUnidentified02C;
    /* 0x030 */ int mUnidentified030;
    /* 0x034 */ bool mUnidentified034;
    /* 0x038 */ int mUnidentified038;
    /* 0x03C */ bool mUnidentified03C;
    /* 0x040 */ int mUnidentified040;
    /* 0x044 */ bool mUnidentified044;
    /* 0x048 */ FEScrollBar mUnidentified048;
    /* 0x1FC */ FEBackButton mUnidentified1FC;
    /* 0x2D4 */ TLComponentInstance* mUnidentified2D4[4];
    /* 0x2E4 */ u16 mUnidentified2E4[4][32];
    /* 0x3E4 */ u16 mUnidentified3E4[4][48];
    /* 0x564 */ u16 mUnidentified564[8];
    /* 0x574 */ FEOnlinePlayerRow mUnidentified574[8];

private:
    bool CanCancelMatchmaking();
    int GetRemainingDraftTime();
    void UpdateTimerText(int time);
}; // size 0xA34

#endif // GAME_SH_SHONLINEMATCHMAKINGDRAFT_H
