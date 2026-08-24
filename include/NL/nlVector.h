#ifndef _NLVECTOR_H_
#define _NLVECTOR_H_

template <typename T, typename Allocator>
class Vector
{
public:
    Vector()
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
            mSize++;
        }

        mSize++;
        mData = Allocator::template New<T>(mSize + 1, 0);
        mCapacity = mSize;

        for (int i = 0; i < mSize; i++)
        {
            mData[i] = *string++;
        }
    }

    Vector(const Vector& other, const char* name = 0)
    {
        mData = Allocator::template New<T>(other.mSize, name);
        mSize = other.mSize;
        mCapacity = other.mSize;
        for (int i = 0; i < mSize; i++)
        {
            mData[i] = other.mData[i];
        }
    }

    Vector(int count, const char* name)
    {
        mData = Allocator::template New<T>(count, name);
        mSize = count;
        mCapacity = count;
        for (int i = 0; i < count; i++)
        {
            mData[i] = T();
        }
    }

    ~Vector();

    Vector& operator=(const Vector& other)
    {
        if (mSize >= other.mSize)
        {
            for (int i = 0; i < other.mSize; i++)
            {
                mData[i] = other.mData[i];
            }
            mSize = other.mSize;
        }
        else
        {
            Vector tmp(other);
            Swap(tmp);
        }
        return *this;
    }

    void Swap(Vector& other);

    void insert(T* at, const T* begin, const T* end)
    {
        int size = end - begin;
        int offset = at - mData;
        reserve(mSize + size);
        at = mData + offset;
        T* item = mData + mSize - 1;
        while (item >= at)
        {
            *(item + size) = *item;
            item--;
        }
        while (begin != end)
        {
            *at = *begin;
            begin++;
            at++;
        }
        mSize += size;
    }

    void reserve(int capacity)
    {
        if (mCapacity < capacity)
        {
            Vector other(capacity, 0);
            for (int i = 0; i < mSize; i++)
            {
                other.mData[i] = mData[i];
            }
            other.mSize = mSize;
            Swap(other);
        }
    }

    void erase(const T* first, const T* last);

    T& operator[](int index)
    {
        return mData[index];
    }

    T* mData;
    int mSize;
    int mCapacity;
};

template <typename T, typename Allocator>
inline Vector<T, Allocator>::~Vector()
{
    Allocator::template Delete<T>(mData);
}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::erase(const T* begin, const T* end)
{
    const T* eraseEnd = end;
    int size = end - begin;
    int offset = begin - mData;
    T* at = mData + offset;
    while (eraseEnd != mData + mSize)
    {
        *at++ = *eraseEnd++;
    }
    mSize -= size;
}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::Swap(Vector& other)
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

#endif // _NLVECTOR_H_
