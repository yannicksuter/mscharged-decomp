#include "NL/plat/ReliableSocket.h"
#include "Game/Sys/debug.h"
#include "NL/plat/TransportConnection.h"

#include "NL/nlMemory.h"
#include "Game/NetworkDiagnostics.h"
#include "NL/nlDebugFile.h"
#include "NL/nlPrint.h"
#include "NL/nlTicker.h"

#include <string.h>

extern char sTransportLogPathFormat[];
extern char sOutgoingConnectionDisplayFormat[];
extern char sOutgoingConnectionLogFormat[];


ReliableSocket::ReliableSocket()
{
    nlBufferedWriterInitialize(&mLogWriter);
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

    InitializeTransportChallengeCipher();
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
