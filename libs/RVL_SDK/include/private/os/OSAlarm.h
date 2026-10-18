#ifndef PRIVATE_OS_ALARM_H
#define PRIVATE_OS_ALARM_H

#include <revolution/os/OSAlarm.h>
#include <revolution/os/OSThread_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif

void __OSSetInternalAlarmUserData(OSAlarm* alarm, void* userData);
void __OSCancelInternalAlarms(OSThread* thread);

#ifdef __cplusplus
}
#endif

#endif // PRIVATE_OS_ALARM_H
