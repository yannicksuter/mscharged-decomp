#ifndef RVL_SDK_GX_FIFO_H
#define RVL_SDK_GX_FIFO_H
#include <revolution/types.h>

#include <revolution/gx/GXInternal.h>

#include <revolution/os.h>
#ifdef __cplusplus
extern "C" {
#endif

GX_PUBLIC_STRUCT_DECL(GXFifoObj, 128);

typedef void (*GXBreakPtCallback)(void);

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size);
void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr);
void GXInitFifoLimits(GXFifoObj* fifo, u32 hiWatermark, u32 loWatermark);

void GXGetGPStatus(GXBool* overhi, GXBool* underlow, GXBool* readIdle,
                   GXBool* cmdIdle, GXBool* brkpt);

void GXSetCPUFifo(GXFifoObj* fifo);
void GXSetGPFifo(GXFifoObj* fifo);
GXBool GXGetCPUFifo(GXFifoObj* fifo);
GXFifoObj* GXGetGPFifo(void);

void GXGetFifoPtrs(const GXFifoObj*, void** readPtr, void** writePtr);

u32 GXGetFifoCount(const GXFifoObj* fifo);
GXBool GXGetFifoWrap(const GXFifoObj* fifo);

GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb);

GXBool __GXIsGPFifoReady(void);
void __GXFifoInit(void);
void __GXCleanGPFifo(void);

void GXEnableBreakPt(void* writePtr);
void GXDisableBreakPt(void);

typedef struct OSThread OSThread;

OSThread* GXGetCurrentGXThread(void);

#ifdef __cplusplus
}
#endif
#endif
