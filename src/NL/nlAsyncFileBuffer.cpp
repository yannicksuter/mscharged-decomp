#include "NL/nlAsyncFileBuffer.h"
#include "NL/nlFile.h"

#include <string.h>

void nlAsyncFileBufferReadComplete(nlFile*, void*, unsigned int size, unsigned long userParam)
{
    nlAsyncFileBuffer* fileBuffer = (nlAsyncFileBuffer*)userParam;

    fileBuffer->readPosition += size;
    fileBuffer->bufferStates[fileBuffer->loadBuffer] = AsyncBufferReady;
    fileBuffer->loadBuffer = 1 - fileBuffer->loadBuffer;
}

nlAsyncFileBuffer* nlAsyncFileBufferInitialize(nlAsyncFileBuffer* fileBuffer)
{
    nlAsyncFileBufferReset(fileBuffer, true);
    return fileBuffer;
}

void nlAsyncFileBufferFinish(nlAsyncFileBuffer* fileBuffer)
{
    nlAsyncFileBufferReset(fileBuffer, false);
}

void nlAsyncFileBufferReset(nlAsyncFileBuffer* fileBuffer, bool constructing)
{
    if (constructing)
    {
        fileBuffer->allocation = 0;
    }
    else if (fileBuffer->allocation != 0)
    {
        delete[] fileBuffer->allocation;
        fileBuffer->allocation = 0;
    }

    fileBuffer->file = 0;
    fileBuffer->fileSize = 0;
    fileBuffer->readPosition = 0;
    fileBuffer->position = 0;
    fileBuffer->bufferSize = 0;
    fileBuffer->loadBuffer = 0;
    fileBuffer->currentBuffer = 0;

    fileBuffer->buffers[0] = 0;
    fileBuffer->bufferPositions[0] = 0;
    fileBuffer->bufferDataSizes[0] = 0;
    fileBuffer->bufferStates[0] = AsyncBufferAvailable;

    fileBuffer->buffers[1] = 0;
    fileBuffer->bufferPositions[1] = 0;
    fileBuffer->bufferDataSizes[1] = 0;
    fileBuffer->bufferStates[1] = AsyncBufferAvailable;
}

int nlAsyncFileBufferGetRemaining(nlAsyncFileBuffer* fileBuffer)
{
    return fileBuffer->fileSize - fileBuffer->position;
}

void nlAsyncFileBufferRead(
    nlAsyncFileBuffer* fileBuffer, void* output, unsigned int size)
{
    int loadBuffer = fileBuffer->loadBuffer;
    if (fileBuffer->bufferStates[loadBuffer] == AsyncBufferAvailable
        && fileBuffer->readPosition != fileBuffer->fileSize)
    {
        int readSize = fileBuffer->bufferSize;
        int remaining = fileBuffer->fileSize - fileBuffer->readPosition;
        if (remaining < readSize)
        {
            readSize = remaining;
        }

        nlReadAsync(fileBuffer->file, fileBuffer->buffers[loadBuffer], readSize,
            nlAsyncFileBufferReadComplete, (unsigned long)fileBuffer, 0);
        fileBuffer->bufferDataSizes[loadBuffer] = readSize;
        fileBuffer->bufferStates[loadBuffer] = AsyncBufferPending;
    }

    int currentBuffer = fileBuffer->currentBuffer;
    int available = fileBuffer->buffers[currentBuffer]
        + fileBuffer->bufferDataSizes[currentBuffer]
        - fileBuffer->bufferPositions[currentBuffer];
    if (size <= available)
    {
        memcpy(output, fileBuffer->bufferPositions[currentBuffer], size);
        fileBuffer->bufferPositions[currentBuffer] += size;
        fileBuffer->position += size;
    }
    else
    {
        if (available > 0)
        {
            memcpy(output, fileBuffer->bufferPositions[currentBuffer], available);
        }

        if (fileBuffer->readPosition < fileBuffer->fileSize)
        {
            fileBuffer->bufferStates[currentBuffer] = AsyncBufferAvailable;
        }

        unsigned int remaining = size - available;
        fileBuffer->currentBuffer = 1 - fileBuffer->currentBuffer;
        currentBuffer = fileBuffer->currentBuffer;
        fileBuffer->bufferPositions[currentBuffer] = fileBuffer->buffers[currentBuffer];

        memcpy((unsigned char*)output + available,
            fileBuffer->bufferPositions[currentBuffer], remaining);
        fileBuffer->bufferPositions[currentBuffer] += remaining;
        fileBuffer->position += size;
    }
}
