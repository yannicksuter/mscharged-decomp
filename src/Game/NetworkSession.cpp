#include "NL/nlSingleton.inl"
#include "NL/plat/SocketNetwork.h"
#include <dwc/dwc_account.h>
#include <dwc/dwc_error.h>
#include <dwc/dwc_init.h>
#include <dwc/dwc_main.h>
#include <dwc/dwc_nastime.h>
#include <dwc/dwc_report.h>
#include <revolution/os/OSThread.h>

#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/FE/FEAudio.h"
#include "NL/nlFunctionMemory.h"
#include "Game/Event.h"
#include "Game/Sys/debug.h"
#include "Game/NetworkRandom_803236CC.h"
#include "Game/FE/feMusic.h"
#include "Game/FriendManager.h"

#include "Game/AI/AIPad.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetTournManager.h"
#include "Game/Task/ResetTask.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/TweakRegistry.h"
#include "Game/main.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkStatsManager.h"

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlTicker.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/DB/StadiumInfo.h"
#include "unclassified/tu_80332DC0.h"
#include "unclassified/tu_80336B2C.h"
#include "unclassified/tu_80338898.h"
#include "Game/SH/SHOnlineFriendsChooseSides.h"
#include "unclassified/tu_8026F444.h"

bool g_bNoPopupNetworkError;
NetworkSession* g_pNetworkSession;
DWCFriendsMatchControl gDWCFriendsMatchControl;
void* gNetworkMemoryPool;
int gNetworkBuildNumberOverride;
int lbl_806E10FC;

float gNetworkLoginTimeout = 180.0f;
unsigned int gNetworkGameCodeR4QP = 0x52345150;
unsigned int gNetworkGameCodeR4QJ = 0x5234514A;
unsigned int gNetworkGameCodeR4QE = 0x52345145;

static MemoryAllocator s_NetworkAllocator;


extern u8 lbl_806E1000;

#include <string.h>
extern float lbl_806E4724;
extern float lbl_806E4728;

struct UnidentifiedVersionInfo
{
    /* 0x0 */ unsigned int mVersionWord;
    /* 0x4 */ u8 mUnidentified04;
};

extern int lbl_806DC8E4;
extern int lbl_80507070[][2];

#include "Game/GameSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Game.h"
#include "Game/FE/feDPD.h"

typedef void (NetworkSession::*UnidentifiedNetworkCallback)();

struct UnidentifiedNetworkCallbackRef
{
    UnidentifiedNetworkCallbackRef(UnidentifiedNetworkCallback callback)
        : mCallback(callback)
    {
    }

    UnidentifiedNetworkCallback mCallback;
};

struct UnidentifiedNetworkBinding
{
    UnidentifiedNetworkCallback mCallback;
    NetworkSession* mTarget;
};

class UnidentifiedNetworkDelegate
{
public:
    void* operator new(unsigned long size) { return AllocateFunctionMemory(size); }
    void operator delete(void* p)
    {
        FreeFunctionMemory(p, sizeof(UnidentifiedNetworkDelegate));
    }

    UnidentifiedNetworkDelegate(const UnidentifiedNetworkBinding& binding)
        : mCallback(binding.mCallback)
        , mTarget(binding.mTarget)
    {
    }

    virtual ~UnidentifiedNetworkDelegate() { }
    virtual void Execute() { (mTarget->*mCallback)(); }
    virtual UnidentifiedNetworkDelegate* Clone();

    /* 0x04 */ UnidentifiedNetworkCallback mCallback;
    /* 0x10 */ NetworkSession* mTarget;
}; // size: 0x14

struct UnidentifiedActionHandle
{
    /* 0x00 */ int mState;
    /* 0x04 */ UnidentifiedNetworkDelegate* mDelegate;
};

class UnidentifiedActionRegistry
{
public:
    virtual void RegistryVirtual00();
    virtual void RegistryVirtual04();
    virtual void Register(UnidentifiedActionHandle* handle, unsigned int* slot, int);
};

static inline UnidentifiedNetworkDelegate* NewNetworkDelegate(
    UnidentifiedNetworkBinding binding)
{
    return new UnidentifiedNetworkDelegate(binding);
}

static inline UnidentifiedNetworkDelegate* CreateNetworkDelegateInner(
    UnidentifiedNetworkCallbackRef callback, NetworkSession* target)
{
    UnidentifiedNetworkBinding binding;
    binding.mCallback = callback.mCallback;
    binding.mTarget = target;
    return NewNetworkDelegate(binding);
}

static inline UnidentifiedNetworkDelegate* CreateNetworkDelegate(
    UnidentifiedNetworkCallbackRef callback, NetworkSession* target)
{
    return CreateNetworkDelegateInner(callback, target);
}

// The two registration dispatchers live inside cGame's still-unreconstructed
// 0x49C..0x4C8 storage; the byte-offset casts express that target fact without
// inventing cGame layout the Game translation unit does not yet own.
static inline void RegisterNetworkAction(
    UnidentifiedActionHandle* handle, UnidentifiedNetworkCallbackRef callback,
    NetworkSession* session, UnidentifiedActionRegistry* registry,
    unsigned int* slot)
{
    handle->mState = 2;
    handle->mDelegate = CreateNetworkDelegate(callback, session);
    registry->Register(handle, slot, -1);
}

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
        mUnidentified244C = 0;
        mDirectSocket = 0;
        mTransport = 0;
        mLobby = 0;
        mElapsedTime = 0.0f;
        mUpdateCount = 0;
        mLastTicker = 0;
        mTournamentMode = 0;

        PushAllocator(&VirtualAllocator);
        if (GetRegion() == 0)
        {
            unsigned int poolSize = 0xB0000 - 0x2A90;
            void* pool = nlMalloc(poolSize, 8, false);
            gNetworkMemoryPool = pool;
            s_NetworkAllocator.Initialize(pool, poolSize);
        }
        else
        {
            unsigned int poolSize = 0x80000 - 0x37D0;
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

        mDirectSocket = new NetworkSocket_801246E4;
        mTransport = new NetworkTransport_8032CA4C;
        mLobby = new NetworkLobby;
        mStatsReporter = new NetworkStatsReporter_8012CE20;
        mRankingReporter = new NetworkRanking_8012D8F4;
    }
    else
    {
        this->InitializeMachines(1, 4);
    }

    mUnidentified246E[0] = 0;
    mUnidentified246E[1] = 0;
    mUnidentified246E[2] = 0;
    mUnidentified246E[3] = 0;
    mUnidentified2472 = 0;
    mUnidentified246C = 0;
    mUnidentified246D = 0;
    mUnidentified247C = 1;
    mUnidentified2474 = 0;
    mUnidentified2478 = 0;
    mOverlayRequest = 3;
    mPoppedOverlay = 3;
}

void NetworkSession::SendTournamentStartToEveryone()
{
    int machineCount = GetMachineRoster()->GetMachineCount();
    unsigned int stadiumRandom = fn_803236CC();
    int stadium = stadiumRandom % 10;
    if (lbl_806DC8E4 != 10)
    {
        stadium = lbl_806DC8E4;
    }

    int first = (s8)lbl_80507070[stadium][0];
    int second = (s8)lbl_80507070[stadium][1];
    if (first == -1)
    {
        first = fn_803236CC() & 0xF;
    }
    if (second == -1)
    {
        second = fn_803236CC() & 0xF;
    }
    if (fn_803236CC() % 1000 >= 500)
    {
        int swap = first;
        first = second;
        second = swap;
    }

    NetMessageTournamentStart message;
    message.mMachineIndex = 0;
    message.mMachineCount = machineCount;
    message.mStadium = stadium;
    message.mUnidentified0B = first;
    message.mUnidentified0C = second;
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
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0x64);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
    unsigned int randomSeed = fn_803236CC();
    unsigned int second = fn_803236CC();
    unsigned int third = fn_803236CC();
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
        message.mStadium = fn_803236CC() & 0xF;
        if (GetTweakBool("/user/media_build", 0))
        {
            while (!IsStadiumUnlocked(message.mStadium))
            {
                message.mStadium = fn_803236CC() & 0xF;
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
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0x64);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
    int count, UnidentifiedDraftEntry* entries, bool unused, u8 flag)
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
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0x200);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
        unsigned int size = lbl_806E2100->fn_8032C830(message, buffer, 0x200);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
    UnidentifiedNetworkMessage* message)
{
    int machineCount = GetMachineRoster()->GetMachineCount();
    for (int machine = 0; machine < machineCount; ++machine)
    {
        u8 buffer[0xFF];
        unsigned int size = lbl_806E2100->fn_8032C830(message, buffer, 0xFF);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
    UnidentifiedNetworkMessage* message)
{
    u8 buffer[0xFF];
    unsigned int size = lbl_806E2100->fn_8032C830(message, buffer, 0xFF);
    unsigned int aid = GetMachineRoster()->GetMachineAid(0);
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
        message.mUnidentified08[machine] =
            mLobby->GetMachineInfo(machine)->mUnidentified18;
    }

    for (int machine = 0; machine < mLobby->GetPlayerCount(); ++machine)
    {
        u8 buffer[0xFF];
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0xFF);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
    UnidentifiedNetworkMessage* message)
{
    for (int machine = 0; machine < GetMachineRoster()->GetMachineCount();
         ++machine)
    {
        u8 buffer[0xFF];
        unsigned int size = lbl_806E2100->fn_8032C830(message, buffer, 0xFF);
        unsigned int aid = GetMachineRoster()->GetMachineAid(machine);
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
    UnidentifiedNetworkMessage* message)
{
    u8 buffer[0xFF];
    unsigned int size = lbl_806E2100->fn_8032C830(message, buffer, 0xFF);
    unsigned int aid = GetMachineRoster()->GetMachineAid(0);
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
        dt = lbl_806E4724;
    }
    else
    {
        dt = nlGetTickerDifference(mLastTicker, nlGetTicker()) / lbl_806E4728;
    }

    unsigned int ticker = nlGetTicker();
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
            mTransport->fn_8032E890();
            if (mTransport->GetPlayerCount() >= 2)
            {
                UnidentifiedDraftEntry entries[8];
                for (int player = 0;
                     player < mTransport->GetPlayerCount(); ++player)
                {
                    UnidentifiedTransportPlayer* info =
                        mTransport->GetPlayerInfo(player);
                    nlStrToWcs(info->mName, entries[player].mName, 0xB);
                    memset(entries[player].mUnidentified32, 0, 0x4C);
                    entries[player].mIndex = player;
                    entries[player].mHead.mScore = 0;
                    entries[player].mHead.mDisplayRank = 0;
                    entries[player].mHead.mWins = 0;
                    entries[player].mHead.mLosses = 0;
                    entries[player].mHead.mUnidentified14 = 0;
                    entries[player].mHead.mDisplayRank =
                        info->mUnidentified0C;
                    entries[player].mHead.mWins =
                        info->mUnidentified10;
                    entries[player].mHead.mLosses =
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
        NetworkStatsManager_8012F378::Instance()->Update(dt);

        if (GetSessionState() == 1)
        {
            fn_80120838();
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
        NetworkStatsManager_8012F378::Instance()->CalculateAndReportGameResult(4);
        PopupNetworkErrorOverlay(this, mOverlayRequest);
        mOverlayRequest = 3;
    }
}

static void* NetworkAlloc(DWCAllocType name, u32 size, int align)
{
    return s_NetworkAllocator.Allocate(size, align, false);
}

static void NetworkFree(DWCAllocType name, void* ptr, u32 size)
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
    lbl_806E2100->fn_8032CA1C(0xD, this);
    lbl_806E2100->fn_8032CA1C(0xF, this);
    lbl_806E2100->fn_8032CA1C(0x12, this);
    lbl_806E2100->fn_8032CA1C(0x13, this);
    lbl_806E2100->fn_8032CA1C(0x14, this);
    lbl_806E2100->fn_8032CA1C(0x15, this);
    lbl_806E2100->fn_8032CA1C(0x0, this);
    lbl_806E2100->fn_8032CA1C(0x1, this);
    lbl_806E2100->fn_8032CA1C(0x8, this);
    lbl_806E2100->fn_8032CA1C(0x9, this);
    lbl_806E2100->fn_8032CA1C(0x1C, this);
    lbl_806E2100->fn_8032CA1C(0x1D, this);

    UnidentifiedVersionInfo info;
    info.mVersionWord = GetNetworkVersionWord();
    info.mUnidentified04 = 1;
    mDirectSocket->Initialize(&info, this);
    mDirectSocket->SetBroadcastEnabled(true);

    mTransport->fn_8032CBC8();
    mStatsReporter->Reset();
}

void NetworkSession::ShutdownLAN()
{
    NetworkDraft::Instance()->Reset(false);
    mStatsReporter->Close();
    NetworkStatsManager_8012F378::Instance()->Reset(false);
    mTransport->fn_8032CEAC();
    mDirectSocket->Shutdown();

    lbl_806E2100->fn_8032CA2C(0xD);
    lbl_806E2100->fn_8032CA2C(0xF);
    lbl_806E2100->fn_8032CA2C(0x12);
    lbl_806E2100->fn_8032CA2C(0x13);
    lbl_806E2100->fn_8032CA2C(0x14);
    lbl_806E2100->fn_8032CA2C(0x15);
    lbl_806E2100->fn_8032CA2C(0x0);
    lbl_806E2100->fn_8032CA2C(0x1);
    lbl_806E2100->fn_8032CA2C(0x8);
    lbl_806E2100->fn_8032CA2C(0x9);
    lbl_806E2100->fn_8032CA2C(0x1C);
    lbl_806E2100->fn_8032CA2C(0x1D);

    mSessionMode = 0;
}

void NetworkSession::InitializeOnline()
{
    if (!SocketNetworkIsStarted())
    {
        return;
    }

    mUnidentified24A4 = 0;

    unsigned int gameCode = gNetworkGameCodeR4QP;
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

    lbl_806E2100->fn_8032CA1C(0xD, this);
    lbl_806E2100->fn_8032CA1C(0xF, this);
    lbl_806E2100->fn_8032CA1C(0x12, this);
    lbl_806E2100->fn_8032CA1C(0x13, this);
    lbl_806E2100->fn_8032CA1C(0x14, this);
    lbl_806E2100->fn_8032CA1C(0x15, this);
    lbl_806E2100->fn_8032CA1C(0x19, this);
    lbl_806E2100->fn_8032CA1C(0x1A, this);
    lbl_806E2100->fn_8032CA1C(0x1B, this);
    lbl_806E2100->fn_8032CA1C(0x0, this);
    lbl_806E2100->fn_8032CA1C(0x1, this);
    lbl_806E2100->fn_8032CA1C(0x8, this);
    lbl_806E2100->fn_8032CA1C(0x9, this);
    lbl_806E2100->fn_8032CA1C(0x1C, this);
    lbl_806E2100->fn_8032CA1C(0x1D, this);

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
    mUnidentified24A4 = fn_80120440();
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

bool NetworkSession::fn_80120440()
{
    GameInfoManager* gameInfo;

    SetSessionState(true);
    mLoginStage = 1;
    mLoginStartTime = 0.0f;
    lbl_806E1000 = 1;

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
    if (NetworkStatsManager_8012F378::Instance()->UsesEuropeanRankings())
    {
        mLoginStage = 3;
        if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(4))
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
        if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(2))
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

void NetworkSession::fn_80120838()
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
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            NetworkRankingMeta* record = NetworkStatsManager_8012F378::Instance()->mHasLocalStats[2] != 0
                                           ? &NetworkStatsManager_8012F378::Instance()->mLocalStats[2]
                                           : 0;
            if (record != 0)
            {
                if (IsNewNetworkSeason(record))
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Starting new friends season clearing stats\n");
                    NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(2, 0);
                    mLoginStage = 4;
                }
                else if (gNetworkMiiChanged != 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Detected Mii change putting friends unchanged "
                        "win/loss stats\n");
                    NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(2, 1);
                    mLoginStage = 4;
                }
                else
                {
                    mLoginStage = 5;
                    if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(2))
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
                NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(2, 0);
                mLoginStage = 4;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting friends stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 4:
        if (NetworkStatsManager_8012F378::Instance()->mScoreRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mScoreRequestSucceeded != 0)
        {
            mLoginStage = 5;
            if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(2))
            {
                tDebugPrintManager::Print(DC_NETWORK, "Error getting nearby stats\n");
            }
            NetworkStatsManager_8012F378::Instance()->SubmitJob(0xA);
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                NetworkStatsManager_8012F378::Instance()->mScoreCategory);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mScoreRequestComplete = 0;
        break;

    case 5:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            NetworkRankingMeta* record = NetworkStatsManager_8012F378::Instance()->mHasLocalStats[0] != 0
                                           ? &NetworkStatsManager_8012F378::Instance()->mLocalStats[0]
                                           : 0;
            if (record != 0)
            {
                if (IsNewNetworkSeason(record))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Starting new season clearing stats\n");
                    NetworkStatsManager_8012F378::Instance()->CommitPendingOnlineTotals(record);
                    NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(0, 0);
                    mLoginStage = 6;
                }
                else if (gNetworkMiiChanged != 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Detected Mii change putting own unchanged win/loss "
                        "stats\n");
                    NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(0, 1);
                    mLoginStage = 6;
                }
                else
                {
                    mLoginStage = 8;
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Login: Transition to "
                        "ELoggingInStage_GettingSODNearbyStats\n");
                    if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(0))
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
                NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(0, 0);
                mLoginStage = 6;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 6:
        if (NetworkStatsManager_8012F378::Instance()->mScoreRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mScoreRequestSucceeded != 0)
        {
            mLoginStage = 7;
            if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(2))
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
                NetworkStatsManager_8012F378::Instance()->mScoreCategory);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mScoreRequestComplete = 0;
        break;

    case 7:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 8;
            tDebugPrintManager::Print(DC_NETWORK,
                "Login: Transition to "
                "ELoggingInStage_GettingSODNearbyStats\n");
            if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(0))
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
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 8:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            NetworkRankingMeta* record = NetworkStatsManager_8012F378::Instance()->mHasLocalStats[1] != 0
                                           ? &NetworkStatsManager_8012F378::Instance()->mLocalStats[1]
                                           : 0;
            if (record != 0)
            {
                if (IsNewNetworkDay(record))
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Starting new day clearing stats\n");
                    NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(1, 0);
                    mLoginStage = 9;
                }
                else if (gNetworkMiiChanged != 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Detected Mii change putting SOD unchanged win/loss "
                        "stats\n");
                    NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(1, 1);
                    mLoginStage = 9;
                }
                else if (!NetworkStatsManager_8012F378::Instance()->UsesEuropeanRankings())
                {
                    mLoginStage = 0xB;
                    if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(4))
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
                    if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(1))
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
                NetworkStatsManager_8012F378::Instance()->PostResetMyPlayerStats(1, 0);
                mLoginStage = 9;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting SOD nearbystats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 9:
        if (NetworkStatsManager_8012F378::Instance()->mScoreRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mScoreRequestSucceeded != 0)
        {
            mLoginStage = 0xA;
            tDebugPrintManager::Print(DC_NETWORK,
                "Login: Transition to "
                "ELoggingInStage_ReGettingSODNearbyStats\n");
            if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(0))
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
                NetworkStatsManager_8012F378::Instance()->mScoreCategory);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mScoreRequestComplete = 0;
        break;

    case 0xA:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            if (!NetworkStatsManager_8012F378::Instance()->UsesEuropeanRankings())
            {
                mLoginStage = 0xB;
                if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(4))
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
                if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(1))
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
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 0xB:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 0xC;
            if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(1))
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
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 0xC:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 0xD;
            if (!NetworkStatsManager_8012F378::Instance()->RequestRankings(3))
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
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
        break;

    case 0xD:
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete == 0)
        {
            break;
        }
        if (NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestSucceeded != 0)
        {
            mLoginStage = 0xE;
            g_pNetworkSession->SetSessionState(2);
            mLoginListener->OnStatsResult(true);
            NetworkStatsManager_8012F378::Instance()->RefreshSaveState_801314D0();
            gNetworkMiiChanged = 0;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Error getting SEASON TOP stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        NetworkStatsManager_8012F378::Instance()->mLeaderboardRequestComplete = 0;
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
    NetworkStatsManager_8012F378::Instance()->Reset(false);
    mLobby->UnregisterMessageReceiver();
    mDirectSocket->Shutdown();
    DWC_Shutdown();
    mUnidentified24A5 = 0;
    SocketNetworkShutdown();

    lbl_806E2100->fn_8032CA2C(0xD);
    lbl_806E2100->fn_8032CA2C(0xF);
    lbl_806E2100->fn_8032CA2C(0x12);
    lbl_806E2100->fn_8032CA2C(0x13);
    lbl_806E2100->fn_8032CA2C(0x14);
    lbl_806E2100->fn_8032CA2C(0x15);
    lbl_806E2100->fn_8032CA2C(0x19);
    lbl_806E2100->fn_8032CA2C(0x1A);
    lbl_806E2100->fn_8032CA2C(0x1B);
    lbl_806E2100->fn_8032CA2C(0x0);
    lbl_806E2100->fn_8032CA2C(0x1);
    lbl_806E2100->fn_8032CA2C(0x8);
    lbl_806E2100->fn_8032CA2C(0x9);
    lbl_806E2100->fn_8032CA2C(0x1C);
    lbl_806E2100->fn_8032CA2C(0x1D);
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

NetworkSocket_801246E4* NetworkSession::GetDirectSocket()
{
    return mDirectSocket;
}

UnidentifiedMachineRoster* NetworkSession::GetMachineRoster()
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

NetworkTransport_8032CA4C* NetworkSession::GetTransport()
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

NetworkStatsInterface* NetworkSession::fn_8012170C()
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

NetworkStatsReporter_8012CE20* NetworkSession::fn_80121738()
{
    if (mSessionMode == 1)
    {
        return mStatsReporter;
    }
    return 0;
}

NetworkRanking_8012D8F4* NetworkSession::fn_80121754()
{
    if (mSessionMode == 2)
    {
        return mRankingReporter;
    }
    return 0;
}

void NetworkSession::ListenerVirtual00(void* buffer, int size)
{
    lbl_806E2100->fn_8032C8CC(-2, static_cast<u8*>(buffer), size);
}

void NetworkSession::ListenerVirtual04(
    int source, void* buffer, int size, bool)
{
    lbl_806E2100->fn_8032C8CC(source, static_cast<u8*>(buffer), size);
}

void NetworkSession::ListenerVirtual08(unsigned int connection, u8* address)
{
    UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster == 0)
    {
        return;
    }
    NetworkSocket_801246E4* socket = g_pNetworkSessionBase->GetDirectSocket();
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

void NetworkSession::ListenerVirtual0C(unsigned int connection, int result)
{
    tDebugPrintManager::Print(DC_NETWORK, "Connected callback result %d\n", result);
    UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->OnConnected(connection, result);
    }
}

void NetworkSession::ListenerVirtual10(unsigned int connection, int reason)
{
    tDebugPrintManager::Print(DC_NETWORK, "Connection closed reason %d\n", reason);
    fn_80124038(connection, reason);
    UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
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

struct RecordedGameConfig
{
    /* 0x00 */ int mStadium;
    /* 0x04 */ int mHomeTeam;
    /* 0x08 */ int mHomeSidekicks[3];
    /* 0x14 */ int mAwayTeam;
    /* 0x18 */ int mAwaySidekicks[3];
    /* 0x24 */ GameplaySettings::eSkillLevel mSkillLevel;
    /* 0x28 */ int mWinBy;
    /* 0x2C */ int mGameTime;
    /* 0x30 */ int mGameGoals;
    /* 0x34 */ int mBestSeries;
    /* 0x38 */ s16 mPlayingSides[16];
}; // size: 0x58

static void CaptureRecordedGameConfig(RecordedGameConfig* config);
static void ApplyRecordedGameConfig(RecordedGameConfig* config);


// Two local pad indices for the machine's one or two local players.

static inline void NotifyGameStarted(NetworkSession* session)
{
    UnidentifiedNetworkSyncState* state = lbl_806E2168;
    int count = session->GetNumMachines();
    fn_80338AD8(state, (s8)session->GetLocalMachineId(), count);
}

static inline void RecordGameConfig(
    NetworkSession* session, unsigned int seed,
    RecordedGameConfig* config)
{
    UnidentifiedNetGameState* state = lbl_806E2164;
    if (state->mRecordingEnabled != 0)
    {
        int count = session->GetNumMachines();
        fn_803380F4(
            state, (s8)session->GetLocalMachineId(), count, seed, config, 0x58);
    }
}

typedef void (NetworkSession::*UnidentifiedSessionCallback)();

static inline void RegisterLoadedGameActions(NetworkSession* session)
{
    UnidentifiedActionHandle first;
    RegisterNetworkAction(
        &first,
        UnidentifiedNetworkCallbackRef(
            &NetworkSession::fn_801239F8),
        session, (UnidentifiedActionRegistry*)((u8*)g_pGame + 0x49C),
        &session->mUnidentified2464);

    UnidentifiedActionHandle second;
    RegisterNetworkAction(
        &second,
        UnidentifiedNetworkCallbackRef(
            &NetworkSession::fn_801239FC),
        session, (UnidentifiedActionRegistry*)((u8*)g_pGame + 0x4C8),
        &session->mUnidentified2468);

    if (NetTournManager::Instance()->mState != 0)
    {
        NetTournManager::Instance()->NotifyGameStarted();
    }

    if (second.mState == 2 && second.mDelegate != 0)
    {
        delete second.mDelegate;
    }
    second.mState = 0;
    if (first.mState == 2 && first.mDelegate != 0)
    {
        delete first.mDelegate;
    }
    first.mState = 0;
}

int NetworkSession::ReceiverVirtual00(
    UnidentifiedNetworkMessage* message)
{
    UnidentifiedMachineRoster* roster = GetMachineRoster();
    int machine = (s8)roster->MachineIdxFromConnection(message->mUnidentified04);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mUnidentified04);
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
        if (lbl_806E2164->mPlaybackEnabled != 0)
        {
            // Retail re-evaluates the playback flag inside the branch,
            // matching an inlined predicate called twice.
            if (lbl_806E2164->mPlaybackEnabled != 0
                && fn_80338284(lbl_806E2164))
            {
            UnidentifiedGameStartInfo* info =
                (UnidentifiedGameStartInfo*)&lbl_806E2164->mConfigSize;
            fn_80332EC0(lbl_806E2164->mRandomSeed);
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
            fn_803330AC()->Reset(0);
            g_pNetworkSessionBase->BaseVirtual3C(info);
            UnidentifiedNetworkSyncState* state = lbl_806E2168;
            int count = g_pNetworkSessionBase->GetNumMachines();
            fn_80338AD8(state, (s8)g_pNetworkSessionBase->GetLocalMachineId(), count);
            }
        }
        else
        {
            BaseVirtual44((NetMessageGameStart*)message);
        }
        mSessionState = 4;
        GameSceneManager::Instance()->fn_801C5FB8((SceneList)0x1D);
        for (int component = 0; component < 4; ++component)
        {
            gFEPointerInstances[component]->SetActiveSlide("waiting", true, false);
        }
        GameSceneManager::Instance()->PushLoadingScene(false);
        break;

    case 0x14:
        NetTournManager::Instance()->TransitionOnlineMenuToTournament(
            static_cast<NetMessageTournamentStart*>(message));
        break;

    case 0x15:
        if (((NetMessageDraft*)message)->mUnidentified0A != 0)
        {
            SHOnlineFriendsChooseSides* scene = static_cast<SHOnlineFriendsChooseSides*>(
                GameSceneManager::Instance()->Push((SceneList)0x38, SCREEN_FORWARD, true));
            scene->SetDraftMessage(*static_cast<NetMessageDraft*>(message));
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
            NetworkStatsManager_8012F378::Instance()->ResetPregameDisconnectState();
        }
        break;

    case 0x19:
    {
        BaseSceneHandler* scene = GameSceneManager::Instance()->GetScene((SceneList)0x38);
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
        TU8026F444Scene* scene = static_cast<TU8026F444Scene*>(
            GameSceneManager::Instance()->Push((SceneList)0x39, SCREEN_FORWARD, true));
        scene->fn_8026F7F8(static_cast<NetMessageCheckConnection*>(message));
        break;
    }

    case 0x1B:
    {
        TU8026F444Scene* scene = static_cast<TU8026F444Scene*>(
            GameSceneManager::Instance()->GetScene((SceneList)0x39));
        if (scene != 0)
        {
            scene->fn_8026FF28(static_cast<NetworkMessageType27_8050B750*>(message));
        }
        break;
    }

    case 0xF:
        tDebugPrintManager::Print(DC_NETWORK, "Received loaded game message from %d\n", machine);
        mUnidentified246E[machine] = 1;
        break;

    case 0x12:
        tDebugPrintManager::Print(DC_NETWORK, "Received loaded game CLIENT message from %d\n", machine);
        mUnidentified246E[machine] = 1;
        break;

    case 0x13:
    {
        tDebugPrintManager::Print(DC_NETWORK, "Received loaded game EVERYONE message from %d\n", machine);
        for (int index = 0; index < this->GetNumMachines(); ++index)
        {
            mUnidentified246E[index] = 1;
        }
        RegisterLoadedGameActions(this);
        mSessionState = 5;
        break;
    }

    case 0x0:
    case 0x1:
        if (lbl_806E2164->mPlaybackEnabled != 0)
        {
            break;
        }
        if (machine < 0 || machine >= this->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded message type %d because from unknown connection "
                "%x\n",
                (u8)message->GetType(), message->mUnidentified04);
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
            fn_803330AC()->RouterVirtual28(
                machine, (NetworkMessageType0_80533B7C*)message);
        }
        else
        {
            NetworkMessageType1_80533B68* bundled =
                (NetworkMessageType1_80533B68*)message;
            fn_803330AC()->RouterVirtual28(machine, &bundled->mMessage0);
            fn_803330AC()->RouterVirtual28(machine, &bundled->mMessage1);
        }
        break;

    case 0x8:
    case 0x9:
        if (lbl_806E2164->mPlaybackEnabled != 0)
        {
            break;
        }
        if (machine < 0 || machine >= this->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded message type %d because from unknown connection "
                "%x\n",
                (u8)message->GetType(), message->mUnidentified04);
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
            fn_803330AC()->RouterVirtual2C(
                machine, (NetworkMessageType8_80533BA4*)message);
        }
        else
        {
            NetworkMessageType9_80533B90* bundled =
                (NetworkMessageType9_80533B90*)message;
            fn_803330AC()->RouterVirtual2C(machine, &bundled->mMessage0);
            fn_803330AC()->RouterVirtual2C(machine, &bundled->mMessage1);
        }
        break;

    case 0x1C:
    {
        if ((s8)this->GetLocalMachineId() != 0)
        {
            break;
        }
        u8 bit = 1 << (s8)((NetMessagePauseRequest_8050AD7C*)message)
                              ->mMachineIndex;
        if (((NetMessagePauseRequest_8050AD7C*)message)->mPaused != 0)
        {
            mUnidentified246C = mUnidentified246C | bit;
        }
        else
        {
            mUnidentified246C = mUnidentified246C & ~bit;
        }

        NetMessagePauseResponse_8050AD68 response;
        response.mMachineMask = mUnidentified246C;
        u8 buffer[0x32];
        unsigned int size = lbl_806E2100->fn_8032C830(&response, buffer, 0x32);
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
        mUnidentified246D =
            ((NetMessagePauseResponse_8050AD68*)message)->mMachineMask;
        break;
    }
    return 1;
}

void NetworkSession::BaseVirtual3C(UnidentifiedGameStartInfo* info)
{
    int myId = info->mMyMachineId;
    int count = info->mMachineCount;
    mMachineCount = count;
    mLocalMachineId = myId;

    UnidentifiedNetworkPeer* peer = mPeers;
    for (int machine = 0; machine < mMachineCount; ++machine)
    {
        peer->mMachineId = machine;
        int players = info->mPlayerCounts[0];
        peer->mUnidentified004 = players;
        for (int player = 0; player < players; ++player)
        {
            fn_80336D50(fn_80336B6C(peer, player), peer, (s8)player, player);
        }
        fn_80336BE0(peer);
        ++peer;
        info = (UnidentifiedGameStartInfo*)((u8*)info + 4);
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
    config->mWinBy = settings->WinBy;
    config->mGameTime = settings->GameTime;
    config->mGameGoals = settings->GameGoals;
    config->mBestSeries = settings->BestSeries;

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
    manager->mUserInfo.mGameplayOptions.WinBy = config->mWinBy;
    manager->mUserInfo.mGameplayOptions.GameTime = config->mGameTime;
    manager->mUserInfo.mGameplayOptions.GameGoals = config->mGameGoals;
    manager->mUserInfo.mGameplayOptions.BestSeries = config->mBestSeries;

    manager = GameInfoManager::GetInstance();
    manager->mCurGameSettings = manager->mUserInfo.mGameplayOptions;

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
        mPeers[machine].mUnidentified004 = players;

        if (machine == mLocalMachineId)
        {
            for (int player = 0; player < players; ++player)
            {
                fn_80336D50(
                    fn_80336B6C(&mPeers[machine], player), &mPeers[machine],
                    (s8)player, gOnlineLocalControllerIndices[player]);
            }
        }
        else
        {
            for (int player = 0; player < players; ++player)
            {
                fn_80336D50(
                    fn_80336B6C(&mPeers[machine], player), &mPeers[machine],
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

    unsigned int seed = message->mRandomSeed;
    fn_80332EC0(seed);
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
        unsigned int side = 0;
        if (message->mUnidentified1B == 0)
        {
            side = NetworkDraft::Instance()->GetDraftTeam(0)
                       ->mPlayers[0]
                       .mDisconnected
                != 0;
        }
        for (int machine = 0; machine < mMachineCount; ++machine)
        {
            int players = mPeers[machine].mUnidentified004;
            for (int player = 0; player < players; ++player)
            {
                GameInfoManager* manager = GameInfoManager::GetInstance();
                manager->SetPlayingSide(
                    (u16)(s8)fn_80336F68((s8)player, (s8)machine), (s16)side);
            }
            side = side == 0;
        }
    }
    else
    {
        u8* sides = (u8*)message;
        for (int machine = 0; machine < mMachineCount; ++machine)
        {
            int players = mPeers[machine].mUnidentified004;
            for (int player = 0; player < players; ++player)
            {
                GameInfoManager* manager = GameInfoManager::GetInstance();
                manager->SetPlayingSide(
                    (u16)(s8)fn_80336F68((s8)player, (s8)machine),
                    (s8)sides[player + 0xB]);
            }
            sides += 2;
        }
    }

    tDebugPrintManager::Print(DC_NETWORK, "StartNetworkedGame: Random seed %x NumMachines:%d "
              "MyMachineID:%d\n",
        seed, mMachineCount, mLocalMachineId);
    fn_803330AC()->Reset(0);
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
    fn_80332EC8();
    fn_80337FF0(lbl_806E2164, 0);
    fn_80338900(lbl_806E2168, 0);
    fn_803330AC()->Reset(0);
    lbl_806E2138->fn_8033288C();

    mUnidentified247C = mUnidentified247C + 1;
    unsigned int seed;
    if (mUnidentified247C == 2)
    {
        seed = mUnidentified2474;
    }
    else
    {
        seed = mUnidentified2478;
    }
    fn_80332EC0(seed);

    NotifyGameStarted(this);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    RecordGameConfig(this, seed, &config);
}

void RestartSinglePlayerGame()
{
    GetFixedUpdateTask()->Reset();
    fn_80332EC8();
    fn_80337FF0(lbl_806E2164, 0);
    fn_80338900(lbl_806E2168, 0);
    fn_803330AC()->Reset(0);
    lbl_806E2138->fn_8033288C();

    unsigned int seed = fn_803236CC();
    fn_80332EC0(seed);

    UnidentifiedNetworkSyncState* state = lbl_806E2168;
    int count = g_pNetworkSessionBase->GetNumMachines();
    fn_80338AD8(state, (s8)g_pNetworkSessionBase->GetLocalMachineId(), count);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    UnidentifiedNetGameState* record = lbl_806E2164;
    if (record->mRecordingEnabled != 0)
    {
        int machines = g_pNetworkSessionBase->GetNumMachines();
        fn_803380F4(
            record, (s8)g_pNetworkSessionBase->GetLocalMachineId(), machines, seed, &config,
            0x58);
    }
}

void StartSinglePlayerGame()
{
    g_pNetworkSessionBase->Shutdown();
    g_pNetworkSessionBase->InitializeMachines(1, 4);

    UnidentifiedNetworkPeer* peer = g_pNetworkSessionBase->GetPeer(0);
    for (int player = 0; player < (int)peer->mUnidentified004; ++player)
    {
        fn_80336D50(fn_80336B6C(peer, player), peer, (s8)player, player);
    }
    AIPadManager::Startup();

    unsigned int seed = fn_803236CC();
    fn_80332EC0(seed);
    tDebugPrintManager::Print(DC_NETWORK, "StartSinglePlayerGame: Set random seed to %x\n", seed);
    fn_803330AC()->Reset(0);

    UnidentifiedNetworkSyncState* state = lbl_806E2168;
    int count = g_pNetworkSessionBase->GetNumMachines();
    fn_80338AD8(state, (s8)g_pNetworkSessionBase->GetLocalMachineId(), count);

    RecordedGameConfig config;
    CaptureRecordedGameConfig(&config);
    UnidentifiedNetGameState* record = lbl_806E2164;
    if (record->mRecordingEnabled != 0)
    {
        int machines = g_pNetworkSessionBase->GetNumMachines();
        fn_803380F4(
            record, (s8)g_pNetworkSessionBase->GetLocalMachineId(), machines, seed, &config,
            0x58);
    }
}

void PlaybackRecordedGame()
{
    if (lbl_806E2164->mPlaybackEnabled == 0)
    {
        return;
    }
    if (!fn_80338284(lbl_806E2164))
    {
        return;
    }

    UnidentifiedGameStartInfo* info =
        (UnidentifiedGameStartInfo*)&lbl_806E2164->mConfigSize;
    fn_80332EC0(lbl_806E2164->mRandomSeed);
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
    fn_803330AC()->Reset(0);
    g_pNetworkSessionBase->BaseVirtual3C(info);
    UnidentifiedNetworkSyncState* state = lbl_806E2168;
    int count = g_pNetworkSessionBase->GetNumMachines();
    fn_80338AD8(state, (s8)g_pNetworkSessionBase->GetLocalMachineId(), count);
}

void NetworkSession::BaseVirtual48(int reason)
{
    mUnidentified244C = reason;
    mSessionState = 6;
    DisconnectEventOwner(&mUnidentified2464);
    DisconnectEventOwner(&mUnidentified2468);

    if (mTournamentMode != 0)
    {
        NetTournManager::Instance()->ResetGameProgressUpdateTimer(
            mUnidentified244C);
    }
    else
    {
        UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
        if (roster != 0)
        {
            roster->Shutdown(true);
        }
    }

    if (GetSessionMode() == 2)
    {
        NetworkStatsManager_8012F378::Instance()->BeginOnlineGame_80131DB4();
    }
}

int NetworkSession::Send(
    s8 player, void* buffer, int size, bool reliable)
{
    NetworkSocket_801246E4* socket = GetDirectSocket();
    if ((int)player == mLocalMachineId)
    {
        socket->Receive(buffer, size);
        return 1;
    }

    UnidentifiedMachineRoster* roster = GetMachineRoster();
    if (roster == 0)
    {
        return 0;
    }

    int machine = player;
    if (NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        machine = (s8)NetTournManager::Instance()->MachineIdxToTournamentIdx(
            (s8)machine);
    }

    unsigned int aid = roster->GetMachineAid((s8)machine);
    if (aid == 0)
    {
        return 0;
    }
    socket->Send(aid, buffer, size, reliable);
    return 1;
}

int NetworkSession::fn_80123314()
{
    if (GetSessionMode() == 0)
    {
        return 0;
    }
    return lbl_806E2164->mPlaybackReady == 0;
}

int NetworkSession::fn_80123360()
{
    if (mSessionState == 5)
    {
        return 1;
    }
    if (!g_pNetworkSession->fn_80123314())
    {
        return 0;
    }

    if (mOverlayRequest == 3)
    {
        for (int machine = 0; machine < this->GetNumMachines(); ++machine)
        {
            if (machine == (s8)this->GetLocalMachineId())
            {
                continue;
            }
            unsigned int aid;
            if ((s8)machine == mLocalMachineId)
            {
                aid = 0xFFFFFFFF;
            }
            else
            {
                UnidentifiedMachineRoster* roster = GetMachineRoster();
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
        if (mUnidentified246E[machine] == 0)
        {
            return 0;
        }
    }

    tDebugPrintManager::Print(DC_NETWORK, "Game has loaded for everyone!\n");
    if (GetMachineRoster()->RosterVirtual08() == 1 && (s8)this->GetLocalMachineId() == 0)
    {
        NetMessageLoadedGameEveryone message;
        u8 buffer[0xC8];
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0xC8);
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

void NetworkSession::fn_801239F8()
{
}

void NetworkSession::fn_801239FC()
{
}

u8 NetworkSession::fn_80123A00()
{
    return mUnidentified246D;
}

void NetworkSession::fn_80123A08()
{
    int ready;
    if (GetSessionMode() == 0)
    {
        ready = 0;
    }
    else
    {
        ready = lbl_806E2164->mPlaybackReady == 0;
    }

    if (ready == 0)
    {
        mSessionState = 5;
        RegisterLoadedGameActions(this);
        return;
    }

    if (GetMachineRoster()->RosterVirtual08() == 0)
    {
        NetMessageLoadedGame message;
        u8 buffer[0xC8];
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0xC8);
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
        mUnidentified246E[0] = 1;
    }
    else
    {
        NetMessageLoadedGameClient message;
        u8 buffer[0xC8];
        unsigned int size = lbl_806E2100->fn_8032C830(&message, buffer, 0xC8);
        tDebugPrintManager::Print(DC_NETWORK, "Machine %d sending Loaded Game CLIENT message to HOST\n",
            (s8)this->GetLocalMachineId());
        Send(0, buffer, size, true);
    }
}

void NetworkSession::SetTournamentMode(u8 tournament)
{
    NetworkLobby* lobby = mSessionMode == 2 ? mLobby : 0;
    if (lobby != 0)
    {
        lobby->mTournamentMode = tournament;
    }
    mTournamentMode = tournament;
}

bool NetworkSession::fn_80123E70(unsigned int connection)
{
    if (connection == 0 || connection == 0xFFFFFFFF
        || connection == 0xFFFFFFFE)
    {
        return false;
    }

    UnidentifiedMachineRoster* roster = GetMachineRoster();
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

void NetworkSession::fn_80123FBC(int overlay)
{
    PopupNetworkErrorOverlay(this, overlay);
}

void NetworkSession::fn_80124038(unsigned int connection, int reason)
{
    switch (GetSessionState())
    {
    case 3:
        if (fn_80123E70(connection))
        {
            if (NetworkStatsManager_8012F378::Instance()->ShouldRestoreDefaultDisconnectLoss())
            {
                NetworkStatsManager_8012F378::Instance()->HandleDisconnect_8013243C(4);
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Connection %x not one of our peers, ignoring\n",
                connection);
        }
        break;

    case 5:
        if (fn_80123E70(connection))
        {
            NetworkStatsManager_8012F378::Instance()->CalculateAndReportGameResult(4);
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

void NetworkSession::fn_801241C8()
{
    if (GetSessionMode() == 2)
    {
        if (NetworkStatsManager_8012F378::Instance() != 0)
        {
            NetworkStatsManager_8012F378::Instance()->MarkDisconnectPending();
        }
        NetworkLobby* lobby = mSessionMode == 2 ? mLobby : 0;
        lobby->CloseConnections();
    }
}

#include "Game/TweakValue.h"

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

static TweakValueBoolImpl_804F4538 s_NoPopupNetworkErrorTweak(
    "g_bNoPopupNetworkError", "Network", &g_bNoPopupNetworkError, true);

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

UnidentifiedNetworkDelegate* UnidentifiedNetworkDelegate::Clone()
{
    return new UnidentifiedNetworkDelegate(*this);
}
#include "Game/NetworkStatsManager.h"
