#ifndef RVL_SDK_GX_RENDER_MODE_OBJ_H
#define RVL_SDK_GX_RENDER_MODE_OBJ_H

#define GX_VFILTER_SZ 7

typedef struct _GXRenderModeObj {
    union {
        unsigned long tvInfo;
        unsigned long viTVmode;
    }; // at 0x0
    unsigned short fbWidth; // at 0x4
    unsigned short efbHeight; // at 0x6
    unsigned short xfbHeight; // at 0x8
    unsigned short viXOrigin; // at 0xA
    unsigned short viYOrigin; // at 0xC
    unsigned short viWidth; // at 0xE
    unsigned short viHeight; // at 0x10
    union {
        unsigned long xfbMode;
        unsigned long xFBmode;
    }; // at 0x14
    unsigned char field_rendering; // at 0x18
    unsigned char aa; // at 0x19
    unsigned char sample_pattern[12][2]; // at 0x1A
    unsigned char vfilter[GX_VFILTER_SZ]; // at 0x32
} GXRenderModeObj;

#endif
