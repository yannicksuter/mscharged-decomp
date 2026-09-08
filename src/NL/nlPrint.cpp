#include "NL/nlPrint.h"

#include <stdarg.h>
#include <stdio.h>

int nlSNPrintf(char* buffer, unsigned long size, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    buffer[size - 1] = '\0';
    return result;
}

int nlSNPrintf(unsigned short* buffer, unsigned long size, const unsigned short* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vswprintf(
        (wchar_t*)buffer, size, (const wchar_t*)format, args);
    va_end(args);
    buffer[size - 1] = '\0';
    return result;
}

int nlVSNPrintf(char* buffer, unsigned long size, const char* format, va_list args)
{
    int result = vsnprintf(buffer, size - 1, format, args);
    buffer[size - 1] = '\0';
    return result;
}
