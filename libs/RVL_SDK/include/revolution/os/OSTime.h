#ifndef REVOLUTION_OS_TIME_H
#define REVOLUTION_OS_TIME_H

#include <revolution/os/OSHardware.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 OSTick;
typedef s64 OSTime;

typedef struct OSCalendarTime {
    s32 sec;
    s32 min;
    s32 hour;
    s32 mday;
    s32 month;
    s32 year;
    s32 wday;
    s32 yday;
    s32 msec;
    s32 usec;
} OSCalendarTime;

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

#define OS_TICKS_DELTA(x, y) ((s32)(x) - (s32)(y))
#define OSDiffTick(tick1, tick0) ((s32)(tick1) - (s32)(tick0))

OSTime OSGetTime(void);
OSTick OSGetTick(void);
OSTime __OSGetSystemTime(void);
OSTime __OSTimeToSystemTime(OSTime time);
void OSTicksToCalendarTime(OSTime time, OSCalendarTime* calendar);
OSTime OSCalendarTimeToTicks(const OSCalendarTime* calendar);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_TIME_H
