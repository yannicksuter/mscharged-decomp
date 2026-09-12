#ifndef UNCLASSIFIED_TU_80245F04_H
#define UNCLASSIFIED_TU_80245F04_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feScrollBar.h"

struct NetworkDraftMachineInfo;

class TU80245F04Scene : public BaseSceneHandler
{
public:
    TU80245F04Scene();
    virtual ~TU80245F04Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_802460D8();
    void fn_802466C0();
    void fn_8024671C();

    struct PlayerMapping
    {
        bool mGuest;
        s8 mTeam;
        NetworkDraftMachineInfo* mMachine;
    };

    /* 0x01C */ int mPlayerCount;
    /* 0x020 */ PlayerMapping mPlayerMappings[4];
    /* 0x040 */ bool mIntroFinished;
    /* 0x044 */ int mCountdown;
    /* 0x048 */ FEScrollBar mScrollBar;
    /* 0x1FC */ bool mErrorPopupOpen;
    /* 0x200 */ TLComponentInstance* mPlayerInstances[4];
    /* 0x210 */ u16 mPlayerNameBuffers[4][0x20];
    /* 0x310 */ u16 mPlayerDescriptionBuffers[4][0x30];
    /* 0x490 */ u16 mCountdownBuffer[8];
    /* 0x4A0 */ FEOnlinePlayerRow mPlayers[4];
private:
    void UpdatePlayerRows();
    void UpdateTimer(int countdown);
    void ShowDisconnectedError();
}; // size 0x700

#endif // UNCLASSIFIED_TU_80245F04_H
