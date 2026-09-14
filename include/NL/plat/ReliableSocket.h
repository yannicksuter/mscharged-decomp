#ifndef NL_PLAT_RELIABLE_SOCKET_H
#define NL_PLAT_RELIABLE_SOCKET_H

#include "types.h"
#include "NL/nlBufferedWriter.h"

class ReliableSocketCallback
{
public:
    virtual void OnConnectionAttempted(u32 connection, int result) = 0;
    virtual void OnConnectionClosed(u32 connection, int reason) = 0;
    virtual void ReliableCallbackVirtual08() = 0;
    virtual void OnMessageReceived(
        u32 connection, void* buffer, int size, bool reliable) = 0;
    virtual void ReliableCallbackVirtual10(
        u32 connection, void* buffer, int size) = 0;
    virtual void OnConnectionRequest(
        u32 connection, u8* address, int a, int b, int c) = 0;
    virtual int SendDatagram(
        void* buffer, int size, const u8* address, u16 port) = 0;
};

class TransportConnection;
class TransportMessage;

struct TransportScreenPrinter
{
    void Printf(const char* format, ...);
    void Print(const char* text);
    void Draw();

    /* 0x000 */ char mLines[10][100];
    /* 0x3E8 */ int mNextLine;
}; // size: 0x3EC

struct ReliableSocket
{
    ReliableSocket();
    void Shutdown();
    int Initialize(ReliableSocketCallback* callback);
    void SetEnabled(bool enabled);
    int Connect(void* connection, const u8* address, u16 port);
    void Disconnect(TransportConnection* connection, bool immediate);
    void Send(int connection,
        void* buffer, int size, bool reliable);
    void SocketVirtual34(u8 connection,
        void* buffer, int size);
    void DebugDraw(int column, int* row, bool showBandwidth);
    void SocketVirtual48();
    void Update();
    void UpdateBandwidth();
    void LogMessage(int size, TransportMessage* message);
    void HandleMessage(TransportMessage* message, const u8* address, u16 port);
    void SendMessage(TransportMessage* message, const u8* address,
        u16 port, bool* error);
    void ReceiveDatagram(void* buffer,
        int size, const u8* address, u16 port);
    void* FindConnection(const u8* address);
    void AcceptConnection(unsigned int connection);
    void RejectConnection(unsigned int connection);

    /* 0x000 */ bool mInitialized;
    /* 0x001 */ u8 mPadding001[3];
    /* 0x004 */ ReliableSocketCallback* mCallback;
    /* 0x008 */ int mConnectionCount;
    /* 0x00C */ TransportConnection* mConnections[16];
    /* 0x04C */ u8 mSendBuffer[0x5B9];
    /* 0x605 */ bool mEnabled;
    /* 0x606 */ u8 mPadding606[2];
    /* 0x608 */ void* mDebugFile;
    /* 0x60C */ nlBufferedWriter mLogWriter;
    /* 0x624 */ u32 mLastUpdateTick;
    /* 0x628 */ u32 mReceivedBytes;
    /* 0x62C */ u32 mSentBytes;
    /* 0x630 */ TransportScreenPrinter mScreenPrinter;
}; // size: 0xA1C

// "Network/TransportLayer" tweak values registered by the manager's static
// initializer and shared with the connection unit.
extern int g_TransportLayerLog;
extern bool s_bDisplayScreenPrinter;
extern bool s_bDisplayBW;
extern int s_nPayloadRedundancy;
extern int s_nPacketRedundancy;
extern int s_nSendKeepAliveMS;
extern int s_nExpireKeepAliveMS;
extern bool s_bExpireKeepAliveEnabled;
extern int s_nSendVoiceMS;
extern int s_nSendVoiceHighwaterNum;
extern int s_nSendPendingAckMS;
extern int s_nResendNormalMS;
extern bool s_nResendNormalAggressive;
extern int s_nResendGroupMS;
extern int s_nSendPingMS;
extern int s_nConnectClientTimeoutMS;
extern int s_nConnectServerTimeoutMS;
extern int s_nClosingTimeoutMS;
extern int s_nSendEveryNthFrame;
extern bool s_bLogTL;
extern bool s_bLogTLUseCache;
extern int s_nBWWindowMS;
extern int sPayloadRedundancySteps[10];


#endif // NL_PLAT_RELIABLE_SOCKET_H
