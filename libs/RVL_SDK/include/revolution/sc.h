#ifndef REVOLUTION_SC_H
#define REVOLUTION_SC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SC_MAX_DEV_ENTRY_FOR_STD 10
#define SC_MAX_DEV_ENTRY_FOR_SMP 6
#define SC_MAX_DEV_ENTRY (SC_MAX_DEV_ENTRY_FOR_STD + SC_MAX_DEV_ENTRY_FOR_SMP)
#define SC_PRDINFO_SIZE 0x100

typedef u8 BD_ADDR[6];
typedef u8 LINK_KEY[16];

typedef enum {
    SC_STATUS_OK,
    SC_STATUS_BUSY,
    SC_STATUS_FATAL,
    SC_STATUS_PARSE
} SCStatus;

typedef enum { SC_ASPECT_STD, SC_ASPECT_WIDE } SCAspectRatio;

typedef enum { SC_EURGB_50_HZ, SC_EURGB_60_HZ } SCEuRgb60Mode;

typedef enum {
    SC_LANG_JP,
    SC_LANG_EN,
    SC_LANG_DE,
    SC_LANG_FR,
    SC_LANG_SP,
    SC_LANG_IT,
    SC_LANG_NL,
    SC_LANG_ZH_S,
    SC_LANG_ZH_T,
    SC_LANG_KR,
    SC_LANG_MAX
} SCLanguage;

typedef enum { SC_INTERLACED, SC_PROGRESSIVE } SCProgressiveMode;

typedef enum { SC_MOTOR_OFF, SC_MOTOR_ON } SCMotorMode;

typedef enum { SC_SND_MONO, SC_SND_STEREO, SC_SND_SURROUND } SCSoundMode;

typedef enum { SC_SENSOR_BAR_BOTTOM, SC_SENSOR_BAR_TOP } SCSensorBarPos;

typedef enum {
    SC_AREA_JPN,
    SC_AREA_USA,
    SC_AREA_EUR,
    SC_AREA_AUS,
    SC_AREA_BRA,
    SC_AREA_TWN,
    SC_AREA_KOR,
    SC_AREA_HKG,
    SC_AREA_ASI,
    SC_AREA_LTN,
    SC_AREA_SAF,
} SCProductArea;

typedef enum {
    SC_ITEM_IPL_CB,
    SC_ITEM_IPL_AR,
    SC_ITEM_IPL_ARN,
    SC_ITEM_IPL_CD,
    SC_ITEM_IPL_CD2,
    SC_ITEM_IPL_DH,
    SC_ITEM_IPL_E60,
    SC_ITEM_IPL_EULA,
    SC_ITEM_IPL_FRC,
    SC_ITEM_IPL_IDL,
    SC_ITEM_IPL_INC,
    SC_ITEM_IPL_LNG,
    SC_ITEM_IPL_NIK,
    SC_ITEM_IPL_PC,
    SC_ITEM_IPL_PGS,
    SC_ITEM_IPL_SSV,
    SC_ITEM_IPL_SADR,
    SC_ITEM_IPL_SND,
    SC_ITEM_IPL_UPT,
    SC_ITEM_NET_CNF,
    SC_ITEM_NET_CTPC,
    SC_ITEM_NET_PROF,
    SC_ITEM_NET_WCPC,
    SC_ITEM_NET_WCFG,
    SC_ITEM_DEV_BTM,
    SC_ITEM_DEV_VIM,
    SC_ITEM_DEV_CTC,
    SC_ITEM_DEV_DSM,
    SC_ITEM_BT_DINF,
    SC_ITEM_BT_SENS,
    SC_ITEM_BT_SPKV,
    SC_ITEM_BT_MOT,
    SC_ITEM_BT_BAR,
    SC_ITEM_DVD_CNF,
    SC_ITEM_WWW_RST,
    SC_ITEM_MAX
} SCItemID;

typedef struct SCIdleModeInfo {
    u8 wc24;
    u8 slotLight;
} SCIdleModeInfo;

#define SC_SIMPLE_ADDRESS_ID_COUNTRY 24
#define SC_SIMPLE_ADDRESS_ID_REGION 16
#define SC_SIMPLE_ADDRESS_ID_CITY 0

typedef struct SCSimpleAddress {
    u32 id;
    u16 countryName[16][64];
    u16 regionName[16][64];
    u16 latitude;
    u16 longitude;
} SCSimpleAddress;

typedef struct SCDevInfo {
    char devName[20];
    char at_0x14[1];
    char UNK_0x15[0xB];
    LINK_KEY linkKey;
    char UNK_0x30[0x10];
} SCDevInfo;

/* the same structure under the SDK's other spelling */
typedef SCDevInfo SC_BT_DEV_INFO;

typedef struct SCBtDeviceInfo {
    BD_ADDR addr;
    SCDevInfo info;
} SCBtDeviceInfo;

typedef struct SCBtDeviceInfoArray {
    u8 numRegist;
    union {
        struct {
            SCBtDeviceInfo regist[SC_MAX_DEV_ENTRY_FOR_STD];
            SCBtDeviceInfo active[SC_MAX_DEV_ENTRY_FOR_SMP];
        };
        SCBtDeviceInfo devices[SC_MAX_DEV_ENTRY];
    };
} SCBtDeviceInfoArray;

typedef struct SCBtCmpDevInfo {
    BD_ADDR addr;      // 0x00
    u8 name[64];       // 0x06
    LINK_KEY linkKey;  // 0x30
} SCBtCmpDevInfo;

typedef struct SCBtCmpDevInfoArray {
    u8 numRegist;                                      // 0x00
    SCBtCmpDevInfo devices[SC_MAX_DEV_ENTRY_FOR_SMP];  // 0x01
} SCBtCmpDevInfoArray;

BOOL SCGetBtCmpDevInfoArray(SCBtCmpDevInfoArray* devInfo);
BOOL SCSetBtCmpDevInfoArray(SCBtCmpDevInfoArray* devInfo);

u32 SCGetBtDpdSensibility(void);
BOOL SCSetBtDpdSensibility(u32 dpdSensibility);

u8 SCGetAspectRatio(void);
s8 SCGetDisplayOffsetH(void);
u8 SCGetEuRgb60Mode(void);
void SCGetIdleMode(SCIdleModeInfo* mode);
u8 SCGetLanguage(void);
u8 SCGetProgressiveMode(void);
u8 SCGetScreenSaverMode(void);
u8 SCGetSoundMode(void);
u32 SCGetCounterBias(void);
void SCGetBtDeviceInfoArray(SCBtDeviceInfoArray* info);
BOOL SCSetBtDeviceInfoArray(const SCBtDeviceInfoArray* info);
u32 SCGetBtDpdSensibility(void);
u8 SCGetWpadMotorMode(void);
BOOL SCSetWpadMotorMode(u8 mode);
u8 SCGetWpadSensorBarPosition(void);
u8 SCGetWpadSpeakerVolume(void);
BOOL SCSetWpadSpeakerVolume(u8 vol);
u32 SCGetSimpleAddressID(void);
BOOL SCGetSimpleAddressData(SCSimpleAddress* address);

typedef void (*SCFlushCallback)(SCStatus status);

void SCInit(void);
void SCFlushAsync(SCFlushCallback callback);
u32 SCCheckStatus(void);

BOOL SCFindByteArrayItem(void* dst, u32 len, SCItemID id);
BOOL SCReplaceByteArrayItem(const void* src, u32 len, SCItemID id);
BOOL SCFindU8Item(u8* dst, SCItemID id);
BOOL SCFindS8Item(s8* dst, SCItemID id);
BOOL SCFindU32Item(u32* dst, SCItemID id);
BOOL SCReplaceU8Item(u8 data, SCItemID id);

BOOL __SCF1(const char* type, char* buf, u32 sz);
BOOL SCGetProductAreaString(char* buf, u32 sz);
s8 SCGetProductArea(void);
char* SCGetProductCode(void);
BOOL SCGetProductSNString(char* buf, u32 sz);
BOOL SCGetProductSN(u32* sn);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_SC_H
