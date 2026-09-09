#include "NL/InflateStream.h"

alloc_func gInflateAlloc;
free_func gInflateFree;
void* gInflateOpaque;

void InflateStream::SetMemoryCallbacks(alloc_func zalloc, free_func zfree, void* opaque)
{
    gInflateAlloc = zalloc;
    gInflateFree = zfree;
    gInflateOpaque = opaque;
}

InflateStream::InflateStream(
    unsigned int sourceSize, void* output, unsigned int outputSize)
    : sourceSize(sourceSize)
    , output(output)
    , outputSize(outputSize)
    , sourceRemaining(sourceSize)
    , complete(false)
{
}

bool InflateStream::Initialize()
{
    state.zalloc = gInflateAlloc;
    state.zfree = gInflateFree;
    state.opaque = gInflateOpaque;
    state.avail_in = 0;
    state.next_in = Z_NULL;
    return inflateInit(&state) == Z_OK;
}

bool InflateStream::Process(void* input, unsigned int size)
{
    if (sourceRemaining <= size)
        size = sourceRemaining;
    state.avail_in = size;
    if (size == 0)
    {
        complete = true;
        return true;
    }

    state.next_in = (Bytef*)input;
    sourceRemaining -= size;
    int ret;
    do
    {
        state.avail_out = 0x40000;
        state.next_out = (Bytef*)output;
        ret = inflate(&state, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR)
            return false;
        switch (ret)
        {
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&state);
            return false;
        }

        output = (Bytef*)output + (0x40000 - state.avail_out);
        if (outputSize != 0 && state.total_out > outputSize)
            return false;
    } while (state.avail_out == 0);

    if (state.avail_in != 0)
        return false;
    if (ret == Z_STREAM_END)
        complete = true;
    return true;
}

int InflateStream::Finish()
{
    return inflateEnd(&state);
}
