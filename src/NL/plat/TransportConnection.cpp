#include "NL/plat/TransportConnection.h"
#include "Game/Sys/debug.h"

#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTicker.h"
#include "NL/nlTime.h"

#include <string.h>

SlotPool<TransportConnection> gTransportConnectionPool(16, 0);

void* TransportConnection::operator new(
    unsigned long, unsigned int, bool)
{
    return gTransportConnectionPool.Allocate();
}

inline void TransportConnection::operator delete(void* connection)
{
    gTransportConnectionPool.Free((TransportConnection*)connection);
}

// Sequence numbers wrap at 16 bits; a is after b when the forward distance
// from b to a is the shorter way around.
static inline bool IsSequenceAfter(u16 a, u16 b)
{
    if (a == b)
    {
        return false;
    }
    return (u16)(b - a) > (u16)(a - b);
}

static inline bool IsSequenceBefore(u16 a, u16 b)
{
    if (a == b)
    {
        return false;
    }
    return (u16)(b - a) < (u16)(a - b);
}

static inline bool ContainsReliableSequence(
    TransportMessage& message, u16 sequence)
{
    for (int i = 0; i < message.GetReliableCount(); i++)
    {
        TransportPacket* packet = message.GetReliablePacket(i);
        if (packet->mSequence == sequence)
        {
            return true;
        }
    }
    return false;
}

inline ReliableSocketCallback*
TransportConnection::GetCallback()
{
    return mSocket->mCallback;
}

inline void TransportConnection::SetClosed()
{
    if (!IsClosed())
    {
        if (mState == STATE_5)
        {
            mState = STATE_9;
        }
        else
        {
            mState = STATE_8;
        }
    }
}

// Connection failure: log, drop into a closed state, and notify the socket
// callback. Expanded at each site (the format literal lives at its first use).
#define TRANSPORT_CONNECTION_ERROR(result, reason)                        \
    do                                                                         \
    {                                                                          \
        int nResult = (result);                                                \
        int nReason = (reason);                                                \
        tDebugPrintManager::Print(DC_NETWORK, "Connection Error result %d reason %d\n", nResult,  \
            nReason);                                                          \
        if (mState < STATE_6)                                              \
        {                                                                      \
            SetClosed();                                           \
            if (!mIncoming)                                             \
            {                                                                  \
                GetCallback()->OnConnectionAttempted(              \
                    (u32)this, nResult);                                       \
            }                                                                  \
        }                                                                      \
        else if (!IsClosed())                                      \
        {                                                                      \
            SetClosed();                                           \
            GetCallback()->OnConnectionClosed((u32)this, nReason); \
        }                                                                      \
    } while (0)

inline bool TransportConnection::IsAckPending()
{
    if (mPendingAckTick != 0
        && (int)nlGetTickerDifference(mPendingAckTick, nlGetTicker())
            > s_nSendPendingAckMS)
    {
        return true;
    }
    return false;
}

inline void TransportConnection::PrepareMessage(
    TransportMessage& message, TransportAck ack)
{
    message.SetAck(ack);
    mPendingAckTick = 0;
    if (mPingSendTick == 0
        || (int)nlGetTickerDifference(mPingSendTick, nlGetTicker())
            > s_nSendPingMS)
    {
        mPingSendTick = message.AddPing();
    }
    if (mPingReceiveTick != 0)
    {
        message.AddPong(mPongTimestamp,
            (int)nlGetTickerDifference(mPingReceiveTick, nlGetTicker()));
        mPingReceiveTick = 0;
    }
    if (mPendingClosedCount > 0)
    {
        message.SetClosed();
        mPendingClosedCount--;
    }
}

inline void TransportConnection::Send(
    TransportMessage& message)
{
    mSocket->SendMessage(&message, mAddress, mPort, &mSocketError);
}

TransportConnection::TransportConnection(
    ReliableSocket* socket, const u8* address, u16 port,
    bool outgoing)
    : mSocket(socket)
    , mPingSendTick(0)
    , mPingReceiveTick(0)
    , mPongTimestamp(0)
    , mPendingAckTick(0)
    , mClosingStartTick(0)
    , mPendingClosedCount(0)
    , mCloseDelayFrames(5)
    , mUpdateFrameCount(0)
    , mOutOfOrderCount(0)
    , mState(STATE_0)
    , mNextSendSequence(0)
    , mNextReceiveSequence(0)
    , mPort(port)
    , mIncoming(false)
    , mSocketError(false)
    , mOutOfMemory(false)
{
    mRoundTripTimeMS = 200;
    mVoiceSendTick = nlGetTicker();
    unsigned long long now = nlGetTime();
    mLastResendTime = now;
    mConnectStartTime = now;
    mKeepAliveReceiveTime = now;
    mKeepAliveSendTime = now;
    *(u32*)mAddress = *(const u32*)address;
    memset(mExpectedChallengeResponse, 0, sizeof(mExpectedChallengeResponse));
    if (outgoing)
    {
        mIncoming = false;
        mState = STATE_1;
    }
    else
    {
        mState = STATE_3;
        mIncoming = true;
    }
}

TransportConnection::~TransportConnection()
{
    for (int i = 0; i < mOutOfOrderCount; i++)
    {
        if (mOutOfOrderPackets[i] != 0)
        {
            delete mOutOfOrderPackets[i];
            mOutOfOrderPackets[i] = 0;
        }
    }
    mOutOfOrderCount = 0;
    while (m_SentNotACKedQ.GetCount() > 0)
    {
        delete m_SentNotACKedQ.Pop();
    }
    while (m_OutgoingSendQ.GetCount() > 0)
    {
        delete m_OutgoingSendQ.Pop();
    }
    while (m_OutgoingUnreliableSendQ.GetCount() > 0)
    {
        delete m_OutgoingUnreliableSendQ.Pop();
    }
    while (m_OutgoingVoiceSendQ.GetCount() > 0)
    {
        delete m_OutgoingVoiceSendQ.Pop();
    }
    while (mFreeAfterSendQ.GetCount() > 0)
    {
        delete mFreeAfterSendQ.Pop();
    }
}

void TransportConnection::Update()
{
    mUpdateFrameCount++;
    if (mUpdateFrameCount % s_nSendEveryNthFrame != 0)
    {
        return;
    }
    if (mState == STATE_8 && mCloseDelayFrames > 0)
    {
        mCloseDelayFrames--;
    }
    if (mState < STATE_6)
    {
        UpdateConnecting();
    }
    else if (mState == STATE_6 || mState == STATE_7)
    {
        UpdateConnected();
    }
    if (!IsClosed())
    {
        if (IsAckPending())
        {
            TransportMessage message;
            PrepareMessage(message, mNextReceiveSequence);
            if (g_TransportLayerLog >= 2)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Explicitly sent a pending ACK by itself\n");
            }
            Send(message);
        }
    }
    CheckTimeouts();
    if (mPendingClosedCount > 0)
    {
        TransportMessage message;
        PrepareMessage(message, mNextReceiveSequence);
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Explicitly sent a Unreliable CLOSED\n");
        }
        Send(message);
    }
}

void TransportConnection::UpdateConnecting()
{
    while (m_OutgoingSendQ.GetCount() > 0)
    {
        TransportMessage message;
        PrepareMessage(message, mNextReceiveSequence);
        TransportPacket* packet = m_OutgoingSendQ.Pop();
        packet->mLastSendTick = nlGetTicker();
        message.AddReliablePacket(packet);
        Send(message);
        if (!m_SentNotACKedQ.IsFull())
        {
            m_SentNotACKedQ.Push(packet);
        }
        else
        {
            mOutOfMemory = true;
            delete packet;
            if (g_TransportLayerLog >= 1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Not yet connected m_SentNotACKedQ out of space, will close connection\n");
            }
        }
    }
    if (m_SentNotACKedQ.GetCount() > 0)
    {
        u32 now = nlGetTicker();
        int count = m_SentNotACKedQ.GetCount();
        for (int i = 0; i < count; i++)
        {
            TransportPacket* packet = m_SentNotACKedQ[i];
            if ((int)nlGetTickerDifference(packet->mLastSendTick, now)
                > s_nResendNormalMS)
            {
                TransportMessage message;
                PrepareMessage(message, mNextReceiveSequence);
                message.AddReliablePacket(packet);
                packet->mLastSendTick = nlGetTicker();
                Send(message);
            }
        }
    }
}

int TransportConnection::ResendNotACKed(
    TransportMessage& message, int minimumMS)
{
    if (m_SentNotACKedQ.GetCount() > 0
        && (int)nlGetTimeDifference(mLastResendTime, nlGetTime()) > minimumMS)
    {
        int limit = s_nResendNormalMS;
        if (s_nResendNormalAggressive
            && mRoundTripTimeMS + s_nSendPendingAckMS < limit)
        {
            limit = mRoundTripTimeMS + s_nSendPendingAckMS;
        }
        u32 now = nlGetTicker();
        bool any = false;
        int count = m_SentNotACKedQ.GetCount();
        for (int i = 0; i < count; i++)
        {
            TransportPacket* packet = m_SentNotACKedQ[i];
            bool resend
                = (int)nlGetTickerDifference(packet->mLastSendTick, now) > limit
                && !ContainsReliableSequence(message, packet->mSequence);
            if (resend)
            {
                any = true;
                break;
            }
        }
        if (any)
        {
            int resent = 0;
            for (int i = 0; i < count; i++)
            {
                TransportPacket* packet
                    = m_SentNotACKedQ[i];
                bool resend
                    = (int)nlGetTickerDifference(packet->mLastSendTick, now)
                        > limit
                    && !ContainsReliableSequence(
                        message, packet->mSequence);
                if (resend)
                {
                    if (!message.AddReliablePacket(packet))
                    {
                        break;
                    }
                    packet->mLastSendTick = nlGetTicker();
                    resent++;
                }
            }
            mLastResendTime = nlGetTime();
            return resent;
        }
    }
    return 0;
}

void TransportConnection::UpdateConnected()
{
    unsigned long long now = nlGetTime();
    int elapsed = (int)nlGetTimeDifference(mKeepAliveSendTime, now);
    if (elapsed > s_nSendKeepAliveMS)
    {
        u8 buffer[50];
        NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
        TransportKeepAlive payload;
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.GetLength();
        SubmitReliable(0xE5, data, length);
        if (g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Submitted Keep Alive For Send after %d ms conn %d.%d\n",
                elapsed, mAddress[2], mAddress[3]);
        }
        mKeepAliveSendTime = now;
    }

    if (m_OutgoingSendQ.GetCount() > 0)
    {
        TransportMessage message;
        PrepareMessage(message, mNextReceiveSequence);
        int index = m_SentNotACKedQ.GetCount() - sPayloadRedundancySteps[0];
        while (m_OutgoingSendQ.GetCount() > 0)
        {
            TransportPacket* packet = m_OutgoingSendQ.Peek();
            if (!message.AddReliablePacket(packet))
            {
                break;
            }
            packet->mLastSendTick = nlGetTicker();
            m_OutgoingSendQ.Pop();
            if (!m_SentNotACKedQ.IsFull())
            {
                m_SentNotACKedQ.Push(packet);
            }
            else
            {
                mOutOfMemory = true;
                mFreeAfterSendQ.Push(packet);
                if (g_TransportLayerLog >= 1)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "m_SentNotACKedQ out of space, will close connection\n");
                }
            }
        }
        while (m_OutgoingUnreliableSendQ.GetCount() > 0)
        {
            TransportPacket* packet
                = m_OutgoingUnreliableSendQ.Peek();
            if (!message.AddUnreliablePacket(packet))
            {
                break;
            }
            m_OutgoingUnreliableSendQ.Pop();
            mFreeAfterSendQ.Push(packet);
        }
        int remaining = s_nPayloadRedundancy;
        int i = 1;
        while (index >= 0 && remaining > 0)
        {
            if (!message.AddReliablePacket(m_SentNotACKedQ[index]))
            {
                break;
            }
            index -= sPayloadRedundancySteps[i++];
            remaining--;
        }
        while (m_OutgoingVoiceSendQ.GetCount() > 0)
        {
            TransportPacket* packet
                = m_OutgoingVoiceSendQ.Peek();
            if (!message.AddVoicePacket(packet))
            {
                break;
            }
            m_OutgoingVoiceSendQ.Pop();
            mFreeAfterSendQ.Push(packet);
        }
        int resent = ResendNotACKed(message, 0);
        if (resent > 0 && g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Piggyback resent %d not ACKed messages\n", resent);
        }
        Send(message);
        if (message.HasVoice())
        {
            mVoiceSendTick = nlGetTicker();
        }
        while (mFreeAfterSendQ.GetCount() > 0)
        {
            delete mFreeAfterSendQ.Pop();
        }
    }

    if (m_SentNotACKedQ.GetCount() > 0)
    {
        TransportMessage message;
        int resent = ResendNotACKed(message, s_nResendGroupMS);
        if (resent > 0)
        {
            PrepareMessage(message, mNextReceiveSequence);
            if (g_TransportLayerLog >= 2)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Resent %d not ACKed messages\n", resent);
            }
            while (m_OutgoingUnreliableSendQ.GetCount() > 0)
            {
                TransportPacket* packet
                    = m_OutgoingUnreliableSendQ.Peek();
                if (!message.AddUnreliablePacket(packet))
                {
                    break;
                }
                m_OutgoingUnreliableSendQ.Pop();
                mFreeAfterSendQ.Push(packet);
            }
            while (m_OutgoingVoiceSendQ.GetCount() > 0)
            {
                TransportPacket* packet
                    = m_OutgoingVoiceSendQ.Peek();
                if (!message.AddVoicePacket(packet))
                {
                    break;
                }
                m_OutgoingVoiceSendQ.Pop();
                mFreeAfterSendQ.Push(packet);
            }
            Send(message);
            if (message.HasVoice())
            {
                mVoiceSendTick = nlGetTicker();
            }
            while (mFreeAfterSendQ.GetCount() > 0)
            {
                delete mFreeAfterSendQ.Pop();
            }
        }
    }

    if (m_OutgoingUnreliableSendQ.GetCount() > 0)
    {
        TransportMessage message;
        PrepareMessage(message, mNextReceiveSequence);
        while (m_OutgoingUnreliableSendQ.GetCount() > 0)
        {
            TransportPacket* packet
                = m_OutgoingUnreliableSendQ.Peek();
            if (!message.AddUnreliablePacket(packet))
            {
                break;
            }
            m_OutgoingUnreliableSendQ.Pop();
            mFreeAfterSendQ.Push(packet);
        }
        while (m_OutgoingVoiceSendQ.GetCount() > 0)
        {
            TransportPacket* packet
                = m_OutgoingVoiceSendQ.Peek();
            if (!message.AddVoicePacket(packet))
            {
                break;
            }
            m_OutgoingVoiceSendQ.Pop();
            mFreeAfterSendQ.Push(packet);
        }
        Send(message);
        if (message.HasVoice())
        {
            mVoiceSendTick = nlGetTicker();
        }
        while (mFreeAfterSendQ.GetCount() > 0)
        {
            delete mFreeAfterSendQ.Pop();
        }
    }

    if (m_OutgoingVoiceSendQ.GetCount() > 0
        && (m_OutgoingVoiceSendQ.GetCount() >= s_nSendVoiceHighwaterNum
            || (int)nlGetTickerDifference(mVoiceSendTick, nlGetTicker())
                > s_nSendVoiceMS))
    {
        TransportMessage message;
        PrepareMessage(message, mNextReceiveSequence);
        while (m_OutgoingVoiceSendQ.GetCount() > 0)
        {
            TransportPacket* packet
                = m_OutgoingVoiceSendQ.Peek();
            if (!message.AddVoicePacket(packet))
            {
                break;
            }
            m_OutgoingVoiceSendQ.Pop();
            mFreeAfterSendQ.Push(packet);
        }
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Explicitly sent %d Voice packets\n",
                message.GetVoiceCount());
        }
        Send(message);
        mVoiceSendTick = nlGetTicker();
        while (mFreeAfterSendQ.GetCount() > 0)
        {
            delete mFreeAfterSendQ.Pop();
        }
    }
}

void TransportConnection::CheckTimeouts()
{
    if (mState < STATE_6)
    {
        bool timedOut = false;
        unsigned long long now = nlGetTime();
        if (!mIncoming)
        {
            if ((int)nlGetTimeDifference(mConnectStartTime, now)
                > s_nConnectClientTimeoutMS)
            {
                timedOut = true;
            }
        }
        else if (mState < STATE_5)
        {
            if ((int)nlGetTimeDifference(mConnectStartTime, now)
                > s_nConnectServerTimeoutMS)
            {
                timedOut = true;
            }
        }
        if (timedOut)
        {
            mPendingClosedCount += 3;
            TRANSPORT_CONNECTION_ERROR(6, 0);
        }
        else if (mSocketError)
        {
            TRANSPORT_CONNECTION_ERROR(3, 3);
        }
        else if (mOutOfMemory)
        {
            TRANSPORT_CONNECTION_ERROR(3, 4);
        }
    }
    else
    {
        switch (mState)
        {
        case STATE_6:
        {
            int elapsed = (int)nlGetTimeDifference(mKeepAliveReceiveTime, nlGetTime());
            if (s_bExpireKeepAliveEnabled && elapsed > s_nExpireKeepAliveMS)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Keep alive not received for %d MS, closing connection\n",
                    elapsed);
                TRANSPORT_CONNECTION_ERROR(6, 0);
            }
            else if (mSocketError)
            {
                TRANSPORT_CONNECTION_ERROR(3, 3);
            }
            else if (mOutOfMemory)
            {
                TRANSPORT_CONNECTION_ERROR(3, 4);
            }
            break;
        }
        case STATE_7:
        {
            if ((int)nlGetTickerDifference(mClosingStartTick, nlGetTicker())
                > s_nClosingTimeoutMS)
            {
                TRANSPORT_CONNECTION_ERROR(6, 0);
            }
            else if (mSocketError)
            {
                TRANSPORT_CONNECTION_ERROR(3, 3);
            }
            else if (mOutOfMemory)
            {
                TRANSPORT_CONNECTION_ERROR(3, 4);
            }
            break;
        }
        }
    }
}

void TransportConnection::SubmitReliable(
    u8 type, const void* buffer, int size)
{
    TransportPacket* packet
        = new (8, false) TransportPacket;
    packet->mLastSendTick = 0;
    packet->mType = type;
    packet->mSequence = mNextSendSequence;
    packet->mSize = size;
    memcpy(packet->mPayload, buffer, size);
    mNextSendSequence++;
    if (!m_OutgoingSendQ.IsFull())
    {
        m_OutgoingSendQ.Push(packet);
    }
    else
    {
        mOutOfMemory = true;
        delete packet;
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "m_OutgoingSendQ out of space, will close connection\n");
        }
    }
}

void TransportConnection::SubmitUnreliable(
    const void* buffer, int size)
{
    TransportPacket* packet
        = new (8, false) TransportPacket;
    packet->mLastSendTick = 0;
    packet->mType = 0xE6;
    packet->mSequence = 0;
    packet->mSize = size;
    memcpy(packet->mPayload, buffer, size);
    if (!m_OutgoingUnreliableSendQ.IsFull())
    {
        m_OutgoingUnreliableSendQ.Push(packet);
    }
    else
    {
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "m_OutgoingUnreliableSendQ out of space, discarding unreliable send payload\n");
        }
        delete packet;
    }
}

void TransportConnection::SubmitVoice(
    const void* buffer, int size)
{
    TransportPacket* packet
        = new (8, false) TransportPacket;
    packet->mLastSendTick = 0;
    packet->mType = 0xE7;
    packet->mSequence = 0;
    packet->mSize = size;
    memcpy(packet->mPayload, buffer, size);
    if (!m_OutgoingVoiceSendQ.IsFull())
    {
        m_OutgoingVoiceSendQ.Push(packet);
    }
    else
    {
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "m_OutgoingVoiceSendQ out of space, discarding payload\n");
        }
        delete packet;
    }
}

void TransportConnection::HandleTransportMessage(
    TransportMessage* message)
{
    if (g_TransportLayerLog >= 3)
    {
        tDebugPrintManager::Print(DC_NETWORK, "HandleTransportMessage: ");
    }
    if (message->HasAck())
    {
        u16 ack = message->GetAck().mSequence;
        if (g_TransportLayerLog >= 3)
        {
            tDebugPrintManager::Print(DC_NETWORK, "ACK %d ", ack);
        }
        for (;;)
        {
            if (m_SentNotACKedQ.GetCount() == 0)
            {
                break;
            }
            TransportPacket* packet = m_SentNotACKedQ.Peek();
            if (!IsSequenceBefore(packet->mSequence, ack))
            {
                break;
            }
            delete m_SentNotACKedQ.Pop();
        }
    }
    message->HasControlFlag80();
    if (message->HasPing())
    {
        mPingReceiveTick = nlGetTicker();
        mPongTimestamp = message->GetPingTimestamp();
    }
    if (message->HasPong())
    {
        u16 other = 0;
        u32 sent = message->GetPongTimestamp(&other);
        u32 latency = (int)nlGetTickerDifference(sent, nlGetTicker());
        latency -= other;
        if (latency > 10000)
        {
            latency = 10000;
        }
        if (g_TransportLayerLog >= 3)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Received PONG 2-way latency = %d, processing on other end time = %d\n",
                latency, other);
        }
        mRoundTripTimeMS = latency;
    }

    u8 count = message->GetUnreliableCount();
    if (count != 0)
    {
        for (u8 i = 0; i < count; i++)
        {
            TransportPacket* packet = message->GetUnreliablePacket(i);
            GetCallback()->OnMessageReceived(
                (u32)this, packet->mPayload, packet->mSize, false);
            delete packet;
        }
    }
    count = message->GetVoiceCount();
    if (count != 0)
    {
        for (u8 i = 0; i < count; i++)
        {
            TransportPacket* packet = message->GetVoicePacket(i);
            GetCallback()->ReliableCallbackVirtual10(
                (u32)this, packet->mPayload, packet->mSize);
            delete packet;
        }
    }
    count = message->GetReliableCount();
    for (u8 i = 0; i < count; i++)
    {
        TransportPacket* packet = message->GetReliablePacket(i);
        if (g_TransportLayerLog >= 3)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Payload %d SN %d ", i, packet->mSequence);
        }
        if (packet->mSequence == mNextReceiveSequence)
        {
            Deliver(packet);
            if (g_TransportLayerLog >= 3)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Delivered payload type %d size %d",
                    packet->mType, packet->mSize);
            }
            delete packet;
            mNextReceiveSequence++;
            if (mPendingAckTick == 0)
            {
                mPendingAckTick = nlGetTicker();
            }
            ProcessHold();
        }
        else if (IsSequenceAfter(packet->mSequence, mNextReceiveSequence))
        {
            if (g_TransportLayerLog >= 3)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Out Of Order ");
            }
            if (!Hold(packet))
            {
                delete packet;
            }
        }
        else
        {
            if (g_TransportLayerLog >= 3)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Discarded duplicate ");
            }
            delete packet;
        }
    }
    if (g_TransportLayerLog >= 3)
    {
        tDebugPrintManager::Print(DC_NETWORK, "END\n");
    }

    if (message->IsClosed())
    {
        if (!IsClosed())
        {
            tDebugPrintManager::Print(DC_NETWORK, "Received unreliable CLOSED message\n");
            TRANSPORT_CONNECTION_ERROR(2, mState != STATE_7);
        }
    }
}

void TransportConnection::Deliver(
    TransportPacket* packet)
{
    if (packet->mType == 0)
    {
        GetCallback()->OnMessageReceived(
            (u32)this, packet->mPayload, packet->mSize, true);
        return;
    }

    NetworkMessageSerializer serializer(0, packet->mPayload, packet->mSize);
    unsigned long long now;
    switch (packet->mType)
    {
    case 0xE0:
    {
        TransportClientChallenge payload;
        payload.Serialize(&serializer);
        HandleClientChallenge(&payload);
        break;
    }
    case 0xE1:
    {
        TransportServerChallenge payload;
        payload.Serialize(&serializer);
        HandleServerChallenge(&payload);
        break;
    }
    case 0xE2:
    {
        TransportClientResponse payload;
        payload.Serialize(&serializer);
        tDebugPrintManager::Print(DC_NETWORK, "Received Client Response Message\n");
        if (mState != STATE_4)
        {
            TRANSPORT_CONNECTION_ERROR(7, 2);
        }
        else if (!CompareTransportChallenge(mExpectedChallengeResponse, payload.mPayload))
        {
            TRANSPORT_CONNECTION_ERROR(7, 2);
        }
        else
        {
            mState = STATE_5;
            GetCallback()->OnConnectionRequest(
                (u32)this, mAddress, 0, 0, 0);
        }
        break;
    }
    case 0xE3:
    {
        TransportServerResponse payload;
        payload.Serialize(&serializer);
        if (mState != STATE_2)
        {
            TRANSPORT_CONNECTION_ERROR(7, 2);
        }
        else if (payload.mAccepted)
        {
            mState = STATE_6;
            GetCallback()->OnConnectionAttempted((u32)this, 0);
        }
        else
        {
            mPendingClosedCount += 3;
            SetClosed();
            GetCallback()->OnConnectionAttempted((u32)this, 2);
        }
        break;
    }
    case 0xE4:
    {
        TransportClosing payload;
        payload.Serialize(&serializer);
        tDebugPrintManager::Print(DC_NETWORK, "Received closing message\n");
        mPendingClosedCount += 3;
        TRANSPORT_CONNECTION_ERROR(2, mState != STATE_7);
        break;
    }
    case 0xE5:
    {
        TransportKeepAlive payload;
        payload.Serialize(&serializer);
        now = nlGetTime();
        if (g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Received keep alive message after %d MS conn %d.%d\n",
                (int)nlGetTimeDifference(mKeepAliveReceiveTime, now), mAddress[2],
                mAddress[3]);
        }
        mKeepAliveReceiveTime = now;
        break;
    }
    }
}

bool TransportConnection::Hold(
    TransportPacket* packet)
{
    int count = mOutOfOrderCount;
    if (count >= 30)
    {
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded Out of Order packet .. no room in out of order hold\n");
        }
        return false;
    }
    if (count == 0)
    {
        mOutOfOrderPackets[0] = packet;
        mOutOfOrderCount = 1;
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Placed payload SN %d in hold\n", packet->mSequence);
        }
        return true;
    }
    if (count == 1)
    {
        TransportPacket* held = mOutOfOrderPackets[0];
        u16 packetSequence = packet->mSequence;
        if (packetSequence == held->mSequence)
        {
            return false;
        }
        if (IsSequenceBefore(packetSequence, held->mSequence))
        {
            mOutOfOrderPackets[1] = held;
            mOutOfOrderPackets[0] = packet;
        }
        else
        {
            mOutOfOrderPackets[1] = packet;
        }
        mOutOfOrderCount = 2;
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Placed 2nd payload SN %d in hold\n",
                packet->mSequence);
        }
        return true;
    }

    int last = mOutOfOrderCount - 1;
    int high = last;
    int low = 0;
    int middle = -1;
    while (high >= low)
    {
        middle = (low + high) >> 1;
        TransportPacket* held = mOutOfOrderPackets[middle];
        u16 heldSequence;
        u16 packetSequence;
        packetSequence = packet->mSequence;
        heldSequence = held->mSequence;
        if (IsSequenceAfter(packetSequence, heldSequence))
        {
            low = middle + 1;
        }
        else if (IsSequenceBefore(packetSequence, heldSequence))
        {
            high = middle - 1;
        }
        else
        {
            break;
        }
    }
    TransportPacket* held = mOutOfOrderPackets[middle];
    if (packet->mSequence == held->mSequence)
    {
        return false;
    }
    if (IsSequenceAfter(packet->mSequence, held->mSequence))
    {
        for (int i = mOutOfOrderCount - 1; i >= middle + 1; i--)
        {
            mOutOfOrderPackets[i + 1] = mOutOfOrderPackets[i];
        }
        mOutOfOrderPackets[middle + 1] = packet;
        mOutOfOrderCount++;
    }
    else
    {
        for (int i = mOutOfOrderCount - 1; i >= middle; i--)
        {
            mOutOfOrderPackets[i + 1] = mOutOfOrderPackets[i];
        }
        mOutOfOrderPackets[middle] = packet;
        mOutOfOrderCount++;
    }
    if (g_TransportLayerLog >= 1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Placed %dth payload SN %d in hold\n",
            mOutOfOrderCount + 1, packet->mSequence);
    }
    return true;
}

void TransportConnection::ProcessHold()
{
    int delivered = 0;
    while (delivered < mOutOfOrderCount)
    {
        if (mOutOfOrderPackets[delivered]->mSequence != mNextReceiveSequence)
        {
            break;
        }
        Deliver(mOutOfOrderPackets[delivered]);
        if (g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Delivered OUT OF ORDER payload type %d size %d",
                mOutOfOrderPackets[delivered]->mType,
                mOutOfOrderPackets[delivered]->mSize);
        }
        delete mOutOfOrderPackets[delivered];
        mNextReceiveSequence++;
        if (mPendingAckTick == 0)
        {
            mPendingAckTick = nlGetTicker();
        }
        delivered++;
    }
    if (delivered != 0)
    {
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Moved Out Of Order Hold up %d spots\n", delivered);
        }
        for (int i = delivered; i < mOutOfOrderCount; i++)
        {
            mOutOfOrderPackets[i - delivered] = mOutOfOrderPackets[i];
        }
        mOutOfOrderCount -= delivered;
    }
}

void TransportConnection::SendClientChallenge()
{
    tDebugPrintManager::Print(DC_NETWORK, "Sending Client Challenge\n");
    u8 buffer[200];
    NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
    TransportClientChallenge payload;
    GenerateTransportChallenge((u32*)payload.mPayload);
    EncodeTransportChallenge(mExpectedChallengeResponse, payload.mPayload);
    payload.Serialize(&serializer);
    u8* data = serializer.mBuffer;
    int length = serializer.GetLength();
    SubmitReliable(0xE0, data, length);
    mState = STATE_1;
}

void TransportConnection::HandleClientChallenge(
    TransportClientChallenge* challenge)
{
    tDebugPrintManager::Print(DC_NETWORK, "Received Client Challenge Message\n");
    mSocket->mScreenPrinter.Print("Received Client Challenge Message\n");
    if (mState != STATE_3)
    {
        TRANSPORT_CONNECTION_ERROR(7, 2);
    }
    else
    {
        TransportServerChallenge payload;
        EncodeTransportChallenge(payload.mClientResponse, challenge->mPayload);
        GenerateTransportChallenge((u32*)payload.mChallenge);
        EncodeTransportChallenge(mExpectedChallengeResponse, payload.mChallenge);
        u8 buffer[200];
        NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.GetLength();
        SubmitReliable(0xE1, data, length);
        mState = STATE_4;
    }
}

void TransportConnection::HandleServerChallenge(
    TransportServerChallenge* challenge)
{
    tDebugPrintManager::Print(DC_NETWORK, "Received Server Challenge Message\n");
    mSocket->mScreenPrinter.Print("Received Server Challenge Message");
    if (mState != STATE_1)
    {
        TRANSPORT_CONNECTION_ERROR(7, 2);
    }
    else if (!CompareTransportChallenge(mExpectedChallengeResponse, challenge->mClientResponse))
    {
        TRANSPORT_CONNECTION_ERROR(7, 2);
    }
    else
    {
        TransportClientResponse payload;
        EncodeTransportChallenge(payload.mPayload, challenge->mChallenge);
        u8 buffer[200];
        NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.GetLength();
        SubmitReliable(0xE2, data, length);
        mState = STATE_2;
    }
}

bool TransportConnection::Accept()
{
    if (mState == STATE_9)
    {
        mState = STATE_8;
        return false;
    }
    if (mState != STATE_5)
    {
        return false;
    }
    TransportServerResponse payload(true);
    u8 buffer[50];
    NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
    payload.Serialize(&serializer);
    u8* data = serializer.mBuffer;
    int length = serializer.GetLength();
    SubmitReliable(0xE3, data, length);
    mState = STATE_6;
    return true;
}

void TransportConnection::Reject()
{
    if (mState == STATE_9)
    {
        mState = STATE_8;
        return;
    }
    if (mState == STATE_5)
    {
        TransportServerResponse payload;
        u8 buffer[50];
        NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.GetLength();
        SubmitReliable(0xE3, data, length);
        mState = STATE_7;
        mClosingStartTick = nlGetTicker();
    }
}

void TransportConnection::Disconnect(bool immediate)
{
    if (immediate)
    {
        if (!IsClosed())
        {
            mPendingClosedCount += 3;
            SetClosed();
            GetCallback()->OnConnectionClosed((u32)this, 0);
        }
    }
    else if (mState != STATE_7 && !IsClosed())
    {
        mState = STATE_7;
        mClosingStartTick = nlGetTicker();
        tDebugPrintManager::Print(DC_NETWORK, "Sending Closing Message\n");
        u8 buffer[50];
        NetworkMessageSerializer serializer(1, buffer, sizeof(buffer));
        TransportClosing payload;
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.GetLength();
        SubmitReliable(0xE4, data, length);
    }
}

bool TransportConnection::IsFinished() const
{
    if (mState == STATE_8 && mPendingClosedCount <= 0 && mCloseDelayFrames <= 0)
    {
        return true;
    }
    return false;
}
