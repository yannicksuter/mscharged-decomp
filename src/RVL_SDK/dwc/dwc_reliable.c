#include <dwc/dwc_reliable.h>

#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_report.h>

typedef struct DWCReliableQueueNode
{
    GT2Connection connection;
    u8* buffer;
    int size;
    struct DWCReliableQueueNode* next;
} DWCReliableQueueNode;

static struct
{
    DWCReliableQueueNode* head;
    int size;
} s_reliable_queue;

typedef GT2Result (*DWCReliableSendCallback)(GT2Connection connection,
    const GT2Byte* message, int len, GT2Bool reliable);

static BOOL DWCi_ProcessReliableQueueWithCallback(DWCReliableSendCallback send);

void DWCi_InitReliableQueue(void)
{
    s_reliable_queue.head = NULL;
    s_reliable_queue.size = 0;
}

void DWCi_CleanupReliableQueue(void)
{
    DWCReliableQueueNode* node = s_reliable_queue.head;

    while (node)
    {
        s_reliable_queue.head = node->next;
        DWC_Free(DWC_ALLOCTYPE_BASE, node->buffer, node->size);
        DWC_Free(DWC_ALLOCTYPE_BASE, node, 0);
        node = s_reliable_queue.head;
    }
    DWCi_InitReliableQueue();
}

BOOL DWCi_IsReliableQueueFree(void)
{
    return s_reliable_queue.size < 0x6000;
}

BOOL DWCi_ProcessReliableQueue(void)
{
    return DWCi_ProcessReliableQueueWithCallback(gt2Send);
}

static BOOL DWCi_ProcessReliableQueueWithCallback(DWCReliableSendCallback send)
{
    DWCReliableQueueNode* node = s_reliable_queue.head;

    while (node)
    {
        int retry;

        for (retry = 0; retry < 3; retry++)
        {
            if (send(node->connection, node->buffer, node->size, GT2True) == GT2Success)
            {
                s_reliable_queue.size -= node->size;
                DWC_Free(DWC_ALLOCTYPE_BASE, node->buffer, node->size);
                node->buffer = NULL;
                node->size = 0;
                node->connection = NULL;
                break;
            }
        }
        if (retry == 3)
        {
            DWC_Printf(DWC_REPORTFLAG_TRANSPORT,
                "DWCi_ProcessReliableQueue() retry count exceeded!\n");
            break;
        }
        node = node->next;
    }

    node = s_reliable_queue.head;
    while (node)
    {
        if (node->buffer)
        {
            break;
        }
        s_reliable_queue.head = node->next;
        DWC_Free(DWC_ALLOCTYPE_BASE, node, 0);
        node = s_reliable_queue.head;
    }
    return node == (void*)NULL;
}

BOOL DWCi_InsertReliableQueue(GT2Connection connection, const u8* buffer, int size)
{
    DWCReliableQueueNode** link = &s_reliable_queue.head;
    DWCReliableQueueNode* node;

    if (s_reliable_queue.size + size > 0x8000)
    {
        return FALSE;
    }
    for (node = s_reliable_queue.head; node; node = node->next)
    {
        link = &node->next;
    }
    *link = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCReliableQueueNode));
    if (!*link)
    {
        return FALSE;
    }
    (*link)->buffer = DWC_Alloc(DWC_ALLOCTYPE_BASE, size);
    if (!(*link)->buffer)
    {
        DWC_Free(DWC_ALLOCTYPE_BASE, *link, sizeof(DWCReliableQueueNode));
        *link = NULL;
        return FALSE;
    }

    (*link)->connection = connection;
    (*link)->size = size;
    (*link)->next = NULL;
    DWCi_Np_CPUCopyFast((*link)->buffer, buffer, size);
    s_reliable_queue.size += size;
    return TRUE;
}
