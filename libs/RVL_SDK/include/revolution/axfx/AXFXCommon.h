#ifndef RVL_SDK_AXFX_COMMON_H
#define RVL_SDK_AXFX_COMMON_H
#include <revolution/types.h>
#include <revolution/axfx/AXFXCommon_fwd.h>
#ifdef __cplusplus
extern "C" {
#endif

struct AXFX_BUS {
    s32* left;     // at 0x0
    s32* right;    // at 0x4
    s32* surround; // at 0x8
};

struct AXFX_BUS_DPL2 {
    s32* left;           // at 0x0
    s32* right;          // at 0x4
    s32* left_surround;  // at 0x8
    s32* right_surround; // at 0xC
};

struct AXFX_BUFFERUPDATE {
    s32* left;     // at 0x0
    s32* right;    // at 0x4
    s32* surround; // at 0x8
};

struct AXFX_BUFFERUPDATE_DPL2 {
    s32* left;           // at 0x0
    s32* right;          // at 0x4
    s32* left_surround;  // at 0x8
    s32* right_surround; // at 0xC
};

#ifdef __cplusplus
}
#endif
#endif
