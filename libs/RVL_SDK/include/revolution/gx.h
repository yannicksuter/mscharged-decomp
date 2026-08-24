#ifndef REVOLUTION_GX_H
#define REVOLUTION_GX_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GX_VFILTER_SZ 7

typedef u8 GXBool;

typedef struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

typedef struct GXRenderModeObj {
    u32 tvInfo;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xfbMode;
    GXBool field_rendering;
    GXBool aa;
    u8 sample_pattern[12][2];
    u8 vfilter[GX_VFILTER_SZ];
} GXRenderModeObj;

typedef enum GXTexFmt {
    GX_TF_I4,
    GX_TF_I8,
    GX_TF_IA4,
    GX_TF_IA8,
    GX_TF_RGB565,
    GX_TF_RGB5A3,
    GX_TF_RGBA8,
    GX_TF_CMPR = 14,
} GXTexFmt;

void GXAbortFrame(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_GX_H
