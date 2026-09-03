#include "NL/nlFile.h"

#include <string.h>

enum AsyncBufferState_802B41B4
{
    BufferAvailable_802B41B4 = 0,
    BufferPending_802B41B4 = 1,
    BufferReady_802B41B4 = 2,
};

struct AsyncFileBuffer_802B41B4
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

extern "C"
{
    void fn_802B41B4(nlFile*, void*, unsigned int, unsigned long);
    void fn_802B4220(AsyncFileBuffer_802B41B4*, bool);
}

extern "C" void fn_802B41B4(nlFile*, void*, unsigned int size, unsigned long userParam)
{
    AsyncFileBuffer_802B41B4* fileBuffer = (AsyncFileBuffer_802B41B4*)userParam;

    fileBuffer->readPosition += size;
    fileBuffer->bufferStates[fileBuffer->loadBuffer] = BufferReady_802B41B4;
    fileBuffer->loadBuffer = 1 - fileBuffer->loadBuffer;
}

extern "C" AsyncFileBuffer_802B41B4* fn_802B41E4(AsyncFileBuffer_802B41B4* fileBuffer)
{
    fn_802B4220(fileBuffer, true);
    return fileBuffer;
}

extern "C" void fn_802B4218(AsyncFileBuffer_802B41B4* fileBuffer)
{
    fn_802B4220(fileBuffer, false);
}

extern "C" void fn_802B4220(AsyncFileBuffer_802B41B4* fileBuffer, bool constructing)
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
    fileBuffer->bufferStates[0] = BufferAvailable_802B41B4;

    fileBuffer->buffers[1] = 0;
    fileBuffer->bufferPositions[1] = 0;
    fileBuffer->bufferDataSizes[1] = 0;
    fileBuffer->bufferStates[1] = BufferAvailable_802B41B4;
}

extern "C" unsigned int fn_802B42B4(AsyncFileBuffer_802B41B4* fileBuffer)
{
    return fileBuffer->fileSize - fileBuffer->position;
}

extern "C" void fn_802B42C4(
    AsyncFileBuffer_802B41B4* fileBuffer, void* output, unsigned int size)
{
    int loadBuffer = fileBuffer->loadBuffer;
    if (fileBuffer->bufferStates[loadBuffer] == BufferAvailable_802B41B4
        && fileBuffer->readPosition != fileBuffer->fileSize)
    {
        int readSize = fileBuffer->bufferSize;
        int remaining = fileBuffer->fileSize - fileBuffer->readPosition;
        if (remaining < readSize)
        {
            readSize = remaining;
        }

        nlReadAsync(fileBuffer->file, fileBuffer->buffers[loadBuffer], readSize,
            fn_802B41B4, (unsigned long)fileBuffer, 0);
        fileBuffer->bufferDataSizes[loadBuffer] = readSize;
        fileBuffer->bufferStates[loadBuffer] = BufferPending_802B41B4;
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
            fileBuffer->bufferStates[currentBuffer] = BufferAvailable_802B41B4;
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
