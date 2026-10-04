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
    mPlayerListener = 0;
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
        m_ConnectionPool[index].mStatus = 0;
    }
    mHostState = 0;
    mJoinState = 0;
    mLaunchState = 0;
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

int LANLobby::CreateGame(int gameType)
{
    u8* address = mSocket->GetLocalAddress();
    if (address == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Failed to Create Game, failed to get local address!\n");
        return 1;
    }
    mIsHost = true;
    mHostState = 0;
    mGameType = gameType;
    mLaunchState = 0;
    mLaunchRequestTicker = 0;
    mLaunchConfirmationPending = false;
    mLocalMachineIndex = 0;
    mPeerCount = 1;
    nlStrNCpy(mPeerInfoList[0].mName, mLocalPlayerName, 11);
    mPeerInfoList[0].mUserMatchDataSize = mUserMatchDataSize;
    memcpy(&mPeerInfoList[0].mDisplayRank, mUserMatchData, mUserMatchDataSize);
    mPeerInfoList[0].mAddress.word = *(u32*)address;
    mPeerInfoList[0].mPort = mSocket->GetLocalPort();
    mPeerInfoList[0].mHostState = 0;
    mPeerInfoList[0].mConnectionConfirmed = false;
    mPeerInfoList[0].mConnectionIndex = -1;
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
    mLaunchState = 0;
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

int LANLobby::JoinGame(LANGameInfo* game, int gameType)
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
            nlStrNCpy(gDirectConnectGameInfo.mHostName, "Server", 11);
            gDirectConnectGameInfo.mTimeSinceSeen = 0.0f;
            gDirectConnectGameInfo.mGameType = 0;
            gDirectConnectGameInfo.mAddress[0] = g_nConnectToServerAddress[0];
            gDirectConnectGameInfo.mAddress[1] = g_nConnectToServerAddress[1];
            gDirectConnectGameInfo.mAddress[2] = g_nConnectToServerAddress[2];
            gDirectConnectGameInfo.mAddress[3] = g_nConnectToServerAddress[3];
            gDirectConnectGameInfo.mPort = g_nConnectToServerPort;
        }
        else
        {
            for (int index = 0; index < mFoundGameCount; ++index)
            {
                if (mFoundGames[index].mGameType == gameType)
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
    m_ConnectionPool[0].mStatus = 1;
    if (mSocket->Connect(&m_ConnectionPool[0].m_Connection,
            game->mAddress,
            game->mPort))
    {
        tDebugPrintManager::Print(DC_NETWORK, "Attempting connection\n");
    }
    else
    {
        m_ConnectionPool[0].m_Connection = 0;
        m_ConnectionPool[0].mStatus = 0;
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
        tDebugPrintManager::Print(DC_NETWORK, "Peer %d address %d.%d.%d.%d port %d hoststate %d connInd %d connConf %d\n", index, mPeerInfoList[index].mAddress.bytes[0], mPeerInfoList[index].mAddress.bytes[1], mPeerInfoList[index].mAddress.bytes[2], mPeerInfoList[index].mAddress.bytes[3], mPeerInfoList[index].mPort, mPeerInfoList[index].mHostState, mPeerInfoList[index].mConnectionIndex, mPeerInfoList[index].mConnectionConfirmed);
    }
    tDebugPrintManager::Print(DC_NETWORK, "Dumping ConnectionPool contents\n");
    for (int index = 0; index < 8; ++index)
    {
        TransportConnection* connection = m_ConnectionPool[index].m_Connection;
        if (connection == 0)
            tDebugPrintManager::Print(DC_NETWORK, "ConnPool %d Status %d\n", index, m_ConnectionPool[index].mStatus);
        else
            tDebugPrintManager::Print(DC_NETWORK, "ConnPool %d Status %d ConnAddr %d.%d.%d.%d\n", index, m_ConnectionPool[index].mStatus, connection->mAddress.bytes[0], connection->mAddress.bytes[1], connection->mAddress.bytes[2], connection->mAddress.bytes[3]);
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
    int connectionIndex = mPeerInfoList[index].mConnectionIndex;
    if (connectionIndex != -1 && m_ConnectionPool[connectionIndex].mStatus == 2)
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
        tDebugPrintManager::Print(DC_NETWORK, "Failed to get connection index from conn addr %d.%d.%d.%d\n", entry->mAddress.bytes[0], entry->mAddress.bytes[1], entry->mAddress.bytes[2], entry->mAddress.bytes[3]);
        return -1;
    }
    for (int index = 0; index < mPeerCount; ++index)
    {
        if (mPeerInfoList[index].mConnectionIndex == connectionIndex)
            return index;
    }
    tDebugPrintManager::Print(DC_NETWORK, "Failed to get peer index from connectionIndex %d conn addr %d.%d.%d.%d\n", connectionIndex, entry->mAddress.bytes[0], entry->mAddress.bytes[1], entry->mAddress.bytes[2], entry->mAddress.bytes[3]);
    DumpPeerInfo();
    return -1;
}

void LANLobby::SetLobbyListener(LANLobbyListener* listener)
{
    mListener = listener;
}

void LANLobby::SetPlayerListener(LANLobbyPlayerListener* listener)
{
    mPlayerListener = listener;
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
                *(u32*)message.mAddress = *(u32*)mSocket->GetLocalAddress();
                message.mPort = mSocket->GetLocalPort();
                nlStrNCpy(message.mName, mLocalPlayerName, 11);
                message.mUserMatchDataSize = mUserMatchDataSize;
                memcpy(message.mUserMatchData, mUserMatchData, mUserMatchDataSize);
                u8 buffer[200];
                int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
                int index = GetConnectionIndex(entry);
                if (index == -1)
                {
                    tDebugPrintManager::Print(DC_NETWORK, "Connection established %x, but cannot find connection in pool\n", connection);
                    return;
                }
                m_ConnectionPool[index].mStatus = 2;
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
                    m_ConnectionPool[index].mStatus = 0;
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
                    m_ConnectionPool[index].mStatus = 2;
            }
            else
            {
                tDebugPrintManager::Print(DC_NETWORK, "ConnectionEstablished returned error %d.  Peer to peer connection failed.\n", result);
                int index = GetConnectionIndex(entry);
                if (index != -1)
                {
                    m_ConnectionPool[index].m_Connection = 0;
                    m_ConnectionPool[index].mStatus = 0;
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
        m_ConnectionPool[index].mStatus = 2;
        return 1;
    }
    if (mTopology == 0)
    {
        int count = mPeerCount;
        for (int peer = 1; peer < count; ++peer)
        {
            if (memcmp(mPeerInfoList[peer].mAddress.bytes, address, 4) == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Connection attempt accepted by client.  Address %d.%d.%d.%d assigned to connection pool %d\n", address[0], address[1], address[2], address[3], index);
                m_ConnectionPool[index].m_Connection = (TransportConnection*)connection;
                m_ConnectionPool[index].mStatus = 2;
                mPeerInfoList[peer].mConnectionIndex = index;
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
                if (mPeerInfoList[peer].mConnectionIndex == index)
                {
                    tDebugPrintManager::Print(DC_NETWORK, "I peer %d (%s) lost connection to peer %d\n", mLocalMachineIndex, mIsHost ? "host" : "client", peer);
                    mPeerInfoList[peer].mConnectionIndex = -1;
                    foundPeer = true;
                }
            }
            if (!foundPeer)
                tDebugPrintManager::Print(DC_NETWORK, "Lost connection. Failed to find which peer was using connection pool %d\n", index);
            m_ConnectionPool[index].m_Connection = 0;
            m_ConnectionPool[index].mStatus = 0;
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

void LANLobby::SendFindGame()
{
    NetMessageFindGame message;
    u32 first = NetworkRandom();
    u32 second = NetworkRandom();
    message.mToken[1] = second;
    message.mToken[0] = first;
    memcpy(mFindGameToken, message.mToken, 8);
    u8 buffer[200];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    mSocket->SendBroadcast(buffer, size);
    mFindGameElapsedTime = 0.0f;
}

void LANLobby::SendFoundGame(const void* token)
{
    NetMessageFoundGame message;
    u8* address = mSocket->GetLocalAddress();
    if (address == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "LANLobby: Not sending found game message because have no local address!\n");
        return;
    }
    u16 port = mSocket->GetLocalPort();
    memcpy(message.mToken, token, 8);
    message.mGameType = mGameType;
    memcpy(message.mAddress, address, 4);
    message.mPort = port;
    nlStrNCpy(message.mHostName, mLocalPlayerName, 11);
    u8 buffer[250];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    mSocket->SendBroadcast(buffer, size);
}

void LANLobby::Update(float dt)
{
    int index = 0;
    while (index < mFoundGameCount)
    {
        mFoundGames[index].mTimeSinceSeen += dt;
        if (mFoundGames[index].mTimeSinceSeen >= g_fLANGameExpireTime)
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
            SendFindGame();
        }
    }
    if (mLaunchState == 1)
    {
        bool ready = CheckPeerStates();
        if (ready)
        {
            if (GetTopology() == 0)
            {
                mLaunchState = 2;
                SendReadyToLaunchRequest();
            }
            else
                mLaunchState = 3;
        }
    }
    if (mLaunchState == 2)
    {
        bool ready = true;
        for (int peer = 1; peer < mPeerCount; ++peer)
        {
            if (!mPeerInfoList[peer].mConnectionConfirmed)
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
                mLaunchState = 0;
            }
        }
        else
            mLaunchState = 3;
    }
    if (mLaunchConfirmationPending && ArePeerConnectionsReady())
    {
        SendReadyToLaunchConfirm();
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
        if (mPeerCount >= 2 && mLaunchState == 0)
        {
            mLaunchState = 1;
            return 0;
        }
        return 2;
    }
    return 3;
}

void LANLobby::CompleteLaunch()
{
    mLaunchState = 4;
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
        int index = mPeerInfoList[peer].mConnectionIndex;
        if (index >= 0 && index < 8)
        {
            if (m_ConnectionPool[index].mStatus != 0)
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

void LANLobby::ProcessReadyToLaunchRequest()
{
    if (!mIsHost)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Received ready to launch request\n");
        if (ArePeerConnectionsReady())
        {
            SendReadyToLaunchConfirm();
            mLaunchConfirmationPending = false;
        }
        else
            mLaunchConfirmationPending = true;
    }
}

void LANLobby::ProcessReadyToLaunchConfirm(int peer, NetMessageReadyToLaunchConfirm* message)
{
    mPeerInfoList[peer].mConnectionConfirmed = message->mConfirmed;
    tDebugPrintManager::Print(DC_NETWORK, "Received ready to launch confirm from peer %d\n", peer);
}

bool LANLobby::ArePeerConnectionsReady()
{
    if (GetTopology() == 0)
    {
        bool ready = true;
        for (int peer = 1; peer < mPeerCount; ++peer)
        {
            if (peer == mLocalMachineIndex)
                continue;
            int index = mPeerInfoList[peer].mConnectionIndex;
            if (index == -1)
            {
                ready = false;
                continue;
            }
            if (m_ConnectionPool[index].mStatus != 2)
                ready = false;
        }
        return ready;
    }
    return true;
}

void LANLobby::SendReadyToLaunchConfirm()
{
    NetMessageReadyToLaunchConfirm message;
    message.mConfirmed = 1;
    u8 buffer[8];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    int index = mPeerInfoList[0].mConnectionIndex;
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
        if (nlStrCmp(mFoundGames[index].mHostName, message->mHostName) == 0)
        {
            mFoundGames[index].mTimeSinceSeen = 0.0f;
            return;
        }
    }
    mFoundGames[mFoundGameCount].mGameType = message->mGameType;
    memcpy(mFoundGames[mFoundGameCount].mAddress, message->mAddress, 4);
    mFoundGames[mFoundGameCount].mPort = message->mPort;
    nlStrNCpy(mFoundGames[mFoundGameCount].mHostName, message->mHostName, 11);
    mFoundGames[mFoundGameCount].mTimeSinceSeen = 0.0f;
    if (mListener != 0)
        mListener->OnGameFound(&mFoundGames[mFoundGameCount]);
    ++mFoundGameCount;
}

void LANLobby::SendGamePeerAdded(int index)
{
    NetMessageGamePeerAdded message;
    *(u32*)message.mPeer.mAddress = mPeerInfoList[index].mAddress.word;
    message.mPeer.mPort = mPeerInfoList[index].mPort;
    nlStrNCpy(message.mPeer.mName, mPeerInfoList[index].mName, 11);
    message.mPeer.mUserMatchDataSize = mPeerInfoList[index].mUserMatchDataSize;
    memcpy(message.mPeer.mUserMatchData, &mPeerInfoList[index].mDisplayRank, mPeerInfoList[index].mUserMatchDataSize);
    message.mPeer.mPeerIndex = index;
    u8 buffer[250];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    for (int peer = 1; peer < index; ++peer)
    {
        mSocket->Send((u32)m_ConnectionPool[mPeerInfoList[peer].mConnectionIndex].m_Connection,
            buffer,
            size,
            true);
    }
}

void LANLobby::SendJoinResponse(TransportConnection* connection, bool accepted)
{
    NetMessageJoinResponse message;
    *(u32*)message.mAddress = *(u32*)mSocket->GetLocalAddress();
    message.mPort = mSocket->GetLocalPort();
    nlStrNCpy(message.mName, mLocalPlayerName, 11);
    message.mUserMatchDataSize = mUserMatchDataSize;
    memcpy(message.mUserMatchData, mUserMatchData, mUserMatchDataSize);
    message.mAccepted = accepted;
    message.mPeerCount = 0;
    if (mPeerCount > 2)
    {
        for (int peer = 1; peer < mPeerCount - 1; ++peer)
        {
            message.mPeers[peer - 1].mAddressWord = mPeerInfoList[peer].mAddress.word;
            message.mPeers[peer - 1].mPort = mPeerInfoList[peer].mPort;
            nlStrNCpy(message.mPeers[peer - 1].mName, mPeerInfoList[peer].mName, 11);
            message.mPeers[peer - 1].mUserMatchDataSize = mPeerInfoList[peer].mUserMatchDataSize;
            memcpy(message.mPeers[peer - 1].mUserMatchData, &mPeerInfoList[peer].mDisplayRank, mPeerInfoList[peer].mUserMatchDataSize);
            message.mPeers[peer - 1].mPeerIndex = peer;
            ++message.mPeerCount;
        }
    }
    u8 buffer[250];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    mSocket->Send((u32)connection, buffer, size, true);
}

void LANLobby::ProcessJoinRequest(int index, NetMessageJoinRequest* message)
{
    TransportConnection* connection = m_ConnectionPool[index].m_Connection;
    if (mPeerCount < mMaxMachineCount && mLaunchState == 0)
    {
        nlStrNCpy(mPeerInfoList[mPeerCount].mName, message->mName, 11);
        mPeerInfoList[mPeerCount].mUserMatchDataSize = message->mUserMatchDataSize;
        memcpy(&mPeerInfoList[mPeerCount].mDisplayRank, message->mUserMatchData, message->mUserMatchDataSize);
        mPeerInfoList[mPeerCount].mAddress = connection->mAddress;
        mPeerInfoList[mPeerCount].mPort = connection->mPort;
        if (GetTopology() == 0)
            mPeerInfoList[mPeerCount].mHostState = 2;
        else
            mPeerInfoList[mPeerCount].mHostState = 3;
        mPeerInfoList[mPeerCount].mConnectionConfirmed = false;
        mPeerInfoList[mPeerCount].mConnectionIndex = index;
        ++mPeerCount;
        SendJoinResponse(connection, true);
        if (mPeerCount > 2 && GetTopology() != 0)
            SendGamePeerAdded(mPeerCount - 1);
        if (mPlayerListener != 0)
            mPlayerListener->OnPlayerListChanged();
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
    if (message->mAccepted)
    {
        nlStrNCpy(mPeerInfoList[0].mName, message->mName, 11);
        mPeerInfoList[0].mUserMatchDataSize = message->mUserMatchDataSize;
        memcpy(&mPeerInfoList[0].mDisplayRank, message->mUserMatchData, message->mUserMatchDataSize);
        mPeerInfoList[0].mAddress.word = *(u32*)message->mAddress;
        mPeerInfoList[0].mPort = message->mPort;
        mPeerInfoList[0].mConnectionIndex = index;
        mPeerInfoList[0].mHostState = 0;
        mPeerInfoList[0].mConnectionConfirmed = false;
        int peer = 1;
        for (int entry = 0; entry < message->mPeerCount; ++peer, ++entry)
        {
            nlStrNCpy(mPeerInfoList[peer].mName, message->mPeers[entry].mName, 11);
            mPeerInfoList[peer].mUserMatchDataSize = message->mPeers[entry].mUserMatchDataSize;
            memcpy(&mPeerInfoList[peer].mDisplayRank, message->mPeers[entry].mUserMatchData, message->mPeers[entry].mUserMatchDataSize);
            mPeerInfoList[peer].mAddress.word = message->mPeers[entry].mAddressWord;
            mPeerInfoList[peer].mPort = message->mPeers[entry].mPort;
            mPeerInfoList[peer].mConnectionIndex = -1;
            mPeerInfoList[peer].mHostState = 0;
            mPeerInfoList[peer].mConnectionConfirmed = false;
        }
        nlStrNCpy(mPeerInfoList[peer].mName, mLocalPlayerName, 11);
        mPeerInfoList[peer].mUserMatchDataSize = mUserMatchDataSize;
        memcpy(&mPeerInfoList[peer].mDisplayRank, mUserMatchData, mUserMatchDataSize);
        mPeerInfoList[peer].mAddress.word = *(u32*)mSocket->GetLocalAddress();
        mPeerInfoList[peer].mPort = mSocket->GetLocalPort();
        mPeerInfoList[peer].mConnectionIndex = -1;
        mPeerInfoList[peer].mHostState = 0;
        mPeerInfoList[peer].mConnectionConfirmed = false;
        mLocalMachineIndex = peer;
        mPeerCount = peer + 1;
        if (mPlayerListener != 0)
            mPlayerListener->OnPlayerListChanged();
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
    s8 peer = message->mPeer.mPeerIndex;
    mPeerInfoList[peer].mAddress.word = *(u32*)message->mPeer.mAddress;
    mPeerInfoList[peer].mPort = message->mPeer.mPort;
    nlStrNCpy(mPeerInfoList[peer].mName, message->mPeer.mName, 11);
    mPeerInfoList[peer].mUserMatchDataSize = message->mPeer.mUserMatchDataSize;
    memcpy(&mPeerInfoList[peer].mDisplayRank, message->mPeer.mUserMatchData, message->mPeer.mUserMatchDataSize);
    mPeerInfoList[peer].mHostState = 0;
    mPeerInfoList[peer].mConnectionConfirmed = false;
    mPeerInfoList[peer].mConnectionIndex = -1;
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
        mPeerInfoList[peer].mConnectionIndex = index;
        m_ConnectionPool[index].mStatus = 1;
        if (mSocket->Connect(&m_ConnectionPool[index].m_Connection,
                message->mPeer.mAddress,
                message->mPeer.mPort))
            tDebugPrintManager::Print(DC_NETWORK, "Attempting peer-peer connection to other client\n");
        else
        {
            m_ConnectionPool[index].m_Connection = 0;
            m_ConnectionPool[index].mStatus = 0;
            mPeerInfoList[peer].mConnectionIndex = -1;
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
            SendFoundGame(static_cast<NetMessageFindGame*>(message)->mToken);
        break;
    case 3:
        if (!mIsHost)
        {
            NetMessageFoundGame* reply = static_cast<NetMessageFoundGame*>(message);
            if (memcmp(reply->mToken, mFindGameToken, 8) == 0)
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
        ProcessReadyToLaunchRequest();
        break;
    case 11:
        if (mIsHost)
        {
            int peer = MachineIdxFromConnection(message->mSource);
            if (peer > 0 && peer < mPeerCount)
                ProcessReadyToLaunchConfirm(peer, static_cast<NetMessageReadyToLaunchConfirm*>(message));
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
                mPeerInfoList[reply->mMachineIndex].mHostState = 3;
                if (reply->mMachineIndex > 1)
                    SendGamePeerAdded(reply->mMachineIndex);
            }
            else
                tDebugPrintManager::Print(DC_NETWORK, "Ignored ClientConfirmedJoin because did not find peer it's from\n");
        }
        break;
    }
    return 1;
}

int LANLobby::GetTopology()
{
    return mTopology;
}

void LANLobby::SetTopology(int topology)
{
    mTopology = topology;
}

int LANLobby::GetMaxMachineCount()
{
    return mMaxMachineCount;
}

void LANLobby::SetMaxMachineCount(int count)
{
    mMaxMachineCount = count;
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
                glFontPrintf(GetDebugFontView(), column, (*row)++, mFoundGames[index].mHostName);
        }
        if (mPeerCount > 0)
        {
            glFontPrintf(GetDebugFontView(), column, (*row)++, "InGame NumPeers:%d", mPeerCount);
            for (int index = 0; index < mPeerCount; ++index)
            {
                if (mIsHost)
                    glFontPrintf(GetDebugFontView(), column, (*row)++, "%d: %d %s", index, mPeerInfoList[index].mHostState, mPeerInfoList[index].mName);
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
