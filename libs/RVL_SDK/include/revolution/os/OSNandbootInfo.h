#ifndef REVOLUTION_OS_NANDBOOTINFO_H
#define REVOLUTION_OS_NANDBOOTINFO_H

#include <revolution/types.h>

typedef struct OSNandbootInfo {
    /* 0x00 */ u32 checkSum;
    /* 0x04 */ u32 argsOff;
#if defined(VERSION_R4QK01)
    /* 0x08 */ u8 unk_0x08[2];
    /* 0x0A */ u8 prevAppType;
    /* 0x0B */ u8 returnValue;
#else
    /* 0x08 */ u32 returnValue;
#endif
    /* 0x0C */ u32 argValue;
#if defined(VERSION_R4QK01)
    /* 0x10 */ u8 unk_0x10[8];
    /* 0x18 */ u64 prevTitleId;
    /* 0x20 */ u8 args[0x1000];
#else
    /* 0x10 */ u8 unk_0x10[0x1020 - 0x10];
#endif
} OSNandbootInfo; // size = 0x1020

#define OS_NANDBOOT_CHECKSUM_SIZE (sizeof(OSNandbootInfo) - 4)

BOOL __OSCreateNandbootInfo(void);
BOOL __OSWriteNandbootInfo(OSNandbootInfo* info);
BOOL __OSReadNandbootInfo(OSNandbootInfo* info);

#endif  // REVOLUTION_OS_NANDBOOTINFO_H
