#include "NL/nlSingleton.inl"
#include "NL/nlFunction.inl"
#include "Game/TweakQuery.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/NetworkMessageRegistry.h"
#include "NL/plat/SocketNetwork.h"
#include <revolution/os/OSThread_fwd.h>
#include <dwc/dwc_init_fwd.h>
#include <dwc/dwc_report_fwd.h>
#include <dwc/dwc_nastime_fwd.h>
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/FE/feDPD.h"
#include "Game/OnlinePlayer.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/FE/FEAudio.h"
#include "NL/nlFunctionMemory.h"
#include "Game/Event.h"
#include "Game/Sys/debug.h"
#include "Game/NetworkRandom.h"
#include "Game/FE/feMusic.h"
#include "Game/FriendManager.h"

#include "Game/AI/AIPad.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetTournManager.h"
#include "Game/TrophyInfo.h"
#include "Game/Task/ResetTask.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkStatsManager.h"

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlTicker.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/InputRouter.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"
#include "Game/SH/SHOnlineFriendsChooseSides.h"
#include "Game/SH/SHOnlineConnectionQuality.h"
#include "Game/UnidentifiedStaticStorage.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

bool g_bNoPopupNetworkError;
NetworkSession* g_pNetworkSession;
DWCFriendsMatchControl gDWCFriendsMatchControl;
void* gNetworkMemoryPool;
int gNetworkBuildNumberOverride;
int lbl_806E10FC;

float gNetworkLoginTimeout = 180.0f;
u32 gNetworkGameCodeR4QP = 0x52345150;
u32 gNetworkGameCodeR4QJ = 0x5234514A;
u32 gNetworkGameCodeR4QE = 0x52345145;

extern NetworkSessionBase* g_pNetworkSessionBase;

static MemoryAllocator s_NetworkAllocator;

#include <string.h>
extern float kInitialNetworkUpdateDelta;
extern float kNetworkMillisecondsPerSecond;

struct UnidentifiedVersionInfo
{
    /* 0x0 */ u32 mVersionWord;
    /* 0x4 */ u8 mUnidentified04;
};

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Game.h"

typedef Detail::MemFunImpl<void, void (NetworkSession::*)()>
    NetworkSessionCallback;
typedef BindExp1<void, NetworkSessionCallback, NetworkSession*>
    NetworkSessionBinding;

extern BaseGameSceneManager* g_pGameSceneManager;

static inline void PushAllocator(MemoryAllocator* pAllocator)
{
    CurrentAllocator = pAllocator;
    AllocatorStack[AllocatorStackDepth++] = pAllocator;
}

static inline void PopAllocator()
{
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void NetworkSession::Create()
{
    NetworkSession* session = new NetworkSession;
    g_pNetworkSession = session;
    g_pNetworkSessionBase = session;
}

void NetworkSession::Initialize(bool first)
{
    if (first)
    {
        this->InitializeMachines(1, 4);

        mSessionMode = 0;
        mSessionState = 0;
        mLoginStage = 0;
        mGameEndReason = 0;
        mDirectSocket = 0;
        mTransport = 0;
        mLobby = 0;
        mElapsedTime = 0.0f;
        mUpdateCount = 0;
        mLastTicker = 0;
        mCupMode = 0;

        PushAllocator(&VirtualAllocator);
        if (GetRegion() == 0)
        {
            u32 poolSize = 0xB0000 - 0x2A90;
            void* pool = nlMalloc(poolSize, 8, false);
            gNetworkMemoryPool = pool;
            s_NetworkAllocator.Initialize(pool, poolSize);
        }
        else
        {
            u32 poolSize = 0x80000 - 0x37D0;
            void* pool = nlMalloc(poolSize, 8, false);
            gNetworkMemoryPool = pool;
            s_NetworkAllocator.Initialize(pool, poolSize);
        }
        PopAllocator();

        mUnidentified24A5 = 0;
        mUnidentified24A4 = 0;
        mDWCErrorCode = 0;
        mDWCErrorType = DWC_ERROR_NONE;
        mDWCLastError = 0;
        mUnidentified2494 = 0;
        mLoginListener = 0;
        mLoginStartTime = 0.0f;

        mDirectSocket = new NetworkSocket;
        mTransport = new LANLobby;
        mLobby = new NetworkLobby;
        mStatsReporter = new NetworkStatsReporter;
        mRankingReporter = new NetworkRanking;
    }
    else
    {
        this->InitializeMachines(1, 4);
    }

    mMachineLoadedGame[0] = 0;
    mMachineLoadedGame[1] = 0;
    mMachineLoadedGame[2] = 0;
    mMachineLoadedGame[3] = 0;
    mUnidentified2472 = 0;
    mPauseRequestMachineMask = 0;
    mPausedMachineMask = 0;
    mUnidentified247C = 1;
    mUnidentified2474 = 0;
    mUnidentified2478 = 0;
    mOverlayRequest = 3;
    mPoppedOverlay = 3;
}

void NetworkSession::SendTournamentStartToEveryone()
{
    int machineCount = GetMachineRoster()->GetMachineCount();
    u32 cupRandom = NetworkRandom();
    s32 cupPersona = cupRandom % 10;
    if (s_nOverrideCupPersona != 10)
    {
        cupPersona = s_nOverrideCupPersona;
    }

    int first = (s8)gCupPersonaStadiums[cupPersona][0];
    int second = (s8)gCupPersonaStadiums[cupPersona][1];
    if (first == -1)
    {
        first = NetworkRandom() & 0xF;
    }
    if (second == -1)
    {
        second = NetworkRandom() & 0xF;
    }
    if (NetworkRandom() % 1000 >= 500)
    {
        int swap = first;
        first = second;
        second = swap;
    }

    NetMessageTournamentStart message;
    message.mMachineIndex = 0;
    message.mMachineCount = machineCount;
    message.mCupPersona = cupPersona;
    message.mFirstStadium = first;
    message.mSecondStadium = second;
    message.mSeedings[0] = 0;
    message.mSeedings[1] = 1;
    message.mSeedings[2] = 2;
    message.mSeedings[3] = 3;
    message.mSeedings[4] = 4;
    message.mSeedings[5] = 5;
    message.mSeedings[6] = 6;
    message.mSeedings[7] = 7;
    NetTournManager::GenerateFirstRoundSeedings(
        machineCount, message.mSeedings);

    for (int machine = 0; machine < machineCount; ++machine)
    {
        u8 buffer[0x64];
        message.mMachineIndex = machine;
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0x64);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendTournamentStartToEveryone to %d because no "
                "connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendGameStartToEveryone()
{
    u32 randomSeed = NetworkRandom();
    u32 second = NetworkRandom();
    u32 third = NetworkRandom();
    int machineCount = GetMachineRoster()->GetMachineCount();

    NetMessageGameStart message;
    message.mRandomSeed = randomSeed;
    message.mMachineIndex = 0;
    message.mMachineCount = machineCount;
    message.mUnidentified20 = second;
    message.mUnidentified24 = third;

    NetworkDraftTeam* home = NetworkDraft::Instance()->GetDraftTeam(0);
    message.mHomeCharacters[0] = home->mCaptain;
    message.mHomeCharacters[1] = home->mSidekick0;
    message.mHomeCharacters[2] = home->mSidekick1;
    message.mHomeCharacters[3] = home->mSidekick2;

    NetworkDraftTeam* away = NetworkDraft::Instance()->GetDraftTeam(1);
    message.mAwayCharacters[0] = away->mCaptain;
    message.mAwayCharacters[1] = away->mSidekick0;
    message.mAwayCharacters[2] = away->mSidekick1;
    message.mAwayCharacters[3] = away->mSidekick2;

    if (IsOnlineRankedMatch())
    {
        message.mStadium = NetworkRandom() & 0xF;
        if (GetTweakBool("/user/media_build", 0))
        {
            while (!IsStadiumUnlocked(message.mStadium))
            {
                message.mStadium = NetworkRandom() & 0xF;
            }
        }
    }
    else
    {
        message.mStadium = GameInfoManager::GetInstance()->GetStadium();
    }

    u8 remote = gOnlineTwoLocalPlayers;
    for (int machine = 0; machine < 4; ++machine)
    {
        if (machine < machineCount)
        {
            if (remote)
            {
                message.mMachineFlags[machine] = 2;
            }
            else
            {
                message.mMachineFlags[machine] = 1;
            }
        }
        else
        {
            message.mMachineFlags[machine] = 0;
        }
    }

    message.mUnidentified1B = 0;
    message.mUnidentified1C[0] = -1;
    message.mUnidentified1C[1] = -1;
    message.mUnidentified1C[2] = -1;

    for (int machine = 0; machine < machineCount; ++machine)
    {
        u8 buffer[0x64];
        message.mMachineIndex = machine;
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0x64);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendGameStartToEveryone to %d because no "
                "connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendDraftToEveryone(
    int count, NetworkDraftMachineInfo* entries, bool unused, u8 flag)
{
    NetMessageDraft message;
    message.mMachineCount = count;
    message.mUnidentified0A = flag;

    for (int entry = 0; entry < count; ++entry)
    {
        message.mEntries[entry] = entries[entry];
    }

    for (int machine = 0; machine < count; ++machine)
    {
        u8 buffer[0x200];
        message.mMachineIndex = machine;
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0x200);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendDraftToEveryone to %d because no connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendDraftToEveryone(NetMessageDraft* message)
{
    for (int machine = 0; machine < message->mMachineCount; ++machine)
    {
        u8 buffer[0x200];
        message->mMachineIndex = machine;
        u32 size = gNetworkMessageRegistry->Serialize(message, buffer, 0x200);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendDraftToEveryone to %d because no connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendSidesChangedToEveryone(
    NetworkMessage* message)
{
    int machineCount = GetMachineRoster()->GetMachineCount();
    for (int machine = 0; machine < machineCount; ++machine)
    {
        u8 buffer[0xFF];
        u32 size = gNetworkMessageRegistry->Serialize(message, buffer, 0xFF);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendSidesChangedToEveryone to %d because no "
                "connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendSidesChangedToHost(
    NetworkMessage* message)
{
    u8 buffer[0xFF];
    u32 size = gNetworkMessageRegistry->Serialize(message, buffer, 0xFF);
    u32 aid = GetMachineRoster()->GetMachineAid(0);
    if (aid == 0xFFFFFFFF)
    {
        mDirectSocket->Receive(buffer, size);
    }
    else if (aid == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to SendSidesChangedToHost because no connection\n");
    }
    else
    {
        mDirectSocket->Send(aid, buffer, size, true);
    }
}

void NetworkSession::SendCheckConnectionToEveryone()
{
    NetMessageCheckConnection message;
    for (int machine = 0; machine < mLobby->GetPlayerCount(); ++machine)
    {
        message.mProfileIds[machine] =
            mLobby->GetMachineInfo(machine)->mProfileId;
    }

    for (int machine = 0; machine < mLobby->GetPlayerCount(); ++machine)
    {
        u8 buffer[0xFF];
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0xFF);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendCheckConnectionToEveryone to %d because no "
                "connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendConnectionDecisionToEveryone(
    NetworkMessage* message)
{
    for (int machine = 0; machine < GetMachineRoster()->GetMachineCount();
         ++machine)
    {
        u8 buffer[0xFF];
        u32 size = gNetworkMessageRegistry->Serialize(message, buffer, 0xFF);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to SendConnectionDecisionToEveryone to %d because no "
                "connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void NetworkSession::SendConnectionDecisionToHost(
    NetworkMessage* message)
{
    u8 buffer[0xFF];
    u32 size = gNetworkMessageRegistry->Serialize(message, buffer, 0xFF);
    u32 aid = GetMachineRoster()->GetMachineAid(0);
    if (aid == 0xFFFFFFFF)
    {
        mDirectSocket->Receive(buffer, size);
    }
    else if (aid == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to SendConnectionDecisionToHost because no connection\n");
    }
    else
    {
        mDirectSocket->Send(aid, buffer, size, true);
    }
}

static inline void PopupNetworkErrorOverlay(
    NetworkSession* session, int overlay)
{
    if (g_bNoPopupNetworkError != 0)
    {
        return;
    }
    if (session->mPoppedOverlay != 3)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Ignored overlayRequest %d because already popped overlay %d\n",
            overlay, session->mPoppedOverlay);
        return;
    }
    session->mPoppedOverlay = overlay;
    if (NetTournManager::Instance()->mState != 0)
    {
        NetTournManager::Instance()->NotifyOverlayPopped(overlay);
    }
}

void NetworkSession::Update()
{
    if (!SocketNetworkIsStarted())
    {
        return;
    }

    float dt;
    if (mLastTicker == 0)
    {
        dt = kInitialNetworkUpdateDelta;
    }
    else
    {
        dt = nlGetTickerDifference(mLastTicker, nlGetTicker()) / kNetworkMillisecondsPerSecond;
    }

    u32 ticker = nlGetTicker();
    mElapsedTime += dt;
    mLastTicker = ticker;
    mUpdateCount++;

    if (mSessionMode == 1)
    {
        mDirectSocket->Update(dt);
        mTransport->Update(dt);
        NetTournManager::Instance()->Update(dt);
        NetworkDraft::Instance()->Update(dt);
        mStatsReporter->Update();

        if (mTransport->mState == 3)
        {
            mTransport->CompleteLaunch();
            if (mTransport->GetPlayerCount() >= 2)
            {
                NetworkDraftMachineInfo entries[8];
                for (int player = 0;
                     player < mTransport->GetPlayerCount(); ++player)
                {
                    TransportPlayerInfo* info =
                        mTransport->GetPlayerInfo(player);
                    nlStrToWcs(info->mName, entries[player].mName, 0xB);
                    memset(entries[player].mMiiData, 0, 0x4C);
                    entries[player].mMachineIndex = player;
                    entries[player].mStats.mScore = 0;
                    entries[player].mStats.mDisplayRank = 0;
                    entries[player].mStats.mWins = 0;
                    entries[player].mStats.mLosses = 0;
                    entries[player].mStats.mUnidentified14 = 0;
                    entries[player].mStats.mDisplayRank =
                        info->mUnidentified0C;
                    entries[player].mStats.mWins =
                        info->mUnidentified10;
                    entries[player].mStats.mLosses =
                        info->mUnidentified12;
                }
                SendDraftToEveryone(mTransport->GetPlayerCount(), entries,
                                    false, 0);
            }
        }
    }
    else if (mSessionMode == 2)
    {
        if (mSessionState != 0 && mUnidentified2494 == 0
            && !mLobby->mMatchmakingThreadRunning)
        {
            DWC_ProcessFriendsMatch();
        }

        mLobby->Update(dt);
        mDirectSocket->Update(dt);
        NetTournManager::Instance()->Update(dt);
        NetworkDraft::Instance()->Update(dt);
        g_pFriendManager->Update(dt);
        mRankingReporter->Update();
        NetworkStatsManager::Instance()->Update(dt);

        if (GetSessionState() == 1)
        {
            UpdateLogin();
        }

        if (mLobby->mGameStarted == 0 && mLobby->GetPlayerCount() > 0)
        {
            mLobby->MarkGameStarted();
            SendCheckConnectionToEveryone();
        }
    }

    if (mOverlayRequest != 3 && mUnidentified2472 != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Finished pending load, now can popup network error overlay "
            "%d\n",
            mOverlayRequest);
        NetworkStatsManager::Instance()->CalculateAndReportGameResult(4);
        PopupNetworkErrorOverlay(this, mOverlayRequest);
        mOverlayRequest = 3;
    }
}

static void* NetworkAlloc(DWCAllocType name, unsigned long size, int align)
{
    return s_NetworkAllocator.Allocate(size, align, false);
}

static void NetworkFree(DWCAllocType name, void* ptr, unsigned long size)
{
    if (ptr == 0)
    {
        return;
    }
    s_NetworkAllocator.Free(ptr);
}

unsigned int GetNetworkVersionWord()
{
    int channel = 10;
    switch (GetRegion())
    {
    case 0:
        channel = 10;
        break;
    case 1:
        channel = 14;
        break;
    case 2:
        channel = 15;
        break;
    }

    unsigned int low = (u16)g_BuildNumber;
    if (gNetworkBuildNumberOverride != 0)
    {
        low = (u16)gNetworkBuildNumberOverride;
    }
    return (low | 0x1B030000) | ((unsigned int)(channel & 0xFF) << 20);
}

void NetworkSession::InitializeLAN()
{
    SocketNetworkStartup();
    if (!SocketNetworkIsStarted())
    {
        return;
    }

    mSessionMode = 1;
    gNetworkMessageRegistry->RegisterReceiver(0xD, this);
    gNetworkMessageRegistry->RegisterReceiver(0xF, this);
    gNetworkMessageRegistry->RegisterReceiver(0x12, this);
    gNetworkMessageRegistry->RegisterReceiver(0x13, this);
    gNetworkMessageRegistry->RegisterReceiver(0x14, this);
    gNetworkMessageRegistry->RegisterReceiver(0x15, this);
    gNetworkMessageRegistry->RegisterReceiver(0x0, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1, this);
    gNetworkMessageRegistry->RegisterReceiver(0x8, this);
    gNetworkMessageRegistry->RegisterReceiver(0x9, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1C, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1D, this);

    UnidentifiedVersionInfo info;
    info.mVersionWord = GetNetworkVersionWord();
    info.mUnidentified04 = 1;
    mDirectSocket->Initialize(&info, this);
    mDirectSocket->SetBroadcastEnabled(true);

    mTransport->Initialize();
    mStatsReporter->Reset();
}

void NetworkSession::ShutdownLAN()
{
    NetworkDraft::Instance()->Reset(false);
    mStatsReporter->Close();
    NetworkStatsManager::Instance()->Reset(false);
    mTransport->UnregisterMessageReceivers();
    mDirectSocket->Shutdown();

    gNetworkMessageRegistry->UnregisterReceiver(0xD);
    gNetworkMessageRegistry->UnregisterReceiver(0xF);
    gNetworkMessageRegistry->UnregisterReceiver(0x12);
    gNetworkMessageRegistry->UnregisterReceiver(0x13);
    gNetworkMessageRegistry->UnregisterReceiver(0x14);
    gNetworkMessageRegistry->UnregisterReceiver(0x15);
    gNetworkMessageRegistry->UnregisterReceiver(0x0);
    gNetworkMessageRegistry->UnregisterReceiver(0x1);
    gNetworkMessageRegistry->UnregisterReceiver(0x8);
    gNetworkMessageRegistry->UnregisterReceiver(0x9);
    gNetworkMessageRegistry->UnregisterReceiver(0x1C);
    gNetworkMessageRegistry->UnregisterReceiver(0x1D);

    mSessionMode = 0;
}

void NetworkSession::InitializeOnline()
{
    if (!SocketNetworkIsStarted())
    {
        return;
    }

    mUnidentified24A4 = 0;

    u32 gameCode = gNetworkGameCodeR4QP;
    if (GetRegion() == 2)
    {
        gameCode = gNetworkGameCodeR4QJ;
    }
    else if (GetRegion() == 0)
    {
        gameCode = gNetworkGameCodeR4QE;
    }

    DWC_Init(DWC_SVR_RELEASE, "mschargedwii", gameCode, NetworkAlloc, NetworkFree);
    DWC_SetReportLevel(0);
    mUnidentified24A5 = 1;
    mSessionMode = 2;

    gNetworkMessageRegistry->RegisterReceiver(0xD, this);
    gNetworkMessageRegistry->RegisterReceiver(0xF, this);
    gNetworkMessageRegistry->RegisterReceiver(0x12, this);
    gNetworkMessageRegistry->RegisterReceiver(0x13, this);
    gNetworkMessageRegistry->RegisterReceiver(0x14, this);
    gNetworkMessageRegistry->RegisterReceiver(0x15, this);
    gNetworkMessageRegistry->RegisterReceiver(0x19, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1A, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1B, this);
    gNetworkMessageRegistry->RegisterReceiver(0x0, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1, this);
    gNetworkMessageRegistry->RegisterReceiver(0x8, this);
    gNetworkMessageRegistry->RegisterReceiver(0x9, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1C, this);
    gNetworkMessageRegistry->RegisterReceiver(0x1D, this);

    UnidentifiedVersionInfo info;
    info.mVersionWord = GetNetworkVersionWord();
    info.mUnidentified04 = 0;
    mDirectSocket->Initialize(&info, this);

    mDWCErrorCode = 0;
    mDWCErrorType = DWC_ERROR_NONE;
    mDWCLastError = 0;
    mUnidentified2494 = 0;
    mLobby->RegisterMessageReceiver();
    mRankingReporter->Reset();
    mUnidentified24A4 = StartLogin();
}

static void* StaticSetInternetThreadFunc(void*)
{
    g_pNetworkSession->InitializeOnline();
    return 0;
}

void NetworkSession::StartLoginThread()
{
    tDebugPrintManager::Print(DC_NETWORK, "Created StaticSetInternetThreadFunc returned %d\n",
        OSCreateThread(
            (OSThread*)mLoginThread, StaticSetInternetThreadFunc, 0,
            mLoginThreadStack + sizeof(mLoginThreadStack), 0x4000, 0xE, 1)
            != 0);
    tDebugPrintManager::Print(DC_NETWORK, "Resumed StaticSetInternetThreadFunc returned %d\n",
        OSResumeThread((OSThread*)mLoginThread));
}

bool NetworkSession::IsLoginThreadComplete()
{
    return OSIsThreadTerminated((OSThread*)mLoginThread) != 0;
}

bool NetworkSession::RequiresDisconnectAfterError()
{
    switch (mDWCErrorType)
    {
    case 7:
        tDebugPrintManager::Print(DC_NETWORK, "DWCError Type is fatal, should we do something?\n");
    case 3:
    case 4:
    case 5:
    case 6:
        return true;
    }
    return false;
}

void NetworkSession::ReadAndClearDWCError()
{
    int error = DWC_GetLastErrorEx(&mDWCErrorCode, &mDWCErrorType);
    mDWCLastError = error;
    if (error != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "DWC_GetLastErrorEx error %d ErrorCode:%d ErrorType:%d\n",
            error, mDWCErrorCode, mDWCErrorType);
        DWC_ClearError();
    }
}

static void StaticDWCLoginCallback(DWCError error, int profileID, void* param)
{
    g_pNetworkSession->DWCLoginCallback(error, profileID, param);
}

bool NetworkSession::StartLogin()
{
    GameInfoManager* gameInfo;

    SetSessionState(true);
    mLoginStage = 1;
    mLoginStartTime = 0.0f;
    gOnlineLoginStarted = 1;

    if (!DWC_CheckUserData(
            (DWCUserData*)GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex)))
    {
        DWC_CreateUserData(
            (DWCUserData*)GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));
    }
    DWC_ReportUserData(
        (DWCUserData*)GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex));

    gameInfo = GameInfoManager::GetInstance();
    DWC_InitFriendsMatch(
        &gDWCFriendsMatchControl, (DWCUserData*)gameInfo->GetSaveSlot(gNetworkSaveSlotIndex), 0x2AAF,
        "mschargedwii", "B4LdGW", 0, 0,
        (DWCFriendData*)gameInfo->GetUnknown0x40(gNetworkSaveSlotIndex, 0), 0x40);

    const u16* name = (const u16*)L"unnamed";
    if (gNetworkMiiNameWide[0] != 0)
    {
        name = gNetworkMiiNameWide;
    }

    bool result = DWC_LoginAsync(name, 0, StaticDWCLoginCallback, 0);
    if (!result)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Initial fail in DWC_LoginAsync\n");
        int error = DWC_GetLastErrorEx(&mDWCErrorCode, &mDWCErrorType);
        mDWCLastError = error;
        if (error != 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "DWC_GetLastErrorEx error %d ErrorCode:%d ErrorType:%d\n",
                error, mDWCErrorCode, mDWCErrorType);
            DWC_ClearError();
        }
        return false;
    }
    tDebugPrintManager::Print(DC_NETWORK, "Starting DWC_LoginAsync\n");
    return true;
}

void NetworkSession::DWCLoginCallback(
    int error, int profileID, void* param)
{
    tDebugPrintManager::Print(DC_NETWORK, "DWCLoginCallback returned %d, profileID %d param %d\n", error,
        profileID, param);

    if (mLoginStage != 1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "DWCLoginCallback ignored because in stage %d\n",
            mLoginStage);
        return;
    }

    mLoginStage = 2;
    if (error != 0)
    {
        if (param == 0)
        {
            ReadAndClearDWCError();
        }
        mLoginListener->OnLoginResult(1);
        return;
    }

    GameInfoManager::GetInstance()->ValidateSaveSlot(gNetworkSaveSlotIndex);
    int check = DWC_GetIngamesnCheckResult();
    tDebugPrintManager::Print(DC_NETWORK, "DWC_GetIngamesnCheckResult returned %d\n", check);
    if (check == 2)
    {
        mLoginListener->OnLoginResult(2);
    }
    else
    {
        mLoginListener->OnLoginResult(0);
    }
}

bool NetworkSession::RequestLoginRankings()
{
    (mSessionMode == 2 ? mRankingReporter : 0)->InitializeRanking();

    bool started;
    if (NetworkStatsManager::Instance()->UsesEuropeanRankings())
    {
        mLoginStage = 3;
        if (!NetworkStatsManager::Instance()->RequestRankings(4))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting friends stats\n");
            started = false;
        }
        else
        {
            started = true;
        }
    }
    else
    {
        mLoginStage = 5;
        if (!NetworkStatsManager::Instance()->RequestRankings(2))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting nearby stats\n");
            started = false;
        }
        else
        {
            started = true;
        }
    }

    if (started)
    {
        mLoginStartTime = mElapsedTime;
        tDebugPrintManager::Print(DC_NETWORK, "Started login timestamp = %f\n", mElapsedTime);
        return true;
    }
    return false;
}

void NetworkSession::UpdateLogin()
{
    if (mLoginStartTime != 0.0f
        && mLoginStartTime + gNetworkLoginTimeout <= mElapsedTime)
    {
        if ((mLoginStage < 0xE || mLoginStage >= 0x10)
            && (mLoginStage >= 3 || mLoginStage < 1))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Aborting Getting Stats in Login Timed out!\n");
            mLoginStage = 0xF;
            mLoginListener->OnStatsResult(false);
        }
        mLoginStartTime = 0.0f;
    }

    switch (mLoginStage)
    {
    case 1:
        if (mDWCLastError == 0)
        {
            int error = DWC_GetLastErrorEx(&mDWCErrorCode, &mDWCErrorType);
            mDWCLastError = error;
            if (error != 0)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "DWC_GetLastErrorEx error %d ErrorCode:%d ErrorType:%d\n",
                    error, mDWCErrorCode, mDWCErrorType);
                DWC_ClearError();
            }
        }
        if (mDWCLastError != 0)
        {
            DWCLoginCallback(mDWCLastError, 0, (void*)1);
        }
        break;

    case 3:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            NetworkRankingMeta* record = NetworkStatsManager::Instance()->mHasLocalStats[2] != 0
                                           ? &NetworkStatsManager::Instance()->mLocalStats[2]
                                           : 0;
            if (record != 0)
            {
                if (IsNewNetworkSeason(record))
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Starting new friends season clearing stats\n");
                    NetworkStatsManager::Instance()->PostResetMyPlayerStats(2, 0);
                    mLoginStage = 4;
                }
                else if (gNetworkMiiChanged != 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Detected Mii change putting friends unchanged "
                        "win/loss stats\n");
                    NetworkStatsManager::Instance()->PostResetMyPlayerStats(2, 1);
                    mLoginStage = 4;
                }
                else
                {
                    mLoginStage = 5;
                    if (!NetworkStatsManager::Instance()->RequestRankings(2))
                    {
                        tDebugPrintManager::Print(DC_NETWORK, "Error getting nearby stats\n");
                    }
                }
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Login: Did not get own friends stats, first time user "
                    "will now put new own stats\n");
                NetworkStatsManager::Instance()->PostResetMyPlayerStats(2, 0);
                mLoginStage = 4;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting friends stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 4:
        if (NetworkStatsManager::Instance()->mScoreRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mScoreRequestSucceeded != 0)
        {
            mLoginStage = 5;
            if (!NetworkStatsManager::Instance()->RequestRankings(2))
            {
                tDebugPrintManager::Print(DC_NETWORK, "Error getting nearby stats\n");
            }
            NetworkStatsManager::Instance()->SubmitJob(0xA);
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                NetworkStatsManager::Instance()->mScoreCategory);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mScoreRequestComplete = 0;
        break;

    case 5:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            NetworkRankingMeta* record = NetworkStatsManager::Instance()->mHasLocalStats[0] != 0
                                           ? &NetworkStatsManager::Instance()->mLocalStats[0]
                                           : 0;
            if (record != 0)
            {
                if (IsNewNetworkSeason(record))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Starting new season clearing stats\n");
                    NetworkStatsManager::Instance()->CommitPendingOnlineTotals(record);
                    NetworkStatsManager::Instance()->PostResetMyPlayerStats(0, 0);
                    mLoginStage = 6;
                }
                else if (gNetworkMiiChanged != 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Detected Mii change putting own unchanged win/loss "
                        "stats\n");
                    NetworkStatsManager::Instance()->PostResetMyPlayerStats(0, 1);
                    mLoginStage = 6;
                }
                else
                {
                    mLoginStage = 8;
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Login: Transition to "
                        "ELoggingInStage_GettingSODNearbyStats\n");
                    if (!NetworkStatsManager::Instance()->RequestRankings(0))
                    {
                        tDebugPrintManager::Print(DC_NETWORK,
                            "Initial failure to RequestRankings "
                            "STRIKER_OF_DAY Nearby\n");
                        mLoginListener->OnStatsResult(false);
                        mLoginStage = 0xF;
                    }
                }
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Login: Did not get own stats, first time user will now "
                    "put new own stats\n");
                NetworkStatsManager::Instance()->PostResetMyPlayerStats(0, 0);
                mLoginStage = 6;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 6:
        if (NetworkStatsManager::Instance()->mScoreRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mScoreRequestSucceeded != 0)
        {
            mLoginStage = 7;
            if (!NetworkStatsManager::Instance()->RequestRankings(2))
            {
                tDebugPrintManager::Print(DC_NETWORK, "Error REgetting nearby stats\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                NetworkStatsManager::Instance()->mScoreCategory);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mScoreRequestComplete = 0;
        break;

    case 7:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 8;
            tDebugPrintManager::Print(DC_NETWORK,
                "Login: Transition to "
                "ELoggingInStage_GettingSODNearbyStats\n");
            if (!NetworkStatsManager::Instance()->RequestRankings(0))
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Initial failure to RequestRankings STRIKER_OF_DAY "
                    "Nearby\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error REgetting nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 8:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            NetworkRankingMeta* record = NetworkStatsManager::Instance()->mHasLocalStats[1] != 0
                                           ? &NetworkStatsManager::Instance()->mLocalStats[1]
                                           : 0;
            if (record != 0)
            {
                if (IsNewNetworkDay(record))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Starting new day clearing stats\n");
                    NetworkStatsManager::Instance()->PostResetMyPlayerStats(1, 0);
                    mLoginStage = 9;
                }
                else if (gNetworkMiiChanged != 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Detected Mii change putting SOD unchanged win/loss "
                        "stats\n");
                    NetworkStatsManager::Instance()->PostResetMyPlayerStats(1, 1);
                    mLoginStage = 9;
                }
                else if (!NetworkStatsManager::Instance()->UsesEuropeanRankings())
                {
                    mLoginStage = 0xB;
                    if (!NetworkStatsManager::Instance()->RequestRankings(4))
                    {
                        tDebugPrintManager::Print(DC_NETWORK,
                            "Initial failure to RequestRankings SEASON "
                            "FRIENDS\n");
                        mLoginListener->OnStatsResult(false);
                        mLoginStage = 0xF;
                    }
                }
                else
                {
                    mLoginStage = 0xC;
                    if (!NetworkStatsManager::Instance()->RequestRankings(1))
                    {
                        tDebugPrintManager::Print(DC_NETWORK,
                            "Initial failure to RequestRankings "
                            "STRIKER_OF_DAY TOP\n");
                        mLoginListener->OnStatsResult(false);
                        mLoginStage = 0xF;
                    }
                }
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Login: Did not get own SOD stats, first time user will "
                    "now put new own SOD stats\n");
                NetworkStatsManager::Instance()->PostResetMyPlayerStats(1, 0);
                mLoginStage = 9;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting SOD nearbystats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 9:
        if (NetworkStatsManager::Instance()->mScoreRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mScoreRequestSucceeded != 0)
        {
            mLoginStage = 0xA;
            tDebugPrintManager::Print(DC_NETWORK,
                "Login: Transition to "
                "ELoggingInStage_ReGettingSODNearbyStats\n");
            if (!NetworkStatsManager::Instance()->RequestRankings(0))
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Initial failure to RE-RequestRankings STRIKER_OF_DAY "
                    "Nearby\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                NetworkStatsManager::Instance()->mScoreCategory);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mScoreRequestComplete = 0;
        break;

    case 0xA:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            if (!NetworkStatsManager::Instance()->UsesEuropeanRankings())
            {
                mLoginStage = 0xB;
                if (!NetworkStatsManager::Instance()->RequestRankings(4))
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Initial failure to RequestRankings SEASON "
                        "FRIENDS\n");
                    mLoginListener->OnStatsResult(false);
                    mLoginStage = 0xF;
                }
            }
            else
            {
                mLoginStage = 0xC;
                if (!NetworkStatsManager::Instance()->RequestRankings(1))
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Initial failure to RequestRankings STRIKER_OF_DAY "
                        "TOP\n");
                    mLoginListener->OnStatsResult(false);
                    mLoginStage = 0xF;
                }
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error REgetting SOD Nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 0xB:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 0xC;
            if (!NetworkStatsManager::Instance()->RequestRankings(1))
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Initial failure to RequestRankings STRIKER_OF_DAY "
                    "TOP\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting SEASON FRIENDS stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 0xC:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 0xD;
            if (!NetworkStatsManager::Instance()->RequestRankings(3))
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Initial failure to RequestRankings SEASON TOP\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting SOD TOP stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 0xD:
        if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 0xE;
            g_pNetworkSession->SetSessionState(2);
            mLoginListener->OnStatsResult(true);
            NetworkStatsManager::Instance()->RefreshSaveState_801314D0();
            gNetworkMiiChanged = 0;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting SEASON TOP stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = 0;
        break;
    }
}

void NetworkSession::ShutdownOnline()
{
    if (ResetTask::s_ResetState != RS_STARTRESET)
    {
        SaveLoad::StartSave(true);
    }
    else if (mLobby->mMatchmakingThreadRunning)
    {
        return;
    }

    mUnidentified2494 = 0;
    DWC_ShutdownFriendsMatch();
    NetworkDraft::Instance()->Reset(false);
    g_pFriendManager->Reset(false);
    mRankingReporter->ShutdownRanking();
    NetworkStatsManager::Instance()->Reset(false);
    mLobby->UnregisterMessageReceiver();
    mDirectSocket->Shutdown();
    DWC_Shutdown();
    mUnidentified24A5 = 0;
    SocketNetworkShutdown();

    gNetworkMessageRegistry->UnregisterReceiver(0xD);
    gNetworkMessageRegistry->UnregisterReceiver(0xF);
    gNetworkMessageRegistry->UnregisterReceiver(0x12);
    gNetworkMessageRegistry->UnregisterReceiver(0x13);
    gNetworkMessageRegistry->UnregisterReceiver(0x14);
    gNetworkMessageRegistry->UnregisterReceiver(0x15);
    gNetworkMessageRegistry->UnregisterReceiver(0x19);
    gNetworkMessageRegistry->UnregisterReceiver(0x1A);
    gNetworkMessageRegistry->UnregisterReceiver(0x1B);
    gNetworkMessageRegistry->UnregisterReceiver(0x0);
    gNetworkMessageRegistry->UnregisterReceiver(0x1);
    gNetworkMessageRegistry->UnregisterReceiver(0x8);
    gNetworkMessageRegistry->UnregisterReceiver(0x9);
    gNetworkMessageRegistry->UnregisterReceiver(0x1C);
    gNetworkMessageRegistry->UnregisterReceiver(0x1D);
    mSessionMode = 0;
}

void NetworkSession::Shutdown()
{
    switch (mSessionMode)
    {
    case 0:
        break;
    case 2:
        ShutdownOnline();
        break;
    case 1:
        ShutdownLAN();
        break;
    }
    mSessionMode = 0;
    mSessionState = 0;
}

int NetworkSession::GetSessionMode()
{
    return mSessionMode;
}

int NetworkSession::GetSessionState()
{
    return mSessionState;
}

void NetworkSession::SetSessionState(int phase)
{
    mSessionState = phase;
}

NetworkSocket* NetworkSession::GetDirectSocket()
{
    return mDirectSocket;
}

NetworkMachineRoster* NetworkSession::GetMachineRoster()
{
    if (mSessionMode == 2)
    {
        return mLobby;
    }
    if (mSessionMode == 1)
    {
        return mTransport;
    }
    return 0;
}

LANLobby* NetworkSession::GetTransport()
{
    if (mSessionMode == 1)
    {
        return mTransport;
    }
    return 0;
}

NetworkLobby* NetworkSession::GetOnlineLobby()
{
    if (mSessionMode == 2)
    {
        return mLobby;
    }
    return 0;
}

NetworkStatsInterface* NetworkSession::GetStatsInterface()
{
    if (mSessionMode == 2)
    {
        return mRankingReporter;
    }
    if (mSessionMode == 1)
    {
        return mStatsReporter;
    }
    return 0;
}

NetworkStatsReporter* NetworkSession::GetStatsReporter()
{
    if (mSessionMode == 1)
    {
        return mStatsReporter;
    }
    return 0;
}

NetworkRanking* NetworkSession::GetRankingReporter()
{
    if (mSessionMode == 2)
    {
        return mRankingReporter;
    }
    return 0;
}

void NetworkSession::ListenerVirtual00(void* buffer, int size)
{
    gNetworkMessageRegistry->Dispatch(-2, static_cast<u8*>(buffer), size);
}

void NetworkSession::OnMessageReceived(
    int source, void* buffer, int size, bool)
{
    gNetworkMessageRegistry->Dispatch(source, static_cast<u8*>(buffer), size);
}

void NetworkSession::OnConnectionRequest(u32 connection, u8* address)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster == 0)
    {
        return;
    }
    NetworkSocket* socket = g_pNetworkSessionBase->GetDirectSocket();
    if (roster->ShouldAcceptConnection(connection, address))
    {
        socket->AcceptConnection(connection);
        tDebugPrintManager::Print(DC_NETWORK, "Accepted connection from %d.%d.%d.%d\n", address[0],
            address[1], address[2], address[3]);
    }
    else
    {
        socket->RejectConnection(connection);
        tDebugPrintManager::Print(DC_NETWORK, "Rejected connection from %d.%d.%d.%d\n", address[0],
            address[1], address[2], address[3]);
    }
}

void NetworkSession::OnConnected(u32 connection, int result)
{
    tDebugPrintManager::Print(DC_NETWORK, "Connected callback result %d\n", result);
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->OnConnected(connection, result);
    }
}

void NetworkSession::OnConnectionClosed(u32 connection, int reason)
{
    tDebugPrintManager::Print(DC_NETWORK, "Connection closed reason %d\n", reason);
    OnGameConnectionLost(connection, reason);
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->OnConnectionClosed(connection, reason);
    }
}

void NetworkSession::ListenerVirtual14()
{
}

void NetworkSession::ListenerVirtual18()
{
}

// Two local pad indices for the machine's one or two local players.

static void CaptureRecordedGameConfig(RecordedGameConfig* config);
static void ApplyRecordedGameConfig(RecordedGameConfig* config);

static inline void NotifyGameStarted(NetworkSession* session)
{
    NetworkSyncState* state = gNetworkSyncState;
    int count = session->GetNumMachines();
    state->SetMachineInfo((s8)session->GetLocalMachineId(), count);
}

static inline void RecordGameConfig(
    NetworkSession* session, u32 seed,
    RecordedGameConfig* config)
{
    NetworkInputRecording* state = gNetworkInputRecording;
    if (state->mRecordingEnabled != 0)
    {
        state->StartNetworkInputRecording((s8)session->GetLocalMachineId(), session->GetNumMachines(), seed, config, 0x58);
    }
}

typedef void (NetworkSession::*UnidentifiedSessionCallback)();

static inline void RegisterLoadedGameActions(NetworkSession* session)
{
    Function<FnVoidVoid> first(NetworkSessionBinding(
        MemFun<NetworkSession, void>(&NetworkSession::OnPauseGame), session));
    UnidentifiedTypedEvent<UnidentifiedEventNoData>* pauseEvent
        = &g_pGame->mUnidentified49C.mEvent00;
    pauseEvent->Add(first, (unsigned int)&session->mUnidentified2464, -1);

    Function<FnVoidVoid> second(NetworkSessionBinding(
        MemFun<NetworkSession, void>(&NetworkSession::OnResumingGame), session));
    UnidentifiedTypedEvent<UnidentifiedEventNoData>* resumingEvent
        = &g_pGame->mUnidentified49C.mEvent01;
    resumingEvent->Add(second, (unsigned int)&session->mUnidentified2468, -1);

    if (NetTournManager::Instance()->mState != 0)
    {
        NetTournManager::Instance()->NotifyGameStarted();
    }
}

int NetworkSession::ProcessMessage(
    NetworkMessage* message)
{
    NetworkMachineRoster* roster = GetMachineRoster();
    int machine = (s8)roster->MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mSource);
        return 1;
    }

    if ((u8)message->GetType() != 0x14 && (u8)message->GetType() != 0xD
        && (u8)message->GetType() != 0x15 && (u8)message->GetType() != 0x19
        && (u8)message->GetType() != 0x1A && (u8)message->GetType() != 0x1B
        && NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        int original = machine;
        machine = (s8)NetTournManager::Instance()->TournamentIdxToMachineIdx(
            machine);
        if (machine < 0 || machine >= this->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded message type %d.  TournamentIdxToMachineIdx "
                "changed ID %d to ID %d, but invalid\n",
                (u8)message->GetType(), (s8)original, machine);
            return 1;
        }
    }

    switch ((u8)message->GetType())
    {
    case 0xD:
        if (gNetworkInputRecording->mPlaybackEnabled != 0)
        {
            // Retail re-evaluates the playback flag inside the branch,
            // matching an inlined predicate called twice.
            if (gNetworkInputRecording->mPlaybackEnabled != 0
                && gNetworkInputRecording->ReadNetworkInputRecordingHeader())
            {
            NetworkGameStartInfo* info =
                (NetworkGameStartInfo*)&gNetworkInputRecording->mConfigSize;
            SetNetworkRandomSeed(gNetworkInputRecording->mRandomSeed);
            RecordedGameConfig* config = info->mConfig;
            ApplyRecordedGameConfig(config);
            tDebugPrintManager::Print(DC_NETWORK,
                "PlaybackRecordedGame: Random seed %x Stadium %d HOME %d "
                "[%d, %d, %d] Vs AWAY %d [%d, %d, %d]\n",
                info->mSeed, config->mStadium, config->mHomeTeam,
                config->mHomeSidekicks[0], config->mHomeSidekicks[1],
                config->mHomeSidekicks[2], config->mAwayTeam,
                config->mAwaySidekicks[0], config->mAwaySidekicks[1],
                config->mAwaySidekicks[2]);
            tDebugPrintManager::Print(DC_NETWORK,
                "PlaybackRecordedGame: Skill %d WinBy %s GameTime %d "
                "GameGoals %d BestSeries %d\n",
                config->mSkillLevel,
                config->mWinBy == 0 ? "Timed" : "Goals",
                config->mGameTime, config->mGameGoals,
                config->mBestSeries);
            GetInputRouter()->Reset(0);
            g_pNetworkSessionBase->BaseVirtual3C(info);
            NetworkSyncState* state = gNetworkSyncState;
            int count = g_pNetworkSessionBase->GetNumMachines();
            state->SetMachineInfo((s8)g_pNetworkSessionBase->GetLocalMachineId(), count);
            }
        }
        else
        {
            BaseVirtual44((NetMessageGameStart*)message);
        }
        mSessionState = 4;
        g_pGameSceneManager->PopToScene((SceneList)0x1D);
        for (int component = 0; component < 4; ++component)
        {
            gFEPointerInstances[component]->SetActiveSlide("waiting", true, false);
        }
        g_pGameSceneManager->PushLoadingScene(false);
        break;

    case 0x14:
        NetTournManager::Instance()->TransitionOnlineMenuToTournament(
            static_cast<NetMessageTournamentStart*>(message));
        break;

    case 0x15:
        if (((NetMessageDraft*)message)->mUnidentified0A != 0)
        {
            SHOnlineFriendsChooseSides* scene = (SHOnlineFriendsChooseSides*)
                g_pGameSceneManager->Push((SceneList)0x38, SCREEN_FORWARD, true);
            scene->SetDraftMessage(*(NetMessageDraft*)message);
        }
        else
        {
            if (IsOnlineRankedMatch())
            {
                NetworkDraft::Instance()->BeginSortedDraft(
                    static_cast<NetMessageDraft*>(message));
            }
            else
            {
                NetworkDraft::Instance()->BeginTeamDraft(
                    static_cast<NetMessageDraft*>(message));
            }
            NetworkStatsManager::Instance()->ResetPregameDisconnectState();
        }
        break;

    case 0x19:
    {
        BaseSceneHandler* scene = g_pGameSceneManager->GetScene((SceneList)0x38);
        SHOnlineFriendsChooseSides* handler = static_cast<SHOnlineFriendsChooseSides*>(scene);
        if (handler != 0)
        {
            handler->OnSidesChanged(static_cast<NetMessageSidesChanged*>(message));
        }
        break;
    }

    case 0x1A:
    {
        mSessionState = 3;
        GetMachineRoster()->OnGameStarted();
        if (IsOnlineRankedMatch())
        {
            FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)0x2A);
        }
        FEMusic::StartStreamIfDifferent(8);
        OnlineConnectionQualityScene* scene = static_cast<OnlineConnectionQualityScene*>(
            g_pGameSceneManager->Push((SceneList)0x39, SCREEN_FORWARD, true));
        scene->OnCheckConnection(static_cast<NetMessageCheckConnection*>(message));
        break;
    }

    case 0x1B:
    {
        OnlineConnectionQualityScene* scene = static_cast<OnlineConnectionQualityScene*>(
            g_pGameSceneManager->GetScene((SceneList)0x39));
        if (scene != 0)
        {
            scene->OnConnectionDecision(static_cast<NetMessageConnectionDecision*>(message));
        }
        break;
    }

    case 0xF:
        tDebugPrintManager::Print(DC_NETWORK, "Received loaded game message from %d\n", machine);
        mMachineLoadedGame[machine] = 1;
        break;

    case 0x12:
        tDebugPrintManager::Print(DC_NETWORK, "Received loaded game CLIENT message from %d\n", machine);
        mMachineLoadedGame[machine] = 1;
        break;

    case 0x13:
    {
        tDebugPrintManager::Print(DC_NETWORK, "Received loaded game EVERYONE message from %d\n", machine);
        for (int index = 0; index < this->GetNumMachines(); ++index)
        {
            mMachineLoadedGame[index] = 1;
        }
        RegisterLoadedGameActions(this);
        mSessionState = 5;
        break;
    }

    case 0x0:
    case 0x1:
        if (gNetworkInputRecording->mPlaybackEnabled != 0)
        {
            break;
        }
        if (machine < 0 || machine >= this->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded message type %d because from unknown connection "
                "%x\n",
                (u8)message->GetType(), message->mSource);
            break;
        }
        if (mSessionState != 5)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Ignoring GameInput Message because in network stage %d",
                mSessionState);
            break;
        }
        if ((u8)message->GetType() == 0)
        {
            GetInputRouter()->ReceiveInput(
                machine, (NetMessageInput*)message);
        }
        else
        {
            NetMessageInputBundle* bundled =
                (NetMessageInputBundle*)message;
            GetInputRouter()->ReceiveInput(machine, &bundled->mMessage0);
            GetInputRouter()->ReceiveInput(machine, &bundled->mMessage1);
        }
        break;

    case 0x8:
    case 0x9:
        if (gNetworkInputRecording->mPlaybackEnabled != 0)
        {
            break;
        }
        if (machine < 0 || machine >= this->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded message type %d because from unknown connection "
                "%x\n",
                (u8)message->GetType(), message->mSource);
            break;
        }
        if (mSessionState != 5)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Ignoring GameInput Message because in network stage %d",
                mSessionState);
            break;
        }
        if ((u8)message->GetType() == 8)
        {
            GetInputRouter()->ReceiveAllInputs(
                machine, (NetMessageAllInputs*)message);
        }
        else
        {
            NetMessageAllInputsBundle* bundled =
                (NetMessageAllInputsBundle*)message;
            GetInputRouter()->ReceiveAllInputs(machine, &bundled->mMessage0);
            GetInputRouter()->ReceiveAllInputs(machine, &bundled->mMessage1);
        }
        break;

    case 0x1C:
    {
        if ((s8)this->GetLocalMachineId() != 0)
        {
            break;
        }
        u8 bit = 1 << (s8)((NetMessagePauseRequest*)message)
                              ->mMachineIndex;
        if (((NetMessagePauseRequest*)message)->mPaused != 0)
        {
            mPauseRequestMachineMask = mPauseRequestMachineMask | bit;
        }
        else
        {
            mPauseRequestMachineMask = mPauseRequestMachineMask & ~bit;
        }

        NetMessagePauseResponse response;
        response.mMachineMask = mPauseRequestMachineMask;
        u8 buffer[0x32];
        u32 size = gNetworkMessageRegistry->Serialize(&response, buffer, 0x32);
        tDebugPrintManager::Print(DC_NETWORK, "HOST sending Pause Response to all clients and myself\n");
        int count = this->GetNumMachines();
        for (s8 target = 0; target < count; ++target)
        {
            Send(target, buffer, size, true);
        }
        break;
    }

    case 0x1D:
        if (machine != 0)
        {
            break;
        }
        mPausedMachineMask =
            ((NetMessagePauseResponse*)message)->mMachineMask;
        break;
    }
    return 1;
}

void NetworkSession::BaseVirtual3C(const NetworkGameStartInfo* info)
{
    mMachineCount = info->mMachineCount;
    mLocalMachineId = info->mMyMachineId;

    int machine;
    NetworkPeer* peer = mPeers;
    for (machine = 0; machine < mMachineCount; ++machine)
    {
        peer->mMachineId = machine;
        int players = info->mPlayerCounts[machine];
        peer->mPlayerCount = players;
        for (int player = 0; player < players; ++player)
        {
            (peer->GetNetworkPeerChannel(player))->Initialize(peer, (s8)player, player);
        }
        peer->ResetNetworkPeerInputs();
        ++peer;
    }
    AIPadManager::Startup();
}

static void CaptureRecordedGameConfig(RecordedGameConfig* config)
{
    config->mStadium = GameInfoManager::GetInstance()->GetStadium();
    config->mHomeTeam = GameInfoManager::GetInstance()->GetTeam(0);
    config->mAwayTeam = GameInfoManager::GetInstance()->GetTeam(1);
    for (int slot = 0; slot < 3; ++slot)
    {
        config->mHomeSidekicks[slot] =
            GameInfoManager::GetInstance()->GetSidekick(0, slot);
        config->mAwaySidekicks[slot] =
            GameInfoManager::GetInstance()->GetSidekick(1, slot);
    }

    const GameplaySettings* settings =
        GameInfoManager::GetInstance()->GetCurrentSettings();
    config->mSkillLevel = settings->SkillLevel;
    config->mWinBy = settings->GameLimitType;
    config->mGameTime = settings->GameTime;
    config->mGameGoals = settings->GoalLimit;
    config->mBestSeries = settings->NumGames;

    for (int pad = 0; pad < 16; ++pad)
    {
        config->mPlayingSides[pad] =
            GameInfoManager::GetInstance()->GetPlayingSide(pad);
    }
}

static void ApplyRecordedGameConfig(RecordedGameConfig* config)
{
    GameInfoManager::GetInstance()->SetStadium(config->mStadium);
    GameInfoManager::GetInstance()->SetTeam(0, config->mHomeTeam);
    GameInfoManager::GetInstance()->SetTeam(1, config->mAwayTeam);
    for (int slot = 0; slot < 3; ++slot)
    {
        GameInfoManager::GetInstance()->SetSidekick(
            0, config->mHomeSidekicks[slot], slot);
        GameInfoManager::GetInstance()->SetSidekick(
            1, config->mAwaySidekicks[slot], slot);
    }

    GameInfoManager* manager = GameInfoManager::GetInstance();
    manager->mUserInfo.mGameplayOptions.SkillLevel = config->mSkillLevel;
    manager->mUserInfo.mGameplayOptions.GameLimitType = config->mWinBy;
    manager->mUserInfo.mGameplayOptions.GameTime = config->mGameTime;
    manager->mUserInfo.mGameplayOptions.GoalLimit = config->mGameGoals;
    manager->mUserInfo.mGameplayOptions.NumGames = config->mBestSeries;

    manager = GameInfoManager::GetInstance();
    manager->mCurGameGameplayOptions = manager->mUserInfo.mGameplayOptions;

    for (int pad = 0; pad < 16; ++pad)
    {
        GameInfoManager::GetInstance()->SetPlayingSide(
            pad, config->mPlayingSides[pad]);
    }
}

void NetworkSession::BaseVirtual40()
{
}

void NetworkSession::BaseVirtual44(NetMessageGameStart* message)
{
    mMachineCount = (s8)message->mMachineCount;
    mLocalMachineId = (s8)message->mMachineIndex;

    u8* entryFlags = (u8*)NetworkDraft::Instance() + 0xD3C;
    for (int machine = 0; machine < mMachineCount; ++machine)
    {
        mPeers[machine].mMachineId = machine;
        int players;
        if (IsOnlineRankedMatch())
        {
            players = message->mMachineFlags[machine];
        }
        else
        {
            players = (entryFlags[0x93] != 0) + 1;
        }
        mPeers[machine].mPlayerCount = players;

        if (machine == mLocalMachineId)
        {
            for (int player = 0; player < players; ++player)
            {
                ((&mPeers[machine])->GetNetworkPeerChannel(player))->Initialize(&mPeers[machine],
                    (s8)player, gOnlineLocalControllerIndices[player]);
            }
        }
        else
        {
            for (int player = 0; player < players; ++player)
            {
                ((&mPeers[machine])->GetNetworkPeerChannel(player))->Initialize(&mPeers[machine],
                    (s8)player, -1);
            }
        }
        entryFlags += 0x80;
    }

    if (message->mUnidentified1B != 0)
    {
        NetTournManager::Instance()->OnTournamentGameStart(message);
    }
    else
    {
        mUnidentified2474 = message->mUnidentified20;
        mUnidentified2478 = message->mUnidentified24;
        mUnidentified247C = 1;
    }

    tDebugPrintManager::Print(DC_NETWORK, "PreStartNetworkedGame NumMachines:%d MyMachineID:%d\n",
        mMachineCount, mLocalMachineId);
    AIPadManager::Startup();

    u32 seed = message->mRandomSeed;
    SetNetworkRandomSeed(seed);
    GameInfoManager::GetInstance()->SetStadium(message->mStadium);
    GameInfoManager::GetInstance()->SetTeam(0, message->mHomeCharacters[0]);
    GameInfoManager::GetInstance()->SetSidekick(
        0, message->mHomeCharacters[1], 0);
    GameInfoManager::GetInstance()->SetSidekick(
        0, message->mHomeCharacters[2], 1);
    GameInfoManager::GetInstance()->SetSidekick(
        0, message->mHomeCharacters[3], 2);
    GameInfoManager::GetInstance()->SetTeam(1, message->mAwayCharacters[0]);
    GameInfoManager::GetInstance()->SetSidekick(
        1, message->mAwayCharacters[1], 0);
    GameInfoManager::GetInstance()->SetSidekick(
        1, message->mAwayCharacters[2], 1);
    GameInfoManager::GetInstance()->SetSidekick(
        1, message->mAwayCharacters[3], 2);
    GameInfoManager::GetInstance()->ResetPlayingSides();

    if (IsOnlineRankedMatch())
    {
        u32 side = 0;
        if (message->mUnidentified1B == 0)
        {
            side = NetworkDraft::Instance()->GetDraftTeam(0)
                       ->mPlayers[0]
                       .mDisconnected
                != 0;
        }
        for (int machine = 0; machine < mMachineCount; ++machine)
        {
            int players = mPeers[machine].mPlayerCount;
            for (int player = 0; player < players; ++player)
            {
                GameInfoManager* manager = GameInfoManager::GetInstance();
                manager->SetPlayingSide(
                    (u16)(s8)GetNetworkPlayerId((s8)player, (s8)machine), (s16)side);
            }
            side = side == 0;
        }
    }
    else
    {
        u8* sides = (u8*)message;
        for (int machine = 0; machine < mMachineCount; ++machine)
        {
            int players = mPeers[machine].mPlayerCount;
            for (int player = 0; player < players; ++player)
            {
                GameInfoManager* manager = GameInfoManager::GetInstance();
                manager->SetPlayingSide(
                    (u16)(s8)GetNetworkPlayerId((s8)player, (s8)machine),
                    (s8)sides[player + 0xB]);
            }
            sides += 2;
        }
    }

    tDebugPrintManager::Print(DC_NETWORK, "StartNetworkedGame: Random seed %x NumMachines:%d "
              "MyMachineID:%d\n",
        seed, mMachineCount, mLocalMachineId);
    GetInputRouter()->Reset(0);
    NotifyGameStarted(this);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    RecordGameConfig(this, seed, &config);
}

void NetworkSession::RematchGame()
{
    tDebugPrintManager::Print(DC_NETWORK, "Rematching network game.  End Frame is %d\n",
        GetFixedUpdateTask()->GetFrame());
    GetFixedUpdateTask()->Reset();
    OnInputSessionReset();
    gNetworkInputRecording->Reset(0);
    gNetworkSyncState->Reset(0);
    GetInputRouter()->Reset(0);
    gInputManager->Reset();

    mUnidentified247C = mUnidentified247C + 1;
    u32 seed;
    if (mUnidentified247C == 2)
    {
        seed = mUnidentified2474;
    }
    else
    {
        seed = mUnidentified2478;
    }
    SetNetworkRandomSeed(seed);

    NotifyGameStarted(this);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    RecordGameConfig(this, seed, &config);
}

void RestartSinglePlayerGame()
{
    GetFixedUpdateTask()->Reset();
    OnInputSessionReset();
    gNetworkInputRecording->Reset(0);
    gNetworkSyncState->Reset(0);
    GetInputRouter()->Reset(0);
    gInputManager->Reset();

    u32 seed = NetworkRandom();
    SetNetworkRandomSeed(seed);

    NetworkSyncState* state = gNetworkSyncState;
    int count = g_pNetworkSessionBase->GetNumMachines();
    state->SetMachineInfo((s8)g_pNetworkSessionBase->GetLocalMachineId(), count);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    NetworkInputRecording* record = gNetworkInputRecording;
    if (record->mRecordingEnabled != 0)
    {
        int machines = g_pNetworkSessionBase->GetNumMachines();
        record->StartNetworkInputRecording((s8)g_pNetworkSessionBase->GetLocalMachineId(), machines, seed, &config,
            0x58);
    }
}

void StartSinglePlayerGame()
{
    NetworkSessionControl& online = *g_pNetworkSessionBase;
    online.Shutdown();
    g_pNetworkSessionBase->InitializeMachines(1, 4);

    NetworkPeer* peer = g_pNetworkSessionBase->GetPeer(0);
    for (int player = 0; player < (int)peer->mPlayerCount; ++player)
    {
        (peer->GetNetworkPeerChannel(player))->Initialize(peer, (s8)player, player);
    }
    AIPadManager::Startup();

    u32 seed = NetworkRandom();
    SetNetworkRandomSeed(seed);
    tDebugPrintManager::Print(DC_NETWORK, "StartSinglePlayerGame: Set random seed to %x\n", seed);
    GetInputRouter()->Reset(0);

    NetworkSyncState* state = gNetworkSyncState;
    int count = g_pNetworkSessionBase->GetNumMachines();
    state->SetMachineInfo((s8)g_pNetworkSessionBase->GetLocalMachineId(), count);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    NetworkInputRecording* record = gNetworkInputRecording;
    if (record->mRecordingEnabled != 0)
    {
        int machines = g_pNetworkSessionBase->GetNumMachines();
        record->StartNetworkInputRecording((s8)g_pNetworkSessionBase->GetLocalMachineId(), machines, seed, &config,
            0x58);
    }
}

void PlaybackRecordedGame()
{
    if (gNetworkInputRecording->mPlaybackEnabled == 0)
    {
        return;
    }
    if (!gNetworkInputRecording->ReadNetworkInputRecordingHeader())
    {
        return;
    }

    NetworkGameStartInfo* info =
        (NetworkGameStartInfo*)&gNetworkInputRecording->mConfigSize;
    SetNetworkRandomSeed(info->mSeed);
    RecordedGameConfig* config = info->mConfig;
    ApplyRecordedGameConfig(config);
    tDebugPrintManager::Print(DC_NETWORK,
        "PlaybackRecordedGame: Random seed %x Stadium %d HOME %d "
        "[%d, %d, %d] Vs AWAY %d [%d, %d, %d]\n",
        info->mSeed, config->mStadium, config->mHomeTeam,
        config->mHomeSidekicks[0], config->mHomeSidekicks[1],
        config->mHomeSidekicks[2], config->mAwayTeam,
        config->mAwaySidekicks[0], config->mAwaySidekicks[1],
        config->mAwaySidekicks[2]);
    tDebugPrintManager::Print(DC_NETWORK,
        "PlaybackRecordedGame: Skill %d WinBy %s GameTime %d GameGoals %d "
        "BestSeries %d\n",
        config->mSkillLevel,
        config->mWinBy == 0 ? "Timed" : "Goals",
        config->mGameTime, config->mGameGoals, config->mBestSeries);
    GetInputRouter()->Reset(0);
    g_pNetworkSessionBase->BaseVirtual3C(info);
    NetworkSyncState* state = gNetworkSyncState;
    int count = g_pNetworkSessionBase->GetNumMachines();
    state->SetMachineInfo((s8)g_pNetworkSessionBase->GetLocalMachineId(), count);
}

void NetworkSession::BaseVirtual48(int reason)
{
    mGameEndReason = reason;
    mSessionState = 6;
    DisconnectEventOwner(&mUnidentified2464);
    DisconnectEventOwner(&mUnidentified2468);

    if (mCupMode != 0)
    {
        NetTournManager::Instance()->ResetGameProgressUpdateTimer(
            mGameEndReason);
    }
    else
    {
        NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
        if (roster != 0)
        {
            roster->Shutdown(true);
        }
    }

    if (GetSessionMode() == 2)
    {
        NetworkStatsManager::Instance()->BeginOnlineGame_80131DB4();
    }
}

int NetworkSession::Send(
    s8 player, void* buffer, int size, bool reliable)
{
    NetworkSocket* socket = GetDirectSocket();
    if ((int)player == mLocalMachineId)
    {
        socket->Receive(buffer, size);
        return 1;
    }

    NetworkMachineRoster* roster = GetMachineRoster();
    if (roster == 0)
    {
        return 0;
    }

    s8 machine = player;
    if (NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        machine = (s8)NetTournManager::Instance()->MachineIdxToTournamentIdx(
            (s8)machine);
    }

    u32 aid = roster->GetMachineAid((s8)machine);
    if (aid == 0)
    {
        return 0;
    }
    socket->Send(aid, buffer, size, reliable);
    return 1;
}

int NetworkSession::IsLiveNetworkGame()
{
    if (GetSessionMode() == 0)
    {
        return 0;
    }
    return gNetworkInputRecording->mPlaybackReady == 0;
}

int NetworkSession::PollGameLoaded()
{
    if (mSessionState == 5)
    {
        return 1;
    }
    if (!g_pNetworkSession->IsLiveNetworkGame())
    {
        return 0;
    }

    if (mOverlayRequest == 3)
    {
        NetworkMachineRoster* roster;
        for (int machine = 0; machine < this->GetNumMachines(); ++machine)
        {
            if (machine == (s8)this->GetLocalMachineId())
            {
                continue;
            }
            u32 aid;
            if ((s8)machine == mLocalMachineId)
            {
                aid = 0xFFFFFFFF;
            }
            else
            {
                roster = GetMachineRoster();
                int index;
                if (NetTournManager::Instance()
                        ->mTournamentMachineMappingActive)
                {
                    index = (s8)NetTournManager::Instance()
                                ->MachineIdxToTournamentIdx((s8)machine);
                }
                else
                {
                    index = (s8)machine;
                }
                aid = roster->GetMachineAid(index);
            }
            if (aid == 0)
            {
                mOverlayRequest = 0;
                break;
            }
        }
    }

    if (mOverlayRequest != 3)
    {
        mSessionState = 5;
        RegisterLoadedGameActions(this);
        return 1;
    }

    for (int machine = 0; machine < this->GetNumMachines(); ++machine)
    {
        if (mMachineLoadedGame[machine] == 0)
        {
            return 0;
        }
    }

    tDebugPrintManager::Print(DC_NETWORK, "Game has loaded for everyone!\n");
    if (GetMachineRoster()->GetTopology() == 1 && (s8)this->GetLocalMachineId() == 0)
    {
        NetMessageLoadedGameEveryone message;
        u8 buffer[0xC8];
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0xC8);
        tDebugPrintManager::Print(DC_NETWORK,
            "HOST sending Loaded Game Everyone message to all clients\n");
        int count = this->GetNumMachines();
        for (s8 target = 1; target < count; ++target)
        {
            Send(target, buffer, size, true);
        }
    }
    mSessionState = 5;
    RegisterLoadedGameActions(this);
    return 1;
}

void NetworkSession::OnPauseGame()
{
}

void NetworkSession::OnResumingGame()
{
}

u8 NetworkSession::GetPausedMachineMask()
{
    return mPausedMachineMask;
}

void NetworkSession::NotifyGameLoaded()
{
    int ready;
    if (GetSessionMode() == 0)
    {
        ready = 0;
    }
    else
    {
        ready = gNetworkInputRecording->mPlaybackReady == 0;
    }

    if (ready == 0)
    {
        mSessionState = 5;
        RegisterLoadedGameActions(this);
        return;
    }

    if (GetMachineRoster()->GetTopology() == 0)
    {
        NetMessageLoadedGame message;
        u8 buffer[0xC8];
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0xC8);
        tDebugPrintManager::Print(DC_NETWORK, "Machine %d sending Loaded Game message\n",
            (s8)this->GetLocalMachineId());
        int count = this->GetNumMachines();
        for (s8 target = 0; target < count; ++target)
        {
            Send(target, buffer, size, true);
        }
    }
    else if ((s8)this->GetLocalMachineId() == 0)
    {
        mMachineLoadedGame[0] = 1;
    }
    else
    {
        NetMessageLoadedGameClient message;
        u8 buffer[0xC8];
        u32 size = gNetworkMessageRegistry->Serialize(&message, buffer, 0xC8);
        tDebugPrintManager::Print(DC_NETWORK, "Machine %d sending Loaded Game CLIENT message to HOST\n",
            (s8)this->GetLocalMachineId());
        Send(0, buffer, size, true);
    }
}

void NetworkSession::SetTournamentMode(u8 value)
{
    NetworkLobby* lobby = mSessionMode == 2 ? mLobby : 0;
    if (lobby != 0)
    {
        ((u8*)lobby)[9] = value;
    }
    mCupMode = value;
}

bool NetworkSession::IsConnectedPeer(u32 connection)
{
    if (!(connection != 0 && connection != 0xFFFFFFFF && connection != 0xFFFFFFFE))
    {
        return false;
    }

    NetworkMachineRoster* roster = GetMachineRoster();
    if (roster == 0)
    {
        return false;
    }

    if (NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        for (int machine = 0; machine < this->GetNumMachines(); ++machine)
        {
            int index
                = NetTournManager::Instance()->MachineIdxToTournamentIdx(
                    machine);
            if (index != -1 && roster->GetMachineAid(index) == connection)
            {
                return true;
            }
        }
    }
    else
    {
        for (int machine = 0; machine < roster->GetMachineCount(); ++machine)
        {
            if (roster->GetMachineAid(machine) == connection)
            {
                return true;
            }
        }
    }
    return false;
}

void NetworkSession::PopupNetworkError(int overlay)
{
    PopupNetworkErrorOverlay(this, overlay);
}

void NetworkSession::OnGameConnectionLost(u32 connection, int reason)
{
    switch (GetSessionState())
    {
    case 3:
        if (IsConnectedPeer(connection))
        {
            if (NetworkStatsManager::Instance()->ShouldRestoreDefaultDisconnectLoss())
            {
                NetworkStatsManager::Instance()->HandleDisconnect_8013243C(4);
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Connection %x not one of our peers, ignoring\n",
                connection);
        }
        break;

    case 5:
        if (IsConnectedPeer(connection))
        {
            NetworkStatsManager::Instance()->CalculateAndReportGameResult(4);
            PopupNetworkErrorOverlay(this, 0);
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Connection %x not one of our peers, ignoring\n",
                connection);
        }
        break;

    case 6:
        tDebugPrintManager::Print(DC_NETWORK,
            "WARNING: Ignored Connection Lost Stage "
            "ENetworkStage_GameEnded\n");
        break;

    case 1:
    case 2:
    case 4:
        break;

    default:
        tDebugPrintManager::Print(DC_NETWORK, "WARNING: Ignored Connection Lost %x unknown network stage "
                  "%d\n",
            connection, mSessionState);
        break;
    }
}

void NetworkSession::DisconnectOnlineMatch()
{
    if (GetSessionMode() == 2)
    {
        if (NetworkStatsManager::Instance() != 0)
        {
            NetworkStatsManager::Instance()->MarkDisconnectPending();
        }
        NetworkLobby* lobby = mSessionMode == 2 ? mLobby : 0;
        lobby->CloseConnections();
    }
}

void SHOnlineFriendsChooseSides::SetDraftMessage(NetMessageDraft message)
{
    mDraftMessage = message;
}

#include "Game/TweakValue.h"

static TweakBoolBinding s_NoPopupNetworkErrorTweak(
    "g_bNoPopupNetworkError", "Network", &g_bNoPopupNetworkError, true);

#include "Game/NetworkStatsManager.h"
