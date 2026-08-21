#ifndef REVOLUTION_OS_MUTEX_H
#define REVOLUTION_OS_MUTEX_H

#include <revolution/os/OSThread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OSMutex {
    OSThreadQueue queue;
    OSThread* thread;
    s32 count;
    OSMutexLink link;
};

typedef struct OSCond {
    OSThreadQueue queue;
} OSCond;

void OSInitMutex(OSMutex* mutex);
void OSLockMutex(OSMutex* mutex);
void OSUnlockMutex(OSMutex* mutex);
void __OSUnlockAllMutex(OSThread* thread);
BOOL OSTryLockMutex(OSMutex* mutex);

void OSInitCond(OSCond* cond);
void OSSignalCond(OSCond* cond);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_MUTEX_H
