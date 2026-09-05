#ifndef NL_UNIDENTIFIEDINFLATESTREAM_802A99E8_H
#define NL_UNIDENTIFIEDINFLATESTREAM_802A99E8_H

#include "zlib.h"

class UnidentifiedInflateStream_802A99E8
{
public:
    UnidentifiedInflateStream_802A99E8(unsigned int sourceSize, void* output, unsigned int outputSize);

    bool fn_802A9A04();
    bool fn_802A9A58(void* input, unsigned int size);
    int fn_802A9B84();

    /* 0x00 */ unsigned int sourceSize;
    /* 0x04 */ void* output;
    /* 0x08 */ unsigned int outputSize;
    /* 0x0C */ unsigned int sourceRemaining;
    /* 0x10 */ bool complete;
    /* 0x14 */ z_stream state;
}; // size: 0x4C

extern "C" void fn_802A99D8(alloc_func zalloc, free_func zfree, void* opaque);

#endif // NL_UNIDENTIFIEDINFLATESTREAM_802A99E8_H
