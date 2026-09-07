#include "Game/NetworkSession.h"
#include "Game/Sys/debug.h"
#include "Game/NetworkDebug_80323B2C.h"
#include "Game/NetworkRandom_803236CC.h"

#include "Game/TweakValue.h"
#include "NL/gl/glFont.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_802B7798.h"
#include "unclassified/tu_80326844.h"
#include "unclassified/tu_80330430.h"

#include <string.h>

extern "C"
{
}

void RegisterNetworkMessages_80330430();
extern char lbl_80584360[12];
extern int lbl_805317E8[4];

float g_fBroadCastFindGameTime = 1.0f;
float g_fLANGameExpireTime = 3.5f;
float g_fLANConfirmConnectionTimeout = 10000.0f;

extern UnidentifiedTransportGame_8032CA4C lbl_805848B8;

NetworkTransport_8032CA4C::NetworkTransport_8032CA4C()
{
    fn_8032CBD0(true);
}

void NetworkTransport_8032CA4C::fn_8032CBC8()
{
    fn_8032CBD0(false);
}

void NetworkTransport_8032CA4C::fn_8032CBD0(bool initialize)
{
    if (initialize)
        mUnidentified008 = false;
    mUnidentified00C = 0;
    mUnidentified010 = 8;
    mUnidentified054 = 0;
    mUnidentified058 = 0;
    if (initialize)
        mUnidentified05C = new (8, false) UnidentifiedTransportGame_8032CA4C[10];
    mUnidentified060 = 0;
    mUnidentified07D = false;
    mUnidentified084 = 0;
    mUnidentified095 = false;
    mUnidentified09C = false;
    mUnidentified098 = 0.0f;
    mUnidentified064[0] = '\0';
    mUnidentified070 = -1;
    mUnidentified074 = 0;
    mUnidentified1C0 = 0;
    mUnidentified1CC = initialize ? 0 : g_pNetworkSessionBase->GetDirectSocket();
    for (int index = 0; index < 8; ++index)
    {
        m_ConnectionPool[index].m_Connection = 0;
        m_ConnectionPool[index].mUnidentified04 = 0;
    }
    mUnidentified080 = 0;
    mUnidentified090 = 0;
    mState = 0;
    mUnidentified08C = 0;
    mUnidentified094 = false;
    for (int index = 0; index < 8; ++index)
        mUnidentified1C4[index] = 0;
    if (!initialize)
    {
        RegisterNetworkMessages_80330430();
        lbl_806E2100->fn_8032CA1C(2, this);
        lbl_806E2100->fn_8032CA1C(3, this);
        lbl_806E2100->fn_8032CA1C(4, this);
        lbl_806E2100->fn_8032CA1C(5, this);
        lbl_806E2100->fn_8032CA1C(7, this);
        lbl_806E2100->fn_8032CA1C(10, this);
        lbl_806E2100->fn_8032CA1C(11, this);
        lbl_806E2100->fn_8032CA1C(12, this);
        if (lbl_80584360[0] != '\0')
            nlStrNCpy(mUnidentified064, lbl_80584360, 11);
        if (mUnidentified064[0] == '\0')
            fn_8032376C(mUnidentified064, 11);
        if (!mUnidentified095)
        {
            mUnidentified095 = true;
            mUnidentified098 = g_fBroadCastFindGameTime;
        }
        mUnidentified008 = true;
    }
}

void NetworkTransport_8032CA4C::fn_8032CEAC()
{
    lbl_806E2100->fn_8032CA2C(2);
    lbl_806E2100->fn_8032CA2C(3);
    lbl_806E2100->fn_8032CA2C(4);
    lbl_806E2100->fn_8032CA2C(5);
    lbl_806E2100->fn_8032CA2C(7);
    lbl_806E2100->fn_8032CA2C(10);
    lbl_806E2100->fn_8032CA2C(11);
    lbl_806E2100->fn_8032CA2C(12);
    if (mUnidentified054 != 0)
        mUnidentified054->UnidentifiedVirtual18();
    mUnidentified060 = 0;
    mUnidentified1CC = 0;
    mUnidentified008 = false;
}

int NetworkTransport_8032CA4C::fn_8032CF60(int value)
{
    u8* address = mUnidentified1CC->GetLocalAddress();
    if (address == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Failed to Create Game, failed to get local address!\n");
        return 1;
    }
    mUnidentified07D = true;
    mUnidentified080 = 0;
    mUnidentified084 = value;
    mState = 0;
    mUnidentified08C = 0;
    mUnidentified094 = false;
    mUnidentified070 = 0;
    mUnidentified1C0 = 1;
    nlStrNCpy(mUnidentified0A0[0].mName, mUnidentified064, 11);
    mUnidentified0A0[0].mUnidentified0B = mUnidentified074;
    memcpy(&mUnidentified0A0[0].mUnidentified0C, mUnidentified075, mUnidentified074);
    memcpy(mUnidentified0A0[0].mUnidentified14, address, 4);
    mUnidentified0A0[0].mUnidentified20 = mUnidentified1CC->GetLocalPort();
    mUnidentified0A0[0].mUnidentified18 = 0;
    mUnidentified0A0[0].mUnidentified22 = false;
    mUnidentified0A0[0].mUnidentified1C = -1;
    if (mUnidentified095)
        mUnidentified095 = false;
    mUnidentified09C = !g_bDirectConnectMode;
    mUnidentified1CC->SocketVirtual10(true);
    if (mUnidentified054 != 0)
        mUnidentified054->UnidentifiedVirtual00(0);
    return 0;
}

int NetworkTransport_8032CA4C::fn_8032D0EC()
{
    return fn_8032D0F4(8);
}

int NetworkTransport_8032CA4C::fn_8032D0F4(int result)
{
    if (!mUnidentified07D)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Ignored abort create game..We are not a host\n");
        return 3;
    }
    mUnidentified07D = false;
    mUnidentified084 = 0;
    mUnidentified094 = false;
    mState = 0;
    mUnidentified08C = 0;
    mUnidentified070 = -1;
    mUnidentified1C0 = 0;
    if (!mUnidentified095)
    {
        mUnidentified095 = true;
        mUnidentified098 = g_fBroadCastFindGameTime;
    }
    mUnidentified09C = false;
    mUnidentified1CC->SocketVirtual10(false);
    if (mUnidentified054 != 0)
        mUnidentified054->UnidentifiedVirtual00(result);
    mUnidentified080 = 0;
    for (int index = 0; index < 8; ++index)
    {
        if (m_ConnectionPool[index].m_Connection != 0)
            mUnidentified1CC->SocketVirtual24(m_ConnectionPool[index].m_Connection, true);
    }
    return 0;
}

int NetworkTransport_8032CA4C::fn_8032D220(UnidentifiedTransportGame_8032CA4C* game, int value)
{
    if (mUnidentified090 != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Ignored join current join state %d\n", mUnidentified090);
        return 2;
    }
    if (mUnidentified1CC->GetLocalAddress() == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Failed to Join Game, failed to get local address!\n");
        return 1;
    }
    if (game == 0)
    {
        if (g_bDirectConnectMode)
        {
            game = &lbl_805848B8;
            nlStrNCpy(game->mUnidentified00, "Server", 11);
            game->mUnidentified0C[0] = lbl_805317E8[0];
            game->mUnidentified0C[1] = lbl_805317E8[1];
            game->mUnidentified0C[2] = lbl_805317E8[2];
            game->mUnidentified0C[3] = lbl_805317E8[3];
            game->mUnidentified14 = 0.0f;
            game->mUnidentified10 = 0;
            game->mUnidentified18 = g_nConnectToServerPort;
        }
        else
        {
            for (int index = 0; index < mUnidentified060; ++index)
            {
                if (mUnidentified05C[index].mUnidentified10 == value)
                {
                    game = &mUnidentified05C[index];
                    break;
                }
            }
            if (game == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK, "No games to join\n");
                return 5;
            }
        }
    }
    mUnidentified094 = false;
    if (mUnidentified00C == 0)
        mUnidentified1CC->SocketVirtual10(true);
    m_ConnectionPool[0].mUnidentified04 = 1;
    if (mUnidentified1CC->Connect(&m_ConnectionPool[0].m_Connection,
            game->mUnidentified0C,
            game->mUnidentified18))
    {
        tDebugPrintManager::Print(DC_NETWORK, "Attempting connection\n");
    }
    else
    {
        m_ConnectionPool[0].m_Connection = 0;
        m_ConnectionPool[0].mUnidentified04 = 0;
        mUnidentified1CC->SocketVirtual10(false);
        tDebugPrintManager::Print(DC_NETWORK, "Connection failed at outset\n");
        return 4;
    }
    mUnidentified090 = 1;
    return 0;
}

void NetworkTransport_8032CA4C::OnGameStarted()
{
    mUnidentified09C = false;
    mUnidentified1CC->SocketVirtual10(false);
    if (mUnidentified07D)
        mUnidentified080 = 2;
    tDebugPrintManager::Print(DC_NETWORK, "LANLobby Game Started\n");
    fn_8032D5E0();
}

void NetworkTransport_8032CA4C::Shutdown(bool)
{
    for (int index = 0; index < 8; ++index)
    {
        if (m_ConnectionPool[index].m_Connection != 0)
            mUnidentified1CC->SocketVirtual24(m_ConnectionPool[index].m_Connection, true);
    }
    mUnidentified07D = false;
    mUnidentified080 = 0;
    if (!mUnidentified095)
    {
        mUnidentified095 = true;
        mUnidentified098 = g_fBroadCastFindGameTime;
    }
    mUnidentified09C = false;
    mUnidentified1CC->SocketVirtual10(false);
    mUnidentified090 = 0;
    mUnidentified094 = false;
    mUnidentified070 = -1;
    mUnidentified1C0 = 0;
}

void NetworkTransport_8032CA4C::fn_8032D5E0()
{
    tDebugPrintManager::Print(DC_NETWORK, "Dumping %d entries in PeerInfoList\n", mUnidentified1C0);
    for (int index = 0; index < mUnidentified1C0; ++index)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Peer %d address %d.%d.%d.%d port %d hoststate %d connInd %d connConf %d\n", index, mUnidentified0A0[index].mUnidentified14[0], mUnidentified0A0[index].mUnidentified14[1], mUnidentified0A0[index].mUnidentified14[2], mUnidentified0A0[index].mUnidentified14[3], mUnidentified0A0[index].mUnidentified20, mUnidentified0A0[index].mUnidentified18, mUnidentified0A0[index].mUnidentified1C, mUnidentified0A0[index].mUnidentified22);
    }
    tDebugPrintManager::Print(DC_NETWORK, "Dumping ConnectionPool contents\n");
    for (int index = 0; index < 8; ++index)
    {
        UnidentifiedTransportConnection* connection = m_ConnectionPool[index].m_Connection;
        if (connection == 0)
            tDebugPrintManager::Print(DC_NETWORK, "ConnPool %d Status %d\n", index, m_ConnectionPool[index].mUnidentified04);
        else
            tDebugPrintManager::Print(DC_NETWORK, "ConnPool %d Status %d ConnAddr %d.%d.%d.%d\n", index, m_ConnectionPool[index].mUnidentified04, connection->mAddress[0], connection->mAddress[1], connection->mAddress[2], connection->mAddress[3]);
    }
}

u32 NetworkTransport_8032CA4C::GetMachineAid(int index)
{
    if (mUnidentified1C0 == 0)
        return 0;
    if (index < 0)
        return 0;
    if (mUnidentified1C0 <= index)
        return 0;
    if (index == mUnidentified070)
        return (u32)-1;
    int connectionIndex = mUnidentified0A0[index].mUnidentified1C;
    if (connectionIndex != -1 && m_ConnectionPool[connectionIndex].mUnidentified04 == 2)
        return (u32)m_ConnectionPool[connectionIndex].m_Connection;
    return 0;
}

int NetworkTransport_8032CA4C::MachineIdxFromConnection(u32 connection)
{
    if (mUnidentified1C0 == 0)
        return -1;
    if (connection == 0 || connection == (u32)-2)
        return -1;
    if (connection == (u32)-1)
        return mUnidentified070;
    UnidentifiedTransportConnection* entry = (UnidentifiedTransportConnection*)connection;
    int connectionIndex = UnidentifiedConnectionIndex(entry);
    if (connectionIndex == -1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Failed to get connection index from conn addr %d.%d.%d.%d\n", entry->mAddress[0], entry->mAddress[1], entry->mAddress[2], entry->mAddress[3]);
        return -1;
    }
    for (int index = 0; index < mUnidentified1C0; ++index)
    {
        if (mUnidentified0A0[index].mUnidentified1C == connectionIndex)
            return index;
    }
    tDebugPrintManager::Print(DC_NETWORK, "Failed to get peer index from connectionIndex %d conn addr %d.%d.%d.%d\n", connectionIndex, entry->mAddress[0], entry->mAddress[1], entry->mAddress[2], entry->mAddress[3]);
    fn_8032D5E0();
    return -1;
}

void NetworkTransport_8032CA4C::fn_8032D94C(UnidentifiedTransportListener_8032CA4C* listener)
{
    mUnidentified054 = listener;
}

void NetworkTransport_8032CA4C::fn_8032D954(UnidentifiedTransportListener_8032D954* listener)
{
    mUnidentified058 = listener;
}

void NetworkTransport_8032CA4C::OnConnected(u32 connection, int result)
{
    UnidentifiedTransportConnection* entry = (UnidentifiedTransportConnection*)connection;
    if (!mUnidentified07D)
    {
        if (mUnidentified090 == 1)
        {
            if (result == 0)
            {
                NetworkMessageType4_80533468 message;
                memcpy(message.mUnidentified08, mUnidentified1CC->GetLocalAddress(), 4);
                message.mUnidentified0C = mUnidentified1CC->GetLocalPort();
                nlStrNCpy(message.mUnidentified0E, mUnidentified064, 11);
                message.mUnidentified19 = mUnidentified074;
                memcpy(message.mUnidentified1A, mUnidentified075, mUnidentified074);
                u8 buffer[200];
                int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
                int index = UnidentifiedConnectionIndex(entry);
                if (index == -1)
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Connection established %x, but cannot find connection in pool\n", connection);
                    return;
                }
                m_ConnectionPool[index].mUnidentified04 = 2;
                mUnidentified1CC->Send(connection, buffer, size, true);
                mUnidentified090 = 2;
                tDebugPrintManager::Print(DC_NETWORK, "Sent Join Request to machine %d\n", index);
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "Join failed because ConnectionEstablished returned error %d\n", result);
                mUnidentified090 = 0;
                mUnidentified094 = false;
                int index = UnidentifiedConnectionIndex(entry);
                if (index != -1)
                {
                    m_ConnectionPool[index].m_Connection = 0;
                    m_ConnectionPool[index].mUnidentified04 = 0;
                }
                if (mUnidentified054 != 0)
                    mUnidentified054->UnidentifiedVirtual04(4);
            }
        }
        else if (mUnidentified00C == 0)
        {
            if (result == 0)
            {
                int index = UnidentifiedConnectionIndex(entry);
                if (index == -1)
                    tDebugPrintManager::Print(DC_NETWORK, "Connection established %x, but cannot find connection in pool\n", connection);
                else
                    m_ConnectionPool[index].mUnidentified04 = 2;
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "ConnectionEstablished returned error %d.  Peer to peer connection failed.\n", result);
                int index = UnidentifiedConnectionIndex(entry);
                if (index != -1)
                {
                    m_ConnectionPool[index].m_Connection = 0;
                    m_ConnectionPool[index].mUnidentified04 = 0;
                }
            }
        }
        else
            tDebugPrintManager::Print(DC_NETWORK, "Ignoring connection established notification, not in peer peer topology, client state = %d ConnResult = %d\n", mUnidentified090, result);
    }
    else
        tDebugPrintManager::Print(DC_NETWORK, "Ignoring connection established notification, host state = %d ConnResult = %d\n", mUnidentified080, result);
}

int NetworkTransport_8032CA4C::ShouldAcceptConnection(u32 connection, u8* address)
{
    int index = UnidentifiedFreeConnectionIndex();
    if (index == -1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Rejected connection attempt from %d.%d.%d.%d because no free connection in pool\n", address[0], address[1], address[2], address[3]);
        return 0;
    }
    if (mUnidentified07D)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Connection attempt accepted by host.  Address %d.%d.%d.%d assigned to connection pool %d\n", address[0], address[1], address[2], address[3], index);
        m_ConnectionPool[index].m_Connection = (UnidentifiedTransportConnection*)connection;
        m_ConnectionPool[index].mUnidentified04 = 2;
        return 1;
    }
    if (mUnidentified00C == 0)
    {
        int count = mUnidentified1C0;
        for (int peer = 1; peer < count; ++peer)
        {
            if (memcmp(mUnidentified0A0[peer].mUnidentified14, address, 4) == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Connection attempt accepted by client.  Address %d.%d.%d.%d assigned to connection pool %d\n", address[0], address[1], address[2], address[3], index);
                m_ConnectionPool[index].m_Connection = (UnidentifiedTransportConnection*)connection;
                m_ConnectionPool[index].mUnidentified04 = 2;
                mUnidentified0A0[peer].mUnidentified1C = index;
                return 1;
            }
        }
        tDebugPrintManager::Print(DC_NETWORK, "Rejected client-client connection attempt from %d.%d.%d.%d because from unknown client\n", address[0], address[1], address[2], address[3]);
        fn_8032D5E0();
        return 0;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Rejected connection attempt from %d.%d.%d.%d because I am not a host and am not in peer-peer topology\n", address[0], address[1], address[2], address[3]);
    return 0;
}

void NetworkTransport_8032CA4C::OnConnectionClosed(u32 connection, int)
{
    bool foundConnection = false;
    bool foundPeer = false;
    for (int index = 0; index < 8; ++index)
    {
        if (m_ConnectionPool[index].m_Connection != 0
            && (u32)m_ConnectionPool[index].m_Connection == connection)
        {
            for (int peer = 0; peer < mUnidentified1C0; ++peer)
            {
                if (mUnidentified0A0[peer].mUnidentified1C == index)
                {
                    tDebugPrintManager::Print(DC_NETWORK, "I peer %d (%s) lost connection to peer %d\n", mUnidentified070, mUnidentified07D ? "host" : "client", peer);
                    mUnidentified0A0[peer].mUnidentified1C = -1;
                    foundPeer = true;
                }
            }
            if (!foundPeer)
                tDebugPrintManager::Print(DC_NETWORK, "Lost connection. Failed to find which peer was using connection pool %d\n", index);
            m_ConnectionPool[index].m_Connection = 0;
            m_ConnectionPool[index].mUnidentified04 = 0;
            foundConnection = true;
        }
    }
    if (!foundConnection)
        tDebugPrintManager::Print(DC_NETWORK, "Lost connection but failed to find connection pool associated with that connection\n");
    else if (!foundPeer)
        tDebugPrintManager::Print(DC_NETWORK, "Because did not find peer using this connection pool, just return\n");
    else if (mUnidentified07D)
    {
        if (mUnidentified054 != 0)
            mUnidentified054->UnidentifiedVirtual00(7);
    }
    else if (mUnidentified054 != 0)
        mUnidentified054->UnidentifiedVirtual04(7);
}

void NetworkTransport_8032CA4C::fn_8032E31C(const void* data)
{
    NetworkMessageType3_805333C8 message;
    u8* address = mUnidentified1CC->GetLocalAddress();
    if (address == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Not sending found game message because have no local address!\n");
        return;
    }
    u16 port = mUnidentified1CC->GetLocalPort();
    memcpy(message.mUnidentified08, data, 8);
    message.mUnidentified10 = mUnidentified084;
    memcpy(message.mUnidentified14, address, 4);
    message.mUnidentified18 = port;
    nlStrNCpy(message.mUnidentified1A, mUnidentified064, 11);
    u8 buffer[250];
    int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
    mUnidentified1CC->SendBroadcast(buffer, size);
}

void NetworkTransport_8032CA4C::Update(float dt)
{
    int index = 0;
    while (index < mUnidentified060)
    {
        mUnidentified05C[index].mUnidentified14 += dt;
        if (mUnidentified05C[index].mUnidentified14 >= g_fLANGameExpireTime)
        {
            if (mUnidentified054 != 0)
                mUnidentified054->UnidentifiedVirtual14(&mUnidentified05C[index]);
            int count = mUnidentified060 - (index + 1);
            if (count >= 1)
                memmove(&mUnidentified05C[index], &mUnidentified05C[index + 1], count * sizeof(*mUnidentified05C));
            --mUnidentified060;
        }
        else
            ++index;
    }
    if (mUnidentified095)
    {
        mUnidentified098 += dt;
        if (mUnidentified098 >= g_fBroadCastFindGameTime)
        {
            NetworkMessageType2_805333DC message;
            message.mUnidentified08[0] = fn_803236CC();
            message.mUnidentified08[1] = fn_803236CC();
            memcpy(mUnidentified1C4, message.mUnidentified08, 8);
            u8 buffer[200];
            int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
            mUnidentified1CC->SendBroadcast(buffer, size);
            mUnidentified098 = 0.0f;
        }
    }
    if (mState == 1)
    {
        bool ready = mUnidentified1C0 >= 2;
        if (ready)
        {
            for (int peer = 1; peer < mUnidentified1C0; ++peer)
            {
                if (mUnidentified0A0[peer].mUnidentified18 != 3)
                {
                    ready = false;
                    break;
                }
            }
        }
        if (ready)
        {
            if (RosterVirtual08() == 0)
            {
                mState = 2;
                fn_8032E8C0();
            }
            else
                mState = 3;
        }
    }
    if (mState == 2)
    {
        bool ready = true;
        for (int peer = 1; peer < mUnidentified1C0; ++peer)
        {
            if (!mUnidentified0A0[peer].mUnidentified22)
                ready = false;
        }
        if (!ready)
        {
            float elapsed = nlGetTickerDifference(mUnidentified08C, nlGetTicker());
            if (elapsed > g_fLANConfirmConnectionTimeout)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Confirm connections timed out after %f ms\n", elapsed);
                if (mUnidentified054 != 0)
                    mUnidentified054->UnidentifiedVirtual08(9);
                mState = 0;
            }
        }
        else
            mState = 3;
    }
    if (mUnidentified094 && fn_8032EA20())
    {
        NetworkMessageType11_80533404 message;
        message.mUnidentified08 = 1;
        u8 buffer[8];
        int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
        int index = mUnidentified0A0[0].mUnidentified1C;
        if (index != -1)
        {
            if (m_ConnectionPool[index].m_Connection != 0)
            {
                mUnidentified1CC->Send((u32)m_ConnectionPool[index].m_Connection, buffer, size, true);
                tDebugPrintManager::Print(DC_NETWORK, "Sent ready to launch confirm\n");
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, m_ConnectionPool[connectionIndex].m_Connection is NULL\n");
        }
        else
            tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, no connection to host!\n");
        mUnidentified094 = false;
    }
}

int NetworkTransport_8032CA4C::GetPlayerCount()
{
    if (mUnidentified07D)
        return mUnidentified1C0;
    return 0;
}

int NetworkTransport_8032CA4C::fn_8032E84C()
{
    if (mUnidentified07D)
    {
        if (mUnidentified1C0 >= 2 && mState == 0)
        {
            mState = 1;
            return 0;
        }
        return 2;
    }
    return 3;
}

void NetworkTransport_8032CA4C::fn_8032E890()
{
    mState = 4;
    if (mUnidentified054 != 0)
        mUnidentified054->UnidentifiedVirtual08(0);
}

void NetworkTransport_8032CA4C::fn_8032E8C0()
{
    mUnidentified08C = nlGetTicker();
    NetworkMessageType10_80533418 message;
    u8 buffer[8];
    int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
    for (int peer = 1; peer < mUnidentified1C0; ++peer)
    {
        int index = mUnidentified0A0[peer].mUnidentified1C;
        if (index >= 0 && index < 8)
        {
            if (m_ConnectionPool[index].mUnidentified04 != 0)
            {
                mUnidentified1CC->Send((u32)m_ConnectionPool[index].m_Connection, buffer, size, true);
                tDebugPrintManager::Print(DC_NETWORK, "Sent ready to launch request to peer %d\n", peer);
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "Failed to send launch request to peer %d connIndx %d is not in use\n", peer, index);
                fn_8032D5E0();
            }
        }
        else
            tDebugPrintManager::Print(DC_NETWORK, "Failed to send launch request to peer %d connectionIndex == %d\n", peer, index);
    }
}

bool NetworkTransport_8032CA4C::fn_8032EA20()
{
    if (RosterVirtual08() == 0)
    {
        bool ready = true;
        for (int peer = 1; peer < mUnidentified1C0; ++peer)
        {
            if (peer != mUnidentified070)
            {
                int index = mUnidentified0A0[peer].mUnidentified1C;
                if (index == -1)
                    ready = false;
                else if (m_ConnectionPool[index].mUnidentified04 != 2)
                    ready = false;
            }
        }
        return ready;
    }
    return true;
}

void NetworkTransport_8032CA4C::fn_8032EB0C()
{
    for (int index = 0; index < mUnidentified060; ++index)
    {
        if (mUnidentified054 != 0)
            mUnidentified054->UnidentifiedVirtual0C(&mUnidentified05C[index]);
    }
}

void NetworkTransport_8032CA4C::fn_8032EB88(NetworkMessageType3_805333C8* message)
{
    if (mUnidentified060 >= 10)
        return;
    for (int index = 0; index < mUnidentified060; ++index)
    {
        if (nlStrCmp(mUnidentified05C[index].mUnidentified00, message->mUnidentified1A) == 0)
        {
            mUnidentified05C[index].mUnidentified14 = 0.0f;
            return;
        }
    }
    mUnidentified05C[mUnidentified060].mUnidentified10 = message->mUnidentified10;
    memcpy(mUnidentified05C[mUnidentified060].mUnidentified0C, message->mUnidentified14, 4);
    mUnidentified05C[mUnidentified060].mUnidentified18 = message->mUnidentified18;
    nlStrNCpy(mUnidentified05C[mUnidentified060].mUnidentified00, message->mUnidentified1A, 11);
    mUnidentified05C[mUnidentified060].mUnidentified14 = 0.0f;
    if (mUnidentified054 != 0)
        mUnidentified054->UnidentifiedVirtual0C(&mUnidentified05C[mUnidentified060]);
    ++mUnidentified060;
}

void NetworkTransport_8032CA4C::fn_8032ED28(int index)
{
    NetworkMessageType7_80533440 message;
    memcpy(message.mUnidentified08.mUnidentified00, mUnidentified0A0[index].mUnidentified14, 4);
    message.mUnidentified08.mUnidentified04 = mUnidentified0A0[index].mUnidentified20;
    nlStrNCpy(message.mUnidentified08.mUnidentified06, mUnidentified0A0[index].mName, 11);
    message.mUnidentified08.mUnidentified12 = mUnidentified0A0[index].mUnidentified0B;
    memcpy(message.mUnidentified08.mUnidentified13, &mUnidentified0A0[index].mUnidentified0C, mUnidentified0A0[index].mUnidentified0B);
    message.mUnidentified08.mUnidentified11 = index;
    u8 buffer[250];
    int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
    for (int peer = 1; peer < index; ++peer)
    {
        mUnidentified1CC->Send((u32)m_ConnectionPool[mUnidentified0A0[peer].mUnidentified1C].m_Connection,
            buffer,
            size,
            true);
    }
}

void NetworkTransport_8032CA4C::fn_8032EE7C(UnidentifiedTransportConnection* connection, bool accepted)
{
    NetworkMessageType5_80533454 message;
    memcpy(message.mUnidentified08, mUnidentified1CC->GetLocalAddress(), 4);
    message.mUnidentified0C = mUnidentified1CC->GetLocalPort();
    nlStrNCpy(message.mUnidentified0F, mUnidentified064, 11);
    message.mUnidentified1A = mUnidentified074;
    memcpy(message.mUnidentified1B, mUnidentified075, mUnidentified074);
    message.mUnidentified0E = accepted;
    message.mUnidentified23 = 0;
    if (mUnidentified1C0 > 2)
    {
        for (int peer = 1; peer < mUnidentified1C0 - 1; ++peer)
        {
            UnidentifiedNetworkPeerInfo_80330430& info = message.mUnidentified24[peer - 1];
            memcpy(info.mUnidentified00, mUnidentified0A0[peer].mUnidentified14, 4);
            info.mUnidentified04 = mUnidentified0A0[peer].mUnidentified20;
            nlStrNCpy(info.mUnidentified06, mUnidentified0A0[peer].mName, 11);
            info.mUnidentified12 = mUnidentified0A0[peer].mUnidentified0B;
            memcpy(info.mUnidentified13, &mUnidentified0A0[peer].mUnidentified0C, mUnidentified0A0[peer].mUnidentified0B);
            info.mUnidentified11 = peer;
            ++message.mUnidentified23;
        }
    }
    u8 buffer[250];
    int size = lbl_806E2100->fn_8032C830(&message, buffer, sizeof(buffer));
    mUnidentified1CC->Send((u32)connection, buffer, size, true);
}

void NetworkTransport_8032CA4C::fn_8032F084(int index, NetworkMessageType4_80533468* message)
{
    UnidentifiedTransportConnection* connection = m_ConnectionPool[index].m_Connection;
    if (mUnidentified1C0 < mUnidentified010 && mState == 0)
    {
        nlStrNCpy(mUnidentified0A0[mUnidentified1C0].mName, message->mUnidentified0E, 11);
        mUnidentified0A0[mUnidentified1C0].mUnidentified0B = message->mUnidentified19;
        memcpy(&mUnidentified0A0[mUnidentified1C0].mUnidentified0C, message->mUnidentified1A, message->mUnidentified19);
        memcpy(mUnidentified0A0[mUnidentified1C0].mUnidentified14, connection->mAddress, 4);
        mUnidentified0A0[mUnidentified1C0].mUnidentified20 = connection->mPort;
        if (RosterVirtual08() == 0)
            mUnidentified0A0[mUnidentified1C0].mUnidentified18 = 2;
        else
            mUnidentified0A0[mUnidentified1C0].mUnidentified18 = 3;
        mUnidentified0A0[mUnidentified1C0].mUnidentified22 = false;
        mUnidentified0A0[mUnidentified1C0].mUnidentified1C = index;
        ++mUnidentified1C0;
        fn_8032EE7C(connection, true);
        if (mUnidentified1C0 > 2 && RosterVirtual08() != 0)
            fn_8032ED28(mUnidentified1C0 - 1);
        if (mUnidentified058 != 0)
            mUnidentified058->UnidentifiedVirtual00();
        tDebugPrintManager::Print(DC_NETWORK, "Approved a join request\n");
        fn_8032D5E0();
    }
    else
    {
        fn_8032EE7C(connection, false);
        tDebugPrintManager::Print(DC_NETWORK, "Rejected a join request\n");
    }
}

void NetworkTransport_8032CA4C::fn_8032F2AC(int index, NetworkMessageType5_80533454* message)
{
    if (mUnidentified090 != 2)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Ignoring join response because in join state %d\n", mUnidentified090);
        return;
    }
    if (message->mUnidentified0E)
    {
        nlStrNCpy(mUnidentified0A0[0].mName, message->mUnidentified0F, 11);
        mUnidentified0A0[0].mUnidentified0B = message->mUnidentified1A;
        memcpy(&mUnidentified0A0[0].mUnidentified0C, message->mUnidentified1B, message->mUnidentified1A);
        memcpy(mUnidentified0A0[0].mUnidentified14, message->mUnidentified08, 4);
        mUnidentified0A0[0].mUnidentified20 = message->mUnidentified0C;
        mUnidentified0A0[0].mUnidentified1C = index;
        mUnidentified0A0[0].mUnidentified18 = 0;
        mUnidentified0A0[0].mUnidentified22 = false;
        int peer = 1;
        for (int entry = 0; entry < message->mUnidentified23; ++entry, ++peer)
        {
            UnidentifiedNetworkPeerInfo_80330430& info = message->mUnidentified24[entry];
            nlStrNCpy(mUnidentified0A0[peer].mName, info.mUnidentified06, 11);
            mUnidentified0A0[peer].mUnidentified0B = info.mUnidentified12;
            memcpy(&mUnidentified0A0[peer].mUnidentified0C, info.mUnidentified13, info.mUnidentified12);
            memcpy(mUnidentified0A0[peer].mUnidentified14, info.mUnidentified00, 4);
            mUnidentified0A0[peer].mUnidentified20 = info.mUnidentified04;
            mUnidentified0A0[peer].mUnidentified1C = -1;
            mUnidentified0A0[peer].mUnidentified18 = 0;
            mUnidentified0A0[peer].mUnidentified22 = false;
        }
        nlStrNCpy(mUnidentified0A0[peer].mName, mUnidentified064, 11);
        mUnidentified0A0[peer].mUnidentified0B = mUnidentified074;
        memcpy(&mUnidentified0A0[peer].mUnidentified0C, mUnidentified075, mUnidentified074);
        memcpy(mUnidentified0A0[peer].mUnidentified14, mUnidentified1CC->GetLocalAddress(), 4);
        mUnidentified0A0[peer].mUnidentified20 = mUnidentified1CC->GetLocalPort();
        mUnidentified0A0[peer].mUnidentified1C = -1;
        mUnidentified0A0[peer].mUnidentified18 = 0;
        mUnidentified0A0[peer].mUnidentified22 = false;
        mUnidentified070 = peer;
        mUnidentified1C0 = peer + 1;
        if (mUnidentified058 != 0)
            mUnidentified058->UnidentifiedVirtual00();
        mUnidentified090 = 3;
        if (mUnidentified054 != 0)
            mUnidentified054->UnidentifiedVirtual04(0);
        tDebugPrintManager::Print(DC_NETWORK, "Successfully joined game.\n");
        fn_8032D5E0();
        if (RosterVirtual08() == 0)
        {
            NetworkMessageType12_805333F0 response;
            response.mUnidentified08 = mUnidentified070;
            u8 buffer[8];
            int size = lbl_806E2100->fn_8032C830(&response, buffer, sizeof(buffer));
            u32 connection = GetMachineAid(0);
            if (connection == 0)
                tDebugPrintManager::Print(DC_NETWORK, "Could not send client confirmed join no connection to host\n");
            else
                mUnidentified1CC->Send(connection, buffer, size, true);
        }
    }
    else
    {
        mUnidentified1CC->SocketVirtual24(m_ConnectionPool[index].m_Connection, true);
        mUnidentified090 = 0;
        mUnidentified094 = false;
        mUnidentified1C0 = 0;
        mUnidentified070 = -1;
        if (mUnidentified054 != 0)
            mUnidentified054->UnidentifiedVirtual04(6);
        tDebugPrintManager::Print(DC_NETWORK, "Join was refused.\n");
    }
}

void NetworkTransport_8032CA4C::fn_8032F6B4(NetworkMessageType7_80533440* message)
{
    UnidentifiedNetworkPeerInfo_80330430& info = message->mUnidentified08;
    int peer = info.mUnidentified11;
    memcpy(mUnidentified0A0[peer].mUnidentified14, info.mUnidentified00, 4);
    mUnidentified0A0[peer].mUnidentified20 = info.mUnidentified04;
    nlStrNCpy(mUnidentified0A0[peer].mName, info.mUnidentified06, 11);
    mUnidentified0A0[peer].mUnidentified0B = info.mUnidentified12;
    memcpy(&mUnidentified0A0[peer].mUnidentified0C, info.mUnidentified13, info.mUnidentified12);
    mUnidentified0A0[peer].mUnidentified18 = 0;
    mUnidentified0A0[peer].mUnidentified22 = false;
    mUnidentified0A0[peer].mUnidentified1C = -1;
    ++mUnidentified1C0;
    tDebugPrintManager::Print(DC_NETWORK, "ProcessGamePeerAdded peer %d added\n", peer);
    fn_8032D5E0();
    if (mUnidentified00C == 0)
    {
        int index = UnidentifiedFreeConnectionIndex();
        if (index == -1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Unable to connect to just added peer, no pool connection space\n");
            return;
        }
        mUnidentified0A0[peer].mUnidentified1C = index;
        m_ConnectionPool[index].mUnidentified04 = 1;
        if (mUnidentified1CC->Connect(&m_ConnectionPool[index].m_Connection,
                info.mUnidentified00,
                info.mUnidentified04))
            tDebugPrintManager::Print(DC_NETWORK, "Attempting peer-peer connection to other client\n");
        else
        {
            m_ConnectionPool[index].m_Connection = 0;
            m_ConnectionPool[index].mUnidentified04 = 0;
            mUnidentified0A0[peer].mUnidentified1C = -1;
            tDebugPrintManager::Print(DC_NETWORK, "Connection failed to other client at outset\n");
        }
    }
}

int NetworkTransport_8032CA4C::ReceiverVirtual00(UnidentifiedNetworkMessage* message)
{
    switch ((u8)message->GetType())
    {
    case 2:
        if (mUnidentified07D && mUnidentified09C)
            fn_8032E31C(static_cast<NetworkMessageType2_805333DC*>(message)->mUnidentified08);
        break;
    case 3:
        if (!mUnidentified07D)
        {
            NetworkMessageType3_805333C8* reply = static_cast<NetworkMessageType3_805333C8*>(message);
            if (memcmp(reply->mUnidentified08, mUnidentified1C4, 8) == 0)
                fn_8032EB88(reply);
        }
        break;
    case 4:
        if (mUnidentified07D)
        {
            int index = UnidentifiedConnectionIndex((UnidentifiedTransportConnection*)message->mUnidentified04);
            if (index >= 0 && index < 8)
                fn_8032F084(index, static_cast<NetworkMessageType4_80533468*>(message));
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored join request because did not find connection in pool\n");
        }
        break;
    case 5:
        if (!mUnidentified07D)
        {
            int index = UnidentifiedConnectionIndex((UnidentifiedTransportConnection*)message->mUnidentified04);
            if (index >= 0 && index < 8)
                fn_8032F2AC(index, static_cast<NetworkMessageType5_80533454*>(message));
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored join response because did not find connection in pool\n");
        }
        break;
    case 7:
        if (!mUnidentified07D)
        {
            int index = UnidentifiedConnectionIndex((UnidentifiedTransportConnection*)message->mUnidentified04);
            if (index >= 0 && index < 8)
                fn_8032F6B4(static_cast<NetworkMessageType7_80533440*>(message));
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored game peer added because did not find connection in pool\n");
        }
        break;
    case 10:
        if (!mUnidentified07D)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Received ready to launch request\n");
            if (fn_8032EA20())
            {
                NetworkMessageType11_80533404 response;
                response.mUnidentified08 = 1;
                u8 buffer[8];
                int size = lbl_806E2100->fn_8032C830(&response, buffer, sizeof(buffer));
                int index = mUnidentified0A0[0].mUnidentified1C;
                if (index != -1)
                {
                    if (m_ConnectionPool[index].m_Connection != 0)
                    {
                        mUnidentified1CC->Send((u32)m_ConnectionPool[index].m_Connection, buffer, size, true);
                        tDebugPrintManager::Print(DC_NETWORK, "Sent ready to launch confirm\n");
                    }
                    else
                        tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, m_ConnectionPool[connectionIndex].m_Connection is NULL\n");
                }
                else
                    tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, no connection to host!\n");
                mUnidentified094 = false;
            }
            else
                mUnidentified094 = true;
        }
        break;
    case 11:
        if (mUnidentified07D)
        {
            int peer = MachineIdxFromConnection(message->mUnidentified04);
            if (peer > 0 && peer < mUnidentified1C0)
            {
                mUnidentified0A0[peer].mUnidentified22 = static_cast<NetworkMessageType11_80533404*>(message)->mUnidentified08;
                tDebugPrintManager::Print(DC_NETWORK, "Received ready to launch confirm from peer %d\n", peer);
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored Ready To Launch Confirm because did not find peer it's from\n");
        }
        break;
    case 12:
        if (mUnidentified07D && RosterVirtual08() == 0)
        {
            int peer = MachineIdxFromConnection(message->mUnidentified04);
            if (peer > 0 && peer < mUnidentified1C0)
            {
                NetworkMessageType12_805333F0* reply = static_cast<NetworkMessageType12_805333F0*>(message);
                mUnidentified0A0[reply->mUnidentified08].mUnidentified18 = 3;
                if (reply->mUnidentified08 > 1)
                    fn_8032ED28(reply->mUnidentified08);
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored ClientConfirmedJoin because did not find peer it's from\n");
        }
        break;
    }
    return 1;
}

int NetworkTransport_8032CA4C::RosterVirtual08()
{
    return mUnidentified00C;
}

void NetworkTransport_8032CA4C::RosterVirtual0C(int value)
{
    mUnidentified00C = value;
}

int NetworkTransport_8032CA4C::GetMaxMachineCount()
{
    return mUnidentified010;
}

void NetworkTransport_8032CA4C::RosterVirtual14(int value)
{
    mUnidentified010 = value;
}

int NetworkTransport_8032CA4C::GetMachineCount()
{
    return mUnidentified1C0;
}

UnidentifiedTransportPlayer* NetworkTransport_8032CA4C::GetPlayerInfo(int index)
{
    if (index >= 0 && index < mUnidentified1C0)
        return &mUnidentified0A0[index];
    return 0;
}

int NetworkTransport_8032CA4C::GetLocalMachineIndex()
{
    if (mUnidentified1C0 <= 0)
        return -1;
    return mUnidentified070;
}

UnidentifiedTransportPlayer* NetworkTransport_8032CA4C::GetLocalPlayerInfo()
{
    int index = GetLocalMachineIndex();
    if (index == -1)
        return 0;
    return &mUnidentified0A0[index];
}

void NetworkTransport_8032CA4C::SetUserMatchData(u8 size, const void* data)
{
    mUnidentified074 = size;
    memcpy(mUnidentified075, data, size);
}

void* NetworkTransport_8032CA4C::GetUserMatchData(u8* size)
{
    *size = mUnidentified074;
    return mUnidentified075;
}

void NetworkTransport_8032CA4C::DebugDraw(int column, int* row)
{
    glFontPrintf(GetDebugFontView(), column, (*row)++, "Name: %s", mUnidentified064);
    if (g_bDisplayNetworkVerbose)
    {
        if (mUnidentified07D)
            glFontPrintf(GetDebugFontView(), column, (*row)++, "Host st: %d", mUnidentified080);
        else
            glFontPrintf(GetDebugFontView(), column, (*row)++, "Client st: %d", mUnidentified090);
        if (mUnidentified060 > 0)
        {
            glFontPrintf(GetDebugFontView(), column, (*row)++, "Num Games:%d", mUnidentified060);
            for (int index = 0; index < mUnidentified060; ++index)
                glFontPrintf(GetDebugFontView(), column, (*row)++, mUnidentified05C[index].mUnidentified00);
        }
        if (mUnidentified1C0 > 0)
        {
            glFontPrintf(GetDebugFontView(), column, (*row)++, "InGame NumPeers:%d", mUnidentified1C0);
            for (int index = 0; index < mUnidentified1C0; ++index)
            {
                if (mUnidentified07D)
                    glFontPrintf(GetDebugFontView(), column, (*row)++, "%d: %d %s", index, mUnidentified0A0[index].mUnidentified18, mUnidentified0A0[index].mName);
                else
                    glFontPrintf(GetDebugFontView(), column, (*row)++, "%d: %s", index, mUnidentified0A0[index].mName);
            }
        }
    }
}

static TweakValueImpl_804F4DC8 lbl_80584868("g_fBroadCastFindGameTime", "Network/LANLobby", &g_fBroadCastFindGameTime, true);
static TweakValueImpl_804F4DC8 lbl_80584888("g_fLANGameExpireTime", "Network/LANLobby", &g_fLANGameExpireTime, true);
static TweakValueImpl_804F4DC8 lbl_805848A8("g_fLANConfirmConnectionTimeout", "Network/LANLobby", &g_fLANConfirmConnectionTimeout, true);
UnidentifiedTransportGame_8032CA4C lbl_805848B8;
