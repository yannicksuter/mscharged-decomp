#ifndef REVOLUTION_OS_TIME_FWD_H
#define REVOLUTION_OS_TIME_FWD_H

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

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_TIME_FWD_H
