#include <private/nhttp.h>

const char NHTTPi_strMultipartBound[] = "--t9Sf4yfjf1RtvDu3AA";

void* NHTTPi_alloc(u32 size, int align);
void NHTTPi_free(void* ptr);
void NHTTPi_SetError(NHTTPBgnEndInfo* info, NHTTPErr error);
void* NHTTPi_memclr(void* destination, u32 size);
s32 NHTTPi_strlen(const char* string);
void* NHTTPi_memcpy(void* destination, const void* source, u32 size);
s32 NHTTPi_strnicmp(const char* left, const char* right, s32 size);
s32 NHTTPi_strToHex(const char* string, s32 length);
s32 NHTTPi_strtonum(const char* string, s32 length);

NHTTPBgnEndInfo* NHTTPi_GetBgnEndInfoP(void* systemInfo);
NHTTPListInfo* NHTTPi_GetListInfoP(void* systemInfo);
NHTTPReqInfo* NHTTPi_GetReqInfoP(void* systemInfo);
void* NHTTPi_GetMutexInfoP(void* systemInfo);
void* NHTTPi_GetThreadInfoP(void* systemInfo);
NHTTPConnectionInfo* NHTTPi_Request2Connection(void* mutexInfo,
    NHTTPRequestInfo* request);
void NHTTPi_lockReqList(void* mutexInfo);
void NHTTPi_unlockReqList(void* mutexInfo);
s32 NHTTPi_setReqQueue(NHTTPListInfo* listInfo, NHTTPRequestInfo* request);
void NHTTPi_kickCommThread(void* threadInfo);
void NHTTPi_SocCancel(void* mutexInfo, NHTTPRequestInfo* request, s32 socket);
BOOL NHTTPi_freeReqQueue(NHTTPListInfo* listInfo, void* mutexInfo,
    s32 requestId);
void NHTTPi_allFreeReqQueue(NHTTPListInfo* listInfo, void* mutexInfo);

void NHTTPi_InitRequestInfo(NHTTPReqInfo* info)
{
    info->reqQueue = NULL;
}

NHTTPRequestInfo* NHTTP_CreateRequest(NHTTPBgnEndInfo* info, const char* url,
    s32 method, void* buffer, u32 bufferSize, void* userParam,
    NHTTPResponseCallback responseCallback, NHTTPResponseCleanup cleanup)
{
    char urlBuffer[0x100];
    NHTTPRequestInfo* request = NULL;
    s32 urlLength;
    s32 prefixLength;
    s32 remaining;
    char* hostStart;
    char* current;
    s32 index;
    s32 decodedLength;
    char* decodedHost;
    s32 escapeCount;
    s32 escapeRemaining;
    s32 decodedChar;
    BOOL foundPath;
    s32 hostLength;
    s32 separator;
    s32 portStart;
    s32 port;

    if (method >= 3 || method < 0)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_UNKNOWN);
        goto error;
    }

    request = NHTTPi_alloc(sizeof(NHTTPRequestInfo), 4);
    if (request == NULL)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_ALLOC);
        goto error;
    }
    NHTTPi_memclr(request, sizeof(NHTTPRequestInfo));

    request->response = NHTTPi_alloc(sizeof(NHTTPResponseInfo), 4);
    if (request->response == NULL)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_ALLOC);
        goto error;
    }
    NHTTPi_memclr(request->response, sizeof(NHTTPResponseInfo));
    request->response->recvBuf.buffer = buffer;
    request->response->recvBuf.bufferSize = bufferSize;
    request->response->recvBuf.callback = responseCallback;
    request->response->recvBuf.cleanup = cleanup;

    urlLength = NHTTPi_strlen(url);
    if (urlLength <= 7)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_DNS);
        goto error;
    }

    NHTTPi_memclr(urlBuffer, urlLength);
    NHTTPi_memcpy(urlBuffer, url, urlLength);

    request->port = 80;
    prefixLength = 7;
    if (NHTTPi_strnicmp(urlBuffer, "http://", 7) != 0)
    {
        if (NHTTPi_strnicmp(urlBuffer, "https://", 8) != 0)
        {
            NHTTPi_SetError(info, NHTTP_ERROR_DNS);
            goto error;
        }
        request->secure = TRUE;
        prefixLength = 8;
        request->port = 443;
    }

    remaining = urlLength - prefixLength;
    hostStart = &urlBuffer[prefixLength];
    if (remaining <= 0)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_DNS);
        goto error;
    }

    current = hostStart;
    index = 0;
    escapeCount = 0;
    escapeRemaining = 0;
    while (index < remaining && *current != '/')
    {
        if (escapeRemaining == 2)
        {
            escapeRemaining--;
        }
        else if (escapeRemaining == 1)
        {
            decodedChar = NHTTPi_strToHex(&hostStart[index - 1], 2);
            escapeRemaining--;
            if ((s8)decodedChar < 0)
            {
                NHTTPi_SetError(info, NHTTP_ERROR_DNS);
                goto error;
            }
            if ((s8)decodedChar == '/')
            {
                escapeCount--;
                break;
            }
        }
        else if (*current == '%')
        {
            escapeRemaining = 2;
            escapeCount++;
        }
        index++;
        current++;
    }

    if (escapeRemaining != 0)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_DNS);
        goto error;
    }

    decodedLength = prefixLength + remaining - escapeCount * 2 + 1;
    request->url = NHTTPi_alloc(decodedLength, 4);
    if (request->url == NULL)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_ALLOC);
        goto error;
    }
    NHTTPi_memclr(request->url, decodedLength);
    NHTTPi_memcpy(request->url, urlBuffer, prefixLength);

    current = hostStart;
    index = 0;
    separator = 0;
    escapeRemaining = 0;
    foundPath = FALSE;
    while (index < remaining)
    {
        if (escapeRemaining == 2)
        {
            escapeRemaining--;
        }
        else if (escapeRemaining == 1)
        {
            decodedChar = NHTTPi_strToHex(&hostStart[index - 1], 2);
            request->url[prefixLength + separator - 1] = decodedChar;
            escapeRemaining--;
            if ((s8)decodedChar == '/')
            {
                foundPath = TRUE;
            }
        }
        else
        {
            if (*current == '/')
            {
                foundPath = TRUE;
            }
            if ((!foundPath) & (*current == '%'))
            {
                escapeRemaining = 2;
            }
            else
            {
                request->url[prefixLength + separator] = *current;
            }
            separator++;
        }
        index++;
        current++;
    }
    request->url[prefixLength + separator] = '\0';

    index = 0;
    decodedHost = request->url + prefixLength;
    current = decodedHost;
    while (index < separator)
    {
        if (*current == '/' || *current == ':')
        {
            request->hostEnd = prefixLength + index;
            break;
        }
        index++;
        current++;
    }

    if (index == separator)
    {
        request->hostEnd = prefixLength + index;
        request->pathStart = prefixLength + index;
    }
    else if (decodedHost[index] == '/')
    {
        request->pathStart = request->hostEnd;
    }
    else if (decodedHost[index] == ':')
    {
        current = decodedHost + index;
        while (index < separator)
        {
            if (*current == '/')
            {
                request->pathStart = prefixLength + index;
                break;
            }
            index++;
            current++;
        }

        if (index == separator)
        {
            request->pathStart = prefixLength + index;
        }
        else
        {
            portStart = request->hostEnd + 1;
            port = NHTTPi_strtonum(request->url + portStart,
                request->pathStart - portStart);
            if (port < 0)
            {
                port = request->port;
            }
            else if (port > 0xFFFF)
            {
                NHTTPi_SetError(info, NHTTP_ERROR_DNS);
                goto error;
            }
            request->port = (u16)port;
        }
    }

    hostLength = request->hostEnd - (7 + (request->secure != FALSE));
    request->host = NHTTPi_alloc(hostLength + 1, 4);
    if (request->host == NULL)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_ALLOC);
        goto error;
    }
    NHTTPi_memclr(request->host, hostLength + 1);
    NHTTPi_memcpy(request->host,
        request->url + 7 + (request->secure != FALSE),
        hostLength);
    NHTTPi_memcpy(request->multipartBoundary, NHTTPi_strMultipartBound, sizeof(request->multipartBoundary));

    request->method = method;
    request->sslId = 0;
    request->clientCertData = NULL;
    request->clientCertSize = 0;
    request->privateKeyData = NULL;
    request->privateKeySize = 0;
    request->rootCAData = NULL;
    request->rootCASize = 0;
    request->clientCertDefault = FALSE;
    request->verifyOption = 0;
    request->response->userParam = userParam;
    request->proxyEnabled = FALSE;
    request->recvBufferSize = 0;
    return request;

error:
    if (request != NULL)
    {
        if (request->url != NULL)
        {
            NHTTPi_free(request->url);
        }
        if (request->host != NULL)
        {
            NHTTPi_free(request->host);
        }
        if (request->response != NULL)
        {
            NHTTPi_free(request->response);
        }
        NHTTPi_free(request);
    }
    return NULL;
}

void NHTTP_DestroyRequest(void* systemInfo, NHTTPRequestInfo* request)
{
    NHTTPConnectionInfo* connection;
    NHTTPHeader* header;
    NHTTPHeader* next;
    NHTTPHeader* post;
    NHTTPHeader* nextPost;
    void* mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);

    connection = NHTTPi_Request2Connection(mutexInfo, request);
    if (connection != NULL)
    {
        connection->response = NULL;
    }
    NHTTPi_free(request->response);

    connection = NHTTPi_Request2Connection(mutexInfo, request);
    if (connection != NULL)
    {
        connection->request = NULL;
    }

    header = request->headers;
    while (header != NULL)
    {
        if (header != header->next)
        {
            next = header->next->next;
            NHTTPi_free(header->next);
            header->next = next;
        }
        else
        {
            NHTTPi_free(header);
            header = NULL;
        }
    }

    post = request->postData;
    while (post != NULL)
    {
        if (post != post->next)
        {
            nextPost = post->next->next;
            NHTTPi_free(post->next);
            post->next = nextPost;
        }
        else
        {
            NHTTPi_free(post);
            post = NULL;
        }
    }

    NHTTPi_free(request->url);
    NHTTPi_free(request->host);
    NHTTPi_free(request);
}

BOOL NHTTPi_destroyRequestObject(void* mutexInfo, NHTTPRequestInfo* request)
{
    NHTTPConnectionInfo* connection;
    NHTTPHeader* header;
    NHTTPHeader* next;
    NHTTPHeader* post;
    NHTTPHeader* nextPost;

    connection = NHTTPi_Request2Connection(mutexInfo, request);
    if (connection != NULL)
    {
        connection->request = NULL;
    }

    header = request->headers;
    while (header != NULL)
    {
        if (header != header->next)
        {
            next = header->next->next;
            NHTTPi_free(header->next);
            header->next = next;
        }
        else
        {
            NHTTPi_free(header);
            header = NULL;
        }
    }

    post = request->postData;
    while (post != NULL)
    {
        if (post != post->next)
        {
            nextPost = post->next->next;
            NHTTPi_free(post->next);
            post->next = nextPost;
        }
        else
        {
            NHTTPi_free(post);
            post = NULL;
        }
    }

    NHTTPi_free(request->url);
    NHTTPi_free(request->host);
    NHTTPi_free(request);
    return TRUE;
}

s32 NHTTP_SendRequestAsync(void* systemInfo, NHTTPRequestInfo* request)
{
    NHTTPBgnEndInfo* info = NHTTPi_GetBgnEndInfoP(systemInfo);
    void* threadInfo = NHTTPi_GetThreadInfoP(systemInfo);
    void* mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);
    s32 requestId;

    if (request->state != 0)
    {
        NHTTPi_SetError(info, NHTTP_ERROR_UNKNOWN);
        return -1;
    }

    NHTTPi_lockReqList(mutexInfo);
    requestId = NHTTPi_setReqQueue(NHTTPi_GetListInfoP(systemInfo), request);
    if (requestId >= 0)
    {
        request->state = 1;
        NHTTPi_kickCommThread(threadInfo);
    }
    else
    {
        NHTTPi_SetError(info, NHTTP_ERROR_ALLOC);
    }
    NHTTPi_unlockReqList(mutexInfo);
    return requestId;
}

BOOL NHTTP_CancelRequestAsync(void* systemInfo, s32 requestId)
{
    BOOL result = FALSE;
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(systemInfo);
    void* mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);
    NHTTPReqQueue* queue = reqInfo->reqQueue;

    NHTTPi_lockReqList(mutexInfo);
    if (queue != NULL && queue->requestId == requestId
        && queue->request->cancel == 0)
    {
        queue->request->cancel = 1;
        NHTTPi_SocCancel(mutexInfo, queue->request, queue->_unk10);
        result = TRUE;
    }

    if (!result)
    {
        result = NHTTPi_freeReqQueue(
            NHTTPi_GetListInfoP(systemInfo), mutexInfo, requestId);
    }
    NHTTPi_unlockReqList(mutexInfo);
    return result;
}

void NHTTPi_cancelAllRequests(void* systemInfo)
{
    NHTTPReqInfo* reqInfo = NHTTPi_GetReqInfoP(systemInfo);
    NHTTPListInfo* listInfo = NHTTPi_GetListInfoP(systemInfo);
    void* mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);
    NHTTPReqQueue* queue = reqInfo->reqQueue;

    NHTTPi_lockReqList(mutexInfo);
    if (queue != NULL && queue->request->cancel == 0)
    {
        queue->request->cancel = 1;
        NHTTPi_SocCancel(mutexInfo, queue->request, queue->_unk10);
    }
    NHTTPi_allFreeReqQueue(listInfo, mutexInfo);
    NHTTPi_unlockReqList(mutexInfo);
}
