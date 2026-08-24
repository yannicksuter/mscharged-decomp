#ifndef REVOLUTION_OS_IPC_H
#define REVOLUTION_OS_IPC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void* __OSGetIPCBufferHi(void);
void* __OSGetIPCBufferLo(void);
void __OSInitIPCBuffer(void);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_IPC_H
