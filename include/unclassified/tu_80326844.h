#ifndef UNCLASSIFIED_TU_80326844_H
#define UNCLASSIFIED_TU_80326844_H

#include "NL/UnidentifiedQueue.h"
#include "types.h"
#include "unclassified/tu_803263E4.h"
#include "unclassified/tu_8032452C.h"
#include "unclassified/tu_8032B6D4.h"

// One reliable transport connection ("Network/TransportLayer"). States:
// 1 client challenge sent, 2 client response sent, 3 server awaiting
// challenge, 4 server challenge sent, 5 server authenticated, 6 connected,
// 7 closing, 8 closed, 9 closed while awaiting acceptance.
class UnidentifiedTransportConnection
{
public:
    static void* operator new(
        unsigned long size, unsigned int alignment, bool fromEnd);
    void operator delete(void* connection);

    UnidentifiedTransportConnection(UnidentifiedReliableSocketState* socket,
        const u8* address, u16 port, bool outgoing);
    ~UnidentifiedTransportConnection();

    void UnidentifiedUpdate();
    void UnidentifiedUpdateConnecting();
    int UnidentifiedResendNotACKed(
        UnidentifiedTransportMessage_8032B6D4& message, int minimumMS);
    void UnidentifiedUpdateConnected();
    void UnidentifiedCheckTimeouts();
    void UnidentifiedSubmitReliable(u8 type, const void* buffer, int size);
    void UnidentifiedSubmitUnreliable(const void* buffer, int size);
    void UnidentifiedSubmitVoice(const void* buffer, int size);
    void HandleTransportMessage(UnidentifiedTransportMessage_8032B6D4* message);
    void UnidentifiedDeliver(UnidentifiedTransportPacket_803263E4* packet);
    bool UnidentifiedHold(UnidentifiedTransportPacket_803263E4* packet);
    void UnidentifiedProcessHold();
    void UnidentifiedSendClientChallenge();
    void UnidentifiedHandleClientChallenge(
        UnidentifiedClientChallenge_8032C294* challenge);
    void UnidentifiedHandleServerChallenge(
        UnidentifiedServerChallenge_8032C308* challenge);
    bool UnidentifiedAccept();
    void UnidentifiedReject();
    void UnidentifiedDisconnect(bool immediate);
    bool UnidentifiedIsFinished() const;

    bool UnidentifiedIsClosed()
    {
        return mUnidentified7D4 == 8 || mUnidentified7D4 == 9;
    }

private:
    void UnidentifiedSetClosed();
    bool UnidentifiedIsAckPending();
    void UnidentifiedPrepareMessage(
        UnidentifiedTransportMessage_8032B6D4& message, u16 ack);
    void UnidentifiedSend(UnidentifiedTransportMessage_8032B6D4& message);
    UnidentifiedReliableSocketCallback* UnidentifiedGetCallback();

public:
    /* 0x000 */ u8 mUnidentified000[0x20];
    /* 0x020 */ UnidentifiedReliableSocketState* mSocket;
    /* 0x024 */ u8 mAddress[4];
    /* 0x028 */ u32 mUnidentified028;
    /* 0x02C */ u32 mUnidentified02C;
    /* 0x030 */ u32 mUnidentified030;
    /* 0x034 */ u32 mUnidentified034;
    /* 0x038 */ u32 mUnidentified038;
    /* 0x040 */ unsigned long long mUnidentified040;
    /* 0x048 */ u32 mUnidentified048;
    /* 0x050 */ unsigned long long mUnidentified050;
    /* 0x058 */ unsigned long long mUnidentified058;
    /* 0x060 */ unsigned long long mUnidentified060;
    /* 0x068 */ int mUnidentified068;
    /* 0x06C */ int mUnidentified06C;
    /* 0x070 */ int mUnidentified070;
    /* 0x074 */ u32 mUnidentified074;
    /* 0x078 */ int mUnidentified078;
    /* 0x07C */ UnidentifiedTransportPacket_803263E4* mUnidentified07C[30];
    /* 0x0F4 */ UnidentifiedTransportQueue<UnidentifiedTransportPacket_803263E4*, 300> m_SentNotACKedQ;
    /* 0x5B4 */ UnidentifiedTransportQueue<UnidentifiedTransportPacket_803263E4*, 20> m_OutgoingSendQ;
    /* 0x614 */ UnidentifiedTransportQueue<UnidentifiedTransportPacket_803263E4*, 20> m_OutgoingUnreliableSendQ;
    /* 0x674 */ UnidentifiedTransportQueue<UnidentifiedTransportPacket_803263E4*, 20> m_OutgoingVoiceSendQ;
    /* 0x6D4 */ UnidentifiedTransportQueue<UnidentifiedTransportPacket_803263E4*, 60> mUnidentified6D4;
    /* 0x7D4 */ int mUnidentified7D4;
    /* 0x7D8 */ u16 mUnidentified7D8;
    /* 0x7DA */ u16 mUnidentified7DA;
    /* 0x7DC */ u16 mPort;
    /* 0x7DE */ bool mUnidentified7DE;
    /* 0x7DF */ bool mUnidentified7DF;
    /* 0x7E0 */ bool mUnidentified7E0;
}; // size: 0x7E8

#endif // UNCLASSIFIED_TU_80326844_H
