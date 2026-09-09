#ifndef GAME_DETERM_DATA_EVENT_H
#define GAME_DETERM_DATA_EVENT_H

#include "NL/nlSlotPool.h"
#include "types.h"

class NetworkMessageSerializer;
struct DetermDataEvent;

extern SlotPool<DetermDataEvent> gDetermDataEventPool;

// Variable-length deterministic data packet. The first byte is the payload
// size; the remaining 32 bytes are copied to and from network messages.
struct DetermDataEvent
{
    DetermDataEvent();
    DetermDataEvent(DetermDataEvent* other);
    DetermDataEvent(const void* data, u32 size);
    ~DetermDataEvent();

    void* operator new(unsigned long)
    {
        return gDetermDataEventPool.Allocate();
    }

    void operator delete(void* data)
    {
        gDetermDataEventPool.Free((DetermDataEvent*)data);
    }

    void Serialize(NetworkMessageSerializer* serializer);

    /* 0x00 */ u8 mSize;
    /* 0x01 */ u8 mPadding01[3];
    /* 0x04 */ u8 mData[0x20];
}; // size: 0x24

#endif // GAME_DETERM_DATA_EVENT_H
