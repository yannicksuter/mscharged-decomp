#ifndef REVOLUTION_OS_LAUNCH_H
#define REVOLUTION_OS_LAUNCH_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void __OSRelaunchTitle(u32);
void __OSReturnToMenul(u32, const char*, ...);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_LAUNCH_H
