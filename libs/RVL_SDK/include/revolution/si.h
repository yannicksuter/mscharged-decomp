#ifndef REVOLUTION_SI_H
#define REVOLUTION_SI_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SI_MAX_SAMPLE_RATE 11

void SIInit(void);
void SISetXY(u32 lines, u32 times);
void SISetSamplingRate(u32 msec);
void SIRefreshSamplingRate(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_SI_H
