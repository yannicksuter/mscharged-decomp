#ifndef RVL_SDK_GX_TEXTURE_TYPES_H
#define RVL_SDK_GX_TEXTURE_TYPES_H

typedef enum _GXTexFilter {
    GX_NEAR,
    GX_LINEAR,
    GX_NEAR_MIP_NEAR,
    GX_LIN_MIP_NEAR,
    GX_NEAR_MIP_LIN,
    GX_LIN_MIP_LIN,
} GXTexFilter;

typedef enum _GXTexWrapMode {
    GX_CLAMP,
    GX_REPEAT,
    GX_MIRROR,

    GX_MAX_TEXWRAPMODE
} GXTexWrapMode;

typedef enum _GXTlutFmt {
    GX_TL_IA8,
    GX_TL_RGB565,
    GX_TL_RGB5A3,

    GX_MAX_TLUTFMT
} GXTlutFmt;

typedef enum _GXAnisotropy {
    GX_ANISO_1,
    GX_ANISO_2,
    GX_ANISO_4,

    GX_MAX_ANISOTROPY
} GXAnisotropy;

typedef enum _GXCITexFmt {
    GX_TF_C4 = 8,
    GX_TF_C8,
    GX_TF_C14X2,
} GXCITexFmt;

typedef enum _GXTexFmt {
    GX_TF_I4,
    GX_TF_I8,
    GX_TF_IA4,
    GX_TF_IA8,
    GX_TF_RGB565,
    GX_TF_RGB5A3,
    GX_TF_RGBA8,
    GX_TF_CMPR = 14,

    GX_CTF_R4 = 32,
    GX_CTF_RA4 = 34,
    GX_CTF_RA8 = 35,
    GX_CTF_YUVA8 = 38,
    GX_CTF_A8 = 39,
    GX_CTF_R8 = 40,
    GX_CTF_G8 = 41,
    GX_CTF_B8 = 42,
    GX_CTF_RG8 = 43,
    GX_CTF_GB8 = 44,

    GX_TF_Z8 = 17,
    GX_TF_Z16 = 19,
    GX_TF_Z24X8 = 22,

    GX_CTF_Z4 = 48,
    GX_CTF_Z8M = 57,
    GX_CTF_Z8L = 58,
    GX_CTF_Z16L = 60,

    GX_TF_A8 = GX_CTF_A8
} GXTexFmt;

typedef enum _GXTexMapID {
    GX_TEXMAP0,
    GX_TEXMAP1,
    GX_TEXMAP2,
    GX_TEXMAP3,
    GX_TEXMAP4,
    GX_TEXMAP5,
    GX_TEXMAP6,
    GX_TEXMAP7,
    GX_MAX_TEXMAP,

    GX_TEXMAP_NULL = 255,
    GX_TEX_DISABLE
} GXTexMapID;

#endif // RVL_SDK_GX_TEXTURE_TYPES_H
