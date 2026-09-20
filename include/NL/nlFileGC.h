#ifndef NL_FILE_GC_H
#define NL_FILE_GC_H

#include "NL/nlFile.h"

struct AsyncEntry;

void nlInitFileSystem(void);
void nlCancelPendingAsyncReads(nlFile* pFile, void (*callback)(nlFile*, void*, unsigned int, unsigned long, void (*)(nlFile*, void*, unsigned int, unsigned long)));
bool nlAsyncReadsPending(nlFile* file);
void nlSeek(nlFile* file, unsigned int offset, unsigned long origin);
void* nlLoadEntireHostFile(const char* fileName, unsigned long* outSize,
    unsigned int alignment, eAllocType type, void* target,
    unsigned long bufferSize);
void nlServiceFileSystem(void);
AsyncEntry* nlGetCurrentAsyncRead();

bool nlAsyncReadBusy(AsyncEntry* entry);
bool nlCancelAsyncRead(AsyncEntry* entry,
    void (*callback)(nlFile*, void*, unsigned int, unsigned long, void (*)(nlFile*, void*, unsigned int, unsigned long)));

#endif // NL_FILE_GC_H
