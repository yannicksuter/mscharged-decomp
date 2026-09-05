#include "Game/NetworkMessages.h"
#include "NL/nlMemory.h"

UnidentifiedMessageSerializer::UnidentifiedMessageSerializer(
    int direction, u8* buffer, unsigned long size)
    : mDirection(direction)
    , mPosition(buffer)
    , mBuffer(buffer)
    , mEnd(buffer + size)
    , mOwnsBuffer(false)
{
}

UnidentifiedMessageSerializer::~UnidentifiedMessageSerializer()
{
    if (mOwnsBuffer && mBuffer != 0)
    {
        delete[] mBuffer;
        mBuffer = 0;
    }
}
