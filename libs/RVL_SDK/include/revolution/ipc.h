#ifndef REVOLUTION_IPC_H
#define REVOLUTION_IPC_H

#include <revolution/os/OSThread.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    volatile u32 IPC_HW_REGS_PPC[] AT_ADDRESS(0xCD000000);
    volatile u32 IPC_HW_REGS[] AT_ADDRESS(0xCD800000);

    typedef enum IPCHwReg
    {
        IPC_PPCMSG = 0x00 / 4,
        IPC_PPCCTRL = 0x04 / 4,
        IPC_ARMMSG = 0x08 / 4,
        IPC_PPCIRQMASK = 0x34 / 4,
        IPC_DIFLAGS = 0x180 / 4,
        IPC_GPIO2DIR = 0x1CC / 4,
        IPC_GPIO2IN = 0x1D0 / 4
    } IPCHwReg;

    typedef enum ACRHwReg
    {
        ACR_PPCIRQFLAG = 0x030,
        ACR_PPCIRQMASK = 0x034,
        ACR_GPIO1BOUT = 0x0C0
    } ACRHwReg;

    typedef enum GPIOReg
    {
        GPIO_POWER = (1 << 0),
        GPIO_SHUTDOWN = (1 << 1),
        GPIO_FAN = (1 << 2),
        GPIO_DCDC = (1 << 3),
        GPIO_DISPIN = (1 << 4),
        GPIO_SLOTLED = (1 << 5),
        GPIO_EJECTBTN = (1 << 6),
        GPIO_SLOTIN = (1 << 7),
        GPIO_SENSORBAR = (1 << 8),
        GPIO_DOEJECT = (1 << 9),
        GPIO_EEP_CS = (1 << 10),
        GPIO_EEP_CLK = (1 << 11),
        GPIO_EEP_MOSI = (1 << 12)
    } GPIOReg;

    inline u32 ACRReadReg(u32 reg)
    {
        return IPC_HW_REGS_PPC[reg / 4];
    }

    inline void ACRWriteReg(u32 reg, u32 val)
    {
        IPC_HW_REGS_PPC[reg / 4] = val;
    }

    typedef enum IPCResult
    {
        IPC_RESULT_FATAL_ERROR = -119,
        IPC_RESULT_BUSY,
        IPC_RESULT_NOTEMPTY = -115,
        IPC_RESULT_ECC_CRIT,
        IPC_RESULT_OPENFD = -111,
        IPC_RESULT_MAXFD = -109,
        IPC_RESULT_MAXBLOCKS,
        IPC_RESULT_MAXFILES,
        IPC_RESULT_NOEXISTS,
        IPC_RESULT_EXISTS,
        IPC_RESULT_CORRUPT = -103,
        IPC_RESULT_ACCESS,
        IPC_RESULT_INVALID,

        IPC_RESULT_ALLOC_FAILED = -22,
        IPC_RESULT_ECC_CRIT_INTERNAL = -12,
        IPC_RESULT_BUSY_INTERNAL = -8,
        IPC_RESULT_NOEXISTS_INTERNAL = -6,
        IPC_RESULT_CONN_MAX_INTERNAL = -5,
        IPC_RESULT_INVALID_INTERNAL = -4,
        IPC_RESULT_EXISTS_INTERNAL = -2,
        IPC_RESULT_ACCESS_INTERNAL = -1,

        IPC_RESULT_OK = 0
    } IPCResult;

    typedef enum IPCRequestType
    {
        IPC_REQ_NONE,
        IPC_REQ_OPEN,
        IPC_REQ_CLOSE,
        IPC_REQ_READ,
        IPC_REQ_WRITE,
        IPC_REQ_SEEK,
        IPC_REQ_IOCTL,
        IPC_REQ_IOCTLV
    } IPCRequestType;

    typedef enum IPCSeekMode
    {
        IPC_SEEK_BEG,
        IPC_SEEK_CUR,
        IPC_SEEK_END
    } IPCSeekMode;

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

    typedef struct IPCOpenArgs
    {
        const char* path;
        IPCOpenMode mode;
    } IPCOpenArgs;

    typedef struct IPCReadWriteArgs
    {
        void* data;
        u32 length;
    } IPCReadWriteArgs;

    typedef struct IPCSeekArgs
    {
        s32 offset;
        IPCSeekMode mode;
    } IPCSeekArgs;

    typedef struct IPCIoctlArgs
    {
        s32 type;
        void* in;
        s32 inSize;
        void* out;
        s32 outSize;
    } IPCIoctlArgs;

    typedef struct IPCIoctlvArgs
    {
        s32 type;
        u32 inCount;
        u32 outCount;
        IPCIOVector* vectors;
    } IPCIoctlvArgs;

    typedef struct IPCRequest
    {
        IPCRequestType type;
        s32 ret;
        s32 fd;
        union
        {
            IPCOpenArgs open;
            IPCReadWriteArgs rw;
            IPCSeekArgs seek;
            IPCIoctlArgs ioctl;
            IPCIoctlvArgs ioctlv;
        };
    } IPCRequest;

    typedef struct IPCRequestEx
    {
        IPCRequest base;
        IPCAsyncCallback callback;
        void* callbackArg;
        BOOL reboot;
        OSThreadQueue queue;
        char padding[64 - 0x34];
    } IPCRequestEx;

    void IPCInit(void);
    u32 IPCReadReg(s32 index);
    void IPCWriteReg(s32 index, u32 value);
    void* IPCGetBufferHi(void);
    void* IPCGetBufferLo(void);
    void IPCSetBufferLo(void* lo);

    s32 IPCCltInit(void);
    s32 IOS_OpenAsync(const char* path, IPCOpenMode mode, IPCAsyncCallback callback,
        void* callbackArg);
    s32 IOS_Open(const char* path, IPCOpenMode mode);
    s32 IOS_CloseAsync(s32 fd, IPCAsyncCallback callback, void* callbackArg);
    s32 IOS_Close(s32 fd);
    s32 IOS_ReadAsync(s32 fd, void* buf, s32 len, IPCAsyncCallback callback,
        void* callbackArg);
    s32 IOS_Read(s32 fd, void* buf, s32 len);
    s32 IOS_WriteAsync(s32 fd, const void* buf, s32 len, IPCAsyncCallback callback,
        void* callbackArg);
    s32 IOS_Write(s32 fd, const void* buf, s32 len);
    s32 IOS_SeekAsync(s32 fd, s32 offset, IPCSeekMode mode,
        IPCAsyncCallback callback, void* callbackArg);
    s32 IOS_Seek(s32 fd, s32 offset, IPCSeekMode mode);
    s32 IOS_IoctlvAsync(s32 fd, s32 type, s32 inCount, s32 outCount,
        IPCIOVector* vectors, IPCAsyncCallback callback, void* callbackArg);
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
