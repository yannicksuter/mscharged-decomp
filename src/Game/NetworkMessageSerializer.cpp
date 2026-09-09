#include "Game/NetworkMessageSerializer.h"
#include "NL/nlMemory.h"

NetworkMessageSerializer::NetworkMessageSerializer(
    int direction, u8* buffer, unsigned long size)
    : mDirection(direction)
    , mPosition(buffer)
    , mBuffer(buffer)
    , mEnd(buffer + size)
    , mOwnsBuffer(false)
{
}

NetworkMessageSerializer::~NetworkMessageSerializer()
{
    if (mOwnsBuffer && mBuffer != 0)
    {
        delete[] mBuffer;
        mBuffer = 0;
    }
}
