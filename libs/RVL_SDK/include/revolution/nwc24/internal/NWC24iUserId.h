#ifndef RVL_SDK_NWC24_INTERNAL_USER_ID_H
#define RVL_SDK_NWC24_INTERNAL_USER_ID_H

#include <revolution/nwc24/NWC24Types.h>

#ifdef __cplusplus
extern "C" {
#endif

NWC24Err NWC24iCheckUserIdCRC(u64 userId);

#ifdef __cplusplus
}
#endif

#endif
