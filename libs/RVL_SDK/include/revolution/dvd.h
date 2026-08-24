#ifndef REVOLUTION_DVD_H
#define REVOLUTION_DVD_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DVDCommandBlock DVDCommandBlock;
typedef struct DVDFileInfo DVDFileInfo;
typedef struct OSAlarm OSAlarm;

#define DVD_DEVICE_CODE_READ (1 << 15)
#define MAKE_DVD_DEVICE_CODE(x) (DVD_DEVICE_CODE_READ | (x))

typedef enum DVDState {
    DVD_STATE_FATAL = -1,
    DVD_STATE_IDLE,
    DVD_STATE_BUSY,
    DVD_STATE_WAITING,
    DVD_STATE_COVER_CLOSED
} DVDState;

typedef void (*DVDAsyncCallback)(s32 result, DVDFileInfo* info);
typedef void (*DVDCommandCallback)(s32 result, DVDCommandBlock* block);

typedef struct DVDDiskID {
    char game[4];
    char company[2];
    u8 disk;
    u8 version;
    u8 strmEnable;
    u8 strmBufSize;
    u8 padding[14];
    u32 rvlMagic;
    u32 gcMagic;
} DVDDiskID;

struct DVDCommandBlock {
    DVDCommandBlock* next;
    DVDCommandBlock* prev;
    u32 command;
    volatile s32 state;
    u32 offset;
    u32 length;
    void* addr;
    u32 transferSize;
    u32 transferTotal;
    DVDDiskID* id;
    DVDCommandCallback callback;
    void* userData;
};

typedef struct DVDDriveInfo {
    u16 revision;
    u16 deviceCode;
    u32 releaseDate;
    char padding[32 - 0x8];
} DVDDriveInfo;

extern BOOL __DVDLongFileNameFlag;
extern volatile u32 __DVDLayoutFormat;

void DVDInit(void);
BOOL DVDInquiryAsync(DVDCommandBlock* block, DVDDriveInfo* info, DVDCommandCallback callback);
BOOL __DVDTestAlarm(const OSAlarm* alarm);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_DVD_H
