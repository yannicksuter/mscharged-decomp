#include "unclassified/tu_80326844.h"
#include "Game/Sys/debug.h"

#include "Game/NetworkSession.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTicker.h"
#include "NL/nlTime.h"

#include <string.h>

SlotPool<UnidentifiedTransportConnection> lbl_80584818(16, 0);

void* UnidentifiedTransportConnection::operator new(
    unsigned long, unsigned int, bool)
{
    return lbl_80584818.Allocate();
}

inline void UnidentifiedTransportConnection::operator delete(void* connection)
{
    lbl_80584818.Free((UnidentifiedTransportConnection*)connection);
}

// Sequence numbers wrap at 16 bits; a is after b when the forward distance
// from b to a is the shorter way around.
static inline bool UnidentifiedIsAfter(u16 a, u16 b)
{
    if (a == b)
    {
        return false;
    }
    return (u16)(b - a) > (u16)(a - b);
}

static inline bool UnidentifiedIsBefore(u16 a, u16 b)
{
    if (a == b)
    {
        return false;
    }
    return (u16)(b - a) < (u16)(a - b);
}

static inline bool UnidentifiedContainsReliable(
    UnidentifiedTransportMessage_8032B6D4& message, u16 sequence)
{
    for (int i = 0; i < fn_8032BC8C(&message); i++)
    {
        if (fn_8032BC94(&message, i)->mUnidentified06 == sequence)
        {
            return true;
        }
    }
    return false;
}

inline UnidentifiedReliableSocketCallback*
UnidentifiedTransportConnection::UnidentifiedGetCallback()
{
    return ((UnidentifiedReliableSocketLayout*)mSocket)->mCallback;
}

inline void UnidentifiedTransportConnection::UnidentifiedSetClosed()
{
    if (!UnidentifiedIsClosed())
    {
        if (mUnidentified7D4 == 5)
        {
            mUnidentified7D4 = 9;
        }
        else
        {
            mUnidentified7D4 = 8;
        }
    }
}

// Connection failure: log, drop into a closed state, and notify the socket
// callback. Expanded at each site (the format literal lives at its first use).
#define UNIDENTIFIED_CONNECTION_ERROR(result, reason)                        \
    do                                                                         \
    {                                                                          \
        int nResult = (result);                                                \
        int nReason = (reason);                                                \
        tDebugPrintManager::Print(DC_NETWORK, "Connection Error result %d reason %d\n", nResult,  \
            nReason);                                                          \
        if (mUnidentified7D4 < 6)                                              \
        {                                                                      \
            UnidentifiedSetClosed();                                           \
            if (!mUnidentified7DE)                                             \
            {                                                                  \
                UnidentifiedGetCallback()->OnConnectionAttempted(              \
                    (u32)this, nResult);                                       \
            }                                                                  \
        }                                                                      \
        else if (!UnidentifiedIsClosed())                                      \
        {                                                                      \
            UnidentifiedSetClosed();                                           \
            UnidentifiedGetCallback()->OnConnectionClosed((u32)this, nReason); \
        }                                                                      \
    } while (0)

inline bool UnidentifiedTransportConnection::UnidentifiedIsAckPending()
{
    if (mUnidentified038 != 0
        && (int)nlGetTickerDifference(mUnidentified038, nlGetTicker())
            > s_nSendPendingAckMS)
    {
        return true;
    }
    return false;
}

inline void UnidentifiedTransportConnection::UnidentifiedPrepareMessage(
    UnidentifiedTransportMessage_8032B6D4& message, u16 ack)
{
    fn_8032B8FC(&message, &ack);
    mUnidentified038 = 0;
    if (mUnidentified028 == 0
        || (int)nlGetTickerDifference(mUnidentified028, nlGetTicker())
            > s_nSendPingMS)
    {
        mUnidentified028 = fn_8032B920(&message);
    }
    if (mUnidentified02C != 0)
    {
        fn_8032B968(&message, mUnidentified030,
            (int)nlGetTickerDifference(mUnidentified02C, nlGetTicker()));
        mUnidentified02C = 0;
    }
    if (mUnidentified06C > 0)
    {
        fn_8032B98C(&message);
        mUnidentified06C--;
    }
}

inline void UnidentifiedTransportConnection::UnidentifiedSend(
    UnidentifiedTransportMessage_8032B6D4& message)
{
    fn_80324EAC(mSocket, &message, mAddress, mPort, &mUnidentified7DF);
}

UnidentifiedTransportConnection::UnidentifiedTransportConnection(
    UnidentifiedReliableSocketState* socket, const u8* address, u16 port,
    bool outgoing)
    : mSocket(socket)
    , mUnidentified028(0)
    , mUnidentified02C(0)
    , mUnidentified030(0)
    , mUnidentified038(0)
    , mUnidentified048(0)
    , mUnidentified06C(0)
    , mUnidentified070(5)
    , mUnidentified074(0)
    , mUnidentified078(0)
    , mUnidentified7D4(0)
    , mUnidentified7D8(0)
    , mUnidentified7DA(0)
    , mPort(port)
    , mUnidentified7DE(false)
    , mUnidentified7DF(false)
    , mUnidentified7E0(false)
{
    mUnidentified068 = 200;
    mUnidentified034 = nlGetTicker();
    unsigned long long now = nlGetTime();
    mUnidentified060 = now;
    mUnidentified040 = now;
    mUnidentified058 = now;
    mUnidentified050 = now;
    *(u32*)mAddress = *(const u32*)address;
    memset(mUnidentified000, 0, sizeof(mUnidentified000));
    if (outgoing)
    {
        mUnidentified7DE = false;
        mUnidentified7D4 = 1;
    }
    else
    {
        mUnidentified7D4 = 3;
        mUnidentified7DE = true;
    }
}

UnidentifiedTransportConnection::~UnidentifiedTransportConnection()
{
    for (int i = 0; i < mUnidentified078; i++)
    {
        if (mUnidentified07C[i] != 0)
        {
            fn_8032644C(mUnidentified07C[i]);
            mUnidentified07C[i] = 0;
        }
    }
    mUnidentified078 = 0;
    while (m_SentNotACKedQ.GetCount() > 0)
    {
        fn_8032644C(m_SentNotACKedQ.Pop());
    }
    while (m_OutgoingSendQ.GetCount() > 0)
    {
        fn_8032644C(m_OutgoingSendQ.Pop());
    }
    while (m_OutgoingUnreliableSendQ.GetCount() > 0)
    {
        fn_8032644C(m_OutgoingUnreliableSendQ.Pop());
    }
    while (m_OutgoingVoiceSendQ.GetCount() > 0)
    {
        fn_8032644C(m_OutgoingVoiceSendQ.Pop());
    }
    while (mUnidentified6D4.GetCount() > 0)
    {
        fn_8032644C(mUnidentified6D4.Pop());
    }
}

void UnidentifiedTransportConnection::UnidentifiedUpdate()
{
    mUnidentified074++;
    if (mUnidentified074 % s_nSendEveryNthFrame != 0)
    {
        return;
    }
    if (mUnidentified7D4 == 8 && mUnidentified070 > 0)
    {
        mUnidentified070--;
    }
    if (mUnidentified7D4 < 6)
    {
        UnidentifiedUpdateConnecting();
    }
    else if (mUnidentified7D4 == 6 || mUnidentified7D4 == 7)
    {
        UnidentifiedUpdateConnected();
    }
    if (!UnidentifiedIsClosed())
    {
        if (UnidentifiedIsAckPending())
        {
            UnidentifiedTransportMessage_8032B6D4 message;
            UnidentifiedPrepareMessage(message, mUnidentified7DA);
            if (g_TransportLayerLog >= 2)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Explicitly sent a pending ACK by itself\n");
            }
            UnidentifiedSend(message);
        }
    }
    UnidentifiedCheckTimeouts();
    if (mUnidentified06C > 0)
    {
        UnidentifiedTransportMessage_8032B6D4 message;
        UnidentifiedPrepareMessage(message, mUnidentified7DA);
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Explicitly sent a Unreliable CLOSED\n");
        }
        UnidentifiedSend(message);
    }
}

void UnidentifiedTransportConnection::UnidentifiedUpdateConnecting()
{
    while (m_OutgoingSendQ.GetCount() > 0)
    {
        UnidentifiedTransportMessage_8032B6D4 message;
        UnidentifiedPrepareMessage(message, mUnidentified7DA);
        UnidentifiedTransportPacket_803263E4* packet = m_OutgoingSendQ.Pop();
        packet->mUnidentified00 = nlGetTicker();
        fn_8032BB4C(&message, packet);
        UnidentifiedSend(message);
        if (!m_SentNotACKedQ.IsFull())
        {
            m_SentNotACKedQ.Push(packet);
        }
        else
        {
            mUnidentified7E0 = true;
            fn_8032644C(packet);
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
            UnidentifiedTransportPacket_803263E4* packet = m_SentNotACKedQ[i];
            if ((int)nlGetTickerDifference(packet->mUnidentified00, now)
                > s_nResendNormalMS)
            {
                UnidentifiedTransportMessage_8032B6D4 message;
                UnidentifiedPrepareMessage(message, mUnidentified7DA);
                fn_8032BB4C(&message, packet);
                packet->mUnidentified00 = nlGetTicker();
                UnidentifiedSend(message);
            }
        }
    }
}

int UnidentifiedTransportConnection::UnidentifiedResendNotACKed(
    UnidentifiedTransportMessage_8032B6D4& message, int minimumMS)
{
    if (m_SentNotACKedQ.GetCount() > 0
        && (int)nlGetTimeDifference(mUnidentified060, nlGetTime()) > minimumMS)
    {
        int limit = s_nResendNormalMS;
        if (s_nResendNormalAggressive
            && mUnidentified068 + s_nSendPendingAckMS < limit)
        {
            limit = mUnidentified068 + s_nSendPendingAckMS;
        }
        u32 now = nlGetTicker();
        int count = m_SentNotACKedQ.GetCount();
        bool any = false;
        int i;
        for (i = 0; i < count; i++)
        {
            UnidentifiedTransportPacket_803263E4* packet = m_SentNotACKedQ[i];
            bool resend
                = (int)nlGetTickerDifference(packet->mUnidentified00, now) > limit
                && !UnidentifiedContainsReliable(message, packet->mUnidentified06);
            if (resend)
            {
                any = true;
                break;
            }
        }
        if (any)
        {
            int resent = 0;
            for (i = 0; i < count; i++)
            {
                UnidentifiedTransportPacket_803263E4* packet
                    = m_SentNotACKedQ[i];
                bool resend
                    = (int)nlGetTickerDifference(packet->mUnidentified00, now)
                        > limit
                    && !UnidentifiedContainsReliable(
                        message, packet->mUnidentified06);
                if (resend)
                {
                    if (!fn_8032BB4C(&message, packet))
                    {
                        break;
                    }
                    packet->mUnidentified00 = nlGetTicker();
                    resent++;
                }
            }
            mUnidentified060 = nlGetTime();
            return resent;
        }
    }
    return 0;
}

void UnidentifiedTransportConnection::UnidentifiedUpdateConnected()
{
    unsigned long long now = nlGetTime();
    int elapsed = (int)nlGetTimeDifference(mUnidentified050, now);
    if (elapsed > s_nSendKeepAliveMS)
    {
        u8 buffer[50];
        UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
        UnidentifiedKeepAlive_8032C528 payload;
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.UnidentifiedGetLength();
        UnidentifiedSubmitReliable(0xE5, data, length);
        if (g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Submitted Keep Alive For Send after %d ms conn %d.%d\n",
                elapsed, mAddress[2], mAddress[3]);
        }
        mUnidentified050 = now;
    }

    if (m_OutgoingSendQ.GetCount() > 0)
    {
        UnidentifiedTransportMessage_8032B6D4 message;
        UnidentifiedPrepareMessage(message, mUnidentified7DA);
        int index = m_SentNotACKedQ.GetCount() - lbl_80531938[0];
        while (m_OutgoingSendQ.GetCount() > 0)
        {
            UnidentifiedTransportPacket_803263E4* packet = m_OutgoingSendQ.Peek();
            if (!fn_8032BB4C(&message, packet))
            {
                break;
            }
            packet->mUnidentified00 = nlGetTicker();
            m_OutgoingSendQ.Pop();
            if (!m_SentNotACKedQ.IsFull())
            {
                m_SentNotACKedQ.Push(packet);
            }
            else
            {
                mUnidentified7E0 = true;
                mUnidentified6D4.Push(packet);
                if (g_TransportLayerLog >= 1)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "m_SentNotACKedQ out of space, will close connection\n");
                }
            }
        }
        while (m_OutgoingUnreliableSendQ.GetCount() > 0)
        {
            UnidentifiedTransportPacket_803263E4* packet
                = m_OutgoingUnreliableSendQ.Peek();
            if (!fn_8032B99C(&message, packet))
            {
                break;
            }
            m_OutgoingUnreliableSendQ.Pop();
            mUnidentified6D4.Push(packet);
        }
        int remaining = s_nPayloadRedundancy;
        int i = 1;
        while (index >= 0 && remaining > 0)
        {
            if (!fn_8032BB4C(&message, m_SentNotACKedQ[index]))
            {
                break;
            }
            index -= lbl_80531938[i++];
            remaining--;
        }
        while (m_OutgoingVoiceSendQ.GetCount() > 0)
        {
            UnidentifiedTransportPacket_803263E4* packet
                = m_OutgoingVoiceSendQ.Peek();
            if (!fn_8032BA74(&message, packet))
            {
                break;
            }
            m_OutgoingVoiceSendQ.Pop();
            mUnidentified6D4.Push(packet);
        }
        int resent = UnidentifiedResendNotACKed(message, 0);
        if (resent > 0 && g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Piggyback resent %d not ACKed messages\n", resent);
        }
        UnidentifiedSend(message);
        if (fn_8032BC30(&message))
        {
            mUnidentified034 = nlGetTicker();
        }
        while (mUnidentified6D4.GetCount() > 0)
        {
            fn_8032644C(mUnidentified6D4.Pop());
        }
    }

    if (m_SentNotACKedQ.GetCount() > 0)
    {
        UnidentifiedTransportMessage_8032B6D4 message;
        int resent = UnidentifiedResendNotACKed(message, s_nResendGroupMS);
        if (resent > 0)
        {
            UnidentifiedPrepareMessage(message, mUnidentified7DA);
            if (g_TransportLayerLog >= 2)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Resent %d not ACKed messages\n", resent);
            }
            while (m_OutgoingUnreliableSendQ.GetCount() > 0)
            {
                UnidentifiedTransportPacket_803263E4* packet
                    = m_OutgoingUnreliableSendQ.Peek();
                if (!fn_8032B99C(&message, packet))
                {
                    break;
                }
                m_OutgoingUnreliableSendQ.Pop();
                mUnidentified6D4.Push(packet);
            }
            while (m_OutgoingVoiceSendQ.GetCount() > 0)
            {
                UnidentifiedTransportPacket_803263E4* packet
                    = m_OutgoingVoiceSendQ.Peek();
                if (!fn_8032BA74(&message, packet))
                {
                    break;
                }
                m_OutgoingVoiceSendQ.Pop();
                mUnidentified6D4.Push(packet);
            }
            UnidentifiedSend(message);
            if (fn_8032BC30(&message))
            {
                mUnidentified034 = nlGetTicker();
            }
            while (mUnidentified6D4.GetCount() > 0)
            {
                fn_8032644C(mUnidentified6D4.Pop());
            }
        }
    }

    if (m_OutgoingUnreliableSendQ.GetCount() > 0)
    {
        UnidentifiedTransportMessage_8032B6D4 message;
        UnidentifiedPrepareMessage(message, mUnidentified7DA);
        while (m_OutgoingUnreliableSendQ.GetCount() > 0)
        {
            UnidentifiedTransportPacket_803263E4* packet
                = m_OutgoingUnreliableSendQ.Peek();
            if (!fn_8032B99C(&message, packet))
            {
                break;
            }
            m_OutgoingUnreliableSendQ.Pop();
            mUnidentified6D4.Push(packet);
        }
        while (m_OutgoingVoiceSendQ.GetCount() > 0)
        {
            UnidentifiedTransportPacket_803263E4* packet
                = m_OutgoingVoiceSendQ.Peek();
            if (!fn_8032BA74(&message, packet))
            {
                break;
            }
            m_OutgoingVoiceSendQ.Pop();
            mUnidentified6D4.Push(packet);
        }
        UnidentifiedSend(message);
        if (fn_8032BC30(&message))
        {
            mUnidentified034 = nlGetTicker();
        }
        while (mUnidentified6D4.GetCount() > 0)
        {
            fn_8032644C(mUnidentified6D4.Pop());
        }
    }

    if (m_OutgoingVoiceSendQ.GetCount() > 0
        && (m_OutgoingVoiceSendQ.GetCount() >= s_nSendVoiceHighwaterNum
            || (int)nlGetTickerDifference(mUnidentified034, nlGetTicker())
                > s_nSendVoiceMS))
    {
        UnidentifiedTransportMessage_8032B6D4 message;
        UnidentifiedPrepareMessage(message, mUnidentified7DA);
        while (m_OutgoingVoiceSendQ.GetCount() > 0)
        {
            UnidentifiedTransportPacket_803263E4* packet
                = m_OutgoingVoiceSendQ.Peek();
            if (!fn_8032BA74(&message, packet))
            {
                break;
            }
            m_OutgoingVoiceSendQ.Pop();
            mUnidentified6D4.Push(packet);
        }
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Explicitly sent %d Voice packets\n",
                fn_8032BCD0(&message));
        }
        UnidentifiedSend(message);
        mUnidentified034 = nlGetTicker();
        while (mUnidentified6D4.GetCount() > 0)
        {
            fn_8032644C(mUnidentified6D4.Pop());
        }
    }
}

void UnidentifiedTransportConnection::UnidentifiedCheckTimeouts()
{
    if (mUnidentified7D4 < 6)
    {
        bool timedOut = false;
        unsigned long long now = nlGetTime();
        if (!mUnidentified7DE)
        {
            if ((int)nlGetTimeDifference(mUnidentified040, now)
                > s_nConnectClientTimeoutMS)
            {
                timedOut = true;
            }
        }
        else if (mUnidentified7D4 < 5)
        {
            if ((int)nlGetTimeDifference(mUnidentified040, now)
                > s_nConnectServerTimeoutMS)
            {
                timedOut = true;
            }
        }
        if (timedOut)
        {
            mUnidentified06C += 3;
            UNIDENTIFIED_CONNECTION_ERROR(6, 0);
        }
        else if (mUnidentified7DF)
        {
            UNIDENTIFIED_CONNECTION_ERROR(3, 3);
        }
        else if (mUnidentified7E0)
        {
            UNIDENTIFIED_CONNECTION_ERROR(3, 4);
        }
    }
    else
    {
        switch (mUnidentified7D4)
        {
        case 6:
        {
            int elapsed = (int)nlGetTimeDifference(mUnidentified058, nlGetTime());
            if (s_bExpireKeepAliveEnabled && elapsed > s_nExpireKeepAliveMS)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Keep alive not received for %d MS, closing connection\n",
                    elapsed);
                UNIDENTIFIED_CONNECTION_ERROR(6, 0);
            }
            else if (mUnidentified7DF)
            {
                UNIDENTIFIED_CONNECTION_ERROR(3, 3);
            }
            else if (mUnidentified7E0)
            {
                UNIDENTIFIED_CONNECTION_ERROR(3, 4);
            }
            break;
        }
        case 7:
        {
            if ((int)nlGetTickerDifference(mUnidentified048, nlGetTicker())
                > s_nClosingTimeoutMS)
            {
                UNIDENTIFIED_CONNECTION_ERROR(6, 0);
            }
            else if (mUnidentified7DF)
            {
                UNIDENTIFIED_CONNECTION_ERROR(3, 3);
            }
            else if (mUnidentified7E0)
            {
                UNIDENTIFIED_CONNECTION_ERROR(3, 4);
            }
            break;
        }
        }
    }
}

void UnidentifiedTransportConnection::UnidentifiedSubmitReliable(
    u8 type, const void* buffer, int size)
{
    UnidentifiedTransportPacket_803263E4* packet
        = new (8, false) UnidentifiedTransportPacket_803263E4;
    packet->mUnidentified00 = 0;
    packet->mUnidentified04 = type;
    packet->mUnidentified06 = mUnidentified7D8;
    packet->mSize = size;
    memcpy(packet->mPayload, buffer, size);
    mUnidentified7D8++;
    if (!m_OutgoingSendQ.IsFull())
    {
        m_OutgoingSendQ.Push(packet);
    }
    else
    {
        mUnidentified7E0 = true;
        fn_8032644C(packet);
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "m_OutgoingSendQ out of space, will close connection\n");
        }
    }
}

void UnidentifiedTransportConnection::UnidentifiedSubmitUnreliable(
    const void* buffer, int size)
{
    UnidentifiedTransportPacket_803263E4* packet
        = new (8, false) UnidentifiedTransportPacket_803263E4;
    packet->mUnidentified00 = 0;
    packet->mUnidentified04 = 0xE6;
    packet->mUnidentified06 = 0;
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
        fn_8032644C(packet);
    }
}

void UnidentifiedTransportConnection::UnidentifiedSubmitVoice(
    const void* buffer, int size)
{
    UnidentifiedTransportPacket_803263E4* packet
        = new (8, false) UnidentifiedTransportPacket_803263E4;
    packet->mUnidentified00 = 0;
    packet->mUnidentified04 = 0xE7;
    packet->mUnidentified06 = 0;
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
        fn_8032644C(packet);
    }
}

void UnidentifiedTransportConnection::HandleTransportMessage(
    UnidentifiedTransportMessage_8032B6D4* message)
{
    if (g_TransportLayerLog >= 3)
    {
        tDebugPrintManager::Print(DC_NETWORK, "HandleTransportMessage: ");
    }
    if (fn_8032BC18(message))
    {
        u16 ack = fn_8032BCFC(message) >> 16;
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
            UnidentifiedTransportPacket_803263E4* packet = m_SentNotACKedQ.Peek();
            if (!UnidentifiedIsBefore(packet->mUnidentified06, ack))
            {
                break;
            }
            fn_8032644C(m_SentNotACKedQ.Pop());
        }
    }
    fn_8032BC24(message);
    if (fn_8032BC3C(message))
    {
        mUnidentified02C = nlGetTicker();
        mUnidentified030 = fn_8032BD08(message);
    }
    if (fn_8032BC48(message))
    {
        u16 other = 0;
        u32 sent = fn_8032BD10(message, &other);
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
        mUnidentified068 = latency;
    }

    u8 count = fn_8032BCA4(message);
    if (count != 0)
    {
        for (u8 i = 0; i < count; i++)
        {
            UnidentifiedTransportPacket_803263E4* packet = fn_8032BCC0(message, i);
            UnidentifiedGetCallback()->ReliableCallbackVirtual0C(
                (u32)this, packet->mPayload, packet->mSize, false);
            fn_8032644C(packet);
        }
    }
    count = fn_8032BCD0(message);
    if (count != 0)
    {
        for (u8 i = 0; i < count; i++)
        {
            UnidentifiedTransportPacket_803263E4* packet = fn_8032BCEC(message, i);
            UnidentifiedGetCallback()->ReliableCallbackVirtual10(
                (u32)this, packet->mPayload, packet->mSize);
            fn_8032644C(packet);
        }
    }
    count = fn_8032BC8C(message);
    for (u8 i = 0; i < count; i++)
    {
        UnidentifiedTransportPacket_803263E4* packet = fn_8032BC94(message, i);
        if (g_TransportLayerLog >= 3)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Payload %d SN %d ", i, packet->mUnidentified06);
        }
        if (packet->mUnidentified06 == mUnidentified7DA)
        {
            UnidentifiedDeliver(packet);
            if (g_TransportLayerLog >= 3)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Delivered payload type %d size %d",
                    packet->mUnidentified04, packet->mSize);
            }
            fn_8032644C(packet);
            mUnidentified7DA++;
            if (mUnidentified038 == 0)
            {
                mUnidentified038 = nlGetTicker();
            }
            UnidentifiedProcessHold();
        }
        else if (UnidentifiedIsAfter(packet->mUnidentified06, mUnidentified7DA))
        {
            if (g_TransportLayerLog >= 3)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Out Of Order ");
            }
            if (!UnidentifiedHold(packet))
            {
                fn_8032644C(packet);
            }
        }
        else
        {
            if (g_TransportLayerLog >= 3)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Discarded duplicate ");
            }
            fn_8032644C(packet);
        }
    }
    if (g_TransportLayerLog >= 3)
    {
        tDebugPrintManager::Print(DC_NETWORK, "END\n");
    }

    if (fn_8032BC54(message))
    {
        if (!UnidentifiedIsClosed())
        {
            tDebugPrintManager::Print(DC_NETWORK, "Received unreliable CLOSED message\n");
            UNIDENTIFIED_CONNECTION_ERROR(2, mUnidentified7D4 != 7);
        }
    }
}

void UnidentifiedTransportConnection::UnidentifiedDeliver(
    UnidentifiedTransportPacket_803263E4* packet)
{
    if (packet->mUnidentified04 == 0)
    {
        UnidentifiedGetCallback()->ReliableCallbackVirtual0C(
            (u32)this, packet->mPayload, packet->mSize, true);
        return;
    }

    UnidentifiedMessageSerializer serializer(0, packet->mPayload, packet->mSize);
    switch (packet->mUnidentified04)
    {
    case 0xE0:
    {
        UnidentifiedClientChallenge_8032C294 payload;
        payload.Serialize(&serializer);
        UnidentifiedHandleClientChallenge(&payload);
        break;
    }
    case 0xE1:
    {
        UnidentifiedServerChallenge_8032C308 payload;
        payload.Serialize(&serializer);
        UnidentifiedHandleServerChallenge(&payload);
        break;
    }
    case 0xE2:
    {
        UnidentifiedClientResponse_8032C3CC payload;
        payload.Serialize(&serializer);
        tDebugPrintManager::Print(DC_NETWORK, "Received Client Response Message\n");
        if (mUnidentified7D4 != 4)
        {
            UNIDENTIFIED_CONNECTION_ERROR(7, 2);
        }
        else if (!fn_8032C640(mUnidentified000, payload.mPayload))
        {
            UNIDENTIFIED_CONNECTION_ERROR(7, 2);
        }
        else
        {
            mUnidentified7D4 = 5;
            UnidentifiedGetCallback()->ReliableCallbackVirtual14(
                (u32)this, mAddress, 0, 0, 0);
        }
        break;
    }
    case 0xE3:
    {
        UnidentifiedServerResponse_8032C440 payload;
        payload.Serialize(&serializer);
        if (mUnidentified7D4 != 2)
        {
            UNIDENTIFIED_CONNECTION_ERROR(7, 2);
        }
        else if (payload.mAccepted)
        {
            mUnidentified7D4 = 6;
            UnidentifiedGetCallback()->OnConnectionAttempted((u32)this, 0);
        }
        else
        {
            mUnidentified06C += 3;
            UnidentifiedSetClosed();
            UnidentifiedGetCallback()->OnConnectionAttempted((u32)this, 2);
        }
        break;
    }
    case 0xE4:
    {
        UnidentifiedClosing_8032C4B4 payload;
        payload.Serialize(&serializer);
        tDebugPrintManager::Print(DC_NETWORK, "Received closing message\n");
        mUnidentified06C += 3;
        UNIDENTIFIED_CONNECTION_ERROR(2, mUnidentified7D4 != 7);
        break;
    }
    case 0xE5:
    {
        UnidentifiedKeepAlive_8032C528 payload;
        payload.Serialize(&serializer);
        unsigned long long now = nlGetTime();
        if (g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Received keep alive message after %d MS conn %d.%d\n",
                (int)nlGetTimeDifference(mUnidentified058, now), mAddress[2],
                mAddress[3]);
        }
        mUnidentified058 = now;
        break;
    }
    }
}

bool UnidentifiedTransportConnection::UnidentifiedHold(
    UnidentifiedTransportPacket_803263E4* packet)
{
    int count = mUnidentified078;
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
        mUnidentified07C[0] = packet;
        mUnidentified078 = 1;
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Placed payload SN %d in hold\n", packet->mUnidentified06);
        }
        return true;
    }
    if (count == 1)
    {
        UnidentifiedTransportPacket_803263E4* held = mUnidentified07C[0];
        if (packet->mUnidentified06 == held->mUnidentified06)
        {
            return false;
        }
        if (UnidentifiedIsBefore(packet->mUnidentified06, held->mUnidentified06))
        {
            mUnidentified07C[1] = held;
            mUnidentified07C[0] = packet;
        }
        else
        {
            mUnidentified07C[1] = packet;
        }
        mUnidentified078 = 2;
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Placed 2nd payload SN %d in hold\n",
                packet->mUnidentified06);
        }
        return true;
    }

    int last = mUnidentified078 - 1;
    int low = 0;
    int high = last;
    int middle = -1;
    while (high >= low)
    {
        middle = (low + high) >> 1;
        UnidentifiedTransportPacket_803263E4* held = mUnidentified07C[middle];
        if (UnidentifiedIsAfter(packet->mUnidentified06, held->mUnidentified06))
        {
            low = middle + 1;
        }
        else if (UnidentifiedIsBefore(
                     packet->mUnidentified06, held->mUnidentified06))
        {
            high = middle - 1;
        }
        else
        {
            break;
        }
    }
    UnidentifiedTransportPacket_803263E4* held = mUnidentified07C[middle];
    if (packet->mUnidentified06 == held->mUnidentified06)
    {
        return false;
    }
    if (UnidentifiedIsAfter(packet->mUnidentified06, held->mUnidentified06))
    {
        for (int i = mUnidentified078 - 1; i >= middle + 1; i--)
        {
            mUnidentified07C[i + 1] = mUnidentified07C[i];
        }
        mUnidentified07C[middle + 1] = packet;
        mUnidentified078++;
    }
    else
    {
        for (int i = mUnidentified078 - 1; i >= middle; i--)
        {
            mUnidentified07C[i + 1] = mUnidentified07C[i];
        }
        mUnidentified07C[middle] = packet;
        mUnidentified078++;
    }
    if (g_TransportLayerLog >= 1)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Placed %dth payload SN %d in hold\n",
            mUnidentified078 + 1, packet->mUnidentified06);
    }
    return true;
}

void UnidentifiedTransportConnection::UnidentifiedProcessHold()
{
    int delivered = 0;
    while (delivered < mUnidentified078)
    {
        if (mUnidentified07C[delivered]->mUnidentified06 != mUnidentified7DA)
        {
            break;
        }
        UnidentifiedDeliver(mUnidentified07C[delivered]);
        if (g_TransportLayerLog >= 2)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Delivered OUT OF ORDER payload type %d size %d",
                mUnidentified07C[delivered]->mUnidentified04,
                mUnidentified07C[delivered]->mSize);
        }
        fn_8032644C(mUnidentified07C[delivered]);
        mUnidentified7DA++;
        if (mUnidentified038 == 0)
        {
            mUnidentified038 = nlGetTicker();
        }
        delivered++;
    }
    if (delivered != 0)
    {
        if (g_TransportLayerLog >= 1)
        {
            tDebugPrintManager::Print(DC_NETWORK, "Moved Out Of Order Hold up %d spots\n", delivered);
        }
        for (int i = delivered; i < mUnidentified078; i++)
        {
            mUnidentified07C[i - delivered] = mUnidentified07C[i];
        }
        mUnidentified078 -= delivered;
    }
}

void UnidentifiedTransportConnection::UnidentifiedSendClientChallenge()
{
    tDebugPrintManager::Print(DC_NETWORK, "Sending Client Challenge\n");
    u8 buffer[200];
    UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
    UnidentifiedClientChallenge_8032C294 payload;
    fn_8032C5E4((u32*)payload.mPayload);
    fn_8032C630(mUnidentified000, payload.mPayload);
    payload.Serialize(&serializer);
    u8* data = serializer.mBuffer;
    int length = serializer.UnidentifiedGetLength();
    UnidentifiedSubmitReliable(0xE0, data, length);
    mUnidentified7D4 = 1;
}

void UnidentifiedTransportConnection::UnidentifiedHandleClientChallenge(
    UnidentifiedClientChallenge_8032C294* challenge)
{
    tDebugPrintManager::Print(DC_NETWORK, "Received Client Challenge Message\n");
    fn_80324CB4(((UnidentifiedReliableSocketLayout*)mSocket)->mDisplayEntries,
        "Received Client Challenge Message\n");
    if (mUnidentified7D4 != 3)
    {
        UNIDENTIFIED_CONNECTION_ERROR(7, 2);
    }
    else
    {
        UnidentifiedServerChallenge_8032C308 payload;
        fn_8032C630(payload.mUnidentified04, challenge->mPayload);
        fn_8032C5E4((u32*)payload.mUnidentified24);
        fn_8032C630(mUnidentified000, payload.mUnidentified24);
        u8 buffer[200];
        UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.UnidentifiedGetLength();
        UnidentifiedSubmitReliable(0xE1, data, length);
        mUnidentified7D4 = 4;
    }
}

void UnidentifiedTransportConnection::UnidentifiedHandleServerChallenge(
    UnidentifiedServerChallenge_8032C308* challenge)
{
    tDebugPrintManager::Print(DC_NETWORK, "Received Server Challenge Message\n");
    fn_80324CB4(((UnidentifiedReliableSocketLayout*)mSocket)->mDisplayEntries,
        "Received Server Challenge Message");
    if (mUnidentified7D4 != 1)
    {
        UNIDENTIFIED_CONNECTION_ERROR(7, 2);
    }
    else if (!fn_8032C640(mUnidentified000, challenge->mUnidentified04))
    {
        UNIDENTIFIED_CONNECTION_ERROR(7, 2);
    }
    else
    {
        UnidentifiedClientResponse_8032C3CC payload;
        fn_8032C630(payload.mPayload, challenge->mUnidentified24);
        u8 buffer[200];
        UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.UnidentifiedGetLength();
        UnidentifiedSubmitReliable(0xE2, data, length);
        mUnidentified7D4 = 2;
    }
}

bool UnidentifiedTransportConnection::UnidentifiedAccept()
{
    if (mUnidentified7D4 == 9)
    {
        mUnidentified7D4 = 8;
        return false;
    }
    if (mUnidentified7D4 != 5)
    {
        return false;
    }
    UnidentifiedServerResponse_8032C440 payload(true);
    u8 buffer[50];
    UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
    payload.Serialize(&serializer);
    u8* data = serializer.mBuffer;
    int length = serializer.UnidentifiedGetLength();
    UnidentifiedSubmitReliable(0xE3, data, length);
    mUnidentified7D4 = 6;
    return true;
}

void UnidentifiedTransportConnection::UnidentifiedReject()
{
    if (mUnidentified7D4 == 9)
    {
        mUnidentified7D4 = 8;
        return;
    }
    if (mUnidentified7D4 == 5)
    {
        UnidentifiedServerResponse_8032C440 payload;
        u8 buffer[50];
        UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.UnidentifiedGetLength();
        UnidentifiedSubmitReliable(0xE3, data, length);
        mUnidentified7D4 = 7;
        mUnidentified048 = nlGetTicker();
    }
}

void UnidentifiedTransportConnection::UnidentifiedDisconnect(bool immediate)
{
    if (immediate)
    {
        if (!UnidentifiedIsClosed())
        {
            mUnidentified06C += 3;
            UnidentifiedSetClosed();
            UnidentifiedGetCallback()->OnConnectionClosed((u32)this, 0);
        }
    }
    else if (mUnidentified7D4 != 7 && !UnidentifiedIsClosed())
    {
        mUnidentified7D4 = 7;
        mUnidentified048 = nlGetTicker();
        tDebugPrintManager::Print(DC_NETWORK, "Sending Closing Message\n");
        u8 buffer[50];
        UnidentifiedMessageSerializer serializer(1, buffer, sizeof(buffer));
        UnidentifiedClosing_8032C4B4 payload;
        payload.Serialize(&serializer);
        u8* data = serializer.mBuffer;
        int length = serializer.UnidentifiedGetLength();
        UnidentifiedSubmitReliable(0xE4, data, length);
    }
}

bool UnidentifiedTransportConnection::UnidentifiedIsFinished() const
{
    if (mUnidentified7D4 == 8 && mUnidentified06C <= 0 && mUnidentified070 <= 0)
    {
        return true;
    }
    return false;
}
