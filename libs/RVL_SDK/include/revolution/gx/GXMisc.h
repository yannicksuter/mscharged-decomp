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

void GXAbortFrame(void);

void GXSetDrawSync(u16 token);

void GXDrawDone(void);
void GXPixModeSync(void);

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback);

#ifdef __cplusplus
}
#endif
#endif
