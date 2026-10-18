#include <dwc/dwc_transport.h>

#include <dwc/dwc_error.h>
#include <dwc/dwc_main.h>
#include <dwc/R4QK01/dwc_match_internal.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_report.h>
#include <dwc/dwc_reliable.h>
#include <dwc/dwci_error.h>
#include <gamespy/gt2/gt2.h>
#include <string.h>

static void DWCs_HandleUnreliableMessage(GT2Connection connection, u8* message, int size);
static BOOL DWCs_SendUnreliableBitmapForwardToServer(u32 bitmap, const void* buffer, int size);

#define DWC_FORWARD_MAGIC "F\xfcW"

typedef struct DWCstTransportForwardHeader
{
    char magic[3];
    u8 srcAid;
    u32 aidBitmap;
} DWCTransportForwardHeader;

enum
{
    DWC_TRANSPORT_KEEPALIVE_DEFAULT = 20000,
    DWC_TRANSPORT_ERROR_QUEUE = -630,
    DWC_TRANSPORT_ERROR_CONNECTION = -640,
    DWC_TRANSPORT_UNRELIABLE_MAX = 1462,
    DWC_TRANSPORT_SYN_BODY_SIZE = 28,
    DWC_TRANSPORT_SYN_ACK_BODY_SIZE = 4,
    DWC_TRANSPORT_ACK_HEADER_SIZE = 8,
    DWC_TRANSPORT_NODE_WIRE_SIZE = 24
};

static DWCTransportInfo* sTransInfo;

static DWCTransportConnection* DWCs_GetTransConnection(u8 aid);
static int DWCs_GetSendState(u8 aid);
static int DWCs_GetRecvState(u8 aid);
static void DWCs_Send(
    u8 aid, const u8* buffer, int size, BOOL reliable);
static void DWCs_EncodeHeader(
    DWCTransportHeader* header, u16 type, int size);
static u16 DWCs_DecodeHeader(const DWCTransportHeader* message);
static s32 DWCs_GetRequiredHeaderSize(u16 type);
static int DWCs_GetOutgoingBufferFreeSize(u8 aid);
static void DWCs_HandleReliableMessage(
    GT2Connection connection, u8* message, int size);
static void DWCs_RecvDataHeader(
    u8 aid, DWCTransportHeader* message, int size);
static void DWCs_RecvDataBody(u8 aid, void* message, int size);
static void DWCs_RecvSystemDataBody(u8 aid, void* message, int size);

static DWCTransportConnection* DWCs_GetTransConnection(u8 aid)
{
    return &sTransInfo->connections[aid];
}

static int DWCs_GetSendState(u8 aid)
{
    return sTransInfo->connections[aid].sendState;
}

static int DWCs_GetRecvState(u8 aid)
{
    return sTransInfo->connections[aid].recvState;
}

static void DWCs_EncodeHeader(
    DWCTransportHeader* header, u16 type, int size)
{
    strncpy(header->magicStrings, DWC_MAGIC_STRINGS, DWC_MAGIC_STRINGS_LEN);

    header->type = DWCi_HtoLEs(type);
    header->size = (int)DWCi_HtoLEl((u32)size);
}

static u16 DWCs_DecodeHeader(const DWCTransportHeader* message)
{
    DWCTransportHeader header;

    DWCi_Np_CpuCopy8(message, &header, sizeof(DWCTransportHeader));

    if (memcmp(header.magicStrings, DWC_MAGIC_STRINGS,
            DWC_MAGIC_STRINGS_LEN)
        == 0)
    {
        return DWCi_LEtoHs(header.type);
    }

    return DWC_SEND_TYPE_INVALID;
}

static s32 DWCs_GetRequiredHeaderSize(u16 type)
{
    s32 size;
    s32 count;

    switch (type)
    {
    case DWC_SEND_TYPE_MATCH_SYN:
        size = sizeof(DWCTransportHeader) + DWC_TRANSPORT_SYN_BODY_SIZE;
        break;
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        size = sizeof(DWCTransportHeader) + DWC_TRANSPORT_SYN_ACK_BODY_SIZE;
        break;
    case DWC_SEND_TYPE_MATCH_ACK:
        count = DWCi_GetNodeCount() - 1;
        if (count == 0)
            size = sizeof(DWCTransportHeader) + DWC_TRANSPORT_ACK_HEADER_SIZE;
        else
            size = sizeof(DWCTransportHeader) + DWC_TRANSPORT_ACK_HEADER_SIZE
                + count * DWC_TRANSPORT_NODE_WIRE_SIZE;
        break;
    default:
        size = sizeof(DWCTransportHeader);
        break;
    }
    return size;
}

static int DWCs_GetOutgoingBufferFreeSize(u8 aid)
{
    static const int gamespyUseSize = 512;
    GT2Connection connection = DWCi_GetGT2Connection(aid);
    int free;

    free = gt2GetOutgoingBufferFreeSpace(connection)
        - DWC_TRANSPORT_GT2HEADER_SIZE - gamespyUseSize;

    return free > 0 ? free : 0;
}

BOOL DWCi_IsSendableReliable(u8 aid, u16 type)
{
    s32 freeSpace;

    if (DWCi_IsError()
        || (type == DWC_SEND_TYPE_USERDATA && !DWC_IsValidAID(aid)))
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "aid %d is unavailable.\n", aid);
        return FALSE;
    }
    if (!DWCi_IsValidAID(aid))
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING,
            "aid %d doesn't have GT2 connection.\n", aid);
        return FALSE;
    }
    if (DWCs_GetSendState(aid) == DWC_TRANSPORT_SEND_BUSY)
    {
        DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
            "+++ Cannot send to %d from %d (busy)\n", aid, DWC_GetMyAID());
        return FALSE;
    }
    freeSpace = DWCs_GetOutgoingBufferFreeSize(aid);
    if (freeSpace < DWCs_GetRequiredHeaderSize(type))
    {
        DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
            "+++ Cannot send to %d from %d (outgoing buffer is not enough) %d < %d\n",
            aid, DWC_GetMyAID(), freeSpace, DWCs_GetRequiredHeaderSize(type));
        return FALSE;
    }
    if (!DWCi_IsReliableQueueFree())
        return FALSE;
    return TRUE;
}

static void DWCs_Send(u8 aid, const u8* buffer, int size, BOOL reliable)
{
    GT2Connection connection = DWCi_GetGT2Connection(aid);
    GT2Result result;

    if (reliable)
    {
        if (DWCi_ProcessReliableQueue())
        {
            result = gt2Send(connection, buffer, size, GT2True);
            if (result == GT2Success)
                return;
            DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
                "gt2Send() Failed code:%d\n", result);
        }
        if (!DWCi_InsertReliableQueue(connection, buffer, size))
            DWCi_SetError(DWC_ERROR_NETWORK,
                DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_TRANSPORT_ERROR_QUEUE);
    }
    else
    {
        gt2Send(connection, buffer, size, GT2False);
    }
}

BOOL DWCi_SendReliable(u16 type, u8 aid, const void* buffer, int size)
{
    DWCTransportConnection* transConnection = DWCs_GetTransConnection(aid);
    DWCTransportHeader header;
    int sendSize;
    int freeSpace;

    if (!DWCi_IsSendableReliable(aid, type))
    {
        return FALSE;
    }

    transConnection->sendState = DWC_TRANSPORT_SEND_BUSY;
    transConnection->sendBuffer = buffer;
    transConnection->sendingSize = 0;
    transConnection->requestSendSize = size;

    DWCs_EncodeHeader(&header, type, size);
    DWCs_Send(aid, (const u8*)&header, sizeof(DWCTransportHeader), TRUE);

    if (size > sTransInfo->sendSplitMax)
    {
        sendSize = sTransInfo->sendSplitMax;
    }
    else
    {
        sendSize = size;
    }

    freeSpace = DWCs_GetOutgoingBufferFreeSize(aid);
    if (sendSize > freeSpace)
        return TRUE;

    DWCs_Send(aid, buffer, sendSize, TRUE);

    transConnection->sendingSize += sendSize;

    if (transConnection->sendingSize == transConnection->requestSendSize)
    {
        int reqSendSize = transConnection->requestSendSize;

        transConnection->sendState = DWC_TRANSPORT_SEND_READY;
        transConnection->sendBuffer = NULL;
        transConnection->sendingSize = 0;
        transConnection->requestSendSize = 0;

        if (sTransInfo->sendCallback && type == DWC_SEND_TYPE_USERDATA)
        {
            sTransInfo->sendCallback(reqSendSize, aid, sTransInfo->sendParam);
        }
    }

    return TRUE;
}

BOOL DWC_SendUnreliable(u8 aid, const void* buffer, int size)
{
    if (DWCi_IsError())
        return FALSE;
    if (!DWC_IsValidAID(aid))
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "aid %d is now unavailable.\n", aid);
        return FALSE;
    }
    if (size > DWC_TRANSPORT_UNRELIABLE_MAX)
    {
        DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
            "+++ SendUnreliable size is too large ( %d > %d )\n",
            size, DWC_TRANSPORT_UNRELIABLE_MAX);
        return FALSE;
    }
    DWCi_SendUnreliable(DWC_GetMyAID(), aid, buffer, size, TRUE);
    if (sTransInfo->sendCallback)
        sTransInfo->sendCallback(size, aid, sTransInfo->sendParam);
    return TRUE;
}

u32 DWC_SendUnreliableBitmap(u32 bitmap, const void* buffer, int size)
{
    u32 connected = 0;
    u32 direct;
    u32 forward;
    u8 aid;

    for (aid = 0; aid < DWC_MAX_CONNECTIONS; aid++)
    {
        if (DWCi_GetGT2Connection(aid))
            connected |= 1 << aid;
    }
    direct = bitmap & connected & ~(1 << DWC_GetMyAID());
    forward = bitmap & ~connected & ~(1 << DWC_GetMyAID());
    for (aid = 0; aid < DWC_MAX_CONNECTIONS; aid++)
    {
        u32 mask = 1 << aid;
        if ((direct & mask) && !DWC_SendUnreliable(aid, buffer, size))
            bitmap &= ~mask;
    }
    if (forward && !DWCs_SendUnreliableBitmapForwardToServer(forward, buffer, size))
        bitmap &= ~forward;
    return bitmap;
}

BOOL DWC_SetRecvBuffer(u8 aid, void* buffer, int size)
{
    DWCTransportConnection* entry = &sTransInfo->connections[aid];
    int recvState = entry->recvState;

    if (recvState == DWC_TRANSPORT_RECV_BODY)
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "+++ Cannot set recv buffer\n");
        return FALSE;
    }

    entry->recvBuffer = buffer;
    entry->recvBufferSize = size;
    entry->recvState = DWC_TRANSPORT_RECV_HEADER;
    entry->recvingSize = 0;
    entry->requestRecvSize = 0;
    return TRUE;
}

BOOL DWC_Ping(u8 aid)
{
    GT2Connection connection;

    if (DWCi_IsError())
    {
        return FALSE;
    }

    connection = DWCi_GetGT2Connection(aid);

    if (aid == DWC_GetMyAID() || !connection
        || gt2GetConnectionState(connection) != GT2Connected)
    {
        DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
            "DWC_Ping:not connected yet:%d\n", aid);
        return FALSE;
    }

    gt2Ping(connection);

    return TRUE;
}

BOOL DWC_SetUserRecvCallback(DWCUserRecvCallback callback, void* param)
{
    if (sTransInfo == NULL)
    {
        return FALSE;
    }
    sTransInfo->recvCallback = callback;
    sTransInfo->recvParam = param;
    return TRUE;
}

BOOL DWC_SetConnectionKeepAliveTime(u32 time)
{
    if (!sTransInfo || !time)
        return FALSE;
    sTransInfo->packetRecvTimeout = time;
    return TRUE;
}

u32 DWC_GetConnectionKeepAliveTime(void)
{
    if (!sTransInfo)
        return 0;
    return sTransInfo->packetRecvTimeout;
}

void DWCi_InitTransport(DWCTransportInfo* info)
{
    sTransInfo = info;
    memset(sTransInfo, 0, sizeof(DWCTransportInfo));
    sTransInfo->sendSplitMax = DWC_TRANSPORT_SEND_MAX;
    DWC_SetConnectionKeepAliveTime(DWC_TRANSPORT_KEEPALIVE_DEFAULT);
    DWCi_InitReliableQueue();
}

void DWCi_RecvCallback(GT2Connection connection, GT2Byte* message, int len,
    GT2Bool reliable)
{
    u8 aid;

    if (!sTransInfo)
        return;
    if (!message || !len)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING,
            "Recv NULL message %x, size = %d\n", message, len);
        return;
    }
    aid = DWCi_GetConnectionAID(connection);
    if (aid < DWC_MAX_CONNECTIONS && aid != DWC_GetMyAID())
        DWCi_ResetLastSomeDataRecvTimeByAid(aid);
    if (reliable)
        DWCs_HandleReliableMessage(connection, message, len);
    else
        DWCs_HandleUnreliableMessage(connection, message, len);
}

void DWCi_PingCallback(GT2Connection connection, int latency)
{
    if (sTransInfo->pingCallback != NULL)
    {
        u8 aid = DWCi_GetConnectionAID(connection);
        sTransInfo->pingCallback(latency, aid, sTransInfo->pingParam);
    }
}

void DWCi_TransportProcess(void)
{
    u8* aidList;
    s32 hostCount;
    s32 i;
    DWCTick currentTick;
    u32 keepAliveTime;

    if (!sTransInfo)
        return;

    hostCount = DWC_GetAIDList(&aidList);
    currentTick = DWCi_Np_GetTick();
    keepAliveTime = DWC_GetConnectionKeepAliveTime();
    if (DWC_IsServerMyself() || (DWCi_IsServerChanging() && DWCi_IsBackupMine()))
    {
        for (i = 0; i < hostCount; i++)
        {
            u8 aid = aidList[i];
            if (aid != DWC_GetMyAID())
            {
                u32 time = (u32)DWCi_Np_TicksToMilliSeconds(
                    currentTick - DWCs_GetTransConnection(aid)->lastPacketRecvTick);
                if (time > keepAliveTime)
                {
                    // The retail diagnostic passes the last receive tick as its final argument.
                    DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
                        "shutdown client: DWCi_TransportProcess:timeout aid=%d,time=%d[ms],timeout time=%d[ms]\n",
                        aid, time, DWCs_GetTransConnection(aid)->lastPacketRecvTick);
                    DWCi_CloseConnectionHard(aid);
                    DWCi_SendServerPolling(TRUE);
                }
            }
        }
    }
    else
    {
        u8 aid = DWCi_GetServerOrSubServerAID();
        if (aid != 0xff && aid != DWC_GetMyAID())
        {
            DWCTick lastRecvTick = DWCs_GetTransConnection(aid)->lastPacketRecvTick;
            u32 time = (u32)DWCi_Np_TicksToMilliSeconds(currentTick - lastRecvTick);
            if (time > keepAliveTime)
            {
                // The retail diagnostic passes the last receive tick as its final argument.
                DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
                    "shutdown server: DWCi_TransportProcess:timeout aid=%d,time=%d[ms],timeout time=%d[ms]\n",
                    aid, time, lastRecvTick);
                DWCi_CloseConnectionHard(aid);
            }
        }
    }

    for (i = 0; i < hostCount; i++)
    {
        u8 aid = aidList[i];
        DWCi_ProcessReliableQueue();

        if (DWC_IsValidAID(aid))
        {
            DWCTransportConnection* transConnection;
            DWCTick recvTick;
            recvTick = DWCi_Np_GetTick();
            transConnection = DWCs_GetTransConnection(aid);
            if (sTransInfo->recvTimeoutCallback
                && transConnection->recvTimeoutTime > 0)
            {
                u32 time = (u32)DWCi_Np_TicksToMilliSeconds(
                    recvTick - transConnection->previousRecvTick);
                if (time > transConnection->recvTimeoutTime)
                {
                    DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
                        "DWCi_TransportProcess:timeout aid=%d,time=%d[ms],timeout time=%d[ms]\n",
                        aid, time, transConnection->recvTimeoutTime);
                    sTransInfo->recvTimeoutCallback(aid, sTransInfo->recvTimeoutParam);
                    transConnection->previousRecvTick = recvTick;
                }
            }
        }

        if (aid != DWC_GetMyAID()
            && DWCs_GetSendState(aid) == DWC_TRANSPORT_SEND_BUSY)
        {
            s32 restSize;
            s32 sendSize;
            s32 freeSpace;
            DWCTransportConnection* transConnection;

            transConnection = DWCs_GetTransConnection(aid);
            restSize = transConnection->requestSendSize - transConnection->sendingSize;
            if (restSize > sTransInfo->sendSplitMax)
                sendSize = sTransInfo->sendSplitMax;
            else
                sendSize = restSize;

            freeSpace = DWCs_GetOutgoingBufferFreeSize(aid);
            if (freeSpace < sendSize)
            {
                DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
                    "DWCi_TransportProcess:freeSpace < sendSize:aid:%d, %d < %d\n",
                    aid, freeSpace, sendSize);
                continue;
            }

            DWCs_Send(aid, transConnection->sendBuffer + transConnection->sendingSize,
                sendSize, TRUE);
            transConnection->sendingSize += sendSize;
            if (transConnection->sendingSize == transConnection->requestSendSize)
            {
                int reqSendSize = transConnection->requestSendSize;
                transConnection->sendState = DWC_TRANSPORT_SEND_READY;
                transConnection->sendBuffer = NULL;
                transConnection->sendingSize = 0;
                transConnection->requestSendSize = 0;
                if (sTransInfo->sendCallback)
                    sTransInfo->sendCallback(reqSendSize, aid, sTransInfo->sendParam);
            }
        }
    }
}

void DWCi_ClearTransConnection(u8 aid)
{
    if (sTransInfo == NULL)
        return;
    sTransInfo->connections[aid].sendingSize = 0;
    sTransInfo->connections[aid].recvingSize = 0;
    sTransInfo->connections[aid].requestSendSize = 0;
    sTransInfo->connections[aid].requestRecvSize = 0;
    sTransInfo->connections[aid].sendState = DWC_TRANSPORT_SEND_READY;
    sTransInfo->connections[aid].recvBuffer = NULL;
    sTransInfo->connections[aid].recvBufferSize = 0;
    sTransInfo->connections[aid].recvState = DWC_TRANSPORT_RECV_NOBUF;
    sTransInfo->connections[aid].lastRecvType = DWC_SEND_TYPE_INVALID;
}

void DWCi_ShutdownTransport(void)
{
    sTransInfo = NULL;
    DWCi_CleanupReliableQueue();
}

void DWCi_ResetLastSomeDataRecvTimeByAid(u8 aid)
{
    sTransInfo->connections[aid].lastPacketRecvTick = DWCi_Np_GetTick();
}

void DWCi_SendUnreliable(u8 srcAid, u8 dstAid, const void* buffer, int size, BOOL forward)
{
    GT2Connection connection;
    BOOL relay = FALSE;

    connection = DWCi_GetGT2Connection(dstAid);
    if (!connection)
    {
        if (!DWC_IsServerMyself())
        {
            DWCNodeInfo* server = DWCi_GetNodeByIndex(0);
            connection = DWCi_GetGT2Connection(server->aid);
            if (!connection)
            {
                DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
                    "+++ drop relay unreliable data. (aid %d->%d)\n", srcAid, dstAid);
                DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
                    "+++ sv gt2Connection is NULL.(sv aid:%d myaid:%d)\n",
                    server->aid, DWC_GetMyAID());
                return;
            }
            DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
                "+++ forward unreliable. sent(aid %d->%d)\n", srcAid, dstAid);
            relay = TRUE;
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
                "+++ drop relay unreliable data. (aid %d->%d)\n", srcAid, dstAid);
            return;
        }
    }
    if (forward == TRUE && relay == TRUE)
    {
        DWCTransportForwardHeader* packet = DWC_Alloc(DWC_ALLOCTYPE_BASE,
            size + sizeof(DWCTransportForwardHeader));
        if (!packet)
            return;
        strncpy(packet->magic, DWC_FORWARD_MAGIC, sizeof(packet->magic));
        packet->srcAid = srcAid;
        packet->aidBitmap = DWCi_HtoLEl(1 << dstAid);
        DWCi_Np_CpuCopy8(buffer, packet + 1, size);
        gt2Send(connection, (const GT2Byte*)packet,
            size + (int)sizeof(DWCTransportForwardHeader), GT2False);
        if (packet)
            DWC_Free(DWC_ALLOCTYPE_BASE, packet, 0);
    }
    else
    {
        gt2Send(connection, buffer, size, GT2False);
    }
}

static BOOL DWCs_SendUnreliableBitmapForwardToServer(u32 bitmap, const void* buffer, int size)
{
    GT2Connection connection = NULL;
    DWCNodeInfo* server;
    DWCTransportForwardHeader* packet;
    u8 aid;

    if (DWCi_IsError())
        return FALSE;
    if (DWC_IsServerMyself() == TRUE)
        return FALSE;
    if (size > DWC_TRANSPORT_UNRELIABLE_MAX)
    {
        DWC_Printf(DWC_REPORTFLAG_SEND_INFO,
            "+++ SendUnreliable size is too large ( %d > %d )\n",
            size, DWC_TRANSPORT_UNRELIABLE_MAX);
        return FALSE;
    }
    server = DWCi_GetNodeByIndex(0);
    if (server)
        connection = DWCi_GetGT2Connection(server->aid);
    if (!server || !connection)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "server is now unavailable.\n");
        return FALSE;
    }
    packet = DWC_Alloc(DWC_ALLOCTYPE_BASE, size + sizeof(DWCTransportForwardHeader));
    if (!packet)
        return FALSE;
    strncpy(packet->magic, DWC_FORWARD_MAGIC, sizeof(packet->magic));
    packet->srcAid = DWC_GetMyAID();
    packet->aidBitmap = DWCi_HtoLEl(bitmap);
    DWCi_Np_CpuCopy8(buffer, packet + 1, size);
    gt2Send(connection, (const GT2Byte*)packet,
        size + (int)sizeof(DWCTransportForwardHeader), GT2False);
    if (packet)
        DWC_Free(DWC_ALLOCTYPE_BASE, packet, 0);
    if (sTransInfo->sendCallback)
    {
        for (aid = 0; aid < DWC_MAX_CONNECTIONS; aid++)
        {
            if (bitmap & (1 << aid))
                sTransInfo->sendCallback(size, aid, sTransInfo->sendParam);
        }
    }
    return TRUE;
}

static void DWCs_HandleReliableMessage(
    GT2Connection connection, u8* message, int size)
{
    u8 aid = DWCi_GetConnectionAID(connection);
    u16 type;

    if (aid == 0xff)
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "+++ Invalid connection. has no DWCConnectionInfo\n");
        DWCi_SetError(DWC_ERROR_NETWORK,
            DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_TRANSPORT_ERROR_CONNECTION);
        return;
    }

    switch (DWCs_GetRecvState(aid))
    {
    case DWC_TRANSPORT_RECV_NOBUF:
        type = DWCs_DecodeHeader((DWCTransportHeader*)message);
        if (type >= DWC_SEND_TYPE_MATCH_SYN
            && type <= DWC_SEND_TYPE_MATCH_ACK)
        {
            DWCs_RecvDataHeader(aid, (DWCTransportHeader*)message, size);
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
                "+++ Recv buffer is not set\n");
        }
        break;
    case DWC_TRANSPORT_RECV_HEADER:
        DWCs_RecvDataHeader(aid, (DWCTransportHeader*)message, size);
        break;
    case DWC_TRANSPORT_RECV_BODY:
        DWCs_RecvDataBody(aid, message, size);
        break;
    case DWC_TRANSPORT_RECV_SYSTEM_DATA:
        DWCs_RecvSystemDataBody(aid, message, size);
        break;
    case DWC_TRANSPORT_RECV_ERROR:
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "+++ Recv size is too large ( buffer size = %d < %d )\n",
            sTransInfo->connections[aid].recvBufferSize, size);
        sTransInfo->connections[aid].recvState = DWC_TRANSPORT_RECV_HEADER;
        sTransInfo->connections[aid].recvingSize = 0;
        sTransInfo->connections[aid].requestRecvSize = 0;
        break;
    default:
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Recv error (state is %d).\n",
            DWCs_GetRecvState(aid));
        DWCi_SetError(DWC_ERROR_NETWORK,
            DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2
                + DWC_ECODE_TYPE_TRANS_HEADER);
        break;
    }
}

static void DWCs_HandleUnreliableMessage(GT2Connection connection, u8* message, int size)
{
    u8 aid = DWCi_GetConnectionAID(connection);
    u8 srcAid;
    DWCTransportConnection* entry;
    BOOL forwarded = FALSE;
    BOOL receive = FALSE;

    if (aid == 0xff)
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "+++ Invalid connection. has no DWCConnectionInfo\n");
        DWCi_SetError(DWC_ERROR_NETWORK,
            DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2 + DWC_TRANSPORT_ERROR_CONNECTION);
        return;
    }
    srcAid = aid;
    if (size >= sizeof(DWCSBMessageHeader) && strncmp((const char*)message, DWC_SB_COMMAND_STRING, 4) == 0)
    {
        DWCi_HandleGT2UnreliableMatchCommandMessage(message, size);
        return;
    }
    if (size >= (int)sizeof(DWCTransportForwardHeader)
        && memcmp(message, DWC_FORWARD_MAGIC, 3) == 0)
    {
        forwarded = TRUE;
    }
    if (forwarded == TRUE)
    {
        DWCTransportForwardHeader* header = (DWCTransportForwardHeader*)message;
        u32 bitmap;
        u8 i;

        srcAid = header->srcAid;
        bitmap = DWCi_LEtoHl(header->aidBitmap);
        if (!DWC_IsServerMyself())
        {
            if (bitmap & (1 << DWC_GetMyAID()))
                receive = TRUE;
            else
                return;
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
                "+++ fwd received unreliable. sent(aid %d->%08x)\n", srcAid, bitmap);
            for (i = 0; i < DWC_MAX_CONNECTIONS; i++)
            {
                if (bitmap & (1 << i))
                {
                    if (i == DWC_GetMyAID())
                        receive = TRUE;
                    else
                        DWCi_SendUnreliable(srcAid, i, message, size, FALSE);
                }
            }
        }
        if (!receive)
            return;
    }
    entry = &sTransInfo->connections[srcAid];
    if (forwarded == TRUE)
    {
        message += sizeof(DWCTransportForwardHeader);
        size -= sizeof(DWCTransportForwardHeader);
    }
    if (entry->recvBuffer && entry->recvBufferSize >= size)
    {
        if (sTransInfo->recvCallback)
            sTransInfo->recvCallback(srcAid, message, size, sTransInfo->recvParam);
        if (sTransInfo->recvTimeoutCallback && entry->recvTimeoutTime)
            entry->previousRecvTick = DWCi_Np_GetTick();
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "Recv data size is too large (%d > %d)\n", size, entry->recvBufferSize);
    }
}

static void DWCs_RecvDataHeader(
    u8 aid, DWCTransportHeader* message, int size)
{
    u16 type;
    DWCTransportConnection* connection = &sTransInfo->connections[aid];
    DWCTransportHeader header;

    connection->lastRecvState = (u8)DWCs_GetRecvState(aid);

    switch (type = DWCs_DecodeHeader(message))
    {
    case DWC_SEND_TYPE_USERDATA:
        if (size != sizeof(DWCTransportHeader))
        {
            DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
                "+++ Invalid header from aid %d\n", aid);
            return;
        }

        DWCi_Np_CpuCopy8(message, &header, sizeof(DWCTransportHeader));
        header.size = (int)DWCi_LEtoHl((u32)header.size);
        header.type = DWCi_LEtoHs(header.type);

        connection->requestRecvSize = header.size;
        connection->recvingSize = 0;

        if (connection->recvBuffer
            && connection->recvBufferSize >= connection->requestRecvSize)
        {
            connection->recvState = DWC_TRANSPORT_RECV_BODY;
        }
        else
        {
            connection->recvState = DWC_TRANSPORT_RECV_ERROR;
        }
        break;

    case DWC_SEND_TYPE_MATCH_SYN:
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
    case DWC_SEND_TYPE_MATCH_ACK:
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO, "Received system header(%d).\n", type);
        connection->recvState = DWC_TRANSPORT_RECV_SYSTEM_DATA;
        break;

    default:
        DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
            "+++ Invalid header from aid %d (type:%d)\n", aid, type);
        break;
    }

    connection->lastRecvType = type;
}

static void DWCs_RecvDataBody(u8 aid, void* message, int size)
{
    DWCTransportConnection* connection;
    int requestSize;

    connection = &sTransInfo->connections[aid];

    if (DWCs_GetRecvState(aid) == DWC_TRANSPORT_RECV_BODY)
    {
        if (connection->recvingSize + size > connection->recvBufferSize)
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "Recv buffer over flow.\n");
            DWCi_SetError(DWC_ERROR_NETWORK,
                DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_GT2
                    + DWC_ECODE_TYPE_TRANS_BODY);
            return;
        }

        DWCi_Np_CpuCopy8(message,
            (u8*)connection->recvBuffer + connection->recvingSize,
            (u32)size);
    }

    connection->recvingSize += size;

    DWC_Printf(DWC_REPORTFLAG_RECV_INFO,
        "aid = %d size = %d/%d state = %d incoming buffer = %d\n", aid,
        connection->recvingSize, connection->requestRecvSize,
        DWCs_GetRecvState(aid),
        gt2GetIncomingBufferFreeSpace(DWCi_GetGT2Connection(aid)));

    if (connection->recvingSize == connection->requestRecvSize)
    {
        requestSize = connection->requestRecvSize;
        connection->recvState = DWC_TRANSPORT_RECV_HEADER;
        connection->recvingSize = 0;
        connection->requestRecvSize = 0;

        if (sTransInfo->recvCallback)
        {
            sTransInfo->recvCallback(aid, connection->recvBuffer, requestSize, sTransInfo->recvParam);
        }
    }

    if (sTransInfo->recvTimeoutCallback && connection->recvTimeoutTime > 0)
    {
        connection->previousRecvTick = DWCi_Np_GetTick();
    }
}

static void DWCs_RecvSystemDataBody(u8 aid, void* message, int size)
{
#pragma unused(size)
    DWCTransportConnection* transConnection = DWCs_GetTransConnection(aid);

    transConnection->recvState = transConnection->lastRecvState;

    switch (transConnection->lastRecvType)
    {
    case DWC_SEND_TYPE_MATCH_SYN:
    case DWC_SEND_TYPE_MATCH_SYN_ACK:
    case DWC_SEND_TYPE_MATCH_ACK:
        DWCi_ProcessMatchSynPacket(
            aid, transConnection->lastRecvType, (u8*)message);
        break;
    }
}
