#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkSession.h"
#include "Game/OnlinePlayer.h"
#include "Game/Sys/debug.h"
#include "Game/NetworkDebug.h"
#include "Game/NetworkRandom.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glFont.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include "NL/nlDebugViews.h"
#include "NL/plat/TransportConnection.h"
#include "Game/LANMessages.h"

#include <string.h>



float g_fBroadCastFindGameTime = 1.0f;
float g_fLANGameExpireTime = 3.5f;
float g_fLANConfirmConnectionTimeout = 10000.0f;

extern LANGameInfo gDirectConnectGameInfo;

LANLobby::LANLobby()
{
    Reset(true);
}

void LANLobby::Initialize()
{
    Reset(false);
}

void LANLobby::Reset(bool initialize)
{
    if (initialize)
        mInitialized = false;
    mTopology = 0;
    mMaxMachineCount = 8;
    mListener = 0;
    mRosterListener = 0;
    if (initialize)
        mFoundGames = new (8, false) LANGameInfo[10];
    mFoundGameCount = 0;
    mIsHost = false;
    mGameType = 0;
    mFindGameEnabled = false;
    mAdvertiseGame = false;
    mFindGameElapsedTime = 0.0f;
    mLocalPlayerName[0] = '\0';
    mLocalMachineIndex = -1;
    mUserMatchDataSize = 0;
    mPeerCount = 0;
    if (initialize)
        mSocket = 0;
    else
        mSocket = g_pNetworkSessionBase->GetDirectSocket();
    for (int index = 0; index < 8; ++index)
    {
        m_ConnectionPool[index].m_Connection = 0;
        m_ConnectionPool[index].mUnidentified04 = 0;
    }
    mHostState = 0;
    mJoinState = 0;
    mState = 0;
    mLaunchRequestTicker = 0;
    mLaunchConfirmationPending = false;
    for (int index = 0; index < 8; ++index)
        mFindGameToken[index] = 0;
    if (!initialize)
    {
        RegisterLANMessages();
        gNetworkMessageRegistry->RegisterReceiver(2, this);
        gNetworkMessageRegistry->RegisterReceiver(3, this);
        gNetworkMessageRegistry->RegisterReceiver(4, this);
        gNetworkMessageRegistry->RegisterReceiver(5, this);
        gNetworkMessageRegistry->RegisterReceiver(7, this);
        gNetworkMessageRegistry->RegisterReceiver(10, this);
        gNetworkMessageRegistry->RegisterReceiver(11, this);
        gNetworkMessageRegistry->RegisterReceiver(12, this);
        if (gNetworkMiiName[0] != '\0')
            nlStrNCpy(mLocalPlayerName, gNetworkMiiName, 11);
        if (mLocalPlayerName[0] == '\0')
            GenerateNetworkName(mLocalPlayerName, 11);
        if (!mFindGameEnabled)
        {
            mFindGameEnabled = true;
            mFindGameElapsedTime = g_fBroadCastFindGameTime;
        }
        mInitialized = true;
    }
}

void LANLobby::UnregisterMessageReceivers()
{
    gNetworkMessageRegistry->UnregisterReceiver(2);
    gNetworkMessageRegistry->UnregisterReceiver(3);
    gNetworkMessageRegistry->UnregisterReceiver(4);
    gNetworkMessageRegistry->UnregisterReceiver(5);
    gNetworkMessageRegistry->UnregisterReceiver(7);
    gNetworkMessageRegistry->UnregisterReceiver(10);
    gNetworkMessageRegistry->UnregisterReceiver(11);
    gNetworkMessageRegistry->UnregisterReceiver(12);
    if (mListener != 0)
        mListener->OnLobbyShutdown();
    mFoundGameCount = 0;
    mSocket = 0;
    mInitialized = false;
}

int LANLobby::CreateGame(int value)
{
    u8* address = mSocket->GetLocalAddress();
    if (address == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Failed to Create Game, failed to get local address!\n");
        return 1;
    }
    mIsHost = true;
    mHostState = 0;
    mGameType = value;
    mState = 0;
    mLaunchRequestTicker = 0;
    mLaunchConfirmationPending = false;
    mLocalMachineIndex = 0;
    mPeerCount = 1;
    nlStrNCpy(mPeerInfoList[0].mName, mLocalPlayerName, 11);
    mPeerInfoList[0].mUnidentified0B = mUserMatchDataSize;
    memcpy(&mPeerInfoList[0].mUnidentified0C, mUserMatchData, mUserMatchDataSize);
    *(u32*)mPeerInfoList[0].mUnidentified14 = *(u32*)address;
    mPeerInfoList[0].mUnidentified20 = mSocket->GetLocalPort();
    mPeerInfoList[0].mUnidentified18 = 0;
    mPeerInfoList[0].mUnidentified22 = false;
    mPeerInfoList[0].mUnidentified1C = -1;
    if (mFindGameEnabled)
        mFindGameEnabled = false;
    if (g_bDirectConnectMode)
        mAdvertiseGame = false;
    else
        mAdvertiseGame = true;
    mSocket->SocketVirtual10(true);
    if (mListener != 0)
        mListener->OnGameCreated(0);
    return 0;
}

int LANLobby::AbortCreateGame()
{
    return AbortCreateGame(8);
}

int LANLobby::AbortCreateGame(int result)
{
    if (!mIsHost)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Ignored abort create game..We are not a host\n");
        return 3;
    }
    mIsHost = false;
    mGameType = 0;
    mLaunchConfirmationPending = false;
    mState = 0;
    mLaunchRequestTicker = 0;
    mLocalMachineIndex = -1;
    mPeerCount = 0;
    if (!mFindGameEnabled)
    {
        mFindGameEnabled = true;
        mFindGameElapsedTime = g_fBroadCastFindGameTime;
    }
    mAdvertiseGame = false;
    mSocket->SocketVirtual10(false);
    if (mListener != 0)
        mListener->OnGameCreated(result);
    mHostState = 0;
    for (int index = 0; index < 8; ++index)
    {
        if (m_ConnectionPool[index].m_Connection != 0)
            mSocket->Disconnect(m_ConnectionPool[index].m_Connection, true);
    }
    return 0;
}

int LANLobby::JoinGame(LANGameInfo* game, int value)
{
    if (mJoinState != 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Ignored join current join state %d\n", mJoinState);
        return 2;
    }
    if (mSocket->GetLocalAddress() == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Failed to Join Game, failed to get local address!\n");
        return 1;
    }
    if (game == 0)
    {
        if (g_bDirectConnectMode)
        {
            game = &gDirectConnectGameInfo;
            nlStrNCpy(gDirectConnectGameInfo.mUnidentified00, "Server", 11);
            gDirectConnectGameInfo.mUnidentified14 = 0.0f;
            gDirectConnectGameInfo.mUnidentified10 = 0;
            gDirectConnectGameInfo.mUnidentified0C[0] = g_nConnectToServerAddress[0];
            gDirectConnectGameInfo.mUnidentified0C[1] = g_nConnectToServerAddress[1];
            gDirectConnectGameInfo.mUnidentified0C[2] = g_nConnectToServerAddress[2];
            gDirectConnectGameInfo.mUnidentified0C[3] = g_nConnectToServerAddress[3];
            gDirectConnectGameInfo.mUnidentified18 = g_nConnectToServerPort;
        }
        else
        {
            for (int index = 0; index < mFoundGameCount; ++index)
            {
                if (mFoundGames[index].mUnidentified10 == value)
                {
                    game = &mFoundGames[index];
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
    mLaunchConfirmationPending = false;
    if (mTopology == 0)
        mSocket->SocketVirtual10(true);
    m_ConnectionPool[0].mUnidentified04 = 1;
    if (mSocket->Connect(&m_ConnectionPool[0].m_Connection,
            game->mUnidentified0C,
            game->mUnidentified18))
    {
        tDebugPrintManager::Print(DC_NETWORK, "Attempting connection\n");
    }
    else
    {
        m_ConnectionPool[0].m_Connection = 0;
        m_ConnectionPool[0].mUnidentified04 = 0;
        mSocket->SocketVirtual10(false);
        tDebugPrintManager::Print(DC_NETWORK, "Connection failed at outset\n");
        return 4;
    }
    mJoinState = 1;
    return 0;
}

void LANLobby::OnGameStarted()
{
    mAdvertiseGame = false;
    mSocket->SocketVirtual10(false);
    if (mIsHost)
        mHostState = 2;
    tDebugPrintManager::Print(DC_NETWORK, "LANLobby Game Started\n");
    DumpPeerInfo();
}

void LANLobby::Shutdown(bool)
{
    for (int index = 0; index < 8; ++index)
    {
        if (m_ConnectionPool[index].m_Connection != 0)
            mSocket->Disconnect(m_ConnectionPool[index].m_Connection, true);
    }
    mIsHost = false;
    mHostState = 0;
    if (!mFindGameEnabled)
    {
        mFindGameEnabled = true;
        mFindGameElapsedTime = g_fBroadCastFindGameTime;
    }
    mAdvertiseGame = false;
    mSocket->SocketVirtual10(false);
    mJoinState = 0;
    mLaunchConfirmationPending = false;
    mLocalMachineIndex = -1;
    mPeerCount = 0;
}

void LANLobby::DumpPeerInfo()
{
    tDebugPrintManager::Print(DC_NETWORK, "Dumping %d entries in PeerInfoList\n", mPeerCount);
    for (int index = 0; index < mPeerCount; ++index)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Peer %d address %d.%d.%d.%d port %d hoststate %d connInd %d connConf %d\n", index, mPeerInfoList[index].mUnidentified14[0], mPeerInfoList[index].mUnidentified14[1], mPeerInfoList[index].mUnidentified14[2], mPeerInfoList[index].mUnidentified14[3], mPeerInfoList[index].mUnidentified20, mPeerInfoList[index].mUnidentified18, mPeerInfoList[index].mUnidentified1C, mPeerInfoList[index].mUnidentified22);
    }
    tDebugPrintManager::Print(DC_NETWORK, "Dumping ConnectionPool contents\n");
    for (int index = 0; index < 8; ++index)
    {
        TransportConnection* connection = m_ConnectionPool[index].m_Connection;
        if (connection == 0)
            tDebugPrintManager::Print(DC_NETWORK, "ConnPool %d Status %d\n", index, m_ConnectionPool[index].mUnidentified04);
        else
            tDebugPrintManager::Print(DC_NETWORK, "ConnPool %d Status %d ConnAddr %d.%d.%d.%d\n", index, m_ConnectionPool[index].mUnidentified04, connection->mAddress[0], connection->mAddress[1], connection->mAddress[2], connection->mAddress[3]);
    }
}

unsigned int LANLobby::GetMachineAid(int index)
{
    if (mPeerCount == 0)
        return 0;
    if (index < 0)
        return 0;
    if (mPeerCount <= index)
        return 0;
    if (index == mLocalMachineIndex)
        return (u32)-1;
    int connectionIndex = mPeerInfoList[index].mUnidentified1C;
    if (connectionIndex != -1 && m_ConnectionPool[connectionIndex].mUnidentified04 == 2)
        return (u32)m_ConnectionPool[connectionIndex].m_Connection;
    return 0;
}

int LANLobby::MachineIdxFromConnection(unsigned int connection)
{
    if (mPeerCount == 0)
        return -1;
    if (connection == 0 || connection == (u32)-2)
        return -1;
    if (connection == (u32)-1)
        return mLocalMachineIndex;
    TransportConnection* entry = (TransportConnection*)connection;
    int connectionIndex = GetConnectionIndex(entry);
    if (connectionIndex == -1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Failed to get connection index from conn addr %d.%d.%d.%d\n", entry->mAddress[0], entry->mAddress[1], entry->mAddress[2], entry->mAddress[3]);
        return -1;
    }
    for (int index = 0; index < mPeerCount; ++index)
    {
        if (mPeerInfoList[index].mUnidentified1C == connectionIndex)
            return index;
    }
    tDebugPrintManager::Print(DC_NETWORK, "Failed to get peer index from connectionIndex %d conn addr %d.%d.%d.%d\n", connectionIndex, entry->mAddress[0], entry->mAddress[1], entry->mAddress[2], entry->mAddress[3]);
    DumpPeerInfo();
    return -1;
}

void LANLobby::SetLobbyListener(LANLobbyListener* listener)
{
    mListener = listener;
}

void LANLobby::SetPlayerListener(LANLobbyPlayerListener* listener)
{
    mRosterListener = listener;
}

void LANLobby::OnConnected(unsigned int connection, int result)
{
    TransportConnection* entry = (TransportConnection*)connection;
    if (!mIsHost)
    {
        if (mJoinState == 1)
        {
            if (result == 0)
            {
                NetMessageJoinRequest message;
                *(u32*)message.mUnidentified08 = *(u32*)mSocket->GetLocalAddress();
                message.mUnidentified0C = mSocket->GetLocalPort();
                nlStrNCpy(message.mUnidentified0E, mLocalPlayerName, 11);
                message.mUnidentified19 = mUserMatchDataSize;
                memcpy(message.mUnidentified1A, mUserMatchData, mUserMatchDataSize);
                u8 buffer[200];
                int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
                int index = GetConnectionIndex(entry);
                if (index == -1)
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Connection established %x, but cannot find connection in pool\n", connection);
                    return;
                }
                m_ConnectionPool[index].mUnidentified04 = 2;
                mSocket->Send(connection, buffer, size, true);
                mJoinState = 2;
                tDebugPrintManager::Print(DC_NETWORK, "Sent Join Request to machine %d\n", index);
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "Join failed because ConnectionEstablished returned error %d\n", result);
                mJoinState = 0;
                mLaunchConfirmationPending = false;
                int index = GetConnectionIndex(entry);
                if (index != -1)
                {
                    m_ConnectionPool[index].m_Connection = 0;
                    m_ConnectionPool[index].mUnidentified04 = 0;
                }
                if (mListener != 0)
                    mListener->OnGameJoined(4);
            }
        }
        else if (mTopology == 0)
        {
            if (result == 0)
            {
                int index = GetConnectionIndex(entry);
                if (index == -1)
                    tDebugPrintManager::Print(DC_NETWORK, "Connection established %x, but cannot find connection in pool\n", connection);
                else
                    m_ConnectionPool[index].mUnidentified04 = 2;
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "ConnectionEstablished returned error %d.  Peer to peer connection failed.\n", result);
                int index = GetConnectionIndex(entry);
                if (index != -1)
                {
                    m_ConnectionPool[index].m_Connection = 0;
                    m_ConnectionPool[index].mUnidentified04 = 0;
                }
            }
        }
        else
            tDebugPrintManager::Print(DC_NETWORK, "Ignoring connection established notification, not in peer peer topology, client state = %d ConnResult = %d\n", mJoinState, result);
    }
    else
        tDebugPrintManager::Print(DC_NETWORK, "Ignoring connection established notification, host state = %d ConnResult = %d\n", mHostState, result);
}

int LANLobby::ShouldAcceptConnection(unsigned int connection, u8* address)
{
    int index = GetFreeConnectionIndex();
    if (index == -1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Rejected connection attempt from %d.%d.%d.%d because no free connection in pool\n", address[0], address[1], address[2], address[3]);
        return 0;
    }
    if (mIsHost)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Connection attempt accepted by host.  Address %d.%d.%d.%d assigned to connection pool %d\n", address[0], address[1], address[2], address[3], index);
        m_ConnectionPool[index].m_Connection = (TransportConnection*)connection;
        m_ConnectionPool[index].mUnidentified04 = 2;
        return 1;
    }
    if (mTopology == 0)
    {
        int count = mPeerCount;
        for (int peer = 1; peer < count; ++peer)
        {
            if (memcmp(mPeerInfoList[peer].mUnidentified14, address, 4) == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Connection attempt accepted by client.  Address %d.%d.%d.%d assigned to connection pool %d\n", address[0], address[1], address[2], address[3], index);
                m_ConnectionPool[index].m_Connection = (TransportConnection*)connection;
                m_ConnectionPool[index].mUnidentified04 = 2;
                mPeerInfoList[peer].mUnidentified1C = index;
                return 1;
            }
        }
        tDebugPrintManager::Print(DC_NETWORK, "Rejected client-client connection attempt from %d.%d.%d.%d because from unknown client\n", address[0], address[1], address[2], address[3]);
        DumpPeerInfo();
        return 0;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Rejected connection attempt from %d.%d.%d.%d because I am not a host and am not in peer-peer topology\n", address[0], address[1], address[2], address[3]);
    return 0;
}

void LANLobby::OnConnectionClosed(unsigned int connection, int)
{
    bool foundConnection = false;
    bool foundPeer = false;
    for (int index = 0; index < 8; ++index)
    {
        if (m_ConnectionPool[index].m_Connection != 0
            && (u32)m_ConnectionPool[index].m_Connection == connection)
        {
            for (int peer = 0; peer < mPeerCount; ++peer)
            {
                if (mPeerInfoList[peer].mUnidentified1C == index)
                {
                    tDebugPrintManager::Print(DC_NETWORK, "I peer %d (%s) lost connection to peer %d\n", mLocalMachineIndex, mIsHost ? "host" : "client", peer);
                    mPeerInfoList[peer].mUnidentified1C = -1;
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
    else if (mIsHost)
    {
        if (mListener != 0)
            mListener->OnGameCreated(7);
    }
    else if (mListener != 0)
        mListener->OnGameJoined(7);
}

void LANLobby::SendFoundGame(const void* data)
{
    NetMessageFoundGame message;
    u8* address = mSocket->GetLocalAddress();
    if (address == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Not sending found game message because have no local address!\n");
        return;
    }
    u16 port = mSocket->GetLocalPort();
    memcpy(message.mUnidentified08, data, 8);
    message.mUnidentified10 = mGameType;
    memcpy(message.mUnidentified14, address, 4);
    message.mUnidentified18 = port;
    nlStrNCpy(message.mUnidentified1A, mLocalPlayerName, 11);
    u8 buffer[250];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    mSocket->SendBroadcast(buffer, size);
}

void LANLobby::Update(float dt)
{
    int index = 0;
    while (index < mFoundGameCount)
    {
        mFoundGames[index].mUnidentified14 += dt;
        if (mFoundGames[index].mUnidentified14 >= g_fLANGameExpireTime)
        {
            if (mListener != 0)
                mListener->OnGameExpired(&mFoundGames[index]);
            int count = mFoundGameCount - (index + 1);
            if (count >= 1)
                memmove(&mFoundGames[index], &mFoundGames[index + 1], count * sizeof(*mFoundGames));
            --mFoundGameCount;
        }
        else
            ++index;
    }
    if (mFindGameEnabled)
    {
        mFindGameElapsedTime += dt;
        if (mFindGameElapsedTime >= g_fBroadCastFindGameTime)
        {
            NetMessageFindGame message;
            u32 first = NetworkRandom();
            u32 second = NetworkRandom();
            message.mUnidentified08[1] = second;
            message.mUnidentified08[0] = first;
            memcpy(mFindGameToken, message.mUnidentified08, 8);
            u8 buffer[200];
            int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
            mSocket->SendBroadcast(buffer, size);
            mFindGameElapsedTime = 0.0f;
        }
    }
    if (mState == 1)
    {
        bool ready = CheckPeerStates();
        if (ready)
        {
            if (GetTopology() == 0)
            {
                mState = 2;
                SendReadyToLaunchRequest();
            }
            else
                mState = 3;
        }
    }
    if (mState == 2)
    {
        bool ready = true;
        for (int peer = 1; peer < mPeerCount; ++peer)
        {
            if (!mPeerInfoList[peer].mUnidentified22)
                ready = false;
        }
        if (!ready)
        {
            float elapsed = nlGetTickerDifference(mLaunchRequestTicker, nlGetTicker());
            if (elapsed > g_fLANConfirmConnectionTimeout)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Confirm connections timed out after %f ms\n", elapsed);
                if (mListener != 0)
                    mListener->OnGameLaunched(9);
                mState = 0;
            }
        }
        else
            mState = 3;
    }
    if (mLaunchConfirmationPending && ArePeerConnectionsReady())
    {
        NetMessageReadyToLaunchConfirm message;
        message.mUnidentified08 = 1;
        u8 buffer[8];
        int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
        int index = mPeerInfoList[0].mUnidentified1C;
        if (index != -1)
        {
            if (m_ConnectionPool[index].m_Connection != 0)
            {
                mSocket->Send((u32)m_ConnectionPool[index].m_Connection, buffer, size, true);
                tDebugPrintManager::Print(DC_NETWORK, "Sent ready to launch confirm\n");
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, m_ConnectionPool[connectionIndex].m_Connection is NULL\n");
        }
        else
            tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, no connection to host!\n");
        mLaunchConfirmationPending = false;
    }
}

int LANLobby::GetPlayerCount()
{
    if (mIsHost)
        return mPeerCount;
    return 0;
}

int LANLobby::StartGame()
{
    if (mIsHost)
    {
        if (mPeerCount >= 2 && mState == 0)
        {
            mState = 1;
            return 0;
        }
        return 2;
    }
    return 3;
}

void LANLobby::CompleteLaunch()
{
    mState = 4;
    if (mListener != 0)
        mListener->OnGameLaunched(0);
}

void LANLobby::SendReadyToLaunchRequest()
{
    mLaunchRequestTicker = nlGetTicker();
    NetMessageReadyToLaunchRequest message;
    u8 buffer[8];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    for (int peer = 1; peer < mPeerCount; ++peer)
    {
        int index = mPeerInfoList[peer].mUnidentified1C;
        if (index >= 0 && index < 8)
        {
            if (m_ConnectionPool[index].mUnidentified04 != 0)
            {
                mSocket->Send((u32)m_ConnectionPool[index].m_Connection, buffer, size, true);
                tDebugPrintManager::Print(DC_NETWORK, "Sent ready to launch request to peer %d\n", peer);
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "Failed to send launch request to peer %d connIndx %d is not in use\n", peer, index);
                DumpPeerInfo();
            }
        }
        else
            tDebugPrintManager::Print(DC_NETWORK, "Failed to send launch request to peer %d connectionIndex == %d\n", peer, index);
    }
}

void LANLobby::EnumerateGames()
{
    for (int index = 0; index < mFoundGameCount; ++index)
    {
        if (mListener != 0)
            mListener->OnGameFound(&mFoundGames[index]);
    }
}

void LANLobby::ProcessFoundGame(NetMessageFoundGame* message)
{
    if (mFoundGameCount >= 10)
        return;
    for (int index = 0; index < mFoundGameCount; ++index)
    {
        if (nlStrCmp(mFoundGames[index].mUnidentified00, message->mUnidentified1A) == 0)
        {
            mFoundGames[index].mUnidentified14 = 0.0f;
            return;
        }
    }
    mFoundGames[mFoundGameCount].mUnidentified10 = message->mUnidentified10;
    memcpy(mFoundGames[mFoundGameCount].mUnidentified0C, message->mUnidentified14, 4);
    mFoundGames[mFoundGameCount].mUnidentified18 = message->mUnidentified18;
    nlStrNCpy(mFoundGames[mFoundGameCount].mUnidentified00, message->mUnidentified1A, 11);
    mFoundGames[mFoundGameCount].mUnidentified14 = 0.0f;
    if (mListener != 0)
        mListener->OnGameFound(&mFoundGames[mFoundGameCount]);
    ++mFoundGameCount;
}

void LANLobby::SendGamePeerAdded(int index)
{
    NetMessageGamePeerAdded message;
    *(u32*)message.mUnidentified08.mUnidentified00 = *(u32*)mPeerInfoList[index].mUnidentified14;
    message.mUnidentified08.mUnidentified04 = mPeerInfoList[index].mUnidentified20;
    nlStrNCpy(message.mUnidentified08.mUnidentified06, mPeerInfoList[index].mName, 11);
    message.mUnidentified08.mUnidentified12 = mPeerInfoList[index].mUnidentified0B;
    memcpy(message.mUnidentified08.mUnidentified13, &mPeerInfoList[index].mUnidentified0C, mPeerInfoList[index].mUnidentified0B);
    message.mUnidentified08.mUnidentified11 = index;
    u8 buffer[250];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    for (int peer = 1; peer < index; ++peer)
    {
        mSocket->Send((u32)m_ConnectionPool[mPeerInfoList[peer].mUnidentified1C].m_Connection,
            buffer,
            size,
            true);
    }
}

void LANLobby::SendJoinResponse(TransportConnection* connection, bool accepted)
{
    NetMessageJoinResponse message;
    *(u32*)message.mUnidentified08 = *(u32*)mSocket->GetLocalAddress();
    message.mUnidentified0C = mSocket->GetLocalPort();
    nlStrNCpy(message.mUnidentified0F, mLocalPlayerName, 11);
    message.mUnidentified1A = mUserMatchDataSize;
    memcpy(message.mUnidentified1B, mUserMatchData, mUserMatchDataSize);
    message.mUnidentified0E = accepted;
    message.mUnidentified23 = 0;
    if (mPeerCount > 2)
    {
        for (int peer = 1; peer < mPeerCount - 1; ++peer)
        {
            message.mUnidentified24[peer - 1].mAddressWord = mPeerInfoList[peer].mAddressWord;
            message.mUnidentified24[peer - 1].mUnidentified04 = mPeerInfoList[peer].mUnidentified20;
            nlStrNCpy(message.mUnidentified24[peer - 1].mUnidentified06, mPeerInfoList[peer].mName, 11);
            message.mUnidentified24[peer - 1].mUnidentified12 = mPeerInfoList[peer].mUnidentified0B;
            memcpy(message.mUnidentified24[peer - 1].mUnidentified13, &mPeerInfoList[peer].mUnidentified0C, mPeerInfoList[peer].mUnidentified0B);
            message.mUnidentified24[peer - 1].mUnidentified11 = peer;
            ++message.mUnidentified23;
        }
    }
    u8 buffer[250];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    mSocket->Send((u32)connection, buffer, size, true);
}

void LANLobby::ProcessJoinRequest(int index, NetMessageJoinRequest* message)
{
    TransportConnection* connection = m_ConnectionPool[index].m_Connection;
    if (mPeerCount < mMaxMachineCount && mState == 0)
    {
        nlStrNCpy(mPeerInfoList[mPeerCount].mName, message->mUnidentified0E, 11);
        mPeerInfoList[mPeerCount].mUnidentified0B = message->mUnidentified19;
        memcpy(&mPeerInfoList[mPeerCount].mUnidentified0C, message->mUnidentified1A, message->mUnidentified19);
        *(u32*)mPeerInfoList[mPeerCount].mUnidentified14 = *(u32*)connection->mAddress;
        mPeerInfoList[mPeerCount].mUnidentified20 = connection->mPort;
        if (GetTopology() == 0)
            mPeerInfoList[mPeerCount].mUnidentified18 = 2;
        else
            mPeerInfoList[mPeerCount].mUnidentified18 = 3;
        mPeerInfoList[mPeerCount].mUnidentified22 = false;
        mPeerInfoList[mPeerCount].mUnidentified1C = index;
        ++mPeerCount;
        SendJoinResponse(connection, true);
        if (mPeerCount > 2 && GetTopology() != 0)
            SendGamePeerAdded(mPeerCount - 1);
        if (mRosterListener != 0)
            mRosterListener->OnPlayerListChanged();
        tDebugPrintManager::Print(DC_NETWORK, "Approved a join request\n");
        DumpPeerInfo();
    }
    else
    {
        SendJoinResponse(connection, false);
        tDebugPrintManager::Print(DC_NETWORK, "Rejected a join request\n");
    }
}

void LANLobby::ProcessJoinResponse(int index, NetMessageJoinResponse* message)
{
    if (mJoinState != 2)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Ignoring join response because in join state %d\n", mJoinState);
        return;
    }
    if (message->mUnidentified0E)
    {
        nlStrNCpy(mPeerInfoList[0].mName, message->mUnidentified0F, 11);
        mPeerInfoList[0].mUnidentified0B = message->mUnidentified1A;
        memcpy(&mPeerInfoList[0].mUnidentified0C, message->mUnidentified1B, message->mUnidentified1A);
        *(u32*)mPeerInfoList[0].mUnidentified14 = *(u32*)message->mUnidentified08;
        mPeerInfoList[0].mUnidentified20 = message->mUnidentified0C;
        mPeerInfoList[0].mUnidentified1C = index;
        mPeerInfoList[0].mUnidentified18 = 0;
        mPeerInfoList[0].mUnidentified22 = false;
        int peer = 1;
        for (int entry = 0; entry < message->mUnidentified23; ++entry, ++peer)
        {
            LANPeerMessageInfo& info = message->mUnidentified24[entry];
            nlStrNCpy(mPeerInfoList[peer].mName, info.mUnidentified06, 11);
            mPeerInfoList[peer].mUnidentified0B = info.mUnidentified12;
            memcpy(&mPeerInfoList[peer].mUnidentified0C, info.mUnidentified13, info.mUnidentified12);
            *(u32*)mPeerInfoList[peer].mUnidentified14 = *(u32*)info.mUnidentified00;
            mPeerInfoList[peer].mUnidentified20 = info.mUnidentified04;
            mPeerInfoList[peer].mUnidentified1C = -1;
            mPeerInfoList[peer].mUnidentified18 = 0;
            mPeerInfoList[peer].mUnidentified22 = false;
        }
        nlStrNCpy(mPeerInfoList[peer].mName, mLocalPlayerName, 11);
        mPeerInfoList[peer].mUnidentified0B = mUserMatchDataSize;
        memcpy(&mPeerInfoList[peer].mUnidentified0C, mUserMatchData, mUserMatchDataSize);
        *(u32*)mPeerInfoList[peer].mUnidentified14 = *(u32*)mSocket->GetLocalAddress();
        mPeerInfoList[peer].mUnidentified20 = mSocket->GetLocalPort();
        mPeerInfoList[peer].mUnidentified1C = -1;
        mPeerInfoList[peer].mUnidentified18 = 0;
        mPeerInfoList[peer].mUnidentified22 = false;
        mLocalMachineIndex = peer;
        mPeerCount = peer + 1;
        if (mRosterListener != 0)
            mRosterListener->OnPlayerListChanged();
        mJoinState = 3;
        if (mListener != 0)
            mListener->OnGameJoined(0);
        tDebugPrintManager::Print(DC_NETWORK, "Successfully joined game.\n");
        DumpPeerInfo();
        if (GetTopology() == 0)
        {
            NetMessageClientConfirmedJoin response(mLocalMachineIndex);
            u8 buffer[8];
            int size = gNetworkMessageRegistry->Serialize(&response, buffer, sizeof(buffer));
            u32 connection = GetMachineAid(0);
            if (connection == 0)
                tDebugPrintManager::Print(DC_NETWORK, "Could not send client confirmed join no connection to host\n");
            else
                mSocket->Send(connection, buffer, size, true);
        }
    }
    else
    {
        mSocket->Disconnect(m_ConnectionPool[index].m_Connection, true);
        mJoinState = 0;
        mLaunchConfirmationPending = false;
        mPeerCount = 0;
        mLocalMachineIndex = -1;
        if (mListener != 0)
            mListener->OnGameJoined(6);
        tDebugPrintManager::Print(DC_NETWORK, "Join was refused.\n");
    }
}

void LANLobby::ProcessGamePeerAdded(NetMessageGamePeerAdded* message)
{
    s8 peer = message->mUnidentified08.mUnidentified11;
    *(u32*)mPeerInfoList[peer].mUnidentified14 = *(u32*)message->mUnidentified08.mUnidentified00;
    mPeerInfoList[peer].mUnidentified20 = message->mUnidentified08.mUnidentified04;
    nlStrNCpy(mPeerInfoList[peer].mName, message->mUnidentified08.mUnidentified06, 11);
    mPeerInfoList[peer].mUnidentified0B = message->mUnidentified08.mUnidentified12;
    memcpy(&mPeerInfoList[peer].mUnidentified0C, message->mUnidentified08.mUnidentified13, message->mUnidentified08.mUnidentified12);
    mPeerInfoList[peer].mUnidentified18 = 0;
    mPeerInfoList[peer].mUnidentified22 = false;
    mPeerInfoList[peer].mUnidentified1C = -1;
    ++mPeerCount;
    tDebugPrintManager::Print(DC_NETWORK, "ProcessGamePeerAdded peer %d added\n", peer);
    DumpPeerInfo();
    if (mTopology == 0)
    {
        int index = GetFreeConnectionIndex();
        if (index == -1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Unable to connect to just added peer, no pool connection space\n");
            return;
        }
        mPeerInfoList[peer].mUnidentified1C = index;
        m_ConnectionPool[index].mUnidentified04 = 1;
        if (mSocket->Connect(&m_ConnectionPool[index].m_Connection,
                message->mUnidentified08.mUnidentified00,
                message->mUnidentified08.mUnidentified04))
            tDebugPrintManager::Print(DC_NETWORK, "Attempting peer-peer connection to other client\n");
        else
        {
            m_ConnectionPool[index].m_Connection = 0;
            m_ConnectionPool[index].mUnidentified04 = 0;
            mPeerInfoList[peer].mUnidentified1C = -1;
            tDebugPrintManager::Print(DC_NETWORK, "Connection failed to other client at outset\n");
        }
    }
}

int LANLobby::ProcessMessage(NetworkMessage* message)
{
    switch ((u8)message->GetType())
    {
    case 2:
        if (mIsHost && mAdvertiseGame)
            SendFoundGame(static_cast<NetMessageFindGame*>(message)->mUnidentified08);
        break;
    case 3:
        if (!mIsHost)
        {
            NetMessageFoundGame* reply = static_cast<NetMessageFoundGame*>(message);
            if (memcmp(reply->mUnidentified08, mFindGameToken, 8) == 0)
                ProcessFoundGame(reply);
        }
        break;
    case 4:
        if (mIsHost)
        {
            int index = GetConnectionIndex((TransportConnection*)message->mSource);
            if (index >= 0 && index < 8)
                ProcessJoinRequest(index, static_cast<NetMessageJoinRequest*>(message));
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored join request because did not find connection in pool\n");
        }
        break;
    case 5:
        if (!mIsHost)
        {
            int index = GetConnectionIndex((TransportConnection*)message->mSource);
            if (index >= 0 && index < 8)
                ProcessJoinResponse(index, static_cast<NetMessageJoinResponse*>(message));
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored join response because did not find connection in pool\n");
        }
        break;
    case 7:
        if (!mIsHost)
        {
            int index = GetConnectionIndex((TransportConnection*)message->mSource);
            if (index >= 0 && index < 8)
                ProcessGamePeerAdded(static_cast<NetMessageGamePeerAdded*>(message));
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored game peer added because did not find connection in pool\n");
        }
        break;
    case 10:
        if (!mIsHost)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Received ready to launch request\n");
            if (ArePeerConnectionsReady())
            {
                NetMessageReadyToLaunchConfirm response;
                response.mUnidentified08 = 1;
                u8 buffer[8];
                int size = gNetworkMessageRegistry->Serialize(&response, buffer, sizeof(buffer));
                int index = mPeerInfoList[0].mUnidentified1C;
                if (index != -1)
                {
                    if (m_ConnectionPool[index].m_Connection != 0)
                    {
                        mSocket->Send((u32)m_ConnectionPool[index].m_Connection, buffer, size, true);
                        tDebugPrintManager::Print(DC_NETWORK, "Sent ready to launch confirm\n");
                    }
                    else
                        tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, m_ConnectionPool[connectionIndex].m_Connection is NULL\n");
                }
                else
                    tDebugPrintManager::Print(DC_NETWORK, "Failed to send ready to launch confirm, no connection to host!\n");
                mLaunchConfirmationPending = false;
            }
            else
                mLaunchConfirmationPending = true;
        }
        break;
    case 11:
        if (mIsHost)
        {
            int peer = MachineIdxFromConnection(message->mSource);
            if (peer > 0 && peer < mPeerCount)
            {
                mPeerInfoList[peer].mUnidentified22 = static_cast<NetMessageReadyToLaunchConfirm*>(message)->mUnidentified08;
                tDebugPrintManager::Print(DC_NETWORK, "Received ready to launch confirm from peer %d\n", peer);
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored Ready To Launch Confirm because did not find peer it's from\n");
        }
        break;
    case 12:
        if (mIsHost && GetTopology() == 0)
        {
            int peer = MachineIdxFromConnection(message->mSource);
            if (peer > 0 && peer < mPeerCount)
            {
                NetMessageClientConfirmedJoin* reply = static_cast<NetMessageClientConfirmedJoin*>(message);
                mPeerInfoList[reply->mUnidentified08].mUnidentified18 = 3;
                if (reply->mUnidentified08 > 1)
                    SendGamePeerAdded(reply->mUnidentified08);
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored ClientConfirmedJoin because did not find peer it's from\n");
        }
        break;
    }
    return 1;
}

bool LANLobby::ArePeerConnectionsReady()
{
    if (GetTopology() == 0)
    {
        bool ready = true;
        for (int peer = 1; peer < mPeerCount; ++peer)
        {
            if (peer != mLocalMachineIndex)
            {
                int index = mPeerInfoList[peer].mUnidentified1C;
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

int LANLobby::GetTopology()
{
    return mTopology;
}

void LANLobby::SetTopology(int value)
{
    mTopology = value;
}

int LANLobby::GetMaxMachineCount()
{
    return mMaxMachineCount;
}

void LANLobby::SetMaxMachineCount(int value)
{
    mMaxMachineCount = value;
}

int LANLobby::GetMachineCount()
{
    return mPeerCount;
}

TransportPlayerInfo* LANLobby::GetPlayerInfo(int index)
{
    if (index >= 0 && index < mPeerCount)
        return &mPeerInfoList[index];
    return 0;
}

int LANLobby::GetLocalMachineIndex()
{
    if (mPeerCount <= 0)
        return -1;
    return mLocalMachineIndex;
}

TransportPlayerInfo* LANLobby::GetLocalPlayerInfo()
{
    int index = GetLocalMachineIndex();
    if (index == -1)
        return 0;
    return &mPeerInfoList[index];
}

void LANLobby::SetUserMatchData(u8 size, const void* data)
{
    mUserMatchDataSize = size;
    memcpy(mUserMatchData, data, size);
}

void* LANLobby::GetUserMatchData(u8* size)
{
    *size = mUserMatchDataSize;
    return mUserMatchData;
}

void LANLobby::DebugDraw(int column, int* row)
{
    glFontPrintf(GetDebugFontView(), column, (*row)++, "Name: %s", mLocalPlayerName);
    if (g_bDisplayNetworkVerbose)
    {
        if (mIsHost)
            glFontPrintf(GetDebugFontView(), column, (*row)++, "Host st: %d", mHostState);
        else
            glFontPrintf(GetDebugFontView(), column, (*row)++, "Client st: %d", mJoinState);
        if (mFoundGameCount > 0)
        {
            glFontPrintf(GetDebugFontView(), column, (*row)++, "Num Games:%d", mFoundGameCount);
            for (int index = 0; index < mFoundGameCount; ++index)
                glFontPrintf(GetDebugFontView(), column, (*row)++, mFoundGames[index].mUnidentified00);
        }
        if (mPeerCount > 0)
        {
            glFontPrintf(GetDebugFontView(), column, (*row)++, "InGame NumPeers:%d", mPeerCount);
            for (int index = 0; index < mPeerCount; ++index)
            {
                if (mIsHost)
                    glFontPrintf(GetDebugFontView(), column, (*row)++, "%d: %d %s", index, mPeerInfoList[index].mUnidentified18, mPeerInfoList[index].mName);
                else
                    glFontPrintf(GetDebugFontView(), column, (*row)++, "%d: %s", index, mPeerInfoList[index].mName);
            }
        }
    }
}

static TweakFloatBinding sBroadCastFindGameTimeTweak("g_fBroadCastFindGameTime", "Network/LANLobby", &g_fBroadCastFindGameTime, true);
static TweakFloatBinding sLANGameExpireTimeTweak("g_fLANGameExpireTime", "Network/LANLobby", &g_fLANGameExpireTime, true);
static TweakFloatBinding sLANConfirmConnectionTimeoutTweak("g_fLANConfirmConnectionTimeout", "Network/LANLobby", &g_fLANConfirmConnectionTimeout, true);
LANGameInfo gDirectConnectGameInfo;
