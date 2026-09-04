#include "Game/NetworkMessages.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_803263E4.h"

struct UnidentifiedTransportMessage_8032B6D4
{
    /* 0x000 */ void* mVTable;
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

struct UnidentifiedPayload32_8032C294
{
    void* mVTable;
    u8 mPayload[0x20];
};

struct UnidentifiedPayload64_8032C308
{
    void* mVTable;
    u8 mPayload[0x40];
};

struct UnidentifiedPayloadByte_8032C440
{
    void* mVTable;
    u8 mPayload;
};

struct UnidentifiedNetworkMessageStorage_8032C66C
{
    u8 mStorage[0x800];
};

extern void* lbl_805324BC[];
extern const char lbl_80532378[];
extern const char lbl_805323B8[];
extern const char lbl_805323F4[];
extern const char lbl_8053241C[];
extern const char lbl_80532454[];
extern const char lbl_80532480[];
SlotPool<UnidentifiedNetworkMessageStorage_8032C66C> lbl_80584840(15, 0);
void* lbl_806E20F8;
extern u8 lbl_806DF708[8];

extern "C"
{
    int fn_8004F594(int channel, const char* format, ...);
    u32 fn_803236CC();
    void* fn_80322924(void* memory);
    void fn_80322D40(void* codec, const void* key, unsigned int size);
    void fn_803232BC(
        void* codec, void* output, const void* input, unsigned int size);
}

extern "C" UnidentifiedTransportMessage_8032B6D4* fn_8032B6D4(
    UnidentifiedTransportMessage_8032B6D4* message)
{
    message->mUnidentified04 = 0;
    message->mVTable = lbl_805324BC;
    message->mFlags = 0;
    message->mUnidentified0A = 0;
    message->mUnidentified0C = 0;
    message->mUnidentified0E = 0;
    message->mTimestamp = 0;
    message->mUnidentified14 = 0;
    message->mUnidentified18 = 0;
    message->mUnreliableCount = 0;
    message->mVoiceCount = 0;
    message->mReliableCount = 0;
    message->mReliable[0] = 0;
    message->mReliable[1] = 0;
    message->mReliable[2] = 0;
    message->mReliable[3] = 0;
    message->mReliable[4] = 0;
    message->mReliable[5] = 0;
    message->mReliable[6] = 0;
    message->mReliable[7] = 0;
    message->mReliable[8] = 0;
    message->mReliable[9] = 0;
    message->mReliable[10] = 0;
    message->mReliable[11] = 0;
    message->mReliable[12] = 0;
    message->mReliable[13] = 0;
    message->mReliable[14] = 0;
    message->mReliable[15] = 0;
    message->mUnreliable[0] = 0;
    message->mUnreliable[1] = 0;
    message->mUnreliable[2] = 0;
    message->mUnreliable[3] = 0;
    message->mUnreliable[4] = 0;
    message->mUnreliable[5] = 0;
    message->mUnreliable[6] = 0;
    message->mUnreliable[7] = 0;
    message->mUnreliable[8] = 0;
    message->mUnreliable[9] = 0;
    message->mUnreliable[10] = 0;
    message->mUnreliable[11] = 0;
    message->mUnreliable[12] = 0;
    message->mUnreliable[13] = 0;
    message->mUnreliable[14] = 0;
    message->mUnreliable[15] = 0;
    message->mVoice[0] = 0;
    message->mVoice[1] = 0;
    message->mVoice[2] = 0;
    message->mVoice[3] = 0;
    message->mVoice[4] = 0;
    message->mVoice[5] = 0;
    message->mVoice[6] = 0;
    message->mVoice[7] = 0;
    message->mVoice[8] = 0;
    message->mVoice[9] = 0;
    message->mVoice[10] = 0;
    message->mVoice[11] = 0;
    message->mVoice[12] = 0;
    message->mVoice[13] = 0;
    message->mVoice[14] = 0;
    message->mVoice[15] = 0;
    message->mSize = 6;
    return message;
}

extern "C" void* fn_8032B7DC(void* message, int shouldDelete)
{
    if (message != 0 && shouldDelete > 0)
    {
        ::operator delete(message);
    }
    return message;
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

extern "C" u8 fn_8032BC8C(
    const UnidentifiedTransportMessage_8032B6D4* message)
{
    return message->mReliableCount;
}

extern "C" UnidentifiedTransportPacket_803263E4* fn_8032BC94(
    const UnidentifiedTransportMessage_8032B6D4* message, unsigned int index)
{
    return message->mReliable[index];
}

extern "C" u8 fn_8032BCA4(
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

extern "C" u8 fn_8032BCD0(
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

extern "C" void fn_8032BD20(UnidentifiedTransportMessage_8032B6D4* message,
    UnidentifiedMessageSerializer* serializer)
{
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
        int i;
        if (serializer->mDirection == 0)
        {
            for (i = 0; i < message->mUnreliableCount; ++i)
            {
                UnidentifiedTransportPacket_803263E4* packet =
                    (UnidentifiedTransportPacket_803263E4*)fn_803263E4(
                        sizeof(UnidentifiedTransportPacket_803263E4), 8,
                        false);
                if (packet != 0)
                {
                    packet->mUnidentified06 = 0;
                }
                message->mUnreliable[i] = packet;
                fn_80326684(packet, serializer);
                message->mUnreliable[i]->mUnidentified00 = 0;
                message->mUnreliable[i]->mUnidentified04 = 0xE6;
            }
        }
        else
        {
            for (i = 0; i < message->mUnreliableCount; ++i)
            {
                fn_80326684(message->mUnreliable[i], serializer);
            }
        }
    }

    serializer->Transfer(&message->mReliableCount,
        sizeof(message->mReliableCount));
    int i;
    if (serializer->mDirection == 0)
    {
        for (i = 0; i < message->mReliableCount; ++i)
        {
            UnidentifiedTransportPacket_803263E4* packet =
                (UnidentifiedTransportPacket_803263E4*)fn_803263E4(
                    sizeof(UnidentifiedTransportPacket_803263E4), 8, false);
            if (packet != 0)
            {
                packet->mUnidentified06 = 0;
            }
            message->mReliable[i] = packet;
            fn_80326464(packet, serializer);
            message->mReliable[i]->mUnidentified00 = 0;
        }
    }
    else
    {
        for (i = 0; i < message->mReliableCount; ++i)
        {
            fn_80326464(message->mReliable[i], serializer);
        }
    }
}

extern "C" void fn_8032C184(UnidentifiedTransportMessage_8032B6D4* message,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&message->mVoiceCount, sizeof(message->mVoiceCount));
    int i;
    if (serializer->mDirection == 0)
    {
        for (i = 0; i < message->mVoiceCount; ++i)
        {
            UnidentifiedTransportPacket_803263E4* packet =
                (UnidentifiedTransportPacket_803263E4*)fn_803263E4(
                    sizeof(UnidentifiedTransportPacket_803263E4), 8, false);
            if (packet != 0)
            {
                packet->mUnidentified06 = 0;
            }
            message->mVoice[i] = packet;
            fn_80326684(packet, serializer);
            message->mVoice[i]->mUnidentified00 = 0;
            message->mVoice[i]->mUnidentified04 = 0xE7;
        }
    }
    else
    {
        for (i = 0; i < message->mVoiceCount; ++i)
        {
            fn_80326684(message->mVoice[i], serializer);
        }
    }
}

extern "C" void fn_8032C294(
    UnidentifiedPayload32_8032C294* payload,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(payload->mPayload, sizeof(payload->mPayload));
}

extern "C" void fn_8032C308(
    UnidentifiedPayload64_8032C308* payload,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(payload->mPayload, 0x20);
    serializer->Transfer(payload->mPayload + 0x20, 0x20);
}

extern "C" void fn_8032C3CC(
    UnidentifiedPayload32_8032C294* payload,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(payload->mPayload, sizeof(payload->mPayload));
}

extern "C" void fn_8032C440(
    UnidentifiedPayloadByte_8032C440* payload,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&payload->mPayload, sizeof(payload->mPayload));
}

extern "C" void fn_8032C4B4(
    UnidentifiedPayloadByte_8032C440* payload,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&payload->mPayload, sizeof(payload->mPayload));
}

extern "C" void fn_8032C528(
    UnidentifiedPayloadByte_8032C440* payload,
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&payload->mPayload, sizeof(payload->mPayload));
}

extern "C" void fn_8032C59C()
{
    void* codec = nlMalloc(8, 8, false);
    if (codec != 0)
    {
        codec = fn_80322924(codec);
    }
    lbl_806E20F8 = codec;
    fn_80322D40(codec, lbl_806DF708, 8);
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

extern "C" void fn_8032C630(const void* input, void* output)
{
    fn_803232BC(lbl_806E20F8, output, input, 0x20);
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
