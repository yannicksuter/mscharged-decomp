#include "Game/Task/NetworkUpdateTask.h"

#include "Game/NetTournManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/tu_801360A4.h"
#include "Game/main.h"

#include "NL/nlMemory.h"
#include "types.h"

struct NetworkManager
{
    u8 padding[4];
    u8 enabled;
};

struct NetworkStatus
{
    u8 padding[0xD];
    u8 enabled;
};

extern NetworkManager* lbl_806E2138;
extern NetworkStatus* lbl_806E2168;
extern u8 lbl_806E1008;

extern "C" void fn_803740B8();
extern "C" void fn_80338898();
extern "C" void fn_80337F68();
extern "C" void fn_8032C7D0();
void RegisterNetworkMessages_801258A8();
extern "C" void fn_803327DC();
extern "C" void* fn_8011166C();
extern "C" void fn_803328AC(NetworkManager*, void*);
extern "C" void fn_80332EDC();
void NetworkUpdateTask::Initialize()
{
    fn_803740B8();
    UnidentifiedNetworkSession::Create();
    fn_80338898();
    fn_80337F68();
    fn_8032C7D0();
    RegisterNetworkMessages_801258A8();
    fn_803327DC();

    void* handler = fn_8011166C();
    if (handler != 0)
    {
        handler = static_cast<u8*>(handler) + 0x20;
    }
    fn_803328AC(lbl_806E2138, handler);

    fn_80332EDC();
    NetTournManager::CreateInstance();
    NetworkDraft::CreateInstance();

    if (lbl_806E1194 == 0)
    {
        void* instance
            = nlMalloc(sizeof(UnidentifiedFriendManager_801360A4), 8, false);
        lbl_806E1194
            = new (instance) UnidentifiedFriendManager_801360A4();
    }

    NetworkStatsManager_8012F378::CreateInstance();
    if (GetRegion() == 2)
    {
        g_nAddHoursTime = 9;
    }
    else if (GetRegion() == 0)
    {
        g_nAddHoursTime = -8;
    }

    lbl_806E1008 = 0;
    lbl_806E2138->enabled = 1;
    lbl_806E2168->enabled = 0;
}

void NetworkUpdateTask::Run(float)
{
    lbl_806E20D8->Update();
}
