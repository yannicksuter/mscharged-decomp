#include <private/nhttp.h>

void* NHTTPi_GetSystemInfoP(void);
NHTTPBgnEndInfo* NHTTPi_GetBgnEndInfoP(void* systemInfo);
void* NHTTPi_GetMutexInfoP(void* systemInfo);
void* NHTTPi_alloc(u32 size, int align);
void NHTTPi_free(void* buffer);
void NHTTPi_SetError(NHTTPBgnEndInfo* info, NHTTPErr error);
NHTTPRequestInfo* NHTTP_CreateRequest(NHTTPBgnEndInfo* info, const char* url,
    s32 method, void* buffer, u32 bufferSize, void* userParam,
    NHTTPResponseCallback responseCallback, NHTTPResponseCleanup cleanup);
s32 NHTTPi_CommitConnectionList(void* mutexInfo,
    NHTTPConnectionInfo* connection);
void NHTTPi_SetVirtualContentLength(NHTTPConnectionInfo* connection,
    u32 length);
NHTTPConnectionInfo* NHTTPi_GetConnection(void* mutexInfo,
    NHTTPConnectionInfo* connection);
s32 NHTTP_SendRequestAsync(void* systemInfo, NHTTPRequestInfo* request);
NHTTPResponseInfo* NHTTPi_Connection2Response(void* mutexInfo,
    NHTTPConnectionInfo* connection);

NHTTPConnectionInfo* NHTTPCreateConnection(const char* url_p, s32 method,
    void* buf_p, u32 len, NHTTPConnectionCallback callback,
    void* userParam_p)
{
    NHTTPConnectionInfo* connection_p = NULL;
    void* sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    void* mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);

    connection_p = NHTTPi_alloc(sizeof(NHTTPConnectionInfo), 4);
    if (connection_p == NULL)
    {
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        return NULL;
    }

    connection_p->request = NHTTP_CreateRequest(bgnEndInfo_p, url_p, method,
        buf_p, len, userParam_p, NULL, NULL);
    if (connection_p->request == NULL)
    {
        NHTTPi_free(connection_p);
        return NULL;
    }

    connection_p->response = connection_p->request->response;
    connection_p->started = FALSE;
    connection_p->callback = callback;
    connection_p->_unk40 = 0;
    connection_p->_unk44 = 0;
    connection_p->requestId = -1;
    NHTTPi_CommitConnectionList(mutexInfo_p, connection_p);
    connection_p->state = NHTTP_ERROR_BUSY;
    OSInitCond(&connection_p->cond);
    OSInitMutex(&connection_p->mutex);
    NHTTPi_SetVirtualContentLength(connection_p, 0);
    connection_p->requestCallback = NULL;
    return connection_p;
}

s32 NHTTPStartConnection(NHTTPConnectionInfo* handle)
{
    void* systemInfo;
    void* mutexInfo;
    NHTTPConnectionInfo* connection;
    s32 requestId;

    systemInfo = NHTTPi_GetSystemInfoP();
    mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);
    connection = NHTTPi_GetConnection(mutexInfo, handle);
    if (connection == NULL)
    {
        return -1;
    }
    if (connection->request == NULL)
    {
        return -1;
    }

    requestId = NHTTP_SendRequestAsync(systemInfo, connection->request);
    connection->requestId = requestId;
    if (requestId >= 0)
    {
        connection->started = TRUE;
    }
    return 0;
}

s32 NHTTPGetBodyBuffer(NHTTPConnectionInfo* handle, void** buffer,
    u32* bufferSize)
{
    void* mutexInfo;
    NHTTPConnectionInfo* connection;
    NHTTPResponseInfo* response;
    s32 result;

    mutexInfo = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    connection = NHTTPi_GetConnection(mutexInfo, handle);
    if (connection != NULL)
    {
        response = NHTTPi_Connection2Response(mutexInfo, connection);
        if (response != NULL)
        {
            *buffer = response->recvBuf_p;
            *bufferSize = response->recvBufLen;
            result = response->bodyLen;
        }
        else
        {
            result = -1;
        }
    }
    else
    {
        result = -1;
    }

    return result;
}

void* NHTTPGetUserParam(NHTTPConnectionInfo* handle)
{
    void* mutexInfo;
    NHTTPConnectionInfo* connection;
    NHTTPResponseInfo* response;
    void* result;

    mutexInfo = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    connection = NHTTPi_GetConnection(mutexInfo, handle);
    if (connection != NULL)
    {
        response = NHTTPi_Connection2Response(mutexInfo, connection);
        if (response != NULL)
        {
            result = response->param_p;
        }
        else
        {
            result = NULL;
        }
    }
    else
    {
        result = NULL;
    }
    return result;
}

s32 NHTTPGetConnectionError(NHTTPConnectionInfo* handle)
{
    void* mutexInfo;
    NHTTPConnectionInfo* connection;
    s32 result;

    mutexInfo = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    connection = NHTTPi_GetConnection(mutexInfo, handle);
    if (connection != NULL)
    {
        result = connection->state;
    }
    else
    {
        result = -1;
    }
    return result;
}
