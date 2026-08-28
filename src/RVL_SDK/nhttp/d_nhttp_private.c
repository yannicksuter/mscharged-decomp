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

NHTTPConnectionInfo* NHTTPCreateConnection(const char* url, s32 method,
    void* buffer, u32 bufferSize, NHTTPConnectionCallback callback,
    void* userParam)
{
    void* systemInfo;
    NHTTPBgnEndInfo* bgnEndInfo;
    void* mutexInfo;
    NHTTPConnectionInfo* connection;

    systemInfo = NHTTPi_GetSystemInfoP();
    bgnEndInfo = NHTTPi_GetBgnEndInfoP(systemInfo);
    mutexInfo = NHTTPi_GetMutexInfoP(systemInfo);
    connection = NHTTPi_alloc(sizeof(NHTTPConnectionInfo), 4);
    if (connection == NULL)
    {
        NHTTPi_SetError(bgnEndInfo, NHTTP_ERROR_ALLOC);
        return NULL;
    }

    connection->request = NHTTP_CreateRequest(bgnEndInfo, url, method, buffer, bufferSize, userParam, NULL, NULL);
    if (connection->request == NULL)
    {
        NHTTPi_free(connection);
        return NULL;
    }

    connection->response = connection->request->response;
    connection->started = FALSE;
    connection->callback = callback;
    connection->_unk40 = 0;
    connection->_unk44 = 0;
    connection->requestId = -1;
    NHTTPi_CommitConnectionList(mutexInfo, connection);
    connection->state = NHTTP_ERROR_BUSY;
    OSInitCond(&connection->cond);
    OSInitMutex(&connection->mutex);
    NHTTPi_SetVirtualContentLength(connection, 0);
    connection->requestCallback = NULL;
    return connection;
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
            *buffer = response->recvBuf.buffer;
            *bufferSize = response->recvBuf.bufferSize;
            result = response->recvBuf._unk4;
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
            result = response->userParam;
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
