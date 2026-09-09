#ifndef NL_INFLATESTREAM_H
#define NL_INFLATESTREAM_H

#include "zlib.h"

class InflateStream
{
public:
    InflateStream(unsigned int sourceSize, void* output, unsigned int outputSize);

    static void SetMemoryCallbacks(alloc_func zalloc, free_func zfree, void* opaque);

    bool Initialize();
    bool Process(void* input, unsigned int size);
    int Finish();

    /* 0x00 */ unsigned int sourceSize;
    /* 0x04 */ void* output;
    /* 0x08 */ unsigned int outputSize;
    /* 0x0C */ unsigned int sourceRemaining;
    /* 0x10 */ bool complete;
    /* 0x14 */ z_stream state;
}; // size: 0x4C

#endif // NL_INFLATESTREAM_H
