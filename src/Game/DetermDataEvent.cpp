#include "Game/DetermDataEvent.h"

#include <string.h>

#include "Game/NetworkMessages.h"

SlotPool<DetermDataEvent> lbl_805848E8(10, 0);

DetermDataEvent::DetermDataEvent()
    : mSize(0)
{
}

DetermDataEvent::DetermDataEvent(const DetermDataEvent* other)
{
    mSize = other->mSize;
    memcpy(mData, other->mData, other->mSize);
}

DetermDataEvent::DetermDataEvent(const void* data, u32 size)
    : mSize(size)
{
    memcpy(mData, data, size);
}

void DetermDataEvent::fn_80331A34(UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mSize, 1);
    serializer->Transfer(mData, mSize);
}
