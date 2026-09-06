#include "Game/NetworkMessages.h"
#include "Game/NetworkRandom_803236CC.h"
#include "NL/blowfish.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_803263E4.h"
#include "unclassified/tu_8032B6D4.h"

struct UnidentifiedNetworkMessageStorage_8032C66C
{
    u8 mStorage[0x800];
};

extern const char lbl_80532378[];
extern const char lbl_805323B8[];
extern const char lbl_805323F4[];
extern const char lbl_8053241C[];
extern const char lbl_80532454[];
extern const char lbl_80532480[];
SlotPool<UnidentifiedNetworkMessageStorage_8032C66C> lbl_80584840(15, 0);
CBlowFish* lbl_806E20F8;
extern u8 lbl_806DF708[8];

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);
}

UnidentifiedTransportMessage_8032B6D4::UnidentifiedTransportMessage_8032B6D4()
{
    mUnidentified04 = 0;
    mFlags = 0;
    mUnidentified0A = 0;
    mUnidentified0C = 0;
    mUnidentified0E = 0;
    mTimestamp = 0;
    mUnidentified14 = 0;
    mUnidentified18 = 0;
    mUnreliableCount = 0;
    mVoiceCount = 0;
    mReliableCount = 0;
    mReliable[0] = 0;
    mReliable[1] = 0;
    mReliable[2] = 0;
    mReliable[3] = 0;
    mReliable[4] = 0;
    mReliable[5] = 0;
    mReliable[6] = 0;
    mReliable[7] = 0;
    mReliable[8] = 0;
    mReliable[9] = 0;
    mReliable[10] = 0;
    mReliable[11] = 0;
    mReliable[12] = 0;
    mReliable[13] = 0;
    mReliable[14] = 0;
    mReliable[15] = 0;
    mUnreliable[0] = 0;
    mUnreliable[1] = 0;
    mUnreliable[2] = 0;
    mUnreliable[3] = 0;
    mUnreliable[4] = 0;
    mUnreliable[5] = 0;
    mUnreliable[6] = 0;
    mUnreliable[7] = 0;
    mUnreliable[8] = 0;
    mUnreliable[9] = 0;
    mUnreliable[10] = 0;
    mUnreliable[11] = 0;
    mUnreliable[12] = 0;
    mUnreliable[13] = 0;
    mUnreliable[14] = 0;
    mUnreliable[15] = 0;
    mVoice[0] = 0;
    mVoice[1] = 0;
    mVoice[2] = 0;
    mVoice[3] = 0;
    mVoice[4] = 0;
    mVoice[5] = 0;
    mVoice[6] = 0;
    mVoice[7] = 0;
    mVoice[8] = 0;
    mVoice[9] = 0;
    mVoice[10] = 0;
    mVoice[11] = 0;
    mVoice[12] = 0;
    mVoice[13] = 0;
    mVoice[14] = 0;
    mVoice[15] = 0;
    mSize = 6;
}

UnidentifiedTransportMessage_8032B6D4::~UnidentifiedTransportMessage_8032B6D4()
{
}

extern "C" void fn_8032B81C(UnidentifiedTransportMessage_8032B6D4* message)
{
    int i;
    for (i = 0; i < message->mReliableCount; ++i)
    {
        fn_8032644C(message->mReliable[i]);
        message->mReliable[i] = 0;
    }
    message->mReliableCount = 0;

    for (i = 0; i < message->mUnreliableCount; ++i)
    {
        fn_8032644C(message->mUnreliable[i]);
        message->mUnreliable[i] = 0;
    }
    message->mUnreliableCount = 0;

    for (i = 0; i < message->mVoiceCount; ++i)
    {
        fn_8032644C(message->mVoice[i]);
        message->mVoice[i] = 0;
    }
    message->mVoiceCount = 0;
}

extern "C" void fn_8032B8FC(
    UnidentifiedTransportMessage_8032B6D4* message, const u16* value)
{
    message->mFlags |= 0x40;
    message->mUnidentified0A = *value;
    message->mSize += 2;
}

extern "C" u32 fn_8032B920(UnidentifiedTransportMessage_8032B6D4* message)
{
    message->mFlags |= 4;
    message->mTimestamp = nlGetTicker();
    message->mSize += 4;
    return message->mTimestamp;
}

extern "C" void fn_8032B968(
    UnidentifiedTransportMessage_8032B6D4* message, u32 value, u16 other)
{
    message->mFlags |= 8;
    message->mUnidentified14 = value;
    message->mUnidentified18 = other;
    message->mSize += 6;
}

extern "C" void fn_8032B98C(UnidentifiedTransportMessage_8032B6D4* message)
{
    message->mFlags |= 0x10;
}

extern "C" bool fn_8032B99C(UnidentifiedTransportMessage_8032B6D4* message,
    UnidentifiedTransportPacket_803263E4* packet)
{
    if (message->mUnreliableCount == 16)
    {
        fn_8004F594(0x10, lbl_80532378);
        return false;
    }
    int size = message->mSize;
    if (size + fn_80326754(packet) > 0x587)
    {
        fn_8004F594(0x10, lbl_805323B8);
        return false;
    }

    message->mFlags |= 0x20;
    message->mUnreliable[message->mUnreliableCount] = packet;
    ++message->mUnreliableCount;
    message->mSize += fn_80326754(packet);
    return true;
}

extern "C" bool fn_8032BA74(UnidentifiedTransportMessage_8032B6D4* message,
    UnidentifiedTransportPacket_803263E4* packet)
{
    if (message->mVoiceCount == 16)
    {
        fn_8004F594(0x10, lbl_805323F4);
        return false;
    }
    int size = message->mSize;
    if (size + fn_80326754(packet) > 0x587)
    {
        fn_8004F594(0x10, lbl_8053241C);
        return false;
    }

    message->mFlags |= 2;
    message->mVoice[message->mVoiceCount] = packet;
    ++message->mVoiceCount;
    message->mSize += fn_80326754(packet);
    return true;
}

extern "C" bool fn_8032BB4C(UnidentifiedTransportMessage_8032B6D4* message,
    UnidentifiedTransportPacket_803263E4* packet)
{
    if (message->mReliableCount == 16)
    {
        fn_8004F594(0x10, lbl_80532454);
        return false;
    }
    int size = message->mSize;
    if (size + fn_8032666C(packet) > 0x587)
    {
        fn_8004F594(0x10, lbl_80532480);
        return false;
    }

    message->mReliable[message->mReliableCount] = packet;
    ++message->mReliableCount;
    message->mSize += fn_8032666C(packet);
    return true;
}

extern "C" bool fn_8032BC18(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return (message->mFlags & 0x40) != 0;
}

extern "C" bool fn_8032BC24(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return (message->mFlags & 0x80) != 0;
}

extern "C" bool fn_8032BC30(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return (message->mFlags & 2) != 0;
}

extern "C" bool fn_8032BC3C(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return (message->mFlags & 4) != 0;
}

extern "C" bool fn_8032BC48(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return (message->mFlags & 8) != 0;
}

extern "C" bool fn_8032BC54(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return (message->mFlags & 0x10) != 0;
}

extern "C" bool fn_8032BC60(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    if (message->mReliableCount == 1
        && message->mReliable[0]->mUnidentified04 == 0xE0)
    {
        return true;
    }
    return false;
}

extern "C" int fn_8032BC8C(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return message->mReliableCount;
}

extern "C" UnidentifiedTransportPacket_803263E4* fn_8032BC94(
    const UnidentifiedTransportMessage_8032B6D4* message, unsigned int index)
{
    return message->mReliable[index];
}

extern "C" int fn_8032BCA4(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    if ((message->mFlags & 0x20) != 0)
    {
        return message->mUnreliableCount;
    }
    return 0;
}

extern "C" UnidentifiedTransportPacket_803263E4* fn_8032BCC0(
    const UnidentifiedTransportMessage_8032B6D4* message, unsigned int index)
{
    return message->mUnreliable[index];
}

extern "C" int fn_8032BCD0(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    if ((message->mFlags & 2) != 0)
    {
        return message->mVoiceCount;
    }
    return 0;
}

extern "C" UnidentifiedTransportPacket_803263E4* fn_8032BCEC(
    const UnidentifiedTransportMessage_8032B6D4* message, unsigned int index)
{
    return message->mVoice[index];
}

extern "C" u32 fn_8032BCFC(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return message->mUnidentified0A << 16;
}

extern "C" u32 fn_8032BD08(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return message->mTimestamp;
}

extern "C" u32 fn_8032BD10(
    UnidentifiedTransportMessage_8032B6D4* message, u16* other)
{
    *other = message->mUnidentified18;
    return message->mUnidentified14;
}

void UnidentifiedTransportMessage_8032B6D4::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    UnidentifiedTransportMessage_8032B6D4* message = this;
    serializer->Transfer(&message->mUnidentified04,
        sizeof(message->mUnidentified04));
    serializer->Transfer(&message->mFlags, sizeof(message->mFlags));

    if ((message->mFlags & 0x40) != 0)
    {
        serializer->Transfer(&message->mUnidentified0A,
            sizeof(message->mUnidentified0A));
    }
    if ((message->mFlags & 0x80) != 0)
    {
        serializer->Transfer(&message->mUnidentified0C,
            sizeof(message->mUnidentified0C));
        serializer->Transfer(&message->mUnidentified0E,
            sizeof(message->mUnidentified0E));
    }
    if ((message->mFlags & 4) != 0)
    {
        serializer->Transfer(&message->mTimestamp, sizeof(message->mTimestamp));
    }
    if ((message->mFlags & 8) != 0)
    {
        serializer->Transfer(&message->mUnidentified14,
            sizeof(message->mUnidentified14));
        serializer->Transfer(&message->mUnidentified18,
            sizeof(message->mUnidentified18));
    }

    if ((message->mFlags & 0x20) != 0)
    {
        serializer->Transfer(
            &message->mUnreliableCount, sizeof(message->mUnreliableCount));
        if (serializer->mDirection == 0)
        {
            for (int i = 0; i < message->mUnreliableCount; ++i)
            {
                UnidentifiedTransportPacket_803263E4* packet =
                    new (8, false) UnidentifiedTransportPacket_803263E4;
                message->mUnreliable[i] = packet;
                fn_80326684(packet, serializer);
                message->mUnreliable[i]->mUnidentified00 = 0;
                message->mUnreliable[i]->mUnidentified04 = 0xE6;
            }
        }
        else
        {
            for (int i = 0; i < message->mUnreliableCount; ++i)
            {
                fn_80326684(message->mUnreliable[i], serializer);
            }
        }
    }

    serializer->Transfer(&message->mReliableCount,
        sizeof(message->mReliableCount));
    if (serializer->mDirection == 0)
    {
        for (int i = 0; i < message->mReliableCount; ++i)
        {
            UnidentifiedTransportPacket_803263E4* packet =
                new (8, false) UnidentifiedTransportPacket_803263E4;
            message->mReliable[i] = packet;
            fn_80326464(packet, serializer);
            message->mReliable[i]->mUnidentified00 = 0;
        }
    }
    else
    {
        for (int i = 0; i < message->mReliableCount; ++i)
        {
            fn_80326464(message->mReliable[i], serializer);
        }
    }
}

extern "C" void fn_8032C184(UnidentifiedTransportMessage_8032B6D4* message,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&message->mVoiceCount, sizeof(message->mVoiceCount));
    if (serializer->mDirection == 0)
    {
        for (int i = 0; i < message->mVoiceCount; ++i)
        {
            UnidentifiedTransportPacket_803263E4* packet =
                new (8, false) UnidentifiedTransportPacket_803263E4;
            message->mVoice[i] = packet;
            fn_80326684(packet, serializer);
            message->mVoice[i]->mUnidentified00 = 0;
            message->mVoice[i]->mUnidentified04 = 0xE7;
        }
    }
    else
    {
        for (int i = 0; i < message->mVoiceCount; ++i)
        {
            fn_80326684(message->mVoice[i], serializer);
        }
    }
}

void UnidentifiedClientChallenge_8032C294::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mPayload, sizeof(mPayload));
}

void UnidentifiedServerChallenge_8032C308::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mUnidentified04, sizeof(mUnidentified04));
    serializer->Transfer(mUnidentified24, sizeof(mUnidentified24));
}

void UnidentifiedClientResponse_8032C3CC::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(mPayload, sizeof(mPayload));
}

void UnidentifiedServerResponse_8032C440::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
}

void UnidentifiedClosing_8032C4B4::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified04, sizeof(mUnidentified04));
}

void UnidentifiedKeepAlive_8032C528::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified04, sizeof(mUnidentified04));
}

extern "C" void fn_8032C59C()
{
    CBlowFish* codec = new (8, false) CBlowFish;
    lbl_806E20F8 = codec;
    codec->Initialize(lbl_806DF708, 8);
}

extern "C" void fn_8032C5E4(u32* values)
{
    u32* position = values;
    u32 byteOffset = 0;
    do
    {
        *position = fn_803236CC();
        byteOffset += sizeof(*values);
        ++position;
    } while (byteOffset < 0x20);
}

extern "C" void fn_8032C630(unsigned char* output, unsigned char* input)
{
    lbl_806E20F8->Encode(input, output, 0x20);
}

extern "C" bool fn_8032C640(const void* first, const void* second)
{
    return memcmp(first, second, 0x20) == 0;
}

extern "C" void* fn_8032C66C(unsigned long, unsigned int, bool)
{
    return lbl_80584840.Allocate();
}

void UnidentifiedNetworkMessage::operator delete(void* message)
{
    lbl_80584840.Free(
        (UnidentifiedNetworkMessageStorage_8032C66C*)message);
}
