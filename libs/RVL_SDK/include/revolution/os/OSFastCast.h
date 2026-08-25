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

// clang-format off
static inline f32 __OSu16tof32(register const u16* arg) {
    register f32 ret;

    asm {
        psq_l ret, 0(arg), 1, 3
    }

    return ret;
}
// clang-format on

static inline void OSu16tof32(const u16* in, f32* out) {
    *out = __OSu16tof32(in);
}

// clang-format off
static inline u16 __OSf32tou16(register f32 arg) {
    f32 a;
    register f32* ptr = &a;
    u16 r;

    asm {
        psq_st arg, 0(ptr), 1, 3
    }

    r = *(u16*)ptr;
    return r;
}
// clang-format on

static inline void OSf32tou16(const f32* in, u16* out) {
    *out = __OSf32tou16(*in);
}

// clang-format off
static inline u8 __OSf32tou8(register f32 arg) {
    f32 a;
    register f32* ptr = &a;
    u8 r;

    asm {
        psq_st arg, 0(ptr), 1, 2
    }

    r = *(u8*)ptr;
    return r;
}
// clang-format on

static inline void OSf32tou8(f32* in, u8* out) {
    *out = __OSf32tou8(*in);
}

// clang-format off
static inline s16 __OSf32tos16(register f32 arg) {
    f32 a;
    register f32* ptr = &a;
    s16 r;

    asm {
        psq_st arg, 0(ptr), 1, 5
    }

    r = *(s16*)ptr;
    return r;
}
// clang-format on

static inline void OSf32tos16(const f32* in, s16* out) {
    *out = __OSf32tos16(*in);
}

#ifdef __cplusplus
}
#endif

#endif
