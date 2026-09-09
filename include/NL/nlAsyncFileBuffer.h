#ifndef NL_ASYNC_FILE_BUFFER_H
#define NL_ASYNC_FILE_BUFFER_H

struct nlFile;

enum nlAsyncBufferState
{
    AsyncBufferAvailable = 0,
    AsyncBufferPending = 1,
    AsyncBufferReady = 2,
};

struct nlAsyncFileBuffer
{
    nlFile* file;
    unsigned char* allocation;
    int fileSize;
    int readPosition;
    int position;
    int bufferSize;
    int loadBuffer;
    int currentBuffer;
    unsigned char* buffers[2];
    unsigned char* bufferPositions[2];
    int bufferDataSizes[2];
    int bufferStates[2];
};

void nlAsyncFileBufferReadComplete(nlFile*, void*, unsigned int size, unsigned long userParam);
nlAsyncFileBuffer* nlAsyncFileBufferInitialize(nlAsyncFileBuffer* fileBuffer);
void nlAsyncFileBufferFinish(nlAsyncFileBuffer* fileBuffer);
void nlAsyncFileBufferReset(nlAsyncFileBuffer* fileBuffer, bool constructing);
int nlAsyncFileBufferGetRemaining(nlAsyncFileBuffer* fileBuffer);
void nlAsyncFileBufferRead(nlAsyncFileBuffer* fileBuffer, void* output, unsigned int size);

#endif // NL_ASYNC_FILE_BUFFER_H
