#ifndef REVOLUTION_OS_ALARM_H
#define REVOLUTION_OS_ALARM_H

#include <revolution/os/OSContext.h>
#include <revolution/types.h>

#define OSGetAlarmUserDataAny(type_, alarm_) ((type_)(OSGetAlarmUserData(alarm_)))
#define OSSetAlarmUserDataAny(alarm_, data_) OSSetAlarmUserData(alarm_, (void*)(data_))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSAlarm OSAlarm;
typedef void (*OSAlarmHandler)(OSAlarm* alarm, OSContext* context);

struct OSAlarm {
    OSAlarmHandler handler;
    u32 tag;
    s64 end;
    OSAlarm* prev;
    OSAlarm* next;
    s64 period;
    s64 start;
    void* userData;
};

typedef struct OSAlarmQueue {
    OSAlarm* head;
    OSAlarm* tail;
} OSAlarmQueue;

void __OSInitAlarm(void);
void OSCreateAlarm(OSAlarm* alarm);
void OSSetAlarm(OSAlarm* alarm, s64 tick, OSAlarmHandler handler);
void OSSetPeriodicAlarm(OSAlarm* alarm, s64 tick, s64 period, OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm* alarm);
void OSSetAlarmTag(OSAlarm* alarm, u32 tag);
void OSSetAlarmUserData(OSAlarm* alarm, void* userData);
void* OSGetAlarmUserData(const OSAlarm* alarm);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_ALARM_H
