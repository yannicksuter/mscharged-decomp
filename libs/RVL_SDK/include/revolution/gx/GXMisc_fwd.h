#ifndef RVL_SDK_GX_MISC_FWD_H
#define RVL_SDK_GX_MISC_FWD_H

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
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback callback);

#ifdef __cplusplus
}
#endif

#endif
