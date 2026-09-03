#ifndef NL_TIME_H
#define NL_TIME_H

#include "types.h"

void nlInitTime();
unsigned long long nlGetTime();
f32 nlGetTimeDifference(unsigned long long startTime, unsigned long long endTime);

#endif // NL_TIME_H
