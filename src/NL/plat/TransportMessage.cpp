#include "Game/NetworkMessage.h"
#include "Game/Sys/debug.h"
#include "Game/NetworkRandom.h"
#include "NL/blowfish.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTicker.h"
#include "NL/plat/TransportPacket.h"
#include "NL/plat/TransportMessage.h"

struct NetworkMessageStorage
{
    u8 mStorage[0x800];
};

SlotPool<NetworkMessageStorage> gNetworkMessagePool(15, 0);
CBlowFish* gTransportChallengeCipher;
static u8 sTransportChallengeKey[8] = {
    0x17, 0xED, 0xFF, 0x12, 0x61, 0x40, 0x25, 0xB2
};


TransportMessage::TransportMessage()
{
    mChecksum = 0;
    mFlags = 0;
    mAck = 0;
    mUnidentified0C = 0;
    mUnidentified0E = 0;
    mTimestamp = 0;
    mPongTimestamp = 0;
    mPongDelayMS = 0;
    mUnreliableCount = 0;
    mVoiceCount = 0;
    mReliableCount = 0;
    for (int i = 0; i < 16; ++i)
    {
        mReliable[i] = 0;
    }
    for (int i = 0; i < 16; ++i)
    {
        mUnreliable[i] = 0;
    }
    for (int i = 0; i < 16; ++i)
    {
        mVoice[i] = 0;
    }
    mSize = 6;
}

TransportMessage::~TransportMessage()
{
}

void TransportMessage::ReleasePackets()
{
    int i;
    for (i = 0; i < mReliableCount; ++i)
    {
        delete mReliable[i];
        mReliable[i] = 0;
    }
    mReliableCount = 0;

    for (i = 0; i < mUnreliableCount; ++i)
    {
        delete mUnreliable[i];
        mUnreliable[i] = 0;
    }
    mUnreliableCount = 0;

    for (i = 0; i < mVoiceCount; ++i)
    {
        delete mVoice[i];
        mVoice[i] = 0;
    }
    mVoiceCount = 0;
}

void TransportMessage::SetAck(const TransportAck& value)
{
    mFlags |= 0x40;
    mAck = value.mSequence;
    mSize += 2;
}

u32 TransportMessage::AddPing()
{
    mFlags |= 4;
    mTimestamp = nlGetTicker();
    mSize += 4;
    return mTimestamp;
}

void TransportMessage::AddPong(unsigned int timestamp, u16 delayMS)
{
    mFlags |= 8;
    mPongTimestamp = timestamp;
    mPongDelayMS = delayMS;
    mSize += 6;
}

void TransportMessage::SetClosed()
{
    mFlags |= 0x10;
}

bool TransportMessage::AddUnreliablePacket(TransportPacket* packet)
{
    if (mUnreliableCount == 16)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to add unreliable payload..too many unreliable payloads\n");
        return false;
    }
    int size = mSize;
    if (size + packet->GetUnreliableSize() > 0x587)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to add unreliable payload..size would be too great\n");
        return false;
    }

    mFlags |= 0x20;
    mUnreliable[mUnreliableCount] = packet;
    ++mUnreliableCount;
    mSize += packet->GetUnreliableSize();
    return true;
}

bool TransportMessage::AddVoicePacket(TransportPacket* packet)
{
    if (mVoiceCount == 16)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to add voice payload..too many\n");
        return false;
    }
    int size = mSize;
    if (size + packet->GetUnreliableSize() > 0x587)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to add voice payload..size would be too great\n");
        return false;
    }

    mFlags |= 2;
    mVoice[mVoiceCount] = packet;
    ++mVoiceCount;
    mSize += packet->GetUnreliableSize();
    return true;
}

bool TransportMessage::AddReliablePacket(TransportPacket* packet)
{
    if (mReliableCount == 16)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to add reliable payload..too many\n");
        return false;
    }
    int size = mSize;
    if (size + packet->GetReliableSize() > 0x587)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Failed to add reliable payload..size would be too great\n");
        return false;
    }

    mReliable[mReliableCount] = packet;
    ++mReliableCount;
    mSize += packet->GetReliableSize();
    return true;
}

bool TransportMessage::HasAck() const
{
    return (mFlags & 0x40) != 0;
}

bool TransportMessage::HasControlFlag80() const
{
    return (mFlags & 0x80) != 0;
}

bool TransportMessage::HasVoice() const
{
    return (mFlags & 2) != 0;
}

bool TransportMessage::HasPing() const
{
    return (mFlags & 4) != 0;
}

bool TransportMessage::HasPong() const
{
    return (mFlags & 8) != 0;
}

bool TransportMessage::IsClosed() const
{
    return (mFlags & 0x10) != 0;
}

bool TransportMessage::IsClientChallenge() const
{
    if (mReliableCount == 1
        && mReliable[0]->mType == 0xE0)
    {
        return true;
    }
    return false;
}

int TransportMessage::GetReliableCount() const
{
    return mReliableCount;
}

TransportPacket* TransportMessage::GetReliablePacket(unsigned int index) const
{
    return mReliable[index];
}

int TransportMessage::GetUnreliableCount() const
{
    if ((mFlags & 0x20) != 0)
    {
        return mUnreliableCount;
    }
    return 0;
}

TransportPacket* TransportMessage::GetUnreliablePacket(unsigned int index) const
{
    return mUnreliable[index];
}

int TransportMessage::GetVoiceCount() const
{
    if ((mFlags & 2) != 0)
    {
        return mVoiceCount;
    }
    return 0;
}

TransportPacket* TransportMessage::GetVoicePacket(unsigned int index) const
{
    return mVoice[index];
}

TransportAck TransportMessage::GetAck() const
{
    return TransportAck(mAck);
}

u32 TransportMessage::GetPingTimestamp() const
{
    return mTimestamp;
}

u32 TransportMessage::GetPongTimestamp(u16* delayMS)
{
    *delayMS = mPongDelayMS;
    return mPongTimestamp;
}

void TransportMessage::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mChecksum,
        sizeof(mChecksum));
    serializer->Transfer(&mFlags, sizeof(mFlags));

    if ((mFlags & 0x40) != 0)
    {
        serializer->Transfer(&mAck,
            sizeof(mAck));
    }
    if ((mFlags & 0x80) != 0)
    {
        serializer->Transfer(&mUnidentified0C,
            sizeof(mUnidentified0C));
        serializer->Transfer(&mUnidentified0E,
            sizeof(mUnidentified0E));
    }
    if ((mFlags & 4) != 0)
    {
        serializer->Transfer(&mTimestamp, sizeof(mTimestamp));
    }
    if ((mFlags & 8) != 0)
    {
        serializer->Transfer(&mPongTimestamp,
            sizeof(mPongTimestamp));
        serializer->Transfer(&mPongDelayMS,
            sizeof(mPongDelayMS));
    }

    if ((mFlags & 0x20) != 0)
    {
        serializer->Transfer(
            &mUnreliableCount, sizeof(mUnreliableCount));
        if (serializer->mDirection == 0)
        {
            for (int i = 0; i < mUnreliableCount; ++i)
            {
                TransportPacket* packet =
                    new (8, false) TransportPacket;
                mUnreliable[i] = packet;
                packet->SerializeUnreliable(serializer);
                mUnreliable[i]->mLastSendTick = 0;
                mUnreliable[i]->mType = 0xE6;
            }
        }
        else
        {
            for (int i = 0; i < mUnreliableCount; ++i)
            {
                mUnreliable[i]->SerializeUnreliable(serializer);
            }
        }
    }

    serializer->Transfer(&mReliableCount,
        sizeof(mReliableCount));
    if (serializer->mDirection == 0)
    {
        for (int i = 0; i < mReliableCount; ++i)
        {
            TransportPacket* packet =
                new (8, false) TransportPacket;
            mReliable[i] = packet;
            packet->SerializeReliable(serializer);
            mReliable[i]->mLastSendTick = 0;
        }
    }
    else
    {
        for (int i = 0; i < mReliableCount; ++i)
        {
            mReliable[i]->SerializeReliable(serializer);
        }
    }
}

void TransportMessage::SerializeVoice(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mVoiceCount, sizeof(mVoiceCount));
    if (serializer->mDirection == 0)
    {
        for (int i = 0; i < mVoiceCount; ++i)
        {
            TransportPacket* packet =
                new (8, false) TransportPacket;
            mVoice[i] = packet;
            packet->SerializeUnreliable(serializer);
            mVoice[i]->mLastSendTick = 0;
            mVoice[i]->mType = 0xE7;
        }
    }
    else
    {
        for (int i = 0; i < mVoiceCount; ++i)
        {
            mVoice[i]->SerializeUnreliable(serializer);
        }
    }
}

void TransportClientChallenge::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mPayload, sizeof(mPayload));
}

void TransportServerChallenge::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mClientResponse, sizeof(mClientResponse));
    serializer->Transfer(mChallenge, sizeof(mChallenge));
}

void TransportClientResponse::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(mPayload, sizeof(mPayload));
}

void TransportServerResponse::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
}

void TransportClosing::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified04, sizeof(mUnidentified04));
}

void TransportKeepAlive::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified04, sizeof(mUnidentified04));
}

void InitializeTransportChallengeCipher()
{
    CBlowFish* codec = new (8, false) CBlowFish;
    gTransportChallengeCipher = codec;
    codec->Initialize(sTransportChallengeKey, 8);
}

void GenerateTransportChallenge(unsigned int* values)
{
    u32* position = values;
    u32 byteOffset = 0;
    do
    {
        *position = NetworkRandom();
        byteOffset += sizeof(*values);
        ++position;
    } while (byteOffset < 0x20);
}

void EncodeTransportChallenge(unsigned char* output, unsigned char* input)
{
    gTransportChallengeCipher->Encode(input, output, 0x20);
}

bool CompareTransportChallenge(const void* first, const void* second)
{
    return memcmp(first, second, 0x20) == 0;
}

void* NetworkMessage::operator new(unsigned long, unsigned int, bool)
{
    return gNetworkMessagePool.Allocate();
}

void NetworkMessage::operator delete(void* message)
{
    gNetworkMessagePool.Free(
        (NetworkMessageStorage*)message);
}
