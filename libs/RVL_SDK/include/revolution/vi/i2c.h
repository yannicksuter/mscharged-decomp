#ifndef RVL_SDK_VI_I2C_H
#define RVL_SDK_VI_I2C_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 __VISendI2CData(u8 slaveAddr, u8* data, s32 byteCount);
void WaitMicroTime(s32 microseconds);

#ifdef __cplusplus
}
#endif

#endif
