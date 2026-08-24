#ifndef NL_STRING_H
#define NL_STRING_H

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "types.h"

namespace Detail
{
struct StringBlock
{
    StringBlock* next;
    u8 storage[0x3FC];
};

class StringBlockAllocator
{
public:
    StringBlockAllocator()
    {
        mFreeList = 0;
        mBuffer = 0;
        mAllocator = 0;

        StringBlockAllocator* allocator = (StringBlockAllocator*)this;
        allocator->mFreeList = (StringBlock*)nlMalloc(0x10000, 8, false);
        allocator->mBuffer = allocator->mFreeList;

        for (int i = 0; i < 63; ++i)
        {
            StringBlock* block = allocator->mBuffer + i;
            block->next = block + 1;
        }
        allocator->mBuffer[63].next = 0;
    }

    ~StringBlockAllocator()
    {
        if (mAllocator != 0)
        {
            nlFree(mBuffer);
        }
        else
        {
            mAllocator->Free(mBuffer);
        }
    }

    void* Allocate(unsigned long)
    {
        if (mFreeList == 0)
        {
            return 0;
        }

        StringBlock* block = mFreeList;
        mFreeList = block->next;
        return block;
    }

    void Free(void* ptr)
    {
        StringBlock* block = (StringBlock*)ptr;
        block->next = mFreeList;
        mFreeList = block;
    }

private:
    StringBlock* mFreeList;
    StringBlock* mBuffer;
    MemoryAllocator* mAllocator;
};

extern StringBlockAllocator sStringBlockAllocator;

class TempStringAllocator
{
public:
    enum
    {
        kAtEnd = false
    };

    template <typename T>
    static T* New(int count, const char*)
    {
        return (T*)Alloc(count * sizeof(T));
    }

    template <typename T>
    static void Delete(T* ptr)
    {
        Free(ptr);
    }

    static void* Alloc(int size)
    {
        return sStringBlockAllocator.Allocate(size);
    }

    static void Free(void* ptr)
    {
        sStringBlockAllocator.Free(ptr);
    }
};
} // namespace Detail

void nlZeroMemory(void* ptr, unsigned long numBytes);
void nlStrToWcs(const char* str, unsigned short* wstr, unsigned long maxLen);
void nlWcsToStr(const unsigned short* wstr, char* str, unsigned long maxLen);
unsigned long nlWcsToul(const unsigned short* str, unsigned short** endPtr, int base);
u32 nlStringLowerHash(const char* str);
u32 nlStringHash(const char* str);

template <typename CharT>
unsigned long nlStrLen(const CharT* str);

template <typename CharT>
int nlStrICmp(const CharT* lhs, const CharT* rhs);

template <typename CharT>
int nlStrNCmp(const CharT* lhs, const CharT* rhs, unsigned long count);

template <typename CharT>
CharT nlToUpper(CharT value);

template <typename CharT>
CharT nlToLower(CharT value);

#include "NL/nlstring_tmpl.h"

#endif // NL_STRING_H
