#ifndef RVL_SDK_GX_MISC_H
#define RVL_SDK_GX_MISC_H
#include <revolution/gx/GXTypes.h>
#include <revolution/types.h>
#include <revolution/gx/GXMisc_fwd.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GXDrawSyncCallback)(u16 token);

void GXSetMisc(GXMiscToken token, u32 val);
void GXResetWriteGatherPipe(void);

void __GXAbort(void);

void GXSetDrawSync(u16 token);

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

void GXPokeARGB(u16 x, u16 y, u32 color);
void GXPeekZ(u16 x, u16 y, u32* z);
void GXPokeZ(u16 x, u16 y, u32 z);

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback);

void __GXPEInit(void);

#ifdef __cplusplus
}
#endif
#endif
