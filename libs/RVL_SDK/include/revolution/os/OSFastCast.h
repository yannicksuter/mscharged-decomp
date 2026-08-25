#ifndef RVL_SDK_OS_FAST_CAST_H
#define RVL_SDK_OS_FAST_CAST_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#define OS_GQR_TYPE_U8 4
#define OS_GQR_TYPE_U16 5
#define OS_GQR_TYPE_S8 6
#define OS_GQR_TYPE_S16 7

// clang-format off
static void OSInitFastCast(void) {
    asm {
        li r3, OS_GQR_TYPE_U8
        oris r3, r3, OS_GQR_TYPE_U8
        mtspr 0x392, r3

        li r3, OS_GQR_TYPE_U16
        oris r3, r3, OS_GQR_TYPE_U16
        mtspr 0x393, r3

        li r3, OS_GQR_TYPE_S8
        oris r3, r3, OS_GQR_TYPE_S8
        mtspr 0x394, r3

        li r3, OS_GQR_TYPE_S16
        oris r3, r3, OS_GQR_TYPE_S16
        mtspr 0x395, r3
    }
}
// clang-format on

#ifdef __cplusplus
}
#endif
#endif
