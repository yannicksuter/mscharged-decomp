#include "Game/NetworkSession.h"
#include "Game/Sys/debug.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "unclassified/tu_802BAE84.h"
#include "unclassified/tu_8032452C.h"

#include <string.h>

struct NetworkSocketInitializeInfo
{
    u32 mVersionWord;
    bool mDirectMode;
};

extern "C"
{
    int DWC_SendUnreliable(u8 aid, const void* buffer, int size);
    long SOGetHostID();
}

int g_nLocalDirectPort = 1000;
static int sLocalAddressColumn = 2;
static int sLocalAddressRow = 5;

extern TweakValueBool_804F4578 g_bDisplayLocAddr;

NetworkSocket_801246E4* NetworkSocket_801246E4::sInstance;

NetworkSocket_801246E4::NetworkSocket_801246E4()
    : mInitialized(false)
    , mDirectMode(true)
{
    mConnectionEnabled = false;
    mVersionWord = 0;
    mListener = 0;
    TransportSocketInitialize(&mBroadcastSocket);
    TransportSocketInitialize(&mDirectSocket);
    mHasLocalAddress = false;
    sInstance = this;
}

void NetworkSocket_801246E4::OnConnectionAttempted(
    u32 connection, int result)
{
    if (result != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Connect failed (%d)\n", result);
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK, "Connected\n");
    }
    mListener->ListenerVirtual0C(connection, result);
}

void NetworkSocket_801246E4::OnConnectionClosed(
    u32 connection, int reason)
{
    tDebugPrintManager::Print(DC_NETWORK, "Connection closed: ");
    if (reason == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Local Close\n");
    }
    else if (reason == 1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Remote Close\n");
    }
    else if (reason == 2)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Communication Error\n");
    }
    else if (reason == 3)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Socket Error\n");
    }
    else if (reason == 4)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Not Enough Memory\n");
    }
    mListener->ListenerVirtual10(connection, reason);
}

void NetworkSocket_801246E4::ReliableCallbackVirtual08()
{
}

void NetworkSocket_801246E4::ReliableCallbackVirtual0C(
    u32 connection, void* buffer, int size, bool reliable)
{
    mListener->ListenerVirtual04(connection, buffer, size, reliable);
}

void NetworkSocket_801246E4::ReliableCallbackVirtual10(
    u32 connection, void* buffer, int size)
{
    mListener->ListenerVirtual18();
}

void NetworkSocket_801246E4::ReliableCallbackVirtual14(
    u32 connection, u8* address, int a, int b, int c)
{
    mListener->ListenerVirtual08(connection, address);
}

int NetworkSocket_801246E4::SendDatagram(
    void* buffer, int size, const u8* address, u16 port)
{
    if (mDirectMode)
    {
        return TransportSocketSendTo(&mDirectSocket, buffer, size, address, port);
    }

    int aid = address[3];
    bool sent = DWC_SendUnreliable(aid, buffer, size);
    if (!sent)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to send message of size %d to aid %d.\n", size, aid);
        return -1;
    }
    return size;
}

void NetworkSocket_801246E4::Initialize(
    void* info, UnidentifiedNetworkConnectionListener* listener)
{
    NetworkSocketInitializeInfo* socketInfo =
        (NetworkSocketInitializeInfo*)info;
    mVersionWord = socketInfo->mVersionWord;
    mDirectMode = socketInfo->mDirectMode;
    mListener = listener;

    if (mDirectMode)
    {
        bool started = false;
        if (!TransportSocketOpen(&mDirectSocket, false))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Direct socket open error\n");
        }
        else if (!TransportSocketBind(&mDirectSocket, (u16)g_nLocalDirectPort))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Direct sock bind failed\n");
        }
        else
        {
            TransportSocketSetNonBlocking(&mDirectSocket, false);
            started = true;
        }

        if (!started)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Failed to startup reliable UDP direct socket\n");
            return;
        }
    }

    fn_80324778(&mReliableSocket,
        static_cast<UnidentifiedReliableSocketCallback*>(this));
    mInitialized = true;
}

void NetworkSocket_801246E4::Shutdown()
{
    if (mDirectMode)
    {
        SetBroadcastEnabled(false);
    }

    if (mInitialized)
    {
        fn_80324828(&mReliableSocket);
        if (mDirectMode)
        {
            TransportSocketClose(&mDirectSocket);
        }
        mInitialized = false;
    }

    mVersionWord = 0;
    mListener = 0;
    mHasLocalAddress = false;
}

void NetworkSocket_801246E4::SetBroadcastEnabled(bool enabled)
{
    if (!enabled && TransportSocketIsOpen(&mBroadcastSocket))
    {
        TransportSocketClose(&mBroadcastSocket);
    }

    if (enabled && !TransportSocketIsOpen(&mBroadcastSocket))
    {
        if (!TransportSocketOpen(&mBroadcastSocket, false))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Broadcast socket open error\n");
        }
        else if (!TransportSocketBind(&mBroadcastSocket, 1001))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Broadcast socket bind failed\n");
        }
        else
        {
            TransportSocketSetNonBlocking(&mBroadcastSocket, false);
        }
    }
}

void NetworkSocket_801246E4::SendBroadcast(void* buffer, int size)
{
    if (!TransportSocketIsOpen(&mBroadcastSocket))
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Broadcast ignored because broadcast is currently turned off.\n");
        return;
    }

    memcpy(mPacketBuffer, &mVersionWord, sizeof(mVersionWord));
    memcpy(mPacketBuffer + sizeof(mVersionWord), buffer, size);
    TransportSocketBroadcast(&mBroadcastSocket, mPacketBuffer,
        size + sizeof(mVersionWord), 1001);
}

void NetworkSocket_801246E4::SocketVirtual10(bool enabled)
{
    fn_80324918(&mReliableSocket, enabled);
    mConnectionEnabled = enabled;
}

bool NetworkSocket_801246E4::SocketVirtual14()
{
    return mConnectionEnabled;
}

bool NetworkSocket_801246E4::Connect(
    void* connection, const u8* address, u16 port)
{
    if (fn_80324920(&mReliableSocket, connection, address, port) != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Failed initial connect attempt\n");
        return false;
    }
    return true;
}

void NetworkSocket_801246E4::AcceptConnection(u32 connection)
{
    fn_80325404(&mReliableSocket, connection);
}

void NetworkSocket_801246E4::RejectConnection(u32 connection)
{
    fn_8032540C(&mReliableSocket, connection);
}

void NetworkSocket_801246E4::SocketVirtual24(
    UnidentifiedTransportConnection* connection, bool immediate)
{
    fn_80324A1C(&mReliableSocket, connection, immediate);
}

void* NetworkSocket_801246E4::FindConnection(const u8* address)
{
    return fn_80325388(&mReliableSocket, address);
}

void NetworkSocket_801246E4::Send(
    int aid, void* buffer, int size, bool reliable)
{
    fn_80324A28(&mReliableSocket, aid, buffer, size, reliable);
}

void NetworkSocket_801246E4::Receive(void* buffer, int size)
{
    mListener->ListenerVirtual04(-1, buffer, size, true);
}

void NetworkSocket_801246E4::SocketVirtual34(
    u8 aid, void* buffer, int size)
{
    fn_80324A4C(&mReliableSocket, aid, buffer, size);
}

void NetworkSocket_801246E4::Update(float)
{
    if (!mInitialized)
    {
        return;
    }

    if (mDirectMode)
    {
        if (TransportSocketIsOpen(&mBroadcastSocket))
        {
            int received = TransportSocketReceiveFrom(
                &mBroadcastSocket, mPacketBuffer, sizeof(mPacketBuffer), 0, 0);
            if (received > 0 && (u32)received >= sizeof(mVersionWord)
                && memcmp(mPacketBuffer, &mVersionWord,
                       sizeof(mVersionWord))
                    == 0)
            {
                mListener->ListenerVirtual00(
                    mPacketBuffer + sizeof(mVersionWord),
                    received - sizeof(mVersionWord));
            }
        }

        if (TransportSocketIsOpen(&mDirectSocket))
        {
            int received;
            do
            {
                u32 address;
                u16 port;
                received = TransportSocketReceiveFrom(&mDirectSocket, mPacketBuffer,
                    sizeof(mPacketBuffer), &address, &port);
                if (received > 0)
                {
                    fn_80325264(&mReliableSocket, mPacketBuffer, received,
                        (u8*)&address, port);
                }
            } while (received > 0);
        }
    }

    fn_80324D1C(&mReliableSocket);
}

void NetworkSocket_801246E4::ReceiveUnreliable(
    u8 aid, void* buffer, int size)
{
    u8 address[4];
    address[3] = aid;
    address[0] = 0;
    address[1] = 0;
    address[2] = 0;
    fn_80325264(&mReliableSocket, buffer, size, address, 0);
}

void NetworkSocket_801246E4::SocketVirtual44(int a, int* b, bool c)
{
    if (mInitialized)
    {
        fn_80324A5C(&mReliableSocket, a, b, c);
    }
}

void NetworkSocket_801246E4::SocketVirtual48()
{
    if (!mInitialized)
    {
        return;
    }

    if (mHasLocalAddress && g_bDisplayLocAddr)
    {
        fn_802BB048(sLocalAddressRow, sLocalAddressColumn, 0, 1,
            "LocAddr %d.%d.%d.%d", mLocalAddress[0], mLocalAddress[1],
            mLocalAddress[2], mLocalAddress[3]);
    }
    fn_80324B54(&mReliableSocket);
}

u8* NetworkSocket_801246E4::GetLocalAddress()
{
    if (!mHasLocalAddress)
    {
        *(u32*)mLocalAddress = 0;
        *(u32*)mLocalAddress = SOGetHostID();
        if (*(u32*)mLocalAddress != 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Acquired local IP address %d.%d.%d.%d\n", mLocalAddress[0],
                mLocalAddress[1], mLocalAddress[2], mLocalAddress[3]);
            mHasLocalAddress = true;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Failed to get local IP address\n");
        }
    }

    if (mHasLocalAddress)
    {
        return mLocalAddress;
    }
    return 0;
}

u16 NetworkSocket_801246E4::GetLocalPort()
{
    return (u16)g_nLocalDirectPort;
}

TweakValueBool_804F4578 g_bDisplayLocAddr(
    "g_bDisplayLocAddr", "Network", true);
static TweakValueIntImpl_804FD898 sLocalDirectPortTweak(
    "g_nLocalDirectPort", "Network", &g_nLocalDirectPort, true);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
