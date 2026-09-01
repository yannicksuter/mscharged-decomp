#include "Game/NetworkSession.h"

#include "Game/AI/AIPad.h"
#include "Game/DB/SaveLoad.h"
#include "Game/GameInfo.h"
#include "Game/ResetTask.h"
#include "Game/NetworkMessages.h"

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "types.h"

// Tournament-side state owned by the 0x80127xxx translation unit.
struct UnidentifiedTournamentState
{
    /* 0x000 */ u8 mUnidentified000[0x8];
    /* 0x008 */ int mUnidentified008;
    /* 0x00C */ u8 mUnidentified00C[0x925 - 0xC];
    /* 0x925 */ u8 mUnidentified925;
};

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

int lbl_806E10D0;
int lbl_806E10D4;
u8 lbl_806E10D8;
int lbl_806E10DC;
double lbl_806E10E0;
bool lbl_806E10E8;
UnidentifiedNetworkSession* lbl_806E10EC;
int lbl_806E10F0;
void* lbl_806E10F4;
int lbl_806E10F8;
int lbl_806E10FC;

float lbl_806DC888 = 180.0f;
u32 lbl_806DC88C = 0x52345150;
u32 lbl_806DC890 = 0x5234514A;
u32 lbl_806DC894 = 0x52345145;

extern UnidentifiedNetworkSession* lbl_806E20D8;
extern void* lbl_806E2100;

static MemoryAllocator s_NetworkAllocator;

extern "C" u32 fn_8032C830(void* codec, void* message, void* buffer, int size);
extern "C" int fn_8004F594(int channel, const char* format, ...);
extern "C" void fn_80338C2C(UnidentifiedNetworkSession* session, int, int);
extern "C" u32 fn_803236CC();
extern "C" void fn_8012785C(int count, u8* seedings);
extern "C" void* fn_80135D5C(NetworkLobby_80133634* lobby, int index);
extern "C" void* fn_8012B288();
extern "C" int* fn_8012C6E0(void* draft, int team);
extern "C" int fn_8025BD88();
extern "C" int fn_802C2C84(const char* path, int);
extern "C" int fn_801B63C8(u8 stadium);
extern u8 lbl_806E18D4;

extern "C" void fn_80374174();
extern "C" int fn_80374308();
extern "C" void fn_8032CA1C(void* codec, int type, UnidentifiedNetworkMessageReceiver* receiver);
extern "C" void fn_8032CA2C(void* codec, int type);
extern "C" void fn_8032CBC8(NetworkTransport_8032CA4C* transport);
extern "C" void fn_8032C8CC(void* codec, int type, u32 a, void* b);
extern "C" void fn_8032CEAC(NetworkTransport_8032CA4C* transport);
extern "C" void fn_8012CF5C(NetworkStatsReporter_8012CE20* reporter);
extern "C" void fn_8012CFEC(NetworkStatsReporter_8012CE20* reporter);
extern "C" void fn_8012B290(void*, int);
struct UnidentifiedStatsTracker
{
    /* 0x00 */ u8 mUnidentified00[0x4];
    /* 0x04 */ u8 mUnidentified04;
    /* 0x05 */ u8 mUnidentified05;
    /* 0x06 */ u8 mUnidentified06[0x6];
    /* 0x0C */ u8 mUnidentified0C;
    /* 0x0D */ u8 mUnidentified0D;
    /* 0x0E */ u8 mUnidentified0E[0x2];
    /* 0x10 */ int mUnidentified10;
    /* 0x14 */ u8 mUnidentified14[0xC];
    /* 0x20 */ u8 mUnidentified20;
    /* 0x21 */ u8 mUnidentified21;
    /* 0x22 */ u8 mUnidentified22;
    /* 0x23 */ u8 mUnidentified23;
    /* 0x24 */ u8 mUnidentified24[0x18];
    /* 0x3C */ u8 mUnidentified3C[0x18];
    /* 0x54 */ u8 mUnidentified54[0x18];
};

extern "C" UnidentifiedStatsTracker* fn_8012F8F8();
extern "C" int fn_80132C74(void* record);
extern "C" int fn_80132D54(void* record);
extern "C" void fn_8012FFB0(UnidentifiedStatsTracker* tracker, void* record);
extern "C" void fn_801304D0(UnidentifiedStatsTracker* tracker, int category, int miiChanged);
extern "C" void fn_80131464(UnidentifiedStatsTracker* tracker, int);
extern "C" void fn_801314D0(UnidentifiedStatsTracker* tracker);
extern "C" void fn_80136140(void* tournament, int);
extern "C" void fn_8012DA5C(NetworkObject_8012D8F4* object);
extern "C" void fn_80133B74(NetworkLobby_80133634* lobby);
extern "C" void DWC_ShutdownFriendsMatch();
extern "C" void DWC_Shutdown();
extern "C" void fn_803742D0();
extern u8 lbl_806E20DC;
extern "C" void fn_8012F900(UnidentifiedStatsTracker*, int);
extern "C" void fn_80133A48(NetworkLobby_80133634* lobby);
extern "C" void fn_8012D9D4(NetworkObject_8012D8F4* object);

extern u16 lbl_8058436C[];
extern int lbl_806E20E0;
extern u8 lbl_806E1000;

struct OSThread;
extern "C" int OSCreateThread(OSThread* thread, void* (*func)(void*), void* param, void* stack, u32 stackSize, int priority, u16 attributes);
extern "C" s32 OSResumeThread(OSThread* thread);
extern "C" int OSIsThreadTerminated(OSThread* thread);

typedef void (*DWCLoginCallbackFn)(int error, int profileID, void* param);
extern "C" int DWC_CheckUserData(void* userdata);
extern "C" void DWC_CreateUserData(void* userdata);
extern "C" void DWC_ReportUserData(void* userdata);
extern "C" s32 DWC_GetLastErrorEx(int* errorCode, int* errorType);
extern "C" void DWC_ClearError();
extern "C" int DWC_InitFriendsMatch(void* connectionInfo, void* userdata, int productID, const char* gameName, const char* secretKey, int sendTime, int recvTime, void* friendList, int friendListLen);
extern "C" int DWC_LoginAsync(const u16* ingamesn, void* unused, DWCLoginCallbackFn callback, void* param);
extern "C" int DWC_GetIngamesnCheckResult();

extern "C" void fn_8012DAA0(NetworkObject_8012D8F4* object);
extern "C" int fn_8012FB8C(UnidentifiedStatsTracker*);
extern "C" int fn_8012FBC8(UnidentifiedStatsTracker*, int);
u32 nlGetTicker();
float nlGetTickerDifference(u32 older, u32 newer);
extern "C" void fn_8032E890();
extern "C" void fn_801289CC(float);
extern "C" void fn_8012BCEC(float);
extern "C" void fn_80136DFC(void* tournament, float dt);
extern "C" void fn_8013202C(float);
extern "C" UnidentifiedTournamentState* fn_80127558();
extern "C" void DWC_ProcessFriendsMatch();
extern "C" void fn_80129104(UnidentifiedTournamentState*, int);
extern "C" void fn_80132968(UnidentifiedStatsTracker*, int);
extern "C" void fn_80135D00(NetworkLobby_80133634* lobby);
void nlStrToWcs(const char* source, u16* dest, unsigned long max);
#include <string.h>
extern void* lbl_806E1194;
extern float lbl_806E4724;
extern float lbl_806E4728;

typedef void* (*DWCAllocEx)(int name, u32 size, int align);
typedef void (*DWCFreeEx)(int name, void* ptr, u32 size);
extern "C" int DWC_Init(int authSvr, const char* gameName, u32 gameCode, DWCAllocEx allocator, DWCFreeEx freeer);
extern "C" void DWC_SetReportLevel(u32 level);

struct UnidentifiedVersionInfo
{
    /* 0x0 */ u32 mVersionWord;
    /* 0x4 */ u8 mUnidentified04;
};

extern int lbl_806DC8E4;
extern int lbl_80507070[][2];

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Game.h"

// Networked game bookkeeping owned by the 0x80336xxx translation unit.
struct UnidentifiedNetGameState
{
    /* 0x00 */ u8 mUnidentified00;
    /* 0x01 */ u8 mUnidentified01[0x2];
    /* 0x03 */ u8 mUnidentified03;
    /* 0x04 */ u8 mUnidentified04;
    /* 0x05 */ u8 mUnidentified05[0xC7];
    /* 0xCC */ u8 mUnidentifiedCC[0x4];
    /* 0xD0 */ void* mUnidentifiedD0;
    /* 0xD4 */ u32 mUnidentifiedD4;
};

class UnidentifiedInputRouter
{
public:
    virtual void RouterVirtual00();
    virtual void RouterVirtual04(int);
    virtual void RouterVirtual08();
    virtual void RouterVirtual0C();
    virtual void RouterVirtual10();
    virtual void RouterVirtual14();
    virtual void RouterVirtual18();
    virtual void RouterVirtual1C();
    virtual void RouterVirtual20(int machine, void* data);
    virtual void RouterVirtual24(int machine, void* data);
};

extern "C" void* fn_802B1C4C(unsigned long size);
extern "C" void fn_802B1D4C(void* p, unsigned long size);

typedef void (UnidentifiedNetworkSession::*UnidentifiedNetworkCallback)();

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
    UnidentifiedNetworkSession* mTarget;
};

class UnidentifiedNetworkDelegate
{
public:
    void* operator new(unsigned long size) { return fn_802B1C4C(size); }
    void operator delete(void* p)
    {
        fn_802B1D4C(p, sizeof(UnidentifiedNetworkDelegate));
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
    /* 0x10 */ UnidentifiedNetworkSession* mTarget;
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
    virtual void Register(UnidentifiedActionHandle* handle, u32* slot, int);
};

static inline UnidentifiedNetworkDelegate* NewNetworkDelegate(
    UnidentifiedNetworkBinding binding)
{
    return new UnidentifiedNetworkDelegate(binding);
}

static inline UnidentifiedNetworkDelegate* CreateNetworkDelegateInner(
    UnidentifiedNetworkCallbackRef callback, UnidentifiedNetworkSession* target)
{
    UnidentifiedNetworkBinding binding;
    binding.mCallback = callback.mCallback;
    binding.mTarget = target;
    return NewNetworkDelegate(binding);
}

static inline UnidentifiedNetworkDelegate* CreateNetworkDelegate(
    UnidentifiedNetworkCallbackRef callback, UnidentifiedNetworkSession* target)
{
    return CreateNetworkDelegateInner(callback, target);
}

// The two registration dispatchers live inside cGame's still-unreconstructed
// 0x49C..0x4C8 storage; the byte-offset casts express that target fact without
// inventing cGame layout the Game translation unit does not yet own.
static inline void RegisterNetworkAction(
    UnidentifiedActionHandle* handle, UnidentifiedNetworkCallbackRef callback,
    UnidentifiedNetworkSession* session, UnidentifiedActionRegistry* registry,
    u32* slot)
{
    handle->mState = 2;
    handle->mDelegate = CreateNetworkDelegate(callback, session);
    registry->Register(handle, slot, -1);
}

extern UnidentifiedNetGameState* lbl_806E2164;
extern void* lbl_806E2168;
extern void* lbl_806E2138;
extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];

extern "C" void fn_801276B8(UnidentifiedTournamentState*, UnidentifiedNetworkMessage*);
extern "C" void fn_80127E0C(UnidentifiedTournamentState*, NetMessageGameStart*);
extern "C" int fn_80127F64(UnidentifiedTournamentState*, int);
extern "C" int fn_80127F74(UnidentifiedTournamentState*, int);
extern "C" void fn_80128E3C(UnidentifiedTournamentState*);
extern "C" void fn_8012935C(UnidentifiedTournamentState*, int);
extern "C" void fn_8013157C(UnidentifiedStatsTracker*);
extern "C" void fn_80131518(UnidentifiedStatsTracker*);
extern "C" void fn_802B2E8C(u32* handle);
extern "C" void fn_80131DB4(UnidentifiedStatsTracker*);
extern "C" int fn_80130888(UnidentifiedStatsTracker*);
extern "C" void fn_8013243C(UnidentifiedStatsTracker*, int);
extern "C" void fn_80134298(NetworkLobby_80133634*);
extern "C" void fn_8012B2E4(void*, UnidentifiedNetworkMessage*);
extern "C" void fn_8012B6B0(void*, UnidentifiedNetworkMessage*);
extern "C" void fn_80332EC0(u32 seed);
extern "C" void fn_80332EC8(u32);
extern "C" UnidentifiedInputRouter* fn_803330AC();
extern "C" s8 fn_80336F68(s8 player, s8 machine);
extern "C" void fn_80337FF0(UnidentifiedNetGameState*, int);
extern "C" void fn_80338900(void*, int);
extern "C" void fn_8033288C(void*);
extern "C" void fn_80338AD8(void*, s8 machine, int count);
extern "C" void fn_803380F4(UnidentifiedNetGameState*, s8 machine, int count, u32 seed, void* config, int size);
extern "C" int fn_80338284();
extern "C" UnidentifiedNetworkPeer* fn_80338BF8(UnidentifiedNetworkSession*, int);
extern "C" u32 fn_80111688(void*);
extern "C" void* fn_8011166C();
extern "C" void fn_801C5FB8(BaseGameSceneManager*, int);
extern "C" void fn_801CBCE4(u32, int);
extern "C" void fn_801FC2B4(int);
extern "C" void fn_8026E338(BaseSceneHandler*, UnidentifiedNetworkMessage*);
extern "C" void fn_8026F7F8(BaseSceneHandler*, UnidentifiedNetworkMessage*);
extern "C" void fn_8026FF28(BaseSceneHandler*, UnidentifiedNetworkMessage*);
extern "C" int fn_80336B6C(UnidentifiedNetworkPeer*, int);
extern "C" void fn_80336BE0(UnidentifiedNetworkPeer*);
extern "C" void fn_80336D50(int, UnidentifiedNetworkPeer*, s8, int);

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

void UnidentifiedNetworkSession::Create()
{
    UnidentifiedNetworkSession* session = new UnidentifiedNetworkSession;
    lbl_806E10EC = session;
    lbl_806E20D8 = session;
}

void UnidentifiedNetworkSession::Initialize(bool first)
{
    if (first)
    {
        fn_80338C2C(this, 1, 4);

        mSessionMode = 0;
        mUnidentified2448 = 0;
        mLoginStage = 0;
        mUnidentified244C = 0;
        mDirectSocket = 0;
        mTransport = 0;
        mLobby = 0;
        mElapsedTime = 0.0f;
        mUpdateCount = 0;
        mLastTicker = 0;
        mUnidentified2473 = 0;

        PushAllocator(&VirtualAllocator);
        if (fn_8011C19C() == 0)
        {
            u32 poolSize = 0xB0000 - 0x2A90;
            void* pool = nlMalloc(poolSize, 8, false);
            lbl_806E10F4 = pool;
            s_NetworkAllocator.Initialize(pool, poolSize);
        }
        else
        {
            u32 poolSize = 0x80000 - 0x37D0;
            void* pool = nlMalloc(poolSize, 8, false);
            lbl_806E10F4 = pool;
            s_NetworkAllocator.Initialize(pool, poolSize);
        }
        PopAllocator();

        mUnidentified24A5 = 0;
        mUnidentified24A4 = 0;
        mDWCErrorCode = 0;
        mDWCErrorType = 0;
        mDWCLastError = 0;
        mUnidentified2494 = 0;
        mLoginListener = 0;
        mLoginStartTime = 0.0f;

        mDirectSocket = new NetworkSocket_801246E4;
        mTransport = new NetworkTransport_8032CA4C;
        mLobby = new NetworkLobby_80133634;
        mStatsReporter = new NetworkStatsReporter_8012CE20;
        mUnidentified2460 = new NetworkObject_8012D8F4;
    }
    else
    {
        fn_80338C2C(this, 1, 4);
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

void UnidentifiedNetworkSession::SendTournamentStartToEveryone()
{
    int machineCount = GetMachineRoster()->GetMachineCount();
    u32 stadiumRandom = fn_803236CC();
    s32 stadium = stadiumRandom % 10;
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
    fn_8012785C(machineCount, message.mSeedings);

    for (int machine = 0; machine < machineCount; ++machine)
    {
        u8 buffer[0x64];
        message.mMachineIndex = machine;
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0x64);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
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

void UnidentifiedNetworkSession::SendGameStartToEveryone()
{
    u32 randomSeed = fn_803236CC();
    u32 second = fn_803236CC();
    u32 third = fn_803236CC();
    int machineCount = GetMachineRoster()->GetMachineCount();

    NetMessageGameStart message;
    message.mRandomSeed = randomSeed;
    message.mMachineIndex = 0;
    message.mMachineCount = machineCount;
    message.mUnidentified20 = second;
    message.mUnidentified24 = third;

    int* home = fn_8012C6E0(fn_8012B288(), 0);
    message.mHomeCharacters[0] = home[1];
    message.mHomeCharacters[1] = home[2];
    message.mHomeCharacters[2] = home[3];
    message.mHomeCharacters[3] = home[4];

    int* away = fn_8012C6E0(fn_8012B288(), 1);
    message.mAwayCharacters[0] = away[1];
    message.mAwayCharacters[1] = away[2];
    message.mAwayCharacters[2] = away[3];
    message.mAwayCharacters[3] = away[4];

    if (fn_8025BD88())
    {
        message.mStadium = fn_803236CC() & 0xF;
        if (fn_802C2C84("/user/media_build", 0))
        {
            while (!fn_801B63C8(message.mStadium))
            {
                message.mStadium = fn_803236CC() & 0xF;
            }
        }
    }
    else
    {
        message.mStadium = GameInfoManager::GetInstance()->GetStadium();
    }

    u8 remote = lbl_806E18D4;
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
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0x64);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
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

void UnidentifiedNetworkSession::SendDraftToEveryone(
    int count, UnidentifiedDraftEntry* entries, bool unused, u8 flag)
{
    NetMessageDraft message;
    message.mMachineCount = count;
    message.mUnidentified0B.mData[0] = -1;
    message.mUnidentified0B.mData[1] = -1;
    message.mUnidentified0B.mData[2] = -1;
    message.mUnidentified0B.mData[3] = -1;
    message.mUnidentified0B.mData[4] = -1;
    message.mUnidentified0B.mData[5] = -1;
    message.mUnidentified0B.mData[6] = -1;
    message.mUnidentified0B.mData[7] = -1;
    message.mUnidentified0A = flag;

    for (int entry = 0; entry < count; ++entry)
    {
        message.mEntries[entry] = entries[entry];
    }

    for (int machine = 0; machine < count; ++machine)
    {
        u8 buffer[0x200];
        message.mMachineIndex = machine;
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0x200);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
                "Failed to SendDraftToEveryone to %d because no connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void UnidentifiedNetworkSession::SendDraftToEveryone(NetMessageDraft* message)
{
    for (int machine = 0; machine < message->mMachineCount; ++machine)
    {
        u8 buffer[0x200];
        message->mMachineIndex = machine;
        u32 size = fn_8032C830(lbl_806E2100, message, buffer, 0x200);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
                "Failed to SendDraftToEveryone to %d because no connection\n",
                machine);
        }
        else
        {
            mDirectSocket->Send(aid, buffer, size, true);
        }
    }
}

void UnidentifiedNetworkSession::SendSidesChangedToEveryone(
    UnidentifiedNetworkMessage* message)
{
    int machineCount = GetMachineRoster()->GetMachineCount();
    for (int machine = 0; machine < machineCount; ++machine)
    {
        u8 buffer[0xFF];
        u32 size = fn_8032C830(lbl_806E2100, message, buffer, 0xFF);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
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

void UnidentifiedNetworkSession::SendSidesChangedToHost(
    UnidentifiedNetworkMessage* message)
{
    u8 buffer[0xFF];
    u32 size = fn_8032C830(lbl_806E2100, message, buffer, 0xFF);
    u32 aid = GetMachineRoster()->GetMachineAid(0);
    if (aid == 0xFFFFFFFF)
    {
        mDirectSocket->Receive(buffer, size);
    }
    else if (aid == 0)
    {
        fn_8004F594(
            0x10,
            "Failed to SendSidesChangedToHost because no connection\n");
    }
    else
    {
        mDirectSocket->Send(aid, buffer, size, true);
    }
}

void UnidentifiedNetworkSession::SendCheckConnectionToEveryone()
{
    NetMessageCheckConnection message;
    for (int machine = 0; machine < mLobby->GetPlayerCount(); ++machine)
    {
        message.mUnidentified08[machine] =
            *(u32*)((u8*)fn_80135D5C(mLobby, machine) + 0x18);
    }

    for (int machine = 0; machine < mLobby->GetPlayerCount(); ++machine)
    {
        u8 buffer[0xFF];
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0xFF);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
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

void UnidentifiedNetworkSession::SendConnectionDecisionToEveryone(
    UnidentifiedNetworkMessage* message)
{
    for (int machine = 0; machine < GetMachineRoster()->GetMachineCount();
         ++machine)
    {
        u8 buffer[0xFF];
        u32 size = fn_8032C830(lbl_806E2100, message, buffer, 0xFF);
        u32 aid = GetMachineRoster()->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            mDirectSocket->Receive(buffer, size);
        }
        else if (aid == 0)
        {
            fn_8004F594(
                0x10,
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

void UnidentifiedNetworkSession::SendConnectionDecisionToHost(
    UnidentifiedNetworkMessage* message)
{
    u8 buffer[0xFF];
    u32 size = fn_8032C830(lbl_806E2100, message, buffer, 0xFF);
    u32 aid = GetMachineRoster()->GetMachineAid(0);
    if (aid == 0xFFFFFFFF)
    {
        mDirectSocket->Receive(buffer, size);
    }
    else if (aid == 0)
    {
        fn_8004F594(
            0x10,
            "Failed to SendConnectionDecisionToHost because no connection\n");
    }
    else
    {
        mDirectSocket->Send(aid, buffer, size, true);
    }
}

static inline void PopupNetworkErrorOverlay(
    UnidentifiedNetworkSession* session, int overlay)
{
    if (lbl_806E10E8 != 0)
    {
        return;
    }
    if (session->mPoppedOverlay != 3)
    {
        fn_8004F594(
            0x10,
            "Ignored overlayRequest %d because already popped overlay %d\n",
            overlay, session->mPoppedOverlay);
        return;
    }
    session->mPoppedOverlay = overlay;
    if (fn_80127558()->mUnidentified008 != 0)
    {
        fn_80129104(fn_80127558(), overlay);
    }
}

void UnidentifiedNetworkSession::Update()
{
    if (!fn_80374308())
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

    u32 ticker = nlGetTicker();
    mElapsedTime += dt;
    mLastTicker = ticker;
    mUpdateCount++;

    if (mSessionMode == 1)
    {
        mDirectSocket->Update(dt);
        mTransport->Update(dt);
        fn_80127558();
        fn_801289CC(dt);
        fn_8012B288();
        fn_8012BCEC(dt);
        mStatsReporter->ReporterVirtual14();

        if (mTransport->mState == 3)
        {
            fn_8032E890();
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
                    entries[player].mHead.mUnidentified04[0] = 0;
                    entries[player].mHead.mUnidentified04[1] = 0;
                    entries[player].mHead.mUnidentified04[2] = 0;
                    entries[player].mHead.mUnidentified04[3] = 0;
                    entries[player].mHead.mUnidentified04[4] = 0;
                    entries[player].mHead.mUnidentified04[1] =
                        info->mUnidentified0C;
                    entries[player].mHead.mUnidentified04[2] =
                        info->mUnidentified10;
                    entries[player].mHead.mUnidentified04[3] =
                        info->mUnidentified12;
                }
                SendDraftToEveryone(mTransport->GetPlayerCount(), entries,
                                    false, 0);
            }
        }
    }
    else if (mSessionMode == 2)
    {
        if (mUnidentified2448 != 0 && mUnidentified2494 == 0
            && mLobby->mUnidentified2F4 == 0)
        {
            DWC_ProcessFriendsMatch();
        }

        mLobby->Update(dt);
        mDirectSocket->Update(dt);
        fn_80127558();
        fn_801289CC(dt);
        fn_8012B288();
        fn_8012BCEC(dt);
        fn_80136DFC(lbl_806E1194, dt);
        mUnidentified2460->ObjectVirtual14();
        fn_8012F8F8();
        fn_8013202C(dt);

        if (OnlineVirtual10() == 1)
        {
            fn_80120838();
        }

        if (mLobby->mUnidentified0E0 == 0 && mLobby->GetPlayerCount() > 0)
        {
            fn_80135D00(mLobby);
            SendCheckConnectionToEveryone();
        }
    }

    if (mOverlayRequest != 3 && mUnidentified2472 != 0)
    {
        fn_8004F594(
            0x10,
            "Finished pending load, now can popup network error overlay "
            "%d\n",
            mOverlayRequest);
        fn_80132968(fn_8012F8F8(), 4);
        PopupNetworkErrorOverlay(this, mOverlayRequest);
        mOverlayRequest = 3;
    }
}

static void* NetworkAlloc(int name, u32 size, int align)
{
    return s_NetworkAllocator.Allocate(size, align, false);
}

static void NetworkFree(int name, void* ptr, u32 size)
{
    if (ptr == 0)
    {
        return;
    }
    s_NetworkAllocator.Free(ptr);
}

void UnidentifiedNetworkSession::OnlineVirtual00()
{
    fn_80374174();
    if (!fn_80374308())
    {
        return;
    }

    mSessionMode = 1;
    fn_8032CA1C(lbl_806E2100, 0xD, this);
    fn_8032CA1C(lbl_806E2100, 0xF, this);
    fn_8032CA1C(lbl_806E2100, 0x12, this);
    fn_8032CA1C(lbl_806E2100, 0x13, this);
    fn_8032CA1C(lbl_806E2100, 0x14, this);
    fn_8032CA1C(lbl_806E2100, 0x15, this);
    fn_8032CA1C(lbl_806E2100, 0x0, this);
    fn_8032CA1C(lbl_806E2100, 0x1, this);
    fn_8032CA1C(lbl_806E2100, 0x8, this);
    fn_8032CA1C(lbl_806E2100, 0x9, this);
    fn_8032CA1C(lbl_806E2100, 0x1C, this);
    fn_8032CA1C(lbl_806E2100, 0x1D, this);

    UnidentifiedVersionInfo info;
    info.mVersionWord = GetVersionWord();
    info.mUnidentified04 = 1;
    mDirectSocket->SocketVirtual00(&info, this);
    mDirectSocket->SocketVirtual08(true);

    fn_8032CBC8(mTransport);
    fn_8012CF5C(mStatsReporter);
}

void UnidentifiedNetworkSession::fn_8011FE40()
{
    fn_8012B290(fn_8012B288(), 0);
    fn_8012CFEC(mStatsReporter);
    fn_8012F900(fn_8012F8F8(), 0);
    fn_8032CEAC(mTransport);
    mDirectSocket->SocketVirtual04();

    fn_8032CA2C(lbl_806E2100, 0xD);
    fn_8032CA2C(lbl_806E2100, 0xF);
    fn_8032CA2C(lbl_806E2100, 0x12);
    fn_8032CA2C(lbl_806E2100, 0x13);
    fn_8032CA2C(lbl_806E2100, 0x14);
    fn_8032CA2C(lbl_806E2100, 0x15);
    fn_8032CA2C(lbl_806E2100, 0x0);
    fn_8032CA2C(lbl_806E2100, 0x1);
    fn_8032CA2C(lbl_806E2100, 0x8);
    fn_8032CA2C(lbl_806E2100, 0x9);
    fn_8032CA2C(lbl_806E2100, 0x1C);
    fn_8032CA2C(lbl_806E2100, 0x1D);

    mSessionMode = 0;
}

void UnidentifiedNetworkSession::OnlineVirtual04()
{
    if (!fn_80374308())
    {
        return;
    }

    mUnidentified24A4 = 0;

    u32 gameCode = lbl_806DC88C;
    if (fn_8011C19C() == 2)
    {
        gameCode = lbl_806DC890;
    }
    else if (fn_8011C19C() == 0)
    {
        gameCode = lbl_806DC894;
    }

    DWC_Init(1, "mschargedwii", gameCode, NetworkAlloc, NetworkFree);
    DWC_SetReportLevel(0);
    mUnidentified24A5 = 1;
    mSessionMode = 2;

    fn_8032CA1C(lbl_806E2100, 0xD, this);
    fn_8032CA1C(lbl_806E2100, 0xF, this);
    fn_8032CA1C(lbl_806E2100, 0x12, this);
    fn_8032CA1C(lbl_806E2100, 0x13, this);
    fn_8032CA1C(lbl_806E2100, 0x14, this);
    fn_8032CA1C(lbl_806E2100, 0x15, this);
    fn_8032CA1C(lbl_806E2100, 0x19, this);
    fn_8032CA1C(lbl_806E2100, 0x1A, this);
    fn_8032CA1C(lbl_806E2100, 0x1B, this);
    fn_8032CA1C(lbl_806E2100, 0x0, this);
    fn_8032CA1C(lbl_806E2100, 0x1, this);
    fn_8032CA1C(lbl_806E2100, 0x8, this);
    fn_8032CA1C(lbl_806E2100, 0x9, this);
    fn_8032CA1C(lbl_806E2100, 0x1C, this);
    fn_8032CA1C(lbl_806E2100, 0x1D, this);

    UnidentifiedVersionInfo info;
    info.mVersionWord = GetVersionWord();
    info.mUnidentified04 = 0;
    mDirectSocket->SocketVirtual00(&info, this);

    mDWCErrorCode = 0;
    mDWCErrorType = 0;
    mDWCLastError = 0;
    mUnidentified2494 = 0;
    fn_80133A48(mLobby);
    fn_8012D9D4(mUnidentified2460);
    mUnidentified24A4 = fn_80120440();
}

static void* StaticSetInternetThreadFunc(void*)
{
    lbl_806E10EC->OnlineVirtual04();
    return 0;
}

void UnidentifiedNetworkSession::fn_801202AC()
{
    fn_8004F594(
        0x10, "Created StaticSetInternetThreadFunc returned %d\n",
        OSCreateThread(
            (OSThread*)mLoginThread, StaticSetInternetThreadFunc, 0,
            mLoginThreadStack + sizeof(mLoginThreadStack), 0x4000, 0xE, 1)
            != 0);
    fn_8004F594(
        0x10, "Resumed StaticSetInternetThreadFunc returned %d\n",
        OSResumeThread((OSThread*)mLoginThread));
}

bool UnidentifiedNetworkSession::fn_80120338()
{
    return OSIsThreadTerminated((OSThread*)mLoginThread) != 0;
}

bool UnidentifiedNetworkSession::fn_80120368()
{
    switch (mDWCErrorType)
    {
    case 7:
        fn_8004F594(
            0x10, "DWCError Type is fatal, should we do something?\n");
    case 3:
    case 4:
    case 5:
    case 6:
        return true;
    }
    return false;
}

void UnidentifiedNetworkSession::fn_801203C0()
{
    int error = DWC_GetLastErrorEx(&mDWCErrorCode, &mDWCErrorType);
    mDWCLastError = error;
    if (error != 0)
    {
        fn_8004F594(
            0x10, "DWC_GetLastErrorEx error %d ErrorCode:%d ErrorType:%d\n",
            error, mDWCErrorCode, mDWCErrorType);
        DWC_ClearError();
    }
}

static void StaticDWCLoginCallback(int error, int profileID, void* param)
{
    lbl_806E10EC->DWCLoginCallback(error, profileID, param);
}

bool UnidentifiedNetworkSession::fn_80120440()
{
    void* friendList;
    GameInfoManager* gameInfo;

    OnlineVirtual14(true);
    mLoginStage = 1;
    mLoginStartTime = 0.0f;
    lbl_806E1000 = 1;

    if (!DWC_CheckUserData(
            GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0)))
    {
        DWC_CreateUserData(
            GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));
    }
    DWC_ReportUserData(
        GameInfoManager::GetInstance()->GetSaveSlot(lbl_806E20E0));

    gameInfo = GameInfoManager::GetInstance();
    friendList = gameInfo->GetUnknown0x40(lbl_806E20E0, 0);
    DWC_InitFriendsMatch(
        &lbl_806E10F0, gameInfo->GetSaveSlot(lbl_806E20E0), 0x2AAF,
        "mschargedwii", "B4LdGW", 0, 0, friendList, 0x40);

    const u16* name = (const u16*)L"";
    if (lbl_8058436C[0] != 0)
    {
        name = lbl_8058436C;
    }

    if (!DWC_LoginAsync(name, 0, StaticDWCLoginCallback, 0))
    {
        fn_8004F594(0x10, "Initial fail in DWC_LoginAsync\n");
        int error = DWC_GetLastErrorEx(&mDWCErrorCode, &mDWCErrorType);
        mDWCLastError = error;
        if (error != 0)
        {
            fn_8004F594(
                0x10,
                "DWC_GetLastErrorEx error %d ErrorCode:%d ErrorType:%d\n",
                error, mDWCErrorCode, mDWCErrorType);
            DWC_ClearError();
        }
        return false;
    }
    fn_8004F594(0x10, "Starting DWC_LoginAsync\n");
    return true;
}

void UnidentifiedNetworkSession::DWCLoginCallback(
    int error, int profileID, void* param)
{
    fn_8004F594(
        0x10, "DWCLoginCallback returned %d, profileID %d param %d\n", error,
        profileID, param);

    if (mLoginStage != 1)
    {
        fn_8004F594(
            0x10, "DWCLoginCallback ignored because in stage %d\n",
            mLoginStage);
        return;
    }

    mLoginStage = 2;
    if (error != 0)
    {
        if (param == 0)
        {
            fn_801203C0();
        }
        mLoginListener->OnLoginResult(1);
        return;
    }

    GameInfoManager::GetInstance()->ValidateSaveSlot(lbl_806E20E0);
    int check = DWC_GetIngamesnCheckResult();
    fn_8004F594(
        0x10, "DWC_GetIngamesnCheckResult returned %d\n", check);
    if (check == 2)
    {
        mLoginListener->OnLoginResult(2);
    }
    else
    {
        mLoginListener->OnLoginResult(0);
    }
}

bool UnidentifiedNetworkSession::fn_80120738()
{
    fn_8012DAA0(mSessionMode == 2 ? mUnidentified2460 : 0);

    bool started;
    if (fn_8012FB8C(fn_8012F8F8()))
    {
        mLoginStage = 3;
        if (!fn_8012FBC8(fn_8012F8F8(), 4))
        {
            fn_8004F594(0x10, "Error getting friends stats\n");
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
        if (!fn_8012FBC8(fn_8012F8F8(), 2))
        {
            fn_8004F594(0x10, "Error getting nearby stats\n");
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
        fn_8004F594(
            0x10, "Started login timestamp = %f\n", mElapsedTime);
        return true;
    }
    return false;
}

void UnidentifiedNetworkSession::fn_80120838()
{
    if (mLoginStartTime != 0.0f
        && mLoginStartTime + lbl_806DC888 <= mElapsedTime)
    {
        if ((mLoginStage < 0xE || mLoginStage >= 0x10)
            && (mLoginStage >= 3 || mLoginStage < 1))
        {
            fn_8004F594(
                0x10, "Aborting Getting Stats in Login Timed out!\n");
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
                fn_8004F594(
                    0x10,
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
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            void* record = fn_8012F8F8()->mUnidentified22 != 0
                               ? fn_8012F8F8()->mUnidentified54
                               : 0;
            if (record != 0)
            {
                if (fn_80132C74(record))
                {
                    fn_8004F594(
                        0x10,
                        "Starting new friends season clearing stats\n");
                    fn_801304D0(fn_8012F8F8(), 2, 0);
                    mLoginStage = 4;
                }
                else if (lbl_806E20DC != 0)
                {
                    fn_8004F594(
                        0x10,
                        "Detected Mii change putting friends unchanged "
                        "win/loss stats\n");
                    fn_801304D0(fn_8012F8F8(), 2, 1);
                    mLoginStage = 4;
                }
                else
                {
                    mLoginStage = 5;
                    if (!fn_8012FBC8(fn_8012F8F8(), 2))
                    {
                        fn_8004F594(0x10, "Error getting nearby stats\n");
                    }
                }
            }
            else
            {
                fn_8004F594(
                    0x10,
                    "Login: Did not get own friends stats, first time user "
                    "will now put new own stats\n");
                fn_801304D0(fn_8012F8F8(), 2, 0);
                mLoginStage = 4;
            }
        }
        else
        {
            fn_8004F594(0x10, "Error getting friends stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 4:
        if (fn_8012F8F8()->mUnidentified0C == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified0D != 0)
        {
            mLoginStage = 5;
            if (!fn_8012FBC8(fn_8012F8F8(), 2))
            {
                fn_8004F594(0x10, "Error getting nearby stats\n");
            }
            fn_80131464(fn_8012F8F8(), 0xA);
        }
        else
        {
            fn_8004F594(
                0x10,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                fn_8012F8F8()->mUnidentified10);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified0C = 0;
        break;

    case 5:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            void* record = fn_8012F8F8()->mUnidentified20 != 0
                               ? fn_8012F8F8()->mUnidentified24
                               : 0;
            if (record != 0)
            {
                if (fn_80132C74(record))
                {
                    fn_8004F594(
                        0x10, "Starting new season clearing stats\n");
                    fn_8012FFB0(fn_8012F8F8(), record);
                    fn_801304D0(fn_8012F8F8(), 0, 0);
                    mLoginStage = 6;
                }
                else if (lbl_806E20DC != 0)
                {
                    fn_8004F594(
                        0x10,
                        "Detected Mii change putting own unchanged win/loss "
                        "stats\n");
                    fn_801304D0(fn_8012F8F8(), 0, 1);
                    mLoginStage = 6;
                }
                else
                {
                    mLoginStage = 8;
                    fn_8004F594(
                        0x10,
                        "Login: Transition to "
                        "ELoggingInStage_GettingSODNearbyStats\n");
                    if (!fn_8012FBC8(fn_8012F8F8(), 0))
                    {
                        fn_8004F594(
                            0x10,
                            "Initial failure to RequestRankings "
                            "STRIKER_OF_DAY Nearby\n");
                        mLoginListener->OnStatsResult(false);
                        mLoginStage = 0xF;
                    }
                }
            }
            else
            {
                fn_8004F594(
                    0x10,
                    "Login: Did not get own stats, first time user will now "
                    "put new own stats\n");
                fn_801304D0(fn_8012F8F8(), 0, 0);
                mLoginStage = 6;
            }
        }
        else
        {
            fn_8004F594(0x10, "Error getting nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 6:
        if (fn_8012F8F8()->mUnidentified0C == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified0D != 0)
        {
            mLoginStage = 7;
            if (!fn_8012FBC8(fn_8012F8F8(), 2))
            {
                fn_8004F594(0x10, "Error REgetting nearby stats\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            fn_8004F594(
                0x10,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                fn_8012F8F8()->mUnidentified10);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified0C = 0;
        break;

    case 7:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            mLoginStage = 8;
            fn_8004F594(
                0x10,
                "Login: Transition to "
                "ELoggingInStage_GettingSODNearbyStats\n");
            if (!fn_8012FBC8(fn_8012F8F8(), 0))
            {
                fn_8004F594(
                    0x10,
                    "Initial failure to RequestRankings STRIKER_OF_DAY "
                    "Nearby\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            fn_8004F594(0x10, "Error REgetting nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 8:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            void* record = fn_8012F8F8()->mUnidentified21 != 0
                               ? fn_8012F8F8()->mUnidentified3C
                               : 0;
            if (record != 0)
            {
                if (fn_80132D54(record))
                {
                    fn_8004F594(0x10, "Starting new day clearing stats\n");
                    fn_801304D0(fn_8012F8F8(), 1, 0);
                    mLoginStage = 9;
                }
                else if (lbl_806E20DC != 0)
                {
                    fn_8004F594(
                        0x10,
                        "Detected Mii change putting SOD unchanged win/loss "
                        "stats\n");
                    fn_801304D0(fn_8012F8F8(), 1, 1);
                    mLoginStage = 9;
                }
                else if (!fn_8012FB8C(fn_8012F8F8()))
                {
                    mLoginStage = 0xB;
                    if (!fn_8012FBC8(fn_8012F8F8(), 4))
                    {
                        fn_8004F594(
                            0x10,
                            "Initial failure to RequestRankings SEASON "
                            "FRIENDS\n");
                        mLoginListener->OnStatsResult(false);
                        mLoginStage = 0xF;
                    }
                }
                else
                {
                    mLoginStage = 0xC;
                    if (!fn_8012FBC8(fn_8012F8F8(), 1))
                    {
                        fn_8004F594(
                            0x10,
                            "Initial failure to RequestRankings "
                            "STRIKER_OF_DAY TOP\n");
                        mLoginListener->OnStatsResult(false);
                        mLoginStage = 0xF;
                    }
                }
            }
            else
            {
                fn_8004F594(
                    0x10,
                    "Login: Did not get own SOD stats, first time user will "
                    "now put new own SOD stats\n");
                fn_801304D0(fn_8012F8F8(), 1, 0);
                mLoginStage = 9;
            }
        }
        else
        {
            fn_8004F594(0x10, "Error getting SOD nearbystats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 9:
        if (fn_8012F8F8()->mUnidentified0C == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified0D != 0)
        {
            mLoginStage = 0xA;
            fn_8004F594(
                0x10,
                "Login: Transition to "
                "ELoggingInStage_ReGettingSODNearbyStats\n");
            if (!fn_8012FBC8(fn_8012F8F8(), 0))
            {
                fn_8004F594(
                    0x10,
                    "Initial failure to RE-RequestRankings STRIKER_OF_DAY "
                    "Nearby\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            fn_8004F594(
                0x10,
                "Error finishing PostResetMyPlayerStats pers cat %d login\n",
                fn_8012F8F8()->mUnidentified10);
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified0C = 0;
        break;

    case 0xA:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            if (!fn_8012FB8C(fn_8012F8F8()))
            {
                mLoginStage = 0xB;
                if (!fn_8012FBC8(fn_8012F8F8(), 4))
                {
                    fn_8004F594(
                        0x10,
                        "Initial failure to RequestRankings SEASON "
                        "FRIENDS\n");
                    mLoginListener->OnStatsResult(false);
                    mLoginStage = 0xF;
                }
            }
            else
            {
                mLoginStage = 0xC;
                if (!fn_8012FBC8(fn_8012F8F8(), 1))
                {
                    fn_8004F594(
                        0x10,
                        "Initial failure to RequestRankings STRIKER_OF_DAY "
                        "TOP\n");
                    mLoginListener->OnStatsResult(false);
                    mLoginStage = 0xF;
                }
            }
        }
        else
        {
            fn_8004F594(
                0x10, "Error REgetting SOD Nearby stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 0xB:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            mLoginStage = 0xC;
            if (!fn_8012FBC8(fn_8012F8F8(), 1))
            {
                fn_8004F594(
                    0x10,
                    "Initial failure to RequestRankings STRIKER_OF_DAY "
                    "TOP\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            fn_8004F594(
                0x10, "Error getting SEASON FRIENDS stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 0xC:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            mLoginStage = 0xD;
            if (!fn_8012FBC8(fn_8012F8F8(), 3))
            {
                fn_8004F594(
                    0x10,
                    "Initial failure to RequestRankings SEASON TOP\n");
                mLoginListener->OnStatsResult(false);
                mLoginStage = 0xF;
            }
        }
        else
        {
            fn_8004F594(0x10, "Error getting SOD TOP stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;

    case 0xD:
        if (fn_8012F8F8()->mUnidentified04 == 0)
        {
            break;
        }
        if (fn_8012F8F8()->mUnidentified05 != 0)
        {
            mLoginStage = 0xE;
            lbl_806E10EC->OnlineVirtual14(2);
            mLoginListener->OnStatsResult(true);
            fn_801314D0(fn_8012F8F8());
            lbl_806E20DC = 0;
        }
        else
        {
            fn_8004F594(
                0x10, "Error getting SEASON TOP stats at login\n");
            mLoginListener->OnStatsResult(false);
            mLoginStage = 0xF;
        }
        fn_8012F8F8()->mUnidentified04 = 0;
        break;
    }
}

void UnidentifiedNetworkSession::fn_801214BC()
{
    if (ResetTask::s_ResetState != RS_STARTRESET)
    {
        SaveLoad::StartSave(true);
    }
    else if (mLobby->mUnidentified2F4 != 0)
    {
        return;
    }

    mUnidentified2494 = 0;
    DWC_ShutdownFriendsMatch();
    fn_8012B290(fn_8012B288(), 0);
    fn_80136140(lbl_806E1194, 0);
    fn_8012DA5C(mUnidentified2460);
    fn_8012F900(fn_8012F8F8(), 0);
    fn_80133B74(mLobby);
    mDirectSocket->SocketVirtual04();
    DWC_Shutdown();
    mUnidentified24A5 = 0;
    fn_803742D0();

    fn_8032CA2C(lbl_806E2100, 0xD);
    fn_8032CA2C(lbl_806E2100, 0xF);
    fn_8032CA2C(lbl_806E2100, 0x12);
    fn_8032CA2C(lbl_806E2100, 0x13);
    fn_8032CA2C(lbl_806E2100, 0x14);
    fn_8032CA2C(lbl_806E2100, 0x15);
    fn_8032CA2C(lbl_806E2100, 0x19);
    fn_8032CA2C(lbl_806E2100, 0x1A);
    fn_8032CA2C(lbl_806E2100, 0x1B);
    fn_8032CA2C(lbl_806E2100, 0x0);
    fn_8032CA2C(lbl_806E2100, 0x1);
    fn_8032CA2C(lbl_806E2100, 0x8);
    fn_8032CA2C(lbl_806E2100, 0x9);
    fn_8032CA2C(lbl_806E2100, 0x1C);
    fn_8032CA2C(lbl_806E2100, 0x1D);
    mSessionMode = 0;
}

void UnidentifiedNetworkSession::OnlineVirtual08()
{
    switch (mSessionMode)
    {
    case 0:
        break;
    case 2:
        fn_801214BC();
        break;
    case 1:
        fn_8011FE40();
        break;
    }
    mSessionMode = 0;
    mUnidentified2448 = 0;
}

int UnidentifiedNetworkSession::OnlineVirtual0C()
{
    return mSessionMode;
}

int UnidentifiedNetworkSession::OnlineVirtual10()
{
    return mUnidentified2448;
}

void UnidentifiedNetworkSession::OnlineVirtual14(int phase)
{
    mUnidentified2448 = phase;
}

NetworkSocket_801246E4* UnidentifiedNetworkSession::GetDirectSocket()
{
    return mDirectSocket;
}

UnidentifiedMachineRoster* UnidentifiedNetworkSession::GetMachineRoster()
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

NetworkTransport_8032CA4C* UnidentifiedNetworkSession::GetTransport()
{
    if (mSessionMode == 1)
    {
        return mTransport;
    }
    return 0;
}

NetworkLobby_80133634* UnidentifiedNetworkSession::fn_801216F0()
{
    if (mSessionMode == 2)
    {
        return mLobby;
    }
    return 0;
}

void* UnidentifiedNetworkSession::fn_8012170C()
{
    if (mSessionMode == 2)
    {
        return mUnidentified2460;
    }
    if (mSessionMode == 1)
    {
        return mStatsReporter;
    }
    return 0;
}

NetworkStatsReporter_8012CE20* UnidentifiedNetworkSession::fn_80121738()
{
    if (mSessionMode == 1)
    {
        return mStatsReporter;
    }
    return 0;
}

NetworkObject_8012D8F4* UnidentifiedNetworkSession::fn_80121754()
{
    if (mSessionMode == 2)
    {
        return mUnidentified2460;
    }
    return 0;
}

void UnidentifiedNetworkSession::ListenerVirtual00(u32 a, void* b)
{
    fn_8032C8CC(lbl_806E2100, -2, a, b);
}

void UnidentifiedNetworkSession::ListenerVirtual04(int type, u32 a, void* b)
{
    fn_8032C8CC(lbl_806E2100, type, a, b);
}

void UnidentifiedNetworkSession::ListenerVirtual08(u32 connection, u8* address)
{
    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    if (roster == 0)
    {
        return;
    }
    NetworkSocket_801246E4* socket = lbl_806E20D8->GetDirectSocket();
    if (roster->ShouldAcceptConnection(connection, address))
    {
        socket->SocketVirtual1C(connection);
        fn_8004F594(
            0x10, "Accepted connection from %d.%d.%d.%d\n", address[0],
            address[1], address[2], address[3]);
    }
    else
    {
        socket->SocketVirtual20(connection);
        fn_8004F594(
            0x10, "Rejected connection from %d.%d.%d.%d\n", address[0],
            address[1], address[2], address[3]);
    }
}

void UnidentifiedNetworkSession::ListenerVirtual0C(u32 connection, int result)
{
    fn_8004F594(0x10, "Connected callback result %d\n", result);
    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    if (roster != 0)
    {
        roster->OnConnected(connection, result);
    }
}

void UnidentifiedNetworkSession::ListenerVirtual10(u32 connection, int reason)
{
    fn_8004F594(0x10, "Connection closed reason %d\n", reason);
    fn_80124038(connection, reason);
    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    if (roster != 0)
    {
        roster->OnConnectionClosed(connection, reason);
    }
}

void UnidentifiedNetworkSession::ListenerVirtual14()
{
}

void UnidentifiedNetworkSession::ListenerVirtual18()
{
}

struct UnidentifiedGameConfig
{
    /* 0x00 */ int mStadium;
    /* 0x04 */ int mHomeTeam;
    /* 0x08 */ int mHomeSidekicks[3];
    /* 0x14 */ int mAwayTeam;
    /* 0x18 */ int mAwaySidekicks[3];
    /* 0x24 */ int mSettings[5];
    /* 0x38 */ s16 mPlayingSides[16];
}; // size: 0x58

extern "C" void fn_80122650(UnidentifiedGameConfig* config);
extern "C" void fn_80122748(UnidentifiedGameConfig* config);

struct UnidentifiedDraftSceneState;
extern "C" void fn_801243A8(
    UnidentifiedDraftSceneState* scene, NetMessageDraft message);
extern "C" void fn_80122DCC();
extern "C" void fn_80122EC0();
extern "C" void fn_8012300C();

// Two local pad indices for the machine's one or two local players.
extern int lbl_806DE668[2];

static inline void NotifyGameStarted(UnidentifiedNetworkSession* session)
{
    void* state = lbl_806E2168;
    int count = fn_80338BF0(session);
    fn_80338AD8(state, (s8)fn_80338C20(session), count);
}

static inline void RecordGameConfig(
    UnidentifiedNetworkSession* session, u32 seed,
    UnidentifiedGameConfig* config)
{
    UnidentifiedNetGameState* state = lbl_806E2164;
    if (state->mUnidentified00 != 0)
    {
        int count = fn_80338BF0(session);
        fn_803380F4(
            state, (s8)fn_80338C20(session), count, seed, config, 0x58);
    }
}



typedef void (UnidentifiedNetworkSession::*UnidentifiedSessionCallback)();

static inline void RegisterLoadedGameActions(UnidentifiedNetworkSession* session)
{
    UnidentifiedActionHandle first;
    RegisterNetworkAction(
        &first,
        UnidentifiedNetworkCallbackRef(
            &UnidentifiedNetworkSession::fn_801239F8),
        session, (UnidentifiedActionRegistry*)((u8*)g_pGame + 0x49C),
        &session->mUnidentified2464);

    UnidentifiedActionHandle second;
    RegisterNetworkAction(
        &second,
        UnidentifiedNetworkCallbackRef(
            &UnidentifiedNetworkSession::fn_801239FC),
        session, (UnidentifiedActionRegistry*)((u8*)g_pGame + 0x4C8),
        &session->mUnidentified2468);

    if (fn_80127558()->mUnidentified008 != 0)
    {
        fn_80128E3C(fn_80127558());
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

int UnidentifiedNetworkSession::ReceiverVirtual00(
    UnidentifiedNetworkMessage* message)
{
    UnidentifiedMachineRoster* roster = GetMachineRoster();
    int machine = (s8)roster->MachineIdxFromConnection(message->mUnidentified04);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        fn_8004F594(
            0x10,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mUnidentified04);
        return 1;
    }

    if ((u8)message->GetType() != 0x14 && (u8)message->GetType() != 0xD
        && (u8)message->GetType() != 0x15 && (u8)message->GetType() != 0x19
        && (u8)message->GetType() != 0x1A && (u8)message->GetType() != 0x1B
        && fn_80127558()->mUnidentified925 != 0)
    {
        int original = machine;
        machine = (s8)fn_80127F74(fn_80127558(), machine);
        if (machine < 0 || machine >= fn_80338BF0(this))
        {
            fn_8004F594(
                0x10,
                "Discarded message type %d.  TournamentIdxToMachineIdx "
                "changed ID %d to ID %d, but invalid\n",
                (u8)message->GetType(), (s8)original, machine);
            return 1;
        }
    }

    switch ((u8)message->GetType())
    {
    case 0xD:
        if (lbl_806E2164->mUnidentified03 != 0)
        {
            // Retail re-evaluates the playback flag inside the branch,
            // matching an inlined predicate called twice.
            if (lbl_806E2164->mUnidentified03 != 0 && fn_80338284())
            {
            UnidentifiedGameStartInfo* info =
                (UnidentifiedGameStartInfo*)lbl_806E2164->mUnidentifiedCC;
            fn_80332EC0(lbl_806E2164->mUnidentifiedD4);
            UnidentifiedGameConfig* config = info->mConfig;
            fn_80122748(config);
            fn_8004F594(
                0x10,
                "PlaybackRecordedGame: Random seed %x Stadium %d HOME %d "
                "[%d, %d, %d] Vs AWAY %d [%d, %d, %d]\n",
                info->mSeed, config->mStadium, config->mHomeTeam,
                config->mHomeSidekicks[0], config->mHomeSidekicks[1],
                config->mHomeSidekicks[2], config->mAwayTeam,
                config->mAwaySidekicks[0], config->mAwaySidekicks[1],
                config->mAwaySidekicks[2]);
            fn_8004F594(
                0x10,
                "PlaybackRecordedGame: Skill %d WinBy %s GameTime %d "
                "GameGoals %d BestSeries %d\n",
                config->mSettings[0],
                config->mSettings[1] == 0 ? "Timed" : "Goals",
                config->mSettings[2], config->mSettings[3],
                config->mSettings[4]);
            fn_803330AC()->RouterVirtual04(0);
            lbl_806E20D8->BaseVirtual3C(info);
            void* state = lbl_806E2168;
            int count = fn_80338BF0(lbl_806E20D8);
            fn_80338AD8(state, (s8)fn_80338C20(lbl_806E20D8), count);
            }
        }
        else
        {
            BaseVirtual44((NetMessageGameStart*)message);
        }
        mUnidentified2448 = 4;
        fn_801C5FB8(lbl_806E1838, 0x1D);
        for (int component = 0; component < 4; ++component)
        {
            lbl_80578450[component]->SetActiveSlide("waiting", true, false);
        }
        lbl_806E1838->PushLoadingScene(false);
        break;

    case 0x14:
        fn_801276B8(fn_80127558(), message);
        break;

    case 0x15:
        if (((NetMessageDraft*)message)->mUnidentified0A != 0)
        {
            UnidentifiedDraftSceneState* scene = (UnidentifiedDraftSceneState*)
                lbl_806E1838->Push((SceneList)0x38, SCREEN_FORWARD, true);
            fn_801243A8(scene, *(NetMessageDraft*)message);
        }
        else
        {
            if (fn_8025BD88())
            {
                fn_8012B2E4(fn_8012B288(), message);
            }
            else
            {
                fn_8012B6B0(fn_8012B288(), message);
            }
            fn_80131518(fn_8012F8F8());
        }
        break;

    case 0x19:
    {
        BaseSceneHandler* scene = lbl_806E1838->GetScene((SceneList)0x38);
        BaseSceneHandler* handler = scene != 0 ? scene : 0;
        if (handler != 0)
        {
            fn_8026E338(handler, message);
        }
        break;
    }

    case 0x1A:
        mUnidentified2448 = 3;
        GetMachineRoster()->RosterVirtual48();
        if (fn_8025BD88())
        {
            fn_801CBCE4(0x89B1FC93, 0x2A);
        }
        fn_801FC2B4(8);
        lbl_806E1838->Push((SceneList)0x39, SCREEN_FORWARD, true);
        fn_8026F7F8(0, message);
        break;

    case 0x1B:
    {
        BaseSceneHandler* scene = lbl_806E1838->GetScene((SceneList)0x39);
        if (scene != 0)
        {
            fn_8026FF28(scene, message);
        }
        break;
    }

    case 0xF:
        fn_8004F594(
            0x10, "Received loaded game message from %d\n", machine);
        mUnidentified246E[machine] = 1;
        break;

    case 0x12:
        fn_8004F594(
            0x10, "Received loaded game CLIENT message from %d\n", machine);
        mUnidentified246E[machine] = 1;
        break;

    case 0x13:
    {
        fn_8004F594(
            0x10, "Received loaded game EVERYONE message from %d\n", machine);
        for (int index = 0; index < fn_80338BF0(this); ++index)
        {
            mUnidentified246E[index] = 1;
        }
        RegisterLoadedGameActions(this);
        mUnidentified2448 = 5;
        break;
    }

    case 0x0:
    case 0x1:
        if (lbl_806E2164->mUnidentified03 != 0)
        {
            break;
        }
        if (machine < 0 || machine >= fn_80338BF0(this))
        {
            fn_8004F594(
                0x10,
                "Discarded message type %d because from unknown connection "
                "%x\n",
                (u8)message->GetType(), message->mUnidentified04);
            break;
        }
        if (mUnidentified2448 != 5)
        {
            fn_8004F594(
                0x10, "Ignoring GameInput Message because in network stage %d",
                mUnidentified2448);
            break;
        }
        if ((u8)message->GetType() == 0)
        {
            fn_803330AC()->RouterVirtual20(machine, message);
        }
        else
        {
            fn_803330AC()->RouterVirtual20(machine, (u8*)message + 0x8);
            fn_803330AC()->RouterVirtual20(machine, (u8*)message + 0xF8);
        }
        break;

    case 0x8:
    case 0x9:
        if (lbl_806E2164->mUnidentified03 != 0)
        {
            break;
        }
        if (machine < 0 || machine >= fn_80338BF0(this))
        {
            fn_8004F594(
                0x10,
                "Discarded message type %d because from unknown connection "
                "%x\n",
                (u8)message->GetType(), message->mUnidentified04);
            break;
        }
        if (mUnidentified2448 != 5)
        {
            fn_8004F594(
                0x10, "Ignoring GameInput Message because in network stage %d",
                mUnidentified2448);
            break;
        }
        if ((u8)message->GetType() == 8)
        {
            fn_803330AC()->RouterVirtual24(machine, message);
        }
        else
        {
            fn_803330AC()->RouterVirtual24(machine, (u8*)message + 0x8);
            fn_803330AC()->RouterVirtual24(machine, (u8*)message + 0x3E0);
        }
        break;

    case 0x1C:
    {
        if ((s8)fn_80338C20(this) != 0)
        {
            break;
        }
        u8 bit = 1 << (s8)((NetMessagePauseResponse_8050AD68*)message)
                              ->mUnidentified08;
        if (((NetMessagePauseResponse_8050AD68*)message)->mUnidentified09 != 0)
        {
            mUnidentified246C = mUnidentified246C | bit;
        }
        else
        {
            mUnidentified246C = mUnidentified246C & ~bit;
        }

        NetMessagePauseResponse_8050AD68 response;
        response.mUnidentified08 = mUnidentified246C;
        u8 buffer[0x32];
        u32 size = fn_8032C830(lbl_806E2100, &response, buffer, 0x32);
        fn_8004F594(
            0x10, "HOST sending Pause Response to all clients and myself\n");
        int count = fn_80338BF0(this);
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
            ((NetMessagePauseResponse_8050AD68*)message)->mUnidentified08;
        break;
    }
    return 1;
}

void UnidentifiedNetworkSession::BaseVirtual3C(UnidentifiedGameStartInfo* info)
{
    int myId = info->mMyMachineId;
    int count = info->mMachineCount;
    mUnidentified0000 = count;
    mUnidentified2424 = myId;

    UnidentifiedNetworkPeer* peer = mPeers;
    for (int machine = 0; machine < mUnidentified0000; ++machine)
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

extern "C" void fn_80122650(UnidentifiedGameConfig* config)
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

    const GameSettings* settings =
        GameInfoManager::GetInstance()->GetCurrentSettings();
    config->mSettings[0] = settings->unknown_0x00;
    config->mSettings[1] = settings->unknown_0x04;
    config->mSettings[2] = settings->unknown_0x08;
    config->mSettings[3] = settings->unknown_0x0C;
    config->mSettings[4] = settings->unknown_0x10;

    for (int pad = 0; pad < 16; ++pad)
    {
        config->mPlayingSides[pad] =
            GameInfoManager::GetInstance()->GetPlayingSide(pad);
    }
}

extern "C" void fn_80122748(UnidentifiedGameConfig* config)
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
    manager->mBaseSettings.unknown_0x00 = config->mSettings[0];
    manager->mBaseSettings.unknown_0x04 = config->mSettings[1];
    manager->mBaseSettings.unknown_0x08 = config->mSettings[2];
    manager->mBaseSettings.unknown_0x0C = config->mSettings[3];
    manager->mBaseSettings.unknown_0x10 = config->mSettings[4];

    manager = GameInfoManager::GetInstance();
    manager->mCurGameSettings.unknown_0x00 = manager->mBaseSettings.unknown_0x00;
    manager->mCurGameSettings.unknown_0x04 = manager->mBaseSettings.unknown_0x04;
    manager->mCurGameSettings.unknown_0x08 = manager->mBaseSettings.unknown_0x08;
    manager->mCurGameSettings.unknown_0x0C = manager->mBaseSettings.unknown_0x0C;
    manager->mCurGameSettings.unknown_0x10 = manager->mBaseSettings.unknown_0x10;
    manager->mCurGameSettings.unknown_0x14 = manager->mBaseSettings.unknown_0x14;
    manager->mCurGameSettings.unknown_0x15 = manager->mBaseSettings.unknown_0x15;
    manager->mCurGameSettings.unknown_0x16 = manager->mBaseSettings.unknown_0x16;
    manager->mCurGameSettings.unknown_0x17 = manager->mBaseSettings.unknown_0x17;
    manager->mCurGameSettings.unknown_0x18 = manager->mBaseSettings.unknown_0x18;
    manager->mCurGameSettings.unknown_0x19 = manager->mBaseSettings.unknown_0x19;
    manager->mCurGameSettings.unknown_0x1A = manager->mBaseSettings.unknown_0x1A;

    for (int pad = 0; pad < 16; ++pad)
    {
        GameInfoManager::GetInstance()->SetPlayingSide(
            pad, config->mPlayingSides[pad]);
    }
}

void UnidentifiedNetworkSession::BaseVirtual40()
{
}

void UnidentifiedNetworkSession::BaseVirtual44(NetMessageGameStart* message)
{
    mUnidentified0000 = (s8)message->mMachineCount;
    mUnidentified2424 = (s8)message->mMachineIndex;

    u8* entryFlags = (u8*)fn_8012B288() + 0xD3C;
    for (int machine = 0; machine < mUnidentified0000; ++machine)
    {
        mPeers[machine].mMachineId = machine;
        int players;
        if (fn_8025BD88())
        {
            players = message->mMachineFlags[machine];
        }
        else
        {
            players = (entryFlags[0x93] != 0) + 1;
        }
        mPeers[machine].mUnidentified004 = players;

        if (machine == mUnidentified2424)
        {
            for (int player = 0; player < players; ++player)
            {
                fn_80336D50(
                    fn_80336B6C(&mPeers[machine], player), &mPeers[machine],
                    (s8)player, lbl_806DE668[player]);
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
        fn_80127E0C(fn_80127558(), message);
    }
    else
    {
        mUnidentified2474 = message->mUnidentified20;
        mUnidentified2478 = message->mUnidentified24;
        mUnidentified247C = 1;
    }

    fn_8004F594(
        0x10, "PreStartNetworkedGame NumMachines:%d MyMachineID:%d\n",
        mUnidentified0000, mUnidentified2424);
    AIPadManager::Startup();

    u32 seed = message->mRandomSeed;
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

    if (fn_8025BD88())
    {
        u32 side = 0;
        if (message->mUnidentified1B == 0)
        {
            side = fn_8012C6E0(fn_8012B288(), 0)[0x90 / 4] != 0;
        }
        for (int machine = 0; machine < mUnidentified0000; ++machine)
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
        for (int machine = 0; machine < mUnidentified0000; ++machine)
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

    fn_8004F594(
        0x10, "StartNetworkedGame: Random seed %x NumMachines:%d "
              "MyMachineID:%d\n",
        seed, mUnidentified0000, mUnidentified2424);
    fn_803330AC()->RouterVirtual04(0);
    NotifyGameStarted(this);

    UnidentifiedGameConfig config;
    fn_80122650(&config);
    RecordGameConfig(this, seed, &config);
}

class UnidentifiedFrameController
{
public:
    virtual void ControllerVirtual00();
    virtual void ControllerVirtual04();
    virtual void ControllerVirtual08();
    virtual void ControllerVirtual0C();
    virtual void ControllerVirtual10();
    virtual void ControllerVirtual14();
    virtual void ControllerVirtual18();
    virtual void ControllerVirtual1C();
    virtual void ControllerVirtual20();
    virtual void ControllerVirtual24();
    virtual void ControllerVirtual28();
    virtual void ControllerVirtual2C();
    virtual void ControllerVirtual30();
    virtual int GetEndFrame();
};

void UnidentifiedNetworkSession::fn_80122C84()
{
    fn_8004F594(
        0x10, "Rematching network game.  End Frame is %d\n",
        ((UnidentifiedFrameController*)fn_8011166C())->GetEndFrame());
    fn_80332EC8(fn_80111688(fn_8011166C()));
    fn_80337FF0(lbl_806E2164, 0);
    fn_80338900(lbl_806E2168, 0);
    fn_803330AC()->RouterVirtual04(0);
    fn_8033288C(lbl_806E2138);

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
    fn_80332EC0(seed);

    NotifyGameStarted(this);

    UnidentifiedGameConfig config;
    fn_80122650(&config);
    RecordGameConfig(this, seed, &config);
}

extern "C" void fn_80122DCC()
{
    fn_80332EC8(fn_80111688(fn_8011166C()));
    fn_80337FF0(lbl_806E2164, 0);
    fn_80338900(lbl_806E2168, 0);
    fn_803330AC()->RouterVirtual04(0);
    fn_8033288C(lbl_806E2138);

    u32 seed = fn_803236CC();
    fn_80332EC0(seed);

    void* state = lbl_806E2168;
    int count = fn_80338BF0(lbl_806E20D8);
    fn_80338AD8(state, (s8)fn_80338C20(lbl_806E20D8), count);

    UnidentifiedGameConfig config;
    fn_80122650(&config);
    UnidentifiedNetGameState* record = lbl_806E2164;
    if (record->mUnidentified00 != 0)
    {
        int machines = fn_80338BF0(lbl_806E20D8);
        fn_803380F4(
            record, (s8)fn_80338C20(lbl_806E20D8), machines, seed, &config,
            0x58);
    }
}

extern "C" void fn_80122EC0()
{
    UnidentifiedNetworkOnlineInterface& online = *lbl_806E20D8;
    online.OnlineVirtual08();
    fn_80338C2C(lbl_806E20D8, 1, 4);

    UnidentifiedNetworkPeer* peer = fn_80338BF8(lbl_806E20D8, 0);
    for (int player = 0; player < (int)peer->mUnidentified004; ++player)
    {
        fn_80336D50(fn_80336B6C(peer, player), peer, (s8)player, player);
    }
    AIPadManager::Startup();

    u32 seed = fn_803236CC();
    fn_80332EC0(seed);
    fn_8004F594(
        0x10, "StartSinglePlayerGame: Set random seed to %x\n", seed);
    fn_803330AC()->RouterVirtual04(0);

    void* state = lbl_806E2168;
    int count = fn_80338BF0(lbl_806E20D8);
    fn_80338AD8(state, (s8)fn_80338C20(lbl_806E20D8), count);

    UnidentifiedGameConfig config;
    fn_80122650(&config);
    UnidentifiedNetGameState* record = lbl_806E2164;
    if (record->mUnidentified00 != 0)
    {
        int machines = fn_80338BF0(lbl_806E20D8);
        fn_803380F4(
            record, (s8)fn_80338C20(lbl_806E20D8), machines, seed, &config,
            0x58);
    }
}

extern "C" void fn_8012300C()
{
    if (lbl_806E2164->mUnidentified03 == 0)
    {
        return;
    }
    if (!fn_80338284())
    {
        return;
    }

    UnidentifiedGameStartInfo* info =
        (UnidentifiedGameStartInfo*)lbl_806E2164->mUnidentifiedCC;
    fn_80332EC0(lbl_806E2164->mUnidentifiedD4);
    UnidentifiedGameConfig* config = info->mConfig;
    fn_80122748(config);
    fn_8004F594(
        0x10,
        "PlaybackRecordedGame: Random seed %x Stadium %d HOME %d "
        "[%d, %d, %d] Vs AWAY %d [%d, %d, %d]\n",
        info->mSeed, config->mStadium, config->mHomeTeam,
        config->mHomeSidekicks[0], config->mHomeSidekicks[1],
        config->mHomeSidekicks[2], config->mAwayTeam,
        config->mAwaySidekicks[0], config->mAwaySidekicks[1],
        config->mAwaySidekicks[2]);
    fn_8004F594(
        0x10,
        "PlaybackRecordedGame: Skill %d WinBy %s GameTime %d GameGoals %d "
        "BestSeries %d\n",
        config->mSettings[0],
        config->mSettings[1] == 0 ? "Timed" : "Goals",
        config->mSettings[2], config->mSettings[3], config->mSettings[4]);
    fn_803330AC()->RouterVirtual04(0);
    lbl_806E20D8->BaseVirtual3C(info);
    void* state = lbl_806E2168;
    int count = fn_80338BF0(lbl_806E20D8);
    fn_80338AD8(state, (s8)fn_80338C20(lbl_806E20D8), count);
}

void UnidentifiedNetworkSession::BaseVirtual48(int reason)
{
    mUnidentified244C = reason;
    mUnidentified2448 = 6;
    fn_802B2E8C(&mUnidentified2464);
    fn_802B2E8C(&mUnidentified2468);

    if (mUnidentified2473 != 0)
    {
        fn_8012935C(fn_80127558(), mUnidentified244C);
    }
    else
    {
        UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
        if (roster != 0)
        {
            roster->RosterVirtual4C(true);
        }
    }

    if (OnlineVirtual0C() == 2)
    {
        fn_80131DB4(fn_8012F8F8());
    }
}

int UnidentifiedNetworkSession::Send(
    s8 player, void* buffer, int size, bool reliable)
{
    NetworkSocket_801246E4* socket = GetDirectSocket();
    if ((int)player == mUnidentified2424)
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
    if (fn_80127558()->mUnidentified925 != 0)
    {
        machine = (s8)fn_80127F64(fn_80127558(), (s8)machine);
    }

    u32 aid = roster->GetMachineAid((s8)machine);
    if (aid == 0)
    {
        return 0;
    }
    socket->Send(aid, buffer, size, reliable);
    return 1;
}

int UnidentifiedNetworkSession::fn_80123314()
{
    if (OnlineVirtual0C() == 0)
    {
        return 0;
    }
    return lbl_806E2164->mUnidentified04 == 0;
}

int UnidentifiedNetworkSession::fn_80123360()
{
    if (mUnidentified2448 == 5)
    {
        return 1;
    }
    if (!lbl_806E10EC->fn_80123314())
    {
        return 0;
    }

    if (mOverlayRequest == 3)
    {
        for (int machine = 0; machine < fn_80338BF0(this); ++machine)
        {
            if (machine == (s8)fn_80338C20(this))
            {
                continue;
            }
            u32 aid;
            if ((s8)machine == mUnidentified2424)
            {
                aid = 0xFFFFFFFF;
            }
            else
            {
                UnidentifiedMachineRoster* roster = GetMachineRoster();
                int index;
                if (fn_80127558()->mUnidentified925 != 0)
                {
                    index = (s8)fn_80127F64(fn_80127558(), (s8)machine);
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
        mUnidentified2448 = 5;
        RegisterLoadedGameActions(this);
        return 1;
    }

    for (int machine = 0; machine < fn_80338BF0(this); ++machine)
    {
        if (mUnidentified246E[machine] == 0)
        {
            return 0;
        }
    }

    fn_8004F594(0x10, "Game has loaded for everyone!\n");
    if (GetMachineRoster()->RosterVirtual08() == 1 && (s8)fn_80338C20(this) == 0)
    {
        NetMessageLoadedGameEveryone message;
        u8 buffer[0xC8];
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0xC8);
        fn_8004F594(
            0x10,
            "HOST sending Loaded Game Everyone message to all clients\n");
        int count = fn_80338BF0(this);
        for (s8 target = 1; target < count; ++target)
        {
            Send(target, buffer, size, true);
        }
    }
    mUnidentified2448 = 5;
    RegisterLoadedGameActions(this);
    return 1;
}

void UnidentifiedNetworkSession::fn_801239F8()
{
}

void UnidentifiedNetworkSession::fn_801239FC()
{
}

u8 UnidentifiedNetworkSession::fn_80123A00()
{
    return mUnidentified246D;
}

void UnidentifiedNetworkSession::fn_80123A08()
{
    int ready;
    if (OnlineVirtual0C() == 0)
    {
        ready = 0;
    }
    else
    {
        ready = lbl_806E2164->mUnidentified04 == 0;
    }

    if (ready == 0)
    {
        mUnidentified2448 = 5;
        RegisterLoadedGameActions(this);
        return;
    }

    if (GetMachineRoster()->RosterVirtual08() == 0)
    {
        NetMessageLoadedGame message;
        u8 buffer[0xC8];
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0xC8);
        fn_8004F594(
            0x10, "Machine %d sending Loaded Game message\n",
            (s8)fn_80338C20(this));
        int count = fn_80338BF0(this);
        for (s8 target = 0; target < count; ++target)
        {
            Send(target, buffer, size, true);
        }
    }
    else if ((s8)fn_80338C20(this) == 0)
    {
        mUnidentified246E[0] = 1;
    }
    else
    {
        NetMessageLoadedGameClient message;
        u8 buffer[0xC8];
        u32 size = fn_8032C830(lbl_806E2100, &message, buffer, 0xC8);
        fn_8004F594(
            0x10, "Machine %d sending Loaded Game CLIENT message to HOST\n",
            (s8)fn_80338C20(this));
        Send(0, buffer, size, true);
    }
}

void UnidentifiedNetworkSession::fn_80123E44(u8 value)
{
    NetworkLobby_80133634* lobby = mSessionMode == 2 ? mLobby : 0;
    if (lobby != 0)
    {
        ((u8*)lobby)[9] = value;
    }
    mUnidentified2473 = value;
}

bool UnidentifiedNetworkSession::fn_80123E70(u32 connection)
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

    if (fn_80127558()->mUnidentified925 != 0)
    {
        for (int machine = 0; machine < fn_80338BF0(this); ++machine)
        {
            int index = fn_80127F64(fn_80127558(), machine);
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

void UnidentifiedNetworkSession::fn_80123FBC(int overlay)
{
    PopupNetworkErrorOverlay(this, overlay);
}

void UnidentifiedNetworkSession::fn_80124038(u32 connection, int reason)
{
    switch (OnlineVirtual10())
    {
    case 3:
        if (fn_80123E70(connection))
        {
            if (fn_80130888(fn_8012F8F8()))
            {
                fn_8013243C(fn_8012F8F8(), 4);
            }
        }
        else
        {
            fn_8004F594(
                0x10, "Connection %x not one of our peers, ignoring\n",
                connection);
        }
        break;

    case 5:
        if (fn_80123E70(connection))
        {
            fn_80132968(fn_8012F8F8(), 4);
            PopupNetworkErrorOverlay(this, 0);
        }
        else
        {
            fn_8004F594(
                0x10, "Connection %x not one of our peers, ignoring\n",
                connection);
        }
        break;

    case 6:
        fn_8004F594(
            0x10,
            "WARNING: Ignored Connection Lost Stage "
            "ENetworkStage_GameEnded\n");
        break;

    case 1:
    case 2:
    case 4:
        break;

    default:
        fn_8004F594(
            0x10, "WARNING: Ignored Connection Lost %x unknown network stage "
                  "%d\n",
            connection, mUnidentified2448);
        break;
    }
}

void UnidentifiedNetworkSession::fn_801241C8()
{
    if (OnlineVirtual0C() == 2)
    {
        if (fn_8012F8F8() != 0)
        {
            fn_8013157C(fn_8012F8F8());
        }
        fn_80134298(mSessionMode == 2 ? mLobby : 0);
    }
}

struct UnidentifiedDraftEntryBlock
{
    u32 mData[0x100];
};

struct UnidentifiedDraftSceneState
{
    /* 0x00 */ u8 mUnidentified00[0x24];
    /* 0x24 */ u32 mUnidentified24;
    /* 0x28 */ s8 mUnidentified28;
    /* 0x29 */ s8 mUnidentified29;
    /* 0x2A */ u8 mUnidentified2A;
    /* 0x2B */ UnidentifiedDraftFooter mUnidentified2B;
    /* 0x33 */ u8 mUnidentified33;
    /* 0x34 */ UnidentifiedDraftEntryBlock mEntries;
};

extern "C" void fn_801243A8(
    UnidentifiedDraftSceneState* scene, NetMessageDraft message)
{
    scene->mUnidentified24 = message.mUnidentified04;
    scene->mUnidentified28 = message.mMachineIndex;
    scene->mUnidentified29 = message.mMachineCount;
    scene->mUnidentified2A = message.mUnidentified0A;
    scene->mUnidentified2B = message.mUnidentified0B;
    scene->mEntries = *(UnidentifiedDraftEntryBlock*)message.mEntries;
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

inline TweakValueBoolImpl_804F4538::TweakValueBoolImpl_804F4538(
    const char* group, const char* name, bool* value, bool defaultValue)
    : m_pValue(value)
{
    mName = name;
    mUnidentified009 = defaultValue;

    if (fn_802C0F04() == 0)
    {
        void* entry = nlMalloc(0x18, 8, true);
        if (entry != 0)
        {
            fn_802C2DF4((TweakPendingValue*)entry, this, group);
        }
        lbl_806E1E90 = group;
    }
    else
    {
        TweakEntry_8052BF00* config = fn_802C0E30();
        TweakEntry_8052BF00* entry = fn_802C4504(config, group, 0);
        if (entry != 0)
        {
            fn_802C5780(entry, this);
        }
    }
}

static TweakValueBoolImpl_804F4538 s_NoPopupNetworkErrorTweak(
    "Network", "g_bNoPopupNetworkError", &lbl_806E10E8, true);

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

UnidentifiedNetworkDelegate* UnidentifiedNetworkDelegate::Clone()
{
    return new UnidentifiedNetworkDelegate(*this);
}
