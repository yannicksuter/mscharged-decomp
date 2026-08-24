#ifndef REVOLUTION_OS_RESET_H
#define REVOLUTION_OS_RESET_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL (*OSShutdownFunction)(BOOL final, u32 event);

typedef struct OSShutdownFunctionInfo OSShutdownFunctionInfo;

struct OSShutdownFunctionInfo {
    OSShutdownFunction func;
    u32 priority;
    OSShutdownFunctionInfo* next;
    OSShutdownFunctionInfo* prev;
};

typedef struct OSShutdownFunctionQueue {
    OSShutdownFunctionInfo* head;
    OSShutdownFunctionInfo* tail;
} OSShutdownFunctionQueue;

enum {
    OS_SHUTDOWN_FATAL = 0,
    OS_SHUTDOWN_REBOOT = 1,
    OS_SHUTDOWN_SHUTDOWN = 2,
    OS_SHUTDOWN_IDLE = 3,
    OS_SHUTDOWN_RESTART = 4,
    OS_SHUTDOWN_RETURN_MENU = 5,
    OS_SHUTDOWN_EXEC = 6,
    OS_SHUTDOWN_LAUNCH = 7
};

void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info);
BOOL __OSCallShutdownFunctions(BOOL final, u32 event);
void __OSShutdownDevices(u32 event);

void OSRebootSystem(void);
void OSShutdownSystem(void);
void OSShutdownSystemForBS(u32 resetCode);
void OSRestart(u32 resetCode);
void OSReturnToMenu(void);
void __OSReturnToMenuForError(void);

u32 OSGetResetCode(void);
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_RESET_H
