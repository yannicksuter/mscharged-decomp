#ifndef REVOLUTION_OS_THREAD_FWD_H
#define REVOLUTION_OS_THREAD_FWD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSThread OSThread;
typedef void* (*OSThreadFunc)(void* argument);

int OSIsThreadTerminated(OSThread* thread);
int OSCreateThread(OSThread* thread, OSThreadFunc function, void* argument,
                   void* stackBegin, unsigned long stackSize,
                   long priority, unsigned short flags);
long OSResumeThread(OSThread* thread);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_THREAD_FWD_H
