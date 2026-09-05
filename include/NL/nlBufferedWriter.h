#ifndef NL_NL_BUFFERED_WRITER_H
#define NL_NL_BUFFERED_WRITER_H

#include "types.h"

struct nlBufferedWriter
{
    /* 0x00 */ void* mFile;
    /* 0x04 */ bool mBuffered;
    /* 0x05 */ bool mWriteToNAND;
    /* 0x06 */ u8 mPadding[2];
    /* 0x08 */ unsigned int mBufferSize;
    /* 0x0C */ unsigned int mFlushThreshold;
    /* 0x10 */ char* mBuffer;
    /* 0x14 */ char* mCurrent;
}; // size: 0x18

extern "C"
{
    void nlBufferedWriterInitialize(nlBufferedWriter* writer);
    void nlBufferedWriterAttach(nlBufferedWriter* writer, void* file,
        bool buffered, unsigned int bufferSize, unsigned int flushThreshold);
    // Flushes and frees the buffer; the caller owns the file handle.
    void nlBufferedWriterFinish(nlBufferedWriter* writer);
    void nlBufferedWriterWriteText(nlBufferedWriter* writer, const char* text);
    void nlBufferedWriterWrite(nlBufferedWriter* writer, const void* data, int size);
    void nlBufferedWriterFlushIfNeeded(nlBufferedWriter* writer);
    void nlBufferedWriterFlush(nlBufferedWriter* writer);
}

#endif // NL_NL_BUFFERED_WRITER_H
