#ifndef REVOLUTION_PAD_H
#define REVOLUTION_PAD_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern u32 __PADSpec;

BOOL __PADDisableRecalibration(BOOL disable);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_PAD_H
