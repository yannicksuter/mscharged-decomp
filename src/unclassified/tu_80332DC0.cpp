#include "unclassified/tu_80332DC0.h"
#include "unclassified/tu_80336B2C.h"

#include <string.h>

#include "Game/Event.h"
#include "Game/TweakValue.h"
#include "NL/nlMath.h"

extern "C" int fn_8004F594(int category, const char* format, ...);
u32 gNetworkRandomSeed = 0x12345678;

int g_TransmitSyncDataEvery = 4;
int g_nTicksPerPacket = 2;
int g_nOverrideTickDelayTarget = -1;
float g_fCSInputQHighwaterMarkMin = 4.0f;
float g_fCSInputQHighwaterMarkDef = 4.0f;
float g_fCSInputQHighwaterMarkMax = 7.0f;
float g_fCSInputQHighwaterMarkInc = 0.8f;
float g_fCSInputQHighwaterMarkDec = 0.03f;
int g_nCSHostDelayMin = 1;
int g_nCSHostDelayMax = 10;
float fMinCongestionMultiplier = 2.0f;
float fDefCongestionMultiplier = 4.0f;
float fMaxCongestionMultiplier = 10.0f;
float fCongestionMultiplierIncrement = 0.3f;
float fCongestionMultiplierDecrement = 0.06f;
int g_nDumpSyncTextAt = -1;
u32 g_SyncMismatchColour = 0xFF0000FF;
u32 g_SyncTextColour = 0xFFFFFFFF;

UnidentifiedInputRouter* lbl_806E2150;
NetworkInputRouter* lbl_806E2154;

static EventDispatcher lbl_8058490C;
static UnidentifiedQueuedEvent<DetermDataEvent> lbl_8058493C(
    &lbl_8058490C, "DetermDataEventQueue", 21);

NetworkMessageType0_80533B7C::~NetworkMessageType0_80533B7C()
{
}

DetermDataEvent::~DetermDataEvent()
{
}

NetworkMessageType8_80533BA4::~NetworkMessageType8_80533BA4()
{
}

extern "C" u32 fn_80332EB8()
{
    return gNetworkRandomSeed;
}

extern "C" void fn_80332EC0(u32 seed)
{
    gNetworkRandomSeed = seed;
}

extern "C" void fn_80332EC8()
{
}

u32 nlRandom(u32 range)
{
    return nlRandom(range, &gNetworkRandomSeed);
}

float nlRandomf(float maximum)
{
    return nlRandomf(maximum, &gNetworkRandomSeed);
}

extern "C" void fn_80332EDC()
{
    lbl_806E2150 = new SimpleInputRouter;
    lbl_806E2154 = new NetworkInputRouter;
}

extern "C" UnidentifiedInputRouter* fn_803330AC()
{
    if (lbl_806E20D8->OnlineVirtual0C() == 0)
    {
        return lbl_806E2150;
    }
    return lbl_806E2154;
}

UnidentifiedInputRouter::~UnidentifiedInputRouter()
{
}

void UnidentifiedInputRouter::Reset(int)
{
    mSession = lbl_806E20D8;

    int machineCount = fn_80338BF0(mSession);
    for (int machine = 0; machine < machineCount; ++machine)
    {
        fn_80336BE0(&mSession->mPeers[machine]);
    }

    for (int input = 0; input < 16; ++input)
    {
        memset(&mInputRecords[input], 0, sizeof(InputRouterRecord));
        mInputStates[input] = 2;
    }

    for (int machine = 0; machine < 4; ++machine)
    {
        mNetworkTicks[machine] = 0;
        mNetworkCRCs[machine] = 0;
        mRemoteTicks[machine] = 0;
        mRandomSeeds[machine] = 0;
    }

    mCurrentCRC = 0;
    mLastGameFrame = -1;
    mOutgoingHead = 0;
    mOutgoingCount = 0;
    mSyncMismatch = false;
    mSyncMismatchReported = false;
    mOutgoingQueueOverflowed = false;
    mStarvedForInput = false;
}

void UnidentifiedInputRouter::RouterVirtual38()
{
    int machineCount = fn_80338BF0(mSession);
    int gameFrame = lbl_806E2138->mFrameProvider->GetFrame();
    u32 seed = fn_80332EB8();

    bool mismatch = false;
    for (int machine = 0; machine < machineCount; ++machine)
    {
        if (mRemoteTicks[machine] != (u32)gameFrame
            || mNetworkCRCs[machine] != mCurrentCRC
            || mRandomSeeds[machine] != seed)
        {
            mismatch = true;
        }
    }

    if (mismatch && !mSyncMismatch)
    {
        mSyncMismatch = true;
    }
}

bool UnidentifiedInputRouter::RouterVirtual34()
{
    while (mOutgoingCount != 0)
    {
        DetermDataEvent* event = mOutgoingDetermData[mOutgoingHead];
        mOutgoingHead = (mOutgoingHead + 1) % mOutgoingCapacity;
        --mOutgoingCount;

        Function<DetermDataEvent*> disposer(
            (void (*)(DetermDataEvent*))DetermDataEvent::operator delete);
        lbl_8058493C.Queue(event, disposer);
    }

    RouterVirtual38();
    return true;
}

void UnidentifiedInputRouter::RouterVirtual3C()
{
    if (!mSyncMismatchReported)
    {
        mSyncMismatchReported = true;
    }
}

extern "C" void fn_80333908(
    UnidentifiedInputRouter* router, const void* data, u32 size)
{
    DetermDataEvent* event = 0;
    if (lbl_805848E8.m_FreeList == 0)
    {
        SlotPoolBase::BaseAddNewBlock(&lbl_805848E8, sizeof(DetermDataEvent));
    }
    if (lbl_805848E8.m_FreeList != 0)
    {
        event = (DetermDataEvent*)lbl_805848E8.m_FreeList;
        lbl_805848E8.m_FreeList = lbl_805848E8.m_FreeList->next;
        event->mSize = size;
        memcpy(event->mData, data, size);
    }

    if (router->mOutgoingCount < router->mOutgoingCapacity)
    {
        u32 position = (router->mOutgoingHead + router->mOutgoingCount)
            % router->mOutgoingCapacity;
        router->mOutgoingDetermData[position] = event;
        ++router->mOutgoingCount;
    }
    else
    {
        fn_8004F594(0x10, "m_OutgoingCustomDetermDataQ overflowed\n");
        router->mOutgoingQueueOverflowed = true;
    }
}

extern "C" void fn_80333A18()
{
    lbl_8058490C.Dispatch(true);
}

extern "C" UnidentifiedQueuedEvent<DetermDataEvent>* fn_80333A30()
{
    return &lbl_8058493C;
}

SimpleInputRouter::~SimpleInputRouter()
{
}

void SimpleInputRouter::Reset(int resetQueues)
{
    UnidentifiedInputRouter::Reset(resetQueues);
}

void SimpleInputRouter::RouterVirtual1C()
{
    int frame = lbl_806E2138->mFrameProvider->GetFrame();
    if (mLastGameFrame != frame)
    {
        mCurrentCRC = 0;
        mLastGameFrame = frame;
    }
}

void SimpleInputRouter::RouterVirtual20()
{
    RouterVirtual38();
}

NetworkInputRouter::~NetworkInputRouter()
{
}

void NetworkInputRouter::Reset(int resetQueues)
{
    UnidentifiedInputRouter::Reset(resetQueues);
    mCongested = false;
    mWasCongested = false;
    mCongestionMultiplier = fDefCongestionMultiplier;
    mUnidentified290 = 0;
    mUnidentified294 = 0;

    for (int machine = 0; machine < 4; ++machine)
    {
        mInputQueues[machine].mHead = 0;
        mInputQueues[machine].mCount = 0;
    }
    mQueueCursor = 0;
    mQueueLimit = 4;
}

void NetworkInputRouter::RouterVirtual24()
{
    mCongested = false;
    int machineCount = fn_80338BF0(mSession);
    for (int machine = 0; machine < machineCount; ++machine)
    {
        if (mInputQueues[machine].mCount <= 1)
        {
            mCongested = true;
        }
    }
    if (mCongested)
    {
        mWasCongested = true;
    }
}

bool NetworkInputRouter::RouterVirtual14()
{
    if (mQueueCursor > mQueueLimit)
    {
        return true;
    }

    int machineCount = fn_80338BF0(mSession);
    for (int machine = 0; machine < machineCount; ++machine)
    {
        if (mInputQueues[machine].mCount == 0)
        {
            return false;
        }
    }
    return true;
}

void NetworkInputRouter::RouterVirtual1C()
{
    for (int machine = 0; machine < 4; ++machine)
    {
        NetworkInputMessageQueue& queue = mInputQueues[machine];
        if (queue.mCount != 0)
        {
            queue.mHead = (queue.mHead + 1) % queue.mCapacity;
            --queue.mCount;
        }
    }
}

void NetworkInputRouter::RouterVirtual20()
{
    RouterVirtual38();
}

void NetworkInputRouter::RouterVirtual28(
    s8 machine, NetworkMessageType0_80533B7C* message)
{
    NetworkInputMessageQueue& queue = mInputQueues[machine];
    if (queue.mCount < queue.mCapacity)
    {
        u32 position = (queue.mHead + queue.mCount) % queue.mCapacity;
        memcpy(&queue.mMessages[position], message, sizeof(*message));
        ++queue.mCount;
    }
    else
    {
        fn_8004F594(0x10, "m_InputQueue[%d] overflowed\n", machine);
        mOutgoingQueueOverflowed = true;
    }
}

void NetworkInputRouter::RouterVirtual30()
{
    if (mStarvedForInput)
    {
        fn_8004F594(0x10, "StarvedForInput");
    }
}

NetworkMessageType9_80533B90::~NetworkMessageType9_80533B90()
{
}

NetworkInputMessageQueue::NetworkInputMessageQueue()
    : mMessages(mStorage)
    , mHead(0)
    , mCount(0)
    , mCapacity(60)
{
}

NetworkInputMessageQueue::~NetworkInputMessageQueue()
{
}

NetworkMessageType1_80533B68::~NetworkMessageType1_80533B68()
{
}

void NetworkInputRouter::RouterVirtual2C(
    s8, NetworkMessageType8_80533BA4*)
{
}

bool NetworkInputRouter::RouterVirtual18()
{
    return !mStarvedForInput;
}

int NetworkInputRouter::RouterVirtual10()
{
    return 1;
}

void SimpleInputRouter::RouterVirtual30()
{
}

void SimpleInputRouter::RouterVirtual2C(
    s8, NetworkMessageType8_80533BA4*)
{
}

void SimpleInputRouter::RouterVirtual28(
    s8, NetworkMessageType0_80533B7C*)
{
}

void SimpleInputRouter::RouterVirtual24()
{
}

bool SimpleInputRouter::RouterVirtual18()
{
    return true;
}

bool SimpleInputRouter::RouterVirtual14()
{
    return true;
}

int SimpleInputRouter::RouterVirtual10()
{
    return 1;
}

static TweakValueIntImpl_804FD898 sTransmitSyncDataEvery(
    "g_TransmitSyncDataEvery", "Network/InputMan", &g_TransmitSyncDataEvery);
static TweakValueIntImpl_804FD898 sTicksPerPacket(
    "g_nTicksPerPacket", "Network/InputMan", &g_nTicksPerPacket);
static TweakValueImpl_804F4DC8 sInputQHighwaterMarkMin(
    "g_fCSInputQHighwaterMarkMin", "Network/InputMan",
    &g_fCSInputQHighwaterMarkMin);
static TweakValueImpl_804F4DC8 sInputQHighwaterMarkDef(
    "g_fCSInputQHighwaterMarkDef", "Network/InputMan",
    &g_fCSInputQHighwaterMarkDef);
static TweakValueImpl_804F4DC8 sInputQHighwaterMarkMax(
    "g_fCSInputQHighwaterMarkMax", "Network/InputMan",
    &g_fCSInputQHighwaterMarkMax);
static TweakValueImpl_804F4DC8 sInputQHighwaterMarkInc(
    "g_fCSInputQHighwaterMarkInc", "Network/InputMan",
    &g_fCSInputQHighwaterMarkInc);
static TweakValueImpl_804F4DC8 sInputQHighwaterMarkDec(
    "g_fCSInputQHighwaterMarkDec", "Network/InputMan",
    &g_fCSInputQHighwaterMarkDec);
static TweakValueIntImpl_804FD898 sHostDelayMin(
    "g_nCSHostDelayMin", "Network/InputMan", &g_nCSHostDelayMin);
static TweakValueIntImpl_804FD898 sHostDelayMax(
    "g_nCSHostDelayMax", "Network/InputMan", &g_nCSHostDelayMax);
static TweakValueImpl_804F4DC8 sCongestionMultiplierMin(
    "fMinCongestionMultiplier", "Network/InputMan",
    &fMinCongestionMultiplier);
static TweakValueImpl_804F4DC8 sCongestionMultiplierDef(
    "fDefCongestionMultiplier", "Network/InputMan",
    &fDefCongestionMultiplier);
static TweakValueImpl_804F4DC8 sCongestionMultiplierMax(
    "fMaxCongestionMultiplier", "Network/InputMan",
    &fMaxCongestionMultiplier);
static TweakValueImpl_804F4DC8 sCongestionMultiplierIncrement(
    "fCongestionMultiplierIncrement", "Network/InputMan",
    &fCongestionMultiplierIncrement);
static TweakValueImpl_804F4DC8 sCongestionMultiplierDecrement(
    "fCongestionMultiplierDecrement", "Network/InputMan",
    &fCongestionMultiplierDecrement);
static TweakValueIntImpl_804FD898 sOverrideTickDelayTarget(
    "g_nOverrideTickDelayTarget", "Network/InputMan",
    &g_nOverrideTickDelayTarget);
static TweakValueIntImpl_804FD898 sDumpSyncTextAt(
    "g_nDumpSyncTextAt", "Network/InputMan", &g_nDumpSyncTextAt);

typedef char VerifyDetermDataEventSize[(sizeof(DetermDataEvent) == 0x24) ? 1 : -1];
typedef char VerifyNetworkMessageType0Size[
    (sizeof(NetworkMessageType0_80533B7C) == 0xF0) ? 1 : -1];
typedef char VerifyNetworkMessageType8Size[
    (sizeof(NetworkMessageType8_80533BA4) == 0x3D8) ? 1 : -1];
typedef char VerifyInputRouterSize[
    (sizeof(UnidentifiedInputRouter) == 0x198) ? 1 : -1];
typedef char VerifyInputQueueSize[
    (sizeof(NetworkInputMessageQueue) == 0x3850) ? 1 : -1];
typedef char VerifyNetworkInputRouterSize[
    (sizeof(NetworkInputRouter) == 0xE5C8) ? 1 : -1];
