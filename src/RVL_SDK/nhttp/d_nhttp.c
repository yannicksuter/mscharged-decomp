#include <private/nhttp.h>

#include <revolution/ncd.h>

#include <string.h>

struct NHTTPBgnEndInfo
{
    NCDIpConfig ipConfig;
};

typedef s32 (*NHTTPProgressCallback)(void* value, u32* arg4, u32* arg8,
    u32 argC, void* userParam);

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

s32 NHTTPi_TemplateConnectionCallback(NHTTPConnectionInfo* handle, s32 state,
    void* param)
{
    void* mutexInfo;
    NHTTPRequestInfo* request;
    NHTTPResponseInfo* response;
    NHTTPConnectionCallbackParam* callbackParam;
    s32 result;

    callbackParam = param;
    mutexInfo = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    handle = NHTTPi_GetConnection(mutexInfo, handle);
    result = 0;

    switch (state)
    {
    case 1:
    {
        NHTTPProgressCallback callback;
        s32 callbackResult;

        if (handle != NULL)
        {
            request = NHTTPi_Connection2Request(mutexInfo, handle);
            if (request == NULL)
                goto progress_error;
            callback = (NHTTPProgressCallback)request->_unk244;
            if (callback == NULL)
                goto progress_error;
            callbackResult = callback(callbackParam->value,
                &callbackParam->_unk4,
                &callbackParam->_unk8,
                callbackParam->_unkC,
                NHTTPGetUserParam(handle));
            goto progress_done;
        }
    progress_error:
        callbackResult = -1;
    progress_done:
        result = callbackResult;
        break;
    }
    case 2:
    {
        NHTTPResponseCallback callback;
        void* buffer;
        u32 contentLength;

        if (handle != NULL)
        {
            response = NHTTPi_Connection2Response(mutexInfo, handle);
            if (response != NULL)
            {
                callback = response->recvBuf.callback;
                if (callback != NULL)
                {
                    buffer = callbackParam->value;
                    contentLength = NHTTPi_GetVirtualContentLength(handle);
                    callbackParam->value = callback(&buffer,
                        &callbackParam->_unk4,
                        contentLength,
                        NHTTPi_alloc,
                        NHTTPi_free,
                        NHTTPGetUserParam(handle));
                    if (callbackParam->value != NULL && buffer != NULL)
                    {
                        callbackParam->_unk8 = 0;
                    }
                }
            }
        }
        result = 0;
        break;
    }
    case 4:
    {
        NHTTPReqCallback callback;
        void* userParam;
        s32 error;

        if (handle != NULL)
        {
            callback = handle->requestCallback;
            if (callback != NULL)
            {
                response = NHTTPi_Connection2Response(mutexInfo, handle);
                if (response != NULL)
                {
                    error = NHTTPGetConnectionError(handle);
                    userParam = NHTTPGetUserParam(handle);
                    callback((NHTTPErr)error, response, userParam);
                }
            }
        }
        result = 0;
        break;
    }
    }

    return result;
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
                request->response->recvBuf.callback = responseCallback;
                request->response->recvBuf.cleanup = cleanup;
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
    if (response->recvBuf.hasResultCode != FALSE)
        return response->recvBuf.resultCode;
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

s32 NHTTPSetProxy(NHTTPRequest* handle, const char* server, u16 port,
    const char* username, const char* password)
{
    NHTTPRequestInfo* request;
    char authorization[0x41];
    s32 serverLength;
    s32 usernameLength;
    s32 passwordLength;

    request = NHTTPi_GetRequest(
        NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), handle);
    if (request == NULL || server == NULL)
    {
        return -1;
    }

    serverLength = NHTTPi_strlen(server);
    if (serverLength > 0x100)
    {
        return -1;
    }
    NHTTPi_memcpy(request->proxyServer, server, serverLength);
    request->proxyPort = port;

    if (username != NULL && password != NULL)
    {
        usernameLength = NHTTPi_strlen(username);
        passwordLength = NHTTPi_strlen(password);
        if (usernameLength > 0x20)
            goto authorization_error;
        if (passwordLength > 0x20)
            goto authorization_error;
        NHTTPi_memclr(authorization, sizeof(authorization));
        NHTTPi_memcpy(authorization, username, usernameLength);
        NHTTPi_memcpy(authorization + usernameLength, ":", 1);
        NHTTPi_memcpy(authorization + usernameLength + 1, password, passwordLength);
        request->proxyAuthorizationLength = NHTTPi_Base64Encode(
            request->proxyAuthorization, authorization);
    }
    goto proxy_ready;

authorization_error:
    return -1;

proxy_ready:
    request->proxyEnabled = TRUE;
    return 0;
}

s32 NHTTPSetProxyDefault(NHTTPRequest* handle)
{
    void* systemInfo;
    NHTTPBgnEndInfo* bgnEndInfo;
    NHTTPRequestInfo* request;
    NCDProxyServerProfile* proxy;
    const char* username;
    const char* password;
    s32 result;

    systemInfo = NHTTPi_GetSystemInfoP();
    request = NHTTPi_GetRequest(NHTTPi_GetMutexInfoP(systemInfo), handle);
    bgnEndInfo = NHTTPi_GetBgnEndInfoP(systemInfo);
    proxy = request->secure != FALSE
              ? &bgnEndInfo->ipConfig.proxy.ssl
              : &bgnEndInfo->ipConfig.proxy.http;

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
