#include "NL/UnidentifiedInflateStream_802A99E8.h"

extern "C"
{
    alloc_func lbl_806E1D60;
    free_func lbl_806E1D64;
    void* lbl_806E1D68;
}

extern "C" void fn_802A99D8(alloc_func zalloc, free_func zfree, void* opaque)
{
    lbl_806E1D60 = zalloc;
    lbl_806E1D64 = zfree;
    lbl_806E1D68 = opaque;
}

UnidentifiedInflateStream_802A99E8::UnidentifiedInflateStream_802A99E8(
    unsigned int sourceSize, void* output, unsigned int outputSize)
    : sourceSize(sourceSize)
    , output(output)
    , outputSize(outputSize)
    , sourceRemaining(sourceSize)
    , complete(false)
{
}

bool UnidentifiedInflateStream_802A99E8::fn_802A9A04()
{
    state.zalloc = lbl_806E1D60;
    state.zfree = lbl_806E1D64;
    state.opaque = lbl_806E1D68;
    state.avail_in = 0;
    state.next_in = Z_NULL;
    return inflateInit(&state) == Z_OK;
}

bool UnidentifiedInflateStream_802A99E8::fn_802A9A58(void* input, unsigned int size)
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

int UnidentifiedInflateStream_802A99E8::fn_802A9B84()
{
    return inflateEnd(&state);
}
