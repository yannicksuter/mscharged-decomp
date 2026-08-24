#ifndef _GLXSWAP_H_
#define _GLXSWAP_H_

#include "types.h"

void glxSwapPost();
void glxSwapPre();
void glxInitSwap(void* fb0, void* fb1);
void glxSwapWaitDrawDone();
void glxSwapBuffers();
void glxSetSwapMode(s32 mode);
void glxSetDrawSyncTimeout(f32 seconds);
void glxRequestScreenShot();
s32 glxGetFrameCount();
void* glxGetBackBuffer();
void* glxGetDisplayedBuffer();
void glxSwapSetBlack(bool black);

#endif // _GLXSWAP_H_
