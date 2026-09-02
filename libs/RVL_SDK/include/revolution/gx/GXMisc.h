#ifndef RVL_SDK_GX_MISC_H
#define RVL_SDK_GX_MISC_H
#include <revolution/gx/GXTypes.h>
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GXDrawDoneCallback)(void);
typedef void (*GXDrawSyncCallback)(u16 token);

void GXSetMisc(GXMiscToken token, u32 val);
void GXFlush(void);
void GXResetWriteGatherPipe(void);

void __GXAbort(void);
void GXAbortFrame(void);

void GXSetDrawSync(u16 token);
u16 GXReadDrawSync(void);

void GXSetDrawDone(void);
void GXWaitDrawDone(void);
void GXDrawDone(void);
void GXPixModeSync(void);
void GXTexModeSync(void);

void GXPokeAlphaMode(GXCompare func, u8 threshold);
void GXPokeAlphaRead(GXAlphaReadMode mode);
void GXPokeAlphaUpdate(GXBool update_enable);
void GXPokeBlendMode(GXBlendMode type, GXBlendFactor src_factor,
                     GXBlendFactor dst_factor, GXLogicOp op);
void GXPokeColorUpdate(GXBool update_enable);
void GXPokeDstAlpha(GXBool enable, u8 alpha);
void GXPokeDither(GXBool dither);
void GXPokeZMode(GXBool compare_enable, GXCompare func,
                 GXBool update_enable);

void GXPeekARGB(u16 x, u16 y, u32* color);
void GXPokeARGB(u16 x, u16 y, u32 color);
void GXPeekZ(u16 x, u16 y, u32* z);
void GXPokeZ(u16 x, u16 y, u32 z);

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback);

void __GXPEInit(void);

#ifdef __cplusplus
}
#endif
#endif
