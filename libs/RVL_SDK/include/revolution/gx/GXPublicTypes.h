#ifndef RVL_SDK_GX_PUBLIC_TYPES_H
#define RVL_SDK_GX_PUBLIC_TYPES_H

typedef unsigned char GXBool;

typedef struct _GXColor {
    unsigned char r, g, b, a;
} GXColor;

typedef enum _GXCompare {
    GX_NEVER,
    GX_LESS,
    GX_EQUAL,
    GX_LEQUAL,
    GX_GREATER,
    GX_NEQUAL,
    GX_GEQUAL,
    GX_ALWAYS
} GXCompare;

typedef enum _GXChannelID {
    GX_COLOR0,
    GX_COLOR1,
    GX_ALPHA0,
    GX_ALPHA1,
    GX_COLOR0A0,
    GX_COLOR1A1,
    GX_COLOR_ZERO,
    GX_ALPHA_BUMP,
    GX_ALPHA_BUMPN,

    GX_COLOR_NULL = 255
} GXChannelID;

#endif // RVL_SDK_GX_PUBLIC_TYPES_H
