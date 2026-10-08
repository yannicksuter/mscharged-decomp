#ifndef REVOLUTION_OS_TIME_FWD_H
#define REVOLUTION_OS_TIME_FWD_H

#include <revolution/os/OSHardware_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long OSTick;
typedef signed long long OSTime;

typedef struct OSCalendarTime {
    signed long sec;
    signed long min;
    signed long hour;
    signed long mday;
    signed long month;
    signed long year;
    signed long wday;
    signed long yday;
    signed long msec;
    signed long usec;
} OSCalendarTime;

OSTime OSGetTime(void);
OSTick OSGetTick(void);
OSTime __OSGetSystemTime(void);
OSTime __OSTimeToSystemTime(OSTime time);
void OSTicksToCalendarTime(OSTime time, OSCalendarTime* calendar);
OSTime OSCalendarTimeToTicks(const OSCalendarTime* calendar);

#define OS_TIME_SPEED (OS_BUS_CLOCK_SPEED / 4)

#define OS_TICKS_TO_SEC(x) ((x) / OS_TIME_SPEED)
#define OS_TICKS_TO_MSEC(x) ((x) / (OS_TIME_SPEED / 1000))
#define OS_TICKS_TO_USEC(x) (((x) * 8) / (OS_TIME_SPEED / 125000))

#define OS_SEC_TO_TICKS(x) ((x) * OS_TIME_SPEED)
#define OS_MSEC_TO_TICKS(x) ((x) * (OS_TIME_SPEED / 1000))
#define OS_USEC_TO_TICKS(x) ((x) * (OS_TIME_SPEED / 125000) / 8)
#define OS_NSEC_TO_TICKS(x) ((x) * (OS_TIME_SPEED / 125000) / 8000)

#define OSTicksToSeconds(ticks) ((ticks) / OS_TIME_SPEED)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIME_SPEED / 1000))
#define OSTicksToMicroseconds(ticks) (((ticks) * 8) / (OS_TIME_SPEED / 125000))
#define OSSecondsToTicks(sec) OS_SEC_TO_TICKS(sec)
#define OSMillisecondsToTicks(msec) OS_MSEC_TO_TICKS(msec)
#define OSMicrosecondsToTicks(usec) OS_USEC_TO_TICKS(usec)
#define OSNanosecondsToTicks(nsec) OS_NSEC_TO_TICKS(nsec)

#define OS_TICKS_DELTA(x, y) ((signed long)(x) - (signed long)(y))
#define OSDiffTick(tick1, tick0) ((signed long)(tick1) - (signed long)(tick0))


#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_TIME_FWD_H
