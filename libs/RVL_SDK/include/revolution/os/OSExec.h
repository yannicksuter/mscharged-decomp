#ifndef REVOLUTION_OS_EXEC_H
#define REVOLUTION_OS_EXEC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSExecParams {
    BOOL valid;
    u32 restartCode;
    u32 bootDol;
    void* regionStart;
    void* regionEnd;
    BOOL argsUseDefault;
    void* argsAddr;
} OSExecParams;

extern BOOL __OSInReboot;
extern OSExecParams __OSRebootParams;

void __OSGetExecParams(OSExecParams* params);
void __OSLaunchMenu(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_EXEC_H
