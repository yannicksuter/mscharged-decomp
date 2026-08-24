#ifndef _NLBASICSTRING_H_
#define _NLBASICSTRING_H_

#include "NL/nlString.h"
#include "NL/nlVector.h"

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

template <typename CharT, typename Allocator>
class BasicString
{
public:
    typedef CharT value_type;

    class Data
    {
    public:
        Data(const CharT* string)
            : mData(string)
            , mRefCount(1)
        {
        }

        Data(const CharT* begin, const CharT* end)
            : mData((int)(end - begin) + 1, 0)
            , mRefCount(1)
        {
            for (int i = 0; i < mData.mSize - 1; i++)
            {
                mData[i] = begin[i];
            }
        }

        Data(const Data& other)
            : mData(other.mData, 0)
            , mRefCount(1)
        {
        }

        void reserve(int capacity);
        void insertRange(CharT* at, const CharT* begin, const CharT* end);
        void erase(const CharT* begin, const CharT* end);

        Data* AddRef()
        {
            ++mRefCount;
            return this;
        }

        void* operator new(unsigned long)
        {
            return Allocator::Alloc();
        }

        void DecRef() const
        {
            if (!--mRefCount)
            {
                delete this;
            }
        }

        CharT& operator[](int index)
        {
            return mData[index];
        }

        const CharT& operator[](int index) const
        {
            return mData[index];
        }

        CharT* begin()
        {
            return mData.mData;
        }

        CharT* end()
        {
            return mData.mData + mData.mSize - 1;
        }

        Data* Cow()
        {
            if (mRefCount == 1)
            {
                return this;
            }
            Data* data = new Data(*this);
            DecRef();
            return data;
        }

        void operator delete(void* ptr)
        {
            if (ptr)
            {
                Allocator::Free(ptr);
            }
        }

        Vector<CharT, Allocator> mData;
        mutable int mRefCount;
    };

    Data* mData;

    BasicString()
        : mData(0)
    {
    }

    BasicString(const CharT* string)
    {
        Data* data = ::new (Allocator::Alloc()) Data(string);
        mData = data;
    }

    BasicString(const CharT* begin, const CharT* end);

    BasicString(Data* data)
        : mData(data)
    {
    }

    BasicString(const BasicString& other)
        : mData(other.mData != 0 ? other.mData->AddRef() : 0)
    {
    }

    ~BasicString()
    {
        if (mData)
        {
            Data* data = mData;
            data->DecRef();
        }
    }

    BasicString& operator=(BasicString other)
    {
        Data* data = mData;
        mData = other.mData;
        other.mData = data;
        return *this;
    }

    BasicString& AppendInPlace(const CharT* string);

    template <typename OtherAllocator>
    inline BasicString& AppendInPlace(
        const BasicString<CharT, OtherAllocator>& other);

    const CharT* c_str() const
    {
        static CharT emptyString = '\0';
        return mData ? mData->mData.mData : &emptyString;
    }

    int size() const
    {
        return mData ? mData->mData.mSize - 1 : 0;
    }
    CharT* begin()
    {
        Cow();
        if (mData)
        {
            return GetData().begin();
        }
        return (CharT*)0;
    }

    CharT* end()
    {
        Cow();
        if (mData)
        {
            return GetData().end();
        }
        return (CharT*)0;
    }

    const CharT* begin() const
    {
        return mData ? mData->mData.mData : (const CharT*)0;
    }

    const CharT* end() const
    {
        return mData ? mData->mData.mData + mData->mData.mSize - 1
                     : (const CharT*)0;
    }

    const CharT& operator[](int index) const
    {
        return mData->mData.mData[index];
    }

    Data& GetData()
    {
        return *mData;
    }

    const Data& GetData() const
    {
        return *mData;
    }

    void Cow()
    {
        if (!mData)
        {
            mData = new Data((const CharT*)0, (const CharT*)0);
        }
        else
        {
            mData = mData->Cow();
        }
    }

    CharT& operator[](int index)
    {
        Cow();
        return GetData()[index];
    }

    template <typename OtherAllocator>
    void insert(CharT* at, const BasicString<CharT, OtherAllocator>& string);

    void insert(CharT* at, const CharT* begin, const CharT* end);
    void erase(const CharT* begin, const CharT* end);

    inline void TrimInPlace(const CharT* chars);
    inline BasicString Trim(const CharT* chars) const;

    BasicString Append(const CharT* string) const;

    template <typename OtherAllocator>
    inline BasicString Append(
        const BasicString<CharT, OtherAllocator>& string) const;
};

template <typename CharT, typename Allocator>
inline void BasicString<CharT, Allocator>::Data::erase(
    const CharT* begin, const CharT* end)
{
    mData.erase(begin, end);
}

template <typename CharT, typename Allocator>
inline BasicString<CharT, Allocator>::BasicString(
    const CharT* begin, const CharT* end)
    : mData(new Data(begin, end))
{
}

template <typename CharT, typename Allocator>
inline void BasicString<CharT, Allocator>::Data::reserve(int capacity)
{
    if (mData.mCapacity < capacity)
    {
        struct RawVector
        {
            CharT* mData;
            int mSize;
            int mCapacity;
        } newVec;

        newVec.mData = Allocator::template New<CharT>(capacity, 0);
        newVec.mSize = capacity;
        newVec.mCapacity = capacity;

        int i = 0;
        for (; i < capacity; i++)
        {
            newVec.mData[i] = CharT();
        }
        for (i = 0; i < mData.mSize; i++)
        {
            newVec.mData[i] = mData.mData[i];
        }
        newVec.mSize = mData.mSize;

        int value = mData.mSize;
        mData.mSize = newVec.mSize;
        newVec.mSize = value;

        value = mData.mCapacity;
        mData.mCapacity = newVec.mCapacity;
        newVec.mCapacity = value;

        CharT* data = mData.mData;
        mData.mData = newVec.mData;
        newVec.mData = data;

        Allocator::template Delete<CharT>(newVec.mData);
    }
}

template <typename CharT, typename Allocator>
inline void BasicString<CharT, Allocator>::Data::insertRange(
    CharT* at, const CharT* begin, const CharT* end)
{
    int size = end - begin;
    int offset = at - mData.mData;
    reserve(mData.mSize + size);

    at = mData.mData + offset;
    CharT* item = mData.mData + mData.mSize - 1;
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
    mData.mSize += size;
}

template <typename CharT, typename Allocator>
inline void BasicString<CharT, Allocator>::erase(
    const CharT* begin, const CharT* end)
{
    Cow();
    GetData().erase(begin, end);
}

template <typename CharT, typename Allocator>
inline void BasicString<CharT, Allocator>::insert(
    CharT* at, const CharT* begin, const CharT* end)
{
    CharT* oldData = this->begin();
    int offset = at - oldData;
    Cow();
    CharT* data = this->begin();
    mData->insertRange(data + offset, begin, end);
}

typedef BasicString<char, Detail::TempStringAllocator> NLString;

#endif // _NLBASICSTRING_H_
