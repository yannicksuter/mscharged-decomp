#include "NL/plat/SocketNetwork.h"
#include "Game/Task/NetworkUpdateTask.h"

#include "unclassified/tu_80332DC0.h"

#include "Game/Task/FixedUpdateTask.h"
#include "unclassified/tu_80332770.h"

#include "Game/NetTournManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/FriendManager.h"
#include "Game/main.h"
#include "unclassified/tu_80336B2C.h"
#include "unclassified/tu_80338898.h"

#include "NL/nlMemory.h"
#include "types.h"

extern u8 lbl_806E1008;

void RegisterNetworkMessages_801258A8();
void NetworkUpdateTask::Initialize()
{
    SocketNetworkInitializeMemory();
    NetworkSession::Create();
    fn_80338898();
    fn_80337F68();
    fn_8032C7D0();
    RegisterNetworkMessages_801258A8();
    fn_803327DC();

    UnidentifiedFixedUpdateTaskBase* handler = GetFixedUpdateTask();
    lbl_806E2138->fn_803328AC(handler);

    fn_80332EDC();
    NetTournManager::CreateInstance();
    NetworkDraft::CreateInstance();

    if (g_pFriendManager == 0)
    {
        void* instance
            = nlMalloc(sizeof(FriendManager), 8, false);
        g_pFriendManager
            = new (instance) FriendManager();
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
    lbl_806E2138->mEnabled = 1;
    lbl_806E2168->mEnabled = false;
}

void NetworkUpdateTask::Run(float)
{
    g_pNetworkSessionBase->Update();
}
