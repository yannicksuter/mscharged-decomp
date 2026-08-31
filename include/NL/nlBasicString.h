#ifndef NL_BASIC_STRING_H
#define NL_BASIC_STRING_H

#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlVector.h"

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
            for (int i = 0; i < mData.mSize - 1; ++i)
            {
                mData[i] = begin[i];
            }
        }

        Data(const Data& other)
            : mData(other.mData, 0)
            , mRefCount(1)
        {
        }

        void reserve(int capacity)
        {
            if (mData.mCapacity < capacity)
            {
                Vector<CharT, Allocator> other(capacity, 0);
                for (int i = 0; i < mData.mSize; ++i)
                {
                    other.mData[i] = mData.mData[i];
                }
                other.mSize = mData.mSize;
                mData.Swap(other);
            }
        }

        void insertRange(CharT* at, const CharT* begin, const CharT* end)
        {
            int count = end - begin;
            int offset = at - mData.mData;
            reserve(mData.mSize + count);

            at = mData.mData + offset;
            CharT* current = mData.mData + mData.mSize - 1;
            while (current >= at)
            {
                *(current + count) = *current;
                --current;
            }
            while (begin != end)
            {
                *at++ = *begin++;
            }
            mData.mSize += count;
        }

        void erase(const CharT* begin, const CharT* end)
        {
            mData.erase(begin, end);
        }

        Data* AddRef()
        {
            ++mRefCount;
            return this;
        }

        void* operator new(unsigned long size)
        {
            return Allocator::Alloc(size);
        }

        void operator delete(void* ptr)
        {
            if (ptr != 0)
            {
                Allocator::Free(ptr);
            }
        }

        void DecRef() const
        {
            if (--mRefCount == 0)
            {
                delete this;
            }
        }

        inline CharT& operator[](int index)
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

        inline Data* Cow()
        {
            if (mRefCount == 1)
            {
                return this;
            }
            Data* data = new Data(*this);
            DecRef();
            return data;
        }

        Vector<CharT, Allocator> mData;
        mutable int mRefCount;
    };

    BasicString()
        : mData(0)
    {
    }

    BasicString(const CharT* string)
    {
        void* storage = Allocator::Alloc(sizeof(Data));
        Data* data = ::new (storage) Data(string);
        mData = data;
    }

    BasicString(const CharT* begin, const CharT* end)
        : mData(new Data(begin, end))
    {
    }

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
        if (mData != 0)
        {
            Data* data = mData;
            data->DecRef();
        }
    }

    BasicString& operator=(BasicString other);

    const CharT* c_str() const
    {
        static CharT emptyString = 0;
        if (mData != 0)
        {
            return mData->mData.mData;
        }
        return &emptyString;
    }

    int size() const;
    CharT* begin();
    CharT* end();

    const CharT& operator[](int index) const
    {
        return mData->mData.mData[index];
    }

    CharT& operator[](int index);

    inline void Cow()
    {
        if (mData == 0)
        {
            mData = new Data((const CharT*)0, (const CharT*)0);
        }
        else
        {
            mData = mData->Cow();
        }
    }

    void erase(const CharT* begin, const CharT* end);
    void insert(CharT* at, const CharT* begin, const CharT* end);

    template <typename OtherAllocator>
    void insert(CharT* at, const BasicString<CharT, OtherAllocator>& rhs);

    BasicString& AppendInPlace(const CharT* str);

    template <typename OtherAllocator>
    inline BasicString& AppendInPlace(const BasicString<CharT, OtherAllocator>& rhs);

    BasicString Append(const CharT* rhs) const;

    template <typename OtherAllocator>
    BasicString Append(const BasicString<CharT, OtherAllocator>& rhs) const;

    void TrimInPlace(const CharT* chars);

    Data* mData;
};

template <typename CharT, typename Allocator>
void BasicString<CharT, Allocator>::TrimInPlace(const CharT* chars)
{
    int i = 0;
    while (i < size())
    {
        const CharT* current;
        for (current = chars; *current != 0; ++current)
        {
            if (*current == (*this)[i])
            {
                break;
            }
        }
        if (*current == 0)
        {
            break;
        }
        ++i;
    }
    erase(begin(), begin() + i);

    i = size() - 1;
    while (i >= 0)
    {
        const CharT* current;
        for (current = chars; *current != 0; ++current)
        {
            if (*current == (*this)[i])
            {
                break;
            }
        }
        if (*current == 0)
        {
            break;
        }
        --i;
    }
    erase(begin() + i + 1, end());
}

template <typename CharT, typename Allocator>
template <typename OtherAllocator>
BasicString<CharT, Allocator>& BasicString<CharT, Allocator>::AppendInPlace(const BasicString<CharT, OtherAllocator>& rhs)
{
    (*this)[0];

    CharT* at;
    Data* currentData = mData;
    if (currentData != 0)
    {
        at = currentData->mData.mData + currentData->mData.mSize - 1;
    }
    else
    {
        at = 0;
    }

    typename BasicString<CharT, OtherAllocator>::Data* rhsData = rhs.mData;
    const CharT* begin;
    if (rhsData != 0)
    {
        begin = rhsData->mData.mData;
    }
    else
    {
        begin = 0;
    }

    insert(at, begin, rhsData != 0 ? rhsData->mData.mData + rhsData->mData.mSize - 1 : 0);
    return *this;
}

template <typename CharT, typename Allocator>
inline BasicString<CharT, Allocator>& BasicString<CharT, Allocator>::AppendInPlace(const CharT* str)
{
    const CharT* rhsEnd = str;
    while (*rhsEnd != 0)
    {
        rhsEnd++;
    }

    (*this)[0];

    CharT* at;
    if (mData != 0)
    {
        at = mData->mData.mData + mData->mData.mSize - 1;
    }
    else
    {
        at = 0;
    }

    insert(at, str, rhsEnd);
    return *this;
}

template <typename CharT, typename Allocator>
template <typename OtherAllocator>
BasicString<CharT, Allocator> BasicString<CharT, Allocator>::Append(const BasicString<CharT, OtherAllocator>& rhs) const
{
    BasicString r(*this);
    r.AppendInPlace(rhs);
    Data* data = r.mData;
    if (data != 0)
    {
        data->mRefCount++;
    }
    else
    {
        data = 0;
    }
    return BasicString(data);
}

template <typename CharT, typename Allocator>
BasicString<CharT, Allocator> BasicString<CharT, Allocator>::Append(const CharT* rhs) const
{
    BasicString r(*this);
    r.AppendInPlace(rhs);
    Data* data = r.mData;
    if (data != 0)
    {
        data->mRefCount++;
    }
    else
    {
        data = 0;
    }
    return BasicString(data);
}

template <typename CharT, typename Allocator>
inline bool operator==(const BasicString<CharT, Allocator>& lhs, const char* rhs)
{
    unsigned int c;
    typename BasicString<CharT, Allocator>::Data* data = lhs.mData;
    int i = 0;
    while (i < (data != 0 ? data->mData.mSize - 1 : 0))
    {
        c = (u8)*rhs;
        if ((CharT)c == 0)
        {
            return false;
        }
        if ((CharT)c != data->mData.mData[i])
        {
            return false;
        }
        ++rhs;
        ++i;
    }
    return *rhs == 0;
}

typedef BasicString<char, Detail::TempStringAllocator> NLString;

#endif // NL_BASIC_STRING_H
