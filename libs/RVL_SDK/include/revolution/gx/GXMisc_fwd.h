#ifndef RVL_SDK_GX_MISC_FWD_H
#define RVL_SDK_GX_MISC_FWD_H

#include <revolution/gx/GXBlendTypes.h>
#include <revolution/gx/GXPublicTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GXDrawDoneCallback)(void);

void GXFlush(void);
void GXAbortFrame(void);
unsigned short GXReadDrawSync(void);
void GXSetDrawDone(void);
void GXWaitDrawDone(void);
void GXDrawDone(void);
void GXPeekARGB(unsigned short x, unsigned short y, unsigned long* color);
void GXPokeBlendMode(GXBlendMode type, GXBlendFactor src_factor,
                     GXBlendFactor dst_factor, GXLogicOp op);
void GXPokeColorUpdate(GXBool update_enable);
void GXPokeARGB(unsigned short x, unsigned short y, unsigned long color);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback callback);

#ifdef __cplusplus
}
#endif

#endif
