#include "Game/NetworkSession.h"
#include <dwc/dwc_transport_fwd.h>
#include <revolution/so_fwd.h>
#include "Game/Sys/debug.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Sys/tweak.h"
#include "NL/plat/ReliableSocket.h"

#include <string.h>

struct NetworkSocketInitializeInfo
{
    u32 mVersionWord;
    bool mDirectMode;
};

int g_nLocalDirectPort = 1000;
static int sLocalAddressColumn = 2;
static int sLocalAddressRow = 5;

extern TweakValueBool g_bDisplayLocAddr;

NetworkSocket* NetworkSocket::sInstance;

NetworkSocket::NetworkSocket()
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

void NetworkSocket::OnConnectionAttempted(
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
    mListener->OnConnected(connection, result);
}

void NetworkSocket::OnConnectionClosed(
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
    mListener->OnConnectionClosed(connection, reason);
}

void NetworkSocket::ReliableCallbackVirtual08()
{
}

void NetworkSocket::OnMessageReceived(
    u32 connection, void* buffer, int size, bool reliable)
{
    mListener->OnMessageReceived(connection, buffer, size, reliable);
}

void NetworkSocket::ReliableCallbackVirtual10(
    u32 connection, void* buffer, int size)
{
    mListener->ListenerVirtual18();
}

void NetworkSocket::OnConnectionRequest(
    u32 connection, u8* address, int a, int b, int c)
{
    mListener->OnConnectionRequest(connection, address);
}

int NetworkSocket::SendDatagram(
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

void NetworkSocket::Initialize(
    void* info, NetworkConnectionListener* listener)
{
    NetworkSocketInitializeInfo* socketInfo =
        (NetworkSocketInitializeInfo*)info;
    mVersionWord = socketInfo->mVersionWord;
    mDirectMode = socketInfo->mDirectMode;
    mListener = listener;

    if (mDirectMode)
    {
        bool started;
        if (!TransportSocketOpen(&mDirectSocket, false))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Direct socket open error\n");
            started = false;
        }
        else if (!TransportSocketBind(&mDirectSocket, (u16)g_nLocalDirectPort))
        {
            tDebugPrintManager::Print(DC_NETWORK, "Direct sock bind failed\n");
            started = false;
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

    mReliableSocket.Initialize(static_cast<ReliableSocketCallback*>(this));
    mInitialized = true;
}

void NetworkSocket::Shutdown()
{
    if (mDirectMode)
    {
        SetBroadcastEnabled(false);
    }

    if (mInitialized)
    {
        mReliableSocket.Shutdown();
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

void NetworkSocket::SetBroadcastEnabled(bool enabled)
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

void NetworkSocket::SendBroadcast(void* buffer, int size)
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

void NetworkSocket::SocketVirtual10(bool enabled)
{
    mReliableSocket.SetEnabled(enabled);
    mConnectionEnabled = enabled;
}

bool NetworkSocket::SocketVirtual14()
{
    return mConnectionEnabled;
}

bool NetworkSocket::Connect(
    void* connection, const u8* address, u16 port, int, int)
{
    if (mReliableSocket.Connect(connection, address, port) != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Failed initial connect attempt\n");
        return false;
    }
    return true;
}

void NetworkSocket::AcceptConnection(u32 connection)
{
    mReliableSocket.AcceptConnection(connection);
}

void NetworkSocket::RejectConnection(u32 connection)
{
    mReliableSocket.RejectConnection(connection);
}

void NetworkSocket::Disconnect(
    TransportConnection* connection, bool immediate)
{
    mReliableSocket.Disconnect(connection, immediate);
}

void* NetworkSocket::FindConnection(const u8* address)
{
    return mReliableSocket.FindConnection(address);
}

void NetworkSocket::Send(
    int aid, void* buffer, int size, bool reliable)
{
    mReliableSocket.Send(aid, buffer, size, reliable);
}

void NetworkSocket::Receive(void* buffer, int size)
{
    mListener->OnMessageReceived(-1, buffer, size, true);
}

void NetworkSocket::SocketVirtual34(
    u8 aid, void* buffer, int size)
{
    mReliableSocket.SocketVirtual34(aid, buffer, size);
}

void NetworkSocket::Update(float)
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
                    mReliableSocket.ReceiveDatagram(mPacketBuffer, received,
                        (u8*)&address, port);
                }
            } while (received > 0);
        }
    }

    mReliableSocket.Update();
}

void NetworkSocket::ReceiveUnreliable(
    u8 aid, void* buffer, int size)
{
    u8 address[4];
    address[3] = aid;
    address[0] = 0;
    address[1] = 0;
    address[2] = 0;
    mReliableSocket.ReceiveDatagram(buffer, size, address, 0);
}

void NetworkSocket::DebugDraw(int a, int* b, bool c)
{
    if (mInitialized)
    {
        mReliableSocket.DebugDraw(a, b, c);
    }
}

void NetworkSocket::SocketVirtual48()
{
    if (!mInitialized)
    {
        return;
    }

    if (mHasLocalAddress && g_bDisplayLocAddr)
    {
        nlScreenPrintf(sLocalAddressRow, sLocalAddressColumn, 0, 1,
            "LocAddr %d.%d.%d.%d", mLocalAddress[0], mLocalAddress[1],
            mLocalAddress[2], mLocalAddress[3]);
    }
    mReliableSocket.SocketVirtual48();
}

u8* NetworkSocket::GetLocalAddress()
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

u16 NetworkSocket::GetLocalPort()
{
    return (u16)g_nLocalDirectPort;
}

TweakValueBool g_bDisplayLocAddr(
    "g_bDisplayLocAddr", "Network", true);
static TweakIntBinding sLocalDirectPortTweak(
    "g_nLocalDirectPort", "Network", &g_nLocalDirectPort, true);

