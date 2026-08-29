#ifndef RVL_SDK_GX_INIT_H
#define RVL_SDK_GX_INIT_H
#include <revolution/types.h>

#include <revolution/gx/GXFifo.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/gx/GXTransform.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GXData {
    union {
        u32 WORD_0x0;
        struct {
            union {
                u16 vNumNot;
                u16 SHORT_0x0;
            };
            union {
                u16 bpSentNot;
                u16 lastWriteWasXF;
            };
        };
    };
    union {
        u16 vNum;
        u16 SHORT_0x4;
    };
    union {
        u16 vLim;
        u16 vlim;
    };
    union {
        u32 cpEnable;
        u32 cpCtrlReg;
    };
    union {
        u32 cpStatus;
        u32 cpStatReg;
    };
    u32 cpClr;
    union {
        u32 vcdLo;
        u32 vcdLoReg;
    };
    union {
        u32 vcdHi;
        u32 vcdHiReg;
    };
    u32 vatA[GX_MAX_VTXFMT];
    u32 vatB[GX_MAX_VTXFMT];
    u32 vatC[GX_MAX_VTXFMT];
    union {
        u32 lpSize;
        u32 linePtWidth;
    };
    union {
        u32 matIdxA;
        u32 matrixIndex0;
    };
    union {
        u32 matIdxB;
        u32 matrixIndex1;
    };
    u32 indexBase[4];
    u32 indexStride[4];
    union {
        u32 ambColor[2];
        GXColor ambColors[2];
    };
    union {
        u32 matColor[2];
        GXColor matColors[2];
    };
    union {
        u32 chanCtrl[4];
        u32 colorControl[4];
    };
    union {
        u32 texGenCtrl[GX_MAX_TEXCOORD];
        u32 texRegs[GX_MAX_TEXCOORD];
    };
    union {
        u32 dualTexGenCtrl[GX_MAX_TEXCOORD];
        u32 dualTexRegs[GX_MAX_TEXCOORD];
    };
    union {
        u32 suTs0[GX_MAX_TEXCOORD];
        u32 txcRegs[GX_MAX_TEXCOORD];
    };
    u32 suTs1[GX_MAX_TEXCOORD];
    union {
        u32 suScis0;
        u32 scissorTL;
    };
    union {
        u32 suScis1;
        u32 scissorBR;
    };
    u32 tref[GX_MAX_TEVSTAGE / 2];
    union {
        u32 iref;
        u32 ras1_iref;
    };
    union {
        u32 bpMask;
        u32 ind_imask;
    };
    union {
        u32 IndTexScale0;
        u32 ras1_ss0;
    };
    union {
        u32 IndTexScale1;
        u32 ras1_ss1;
    };
    u32 tevc[GX_MAX_TEVSTAGE];
    u32 teva[GX_MAX_TEVSTAGE];
    u32 tevKsel[GX_MAX_TEVSTAGE / 2];
    union {
        u32 cmode0;
        u32 blendMode;
    };
    union {
        u32 cmode1;
        u32 dstAlpha;
    };
    union {
        u32 zmode;
        u32 zMode;
    };
    union {
        u32 peCtrl;
        u32 zControl;
    };
    u32 cpDispSrc;
    u32 cpDispSize;
    u32 cpDispStride;
    u32 cpDisp;
    u32 cpTexSrc;
    u32 cpTexSize;
    u32 cpTexStride;
    u32 cpTex;
    GXBool cpTexZ;
    u8 padding_251[3];
    u32 genMode;
    GXTexRegion TexRegions0[GX_MAX_TEXMAP];
    GXTexRegion TexRegions1[GX_MAX_TEXMAP];
    GXTexRegion TexRegions2[GX_MAX_TEXMAP];
    GXTlutRegion TlutRegions[GX_MAX_TLUT_ALL];
    GXTexRegionCallback texRegionCallback;
    GXTlutRegionCallback tlutRegionCallback;
    union {
        GXAttrType nrmType;
        GXAttrType normalType;
    };
    union {
        GXBool hasNrms;
        GXBool normal;
    };
    union {
        GXBool hasBiNrms;
        GXBool binormal;
    };
    u8 padding_526[2];
    GXProjectionType projType;
    union {
        f32 projMtx[GX_PROJECTION_SZ - 1];
        f32 proj[GX_PROJECTION_SZ - 1];
    };
    union {
        struct {
            f32 vpLeft;
            f32 vpTop;
            f32 vpWd;
            f32 vpHt;
            f32 vpNearz;
            f32 vpFarz;
        };
        struct {
            f32 vpOx;
            f32 vpOy;
            f32 vpSx;
            f32 vpSy;
            f32 vpNear;
            f32 vpFar;
        };
        f32 view[GX_VIEWPORT_SZ];
    };
    union {
        f32 zOffset;
        f32 offsetZ;
    };
    union {
        f32 zScale;
        f32 scaleZ;
    };
    u32 tImage0[GX_MAX_TEXMAP];
    u32 tMode0[GX_MAX_TEXMAP];
    u32 texmapId[GX_MAX_TEVSTAGE];
    u32 tcsManEnab;
    u32 tevTcEnab;
    GXPerf0 perf0;
    GXPerf1 perf1;
    u32 perfSel;
    union {
        GXBool inDispList;
        GXBool dlistActive;
    };
    union {
        GXBool dlSaveContext;
        GXBool dlistSave;
    };
    union {
        GXBool abtWaitPECopy;
        u8 BYTE_0x5FA;
    };
    union {
        u8 dirtyVAT;
        u8 vatDirtyFlags;
    };
    union {
        u32 dirtyState;
        u32 gxDirtyFlags;
    };
} GXData;

extern GXData* const __GXData;

#define gx __GXData
#define gxdt __GXData

GXFifoObj* GXInit(void*, u32);
void __GXInitGX(void);

#ifdef __cplusplus
}
#endif
#endif
