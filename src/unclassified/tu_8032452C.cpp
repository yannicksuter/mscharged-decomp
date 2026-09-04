#include "unclassified/tu_8032452C.h"

#include "Game/NetworkSession.h"
#include "NL/nlDebugFile.h"
#include "NL/nlPrint.h"
#include "NL/nlTicker.h"

#include <string.h>

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

struct UnidentifiedTransportConnection
{
    /* 0x00 */ u8 mData[0x24];
    /* 0x24 */ u8 mAddress[4];
};

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

extern char lbl_805319B8[];
extern char lbl_805319D0[];
extern char lbl_805319F8[];
extern int lbl_806DF6A0;
extern bool lbl_806E20E8;
extern bool lbl_806E20E9;

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);
    void fn_802B77B0(UnidentifiedTransportLogWriter* writer);
    void fn_802B77D4(UnidentifiedTransportLogWriter* writer, void* file,
        bool buffered, unsigned int bufferSize,
        unsigned int flushThreshold);
    void fn_802B7848(UnidentifiedTransportLogWriter* writer);
    void fn_803239A8(char* text, unsigned long size, bool arg2);
    void fn_80326A10(void* connection, bool arg1);
    void* fn_80326844(unsigned long size, unsigned int alignment,
        bool clear);
    void* fn_803268AC(void* connection,
        UnidentifiedReliableSocketState* socket, const u8* address,
        u16 port, bool outgoing);
    void fn_80324BE0(UnidentifiedTransportDisplayEntry* entries,
        const char* format, ...);
    void fn_803290A4(int aid, int arg1);
    void fn_803291AC(int aid, void* buffer, int size);
    void fn_803292B4(u8 aid, void* buffer, int size);
    void fn_8032A820(void* connection);
    void fn_8032B068(u32 connection);
    void fn_8032B208(u32 connection);
    void fn_8032B384(void* a, void* b);
    void fn_8032C59C();
}

UnidentifiedReliableSocketState::UnidentifiedReliableSocketState()
{
    UnidentifiedReliableSocketLayout* self =
        (UnidentifiedReliableSocketLayout*)this;
    fn_802B77B0(&self->mLogWriter);
    self->mDisplayEntries[0].mActive = false;
    self->mDisplayEntries[1].mActive = false;
    self->mDisplayEntries[2].mActive = false;
    self->mDisplayEntries[3].mActive = false;
    self->mDisplayEntries[4].mActive = false;
    self->mDisplayEntries[5].mActive = false;
    self->mDisplayEntries[6].mActive = false;
    self->mDisplayEntries[7].mActive = false;
    self->mDisplayEntries[8].mActive = false;
    self->mDisplayEntries[9].mActive = false;
    self->mUnidentifiedA18 = 0;

    fn_8032C59C();
    self->mCallback = 0;
    self->mConnectionCount = 0;
    self->mInitialized = false;
    self->mEnabled = false;
    self->mDebugFile = 0;
    self->mLastUpdateTick = 0;
    self->mSentBytes = 0;
    self->mReceivedBytes = 0;
    self->mDisplayEntries[0].mActive = false;
    self->mDisplayEntries[1].mActive = false;
    self->mDisplayEntries[2].mActive = false;
    self->mDisplayEntries[3].mActive = false;
    self->mDisplayEntries[4].mActive = false;
    self->mDisplayEntries[5].mActive = false;
    self->mDisplayEntries[6].mActive = false;
    self->mDisplayEntries[7].mActive = false;
    self->mDisplayEntries[8].mActive = false;
    self->mDisplayEntries[9].mActive = false;
    self->mUnidentifiedA18 = 0;
}

extern "C" int fn_80324778(UnidentifiedReliableSocketState* socket,
    UnidentifiedReliableSocketCallback* callback)
{
    UnidentifiedReliableSocketLayout* self =
        (UnidentifiedReliableSocketLayout*)socket;
    self->mCallback = callback;

    if (lbl_806E20E8)
    {
        char name[100];
        char path[200];
        fn_803239A8(name, sizeof(name), false);
        nlSNPrintf(path, sizeof(path), lbl_805319B8, name);
        self->mDebugFile = nlOpenFileDebug(path, false, false);
        if (nlDebugFileIsValid(self->mDebugFile))
        {
            fn_802B77D4(&self->mLogWriter, self->mDebugFile,
                lbl_806E20E9, 20000, 14000);
        }
    }

    self->mLastUpdateTick = nlGetTicker();
    self->mInitialized = true;
    return 1;
}

extern "C" void fn_80324828(UnidentifiedReliableSocketState* socket)
{
    UnidentifiedReliableSocketLayout* self =
        (UnidentifiedReliableSocketLayout*)socket;
    for (int i = 0; i < self->mConnectionCount; i++)
    {
        fn_80326A10(self->mConnections[i], true);
        self->mConnections[i] = 0;
    }
    self->mConnectionCount = 0;

    fn_802B7848(&self->mLogWriter);
    if (nlDebugFileIsValid(self->mDebugFile))
    {
        nlCloseFileDebug(self->mDebugFile);
        self->mDebugFile = 0;
    }

    self->mCallback = 0;
    self->mConnectionCount = 0;
    self->mInitialized = false;
    self->mEnabled = false;
    self->mDebugFile = 0;
    self->mLastUpdateTick = 0;
    self->mSentBytes = 0;
    self->mReceivedBytes = 0;
    self->mDisplayEntries[0].mActive = false;
    self->mDisplayEntries[1].mActive = false;
    self->mDisplayEntries[2].mActive = false;
    self->mDisplayEntries[3].mActive = false;
    self->mDisplayEntries[4].mActive = false;
    self->mDisplayEntries[5].mActive = false;
    self->mDisplayEntries[6].mActive = false;
    self->mDisplayEntries[7].mActive = false;
    self->mDisplayEntries[8].mActive = false;
    self->mDisplayEntries[9].mActive = false;
    self->mUnidentifiedA18 = 0;
}

extern "C" void fn_80324918(
    UnidentifiedReliableSocketState* socket, bool enabled)
{
    ((UnidentifiedReliableSocketLayout*)socket)->mEnabled = enabled;
}

extern "C" int fn_80324920(UnidentifiedReliableSocketState* socket,
    void* connection, const u8* address, u16 port)
{
    UnidentifiedReliableSocketLayout* self =
        (UnidentifiedReliableSocketLayout*)socket;
    fn_80324BE0(&self->mDisplayEntries[0], lbl_805319D0, address[0],
        address[1], address[2], address[3], port);
    if (lbl_806DF6A0 >= 1)
    {
        fn_8004F594(0x10, lbl_805319F8,
            address[0], address[1], address[2], address[3], port);
    }

    void* result = fn_80326844(2024, 8, false);
    if (result != 0)
    {
        result = fn_803268AC(result, socket, address, port, true);
    }
    self->mConnections[self->mConnectionCount] =
        (UnidentifiedTransportConnection*)result;
    self->mConnectionCount++;
    *(void**)connection = result;
    fn_8032A820(result);
    return 0;
}

extern "C" void fn_80324A1C(
    UnidentifiedReliableSocketState*, void* a, void* b)
{
    fn_8032B384(a, b);
}

extern "C" void fn_80324A28(UnidentifiedReliableSocketState*, int aid,
    void* buffer, int size, bool reliable)
{
    if (reliable)
    {
        fn_803290A4(aid, 0);
    }
    else
    {
        fn_803291AC(aid, buffer, size);
    }
}

extern "C" void fn_80324A4C(UnidentifiedReliableSocketState*, u8 aid,
    void* buffer, int size)
{
    fn_803292B4(aid, buffer, size);
}

extern "C" void* fn_80325388(
    UnidentifiedReliableSocketState* socket, const u8* address)
{
    for (int i = 0;
        i < ((UnidentifiedReliableSocketLayout*)socket)->mConnectionCount;
        i++)
    {
        if (memcmp(((UnidentifiedReliableSocketLayout*)socket)
                           ->mConnections[i]
                           ->mAddress,
                address, 4)
            == 0)
        {
            return ((UnidentifiedReliableSocketLayout*)socket)
                ->mConnections[i];
        }
    }
    return 0;
}

extern "C" void fn_80325404(
    UnidentifiedReliableSocketState*, u32 connection)
{
    fn_8032B068(connection);
}

extern "C" void fn_8032540C(
    UnidentifiedReliableSocketState*, u32 connection)
{
    fn_8032B208(connection);
}
