#ifndef _RVL_SDK_WENC_H
#define _RVL_SDK_WENC_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WENC_FLAG_USER_INFO = (1 << 0),
} WENCFlag;

typedef struct WENCInfo {
    u8 data[0x20];
} WENCInfo;

s32 WENCGetEncodeData(WENCInfo* info, u32 flag, const s16* pcmData, s32 samples, u8* adpcmData);

#ifdef __cplusplus
}
#endif

#endif
