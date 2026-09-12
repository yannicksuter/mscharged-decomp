#include "NL/nlString.h"

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

#include <stdlib.h>

void nlStrToWcs(const char* str, unsigned short* wstr, unsigned long maxLen)
{
    unsigned short* dest = wstr;
    unsigned long remaining = maxLen;

    while (remaining-- != 0 && (*dest = (s16)*str) != 0)
    {
        ++dest;
        ++str;
    }
    wstr[maxLen - 1] = 0;
}

void nlWcsToStr(const unsigned short* wstr, char* str, unsigned long maxLen)
{
    unsigned short value;
    unsigned long remaining = maxLen;
    char* dest = str;
    while (remaining != 0)
    {
        value = *wstr;
        *str = value;
        if (value == 0)
        {
            break;
        }
        ++str;
        ++wstr;
        --remaining;
    }
    dest[maxLen - 1] = 0;
}

unsigned long nlWcsToul(const unsigned short* str, unsigned short**, int base)
{
    char buffer[100];
    nlWcsToStr(str, buffer, sizeof(buffer));
    return strtoul(buffer, 0, base);
}

namespace Detail
{
StringBlockAllocator sStringBlockAllocator;
TempStringAllocatorStorage sTempStringAllocatorPool;
} // namespace Detail
