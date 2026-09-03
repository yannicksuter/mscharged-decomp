#include "Game/NetworkSession.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "unclassified/tu_802BAE84.h"

#include <string.h>

struct NetworkSocketInitializeInfo
{
    u32 mVersionWord;
    bool mDirectMode;
};

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);
    int DWC_SendUnreliable(u8 aid, const void* buffer, int size);
    long SOGetHostID();

    void fn_8032452C(UnidentifiedReliableSocketState* socket);
    int fn_80324778(UnidentifiedReliableSocketState* socket,
        UnidentifiedReliableSocketCallback* callback);
    void fn_80324828(UnidentifiedReliableSocketState* socket);
    void fn_80324918(UnidentifiedReliableSocketState* socket, bool enabled);
    int fn_80324920(UnidentifiedReliableSocketState* socket,
        void* connection, const u8* address, u16 port);
    void fn_80324A1C(
        UnidentifiedReliableSocketState* socket, void* a, void* b);
    void fn_80324A28(UnidentifiedReliableSocketState* socket, int aid,
        void* buffer, int size, bool reliable);
    void fn_80324A4C(UnidentifiedReliableSocketState* socket, u8 aid,
        void* buffer, int size);
    void fn_80324A5C(UnidentifiedReliableSocketState* socket, void* a,
        void* b, bool c);
    void fn_80324B54(UnidentifiedReliableSocketState* socket);
    void fn_80324D1C(UnidentifiedReliableSocketState* socket);
    void fn_80325264(UnidentifiedReliableSocketState* socket, void* buffer,
        int size, const u8* address, u16 port);
    void* fn_80325388(
        UnidentifiedReliableSocketState* socket, const u8* address);
    void fn_80325404(UnidentifiedReliableSocketState* socket, u32 connection);
    void fn_8032540C(UnidentifiedReliableSocketState* socket, u32 connection);

    void fn_80374D68(UnidentifiedDatagramSocket* socket);
    bool fn_80374D74(UnidentifiedDatagramSocket* socket, bool stream);
    bool fn_80374DF0(UnidentifiedDatagramSocket* socket, u16 port);
    void fn_80374EA8(UnidentifiedDatagramSocket* socket);
    bool fn_80374EEC(UnidentifiedDatagramSocket* socket);
    void fn_80374F04(UnidentifiedDatagramSocket* socket, bool blocking);
    int fn_80375018(UnidentifiedDatagramSocket* socket, void* buffer,
        int size, u16 port);
    int fn_803750A0(UnidentifiedDatagramSocket* socket, void* buffer,
        int size, const u8* address, u16 port);
    int fn_80375138(UnidentifiedDatagramSocket* socket, void* buffer,
        int size, u32* address, u16* port);
}

int g_nLocalDirectPort = 1000;
static int sLocalAddressColumn = 2;
static int sLocalAddressRow = 5;

extern TweakValueBool_804F4578 g_bDisplayLocAddr;

NetworkSocket_801246E4* NetworkSocket_801246E4::sInstance;

NetworkSocket_801246E4::NetworkSocket_801246E4()
{
    mInitialized = false;
    mDirectMode = true;
    fn_8032452C(&mReliableSocket);
    mConnectionEnabled = false;
    mVersionWord = 0;
    mListener = 0;
    fn_80374D68(&mBroadcastSocket);
    fn_80374D68(&mDirectSocket);
    mHasLocalAddress = false;
    sInstance = this;
}

void NetworkSocket_801246E4::OnConnectionAttempted(
    u32 connection, int result)
{
    if (result != 0)
    {
        fn_8004F594(0x10, "Connect failed (%d)\n", result);
    }
    else
    {
        fn_8004F594(0x10, "Connected\n");
    }
    mListener->ListenerVirtual0C(connection, result);
}

void NetworkSocket_801246E4::OnConnectionClosed(
    u32 connection, int reason)
{
    fn_8004F594(0x10, "Connection closed: ");
    if (reason == 0)
    {
        fn_8004F594(0x10, "Local Close\n");
    }
    else if (reason == 1)
    {
        fn_8004F594(0x10, "Remote Close\n");
    }
    else if (reason == 2)
    {
        fn_8004F594(0x10, "Communication Error\n");
    }
    else if (reason == 3)
    {
        fn_8004F594(0x10, "Socket Error\n");
    }
    else if (reason == 4)
    {
        fn_8004F594(0x10, "Not Enough Memory\n");
    }
    mListener->ListenerVirtual10(connection, reason);
}

void NetworkSocket_801246E4::ReliableCallbackVirtual08()
{
}

void NetworkSocket_801246E4::ReliableCallbackVirtual0C(
    int source, void* buffer, int size, bool reliable)
{
    mListener->ListenerVirtual04(source, buffer, size, reliable);
}

void NetworkSocket_801246E4::ReliableCallbackVirtual10()
{
    mListener->ListenerVirtual18();
}

void NetworkSocket_801246E4::ReliableCallbackVirtual14(
    u32 connection, u8* address)
{
    mListener->ListenerVirtual08(connection, address);
}

int NetworkSocket_801246E4::SendDatagram(
    void* buffer, int size, const u8* address, u16 port)
{
    if (mDirectMode)
    {
        return fn_803750A0(&mDirectSocket, buffer, size, address, port);
    }

    int aid = address[3];
    if (!DWC_SendUnreliable(aid, buffer, size))
    {
        fn_8004F594(0x10,
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
        if (!fn_80374D74(&mDirectSocket, false))
        {
            fn_8004F594(0x10, "Direct socket open error\n");
        }
        else if (!fn_80374DF0(&mDirectSocket, (u16)g_nLocalDirectPort))
        {
            fn_8004F594(0x10, "Direct sock bind failed\n");
        }
        else
        {
            fn_80374F04(&mDirectSocket, false);
            started = true;
        }

        if (!started)
        {
            fn_8004F594(
                0x10, "Failed to startup reliable UDP direct socket\n");
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
            fn_80374EA8(&mDirectSocket);
        }
        mInitialized = false;
    }

    mVersionWord = 0;
    mListener = 0;
    mHasLocalAddress = false;
}

void NetworkSocket_801246E4::SetBroadcastEnabled(bool enabled)
{
    if (!enabled && fn_80374EEC(&mBroadcastSocket))
    {
        fn_80374EA8(&mBroadcastSocket);
    }

    if (enabled && !fn_80374EEC(&mBroadcastSocket))
    {
        if (!fn_80374D74(&mBroadcastSocket, false))
        {
            fn_8004F594(0x10, "Broadcast socket open error\n");
        }
        else if (!fn_80374DF0(&mBroadcastSocket, 1001))
        {
            fn_8004F594(0x10, "Broadcast socket bind failed\n");
        }
        else
        {
            fn_80374F04(&mBroadcastSocket, false);
        }
    }
}

void NetworkSocket_801246E4::SendBroadcast(void* buffer, int size)
{
    if (!fn_80374EEC(&mBroadcastSocket))
    {
        fn_8004F594(0x10,
            "Broadcast ignored because broadcast is currently turned off.\n");
        return;
    }

    memcpy(mPacketBuffer, &mVersionWord, sizeof(mVersionWord));
    memcpy(mPacketBuffer + sizeof(mVersionWord), buffer, size);
    fn_80375018(&mBroadcastSocket, mPacketBuffer,
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
        fn_8004F594(0x10, "Failed initial connect attempt\n");
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

void NetworkSocket_801246E4::SocketVirtual24(void* a, void* b)
{
    fn_80324A1C(&mReliableSocket, a, b);
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
        if (fn_80374EEC(&mBroadcastSocket))
        {
            int received = fn_80375138(
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

        if (fn_80374EEC(&mDirectSocket))
        {
            int received;
            do
            {
                u32 address;
                u16 port;
                received = fn_80375138(&mDirectSocket, mPacketBuffer,
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

void NetworkSocket_801246E4::SocketVirtual44(void* a, void* b, bool c)
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
            fn_8004F594(0x10,
                "Acquired local IP address %d.%d.%d.%d\n", mLocalAddress[0],
                mLocalAddress[1], mLocalAddress[2], mLocalAddress[3]);
            mHasLocalAddress = true;
        }
        else
        {
            fn_8004F594(0x10, "Failed to get local IP address\n");
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
    "g_nLocalDirectPort", "Network", &g_nLocalDirectPort);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
