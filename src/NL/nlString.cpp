#include "NL/nlString.h"

#include <mem.h>

extern "C" void DCZeroRange(void* address, u32 length);

u32 nlStringHash(const char* str)
{
    u32 hash = -1;
    while (*str)
    {
        hash += hash << 5;
        hash += (unsigned char)*str++;
    }
    return hash;
}

u32 nlStringLowerHash(const char* str)
{
    u32 hash = -1;
    while (*str)
    {
        hash += hash << 5;
        hash += nlToLower<unsigned char>(*(unsigned char*)str++);
    }
    return hash;
}

void nlZeroMemory(void* ptr, unsigned long numBytes)
{
    if (numBytes >= 0x80)
    {
        unsigned long address = (unsigned long)ptr;
        unsigned long end = address + numBytes;
        unsigned long alignedStart = (address + 0x1F) & ~0x1F;
        unsigned long alignedEnd = end & ~0x1F;

        while (address < alignedStart)
        {
            *(unsigned char*)address = 0;
            address++;
        }

        if (alignedEnd > alignedStart)
        {
            DCZeroRange((void*)alignedStart, alignedEnd - alignedStart);
        }

        while (alignedEnd < end)
        {
            *(unsigned char*)alignedEnd = 0;
            alignedEnd++;
        }
    }
    else
    {
        memset(ptr, 0, numBytes);
    }
}
