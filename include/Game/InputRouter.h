#ifndef GAME_NETWORK_INPUT_ROUTER_H
#define GAME_NETWORK_INPUT_ROUTER_H

#include "Game/PackedDetInput.h"
#include "Game/NetworkSession.h"
#include "NL/CircularQueue.h"
#include "NL/nlMemory.h"
#include "types.h"
#include "Game/InputManager.h"

class InputRouter
{
public:
    void* operator new(unsigned long size)
    {
        return nlMalloc(size, 8, false);
    }

    InputRouter()
    {
        Reset(1);
    }
    virtual ~InputRouter();
    void QueueDetermData(const void* data, u32 size);
    virtual void Reset(int resetQueues);
    virtual int GetUpdateCount() = 0;
    virtual bool HasInput() = 0;
    virtual bool CanCaptureInput() = 0;
    virtual void OnInputCaptured() = 0;
    virtual void OnInputReady() = 0;
    virtual void CheckCongestion() = 0;
    virtual void ReceiveInput(
        s8 machine, NetMessageInput* message) = 0;
    virtual void ReceiveAllInputs(
        s8 machine, NetMessageAllInputs* message) = 0;
    virtual void DebugDraw(int column, int* row) = 0;
    virtual bool ProcessPlaybackFrame();
    virtual void CheckSyncMismatch();
    virtual void MarkSyncMismatchReported();

    /* 0x004 */ PackedDetInput mInputRecords[16];
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
    /* 0x15C */ StaticCircularQueue<DetermDataEvent*, 10> m_OutgoingCustomDetermDataQ;
    /* 0x194 */ NetworkSessionBase* mSession;
}; // size: 0x198

class NetworkInputMessageQueue
{
public:
    NetworkInputMessageQueue();
    ~NetworkInputMessageQueue();

    /* 0x0000 */ NetMessageInput* mMessages;
    /* 0x0004 */ u32 mHead;
    /* 0x0008 */ u32 mCount;
    /* 0x000C */ u32 mCapacity;
    /* 0x0010 */ NetMessageInput mStorage[60];
}; // size: 0x3850

class SimpleInputRouter : public InputRouter
{
public:
    SimpleInputRouter()
    {
        Reset(1);
    }

    virtual ~SimpleInputRouter();
    virtual void Reset(int resetQueues);
    virtual int GetUpdateCount();
    virtual bool HasInput();
    virtual bool CanCaptureInput();
    virtual void OnInputCaptured();
    virtual void OnInputReady();
    virtual void CheckCongestion();
    virtual void ReceiveInput(
        s8 machine, NetMessageInput* message);
    virtual void ReceiveAllInputs(
        s8 machine, NetMessageAllInputs* message);
    virtual void DebugDraw(int column, int* row);
};

class NetworkInputRouter : public InputRouter
{
public:
    NetworkInputRouter()
    {
        Reset(1);
    }
    virtual ~NetworkInputRouter();
    void CheckPeerSynchronization();
    virtual void Reset(int resetQueues);
    virtual int GetUpdateCount();
    virtual bool HasInput();
    virtual bool CanCaptureInput();
    virtual void OnInputCaptured();
    virtual void OnInputReady();
    virtual void CheckCongestion();
    virtual void ReceiveInput(
        s8 machine, NetMessageInput* message);
    virtual void ReceiveAllInputs(
        s8 machine, NetMessageAllInputs* message);
    virtual void DebugDraw(int column, int* row);

    /* 0x0198 */ bool mCongested;
    /* 0x0199 */ bool mWasCongested;
    /* 0x019A */ u8 mPadding19A[2];
    /* 0x019C */ float mCongestionMultiplier;
    /* 0x01A0 */ NetMessageInput mCurrentMessage;
    /* 0x0290 */ u8 mUnidentified290;
    /* 0x0291 */ u8 mPadding291[3];
    /* 0x0294 */ u32 mUnidentified294;
    /* 0x0298 */ NetMessageInputBundle mBundledMessage;
    /* 0x0480 */ NetworkInputMessageQueue mInputQueues[4];
    /* 0xE5C0 */ u32 mQueueCursor;
    /* 0xE5C4 */ u32 mQueueLimit;
}; // size: 0xE5C8

extern u32 gNetworkRandomSeed;

u32 GetNetworkRandomSeed();
void SetNetworkRandomSeed(u32 seed);
void OnInputSessionReset();
void InitializeInputRouters();
InputRouter* GetInputRouter();
void DispatchDetermDataEvents();

#endif // GAME_NETWORK_INPUT_ROUTER_H
