#ifndef REVOLUTION_OS_THREAD_H
#define REVOLUTION_OS_THREAD_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_PRIORITY_MIN 0
#define OS_PRIORITY_MAX 31
#define OS_THREAD_STACK_MAGIC 0xDEADBABE

typedef enum OSThreadState {
    OS_THREAD_STATE_EXITED = 0,
    OS_THREAD_STATE_READY = 1,
    OS_THREAD_STATE_RUNNING = 2,
    OS_THREAD_STATE_SLEEPING = 4,
    OS_THREAD_STATE_MORIBUND = 8,
} OSThreadState;

typedef enum OSThreadFlag {
    OS_THREAD_DETACHED = (1 << 0),
} OSThreadFlag;

typedef struct OSThread OSThread;
typedef struct OSMutex OSMutex;

typedef struct OSThreadQueue {
    OSThread* head;
    OSThread* tail;
} OSThreadQueue;

typedef struct OSMutexQueue {
    OSMutex* head;
    OSMutex* tail;
} OSMutexQueue;

typedef struct OSMutexLink {
    OSMutex* next;
    OSMutex* prev;
} OSMutexLink;

struct OSThread {
    OSContext context;
    u16 state;
    u16 flags;
    s32 suspend;
    s32 priority;
    s32 base;
    u32 val;
    OSThreadQueue* queue;
    OSThread* next;
    OSThread* prev;
    OSThreadQueue joinQueue;
    OSMutex* mutex;
    OSMutexQueue mutexQueue;
    OSThread* nextActive;
    OSThread* prevActive;
    u32* stackBegin;
    u32* stackEnd;
    s32 error;
    void* specific[2];
};

// Compatibility spelling used by the existing mutex reconstruction.
#define queueMutex mutexQueue

typedef void (*OSSwitchThreadCallback)(OSThread* current, OSThread* next);
typedef void* (*OSThreadFunc)(void* argument);

OSSwitchThreadCallback
OSSetSwitchThreadCallback(OSSwitchThreadCallback callback);
void __OSThreadInit(void);
void OSSetCurrentThread(OSThread* thread);
void OSInitMutexQueue(OSMutexQueue* queue);
void OSInitThreadQueue(OSThreadQueue* queue);
OSThread* OSGetCurrentThread(void);
BOOL OSIsThreadTerminated(OSThread* thread);
s32 OSDisableScheduler(void);
s32 OSEnableScheduler(void);
s32 __OSGetEffectivePriority(OSThread* thread);
void __OSPromoteThread(OSThread* thread, s32 priority);
void __OSReschedule(void);
void OSYieldThread(void);
BOOL OSCreateThread(OSThread* thread, OSThreadFunc function, void* argument,
                    void* stackBegin, u32 stackSize, s32 priority, u16 flags);
void OSExitThread(OSThread* value);
void OSCancelThread(OSThread* thread);
BOOL OSJoinThread(OSThread* thread, void* value);
void OSDetachThread(OSThread* thread);
s32 OSResumeThread(OSThread* thread);
s32 OSSuspendThread(OSThread* thread);
void OSSleepThread(OSThreadQueue* queue);
void OSWakeupThread(OSThreadQueue* queue);
BOOL OSSetThreadPriority(OSThread* thread, s32 priority);
void OSClearStack(u8 value);
void OSSleepTicks(s64 ticks);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_THREAD_H
