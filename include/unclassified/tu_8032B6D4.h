#ifndef UNCLASSIFIED_TU_8032B6D4_H
#define UNCLASSIFIED_TU_8032B6D4_H

#include "Game/NetworkMessages.h"
#include "types.h"
#include "unclassified/tu_803263E4.h"

// Transport-layer datagram assembled by the reliable connection layer: the
// optional control fields and the reliable/unreliable/voice payload packets
// that ride in one socket send.
class UnidentifiedTransportMessage_8032B6D4
{
public:
    UnidentifiedTransportMessage_8032B6D4();
    ~UnidentifiedTransportMessage_8032B6D4();
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x004 */ u32 mUnidentified04;
    /* 0x008 */ u8 mFlags;
    /* 0x009 */ u8 mPadding09;
    /* 0x00A */ u16 mUnidentified0A;
    /* 0x00C */ u16 mUnidentified0C;
    /* 0x00E */ u8 mUnidentified0E;
    /* 0x00F */ u8 mPadding0F;
    /* 0x010 */ u32 mTimestamp;
    /* 0x014 */ u32 mUnidentified14;
    /* 0x018 */ u16 mUnidentified18;
    /* 0x01A */ u8 mUnreliableCount;
    /* 0x01B */ u8 mPadding1B;
    /* 0x01C */ UnidentifiedTransportPacket_803263E4* mUnreliable[16];
    /* 0x05C */ u8 mVoiceCount;
    /* 0x05D */ u8 mPadding5D[3];
    /* 0x060 */ UnidentifiedTransportPacket_803263E4* mVoice[16];
    /* 0x0A0 */ u8 mReliableCount;
    /* 0x0A1 */ u8 mPaddingA1[3];
    /* 0x0A4 */ UnidentifiedTransportPacket_803263E4* mReliable[16];
    /* 0x0E4 */ int mSize;
}; // size: 0xE8

// Connection handshake and control payloads carried in packets of types
// 0xE0-0xE5. Each serializes itself through the shared message serializer.
class UnidentifiedClientChallenge_8032C294
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x04 */ u8 mPayload[0x20];
}; // size: 0x24

class UnidentifiedServerChallenge_8032C308
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x04 */ u8 mUnidentified04[0x20];
    /* 0x24 */ u8 mUnidentified24[0x20];
}; // size: 0x44

class UnidentifiedClientResponse_8032C3CC
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x04 */ u8 mPayload[0x20];
}; // size: 0x24

class UnidentifiedServerResponse_8032C440
{
public:
    UnidentifiedServerResponse_8032C440(bool accepted = false)
        : mAccepted(accepted)
    {
    }
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x04 */ bool mAccepted;
}; // size: 0x08

class UnidentifiedClosing_8032C4B4
{
public:
    UnidentifiedClosing_8032C4B4()
        : mUnidentified04(0)
    {
    }
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x04 */ u8 mUnidentified04;
}; // size: 0x08

class UnidentifiedKeepAlive_8032C528
{
public:
    UnidentifiedKeepAlive_8032C528()
        : mUnidentified04(0)
    {
    }
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);

    /* 0x04 */ u8 mUnidentified04;
}; // size: 0x08

extern "C"
{
    void fn_8032B81C(UnidentifiedTransportMessage_8032B6D4* message);
    void fn_8032B8FC(
        UnidentifiedTransportMessage_8032B6D4* message, const u16* value);
    u32 fn_8032B920(UnidentifiedTransportMessage_8032B6D4* message);
    void fn_8032B968(
        UnidentifiedTransportMessage_8032B6D4* message, u32 value, u16 other);
    void fn_8032B98C(UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032B99C(UnidentifiedTransportMessage_8032B6D4* message,
        UnidentifiedTransportPacket_803263E4* packet);
    bool fn_8032BA74(UnidentifiedTransportMessage_8032B6D4* message,
        UnidentifiedTransportPacket_803263E4* packet);
    bool fn_8032BB4C(UnidentifiedTransportMessage_8032B6D4* message,
        UnidentifiedTransportPacket_803263E4* packet);
    bool fn_8032BC18(const UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032BC24(const UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032BC30(const UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032BC3C(const UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032BC48(const UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032BC54(const UnidentifiedTransportMessage_8032B6D4* message);
    bool fn_8032BC60(const UnidentifiedTransportMessage_8032B6D4* message);
    int fn_8032BC8C(const UnidentifiedTransportMessage_8032B6D4* message);
    UnidentifiedTransportPacket_803263E4* fn_8032BC94(
        const UnidentifiedTransportMessage_8032B6D4* message,
        unsigned int index);
    int fn_8032BCA4(const UnidentifiedTransportMessage_8032B6D4* message);
    UnidentifiedTransportPacket_803263E4* fn_8032BCC0(
        const UnidentifiedTransportMessage_8032B6D4* message,
        unsigned int index);
    int fn_8032BCD0(const UnidentifiedTransportMessage_8032B6D4* message);
    UnidentifiedTransportPacket_803263E4* fn_8032BCEC(
        const UnidentifiedTransportMessage_8032B6D4* message,
        unsigned int index);
    u32 fn_8032BCFC(const UnidentifiedTransportMessage_8032B6D4* message);
    u32 fn_8032BD08(const UnidentifiedTransportMessage_8032B6D4* message);
    u32 fn_8032BD10(UnidentifiedTransportMessage_8032B6D4* message, u16* other);
    void fn_8032C184(UnidentifiedTransportMessage_8032B6D4* message,
        UnidentifiedMessageSerializer* serializer);
    void fn_8032C59C();
    void fn_8032C5E4(u32* values);
    void fn_8032C630(unsigned char* output, unsigned char* input);
    bool fn_8032C640(const void* first, const void* second);
}

#endif // UNCLASSIFIED_TU_8032B6D4_H
