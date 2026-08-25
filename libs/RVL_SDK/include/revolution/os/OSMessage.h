#ifndef REVOLUTION_OS_MESSAGE_H
#define REVOLUTION_OS_MESSAGE_H

#include <revolution/os/OSThread.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* OSMessage;

typedef struct OSMessageQueue {
    OSThreadQueue queueSend;
    OSThreadQueue queueReceive;
    OSMessage* msgArray;
    s32 msgCount;
    s32 firstIndex;
    s32 usedCount;
} OSMessageQueue;

#define OS_MESSAGE_NO_FLAGS 0
#define OS_MESSAGE_BLOCK (1 << 0)

void OSInitMessageQueue(OSMessageQueue* queue, OSMessage* msgArray, s32 msgCount);
BOOL OSSendMessage(OSMessageQueue* queue, OSMessage msg, s32 flags);
BOOL OSReceiveMessage(OSMessageQueue* queue, OSMessage* msg, s32 flags);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_MESSAGE_H
