#include <private/nhttp.h>

#include <revolution/ncd.h>

typedef struct NHTTPBgnEndInfo
{
    NCDIpConfig ipConfig;
    NHTTPAlloc alloc;
    NHTTPFree free;
    BOOL started;
    s32 socket;
    s32 sslError;
    NHTTPErr error;
    u32 _unk7DC;
    void* threadStack;
} NHTTPBgnEndInfo;

typedef struct NHTTPThreadInfo
{
    OSMessageQueue messageQueue;
    OSMessage messages[3];
    OSThread thread;
    BOOL createMessageQueue;
    u8 _unk34C[0x14];
    char sendBuffer[0x100];
} NHTTPThreadInfo;

typedef struct NHTTPThreadWork
{
    /* 0x000 */ s32 requestId;
    /* 0x004 */ char hostname[0x100];
    /* 0x104 */ char recvBuffer[0x200];
    /* 0x304 */ char statusLine[0xE];
    /* 0x314 */ u32 hostAddr;
    /* 0x318 */ u32 lastHostAddr;
    /* 0x31C */ s32 port;
    /* 0x320 */ s32 lastPort;
    /* 0x324 */ s32 sendLength;
    /* 0x328 */ s32 headerLength;
    /* 0x32C */ s32 contentLength;
    /* 0x330 */ s32 error;
    /* 0x334 */ s32 resume;
    /* 0x338 */ s32 reuse;
    /* 0x33C */ s32 chunked;
} NHTTPThreadWork;

void* NHTTPi_alloc(u32 size, int align);
void NHTTPi_free(void* ptr);
void NHTTPi_SetError(NHTTPBgnEndInfo* info, NHTTPErr error);
void* NHTTPi_memclr(void* destination, u32 size);
s32 NHTTPi_strlen(const char* string);
void* NHTTPi_memcpy(void* destination, const void* source, u32 size);
s32 NHTTPi_strcmp(const char* left, const char* right);
s32 NHTTPi_strnicmp(const char* left, const char* right, s32 size);
s32 NHTTPi_strToInt(const char* string, s32 length);
s32 NHTTPi_strToHex(const char* string, s32 length);
s32 NHTTPi_intToStr(char* destination, u32 value);
s32 NHTTPi_encodeUrlChar(char* destination, char c);
s32 NHTTPi_getUrlEncodedSize(const char* string);
s32 NHTTPi_getUrlEncodedSize2(const char* string, s32 length);

void* NHTTPi_GetSystemInfoP(void);
NHTTPBgnEndInfo* NHTTPi_GetBgnEndInfoP(void* systemInfo);
NHTTPListInfo* NHTTPi_GetListInfoP(void* systemInfo);
NHTTPReqInfo* NHTTPi_GetReqInfoP(void* systemInfo);
NHTTPThreadInfo* NHTTPi_GetThreadInfoP(void* systemInfo);
void* NHTTPi_GetMutexInfoP(void* systemInfo);

void NHTTPi_lockReqList(void* mutexInfo);
void NHTTPi_unlockReqList(void* mutexInfo);
NHTTPReqQueue* NHTTPi_getReqFromReqQueue(NHTTPListInfo* listInfo);
NHTTPConnectionInfo* NHTTPi_Request2Connection(void* mutexInfo,
    NHTTPRequestInfo* request);
NHTTPConnectionInfo* NHTTPi_Response2Connection(void* mutexInfo,
    NHTTPResponseInfo* response);
void NHTTPi_destroyRequestObject(void* mutexInfo, NHTTPRequestInfo* request);
void NHTTPi_SetVirtualContentLength(NHTTPConnectionInfo* connection,
    u32 contentLength);
s32 NHTTPi_PostSendCallback(void* mutexInfo, NHTTPConnectionInfo* connection,
    void* value, u32 arg);
void NHTTPi_BufferFullCallback(void* mutexInfo,
    NHTTPConnectionInfo* connection);
void NHTTPi_ReceivedCallback(void* mutexInfo,
    NHTTPConnectionInfo* connection);
void NHTTPi_CompleteCallback(void* mutexInfo,
    NHTTPConnectionInfo* connection);
void NHTTPi_idleCommThread(NHTTPThreadInfo* info);

s32 NHTTPi_GetSSLError(NHTTPBgnEndInfo* info);
u32 NHTTPi_resolveHostname(NHTTPRequestInfo* request, const char* hostname);
s32 NHTTPi_SocOpen(NHTTPRequestInfo* request);
s32 NHTTPi_SocClose(void* mutexInfo, NHTTPRequestInfo* request, s32 socket);
s32 NHTTPi_SocConnect(NHTTPBgnEndInfo* info, void* mutexInfo,
    NHTTPRequestInfo* request, s32 socket, u32 address, u32 port);
s32 NHTTPi_SocSSLConnect(NHTTPBgnEndInfo* info, void* mutexInfo,
    NHTTPRequestInfo* request, s32 socket);
s32 NHTTPi_SocSend(NHTTPRequestInfo* request, s32 socket, const char* buffer,
    s32 length, s32 flags);
s32 NHTTPi_SocRecv(NHTTPRequestInfo* request, s32 socket, char* buffer,
    s32 length, s32 flags);

NHTTPHeader* NHTTPi_getHdrFromList(NHTTPHeader** list);
BOOL NHTTPi_isRecvBufFull(NHTTPResponseInfo* response, u32 received);
s32 NHTTPi_RecvBuf(NHTTPRequestInfo* request, s32 socket, s32 offset,
    s32 flags);
s32 NHTTPi_RecvBufN(NHTTPRequestInfo* request, s32 socket, u32 offset,
    s32 length, s32 flags);
s32 NHTTPi_getHeaderValue(NHTTPHdrRecvBuf* recvBuf, const char* name,
    s32* position);
s32 NHTTPi_findNextLineHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start,
    s32 end, s32* separator, s32* lineBreakLength);
BOOL NHTTPi_loadFromHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, char* destination,
    s32 start, s32 length);
s32 NHTTPi_compareTokenN_HdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start,
    s32 end, const char* token, s8 terminal);

s32 NHTTPi_CheckHeaderEnd(char* buffer, s32 position);
s32 NHTTPi_SaveBuf(NHTTPRequestInfo* request, char* buffer, s32 socket,
    s32* bufferedBytes, const char* data, s32 length);
BOOL NHTTPi_GetPostContentlength(void* mutexInfo, NHTTPRequestInfo* request,
    void* value, s32* contentLength, s32 format);
s32 NHTTPi_SendPostData(void* mutexInfo, NHTTPRequestInfo* request,
    char* buffer, void* value, s32 socket, s32* bufferedBytes, s32 format);
BOOL NHTTPi_BufFull(void* mutexInfo, NHTTPResponseInfo* response);
s32 NHTTPi_SendProxyConnectMethod(NHTTPThreadWork* work);
BOOL NHTTPi_RecvProxyConnectHeader(NHTTPThreadWork* work);
s32 NHTTPi_SendHeaderList(NHTTPThreadWork* work);
s32 NHTTPi_SendProcPostDataRaw(NHTTPThreadWork* work);
s32 NHTTPi_SendProcPostDataBinary(NHTTPThreadWork* work);
s32 NHTTPi_SendProcPostDataAscii(NHTTPThreadWork* work);
void NHTTPi_ThreadReqEnd(NHTTPThreadWork* work);
BOOL NHTTPi_ThreadExecReqQueue(NHTTPThreadWork* work);
BOOL NHTTPi_ThreadHostAddrProc(NHTTPThreadWork* work);
BOOL NHTTPi_ThreadConnectProc(NHTTPThreadWork* work);
s32 NHTTPi_ThreadProxyProc(NHTTPThreadWork* work);
s32 NHTTPi_ThreadSendProc(NHTTPThreadWork* work);
BOOL NHTTPi_ThreadRecvHeaderProc(NHTTPThreadWork* work);
BOOL NHTTPi_ThreadParseHeaderProc(NHTTPThreadWork* work);
BOOL NHTTPi_ThreadRecvBodyProc(NHTTPThreadWork* work);
void NHTTPi_CommThreadProcMain(void* argument);

#define NHTTPi_SEND(work, data, length, err)                                  \
    do                                                                        \
    {                                                                         \
        s32 sendLength = (length);                                            \
        const char* sendData = (data);                                        \
        void* system = NHTTPi_GetSystemInfoP();                               \
        s32 result = NHTTPi_SaveBuf(                                          \
            NHTTPi_GetReqInfoP(system)->reqQueue->request,                    \
            NHTTPi_GetThreadInfoP(system)->sendBuffer,                        \
            NHTTPi_GetBgnEndInfoP(system)->socket, &(work)->sendLength,       \
            sendData, sendLength);                                            \
        if (result < 0)                                                       \
        {                                                                     \
            (err) = 1;                                                        \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            (err) = (result == 0) ? 2 : 0;                                    \
        }                                                                     \
    } while (0)

void NHTTPi_InitThreadInfo(NHTTPThreadInfo* info)
{
    info->createMessageQueue = FALSE;
}

void NHTTPi_IsCreateCommThreadMessageQueueOn(NHTTPThreadInfo* info)
{
    info->createMessageQueue = TRUE;
}

BOOL NHTTPi_IsCreateCommThreadMessageQueue(NHTTPThreadInfo* info)
{
    return info->createMessageQueue;
}

s32 NHTTPi_CheckHeaderEnd(char* buffer, s32 position)
{
    if (buffer[(position - 2) & 3] == '\r' && buffer[(position - 1) & 3] == '\r')
    {
        return 1;
    }
    if (buffer[(position - 2) & 3] == '\n' && buffer[(position - 1) & 3] == '\n')
    {
        return 1;
    }
    if (buffer[(position - 4) & 3] == '\r' && buffer[(position - 3) & 3] == '\n'
        && buffer[(position - 2) & 3] == '\r'
        && buffer[(position - 1) & 3] == '\n')
    {
        return 1;
    }
    return 0;
}

s32 NHTTPi_SaveBuf(NHTTPRequestInfo* request, char* buffer, s32 socket,
    s32* bufferedBytes, const char* data, s32 length)
{
    s32 remain = length;
    while (remain > 0)
    {
        if (request->cancel)
        {
            return -1;
        }
        {
            s32 chunk = remain;
            s32 room = 0x100 - *bufferedBytes;
            if (remain > room)
            {
                chunk = room;
            }
            NHTTPi_memcpy(buffer + *bufferedBytes, data, chunk);
            data += chunk;
            remain -= chunk;
            *bufferedBytes += chunk;
            if (*bufferedBytes == 0x100)
            {
                s32 sent = NHTTPi_SocSend(request, socket, buffer, 0x100, 0);
                if (sent <= 0)
                {
                    return sent;
                }
                *bufferedBytes -= sent;
            }
        }
    }
    return length;
}

BOOL NHTTPi_GetPostContentlength(void* mutexInfo, NHTTPRequestInfo* request,
    void* value, s32* contentLength, s32 format)
{
    u32 total = 0;
    NHTTPConnectionInfo* connection =
        NHTTPi_Request2Connection(mutexInfo, request);
    if (connection == NULL)
    {
        return FALSE;
    }
    connection->_unk40 = 0;
    for (;;)
    {
        if (request->cancel)
        {
            return FALSE;
        }
        connection->_unk44 = 0;
        if (NHTTPi_PostSendCallback(mutexInfo, connection, value, total) < 0)
        {
            return FALSE;
        }
        {
            u32 size = connection->_unk44;
            void* data = (void*)connection->_unk40;
            if (size == 0)
            {
                break;
            }
            if (data == NULL)
            {
                return FALSE;
            }
            total += size;
            switch (format)
            {
            case 0:
            case 1:
                *contentLength += size;
                break;
            case 2:
                *contentLength += NHTTPi_getUrlEncodedSize2(data, size);
                break;
            default:
                break;
            }
        }
    }
    return TRUE;
}

s32 NHTTPi_SendPostData(void* mutexInfo, NHTTPRequestInfo* request,
    char* buffer, void* value, s32 socket, s32* bufferedBytes, s32 format)
{
    u32 total = 0;
    NHTTPConnectionInfo* connection =
        NHTTPi_Request2Connection(mutexInfo, request);
    if (connection == NULL)
    {
        return 3;
    }
    connection->_unk40 = 0;
    for (;;)
    {
        if (request->cancel)
        {
            return 3;
        }
        connection->_unk44 = 0;
        if (NHTTPi_PostSendCallback(mutexInfo, connection, value, total) < 0)
        {
            return 3;
        }
        {
            u32 size = connection->_unk44;
            char* data = (char*)connection->_unk40;
            if (size == 0)
            {
                break;
            }
            if (data == NULL)
            {
                return 3;
            }
            total += size;
            switch (format)
            {
            case 0:
            case 1:
            {
                s32 result = NHTTPi_SaveBuf(request, buffer, socket,
                    bufferedBytes, data, size);
                if (result < 0)
                {
                    return 1;
                }
                if (result == 0)
                {
                    return 2;
                }
                break;
            }
            case 2:
            {
                char* p = data;
                u32 i;
                for (i = 0; i < size; i++, p++)
                {
                    char encoded[3];
                    s32 result;
                    NHTTPi_memclr(encoded, 3);
                    result = NHTTPi_SaveBuf(request, buffer, socket,
                        bufferedBytes, encoded,
                        NHTTPi_encodeUrlChar(encoded, *p));
                    if (result < 0)
                    {
                        return 1;
                    }
                    if (result == 0)
                    {
                        return 2;
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }
    return 0;
}

BOOL NHTTPi_BufFull(void* mutexInfo, NHTTPResponseInfo* response)
{
    BOOL result = FALSE;
    BOOL full = NHTTPi_isRecvBufFull(response, response->recvBuf._unk4);
    if (response->recvBuf.bufferSize == 0 || response->recvBuf.buffer == NULL
        || full)
    {
        NHTTPConnectionInfo* connection =
            NHTTPi_Response2Connection(mutexInfo, response);
        if (connection != NULL)
        {
            NHTTPi_BufferFullCallback(mutexInfo, connection);
            if (response->recvBuf.buffer != NULL
                && response->recvBuf.bufferSize != 0)
            {
                if (!NHTTPi_isRecvBufFull(response, response->recvBuf._unk4))
                {
                    result = TRUE;
                }
            }
        }
    }
    else if (!full)
    {
        result = TRUE;
    }
    return result;
}

s32 NHTTPi_SendProxyConnectMethod(NHTTPThreadWork* work)
{
    char portString[0x10];
    s32 portLength;
    s32 err;
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    char* buffer = NHTTPi_GetThreadInfoP(system)->sendBuffer;
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    portLength = NHTTPi_intToStr(portString, request->port);

    NHTTPi_SEND(work, "CONNECT ", 8, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, request->url + 8, request->hostEnd - 8, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, ":", 1, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, portString, portLength, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, " HTTP/1.1\r\n", 11, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "Host: ", 6, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, request->url + 8, request->hostEnd - 8, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, ":", 1, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, portString, portLength, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "Content-Length: 0\r\nPragma: no-cache\r\n", 37, err);
    if (err != 0)
    {
        return err;
    }
    if (request->proxyAuthorizationLength != 0)
    {
        NHTTPRequestInfo* current =
            NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP())->reqQueue->request;
        NHTTPi_SEND(work, "Proxy-Authorization: Basic ", 27, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, current->proxyAuthorization,
            current->proxyAuthorizationLength, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
    }
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (work->sendLength > 0)
    {
        s32 sent = NHTTPi_SocSend(request, bgnEndInfo->socket, buffer,
            work->sendLength, 0);
        if (sent < 0)
        {
            return 1;
        }
        if (sent == 0)
        {
            return 2;
        }
    }
    work->sendLength = 0;
    NHTTPi_memclr(buffer, 0x100);
    return 0;
}

BOOL NHTTPi_RecvProxyConnectHeader(NHTTPThreadWork* work)
{
    char buffer[0x200];
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(system);
    NHTTPThreadInfo* threadInfo = NHTTPi_GetThreadInfoP(system);
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    char* sendBuffer = threadInfo->sendBuffer;
    NHTTPRequestInfo* request = reqInfo->reqQueue->request;
    NHTTPResponseInfo* response = request->response;
    BOOL ok = FALSE;
    s32 received = 0;
    for (;;)
    {
        s32 result = NHTTPi_SocRecv(request, bgnEndInfo->socket,
            buffer + received, 0x200 - received, 0);
        received += result;
        response->recvBuf.resultCode = NHTTPi_strToInt(buffer + 9, 3);
        if (NHTTPi_strnicmp(buffer, "HTTP/", 5) == 0 && buffer[8] == ' '
            && response->recvBuf.resultCode == 200)
        {
            ok = TRUE;
        }
        {
            char* p = buffer;
            s32 i = 0;
            BOOL end = FALSE;
            for (; i < received; p++, i++)
            {
                if (i > 1 && p[-1] == '\r' && p[0] == '\r')
                {
                    end = TRUE;
                }
                else if (i > 1 && p[-1] == '\n' && p[0] == '\n')
                {
                    end = TRUE;
                }
                else if (i > 3 && p[-3] == '\r' && p[-2] == '\n'
                    && p[-1] == '\r' && p[0] == '\n')
                {
                    end = TRUE;
                }
            }
            if (end)
            {
                return ok != 0;
            }
        }
        if (result < 0)
        {
            return FALSE;
        }
        if (received < 0x200)
        {
            continue;
        }
        result = NHTTPi_SocRecv(request, bgnEndInfo->socket, sendBuffer, 1, 0);
        if (result < 0)
        {
            return FALSE;
        }
        if (result == 0)
        {
            continue;
        }
        return FALSE;
    }
}

s32 NHTTPi_SendHeaderList(NHTTPThreadWork* work)
{
    s32 err;
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPHeader* header = NHTTPi_getHdrFromList(&request->headers);
    while (header != NULL)
    {
        NHTTPi_SEND(work, header->name, NHTTPi_strlen(header->name), err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, ": ", 2, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, header->value, NHTTPi_strlen(header->value), err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_free(header);
        header = NHTTPi_getHdrFromList(&request->headers);
    }
    return 0;
}

s32 NHTTPi_SendProcPostDataRaw(NHTTPThreadWork* work)
{
    s32 contentLength;
    char numString[0x10];
    s32 numLength;
    s32 err;
    void* system = NHTTPi_GetSystemInfoP();
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPThreadInfo* threadInfo = NHTTPi_GetThreadInfoP(system);
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    char* buffer = threadInfo->sendBuffer;
    contentLength = 0;
    if (request->postBuffer == NULL)
    {
        if (!NHTTPi_GetPostContentlength(mutexInfo, request, NULL,
                &contentLength, 0))
        {
            return 3;
        }
    }
    else
    {
        contentLength = request->postBufferSize;
    }
    numLength = NHTTPi_intToStr(numString, contentLength);
    NHTTPi_SEND(work, "Content-Length: ", 16, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, numString, numLength, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    if (request->postBuffer == NULL)
    {
        s32 result = NHTTPi_SendPostData(mutexInfo, request, buffer, NULL,
            bgnEndInfo->socket, &work->sendLength, 0);
        if (result != 0)
        {
            return result;
        }
    }
    else
    {
        NHTTPi_SEND(work, request->postBuffer, request->postBufferSize, err);
        if (err != 0)
        {
            return err;
        }
    }
    return 0;
}

static const char NHTTPi_strContentDisposition[] =
    "Content-Disposition: form-data; name=\"";
static const char NHTTPi_strContentTypeBinary[] =
    "Content-Type: application/octet-stream\r\n"
    "Content-Transfer-Encoding: binary\r\n";
static const char NHTTPi_strContentTypeAscii[] =
    "Content-Type: application/x-www-form-urlencoded\r\n";
static const char NHTTPi_strContentTypeMultipart[] =
    "Content-Type: multipart/form-data; boundary=";

s32 NHTTPi_SendProcPostDataBinary(NHTTPThreadWork* work)
{
    s32 contentLength;
    char numString[0x10];
    s32 numLength;
    s32 err;
    NHTTPHeader* item;
    void* system = NHTTPi_GetSystemInfoP();
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPThreadInfo* threadInfo = NHTTPi_GetThreadInfoP(system);
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    char* buffer = threadInfo->sendBuffer;
    contentLength = 0;
    for (item = request->postData; item != NULL;)
    {
        contentLength += 0x16;
        contentLength += NHTTPi_strlen(item->name) + 0x29;
        if (item->_unk14 != 0)
        {
            contentLength += 0x4B;
        }
        contentLength += 2;
        if (item->value == NULL)
        {
            if (!NHTTPi_GetPostContentlength(mutexInfo, request, item->name,
                    &contentLength, 1))
            {
                return 3;
            }
        }
        else
        {
            contentLength += item->length;
        }
        contentLength += 2;
        if (item == request->postData->next)
        {
            break;
        }
        item = item->prev;
    }
    contentLength += 0x18;
    numLength = NHTTPi_intToStr(numString, contentLength);
    NHTTPi_SEND(work, NHTTPi_strContentTypeMultipart, 44, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, request->multipartBoundary + 2, 18, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "Content-Length: ", 16, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, numString, numLength, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    for (item = request->postData; item != NULL;)
    {
        NHTTPi_SEND(work, request->multipartBoundary, 20, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, NHTTPi_strContentDisposition, 38, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, item->name, NHTTPi_strlen(item->name), err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, "\"\r\n", 3, err);
        if (err != 0)
        {
            return err;
        }
        if (item->_unk14 != 0)
        {
            NHTTPi_SEND(work, NHTTPi_strContentTypeBinary, 75, err);
            if (err != 0)
            {
                return err;
            }
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
        if (err != 0)
        {
            return err;
        }
        if (item->value == NULL)
        {
            s32 result = NHTTPi_SendPostData(mutexInfo, request, buffer,
                item->name, bgnEndInfo->socket, &work->sendLength, 1);
            if (result != 0)
            {
                return result;
            }
        }
        else
        {
            NHTTPi_SEND(work, item->value, item->length, err);
            if (err != 0)
            {
                return err;
            }
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
        if (err != 0)
        {
            return err;
        }
        if (item == request->postData->next)
        {
            break;
        }
        item = item->prev;
    }
    NHTTPi_SEND(work, request->multipartBoundary, 20, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "--\r\n", 4, err);
    if (err != 0)
    {
        return err;
    }
    return 0;
}

s32 NHTTPi_SendProcPostDataAscii(NHTTPThreadWork* work)
{
    s32 contentLength;
    char numString[0x10];
    s32 numLength;
    s32 err;
    NHTTPHeader* item;
    void* system = NHTTPi_GetSystemInfoP();
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPThreadInfo* threadInfo = NHTTPi_GetThreadInfoP(system);
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    char* buffer = threadInfo->sendBuffer;
    contentLength = 0;
    for (item = request->postData; item != NULL;)
    {
        contentLength += NHTTPi_getUrlEncodedSize(item->name) + 1;
        if (item->value == NULL)
        {
            if (!NHTTPi_GetPostContentlength(mutexInfo, request, item->name,
                    &contentLength, 2))
            {
                return 3;
            }
        }
        else
        {
            contentLength += NHTTPi_getUrlEncodedSize(item->value);
        }
        if (item == request->postData->next)
        {
            break;
        }
        contentLength += 1;
        item = item->prev;
    }
    numLength = NHTTPi_intToStr(numString, contentLength);
    NHTTPi_SEND(work, NHTTPi_strContentTypeAscii, 49, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "Content-Length: ", 16, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, numString, numLength, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    for (item = request->postData; item != NULL;)
    {
        s32 i;
        for (i = 0; item->name[i] != 0; i++)
        {
            NHTTPi_SEND(work, numString,
                NHTTPi_encodeUrlChar(numString, item->name[i]), err);
            if (err != 0)
            {
                return err;
            }
        }
        NHTTPi_SEND(work, "=", 1, err);
        if (err != 0)
        {
            return err;
        }
        if (item->value == NULL)
        {
            s32 result = NHTTPi_SendPostData(mutexInfo, request, buffer,
                item->name, bgnEndInfo->socket, &work->sendLength, 2);
            if (result != 0)
            {
                return result;
            }
        }
        else
        {
            for (i = 0; item->value[i] != 0; i++)
            {
                NHTTPi_SEND(work, numString,
                    NHTTPi_encodeUrlChar(numString, item->value[i]), err);
                if (err != 0)
                {
                    return err;
                }
            }
        }
        if (item == request->postData->next)
        {
            break;
        }
        NHTTPi_SEND(work, "&", 1, err);
        if (err != 0)
        {
            return err;
        }
        item = item->prev;
    }
    return 0;
}

void NHTTPi_ThreadReqEnd(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(system);
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPRequestInfo* request = reqInfo->reqQueue->request;
    NHTTPResponseInfo* response = request->response;
    NHTTPConnectionInfo* connection =
        NHTTPi_Request2Connection(mutexInfo, request);
    if (request->cancel)
    {
        work->error = NHTTP_ERROR_CANCELED;
        work->reuse = 0;
    }
    if (work->reuse == 0)
    {
        if (bgnEndInfo->socket >= 0)
        {
            if (NHTTPi_SocClose(mutexInfo, request, bgnEndInfo->socket) < 0)
            {
                work->error = NHTTP_ERROR_REVOLUTIONWIFI;
            }
            bgnEndInfo->socket = -1;
        }
    }
    if (work->error == 0)
    {
        response->recvBuf.succeeded = TRUE;
    }
    else
    {
        response->recvBuf.succeeded = FALSE;
        NHTTPi_SetError(bgnEndInfo, (NHTTPErr)work->error);
        if (response->recvBuf.buffer == work->recvBuffer)
        {
            response->recvBuf.buffer = NULL;
            response->recvBuf.bufferSize = 0;
        }
    }
    if (connection != NULL)
    {
        connection->state = work->error;
    }
    NHTTPi_lockReqList(mutexInfo);
    NHTTPi_free(reqInfo->reqQueue);
    reqInfo->reqQueue = NULL;
    NHTTPi_unlockReqList(mutexInfo);
    NHTTPi_destroyRequestObject(mutexInfo, request);
    if (connection != NULL)
    {
        if (response->recvBuf.succeeded)
        {
            connection->started = 5;
        }
    }
    NHTTPi_CompleteCallback(mutexInfo, connection);
    if (connection != NULL)
    {
        OSSignalCond(&connection->cond);
    }
}

BOOL NHTTPi_ThreadExecReqQueue(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPReqQueue* queue;
    NHTTPi_lockReqList(mutexInfo);
    queue = NHTTPi_getReqFromReqQueue(NHTTPi_GetListInfoP(system));
    if (queue != NULL)
    {
        NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(system);
        work->requestId = queue->requestId;
        reqInfo->reqQueue = queue;
    }
    else
    {
        work->requestId = -1;
    }
    NHTTPi_unlockReqList(mutexInfo);
    if (work->requestId < 0)
    {
        NHTTPi_idleCommThread(NHTTPi_GetThreadInfoP(system));
        return FALSE;
    }
    return TRUE;
}

BOOL NHTTPi_ThreadHostAddrProc(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    char* host = request->host;
    if (request->proxyEnabled)
    {
        host = request->proxyServer;
    }
    if (NHTTPi_strlen(host) == 0 || NHTTPi_strcmp(host, work->hostname) != 0)
    {
        work->hostAddr = NHTTPi_resolveHostname(request, host);
        if (work->hostAddr == 0)
        {
            if (request->proxyEnabled)
            {
                work->error = NHTTP_ERROR_DNS_PROXY;
                return FALSE;
            }
            work->error = NHTTP_ERROR_DNS;
            return FALSE;
        }
    }
    else
    {
        work->hostAddr = work->lastHostAddr;
    }
    NHTTPi_memclr(work->hostname, 0x100);
    NHTTPi_memcpy(work->hostname, host, NHTTPi_strlen(host));
    work->port = request->port;
    if (request->proxyEnabled)
    {
        work->port = request->proxyPort;
    }
    if (work->hostAddr != work->lastHostAddr || work->port != work->lastPort
        || request->secure == 1)
    {
        work->reuse = 0;
    }
    work->lastHostAddr = work->hostAddr;
    work->lastPort = work->port;
    return TRUE;
}

BOOL NHTTPi_ThreadConnectProc(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(system);
    NHTTPRequestInfo* request = reqInfo->reqQueue->request;
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    if (work->reuse == 0)
    {
        if (bgnEndInfo->socket >= 0)
        {
            if (NHTTPi_SocClose(mutexInfo, request, bgnEndInfo->socket) < 0)
            {
                bgnEndInfo->socket = -1;
                work->error = NHTTP_ERROR_REVOLUTIONWIFI;
                return FALSE;
            }
        }
        bgnEndInfo->socket = NHTTPi_SocOpen(request);
        if (bgnEndInfo->socket < 0)
        {
            work->error = NHTTP_ERROR_SOCKET;
            return FALSE;
        }
        NHTTPi_lockReqList(mutexInfo);
        reqInfo->reqQueue->_unk10 = bgnEndInfo->socket;
        NHTTPi_unlockReqList(mutexInfo);
        if (request->cancel)
        {
            return FALSE;
        }
        if (NHTTPi_SocConnect(bgnEndInfo, mutexInfo, request,
                bgnEndInfo->socket, work->hostAddr, work->port) < 0)
        {
            if (request->proxyEnabled)
            {
                work->error = NHTTP_ERROR_CONNECT_PROXY;
                work->lastHostAddr = work->hostAddr;
                return FALSE;
            }
            if (NHTTPi_GetSSLError(bgnEndInfo) != 0)
            {
                work->error = NHTTP_ERROR_SSL;
                return FALSE;
            }
            work->error = NHTTP_ERROR_CONNECT;
            return FALSE;
        }
    }
    else
    {
        NHTTPi_lockReqList(mutexInfo);
        reqInfo->reqQueue->_unk10 = bgnEndInfo->socket;
        NHTTPi_unlockReqList(mutexInfo);
    }
    return TRUE;
}

s32 NHTTPi_ThreadProxyProc(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(system);
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPRequestInfo* request = reqInfo->reqQueue->request;
    work->error = NHTTP_ERROR_REVOLUTIONWIFI;
    if (request->secure != 0 && request->proxyEnabled != 0)
    {
        s32 result = NHTTPi_SendProxyConnectMethod(work);
        if (result != 0)
        {
            return result;
        }
        if (!NHTTPi_RecvProxyConnectHeader(work))
        {
            return 1;
        }
        if (NHTTPi_SocSSLConnect(bgnEndInfo, mutexInfo, request,
                bgnEndInfo->socket) != 0)
        {
            if (NHTTPi_GetSSLError(bgnEndInfo) != 0)
            {
                work->error = NHTTP_ERROR_SSL;
            }
            return 1;
        }
    }
    return 0;
}

s32 NHTTPi_ThreadSendProc(NHTTPThreadWork* work)
{
    s32 err;
    s32 schemeLength;
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    NHTTPConnectionInfo* connection =
        NHTTPi_Request2Connection(mutexInfo, request);
    char* buffer = NHTTPi_GetThreadInfoP(system)->sendBuffer;
    s32 urlLength = NHTTPi_strlen(request->url);
    err = 0;
    work->error = NHTTP_ERROR_REVOLUTIONWIFI;
    if (connection != NULL)
    {
        connection->started = 2;
    }
    work->sendLength = 0;
    switch (request->method)
    {
    case NHTTP_REQMETHOD_GET:
        NHTTPi_SEND(work, "GET ", 4, err);
        break;
    case NHTTP_REQMETHOD_POST:
        NHTTPi_SEND(work, "POST ", 5, err);
        break;
    case NHTTP_REQMETHOD_HEAD:
        NHTTPi_SEND(work, "HEAD ", 5, err);
        break;
    default:
        break;
    }
    if (err != 0)
    {
        return err;
    }
    if (request->proxyEnabled != 0 && request->secure == 0)
    {
        NHTTPi_SEND(work, request->url, urlLength, err);
        if (err != 0)
        {
            return err;
        }
    }
    else if (urlLength > request->pathStart)
    {
        NHTTPi_SEND(work, request->url + request->pathStart,
            urlLength - request->pathStart, err);
        if (err != 0)
        {
            return err;
        }
    }
    else
    {
        NHTTPi_SEND(work, "/", 1, err);
        if (err != 0)
        {
            return err;
        }
    }
    NHTTPi_SEND(work, " HTTP/1.1\r\n", 11, err);
    if (err != 0)
    {
        return err;
    }
    schemeLength = (request->secure != 0) + 7;
    NHTTPi_SEND(work, "Host: ", 6, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, request->url + schemeLength,
        request->hostEnd - schemeLength, err);
    if (err != 0)
    {
        return err;
    }
    NHTTPi_SEND(work, "\r\n", 2, err);
    if (err != 0)
    {
        return err;
    }
    if (request->proxyEnabled != 0 && request->secure == 0
        && request->proxyAuthorizationLength > 0)
    {
        NHTTPRequestInfo* current =
            NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP())->reqQueue->request;
        NHTTPi_SEND(work, "Proxy-Authorization: Basic ", 27, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, current->proxyAuthorization,
            current->proxyAuthorizationLength, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
    }
    if (err != 0)
    {
        return err;
    }
    if (request->authorizationLength > 0)
    {
        NHTTPRequestInfo* current =
            NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP())->reqQueue->request;
        NHTTPi_SEND(work, "Authorization: Basic ", 21, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, current->authorization,
            current->authorizationLength, err);
        if (err != 0)
        {
            return err;
        }
        NHTTPi_SEND(work, "\r\n", 2, err);
    }
    if (err != 0)
    {
        return err;
    }
    err = NHTTPi_SendHeaderList(work);
    if (err != 0)
    {
        return err;
    }
    if (request->method == NHTTP_REQMETHOD_POST)
    {
        s32 result;
        if (request->_unk10 != 0)
        {
            result = NHTTPi_SendProcPostDataRaw(work);
        }
        else
        {
            BOOL binary;
            if (request->_unkD0 == 0)
            {
                NHTTPHeader* item;
                binary = FALSE;
                for (item = request->postData; item != NULL;)
                {
                    if (item->_unk14 != 0)
                    {
                        binary = TRUE;
                        break;
                    }
                    if (item == request->postData->next)
                    {
                        break;
                    }
                    item = item->prev;
                }
            }
            else
            {
                binary = (request->_unkD0 - 2) >> 5;
            }
            if (binary)
            {
                result = NHTTPi_SendProcPostDataBinary(work);
            }
            else
            {
                result = NHTTPi_SendProcPostDataAscii(work);
            }
        }
        if (result != 0)
        {
            if (result == 3)
            {
                work->error = NHTTP_ERROR_SOCKET;
            }
            return result;
        }
    }
    else
    {
        NHTTPi_SEND(work, "\r\n", 2, err);
        if (err != 0)
        {
            return err;
        }
    }
    if (work->sendLength > 0)
    {
        s32 sent = NHTTPi_SocSend(request, bgnEndInfo->socket, buffer,
            work->sendLength, 0);
        if (sent < 0)
        {
            return 1;
        }
        if (sent == 0)
        {
            return 2;
        }
    }
    return 0;
}

BOOL NHTTPi_ThreadRecvHeaderProc(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPResponseInfo* response = request->response;
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPConnectionInfo* connection =
        NHTTPi_Request2Connection(mutexInfo, request);
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    char last4[4] = { 0, 0, 0, 0 };
    NHTTPRecvBufBlock* block;
    if (connection != NULL)
    {
        connection->started = 3;
    }
    response->recvBuf.length = 0;
    NHTTPi_memclr(work->statusLine, 0xE);
    block = response->recvBuf.blocks;
    work->headerLength = 0;
    for (;;)
    {
        s32 received;
        if (request->cancel)
        {
            return FALSE;
        }
        if (work->headerLength < 0x400)
        {
            received = NHTTPi_SocRecv(request, bgnEndInfo->socket,
                (char*)response->recvBuf.data + work->headerLength, 1, 0);
            last4[work->headerLength & 3] =
                response->recvBuf.data[work->headerLength];
        }
        else
        {
            char* p;
            u32 offset = work->headerLength & 0x1FF;
            if (offset == 0)
            {
                if (block != NULL)
                {
                    block->next =
                        (NHTTPRecvBufBlock*)NHTTPi_alloc(0x204, 4);
                    block = block->next;
                }
                else
                {
                    block = (NHTTPRecvBufBlock*)NHTTPi_alloc(0x204, 4);
                    response->recvBuf.blocks = block;
                }
                if (block == NULL)
                {
                    work->error = NHTTP_ERROR_ALLOC;
                    return FALSE;
                }
                block->next = NULL;
            }
            p = (char*)block->data + offset;
            received = NHTTPi_SocRecv(request, bgnEndInfo->socket, p, 1, 0);
            last4[work->headerLength & 3] = *p;
        }
        if (received <= 0)
        {
            work->error = NHTTP_ERROR_REVOLUTIONWIFI;
            return FALSE;
        }
        work->headerLength += received;
        if (NHTTPi_CheckHeaderEnd(last4, work->headerLength))
        {
            break;
        }
    }
    response->recvBuf.length = work->headerLength;
    if (response->recvBuf.length == 0)
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    return TRUE;
}

BOOL NHTTPi_ThreadParseHeaderProc(NHTTPThreadWork* work)
{
    s32 position;
    s32 separator;
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPResponseInfo* response = request->response;
    char* buffer = NHTTPi_GetThreadInfoP(system)->sendBuffer;
    if (!NHTTPi_loadFromHdrRecvBuf(&response->recvBuf, work->statusLine, 0,
            0xE))
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    if (NHTTPi_strnicmp(work->statusLine, "HTTP/", 5) != 0)
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    if (work->statusLine[8] != ' ')
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    response->recvBuf.resultCode = NHTTPi_strToInt(work->statusLine + 9, 3);
    if (response->recvBuf.resultCode < 0)
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    if (NHTTPi_findNextLineHdrRecvBuf(&response->recvBuf, 0xC,
            response->recvBuf.length, &separator, NULL) < 0)
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    work->contentLength = NHTTPi_getHeaderValue(&response->recvBuf,
        "Content-Length", &position);
    if (work->contentLength == 0)
    {
        work->error = 0;
        return FALSE;
    }
    if (work->contentLength > 0x100)
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    if (work->contentLength > 0)
    {
        if (!NHTTPi_loadFromHdrRecvBuf(&response->recvBuf, buffer, position,
                work->contentLength))
        {
            work->error = NHTTP_ERROR_HTTPPARSE;
            return FALSE;
        }
        work->contentLength = NHTTPi_strToInt(buffer, work->contentLength);
        if (work->contentLength < 0)
        {
            work->error = NHTTP_ERROR_HTTPPARSE;
            return FALSE;
        }
        response->recvBuf._unkC = work->contentLength;
    }
    else
    {
        response->recvBuf._unkC = -1;
    }
    if (request->secure != 0)
    {
        work->reuse = 0;
    }
    else
    {
        s32 length = NHTTPi_getHeaderValue(&response->recvBuf, "Connection",
            &position);
        if (length == 0)
        {
            work->error = NHTTP_ERROR_HTTPPARSE;
            work->reuse = 0;
            return FALSE;
        }
        if (length > 0x100)
        {
            work->reuse = 0;
        }
        else if (length > 0)
        {
            if (NHTTPi_compareTokenN_HdrRecvBuf(&response->recvBuf, position,
                    position + length, "Keep-Alive", 0) == 0)
            {
                work->reuse = 1;
            }
            else
            {
                work->reuse = 0;
            }
        }
        else
        {
            work->reuse = 0;
        }
    }
    work->chunked = NHTTPi_getHeaderValue(&response->recvBuf,
        "Transfer-Encoding", &position);
    if (work->chunked == 0)
    {
        work->error = NHTTP_ERROR_HTTPPARSE;
        return FALSE;
    }
    if (work->chunked > 0x100)
    {
        work->chunked = 0;
    }
    else if (work->chunked > 0)
    {
        work->chunked = NHTTPi_compareTokenN_HdrRecvBuf(&response->recvBuf,
            position, position + work->chunked, "chunked", ';') == 0;
    }
    else
    {
        work->chunked = 0;
    }
    work->error = 0;
    response->recvBuf.hasResultCode = TRUE;
    return TRUE;
}

BOOL NHTTPi_ThreadRecvBodyProc(NHTTPThreadWork* work)
{
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPRequestInfo* request =
        NHTTPi_GetReqInfoP(system)->reqQueue->request;
    NHTTPResponseInfo* response = request->response;
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    void* mutexInfo = NHTTPi_GetMutexInfoP(system);
    NHTTPConnectionInfo* connection =
        NHTTPi_Request2Connection(mutexInfo, request);
    char* buffer = NHTTPi_GetThreadInfoP(system)->sendBuffer;
    if (request->method == NHTTP_REQMETHOD_HEAD
        || response->recvBuf.resultCode == 204
        || response->recvBuf.resultCode == 304
        || (response->recvBuf.resultCode >= 100
            && response->recvBuf.resultCode < 200))
    {
        return TRUE;
    }
    NHTTPi_SetVirtualContentLength(connection, 0);
    if (connection != NULL)
    {
        connection->started = 4;
    }
    if (work->contentLength >= 0)
    {
        NHTTPi_SetVirtualContentLength(connection, work->contentLength);
        while (work->contentLength > 0)
        {
            s32 received;
            if (work->error != NHTTP_ERROR_BUFFULL)
            {
                if (!NHTTPi_BufFull(mutexInfo, response))
                {
                    work->error = NHTTP_ERROR_BUFFULL;
                    response->recvBuf.buffer = work->recvBuffer;
                    response->recvBuf.bufferSize = 0x200;
                }
            }
            if (work->error == NHTTP_ERROR_BUFFULL)
            {
                received = NHTTPi_RecvBufN(request, bgnEndInfo->socket, 0,
                    work->contentLength, 0);
            }
            else
            {
                received = NHTTPi_RecvBufN(request, bgnEndInfo->socket,
                    response->recvBuf._unk4, work->contentLength, 0);
            }
            if (received < 0)
            {
                return FALSE;
            }
            if (received == 0)
            {
                break;
            }
            if (work->error != NHTTP_ERROR_BUFFULL)
            {
                response->recvBuf._unk4 += received;
                response->recvBuf._unk8 += received;
            }
            work->contentLength -= received;
        }
        if (work->error != NHTTP_ERROR_BUFFULL)
        {
            if (work->contentLength != 0)
            {
                if (NHTTPi_isRecvBufFull(response, response->recvBuf._unk4))
                {
                    work->error = NHTTP_ERROR_BUFFULL;
                }
                else
                {
                    work->error = NHTTP_ERROR_REVOLUTIONWIFI;
                }
            }
            else
            {
                work->error = 0;
            }
        }
    }
    else
    {
        work->error = NHTTP_ERROR_REVOLUTIONWIFI;
        if (work->chunked != 0)
        {
            for (;;)
            {
                char crlf[2];
                s32 chunkSize;
                crlf[0] = 0;
                crlf[1] = 0;
                work->headerLength = 0;
                while (work->headerLength < 0x100)
                {
                    char c;
                    s32 received = NHTTPi_SocRecv(request,
                        bgnEndInfo->socket, buffer + work->headerLength, 1,
                        0);
                    if (received < 0)
                    {
                        return FALSE;
                    }
                    c = buffer[work->headerLength];
                    crlf[work->headerLength & 1] = c;
                    if (c == ';'
                        || (c == '\n'
                            && crlf[(work->headerLength - 1) & 1] == '\r'))
                    {
                        s32 lineLength = work->headerLength;
                        if (c == '\n')
                        {
                            lineLength--;
                        }
                        else
                        {
                            char ring[2];
                            s32 socket = bgnEndInfo->socket;
                            s32 total = 0;
                            s32 i = 0;
                            s32 result;
                            ring[0] = 0;
                            ring[1] = 0;
                            for (;;)
                            {
                                if (ring[i & 1] == '\r'
                                    && ring[(i - 1) & 1] == '\n')
                                {
                                    result = total;
                                    break;
                                }
                                result = NHTTPi_SocRecv(request, socket,
                                    &ring[i & 1], 1, 0);
                                if (result <= 0)
                                {
                                    break;
                                }
                                total += result;
                                i++;
                            }
                            if (result <= 0)
                            {
                                return FALSE;
                            }
                        }
                        if (lineLength == 0)
                        {
                            return FALSE;
                        }
                        chunkSize = NHTTPi_strToHex(buffer, lineLength);
                        if (chunkSize < 0)
                        {
                            return FALSE;
                        }
                        break;
                    }
                    work->headerLength++;
                }
                if (work->headerLength == 0x100)
                {
                    work->error = NHTTP_ERROR_HTTPPARSE;
                    return FALSE;
                }
                if (chunkSize <= 0)
                {
                    break;
                }
                NHTTPi_SetVirtualContentLength(connection, chunkSize);
                while (chunkSize > 0)
                {
                    s32 received;
                    if (work->error != NHTTP_ERROR_BUFFULL)
                    {
                        if (!NHTTPi_BufFull(mutexInfo, response))
                        {
                            work->error = NHTTP_ERROR_BUFFULL;
                            response->recvBuf.buffer = work->recvBuffer;
                            response->recvBuf.bufferSize = 0x200;
                        }
                    }
                    if (work->error == NHTTP_ERROR_BUFFULL)
                    {
                        received = NHTTPi_RecvBufN(request,
                            bgnEndInfo->socket, 0, chunkSize, 0);
                    }
                    else
                    {
                        received = NHTTPi_RecvBufN(request,
                            bgnEndInfo->socket, response->recvBuf._unk4,
                            chunkSize, 0);
                    }
                    if (received <= 0)
                    {
                        return FALSE;
                    }
                    chunkSize -= received;
                    response->recvBuf._unk4 += received;
                    response->recvBuf._unk8 += received;
                    if (chunkSize == 0)
                    {
                        received = NHTTPi_SocRecv(request,
                            bgnEndInfo->socket, buffer, 2, 0);
                        if (received <= 0)
                        {
                            return FALSE;
                        }
                    }
                }
            }
            {
                char ring[2];
                s32 socket = bgnEndInfo->socket;
                s32 i = 0;
                ring[0] = 0;
                ring[1] = 0;
                for (;;)
                {
                    if (ring[i & 1] == '\r' && ring[(i - 1) & 1] == '\n')
                    {
                        break;
                    }
                    if (NHTTPi_SocRecv(request, socket, &ring[i & 1], 1, 0)
                        <= 0)
                    {
                        break;
                    }
                    i++;
                }
                work->error = 0;
            }
        }
        else
        {
            for (;;)
            {
                s32 received;
                if (!NHTTPi_BufFull(mutexInfo, response))
                {
                    work->error = NHTTP_ERROR_BUFFULL;
                    response->recvBuf.buffer = work->recvBuffer;
                    response->recvBuf.bufferSize = 0x200;
                }
                if (work->error == NHTTP_ERROR_BUFFULL)
                {
                    received = NHTTPi_RecvBuf(request, bgnEndInfo->socket, 0,
                        0);
                }
                else
                {
                    received = NHTTPi_RecvBuf(request, bgnEndInfo->socket,
                        response->recvBuf._unk4, 0);
                }
                if (received < 0)
                {
                    return FALSE;
                }
                if (received == 0)
                {
                    if (work->error != NHTTP_ERROR_BUFFULL)
                    {
                        work->error = 0;
                    }
                    break;
                }
                response->recvBuf._unk4 += received;
                response->recvBuf._unk8 += received;
            }
        }
    }
    connection = NHTTPi_Response2Connection(mutexInfo, response);
    if (work->error == 0 && connection != NULL)
    {
        NHTTPi_ReceivedCallback(mutexInfo, connection);
    }
    return TRUE;
}

void NHTTPi_CommThreadProcMain(void* argument)
{
    NHTTPThreadWork work;
    void* system = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgnEndInfo = NHTTPi_GetBgnEndInfoP(system);
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(system);
    work.requestId = -1;
    NHTTPi_memclr(work.hostname, 0x100);
    NHTTPi_memclr(work.recvBuffer, 0x200);
    work.hostAddr = (u32)-1;
    work.lastHostAddr = (u32)-1;
    work.sendLength = 0;
    work.reuse = 0;
    work.chunked = 0;
    work.resume = 0;
    work.contentLength = 0;
    work.error = 0;
    while (bgnEndInfo->_unk7DC == 0)
    {
        if (work.resume == 0)
        {
            if (!NHTTPi_ThreadExecReqQueue(&work))
            {
                continue;
            }
            if (reqInfo->reqQueue->request->cancel)
            {
                NHTTPi_ThreadReqEnd(&work);
                continue;
            }
            if (!NHTTPi_ThreadHostAddrProc(&work))
            {
                NHTTPi_ThreadReqEnd(&work);
                continue;
            }
        }
        if (work.resume == 1)
        {
            work.resume = 0;
        }
        if (!NHTTPi_ThreadConnectProc(&work))
        {
            NHTTPi_ThreadReqEnd(&work);
            continue;
        }
        switch (NHTTPi_ThreadProxyProc(&work))
        {
        case 0:
            break;
        case 2:
            work.resume = 1;
            continue;
        case 1:
            NHTTPi_ThreadReqEnd(&work);
            continue;
        default:
            break;
        }
        switch (NHTTPi_ThreadSendProc(&work))
        {
        case 0:
            break;
        case 2:
            work.resume = 1;
            continue;
        case 1:
        case 3:
            NHTTPi_ThreadReqEnd(&work);
            continue;
        default:
            break;
        }
        if (reqInfo->reqQueue->request->cancel)
        {
            NHTTPi_ThreadReqEnd(&work);
            continue;
        }
        if (!NHTTPi_ThreadRecvHeaderProc(&work))
        {
            NHTTPi_ThreadReqEnd(&work);
            continue;
        }
        if (!NHTTPi_ThreadParseHeaderProc(&work))
        {
            NHTTPi_ThreadReqEnd(&work);
            continue;
        }
        NHTTPi_ThreadRecvBodyProc(&work);
        NHTTPi_ThreadReqEnd(&work);
    }
}
