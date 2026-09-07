#ifndef NL_UNIDENTIFIED_QUEUE_H
#define NL_UNIDENTIFIED_QUEUE_H

#include "types.h"

template <class T>
class UnidentifiedQueueBase
{
public:
    UnidentifiedQueueBase(T* storage, unsigned int capacity)
        : mHead(0)
        , mCount(0)
        , mCapacity(capacity)
    {
        mBuffer = storage;
    }

    int GetCount() { return mCount; }
    bool IsFull() { return mCount >= mCapacity; }
    T& Peek() { return mBuffer[mHead]; }
    T& operator[](int index) { return mBuffer[(mHead + index) % mCapacity]; }

    T* UnidentifiedPushSlot()
    {
        if (IsFull())
        {
            return 0;
        }
        return &mBuffer[(mHead + mCount++) % mCapacity];
    }

    void Push(const T& value) { *UnidentifiedPushSlot() = value; }

    T& Pop()
    {
        if (mCount == 0)
        {
            return mBuffer[mHead & mCapacity];
        }
        mCount--;
        T& value = mBuffer[mHead];
        mHead++;
        if (mHead % mCapacity == 0)
        {
            mHead = 0;
        }
        return value;
    }

    /* 0x00 */ T* mBuffer;
    /* 0x04 */ u32 mHead;
    /* 0x08 */ int mCount;
    /* 0x0C */ u32 mCapacity;
};

template <class T, int N>
class UnidentifiedTransportQueue : public UnidentifiedQueueBase<T>
{
public:
    UnidentifiedTransportQueue()
        : UnidentifiedQueueBase<T>(mStorage, N)
    {
    }

    T mStorage[N];
};

#endif // NL_UNIDENTIFIED_QUEUE_H
