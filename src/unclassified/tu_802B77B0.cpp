#include "NL/nlDebugFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "unclassified/tu_80376888.h"

extern "C" void* memcpy(void* dest, const void* src, unsigned long count);
extern "C" void* memset(void* dest, int value, unsigned long count);

struct UnidentifiedBufferedWriter
{
    void* mFile;
    bool mBuffered;
    bool mWriteToNAND;
    unsigned char mPadding[2];
    unsigned int mBufferSize;
    unsigned int mFlushThreshold;
    char* mBuffer;
    char* mCurrent;
};

extern "C" void fn_802B7AE4(UnidentifiedBufferedWriter* writer);

extern "C" void fn_802B77B0(UnidentifiedBufferedWriter* writer)
{
    writer->mFile = 0;
    writer->mBuffered = false;
    writer->mWriteToNAND = false;
    writer->mBufferSize = 0;
    writer->mFlushThreshold = 0;
    writer->mBuffer = 0;
    writer->mCurrent = 0;
}

extern "C" void fn_802B77D4(UnidentifiedBufferedWriter* writer, void* file,
    bool buffered, unsigned int bufferSize, unsigned int flushThreshold)
{
    writer->mFile = file;
    writer->mBuffered = buffered;
    writer->mWriteToNAND = false;
    if (buffered)
    {
        writer->mBufferSize = bufferSize;
        writer->mFlushThreshold = flushThreshold;
        writer->mBuffer = (char*)nlMalloc(bufferSize, 8, false);
        writer->mCurrent = writer->mBuffer;
    }
    else
    {
        writer->mBufferSize = 0;
        writer->mFlushThreshold = 0;
        writer->mBuffer = 0;
        writer->mCurrent = 0;
    }
}

extern "C" void fn_802B7848(UnidentifiedBufferedWriter* writer)
{
    fn_802B7AE4(writer);

    if (writer->mWriteToNAND && writer->mCurrent > writer->mBuffer)
    {
        unsigned int remainder =
            (unsigned int)(writer->mCurrent - writer->mBuffer) & 0x1F;
        if (remainder != 0)
        {
            unsigned int padding = 0x20 - remainder;
            memset(writer->mCurrent, 0, padding);
            writer->mCurrent += padding;
        }
        fn_80376C78(writer->mBuffer,
            (unsigned int)(writer->mCurrent - writer->mBuffer), 0);
    }

    writer->mFile = 0;
    writer->mBuffered = false;
    if (writer->mBuffer != 0)
    {
        delete[] writer->mBuffer;
        writer->mBuffer = 0;
    }
    writer->mFlushThreshold = 0;
    writer->mBufferSize = 0;
    writer->mCurrent = 0;
}

extern "C" void fn_802B7904(
    UnidentifiedBufferedWriter* writer, const char* text)
{
    if (!writer->mBuffered)
    {
        nlWriteLineDebug(writer->mFile, text, false);
    }
    else
    {
        int length = nlStrLen(text);
        if (length > 0)
        {
            if (writer->mCurrent + length
                >= writer->mBuffer + writer->mBufferSize)
            {
                fn_802B7AE4(writer);
            }
            memcpy(writer->mCurrent, text, length);
            writer->mCurrent += length;
        }
    }
}

extern "C" void fn_802B79C8(UnidentifiedBufferedWriter* writer,
    const char* data, int size)
{
    if (!writer->mBuffered)
    {
        nlWriteBuffer(writer->mFile, data, size);
    }
    else if (size > 0)
    {
        if (writer->mCurrent + size
            >= writer->mBuffer + writer->mBufferSize)
        {
            fn_802B7AE4(writer);
        }
        memcpy(writer->mCurrent, data, size);
        writer->mCurrent += size;
    }
}

extern "C" void fn_802B7A64(UnidentifiedBufferedWriter* writer)
{
    if (writer->mWriteToNAND)
    {
        return;
    }

    if (!writer->mBuffered)
    {
        nlFlushFileDebug(writer->mFile);
    }
    else if (writer->mCurrent
        > writer->mBuffer + writer->mFlushThreshold)
    {
        nlWriteBuffer(writer->mFile, writer->mBuffer,
            writer->mCurrent - writer->mBuffer);
        nlFlushFileDebug(writer->mFile);
        writer->mCurrent = writer->mBuffer;
    }
}

extern "C" void fn_802B7AE4(UnidentifiedBufferedWriter* writer)
{
    if (writer->mWriteToNAND)
    {
        if (writer->mCurrent > writer->mBuffer)
        {
            unsigned int size = writer->mCurrent - writer->mBuffer;
            unsigned int remainder = size & 0x1F;
            if (remainder == 0)
            {
                fn_80376C78(writer->mBuffer, size, 0);
                writer->mCurrent = writer->mBuffer;
            }
            else
            {
                int alignedSize = size - remainder;
                if (alignedSize >= 0x20)
                {
                    fn_80376C78(writer->mBuffer, alignedSize, 0);
                    memcpy(writer->mBuffer,
                        writer->mBuffer + alignedSize, remainder);
                    writer->mCurrent = writer->mBuffer + remainder;
                }
            }
        }
    }
    else if (!writer->mBuffered)
    {
        nlFlushFileDebug(writer->mFile);
    }
    else
    {
        char* buffer = writer->mBuffer;
        char* current = writer->mCurrent;
        if (current > buffer)
        {
            nlWriteBuffer(writer->mFile, buffer, current - buffer);
            nlFlushFileDebug(writer->mFile);
            writer->mCurrent = writer->mBuffer;
        }
    }
}
