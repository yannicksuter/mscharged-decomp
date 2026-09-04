#ifndef UNCLASSIFIED_TU_80332DC0_H
#define UNCLASSIFIED_TU_80332DC0_H

#include <string.h>

#include "Game/NetworkSession.h"
#include "NL/nlMemory.h"
#include "types.h"
#include "unclassified/tu_80332770.h"

struct InputRouterRecord
{
    u8 mData[0x10];
};

class UnidentifiedInputRouter
{
public:
    void* operator new(unsigned long size)
    {
        return nlMalloc(size, 8, false);
    }

    UnidentifiedInputRouter()
        : mOutgoingDetermData(mOutgoingDetermDataStorage)
        , mOutgoingHead(0)
        , mOutgoingCount(0)
        , mOutgoingCapacity(10)
    {
        for (int i = 0; i < 16; ++i)
        {
            memset(&mInputRecords[i], 0, sizeof(InputRouterRecord));
        }
        Reset(1);
    }
    virtual ~UnidentifiedInputRouter();
    virtual void Reset(int resetQueues);
    virtual int RouterVirtual10() = 0;
    virtual bool RouterVirtual14() = 0;
    virtual bool RouterVirtual18() = 0;
    virtual void RouterVirtual1C() = 0;
    virtual void RouterVirtual20() = 0;
    virtual void RouterVirtual24() = 0;
    virtual void RouterVirtual28(
        s8 machine, NetworkMessageType0_80533B7C* message) = 0;
    virtual void RouterVirtual2C(
        s8 machine, NetworkMessageType8_80533BA4* message) = 0;
    virtual void RouterVirtual30() = 0;
    virtual bool RouterVirtual34();
    virtual void RouterVirtual38();
    virtual void RouterVirtual3C();

    /* 0x004 */ InputRouterRecord mInputRecords[16];
    /* 0x104 */ u8 mInputStates[16];
    /* 0x114 */ u16 mNetworkTicks[4];
    /* 0x11C */ u32 mNetworkCRCs[4];
    /* 0x12C */ u32 mRemoteTicks[4];
    /* 0x13C */ u32 mRandomSeeds[4];
    /* 0x14C */ bool mSyncMismatch;
    /* 0x14D */ bool mSyncMismatchReported;
    /* 0x14E */ bool mOutgoingQueueOverflowed;
    /* 0x14F */ bool mStarvedForInput;
    /* 0x150 */ u32 mCurrentCRC;
    /* 0x154 */ s32 mLastGameFrame;
    /* 0x158 */ u32 mPadding158;
    /* 0x15C */ DetermDataEvent** mOutgoingDetermData;
    /* 0x160 */ u32 mOutgoingHead;
    /* 0x164 */ u32 mOutgoingCount;
    /* 0x168 */ u32 mOutgoingCapacity;
    /* 0x16C */ DetermDataEvent* mOutgoingDetermDataStorage[10];
    /* 0x194 */ UnidentifiedNetworkSession* mSession;
}; // size: 0x198

class NetworkInputMessageQueue
{
public:
    NetworkInputMessageQueue();
    ~NetworkInputMessageQueue();

    /* 0x0000 */ NetworkMessageType0_80533B7C* mMessages;
    /* 0x0004 */ u32 mHead;
    /* 0x0008 */ u32 mCount;
    /* 0x000C */ u32 mCapacity;
    /* 0x0010 */ NetworkMessageType0_80533B7C mStorage[60];
}; // size: 0x3850

class SimpleInputRouter : public UnidentifiedInputRouter
{
public:
    SimpleInputRouter()
    {
        Reset(1);
    }

    virtual ~SimpleInputRouter();
    virtual void Reset(int resetQueues);
    virtual int RouterVirtual10();
    virtual bool RouterVirtual14();
    virtual bool RouterVirtual18();
    virtual void RouterVirtual1C();
    virtual void RouterVirtual20();
    virtual void RouterVirtual24();
    virtual void RouterVirtual28(
        s8 machine, NetworkMessageType0_80533B7C* message);
    virtual void RouterVirtual2C(
        s8 machine, NetworkMessageType8_80533BA4* message);
    virtual void RouterVirtual30();
};

class NetworkInputRouter : public UnidentifiedInputRouter
{
public:
    NetworkInputRouter()
    {
        Reset(1);
    }
    virtual ~NetworkInputRouter();
    virtual void Reset(int resetQueues);
    virtual int RouterVirtual10();
    virtual bool RouterVirtual14();
    virtual bool RouterVirtual18();
    virtual void RouterVirtual1C();
    virtual void RouterVirtual20();
    virtual void RouterVirtual24();
    virtual void RouterVirtual28(
        s8 machine, NetworkMessageType0_80533B7C* message);
    virtual void RouterVirtual2C(
        s8 machine, NetworkMessageType8_80533BA4* message);
    virtual void RouterVirtual30();

    /* 0x0198 */ bool mCongested;
    /* 0x0199 */ bool mWasCongested;
    /* 0x019A */ u8 mPadding19A[2];
    /* 0x019C */ float mCongestionMultiplier;
    /* 0x01A0 */ NetworkMessageType0_80533B7C mCurrentMessage;
    /* 0x0290 */ u8 mUnidentified290;
    /* 0x0291 */ u8 mPadding291[3];
    /* 0x0294 */ u32 mUnidentified294;
    /* 0x0298 */ NetworkMessageType1_80533B68 mBundledMessage;
    /* 0x0480 */ NetworkInputMessageQueue mInputQueues[4];
    /* 0xE5C0 */ u32 mQueueCursor;
    /* 0xE5C4 */ u32 mQueueLimit;
}; // size: 0xE5C8

extern u32 gNetworkRandomSeed;

extern "C" u32 fn_80332EB8();
extern "C" void fn_80332EC0(u32 seed);
extern "C" void fn_80332EC8();
extern "C" void fn_80332EDC();
extern "C" UnidentifiedInputRouter* fn_803330AC();
extern "C" void fn_80333908(
    UnidentifiedInputRouter* router, const void* data, u32 size);
extern "C" void fn_80333A18();

#endif // UNCLASSIFIED_TU_80332DC0_H
