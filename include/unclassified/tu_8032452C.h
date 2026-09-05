#ifndef UNCLASSIFIED_TU_8032452C_H
#define UNCLASSIFIED_TU_8032452C_H

#include "types.h"

class UnidentifiedReliableSocketCallback;
class UnidentifiedTransportConnection;
class UnidentifiedTransportMessage_8032B6D4;
struct UnidentifiedReliableSocketState;

struct UnidentifiedTransportLogWriter
{
    /* 0x00 */ void* mFile;
    /* 0x04 */ bool mBuffered;
    /* 0x05 */ bool mWriteToNAND;
    /* 0x06 */ u8 mPadding[2];
    /* 0x08 */ u32 mBufferSize;
    /* 0x0C */ u32 mFlushThreshold;
    /* 0x10 */ char* mBuffer;
    /* 0x14 */ char* mCurrent;
}; // size: 0x18

struct UnidentifiedTransportDisplayEntry
{
    /* 0x00 */ bool mActive;
    /* 0x01 */ u8 mData[0x63];
}; // size: 0x64

struct UnidentifiedReliableSocketLayout
{
    /* 0x000 */ bool mInitialized;
    /* 0x001 */ u8 mPadding001[3];
    /* 0x004 */ UnidentifiedReliableSocketCallback* mCallback;
    /* 0x008 */ int mConnectionCount;
    /* 0x00C */ UnidentifiedTransportConnection* mConnections[382];
    /* 0x604 */ u8 mUnidentified604;
    /* 0x605 */ bool mEnabled;
    /* 0x606 */ u8 mPadding606[2];
    /* 0x608 */ void* mDebugFile;
    /* 0x60C */ UnidentifiedTransportLogWriter mLogWriter;
    /* 0x624 */ u32 mLastUpdateTick;
    /* 0x628 */ u32 mReceivedBytes;
    /* 0x62C */ u32 mSentBytes;
    /* 0x630 */ UnidentifiedTransportDisplayEntry mDisplayEntries[10];
    /* 0xA18 */ u32 mUnidentifiedA18;
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
extern int lbl_80531938[10];

extern "C"
{
    int fn_80324778(UnidentifiedReliableSocketState* socket,
        UnidentifiedReliableSocketCallback* callback);
    void fn_80324828(UnidentifiedReliableSocketState* socket);
    void fn_80324918(
        UnidentifiedReliableSocketState* socket, bool enabled);
    int fn_80324920(UnidentifiedReliableSocketState* socket,
        void* connection, const u8* address, u16 port);
    void fn_80324A1C(UnidentifiedReliableSocketState* socket,
        UnidentifiedTransportConnection* connection, bool immediate);
    void fn_80324A28(UnidentifiedReliableSocketState* socket, int aid,
        void* buffer, int size, bool reliable);
    void fn_80324A4C(UnidentifiedReliableSocketState* socket, u8 aid,
        void* buffer, int size);
    void fn_80324A5C(UnidentifiedReliableSocketState* socket, void* a,
        void* b, bool c);
    void fn_80324B54(UnidentifiedReliableSocketState* socket);
    void fn_80324CB4(
        UnidentifiedTransportDisplayEntry* entries, const char* text);
    void fn_80324D1C(UnidentifiedReliableSocketState* socket);
    void fn_80324EAC(UnidentifiedReliableSocketState* socket,
        UnidentifiedTransportMessage_8032B6D4* message, const u8* address,
        u16 port, bool* error);
    void fn_80325264(UnidentifiedReliableSocketState* socket, void* buffer,
        int size, const u8* address, u16 port);
    void* fn_80325388(
        UnidentifiedReliableSocketState* socket, const u8* address);
    void fn_80325404(
        UnidentifiedReliableSocketState* socket, u32 connection);
    void fn_8032540C(
        UnidentifiedReliableSocketState* socket, u32 connection);
}

#endif // UNCLASSIFIED_TU_8032452C_H
