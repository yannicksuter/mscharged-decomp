#include <dwc/dwc_transport.h>

#include <dwc/dwc_main.h>
#include <dwc/dwc_report.h>
#include <dwc/dwci_error.h>
#include <gamespy/gt2/gt2.h>
#include <revolution/os/OSTime.h>
#include <string.h>

typedef struct DWCTransportEntryView
{
    const void* _00;
    void* recvBuffer;
    int recvBufferSize;
    int _0C;
    int _10;
    int _14;
    int _18;
    s8 _1C;
    u8 recvState;
    s8 _1E;
    u8 _1F[3];
    u16 _22;
    u8 _24[4];
    u64 _28;
    int _30;
    u8 _34[4];
} DWCTransportEntryView;

typedef struct DWCTransportControlView
{
    DWCTransportEntryView entries[32];
    DWCUserSendCallback sendCallback;
    DWCUserRecvCallback recvCallback;
    void* recvTimeoutCallback;
    DWCUserPingCallback pingCallback;
    u16 maxSplit;
    u8 _712[6];
} DWCTransportControlView;

static DWCTransportControlView* lbl_806E2F00;

GT2Connection DWCi_GetGT2Connection(u8 aid);
u8 DWCi_GetConnectionAid(GT2Connection connection);
BOOL DWC_isValidAid(u8 aid);
void fn_8049B674(GT2Connection connection, GT2Byte* message, int len);

typedef struct DWCTransportHeaderView
{
    u32 size;
    u16 type;
    char magic[2];
} DWCTransportHeaderView;

BOOL fn_8049AE0C(int type, u8 aid, const void* buffer, int size)
{
    DWCTransportEntryView* entry = &lbl_806E2F00->entries[aid];
    DWCTransportHeaderView header;
    int freeSpace;
    BOOL sendable;

    if (DWCi_IsError()
        || (type == 1 && !DWC_IsValidAID(aid))
        || !DWC_isValidAid(aid))
    {
        DWC_Printf(8, "aid %d is unavailable.\n", aid);
        sendable = FALSE;
    }
    else if (entry->_1C == 1)
    {
        DWC_Printf(0x8000,
            "+++ Cannot send to %d from %d (busy)\n",
            aid,
            DWC_GetMyAID());
        sendable = FALSE;
    }
    else
    {
        freeSpace = gt2GetOutgoingBufferFreeSpace(DWCi_GetGT2Connection(aid)) - 519;
        freeSpace = freeSpace > 0 ? freeSpace : 0;
        if (freeSpace < (type < 5 && type >= 2 ? 12 : 8))
        {
            DWC_Printf(0x8000,
                "+++ Cannot send to %d from %d (outgoing buffer is not enough) %d < %d\n",
                aid,
                DWC_GetMyAID(),
                freeSpace,
                type < 5 && type >= 2 ? 12 : 8);
            sendable = FALSE;
        }
        else
        {
            sendable = TRUE;
        }
    }

    if (!sendable)
    {
        return FALSE;
    }

    entry->_1C = 1;
    entry->_00 = buffer;
    entry->_0C = 0;
    entry->_14 = size;

    strncpy(header.magic, "DT", 2);
    header.size = (((u32)size & 0xFF000000) >> 24)
                | (((u32)size & 0x00FF0000) >> 8)
                | (((u32)size & 0x0000FF00) << 8)
                | (((u32)size & 0x000000FF) << 24);
    header.type = (u16)(((u16)type >> 8) | ((u16)type << 8));
    gt2Send(DWCi_GetGT2Connection(aid),
        (const GT2Byte*)&header,
        sizeof(header),
        GT2True);

    if (size > lbl_806E2F00->maxSplit)
    {
        size = lbl_806E2F00->maxSplit;
    }
    gt2Send(DWCi_GetGT2Connection(aid), buffer, size, GT2True);
    entry->_0C += size;

    size = entry->_14;
    if (entry->_0C == size)
    {
        entry->_1C = 0;
        entry->_00 = NULL;
        entry->_0C = 0;
        entry->_14 = 0;
        if (lbl_806E2F00->sendCallback != NULL && type == 1)
        {
            lbl_806E2F00->sendCallback(size, aid);
        }
    }
    return TRUE;
}

BOOL DWC_SendUnreliable(u8 aid, const void* buffer, int size)
{
    if (DWCi_IsError())
    {
        return FALSE;
    }

    if (!DWC_IsValidAID(aid))
    {
        DWC_Printf(8, "aid %d is now unavailable.\n", aid);
        return FALSE;
    }

    if (size > lbl_806E2F00->maxSplit)
    {
        DWC_Printf(0x8000,
            "+++ SendUnreliable size is too large ( %d > %d ) \n",
            size,
            lbl_806E2F00->maxSplit);
        return FALSE;
    }

    gt2Send(DWCi_GetGT2Connection(aid), buffer, size, GT2False);
    if (lbl_806E2F00->sendCallback != NULL)
    {
        lbl_806E2F00->sendCallback(size, aid);
    }
    return TRUE;
}

BOOL DWC_SetRecvBuffer(u8 aid, void* buffer, int size)
{
    DWCTransportEntryView* entry = &lbl_806E2F00->entries[aid];
    int recvState = entry->recvState;

    if (recvState == 2)
    {
        DWC_Printf(0x10000, "+++ Cannot set recv buffer\n");
        return FALSE;
    }

    entry->recvBuffer = buffer;
    entry->recvBufferSize = size;
    entry->recvState = 1;
    entry->_10 = 0;
    entry->_18 = 0;
    return TRUE;
}

BOOL DWC_SetUserRecvCallback(DWCUserRecvCallback callback)
{
    if (lbl_806E2F00 == NULL)
    {
        return FALSE;
    }
    lbl_806E2F00->recvCallback = callback;
    return TRUE;
}

void DWCi_InitTransport(void* control)
{
    lbl_806E2F00 = control;
    memset(lbl_806E2F00, 0, sizeof(DWCTransportControlView));
    lbl_806E2F00->maxSplit = 1465;
}

void fn_8049B244(GT2Connection connection, GT2Byte* message, int len,
    GT2Bool reliable)
{
    DWCTransportEntryView* entry;
    u8 aid;

    if (lbl_806E2F00 == NULL)
    {
        return;
    }
    if (message == NULL || len == 0)
    {
        DWC_Printf(8, "Recv NULL message %x, size = %d\n", message, len);
        return;
    }
    if (reliable)
    {
        fn_8049B674(connection, message, len);
        return;
    }

    aid = DWCi_GetConnectionAid(connection);
    entry = &lbl_806E2F00->entries[aid];
    if (entry->recvBuffer != NULL && entry->recvBufferSize >= len)
    {
        memcpy(entry->recvBuffer, message, len);
    }
    else
    {
        DWC_Printf(0x10000,
            "Recv data size is too large (%d > %d)\n",
            len,
            entry->recvBufferSize);
        return;
    }

    if (lbl_806E2F00->recvCallback != NULL)
    {
        lbl_806E2F00->recvCallback(aid, entry->recvBuffer, len);
    }
    if (lbl_806E2F00->recvTimeoutCallback != NULL && entry->_30 != 0)
    {
        entry->_28 = OSGetTime();
    }
}

void fn_8049B380(GT2Connection connection, int latency)
{
    if (lbl_806E2F00->pingCallback != NULL)
    {
        u8 aid = DWCi_GetConnectionAid(connection);
        lbl_806E2F00->pingCallback(latency, aid);
    }
}

void fn_8049B5EC(u8 aid)
{
    if (lbl_806E2F00 == NULL)
    {
        return;
    }
    lbl_806E2F00->entries[aid]._0C = 0;
    lbl_806E2F00->entries[aid]._10 = 0;
    lbl_806E2F00->entries[aid]._14 = 0;
    lbl_806E2F00->entries[aid]._18 = 0;
    lbl_806E2F00->entries[aid]._1C = 0;
    if (lbl_806E2F00->entries[aid].recvState)
    {
        lbl_806E2F00->entries[aid].recvState = 1;
    }
    lbl_806E2F00->entries[aid]._22 = 0;
}

void fn_8049B668(void)
{
    lbl_806E2F00 = NULL;
}
