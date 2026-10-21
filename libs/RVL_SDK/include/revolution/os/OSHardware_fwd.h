#ifndef REVOLUTION_OS_HARDWARE_FWD_H
#define REVOLUTION_OS_HARDWARE_FWD_H

#include <decomp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed Wii time-base frequency, in Hz. */
#define OS_TIMER_CLOCK 60750000u

unsigned long OS_BUS_CLOCK_SPEED AT_ADDRESS(0x800000F8);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_HARDWARE_FWD_H
