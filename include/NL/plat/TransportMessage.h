#ifndef NL_PLAT_TRANSPORTMESSAGE_H
#define NL_PLAT_TRANSPORTMESSAGE_H

#include "Game/NetworkMessageSerializer.h"
#include "types.h"
#include "NL/plat/TransportPacket.h"

struct TransportAck
{
    TransportAck(u16 sequence)
        : mSequence(sequence)
    {
    }

    u16 mSequence;
};

// Connection handshake and control payloads carried in packets of types
// 0xE0-0xE5. Each serializes itself through the shared message serializer.
class TransportClientChallenge
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);

    /* 0x04 */ u8 mPayload[0x20];
}; // size: 0x24

class TransportServerChallenge
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);

    /* 0x04 */ u8 mClientResponse[0x20];
    /* 0x24 */ u8 mChallenge[0x20];
}; // size: 0x44

class TransportClientResponse
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);

    /* 0x04 */ u8 mPayload[0x20];
}; // size: 0x24

class TransportServerResponse
{
public:
    TransportServerResponse(bool accepted = false)
        : mAccepted(accepted)
    {
    }
    virtual void Serialize(NetworkMessageSerializer* serializer);

    /* 0x04 */ bool mAccepted;
}; // size: 0x08

class TransportClosing
{
public:
    TransportClosing()
        : mUnidentified04(0)
    {
    }
    virtual void Serialize(NetworkMessageSerializer* serializer);

    /* 0x04 */ u8 mUnidentified04;
}; // size: 0x08

class TransportKeepAlive
{
public:
    TransportKeepAlive()
        : mUnidentified04(0)
    {
    }
    virtual void Serialize(NetworkMessageSerializer* serializer);

    /* 0x04 */ u8 mUnidentified04;
}; // size: 0x08

// Transport-layer datagram assembled by the reliable connection layer: the
// optional control fields and the reliable/unreliable/voice payload packets
// that ride in one socket send.
class TransportMessage
{
public:
    TransportMessage();
    ~TransportMessage();
    virtual void Serialize(NetworkMessageSerializer* serializer);

    void ReleasePackets();
    void SetAck(const TransportAck& value);
    u32 AddPing();
    void AddPong(unsigned int timestamp, u16 delayMS);
    void SetClosed();
    bool AddUnreliablePacket(TransportPacket* packet);
    bool AddVoicePacket(TransportPacket* packet);
    bool AddReliablePacket(TransportPacket* packet);
    bool HasAck() const;
    bool HasControlFlag80() const;
    bool HasVoice() const;
    bool HasPing() const;
    bool HasPong() const;
    bool IsClosed() const;
    bool IsClientChallenge() const;
    int GetReliableCount() const;
    TransportPacket* GetReliablePacket(unsigned int index) const;
    int GetUnreliableCount() const;
    TransportPacket* GetUnreliablePacket(unsigned int index) const;
    int GetVoiceCount() const;
    TransportPacket* GetVoicePacket(unsigned int index) const;
    TransportAck GetAck() const;
    u32 GetPingTimestamp() const;
    u32 GetPongTimestamp(u16* delayMS);
    void SerializeVoice(NetworkMessageSerializer* serializer);

    /* 0x004 */ u32 mChecksum;
    /* 0x008 */ u8 mFlags;
    /* 0x009 */ u8 mPadding09;
    /* 0x00A */ u16 mAck;
    /* 0x00C */ u16 mUnidentified0C;
    /* 0x00E */ u8 mUnidentified0E;
    /* 0x00F */ u8 mPadding0F;
    /* 0x010 */ u32 mTimestamp;
    /* 0x014 */ u32 mPongTimestamp;
    /* 0x018 */ u16 mPongDelayMS;
    /* 0x01A */ u8 mUnreliableCount;
    /* 0x01B */ u8 mPadding1B;
    /* 0x01C */ TransportPacket* mUnreliable[16];
    /* 0x05C */ u8 mVoiceCount;
    /* 0x05D */ u8 mPadding5D[3];
    /* 0x060 */ TransportPacket* mVoice[16];
    /* 0x0A0 */ u8 mReliableCount;
    /* 0x0A1 */ u8 mPaddingA1[3];
    /* 0x0A4 */ TransportPacket* mReliable[16];
    /* 0x0E4 */ int mSize;
}; // size: 0xE8


void InitializeTransportChallengeCipher();
void GenerateTransportChallenge(unsigned int* values);
void EncodeTransportChallenge(unsigned char* output, unsigned char* input);
bool CompareTransportChallenge(const void* first, const void* second);

#endif // NL_PLAT_TRANSPORTMESSAGE_H
