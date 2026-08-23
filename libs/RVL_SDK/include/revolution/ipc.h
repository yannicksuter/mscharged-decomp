#ifndef REVOLUTION_IPC_H
#define REVOLUTION_IPC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    volatile u32 IPC_HW_REGS_PPC[] AT_ADDRESS(0xCD000000);
    volatile u32 IPC_HW_REGS[] AT_ADDRESS(0xCD800000);

    typedef enum IPCHwReg
    {
        IPC_PPCIRQMASK = 0x34 / 4,
        IPC_DIFLAGS = 0x180 / 4,
        IPC_GPIO2DIR = 0x1CC / 4,
        IPC_GPIO2IN = 0x1D0 / 4
    } IPCHwReg;

    typedef enum IPCResult
    {
        IPC_RESULT_CONN_MAX_INTERNAL = -5,
        IPC_RESULT_INVALID_INTERNAL = -4,
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

    typedef struct IPCIOVector
    {
        void* base;
        u32 length;
    } IPCIOVector;

    typedef struct IPCRequestEx IPCRequestEx;

    void IPCInit(void);
    u32 IPCReadReg(s32 index);
    void IPCWriteReg(s32 index, u32 value);
    void* IPCGetBufferHi(void);
    void* IPCGetBufferLo(void);
    void IPCSetBufferLo(void* lo);

    s32 IOS_Open(const char* path, IPCOpenMode mode);
    s32 IOS_IoctlAsync(s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize,
        IPCAsyncCallback callback, void* callbackArg);
    s32 IOS_Ioctl(s32 fd, s32 type, void* in, s32 inSize, void* out, s32 outSize);
    s32 IOS_Ioctlv(s32 fd, s32 type, s32 inCount, s32 outCount,
        IPCIOVector* vectors);
    s32 IOS_IoctlvReboot(s32 fd, s32 type, s32 inCount, s32 outCount,
        IPCIOVector* vectors);

    s32 iosCreateHeap(void* base, u32 size);
    void* iosAllocAligned(s32 handle, u32 size, u32 align);
    s32 iosFree(s32 handle, void* block);

    void IPCiProfInit(void);
    void IPCiProfQueueReq(IPCRequestEx* req, s32 fd);
    void IPCiProfAck(void);
    void IPCiProfReply(IPCRequestEx* req, s32 fd);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_IPC_H
