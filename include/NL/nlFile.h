#ifndef NL_FILE_H
#define NL_FILE_H

#include "types.h"

class MemoryAllocator;
class nlFile;

typedef void (*ReadAsyncCallback)(nlFile*, void*, unsigned int, unsigned long);
typedef void (*LoadAsyncCallback)(void*, unsigned long, void*);

enum eAllocType
{
    AllocateStart = 0,
    AllocateEnd = 1,
};

class nlFile
{
public:
    nlFile();
    virtual ~nlFile();

    virtual u32 FileSize(unsigned int* size) = 0;
    virtual void Read(void* buffer, unsigned int size, unsigned long bufferSize) = 0;
};

nlFile* nlOpen(const char* filename);
bool nlReadAsync(nlFile* file, void* buffer, unsigned int size, ReadAsyncCallback callback, unsigned long userParam, unsigned long bufferSize);

unsigned int nlFileSize(nlFile* file, unsigned int* size);
void nlRead(nlFile* file, void* buffer, unsigned int size, unsigned long bufferSize);
void nlClose(nlFile* file);
bool nlFileExists(const char* filename);
void* nlLoadEntireFile(const char* filename, unsigned long* outSize, unsigned int alignment, eAllocType type, void* buffer, unsigned long bufferSize, MemoryAllocator* allocator);
bool nlLoadEntireFileAsync(const char* filename, LoadAsyncCallback callback, void* userData, unsigned int alignment, eAllocType type, void* buffer, unsigned long bufferSize, MemoryAllocator* allocator);

#endif // NL_FILE_H
