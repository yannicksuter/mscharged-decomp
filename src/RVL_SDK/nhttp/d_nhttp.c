#include <private/nhttp.h>

#include <revolution/ncd.h>

#include <string.h>

struct NHTTPBgnEndInfo
{
    NCDIpConfig ipConfig;
};

typedef s32 (*NHTTPPostSend)(const char* label, char** value_p,
    u32* length_p, s32 offset, void* userParam);

void* NHTTPi_GetSystemInfoP(void);
NHTTPBgnEndInfo* NHTTPi_GetBgnEndInfoP(void* systemInfo);
void* NHTTPi_GetMutexInfoP(void* systemInfo);
BOOL NHTTPi_Startup(void* systemInfo, NHTTPAlloc alloc, NHTTPFree free,
    u32 priority);
void NHTTPi_CleanupAsync(void* systemInfo, NHTTPCleanupCallback callback);
s32 NHTTPi_GetSSLError(NHTTPBgnEndInfo* info);
void* NHTTPi_alloc(u32 size, int align);
void NHTTPi_free(void* buffer);
s32 NHTTPi_strlen(const char* string);
void* NHTTPi_memcpy(void* destination, const void* source, u32 size);
void* NHTTPi_memclr(void* destination, u32 size);
s32 NHTTPi_Base64Encode(char* destination, const char* source);
NHTTPConnectionInfo* NHTTPCreateConnection(const char* url, s32 method,
    void* buffer, u32 bufferSize, NHTTPConnectionCallback callback,
    void* userParam);
s32 NHTTPStartConnection(NHTTPConnectionInfo* connection);
s32 NHTTPGetBodyBuffer(NHTTPConnectionInfo* connection, void** buffer,
    u32* bufferSize);
void* NHTTPGetUserParam(NHTTPConnectionInfo* connection);
s32 NHTTPGetConnectionError(NHTTPConnectionInfo* connection);
NHTTPConnectionInfo* NHTTPi_GetConnection(void* mutexInfo,
    NHTTPConnectionInfo* connection);
NHTTPRequestInfo* NHTTPi_Connection2Request(void* mutexInfo,
    NHTTPConnectionInfo* connection);
NHTTPResponseInfo* NHTTPi_Connection2Response(void* mutexInfo,
    NHTTPConnectionInfo* connection);
NHTTPConnectionInfo* NHTTPi_Request2Connection(void* mutexInfo,
    NHTTPRequestInfo* request);
NHTTPConnectionInfo* NHTTPi_Response2Connection(void* mutexInfo,
    NHTTPResponseInfo* response);
NHTTPRequestInfo* NHTTPi_GetRequest(void* mutexInfo,
    NHTTPRequestInfo* request);
NHTTPResponseInfo* NHTTPi_GetResponse(void* mutexInfo,
    NHTTPResponseInfo* response);
s32 NHTTPi_OmitConnectionList(void* mutexInfo,
    NHTTPConnectionInfo* connection);
u32 NHTTPi_GetVirtualContentLength(NHTTPConnectionInfo* connection);
BOOL NHTTP_AddHeaderField(NHTTPRequestInfo* request, NHTTPBgnEndInfo* info,
    const char* name, const char* value);
BOOL NHTTP_AddPostDataAscii(NHTTPRequestInfo* request, NHTTPBgnEndInfo* info,
    const char* name, const char* value);
BOOL NHTTP_CancelRequestAsync(void* systemInfo, s32 requestId);
void NHTTP_DestroyRequest(void* systemInfo, NHTTPRequestInfo* request);
void NHTTP_DestroyResponse(void* mutexInfo, NHTTPResponseInfo* response);

s32 NHTTPStartup(NHTTPAlloc alloc, NHTTPFree free, u32 priority)
{
    return NHTTPi_Startup(NHTTPi_GetSystemInfoP(), alloc, free, priority)
             ? 0
             : -1;
}

void NHTTPCleanupAsync(NHTTPCleanupCallback callback)
{
    NHTTPi_CleanupAsync(NHTTPi_GetSystemInfoP(), callback);
}

s32 NHTTPSSLGetError(void)
{
    return NHTTPi_GetSSLError(NHTTPi_GetSystemInfoP());
}

static s32 NHTTPi_PostSendCallbackWrap(void* mutexInfo_p,
    NHTTPConnectionInfo* connection_p, void* arg_p)
{
    if (connection_p != NULL)
    {
        NHTTPRequestInfo* req_p =
            NHTTPi_Connection2Request(mutexInfo_p, connection_p);
        if (req_p != NULL)
        {
            NHTTPPostSend postSend = (NHTTPPostSend)req_p->_unk244;
            if (postSend != NULL)
            {
                NHTTPPostSendArg* postSendArg_p = arg_p;
                return postSend(postSendArg_p->label,
                    &postSendArg_p->buf, &postSendArg_p->size,
                    postSendArg_p->offset,
                    NHTTPGetUserParam(connection_p));
            }
        }
    }
    return -1;
}

static s32 NHTTPi_BufFullCallbackWrap(void* mutexInfo_p,
    NHTTPConnectionInfo* connection_p, void* arg_p)
{
    if (connection_p != NULL)
    {
        NHTTPResponseInfo* res_p =
            NHTTPi_Connection2Response(mutexInfo_p, connection_p);
        if (res_p != NULL)
        {
            NHTTPResponseCallback bufFull = res_p->bufFull;
            if (bufFull != NULL)
            {
                NHTTPBodyBufArg* bodyBufArg_p = arg_p;
                char* oldBuf_p = bodyBufArg_p->buf;
                u32 contentLength =
                    NHTTPi_GetVirtualContentLength(connection_p);
                bodyBufArg_p->buf = bufFull((void**)&oldBuf_p,
                    &bodyBufArg_p->size, contentLength, NHTTPi_alloc,
                    NHTTPi_free, NHTTPGetUserParam(connection_p));
                if (bodyBufArg_p->buf != NULL && oldBuf_p != NULL)
                {
                    bodyBufArg_p->offset = 0;
                }
            }
        }
    }
    return 0;
}

static s32 NHTTPi_CompleteCallbackWrap(void* mutexInfo_p,
    NHTTPConnectionInfo* connection_p)
{
    if (connection_p != NULL)
    {
        NHTTPReqCallback reqCallback = connection_p->requestCallback;
        if (reqCallback != NULL)
        {
            NHTTPResponseInfo* res_p =
                NHTTPi_Connection2Response(mutexInfo_p, connection_p);
            if (res_p != NULL)
            {
                NHTTPErr result = NHTTPGetConnectionError(connection_p);
                reqCallback(
                    result, res_p, NHTTPGetUserParam(connection_p));
            }
        }
    }
    return 0;
}

s32 NHTTPi_TemplateConnectionCallback(NHTTPConnectionInfo* handle,
    NHTTPConnectionEvent event, void* arg_p)
{
    void* sysInfo_p = NHTTPi_GetSystemInfoP();
    void* mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnectionInfo* connection_p =
        NHTTPi_GetConnection(mutexInfo_p, handle);
    s32 ret = 0;

    switch (event)
    {
    case NHTTP_EVENT_POST_SEND:
        ret = NHTTPi_PostSendCallbackWrap(
            mutexInfo_p, connection_p, arg_p);
        break;
    case NHTTP_EVENT_BODY_RECV_FULL:
        ret = NHTTPi_BufFullCallbackWrap(
            mutexInfo_p, connection_p, arg_p);
        break;
    case NHTTP_EVENT_BODY_RECV_DONE:
        break;
    case NHTTP_EVENT_COMPLETE:
        ret = NHTTPi_CompleteCallbackWrap(mutexInfo_p, connection_p);
        break;
    default:
        break;
    }
    return ret;
}

NHTTPRequest* NHTTPCreateRequest(const char* url, NHTTPReqMethod method,
    void* buffer, u32 bufferSize, NHTTPReqCallback callback, void* userParam)
{
    return NHTTPCreateRequestEx(url, method, buffer, bufferSize, callback, userParam, NULL, NULL);
}

NHTTPRequest* NHTTPCreateRequestEx(const char* url, NHTTPReqMethod method,
    void* buffer, u32 bufferSize, NHTTPReqCallback callback, void* userParam,
    NHTTPResponseCallback responseCallback, NHTTPResponseCleanup cleanup)
{
    void* systemInfo;
    void* mutexInfo;
    NHTTPConnectionInfo* connection;
    NHTTPRequestInfo* request;

    connection = NHTTPCreateConnection(url, method, buffer, bufferSize, NHTTPi_TemplateConnectionCallback, userParam);
    request = NULL;
    if (connection != NULL)
    {
        systemInfo = NHTTPi_GetSystemInfoP();
        mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);
        connection = NHTTPi_GetConnection(mutexInfo, connection);
        request = NHTTPi_Connection2Request(mutexInfo, connection);
        if (request != NULL)
        {
            if (request->response != NULL)
            {
                connection->requestCallback = callback;
                request->response->bufFull = responseCallback;
                request->response->freeBuf = cleanup;
                return request;
            }
            else
            {
                NHTTP_DestroyRequest(systemInfo, request);
                NHTTPi_OmitConnectionList(mutexInfo, connection);
                NHTTPi_free(connection);
                request = NULL;
            }
        }
    }
    return request;
}

s32 NHTTPAddHeaderField(NHTTPRequest* handle, const char* name,
    const char* value)
{
    void* systemInfo;
    NHTTPRequestInfo* request;
    s32 result;

    systemInfo = NHTTPi_GetSystemInfoP();
    request = NHTTPi_GetRequest(NHTTPi_GetMutexInfoP(systemInfo), handle);
    if (request != NULL)
    {
        result = NHTTP_AddHeaderField(request,
                     NHTTPi_GetBgnEndInfoP(systemInfo),
                     name,
                     value)
                   ? 0
                   : -1;
    }
    else
    {
        result = -1;
    }
    return result;
}

s32 NHTTPAddPostDataAscii(NHTTPRequest* handle, const char* name,
    const char* value)
{
    void* systemInfo;
    NHTTPRequestInfo* request;
    s32 result;

    systemInfo = NHTTPi_GetSystemInfoP();
    request = NHTTPi_GetRequest(NHTTPi_GetMutexInfoP(systemInfo), handle);
    if (request != NULL)
    {
        result = NHTTP_AddPostDataAscii(request,
                     NHTTPi_GetBgnEndInfoP(systemInfo),
                     name,
                     value)
                   ? 0
                   : -1;
    }
    else
    {
        result = -1;
    }
    return result;
}

NHTTPConnection NHTTPSendRequestAsync(NHTTPRequest* request)
{
    NHTTPConnectionInfo* connection;
    s32 result;

    connection = NHTTPi_Request2Connection(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), request);
    if (connection != NULL && NHTTPStartConnection(connection) == 0)
    {
        result = connection->requestId;
    }
    else
    {
        result = -1;
    }
    return result;
}

s32 NHTTPCancelRequestAsync(NHTTPConnection connection)
{
    return NHTTP_CancelRequestAsync(NHTTPi_GetSystemInfoP(), connection)
             ? 0
             : -1;
}

void NHTTPDestroyResponse(NHTTPResponse* handle)
{
    void* mutexInfo;
    NHTTPConnectionInfo* connection;

    mutexInfo = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    connection = NHTTPi_Response2Connection(mutexInfo, handle);
    if (connection != NULL)
    {
        if (NHTTPi_Connection2Response(mutexInfo, connection) != NULL)
        {
            NHTTP_DestroyResponse(mutexInfo, connection->response);
        }
        NHTTPi_OmitConnectionList(mutexInfo, connection);
        NHTTPi_free(connection);
    }
}

s32 NHTTPGetBodyAll(NHTTPResponse* response, void** buffer)
{
    NHTTPConnectionInfo* connection;
    u32 bufferSize;
    s32 result;

    connection = NHTTPi_Response2Connection(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), response);
    if (connection != NULL)
    {
        bufferSize = 0;
        result = NHTTPGetBodyBuffer(connection, buffer, &bufferSize);
    }
    else
    {
        result = -1;
    }
    return result;
}

s32 NHTTPGetResultCode(NHTTPResponse* handle)
{
    NHTTPResponseInfo* response;
    response = NHTTPi_GetResponse(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), handle);
    if (response == NULL)
        return -1;
    if (response->isHeaderParse != FALSE)
        return response->httpStatus;
    return -1;
}

s32 NHTTPSetVerifyOption(NHTTPRequest* handle, u32 option)
{
    NHTTPRequestInfo* request;
    s32 result;

    request = NHTTPi_GetRequest(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), handle);
    if (request == NULL)
    {
        result = -1;
    }
    else
    {
        request->verifyOption = option;
        result = 0;
    }
    return result;
}

s32 NHTTPSetProxy(NHTTPRequest* handle, const char* proxy_p, u16 port,
    const char* username_p, const char* password_p)
{
    void* sysInfo_p = NHTTPi_GetSystemInfoP();
    void* mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPRequestInfo* req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    s32 proxyLen = 0;

    if (req_p == NULL || proxy_p == NULL)
    {
        return -1;
    }

    proxyLen = NHTTPi_strlen(proxy_p);
    if (proxyLen > 0x100)
    {
        return -1;
    }
    NHTTPi_memcpy(req_p->proxyServer, proxy_p, proxyLen);
    req_p->proxyPort = port;

    if (username_p != NULL && password_p != NULL)
    {
        s32 username_len = 0;
        s32 password_len = 0;

        username_len = NHTTPi_strlen(username_p);
        password_len = NHTTPi_strlen(password_p);
        if (username_len <= 0x20 && password_len <= 0x20)
        {
            char tmpbuf[0x41];
            NHTTPi_memclr(tmpbuf, sizeof(tmpbuf));
            NHTTPi_memcpy(tmpbuf, username_p, username_len);
            NHTTPi_memcpy(&(tmpbuf[username_len]), ":", 1);
            NHTTPi_memcpy(
                &(tmpbuf[(username_len + 1)]), password_p, password_len);
            req_p->proxyAuthorizationLength =
                NHTTPi_Base64Encode(req_p->proxyAuthorization, tmpbuf);
        }
        else
        {
            return -1;
        }
    }

    req_p->proxyEnabled = TRUE;
    return 0;
}

s32 NHTTPSetProxyDefault(NHTTPRequest* handle)
{
    NCDProxyServerProfile* proxy;
    void* systemInfo;
    NHTTPBgnEndInfo* bgnEndInfo;
    NHTTPRequestInfo* request;
    const char* username;
    const char* password;
    s32 result;

    systemInfo = NHTTPi_GetSystemInfoP();
    request = NHTTPi_GetRequest(NHTTPi_GetMutexInfoP(systemInfo), handle);
    bgnEndInfo = NHTTPi_GetBgnEndInfoP(systemInfo);
    if (request->secure != FALSE)
    {
        proxy = &bgnEndInfo->ipConfig.proxy.ssl;
    }
    else
    {
        proxy = &bgnEndInfo->ipConfig.proxy.http;
    }

    if (proxy->mode == 1 && proxy->port != 0 && strlen(proxy->server) >= 6)
    {
        username = NULL;
        password = NULL;
        if (proxy->authType == 1)
        {
            username = proxy->username;
            password = proxy->password;
        }
        OSReport("Using proxy server %s:%d (%s/%s).\n", proxy->server, proxy->port, username != NULL ? username : "[no-auth]", password != NULL ? password : "[no-auth]");
        result = NHTTPSetProxy(request, proxy->server, proxy->port, username, password);
        if (result < 0)
        {
            OSReport("NHTTPSetProxy failed.(%d)\n", result);
            OSPanic("d_nhttp.c", 0x346, "NHTTPSetProxy");
        }
        else
        {
            return 0;
        }
    }
    return -1;
}

s32 NHTTPSetClientCertDefault(NHTTPRequest* handle)
{
    NHTTPRequestInfo* request;

    request = NHTTPi_GetRequest(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), handle);
    if (request == NULL)
    {
        return -1;
    }
    request->clientCertDefault = TRUE;
    request->clientCertData = NULL;
    request->clientCertSize = 0;
    request->privateKeyData = NULL;
    request->privateKeySize = 0;
    return 0;
}

s32 NHTTPSetRootCADefault(NHTTPRequest* handle)
{
    NHTTPRequestInfo* request;

    request = NHTTPi_GetRequest(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), handle);
    if (request == NULL)
    {
        return -1;
    }
    request->rootCAData = NULL;
    request->rootCASize = 0;
    return 0;
}
