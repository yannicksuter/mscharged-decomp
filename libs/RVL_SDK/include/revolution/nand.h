#ifndef REVOLUTION_NAND_H
#define REVOLUTION_NAND_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NAND_RESULT_FATAL_ERROR (-128)
#define NAND_RESULT_UNKNOWN (-64)
#define NAND_RESULT_MAXDEPTH (-16)
#define NAND_RESULT_AUTHENTICATION (-15)
#define NAND_RESULT_OPENFD (-14)
#define NAND_RESULT_NOTEMPTY (-13)
#define NAND_RESULT_NOEXISTS (-12)
#define NAND_RESULT_MAXFILES (-11)
#define NAND_RESULT_MAXFD (-10)
#define NAND_RESULT_MAXBLOCKS (-9)
#define NAND_RESULT_INVALID (-8)
#define NAND_RESULT_EXISTS (-6)
#define NAND_RESULT_ECC_CRIT (-5)
#define NAND_RESULT_CORRUPT (-4)
#define NAND_RESULT_BUSY (-3)
#define NAND_RESULT_ALLOC_FAILED (-2)
#define NAND_RESULT_ACCESS (-1)
#define NAND_RESULT_OK 0

#define NAND_MAX_PATH 64

typedef enum NANDAccessType {
    NAND_ACCESS_NONE,
    NAND_ACCESS_READ,
    NAND_ACCESS_WRITE,
    NAND_ACCESS_RW
} NANDAccessType;

typedef enum NANDSeekMode {
    NAND_SEEK_BEG,
    NAND_SEEK_CUR,
    NAND_SEEK_END
} NANDSeekMode;

typedef struct NANDCommandBlock NANDCommandBlock;
typedef void (*NANDCallback)(s32 result, NANDCommandBlock* block);

typedef struct NANDStatus {
    u32 ownerId;
    u16 groupId;
    u8 attribute;
    u8 permission;
} NANDStatus;

typedef struct NANDFileInfo {
    s32 fileDescriptor;
    s32 origFd;
    char origPath[NAND_MAX_PATH];
    char tmpPath[NAND_MAX_PATH];
    u8 accType;
    u8 stage;
    u8 mark;
} NANDFileInfo;

struct NANDCommandBlock {
    void* userData;
    NANDCallback callback;
    NANDFileInfo* fileInfo;
    void* bytes;
    void* inodes;
    NANDStatus* status;
    u32 ownerId;
    u16 groupId;
    u8 nextStage;
    u32 attr;
    u32 ownerAcc;
    u32 groupAcc;
    u32 othersAcc;
    u32 num;
    char absPath[NAND_MAX_PATH];
    u32* length;
    u32* pos;
    s32 state;
    void* copyBuf;
    u32 bufLength;
    u8* type;
    u32 uniqNo;
    u32 reqBlocks;
    u32 reqInodes;
    u32* answer;
    u32 homeBlocks;
    u32 homeInodes;
    u32 userBlocks;
    u32 userInodes;
    u32 workBlocks;
    u32 workInodes;
    const char** dir;
};

s32 NANDInit(void);
s32 NANDDelete(const char* path);
s32 NANDOpen(const char* path, NANDFileInfo* info, u8 access);
s32 NANDOpenAsync(const char* path, NANDFileInfo* info, u8 access,
                  NANDCallback callback, NANDCommandBlock* block);
s32 NANDClose(NANDFileInfo* info);
s32 NANDCloseAsync(NANDFileInfo* info, NANDCallback callback,
                   NANDCommandBlock* block);
s32 NANDRead(NANDFileInfo* info, void* buffer, u32 length);
s32 NANDReadAsync(NANDFileInfo* info, void* buffer, u32 length,
                  NANDCallback callback, NANDCommandBlock* block);
s32 NANDWrite(NANDFileInfo* info, const void* buffer, u32 length);
s32 NANDWriteAsync(NANDFileInfo* info, const void* buffer, u32 length,
                   NANDCallback callback, NANDCommandBlock* block);
s32 NANDSeekAsync(NANDFileInfo* info, s32 offset, NANDSeekMode whence,
                  NANDCallback callback, NANDCommandBlock* block);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_NAND_H
