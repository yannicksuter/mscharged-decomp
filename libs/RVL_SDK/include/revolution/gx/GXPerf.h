#ifndef RVL_SDK_GX_PERF_H
#define RVL_SDK_GX_PERF_H

#include <revolution/gx/GXTypes.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void GXSetGPMetric(GXPerf0 perf0, GXPerf1 perf1);
    void GXClearGPMetric(void);

#ifdef __cplusplus
}
#endif

#endif
