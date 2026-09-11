#ifndef RVL_SDK_DVD_BROADWAY_H
#define RVL_SDK_DVD_BROADWAY_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#define DVD_LOW_OFFSET(x) ((x) >> 2)
#define DVD_LOW_SPEED(x) (((x) & 3) << 16)

// Forward declarations
typedef struct DVDDiskID DVDDiskID;
typedef struct DVDDriveInfo DVDDriveInfo;
typedef struct ESPTicket ESPTicket;
typedef struct ESPTmd ESPTmd;
typedef struct OSAlarm OSAlarm;

typedef enum {
    DVD_INTTYPE_TC = (1 << 0),   // Transaction callback?
    DVD_INTTYPE_DE = (1 << 1),   // Drive error
    DVD_INTTYPE_CVR = (1 << 2),  // Something with DVD cover
    DVD_INTTYPE_BR = (1 << 3),   // Break requested
    DVD_INTTYPE_TIME = (1 << 4), // Time out
    DVD_INTTYPE_SERR = (1 << 5), // Security error
    DVD_INTTYPE_VERR = (1 << 6), // Verify error
    DVD_INTTYPE_ARGS = (1 << 7), // Bad arguments
} DVDLowIntType;

// DICVR - DI Cover Register (via DVDLowGetCoverRegister)
#define DVD_DICVR_CVR (1 << 0)
#define DVD_DICVR_CVRINTMASK (1 << 1)
#define DVD_DICVR_CVRINT (1 << 2)

typedef struct diRegVals {
    u32 ImmRegVal;
    u32 CoverRegVal;
    u32 pad[6];
} diRegVals_t;

typedef struct diCommand {
    u8 theCommand;
    u8 pad1[3];
    u32 arg[5];
    u32 pad2[2];
} diCommand_t;

typedef struct DVDVideoPhysical {
    u8 data[2048];
} DVDVideoPhysical;

typedef struct DVDVideoDiscKey {
    u8 data[2048];
} DVDVideoDiscKey;

typedef struct DVDDiskBca {
    u8 optionalInfo[52];
    u8 manufacturerCode[2];
    u8 recorderDeviceCode[2];
    u8 APMRecorderDeviceCode[1];
    u8 discManufactureDate[2];
    u8 discManufactureTime[2];
    u8 discNumber[3];
} DVDDiskBca;

typedef struct DVDLowDriveSer {
    u8 data[12];
    u8 padding[20];
} DVDLowDriveSer;

typedef struct DVDVideoReportKey {
    u8 data[32];
} DVDVideoReportKey;

typedef void (*DVDLowCallback)(u32 intType);

BOOL DVDLowInit(void);
BOOL DVDLowReadDiskID(DVDDiskID* out, DVDLowCallback callback);
BOOL DVDLowOpenPartition(u32 offset, const ESPTicket* ticket, u32 certsSize,
                         const void* certs, ESPTmd* tmd,
                         DVDLowCallback callback);
BOOL DVDLowClosePartition(DVDLowCallback callback);
BOOL DVDLowUnencryptedRead(void* dst, u32 size, u32 offset,
                           DVDLowCallback callback);
BOOL DVDLowStopMotor(BOOL eject, BOOL kill, DVDLowCallback callback);
BOOL DVDLowInquiry(DVDDriveInfo* out, DVDLowCallback callback);
BOOL DVDLowRequestError(DVDLowCallback callback);
BOOL DVDLowSetSpinupFlag(BOOL enable);
BOOL DVDLowReset(DVDLowCallback callback);
BOOL DVDLowAudioBufferConfig(BOOL enable, u32 size, DVDLowCallback callback);
BOOL DVDLowSetMaximumRotation(u32 speed, DVDLowCallback callback);
BOOL DVDLowRead(void* dst, u32 size, u32 offset, DVDLowCallback callback);
BOOL DVDLowSeek(u32 offset, DVDLowCallback callback);
u32 DVDLowGetCoverRegister(void);
BOOL DVDLowPrepareCoverRegister(DVDLowCallback callback);
u32 DVDLowGetImmBufferReg(void);
BOOL DVDLowUnmaskStatusInterrupts(void);
BOOL DVDLowMaskCoverInterrupt(void);
BOOL DVDLowClearCoverInterrupt(DVDLowCallback callback);
BOOL __DVDLowTestAlarm(const OSAlarm* alarm);

#ifdef __cplusplus
}
#endif
#endif
