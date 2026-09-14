#ifndef GAME_SH_SHONLINEHUB_H
#define GAME_SH_SHONLINEHUB_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feButtonComponent.h"
#include "Game/NetworkStats.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;

class SHOnlineHub : public BaseSceneHandler
{
public:
    SHOnlineHub();
    virtual ~SHOnlineHub();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void OnPointerPress(unsigned int index, void* context);
    void OnDialogDismissed();
    void OnErrorDismissed();
    void UpdateFriendAndSeasonText();
    void UpdateLocalStats();
    void UpdateStrikerOfTheDay();
    void InitializeButtons();
    void OnPointerEnter(unsigned int index, void* context);
    void OnPointerLeave(unsigned int index, void* context);

    /* 0x01C */ u32 mUnidentified01C;
    /* 0x020 */ FEPointerButton mUnidentified020[4];
    /* 0x2F0 */ TLComponentInstance* mUnidentified2F0[4];
    /* 0x300 */ FEPointerButton mUnidentified300;
    /* 0x3B4 */ TLComponentInstance* mUnidentified3B4;
    /* 0x3B8 */ FEBackButton mUnidentified3B8;
    /* 0x490 */ ButtonComponent mUnidentified490;
    /* 0x4B4 */ bool mUnidentified4B4;
    /* 0x4B8 */ int mUnidentified4B8[4];
    /* 0x4C8 */ u16 mUnidentified4C8[48];
    /* 0x528 */ u16 mUnidentified528[48];
    /* 0x588 */ float mUnidentified588;
    /* 0x58C */ bool mUnidentified58C;
    /* 0x58D */ bool mUnidentified58D;
    /* 0x590 */ NetworkRankingMeta mUnidentified590;
    /* 0x5A8 */ NetworkRankingMeta mUnidentified5A8;
    /* 0x5C0 */ NetworkStatsPlayer mUnidentified5C0;
    /* 0x628 */ NetworkRankingMeta mUnidentified628;
    /* 0x640 */ u16 mUnidentified640[24];
    /* 0x670 */ u16 mUnidentified670[48];
    /* 0x6D0 */ u16 mUnidentified6D0[48];
    /* 0x730 */ u16 mUnidentified730[48];
    /* 0x790 */ u16 mUnidentified790[128];
    /* 0x890 */ int mUnidentified890;
    /* 0x894 */ int mUnidentified894;
}; // size 0x898

#endif // GAME_SH_SHONLINEHUB_H
