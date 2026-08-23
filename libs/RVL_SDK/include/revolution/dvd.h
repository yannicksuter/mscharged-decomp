#ifndef REVOLUTION_DVD_H
#define REVOLUTION_DVD_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSAlarm OSAlarm;

BOOL __DVDTestAlarm(const OSAlarm* alarm);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_DVD_H
