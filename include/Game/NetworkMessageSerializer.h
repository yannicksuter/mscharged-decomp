#ifndef GAME_MESSAGE_SERIALIZER_H
#define GAME_MESSAGE_SERIALIZER_H

#include <string.h>

#include "types.h"

class NetworkMessageSerializer
{
public:
    NetworkMessageSerializer(int direction, u8* buffer, unsigned long size);
    ~NetworkMessageSerializer();

    int GetLength() const { return mPosition - mBuffer; }

    void Transfer(void* value, unsigned long size)
    {
        if (mDirection == 0)
        {
            memcpy(value, mPosition, size);
            mPosition += size;
        }
        else
        {
            memcpy(mPosition, value, size);
            mPosition += size;
        }
    }

    /* 0x00 */ int mDirection;
    /* 0x04 */ u8* mPosition;
    /* 0x08 */ u8* mBuffer;
    /* 0x0C */ u8* mEnd;
    /* 0x10 */ bool mOwnsBuffer;
}; // size: 0x14

#endif // GAME_MESSAGE_SERIALIZER_H
