#include "Game/NetworkDiagnostics.h"

#include <revolution/os/OSContext.h>
#include <revolution/os/OSTime.h>

#include "NL/nlPrint.h"

void FormatNetworkTimestamp(char* text, unsigned long size, bool arg2)
{
    OSCalendarTime calendar;
    OSTicksToCalendarTime(OSGetTime(), &calendar);
    if (arg2)
    {
        nlSNPrintf(text, size, "%d_%d_%d.%d", calendar.hour, calendar.min, calendar.sec, calendar.msec);
    }
    else
    {
        nlSNPrintf(text, size, "%d_%d__%d_%d_%d", calendar.month + 1, calendar.mday, calendar.hour, calendar.min, calendar.sec);
    }
}

int FormatNetworkCallStack(int maxDepth, char* buffer, int size)
{
    buffer[0] = '\0';
    u32* sp = (u32*)OSGetStackPointer();
    if (sp != 0 && sp != (u32*)0xFFFFFFFF)
    {
        sp = (u32*)*sp;
    }

    int depth = 0;
    while (sp != 0 && sp != (u32*)0xFFFFFFFF && depth++ < maxDepth)
    {
        int length = nlSNPrintf(buffer, size, "%08x ", sp[1]);
        if (length == -1)
        {
            break;
        }
        size -= length;
        buffer += length;
        if (size <= 1)
        {
            break;
        }
        sp = (u32*)*sp;
    }
    if (size >= 1)
    {
        nlSNPrintf(buffer, size, "\n");
    }
    return 1;
}
