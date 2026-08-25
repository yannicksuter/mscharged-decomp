#ifndef RVL_SDK_BTE_H
#define RVL_SDK_BTE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The Revolution SDK's WPAD and WUD layers sit on Broadcom's BTE Bluetooth
 * stack and use a handful of its public types. Only the declarations those
 * two headers actually need are reproduced here; the BTE stack itself is not
 * part of this project and none of it is compiled.
 *
 * Names, widths and BD_ADDR_LEN follow Broadcom's published BTE/Bluedroid
 * headers (bt_types.h, bta_hh_api.h, gki data_types.h).
 */

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef unsigned char BOOLEAN;

#define BD_ADDR_LEN 6
typedef UINT8 BD_ADDR[BD_ADDR_LEN];
typedef UINT8* BD_ADDR_PTR;

typedef UINT16 tBTA_HH_ATTR_MASK;
typedef UINT32 tBTA_SERVICE_MASK;

typedef struct {
    UINT16 event;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} BT_HDR;

typedef UINT8 tBTM_STATUS;

/* The BTE entry points WPAD calls into. */
void* GKI_getbuf(UINT16 size);
void BTA_DmSendHciReset(void);
void BTA_HhClose(UINT8 dev_handle);
void BTA_HhGetAclQueueInfo(void);
void BTA_HhSendData(UINT8 dev_handle, BT_HDR* p_buf);
tBTM_STATUS btm_remove_acl(BD_ADDR bd_addr);

#ifdef __cplusplus
}
#endif

#endif  // RVL_SDK_BTE_H
