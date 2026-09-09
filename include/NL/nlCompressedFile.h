#ifndef NL_COMPRESSED_FILE_H
#define NL_COMPRESSED_FILE_H

#include "NL/nlFile.h"

bool nlLoadCompressedFileAsync(const char* path, LoadAsyncCallback callback,
    void* userData, unsigned int alignment, eAllocType allocType,
    unsigned int chunkSize, void* readBuffer0, void* readBuffer1,
    void*, unsigned long param, MemoryAllocator* allocator);

#endif // NL_COMPRESSED_FILE_H
