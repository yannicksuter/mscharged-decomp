#ifndef NL_PLAT_TRANSPORTCONNECTION_H
#define NL_PLAT_TRANSPORTCONNECTION_H

#include "NL/CircularQueue.h"
#include "types.h"
#include "NL/plat/TransportPacket.h"
#include "NL/plat/ReliableSocket.h"
#include "NL/plat/TransportMessage.h"

// One reliable transport connection ("Network/TransportLayer"). States:
// 1 client challenge sent, 2 client response sent, 3 server awaiting
// challenge, 4 server challenge sent, 5 server authenticated, 6 connected,
// 7 closing, 8 closed, 9 closed while awaiting acceptance.
class TransportConnection
{
public:
    enum State
    {
        STATE_0,
        STATE_1,
        STATE_2,
        STATE_3,
        STATE_4,
        STATE_5,
        STATE_6,
        STATE_7,
        STATE_8,
        STATE_9,
    };

    static void* operator new(
        unsigned long size, unsigned int alignment, bool fromEnd);
    void operator delete(void* connection);

    TransportConnection(ReliableSocket* socket,
        const u8* address, u16 port, bool outgoing);
    ~TransportConnection();

    void Update();
    void UpdateConnecting();
    int ResendNotACKed(
        TransportMessage& message, int minimumMS);
    void UpdateConnected();
    void CheckTimeouts();
    void SubmitReliable(u8 type, const void* buffer, int size);
    void SubmitUnreliable(const void* buffer, int size);
    void SubmitVoice(const void* buffer, int size);
    void HandleTransportMessage(TransportMessage* message);
    void Deliver(TransportPacket* packet);
    bool Hold(TransportPacket* packet);
    void ProcessHold();
    void SendClientChallenge();
    void HandleClientChallenge(
        TransportClientChallenge* challenge);
    void HandleServerChallenge(
        TransportServerChallenge* challenge);
    bool Accept();
    void Reject();
    void Disconnect(bool immediate);
    bool IsFinished() const;

    State GetState() const { return mState; }

    bool IsClosed()
    {
        return mState == STATE_8 || mState == STATE_9;
    }

private:
    void SetClosed();
    bool IsAckPending();
    void PrepareMessage(
        TransportMessage& message, TransportAck ack);
    void Send(TransportMessage& message);
    ReliableSocketCallback* GetCallback();

public:
    /* 0x000 */ u8 mExpectedChallengeResponse[0x20];
    /* 0x020 */ ReliableSocket* mSocket;
    /* 0x024 */ u8 mAddress[4];
    /* 0x028 */ u32 mPingSendTick;
    /* 0x02C */ u32 mPingReceiveTick;
    /* 0x030 */ u32 mPongTimestamp;
    /* 0x034 */ u32 mVoiceSendTick;
    /* 0x038 */ u32 mPendingAckTick;
    /* 0x040 */ unsigned long long mConnectStartTime;
    /* 0x048 */ u32 mClosingStartTick;
    /* 0x050 */ unsigned long long mKeepAliveSendTime;
    /* 0x058 */ unsigned long long mKeepAliveReceiveTime;
    /* 0x060 */ unsigned long long mLastResendTime;
    /* 0x068 */ int mRoundTripTimeMS;
    /* 0x06C */ int mPendingClosedCount;
    /* 0x070 */ int mCloseDelayFrames;
    /* 0x074 */ u32 mUpdateFrameCount;
    /* 0x078 */ int mOutOfOrderCount;
    /* 0x07C */ TransportPacket* mOutOfOrderPackets[30];
    /* 0x0F4 */ StaticCircularQueue<TransportPacket*, 300> m_SentNotACKedQ;
    /* 0x5B4 */ StaticCircularQueue<TransportPacket*, 20> m_OutgoingSendQ;
    /* 0x614 */ StaticCircularQueue<TransportPacket*, 20> m_OutgoingUnreliableSendQ;
    /* 0x674 */ StaticCircularQueue<TransportPacket*, 20> m_OutgoingVoiceSendQ;
    /* 0x6D4 */ StaticCircularQueue<TransportPacket*, 60> mFreeAfterSendQ;
    /* 0x7D4 */ State mState;
    /* 0x7D8 */ u16 mNextSendSequence;
    /* 0x7DA */ u16 mNextReceiveSequence;
    /* 0x7DC */ u16 mPort;
    /* 0x7DE */ bool mIncoming;
    /* 0x7DF */ bool mSocketError;
    /* 0x7E0 */ bool mOutOfMemory;
}; // size: 0x7E8

#endif // NL_PLAT_TRANSPORTCONNECTION_H
