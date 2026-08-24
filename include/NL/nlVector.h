#ifndef NL_VECTOR_H
#define NL_VECTOR_H

template <typename T, typename Allocator>
class Vector
{
public:
    Vector()
        : mData(0)
        , mSize(0)
        , mCapacity(0)
    {
    }

    Vector(const T* string)
    {
        mData = 0;
        mSize = 0;
        mCapacity = 0;

        const T* scan = string;
        while (*scan++ != 0)
        {
            ++mSize;
        }

        ++mSize;
        mData = Allocator::template New<T>(mSize + 1, 0);
        mCapacity = mSize;

        for (int i = 0; i < mSize; ++i)
        {
            mData[i] = *string++;
        }
    }

    Vector(const Vector& other, const char* name = 0)
    {
        mData = Allocator::template New<T>(other.mSize, name);
        mSize = other.mSize;
        mCapacity = other.mSize;
        for (int i = 0; i < mSize; ++i)
        {
            mData[i] = other.mData[i];
        }
    }

    Vector(int count, const char* name)
    {
        mData = Allocator::template New<T>(count, name);
        mSize = count;
        mCapacity = count;
        for (int i = 0; i < count; ++i)
        {
            mData[i] = T();
        }
    }

    ~Vector()
    {
        Allocator::template Delete<T>(mData);
    }

    void reserve(int capacity)
    {
        if (mCapacity < capacity)
        {
            Vector other(capacity, 0);
            for (int i = 0; i < mSize; ++i)
            {
                other.mData[i] = mData[i];
            }
            other.mSize = mSize;
            Swap(other);
        }
    }

    void erase(const T* begin, const T* end)
    {
        const T* eraseEnd = end;
        int count = end - begin;
        int offset = begin - mData;
        T* at = mData + offset;
        while (eraseEnd != mData + mSize)
        {
            *at++ = *eraseEnd++;
        }
        mSize -= count;
    }

    void Swap(Vector& other)
    {
        int oldSize = mSize;
        mSize = other.mSize;
        other.mSize = oldSize;

        int oldCapacity = mCapacity;
        mCapacity = other.mCapacity;
        other.mCapacity = oldCapacity;

        T* oldData = mData;
        mData = other.mData;
        other.mData = oldData;
    }

    T& operator[](int index)
    {
        return mData[index];
    }

    T* mData;
    int mSize;
    int mCapacity;
};

#endif // NL_VECTOR_H
