#pragma once

#include <revolution/types.h>

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

s64 DWCi_Np_GetTimeInSeconds(void);
struct tm* fn_8049BB54(s64* theTime);
u64 DWCi_Np_GetConsoleId(void);

#ifdef __cplusplus
}
#endif
