#pragma once

#include <revolution/types.h>

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

s64 DWCi_Np_GetTimeInSeconds(void);
struct tm* fn_8049BB54(s64* theTime);
u64 DWCi_Np_GetConsoleId(void);

static inline u32 DWCi_HtoLEl(u32 data)
{
    u32 tmp;
    tmp = ((data >> 8) & 0x00ff00ff) | ((data << 8) & 0xff00ff00);
    return (tmp >> 16) | (tmp << 16);
}

static inline void DWCi_Np_ToLE(const void* srcp, void* dstp, u32 size)
{
    u32 i;

    for (i = 0; i < size / 4; i++)
    {
        ((u32*)dstp)[i] = DWCi_HtoLEl(((u32*)srcp)[i]);
    }
}

#ifdef __cplusplus
}
#endif
