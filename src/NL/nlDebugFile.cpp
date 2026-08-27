#include "NL/nlDebugFile.h"

#include "NL/nlString.h"

extern "C"
{
    typedef struct _FILE FILE;

    FILE* fopen(const char* filename, const char* mode);
    unsigned long fwrite(const void* buffer, unsigned long size, unsigned long count, FILE* stream);
    int fputs(const char* string, FILE* stream);
    int fflush(FILE* stream);
    int fclose(FILE* stream);
}

bool lbl_806E1D88;

void* nlOpenFileDebug(const char* fileName, bool bBinary, bool bAppend)
{
    char fullName[256];

    nlStrNCat(fullName, "", fileName, 250);
    for (char* character = fullName; *character != '\0'; character++)
    {
        if (*character == '/')
        {
            *character = '\\';
        }
    }

    if (bBinary != 0)
    {
        return (void*)fopen(fullName, "wb");
    }
    if (bAppend != 0)
    {
        return (void*)fopen(fullName, "at");
    }
    return (void*)fopen(fullName, "wt");
}

void nlWriteBuffer(void* file, const char* buffer, int size)
{
    if (!lbl_806E1D88)
    {
        fwrite(buffer, 1, size, (FILE*)file);
    }
}

void nlWriteLineDebug(void* file, const char* buffer, bool flush)
{
    if (!lbl_806E1D88)
    {
        fputs(buffer, (FILE*)file);
        if (flush != 0)
        {
            fflush((FILE*)file);
        }
    }
}

void nlFlushFileDebug(void* fp)
{
    fflush((FILE*)fp);
}

void nlCloseFileDebug(void* fp)
{
    fclose((FILE*)fp);
}

bool nlDebugFileIsValid(void* fp)
{
    return fp != 0;
}
