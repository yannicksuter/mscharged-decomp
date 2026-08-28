#include "uusb.h"

#include <string.h>

#include <decomp.h>

#include "data_types.h"
#include "gki_target.h"

#include "bte.h"
#include "gki.h"
#include "hci.h"
#include "l2c_int.h"

#define IS_BTE
#include <revolution.h>
#include <revolution/usb/usb.h>
#include <revolution/ipc.h>

#undef NULL
#define NULL ((void*)(0))

#define UUSBDBG(param, ...)               \
    do                                    \
    {                                     \
        if (usb.trace_state)              \
            OSReport(param, __VA_ARGS__); \
    } while (FALSE)

#define UUSBDBG0(param)      \
    do                       \
    {                        \
        if (usb.trace_state) \
            OSReport(param); \
    } while (FALSE)

typedef struct
{
    IPCResult fd;
    char pad0_[0x10 - (0x00 + 0x04)];
    u8 bulkEndpoint1;
    u8 bulkEndpoint2;
    u8 intrEndpoint1;
    char at_0x13;
    int vid;
    int pid;
    UINT8 cmd_buffer_pool;
    UINT8 acl_buffer_pool;
    char pad2_[0x20 - (0x1d + 0x01)];
    tUUSB_CBACK* cback;
    tUUSB* p_uusb_cfg;
    UINT8 trace_state;
    UINT8 state;
    char pad4_[1];
    UINT8 reading_intr_data;
    BUFFER_Q bulk_buffer_q;
    UINT8 at_0x38;
    BUFFER_Q intr_buffer_q;
    UINT8 at_0x48;
    char pad8_[3];
} tUUSB_CB;

static void uusb_ReleaseCmdBufferPool(void);
static void uusb_ReleaseAclBufferPool(void);
static void uusb_CloseDeviceCB(IPCResult result, void* p_data);
static void uusb_ReadIntrDataCB(IPCResult result, void* p_data);
static void uusb_ReadBulkDataCB(IPCResult result, void* p_data);
static void uusb_issue_bulk_read(void);
static void uusb_issue_intr_read(void);
static void uusb_WriteCtrlDataCB(IPCResult result, void* p_data);
static void uusb_WriteBulkDataCB(IPCResult result, void* p_data);
static long uusb_get_devId(int vid, int pid);

static tUUSB_CB usb;
unsigned char __uusb_ppc_stack1[0x1000] __attribute__((aligned(32)));
unsigned char __uusb_ppc_stack2[0x1000] __attribute__((aligned(32)));

UINT32 volatile wait4hci = TRUE;
static long __ntd_ios_file_descriptor = -1l;

static int __ntd_pid;
static int __ntd_vid;
static BOOLEAN __ntd_pid_vid_specified;
static UINT8 uusb_g_usb_devid_found = FALSE;
static UINT8 uusb_g_trace_state_initialized = FALSE;
UINT32 __ntd_ohci = 0;
UINT32 __ntd_ohci_init_flag = FALSE;

void __ntd_set_ohci(UINT8 ohci)
{
    if (ohci == 0)
    {
        __ntd_ohci = 0;
        __ntd_ohci_init_flag = TRUE;
    }
    else if (ohci == 1)
    {
        __ntd_ohci = 1;
        __ntd_ohci_init_flag = TRUE;
    }
}

void __ntd_set_pid_vid(int pid, int vid)
{
    __ntd_vid = vid;
    __ntd_pid = pid;
    __ntd_pid_vid_specified = TRUE;
}

int __ntd_get_usb_file_descriptor(void)
{
    return __ntd_ios_file_descriptor;
}

UINT32 __ntd_get_allocated_mem_size(void)
{
    UINT32 size = 0;

    size += 0x07620;
    size += 0x10000;
    size -= 0x02BA8;

    return size;
}

void uusb_set_trace_state(UINT8 state)
{
    usb.trace_state = state;

    uusb_g_trace_state_initialized = TRUE;
}

UINT8 uusb_get_trace_state(void)
{
    return usb.trace_state;
}

static void uusb_ReleaseCmdBufferPool(void)
{
    GKI_delete_pool(usb.cmd_buffer_pool);

    usb.cmd_buffer_pool = GKI_INVALID_POOL;
}

static void uusb_ReleaseAclBufferPool(void)
{
    GKI_delete_pool(usb.acl_buffer_pool);

    usb.acl_buffer_pool = GKI_INVALID_POOL;
}

static void uusb_CloseDeviceCB(IPCResult result, void* p_data)
{
    usb.bulkEndpoint1 = 0;
    usb.bulkEndpoint2 = 0;
    usb.intrEndpoint1 = 0;
    usb.at_0x13 = 0;

    usb.fd = 0;

    uusb_g_usb_devid_found = 0;

    UUSB_Unregister();

    if (usb.cback)
        (*usb.cback)(4, result);
}

static void uusb_ReadIntrDataCB(IPCResult result, void* p_data)
{
    int sp14 = 0;
    UINT32 len;
    void* p_buffer = NULL;
    HC_BT_HDR* p_buf = p_data;
    UINT8* p;

    UUSBDBG0("HCISU_LOG: uusb_ReadIntrDataCB called\n");
    usb.reading_intr_data = FALSE;

    if (usb.state != 2)
    {
        UUSBDBG0("HCISU_ERR: uusb_ReadIntrDataCB: usb_state != UUSB_PPC_OPENED_ST\n");
        UUSBDBG0("HCISU_ERR: ************\n* uusb_ReadIntrDataCB(): usb_state != UUSB_PPC_OPENED_ST - stop reading\n************\n");
        GKI_freebuf(p_buf);
        p_buf = NULL;
        uusb_ReleaseCmdBufferPool();

        return;
    }

    if (p_buf == NULL)
        goto end;

    if (result <= 0)
    {
        UUSBDBG("HCISU_ERR: uusb_ReadIntrDataCB(): Got Error code %d\n", result);
        GKI_freebuf(p_buf);
        p_buf = NULL;
        goto end;
    }

    p_buffer = GKI_getpoolbuf(GKI_POOL_ID_2);
    if (!p_buffer)
    {
        GKI_freebuf(p_buf);
        p_buf = NULL;
        goto end;
    }

    p_buf->event = 0x1000;
    p_buf->len = result;

    len = sizeof *p_buf + p_buf->len + p_buf->offset;
    len = (len >> 2 << 2) + 4; // round up to 4
    memcpy(p_buffer, p_buf, len);

    UUSBDBG("HCISU_LOG: uusb_ReadIntrDataCB: len(%d) offset (%d) cp_len(%d) data = ",
        ((HC_BT_HDR*)p_buffer)->len,
        ((HC_BT_HDR*)p_buffer)->offset,
        len);
    p = (UINT8*)((HC_BT_HDR*)p_buffer + 1)
      + ((HC_BT_HDR*)p_buffer)->offset;
    for (sp14 = 0; sp14 < result; ++sp14)
    {
        UUSBDBG("%02x ", *p);
        ++p;
    }
    UUSBDBG0("\n");

    OSSwitchFiberEx((unsigned long)p_buffer, 0, 0, 0, &bta_ci_hci_msg_handler, __uusb_ppc_stack1 + sizeof __uusb_ppc_stack1);

    GKI_freebuf(p_buf);
    p_buf = NULL;

end:
    uusb_issue_intr_read();
}

static void uusb_ReadBulkDataCB(IPCResult result, void* p_data)
{
    int sp14 = 0;
    UINT32 len;
    void* p_buffer = NULL;
    HC_BT_HDR* p_buf = p_data;
    UINT8* p;

    if (usb.state != 2)
    {
        UUSBDBG0("HCISU_ERR: uusb_readBulkDataCB: usb_state != UUSB_PPC_OPENED_ST\n");
        UUSBDBG0("HCISU_ERR: ****\n* uusb_readBulkDataCB(): usb_state != UUSB_PPC_OPENED_ST - stop reading\n****\n");
        GKI_freebuf(p_buf);
        p_buf = NULL;
        uusb_ReleaseAclBufferPool();

        return;
    }

    if (result <= 0)
    {
        UUSBDBG("HCISU_ERR: uusb_ReadBulkDataCB(): Got Error code %d\n", result);
        GKI_freebuf(p_buf);
        p_buf = NULL;
        goto end;
    }

    p_buf->len = result;

    p_buffer = GKI_getpoolbuf(GKI_POOL_ID_3);
    if (!p_buffer)
    {
        GKI_freebuf(p_buf);
        p_buf = NULL;
        goto end;
    }

    len = sizeof *p_buf + p_buf->len + p_buf->offset;
    len = (len >> 2 << 2) + 4;
    memcpy(p_buffer, p_buf, len);

    UUSBDBG("HCISU_LOG: uusb_ReadBulkDataCB: len(%d) offset(%d) cp_len(%d) data = ",
        ((HC_BT_HDR*)p_buffer)->len,
        ((HC_BT_HDR*)p_buffer)->offset,
        len);
    p = (UINT8*)((HC_BT_HDR*)p_buffer + 1)
      + ((HC_BT_HDR*)p_buffer)->offset;
    for (sp14 = 0; sp14 < ((HC_BT_HDR*)p_buffer)->len; ++sp14)
    {
        UUSBDBG("%02x ", *p);
        ++p;
    }
    UUSBDBG0("\n");

    p_buffer = l2cap_link_chk_pkt_start((BT_HDR*)p_buffer);

    if (p_buffer != NULL && l2cap_link_chk_pkt_end())
    {
        OSSwitchFiberEx((unsigned long)p_buffer, 0, 0, 0, &bta_ci_hci_msg_handler, __uusb_ppc_stack2 + sizeof __uusb_ppc_stack2);
        p_buffer = 0;
    }

    GKI_freebuf(p_buf);
    p_buf = NULL;

end:
    uusb_issue_bulk_read();
}

static void uusb_issue_bulk_read(void)
{
    HC_BT_HDR* p_buf = NULL;
    void* p_buffer;
    IPCResult ret;

    for (;;)
    {
        p_buf = GKI_getpoolbuf(usb.acl_buffer_pool);
        if (p_buf != NULL)
            break;
        UUSBDBG0("HCISU_ERR: ****\n* uusb_issue_bulk_read(): unable to get buffer - try again\n****\n");
    }

    p_buf->event = 0x1100;
    p_buf->len = 0;

    p_buffer = (void*)OSRoundUp32B((UINT8*)(p_buf + 1));

    p_buf->offset = (UINT8*)p_buffer - (UINT8*)(p_buf + 1);

    ret = IUSB_ReadBlkMsgAsync(usb.fd, usb.bulkEndpoint2, GKI_get_buf_size(p_buf) - 0x28 - p_buf->offset, p_buffer, uusb_ReadBulkDataCB, p_buf);

    if (ret != 0)
    {
        UUSBDBG("HCISU_ERR: uusb_issue_bulk_read: IUSB_ReadBlkMsgAsync failed with err = %d\n", ret);
        UUSBDBG("HCISU_ERR: ****\n* uusb_issue_bulk_read: IUSB_ReadBlkMsgAsync failed with err = %d\n****\n", ret);
        GKI_freebuf(p_buf);
    }
}

static void uusb_issue_intr_read(void)
{
    HC_BT_HDR* p_buf = NULL;
    void* p_buffer = NULL;
    IPCResult ret;

    for (;;)
    {
        p_buf = GKI_getpoolbuf(usb.cmd_buffer_pool);
        if (p_buf != NULL)
            break;
        UUSBDBG0("HCISU_ERR: ****\n* uusb_issue_intr_read(): unable to get buffer - try again\n****\n");
    }

    p_buf->event = 0x1000;
    p_buf->len = 0;

    p_buffer = (void*)OSRoundUp32B((UINT8*)(p_buf + 1));

    p_buf->offset = (UINT8*)p_buffer - (UINT8*)(p_buf + 1);

    ret = IUSB_ReadIntrMsgAsync(usb.fd, usb.intrEndpoint1, GKI_get_buf_size(p_buf) - 0x28 - p_buf->offset, p_buffer, uusb_ReadIntrDataCB, p_buf);

    if (ret != 0)
    {
        UUSBDBG("HCISU_ERR: IUSB_ReadIntrMsgAsync failed with err = %d\n", ret);
        UUSBDBG("HCISU_ERR: ****\n* uusb_issue_intr_read(): IUSB_ReadIntrMsgAsync failed with err = %d\n****\n", ret);
        GKI_freebuf(p_buf);
    }

    usb.reading_intr_data = TRUE;
}

static void uusb_WriteCtrlDataCB(IPCResult result, void* p_data)
{
    HC_BT_HDR* p_buf = NULL;
    void* p_buffer = NULL;
    IPCResult ret = 0;

    UUSBDBG("HCISU_LOG: uusb_WriteCtrlDataCB called with err = %d\n", result);
    if (result < 0)
        UUSBDBG("HCISU_ERR: uusb_WriteCtrlDataCB(): Got error (%d)\n", result);

    if (p_data != NULL)
    {
        GKI_freebuf(p_data);
        p_data = NULL;

        GKI_disable();

        --usb.at_0x48;

        GKI_enable();
    }

    if (usb.at_0x48 >= 5)
        return;

    if (usb.intr_buffer_q.count == 0)
        return;

    p_buf = GKI_dequeue(&usb.intr_buffer_q);
    p_buffer = (UINT8*)(p_buf + 1) + p_buf->offset;

    ret = IUSB_WriteCtrlMsgAsync(usb.fd, 0x20, 0, 0, 0, p_buf->len, p_buffer, uusb_WriteCtrlDataCB, p_buf);

    if (ret < 0)
    {
        UUSBDBG0("HCISU_ERR: uusb_WriteCtrlDataCB(): error - unable to write data packet\n");
        GKI_enqueue_head(&usb.intr_buffer_q, p_buf);
    }
    else
    {
        GKI_disable();

        ++usb.at_0x48;

        GKI_enable();
    }
}

static void uusb_WriteBulkDataCB(IPCResult result, void* p_data)
{
    HC_BT_HDR* p_buf = NULL;
    void* p_buffer = NULL;
    IPCResult ret = 0;

    UUSBDBG("HCISU_LOG: uusb_WriteBulkDataCB called with err = %d\n", result);
    if (result < 0)
        UUSBDBG("HCISU_ERR: uusb_WriteBulkDataCB(): Got error (%d)\n", result);

    if (p_data != NULL)
    {
        GKI_freebuf(p_data);
        p_data = NULL;

        GKI_disable();

        --usb.at_0x38;

        GKI_enable();
    }

    if (usb.at_0x38 >= 5)
        return;

    if (usb.bulk_buffer_q.count == 0)
        return;

    p_buf = GKI_dequeue(&usb.bulk_buffer_q);

    p_buffer = (UINT8*)(p_buf + 1) + p_buf->offset;
    ret = IUSB_WriteBlkMsgAsync(usb.fd, usb.bulkEndpoint1, p_buf->len, p_buffer, uusb_WriteBulkDataCB, p_buf);

    if (ret < 0)
    {
        UUSBDBG0("HCISU_ERR: uusb_WriteBulkDataCB(): error - unable to write data packet\n");
        GKI_enqueue_head(&usb.bulk_buffer_q, p_buf);
    }
    else
    {
        GKI_disable();

        ++usb.at_0x38;

        GKI_enable();
    }
}

static long uusb_get_devId(int vid, int pid)
{
    IPCResult ret;

    if (__ntd_ohci_init_flag == TRUE)
    {
        if (__ntd_ohci == 0)
            ret = IUSB_OpenDeviceIds("oh0", vid, pid, &usb.fd);
        else if (__ntd_ohci == 1)
            ret = IUSB_OpenDeviceIds("oh1", vid, pid, &usb.fd);
    }
    else
    {
        ret = IUSB_OpenDeviceIds("oh1", vid, pid, &usb.fd);
    }

    if (ret < 0)
    {
        UUSBDBG("HCISU_ERR: ERROR from IUSB_OpenDeviceIds (%d) : Device (vendor 0x%x product 0x%x) not found\n",
            ret,
            0x057E,
            0x0305);
        UUSBDBG0("HCISU_ERR: No suitable Bluetooth Device was found. Exiting...\n");
    }
    else
        __ntd_ios_file_descriptor = ret;

    return ret;
}

void UUSB_Register(tUUSB* uusb)
{
    IPCResult ret = 0;

    memset(&usb, 0, sizeof usb);

    GKI_disable();

    usb.state = 5;

    GKI_enable();

    if (!uusb_g_trace_state_initialized)
        uusb_set_trace_state(0);

    ret = IPCCltInit();
    if (ret != 0)
    {
        UUSBDBG0("HCISU_ERR: UUSB_Register: ERROR - failed IPCCltInit\n");
        return;
    }

    ret = IUSB_OpenLib();
    if (ret != 0)
    {
        UUSBDBG0("HCISU_ERR: UUSB_Register: ERROR - failed IUSB_OpenLib\n");
        return;
    }

    usb.p_uusb_cfg = uusb;

    if (__ntd_pid_vid_specified == TRUE)
    {
        usb.vid = __ntd_vid;
        usb.pid = __ntd_pid;
    }
    else
    {
        usb.vid = 0x057E; // 1406 - Nintendo Co., Ltd.
        usb.pid = 0x0305; //  773 - Broadcom BCM2045A Bluetooth Radio [Nintendo Wii/Wii U]
    }

    usb.bulkEndpoint1 = 0;
    usb.bulkEndpoint2 = 0;
    usb.intrEndpoint1 = 0;
    usb.at_0x13 = 0;

    if (uusb_get_devId(usb.vid, usb.pid) < 0)
    {
        UUSBDBG0("HCISU_ERR: UUSB_Register: Error returned from get_devId()\n");
        return;
    }

    UUSBDBG("HCISU_LOG: UUSB_Register: uusb_get_dev_id returned with usb.devId = %d\n",
        usb.fd);

    usb.bulkEndpoint1 = 2;
    usb.bulkEndpoint2 = 130;
    usb.intrEndpoint1 = 129;
    usb.at_0x13 = 0;

    GKI_init_q(&usb.bulk_buffer_q);
    usb.at_0x38 = 0;

    GKI_init_q(&usb.intr_buffer_q);
    usb.at_0x48 = 0;

    usb.cmd_buffer_pool = GKI_create_pool(660, 45, 1, NULL);
    usb.acl_buffer_pool = GKI_create_pool(1800, 30, 1, NULL);

    if (usb.cmd_buffer_pool == GKI_INVALID_POOL
        || usb.acl_buffer_pool == GKI_INVALID_POOL)
    {
        UUSBDBG0("HCISU_LOG: UUSB_Register: Unable to create memory pools\n");
        return;
    }

    UUSBDBG("HCISU_LOG: UUSB_Register: hci_cmd_usb_pool_id (%d) hci_acl_usb_pool_id(%d)\n",
        usb.cmd_buffer_pool,
        usb.acl_buffer_pool);

    GKI_disable();

    usb.state = 4;

    GKI_enable();

    wait4hci = TRUE;
}

void UUSB_Open(tUUSB* uusb, tUUSB_CBACK* cback)
{
    if (usb.state != 4)
    {
        UUSBDBG0("HCISU_LOG: UUSB_Open: USB is not in registered state, unable to open\n");
        return;
    }

    if (usb.cmd_buffer_pool == GKI_INVALID_POOL
        || usb.acl_buffer_pool == GKI_INVALID_POOL)
    {
        UUSBDBG0("HCISU_LOG: UUSB_Open: USB Buffer Pool's Invalid\n");
        UUSBDBG("HCISU_LOG: UUSB_Open: hci_cmd_usb_pool_id = %d\n",
            usb.cmd_buffer_pool);
        UUSBDBG("HCISU_LOG: UUSB_Open: hci_acl_usb_pool_id = %d\n",
            usb.acl_buffer_pool);
        return;
    }

    GKI_disable();

    usb.cback = cback;
    usb.state = 2;

    GKI_enable();

    uusb_issue_intr_read();
    uusb_issue_bulk_read();

    wait4hci = FALSE;

    (void)uusb;
}

UINT16 UUSB_Read(UINT8 param_1, void* param_2, UINT16 param_3)
{
    return 0;
}

UINT16 UUSB_Write(UINT8 param_1, void* p_data, UINT16 len, void* param_4)
{
    UINT16 sp18 = 0;
    IPCResult ret = 0;
    void* p_buffer = NULL;
    HC_BT_HDR* p_buf = NULL;

    if (usb.state != 2)
    {
        UUSBDBG0("HCISU_ERR: UUSB_Write: usb_state != UUSB_PPC_OPENED_ST\n");
        return 0;
    }

    UUSBDBG("HCISU_LOG: buffer size = %d\n",
        GKI_get_buf_size(param_4) - sizeof(HC_BT_HDR));
    UUSBDBG("HCISU_LOG: UUSB_Write: ep(%d) len(%d) data = ", param_1, len);
    for (sp18 = 0; sp18 < len; ++sp18)
        UUSBDBG("%02x ", *((UINT8*)p_data + sp18));
    UUSBDBG0("\n");

    switch (param_1)
    {
    case 0:
        p_buf = GKI_getpoolbuf(usb.cmd_buffer_pool);

        if (p_buf == NULL)
        {
            UUSBDBG0("HCISU_ERR: UUSB_Write: Unable to get buffer from hci_cmd_usb_pool\n");
            return 0;
        }

        p_buf->len = len;

        p_buffer = (void*)OSRoundUp32B((UINT8*)(p_buf + 1));

        p_buf->offset = (UINT8*)p_buffer - (UINT8*)(p_buf + 1);

        memcpy(p_buffer, p_data, len);

        if (usb.at_0x48 < 5 && usb.intr_buffer_q.count == 0)
        {
            ret = IUSB_WriteCtrlMsgAsync(usb.fd, 0x20, 0, 0, 0, len, p_buffer, uusb_WriteCtrlDataCB, p_buf);
        }
        else
        {
            GKI_enqueue(&usb.intr_buffer_q, p_buf);
            UUSBDBG("HCISU_ERR: UUSB_WRITE(): Ctrl Queue count =  %d\n",
                usb.intr_buffer_q.count);

            return 0;
        }

        if (ret != 0)
        {
            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");
            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");
            UUSBDBG("HCISU_ERR: * UUSB_Write(): IUSB_WriteCtrlMsgAsync error...  (%d), writes outstanding (%d)\n",
                ret,
                usb.at_0x48);
            if (ret != -22)
                UUSBDBG("HCISU_ERR: * UUSB_Write(): IUSB_WriteCtrlMsgAsync error... IOS_ERROR_FAIL_ALLOC  (%d), writes outstanding (%d)\n",
                    ret,
                    usb.at_0x38);

            if (ret == -8)
                UUSBDBG("HCISU_ERR: * UUSB_Write(): IUSB_WriteCtrlMsgAsync error... IOS_ERROR_QFULL  (%d), writes outstanding (%d)\n",
                    ret,
                    usb.at_0x38);

            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");
            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");

            GKI_freebuf(p_buf);
            p_buf = NULL;
        }
        else
        {
            GKI_disable();

            ++usb.at_0x48;

            GKI_enable();
        }

        break;

    case 2:
        p_buf = GKI_getpoolbuf(usb.acl_buffer_pool);

        if (p_buf == NULL)
        {
            UUSBDBG0("HCISU_ERR: UUSB_Write: Unable to get buffer from hci_acl_usb_pool\n");
            return 0;
        }

        p_buf->len = len;

        p_buffer = (void*)OSRoundUp32B((UINT8*)(p_buf + 1));

        p_buf->offset = (UINT8*)p_buffer - (UINT8*)(p_buf + 1);

        memcpy(p_buffer, p_data, len);

        if (len > 190)
            UUSBDBG0("HCISU_ERR: UUSB_Write - writing mem data\n");

        if (usb.at_0x38 < 5 && usb.bulk_buffer_q.count == 0)
        {
            ret = IUSB_WriteBlkMsgAsync(usb.fd, usb.bulkEndpoint1, len, p_buffer, uusb_WriteBulkDataCB, p_buf);
        }
        else
        {
            GKI_enqueue(&usb.bulk_buffer_q, p_buf);
            UUSBDBG("HCISU_ERR: UUSB_WRITE(): Bulk Queue count =  %d\n",
                usb.bulk_buffer_q.count);

            return 0;
        }

        if (ret != 0)
        {
            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");
            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");
            UUSBDBG("HCISU_ERR: * UUSB_Write(): IUSB_WriteBlkMsgAsync error...  (%d), writes outstanding (%d)\n",
                ret,
                usb.at_0x38);
            if (ret != -22)
                UUSBDBG("HCISU_ERR: * UUSB_Write(): IUSB_WriteBlkMsgAsync error... IOS_ERROR_FAIL_ALLOC  (%d), writes outstanding (%d)\n",
                    ret,
                    usb.at_0x38);

            if (ret == -8)
                UUSBDBG("HCISU_ERR: * UUSB_Write(): IUSB_WriteBlkMsgAsync error... IOS_ERROR_QFULL  (%d), writes outstanding (%d)\n",
                    ret,
                    usb.at_0x38);

            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");
            UUSBDBG0("HCISU_ERR: *************************************************************************************\n");

            GKI_freebuf(p_buf);
            p_buf = NULL;
        }
        else
        {
            GKI_disable();

            ++usb.at_0x38;

            GKI_enable();
        }

        break;

    case 3:
        break;
    }

    (void)param_4;

    return ret;
}

void UUSB_Close(void)
{
    GKI_disable();

    usb.state = 0;

    GKI_enable();

    if (usb.bulk_buffer_q.count)
    {
        while (usb.bulk_buffer_q.count)
            GKI_freebuf(GKI_dequeue(&usb.bulk_buffer_q));
    }

    if (usb.intr_buffer_q.count)
    {
        while (usb.intr_buffer_q.count)
            GKI_freebuf(GKI_dequeue(&usb.intr_buffer_q));
    }

    IUSB_CloseDeviceAsync(usb.fd, uusb_CloseDeviceCB, NULL);
}

void UUSB_Unregister(void)
{
    IUSB_CloseLib();

    GKI_disable();

    usb.state = 5;
    usb.trace_state = 0;

    uusb_g_trace_state_initialized = FALSE;

    GKI_enable();
}
