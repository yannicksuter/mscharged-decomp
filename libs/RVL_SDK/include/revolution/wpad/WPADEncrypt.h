#ifndef RVL_SDK_WPAD_ENCRYPT_H
#define RVL_SDK_WPAD_ENCRYPT_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void WPADiDecode(s32 chan, void* buffer, u16 length, u16 offset);

#ifdef __cplusplus
}
#endif

#endif // RVL_SDK_WPAD_ENCRYPT_H
