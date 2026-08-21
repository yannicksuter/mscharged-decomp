#ifndef REVOLUTION_IPC_H
#define REVOLUTION_IPC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum IPCResult
    {
        IPC_RESULT_OK = 0
    } IPCResult;

    typedef enum IPCOpenMode
    {
        IPC_OPEN_NONE = 0,
        IPC_OPEN_READ = (1 << 0),
        IPC_OPEN_WRITE = (1 << 1),
        IPC_OPEN_RW = IPC_OPEN_READ | IPC_OPEN_WRITE
    } IPCOpenMode;

    typedef s32 (*IPCAsyncCallback)(s32 result, void* arg);

    s32 IOS_Open(const char* path, IPCOpenMode mode);
    s32 IOS_IoctlAsync(s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize,
        IPCAsyncCallback callback, void* callbackArg);
    s32 IOS_Ioctl(s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_IPC_H
