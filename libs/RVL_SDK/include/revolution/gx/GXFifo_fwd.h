#ifndef RVL_SDK_GX_FIFO_FWD_H
#define RVL_SDK_GX_FIFO_FWD_H

#include <revolution/gx/GXPublicTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void GXGetGPStatus(GXBool* overhi, GXBool* underlow, GXBool* readIdle,
                   GXBool* cmdIdle, GXBool* brkpt);

#ifdef __cplusplus
}
#endif

#endif
