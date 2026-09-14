#ifndef NL_STRING_H
#define NL_STRING_H

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"
#include "types.h"

#include <cstring>


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

class TempStringAllocatorStorage
{
public:
    struct Layer
    {
        TempStringAllocatorPool pool;
    };

    Layer allocator;
};

extern TempStringAllocatorStorage sTempStringAllocatorPool;

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

class TempStringPoolAllocator
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
        void* ptr = sTempStringAllocatorPool.allocator.pool.Allocate(size + 4);
        memcpy(ptr, &size, 4);
        return (char*)ptr + 4;
    }

    static void Free(void* ptr)
    {
        unsigned long size;
        memcpy(&size, (char*)ptr - 4, 4);
        sTempStringAllocatorPool.allocator.pool.Free(
            (char*)ptr - 4, size);
    }
};

class StringAllocator_801CBA50
{
public:
    enum { kAtEnd = true };

    template <typename T>
    static T* New(int count, const char* name)
    {
        return new (8, kAtEnd, name) T[count];
    }

    template <typename T>
    static void Delete(T* ptr)
    {
        delete[] ptr;
    }

    static void* Alloc(int size)
    {
        return nlMalloc(size, 8, kAtEnd);
    }

    static void Free(void* ptr)
    {
        nlFree(ptr);
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
CharT* nlStrNCpy(CharT* str1, const CharT* str2, unsigned long len);

template <typename CharT>
CharT* nlStrNCat(CharT* dest, const CharT* a, const CharT* b, unsigned long maxsize);

template <typename CharT>
int nlStrCmp(const CharT* a, const CharT* b);

template <typename CharT>
int nlStrICmp(const CharT* lhs, const CharT* rhs);

template <typename CharT>
int nlStrNICmp(const CharT* lhs, const CharT* rhs, unsigned long count);

template <typename CharT>
int nlStrNCmp(const CharT* lhs, const CharT* rhs, unsigned long count);

template <typename CharT>
CharT nlToUpper(CharT value);

template <typename CharT>
CharT nlToLower(CharT value);

template <typename CharT>
CharT* nlToLower(CharT* str);

template <typename CharT>
CharT* nlStrChr(const CharT* str, CharT c);

#include "NL/nlPrint.h"

#endif // NL_STRING_H
