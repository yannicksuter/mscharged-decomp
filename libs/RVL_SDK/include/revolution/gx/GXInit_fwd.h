#ifndef RVL_SDK_GX_INIT_FWD_H
#define RVL_SDK_GX_INIT_FWD_H

#include <revolution/gx/GXFifo_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif

GXFifoObj* GXInit(void* base, unsigned long size);

#ifdef __cplusplus
}
#endif

#endif // RVL_SDK_GX_INIT_FWD_H
