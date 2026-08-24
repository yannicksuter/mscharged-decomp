#ifndef REVOLUTION_OS_NANDBOOTINFO_H
#define REVOLUTION_OS_NANDBOOTINFO_H

#include <revolution/types.h>

typedef struct OSNandbootInfo {
    /* 0x00 */ u32 checkSum;
    /* 0x04 */ u32 argsOff;
    /* 0x08 */ u32 returnValue;
    /* 0x0C */ u32 argValue;
    /* 0x10 */ u8 unk_0x10[0x1020 - 0x10];
} OSNandbootInfo; // size = 0x1020

#define OS_NANDBOOT_CHECKSUM_SIZE (sizeof(OSNandbootInfo) - 4)

BOOL __OSCreateNandbootInfo(void);
BOOL __OSWriteNandbootInfo(OSNandbootInfo* info);
BOOL __OSReadNandbootInfo(OSNandbootInfo* info);

#endif  // REVOLUTION_OS_NANDBOOTINFO_H
