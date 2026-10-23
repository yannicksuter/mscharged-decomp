#ifndef RVL_SDK_ESP_H
#define RVL_SDK_ESP_H
#include <revolution/types.h>
#include <revolution/esp/ESPTypes_fwd.h>

#include <revolution/arc.h>
#include <revolution/ipc.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ES_IOCTLV_LAUNCH_TITLE = 8,
    ES_IOCTLV_READ_CONTENT_FILE = 10,
    ES_IOCTLV_GET_NUM_TICKET_VIEWS = 18,
    ES_IOCTLV_GET_TICKET_VIEWS = 19,
    ES_IOCTLV_GET_DATA_DIR = 29,
    ES_IOCTLV_GET_TITLE_ID = 32,
    ES_IOCTLV_SEEK_CONTENT_FILE = 35,
} ESIoctl;

struct ESPTicket {
    u8 dummy[0x2A4];
};

struct ESPTmd {
    u8 dummy[0x49E4];
};

#if defined(VERSION_R4QK01)
typedef struct ESLpEntry {
    u32 code;
    u32 limit;
} ESLpEntry;

#pragma pack(push, 4)
struct ESPTicketView {
    u8 unk_0x00[4];
    u64 ticketId;
    u8 unk_0x0C[0x98 - 0x0C];
    ESLpEntry limits[8];
};
#pragma pack(pop)

s32 ESP_InitLib(void);
s32 ESP_CloseLib(void);
s32 ESP_GetDataDir(u64 titleId, char* dataDir);
s32 ESP_GetTitleId(u64* titleId);
s32 ESP_GetTicketViews(u64 titleId, ESPTicketView* ticketViewList, u32* numTicketViews);
s32 ESP_LaunchTitle(u64 titleId, ESPTicketView* ticketView);
s32 ESP_DiGetTicketView(const ESPTicket* ticket, ESPTicketView* ticketView);
s32 ESP_DiGetTmd(ESPTmd* tmd, u32* numTmdBytes);
s32 ESP_GetConsumption(u64 ticketId, ESLpEntry* entries, u32* numEntries);
#endif

s32 ESP_ReadContentFile(s32 fd, void* dst, u32 len);
s32 ESP_SeekContentFile(s32 fd, s32 offset, s32 origin);

#ifdef __cplusplus
}
#endif
#endif
