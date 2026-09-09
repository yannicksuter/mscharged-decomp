#ifndef REVOLUTION_OS_TIME_FWD_H
#define REVOLUTION_OS_TIME_FWD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long OSTick;
typedef signed long long OSTime;

OSTime OSGetTime(void);
OSTick OSGetTick(void);
OSTime __OSGetSystemTime(void);
OSTime __OSTimeToSystemTime(OSTime time);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_TIME_FWD_H
