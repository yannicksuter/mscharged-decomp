#include "Game/NetworkMessageRegistry.h"
#include "NL/plat/SocketNetwork.h"
#include "Game/Task/NetworkUpdateTask.h"

#include "Game/InputRouter.h"

#include "Game/Task/FixedUpdateTask.h"
#include "Game/InputManager.h"

#include "Game/NetTournManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/FriendManager.h"
#include "Game/main.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"

#include "NL/nlMemory.h"
#include "types.h"

extern u8 lbl_806E1008;

void RegisterNetworkMessages_801258A8();
void NetworkUpdateTask::Initialize()
{
    SocketNetworkInitializeMemory();
    NetworkSession::Create();
    InitializeNetworkSyncState();
    InitializeNetworkInputRecording();
    InitializeNetworkMessageRegistry();
    RegisterNetworkMessages_801258A8();
    InitializeInputManager();

    InputFrameProvider* handler = GetFixedUpdateTask();
    gInputManager->SetFrameProvider(handler);

    InitializeInputRouters();
    NetTournManager::CreateInstance();
    NetworkDraft::CreateInstance();

    if (g_pFriendManager == 0)
    {
        void* instance
            = nlMalloc(sizeof(FriendManager), 8, false);
        g_pFriendManager
            = new (instance) FriendManager();
    }

    NetworkStatsManager::CreateInstance();
    if (GetRegion() == 2)
    {
        g_nAddHoursTime = 9;
    }
    else if (GetRegion() == 0)
    {
        g_nAddHoursTime = -8;
    }

    lbl_806E1008 = 0;
    gInputManager->mEnabled = 1;
    gNetworkSyncState->mEnabled = false;
}

void NetworkUpdateTask::Run(float)
{
    g_pNetworkSessionBase->Update();
}
