#include "Game/DetermDataEvent.h"

#include <string.h>

#include "Game/NetworkMessageSerializer.h"

SlotPool<DetermDataEvent> gDetermDataEventPool(10, 0);

DetermDataEvent::DetermDataEvent()
    : mSize(0)
{
}

DetermDataEvent::DetermDataEvent(DetermDataEvent* other)
{
    mSize = other->mSize;
    memcpy(mData, other->mData, other->mSize);
}

DetermDataEvent::DetermDataEvent(const void* data, u32 size)
    : mSize(size)
{
    memcpy(mData, data, size);
}

void DetermDataEvent::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mSize, 1);
    serializer->Transfer(mData, mSize);
}
