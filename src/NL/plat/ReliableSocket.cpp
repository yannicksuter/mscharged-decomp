#include "NL/plat/ReliableSocket.h"
#include "Game/Sys/debug.h"
#include "NL/plat/TransportConnection.h"

#include "NL/nlMemory.h"
#include "Game/NetworkDiagnostics.h"
#include "NL/nlDebugFile.h"
#include "NL/nlPrint.h"
#include "NL/nlTicker.h"

#include <string.h>

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Sys/tweak.h"
#include "NL/gl/glFont.h"
#include "NL/nlMain.h"
#include "NL/nlString.h"
#include "NL/nlDebugViews.h"

#include <stdarg.h>

int g_TransportLayerLog = 1;
int s_nPayloadRedundancy = 3;
int s_nPacketRedundancy = 1;
int s_nSendKeepAliveMS = 1000;
int s_nExpireKeepAliveMS = 6000;
bool s_bExpireKeepAliveEnabled = true;
int s_nSendVoiceMS = 100;
int s_nSendVoiceHighwaterNum = 4;
int s_nSendPendingAckMS = 50;
int s_nResendNormalMS = 500;
bool s_nResendNormalAggressive = true;
int s_nResendGroupMS = 30;
int s_nSendPingMS = 2000;
int s_nConnectClientTimeoutMS = 10000;
int s_nConnectServerTimeoutMS = 10000;
int s_nClosingTimeoutMS = 500;
int s_nSendEveryNthFrame = 1;
int s_nBWWindowMS = 200;
u32 lbl_806DF6E8 = 0x544C4159;
bool s_bLogTL = false;
bool s_bLogTLUseCache = false;
bool s_bDisplayBW = false;
bool s_bDisplayScreenPrinter = false;
float lbl_806E20EC;
float lbl_806E20F0;
int lbl_806E20F4;

int sPayloadRedundancySteps[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };


ReliableSocket::ReliableSocket()
{
    nlBufferedWriterInitialize(&mLogWriter);
    for (int i = 0; i < 10; ++i)
    {
        mScreenPrinter.mLines[i][0] = '\0';
    }
    mScreenPrinter.mNextLine = 0;

    InitializeTransportChallengeCipher();
    mCallback = 0;
    mConnectionCount = 0;
    mInitialized = false;
    mEnabled = false;
    mDebugFile = 0;
    mLastUpdateTick = 0;
    mSentBytes = 0;
    mReceivedBytes = 0;
    for (int i = 0; i < 10; ++i)
    {
        mScreenPrinter.mLines[i][0] = '\0';
    }
    mScreenPrinter.mNextLine = 0;
}

void ReliableSocket::LogMessage(int size, TransportMessage* message)
{
    if (nlDebugFileIsValid(mDebugFile))
    {
        int count = message->GetReliableCount();
        char text[200];
        FormatNetworkTimestamp(text, sizeof(text), true);
        nlBufferedWriterWriteText(&mLogWriter, text);
        nlSNPrintf(text, sizeof(text),
            " Size %d TMSG ACK:%d PI:%d PO:%d CL:%d UR:%d VO:%d RE:%d ",
            size, message->HasAck(), message->HasPing(), message->HasPong(),
            message->IsClosed(), message->GetUnreliableCount(),
            message->GetVoiceCount(), count);
        nlBufferedWriterWriteText(&mLogWriter, text);
        for (int i = 0; i < count; i++)
        {
            TransportPacket* packet = message->GetReliablePacket(i);
            nlSNPrintf(text, sizeof(text), "R%d: Ty:%d SN:%d Sz:%d ",
                i, packet->mType, packet->mSequence, packet->mSize);
            nlBufferedWriterWriteText(&mLogWriter, text);
        }
        nlBufferedWriterWriteText(&mLogWriter, "\n");
        lbl_806E20F4++;
        if (lbl_806E20F4 % 10 == 0)
        {
            nlBufferedWriterFlushIfNeeded(&mLogWriter);
        }
    }
}

char sTransportLogPathFormat[] = "GameLog/TLLog_%s.txt";
int ReliableSocket::Initialize(ReliableSocketCallback* callback)
{
    mCallback = callback;

    if (s_bLogTL)
    {
        char name[100];
        char path[200];
        FormatNetworkTimestamp(name, sizeof(name), false);
        nlSNPrintf(path, sizeof(path), sTransportLogPathFormat, name);
        mDebugFile = nlOpenFileDebug(path, false, false);
        if (nlDebugFileIsValid(mDebugFile))
        {
            nlBufferedWriterAttach(&mLogWriter, mDebugFile,
                s_bLogTLUseCache, 20000, 14000);
        }
    }

    mLastUpdateTick = nlGetTicker();
    mInitialized = true;
    return 1;
}

void ReliableSocket::Shutdown()
{
    for (int i = 0; i < mConnectionCount; i++)
    {
        delete mConnections[i];
        mConnections[i] = 0;
    }
    mConnectionCount = 0;

    nlBufferedWriterFinish(&mLogWriter);
    if (nlDebugFileIsValid(mDebugFile))
    {
        nlCloseFileDebug(mDebugFile);
        mDebugFile = 0;
    }

    mCallback = 0;
    mConnectionCount = 0;
    mInitialized = false;
    mEnabled = false;
    mDebugFile = 0;
    mLastUpdateTick = 0;
    mSentBytes = 0;
    mReceivedBytes = 0;
    mScreenPrinter.mLines[0][0] = '\0';
    mScreenPrinter.mLines[1][0] = '\0';
    mScreenPrinter.mLines[2][0] = '\0';
    mScreenPrinter.mLines[3][0] = '\0';
    mScreenPrinter.mLines[4][0] = '\0';
    mScreenPrinter.mLines[5][0] = '\0';
    mScreenPrinter.mLines[6][0] = '\0';
    mScreenPrinter.mLines[7][0] = '\0';
    mScreenPrinter.mLines[8][0] = '\0';
    mScreenPrinter.mLines[9][0] = '\0';
    mScreenPrinter.mNextLine = 0;
}

void ReliableSocket::SetEnabled(bool enabled)
{
    mEnabled = enabled;
}

char sOutgoingConnectionDisplayFormat[] = "New Out connection %d.%d.%d.%d port %d";
char sOutgoingConnectionLogFormat[] = "Creating new outgoing connection address %d.%d.%d.%d port %d\n";
int ReliableSocket::Connect(void* connection, const u8* address, u16 port)
{
    mScreenPrinter.Printf(sOutgoingConnectionDisplayFormat, address[0],
        address[1], address[2], address[3], port);
    if (g_TransportLayerLog >= 1)
    {
        tDebugPrintManager::Print(DC_NETWORK, sOutgoingConnectionLogFormat,
            address[0], address[1], address[2], address[3], port);
    }

    TransportConnection* result = new (8, false)
        TransportConnection(this, address, port, true);
    mConnections[mConnectionCount] = result;
    mConnectionCount++;
    *(void**)connection = result;
    result->SendClientChallenge();
    return 0;
}

void ReliableSocket::Disconnect(TransportConnection* connection, bool immediate)
{
    connection->Disconnect(immediate);
}

void ReliableSocket::Send(int connection,
    void* buffer, int size, bool reliable)
{
    if (reliable)
    {
        ((TransportConnection*)connection)
            ->SubmitReliable(0, buffer, size);
    }
    else
    {
        ((TransportConnection*)connection)
            ->SubmitUnreliable(buffer, size);
    }
}

void ReliableSocket::SocketVirtual34(u8 connection,
    void* buffer, int size)
{
    ((TransportConnection*)connection)
        ->SubmitVoice(buffer, size);
}

void ReliableSocket::DebugDraw(int column, int* row, bool showBandwidth)
{
    if (showBandwidth)
    {
        glFontPrintf(GetDebugFontView(), column, (*row)++,
            "Recv %d bytes/s", (int)lbl_806E20F0);
        glFontPrintf(GetDebugFontView(), column, (*row)++,
            "Send %d bytes/s", (int)lbl_806E20EC);
    }
    for (int i = 0; i < mConnectionCount; i++)
    {
        glFontPrintf(GetDebugFontView(), column, (*row)++,
            "Lat: %d ms\n", (u32)mConnections[i]->mRoundTripTimeMS / 2);
    }
}

int lbl_806DF6F0 = 5;
int lbl_806DF6F4 = 5;
inline void TransportScreenPrinter::Draw()
{
    for (int i = 0; i < 10; i++)
    {
        if (mLines[i][0] != '\0')
        {
            nlScreenPrintf(lbl_806DF6F4, i + lbl_806DF6F0, true, 0, mLines[i]);
        }
    }
}

void ReliableSocket::SocketVirtual48()
{
    if (s_bDisplayScreenPrinter)
    {
        mScreenPrinter.Draw();
    }
}

void TransportScreenPrinter::Printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    nlVSNPrintf(mLines[mNextLine], sizeof(mLines[mNextLine]), format, args);
    va_end(args);
    mNextLine++;
    if (mNextLine >= 10)
    {
        mNextLine = 0;
    }
}

void TransportScreenPrinter::Print(const char* text)
{
    nlStrNCpy(mLines[mNextLine], text, sizeof(mLines[mNextLine]));
    mNextLine++;
    if (mNextLine >= 10)
    {
        mNextLine = 0;
    }
}

inline void ReliableSocket::UpdateBandwidth()
{
    u32 tick = nlGetTicker();
    float elapsed = nlGetTickerDifference(mLastUpdateTick, tick);
    if ((int)elapsed > s_nBWWindowMS)
    {
        lbl_806E20F0 = 1000.0f * mReceivedBytes / elapsed;
        lbl_806E20EC = 1000.0f * mSentBytes / elapsed;
        mLastUpdateTick = tick;
        mSentBytes = 0;
        mReceivedBytes = 0;
    }
}

void ReliableSocket::Update()
{
    for (int i = 0; i < mConnectionCount; i++)
    {
        mConnections[i]->Update();
    }
    int i = 0;
    while (i < mConnectionCount)
    {
        if (mConnections[i]->IsFinished())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Deleting connection address %d.%d.%d.%d state %d host %d",
                mConnections[i]->mAddress[0], mConnections[i]->mAddress[1],
                mConnections[i]->mAddress[2], mConnections[i]->mAddress[3],
                mConnections[i]->GetState(), mConnections[i]->mIncoming);
            delete mConnections[i];
            mConnections[i] = 0;
            for (int j = i; j < mConnectionCount - 1; j++)
            {
                mConnections[j] = mConnections[j + 1];
            }
            mConnectionCount--;
        }
        else
        {
            i++;
        }
    }
    UpdateBandwidth();
}

void ReliableSocket::SendMessage(TransportMessage* message, const u8* address,
    u16 port, bool* error)
{
    if (error != 0 && *error)
    {
        return;
    }
    NetworkMessageSerializer serializer(1, mSendBuffer, sizeof(mSendBuffer));
    message->Serialize(&serializer);
    if (message->HasVoice())
    {
        message->SerializeVoice(&serializer);
    }
    int size = serializer.GetLength();
    int payloadSize = size - sizeof(u32);
    if (payloadSize > 0)
    {
        u32 checksum = nlChecksum32(mSendBuffer + sizeof(u32), payloadSize);
        checksum ^= lbl_806DF6E8;
        memcpy(mSendBuffer, &checksum, sizeof(checksum));
    }
    mSentBytes += size + 28;
    if (s_bLogTL)
    {
        LogMessage(size, message);
    }
    int result = mCallback->SendDatagram(mSendBuffer, size, address, port);
    if (result < 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Error sending to address %d.%d.%d.%d port %d... result: %d\n",
            address[0], address[1], address[2], address[3], port, result);
        if (error != 0)
        {
            *error = true;
        }
    }
}

void ReliableSocket::HandleMessage(TransportMessage* message, const u8* address,
    u16 port)
{
    TransportConnection* connection = (TransportConnection*)FindConnection(address);
    if (connection == 0)
    {
        if (!message->IsClientChallenge())
        {
            if (!message->IsClosed())
            {
                TransportMessage response;
                response.SetClosed();
                SendMessage(&response, address, port, 0);
            }
            tDebugPrintManager::Print(DC_NETWORK,
                "Not a client challenge so do not accept incoming connection\n");
            message->ReleasePackets();
            return;
        }
        if (!mEnabled)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Not listening so not creating new connection\n");
            message->ReleasePackets();
            return;
        }
        mScreenPrinter.Printf("New Inc connection %d.%d.%d.%d port %d",
            address[0], address[1], address[2], address[3], port);
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Creating new incoming connection address %d.%d.%d.%d port %d\n",
                address[0], address[1], address[2], address[3], port);
        }
        connection = new (8, false) TransportConnection(this, address, port, false);
        mConnections[mConnectionCount] = connection;
        mConnectionCount++;
    }
    if (connection->IsClosed())
    {
        if (!message->IsClosed())
        {
            TransportMessage response;
            response.SetClosed();
            SendMessage(&response, address, port, 0);
        }
        message->ReleasePackets();
    }
    else
    {
        connection->HandleTransportMessage(message);
    }
}

static inline bool CheckTransportChecksum(const void* buffer, int size)
{
    if (size - (int)sizeof(u32) <= 0)
    {
        return false;
    }
    u32 checksum;
    memcpy(&checksum, buffer, sizeof(checksum));
    u32 calculated = nlChecksum32((const u8*)buffer + sizeof(u32), size - sizeof(u32));
    calculated ^= lbl_806DF6E8;
    return checksum == calculated;
}

void ReliableSocket::ReceiveDatagram(void* buffer, int size,
    const u8* address, u16 port)
{
    mReceivedBytes += size + 28;
    if ((u32)size <= sizeof(u32))
    {
        return;
    }
    if (CheckTransportChecksum(buffer, size))
    {
        TransportMessage message;
        NetworkMessageSerializer serializer(0, (u8*)buffer, size);
        message.Serialize(&serializer);
        if (message.HasVoice())
        {
            message.SerializeVoice(&serializer);
        }
        HandleMessage(&message, address, port);
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Warning: Discarded message failed CRC Msg Size %d\n", size);
    }
}

void* ReliableSocket::FindConnection(const u8* address)
{
    for (int i = 0; i < mConnectionCount; i++)
    {
        if (memcmp(mConnections[i]->mAddress,
                address, 4)
            == 0)
        {
            return mConnections[i];
        }
    }
    return 0;
}

void ReliableSocket::AcceptConnection(unsigned int connection)
{
    ((TransportConnection*)connection)->Accept();
}

void ReliableSocket::RejectConnection(unsigned int connection)
{
    ((TransportConnection*)connection)->Reject();
}

static TweakIntBinding lbl_80584530(
    "g_TransportLayerLog", "Network/TransportLayer", &g_TransportLayerLog, false);
static TweakBoolBinding lbl_80584550(
    "s_bDisplayScreenPrinter", "Network/TransportLayer", &s_bDisplayScreenPrinter, false);
static TweakBoolBinding lbl_80584570(
    "s_bDisplayBW", "Network/TransportLayer", &s_bDisplayBW, false);
static TweakIntBinding lbl_80584590(
    "s_nPayloadRedundancy", "Network/TransportLayer", &s_nPayloadRedundancy, false);
static TweakIntBinding lbl_805845B0(
    "s_nPacketRedundancy", "Network/TransportLayer", &s_nPacketRedundancy, false);
static TweakIntBinding lbl_805845D0(
    "s_nSendKeepAliveMS", "Network/TransportLayer", &s_nSendKeepAliveMS, false);
static TweakIntBinding lbl_805845F0(
    "s_nExpireKeepAliveMS", "Network/TransportLayer", &s_nExpireKeepAliveMS, false);
static TweakBoolBinding lbl_80584610(
    "s_bExpireKeepAliveEnabled", "Network/TransportLayer", &s_bExpireKeepAliveEnabled, false);
static TweakIntBinding lbl_80584630(
    "s_nSendVoiceMS", "Network/TransportLayer", &s_nSendVoiceMS, false);
static TweakIntBinding lbl_80584650(
    "s_nSendVoiceHighwaterNum", "Network/TransportLayer", &s_nSendVoiceHighwaterNum, false);
static TweakIntBinding lbl_80584670(
    "s_nSendPendingAckMS", "Network/TransportLayer", &s_nSendPendingAckMS, false);
static TweakIntBinding lbl_80584690(
    "s_nResendNormalMS", "Network/TransportLayer", &s_nResendNormalMS, false);
static TweakBoolBinding lbl_805846B0(
    "s_nResendNormalAggressive", "Network/TransportLayer", &s_nResendNormalAggressive, false);
static TweakIntBinding lbl_805846D0(
    "s_nResendGroupMS", "Network/TransportLayer", &s_nResendGroupMS, false);
static TweakIntBinding lbl_805846F0(
    "s_nSendPingMS", "Network/TransportLayer", &s_nSendPingMS, false);
static TweakIntBinding lbl_80584710(
    "s_nConnectClientTimeoutMS", "Network/TransportLayer", &s_nConnectClientTimeoutMS, false);
static TweakIntBinding lbl_80584730(
    "s_nConnectServerTimeoutMS", "Network/TransportLayer", &s_nConnectServerTimeoutMS, false);
static TweakIntBinding lbl_80584750(
    "s_nClosingTimeoutMS", "Network/TransportLayer", &s_nClosingTimeoutMS, false);
static TweakIntBinding lbl_80584770(
    "s_nSendEveryNthFrame", "Network/TransportLayer", &s_nSendEveryNthFrame, false);
static TweakBoolBinding lbl_80584790(
    "s_bLogTL", "Network/TransportLayer", &s_bLogTL, false);
static TweakBoolBinding lbl_805847B0(
    "s_bLogTLUseCache", "Network/TransportLayer", &s_bLogTLUseCache, false);
static TweakIntBinding lbl_805847D0(
    "s_nBWWindowMS", "Network/TransportLayer", &s_nBWWindowMS, false);
