#ifndef PRIVATE_NHTTP_H
#define PRIVATE_NHTTP_H

#include <revolution/nhttp.h>
#include <revolution/os.h>
#include <revolution/ssl.h>

typedef struct NHTTPBgnEndInfo NHTTPBgnEndInfo;
typedef struct NHTTPRequestInfo NHTTPRequestInfo;
typedef struct NHTTPConnectionInfo NHTTPConnectionInfo;

typedef s32 (*NHTTPConnectionCallback)(NHTTPConnectionInfo* connection,
    s32 state, void* param);

typedef struct NHTTPConnectionCallbackParam
{
    void* value;
    u32 _unk4;
    u32 _unk8;
    u32 _unkC;
} NHTTPConnectionCallbackParam;

typedef struct NHTTPHeader
{
    struct NHTTPHeader* next;
    struct NHTTPHeader* prev;
    char* name;
    char* value;
    u32 length;
    u32 _unk14;
} NHTTPHeader;

typedef struct NHTTPReqQueue
{
    struct NHTTPReqQueue* next;
    struct NHTTPReqQueue* prev;
    s32 requestId;
    NHTTPRequestInfo* request;
    s32 _unk10;
} NHTTPReqQueue;

typedef struct NHTTPListInfo
{
    NHTTPReqQueue* reqQueue;
    s32 nextRequestId;
} NHTTPListInfo;

typedef struct NHTTPReqInfo
{
    NHTTPReqQueue* reqQueue;
} NHTTPReqInfo;

typedef struct NHTTPRecvBufBlock
{
    struct NHTTPRecvBufBlock* next;
    s8 data[0x200];
} NHTTPRecvBufBlock;

typedef struct NHTTPHdrRecvBuf
{
    s32 length;
    s32 _unk4;
    s32 _unk8;
    s32 _unkC;
    BOOL succeeded;
    BOOL hasResultCode;
    s32 resultCode;
    u32 bufferSize;
    void* _unk20;
    void* _unk24;
    void* buffer;
    NHTTPResponseCallback callback;
    NHTTPResponseCleanup cleanup;
    NHTTPRecvBufBlock* blocks;
    s8 data[0x400];
} NHTTPHdrRecvBuf;

typedef struct NHTTPResponseInfo
{
    NHTTPHdrRecvBuf recvBuf;
    void* userParam;
} NHTTPResponseInfo;

struct NHTTPRequestInfo
{
    BOOL cancel;
    s32 state;
    BOOL secure;
    BOOL proxyEnabled;
    void* _unk10;
    s32 hostEnd;
    s32 pathStart;
    s32 method;
    u32 port;
    char* url;
    char* host;
    NHTTPResponseInfo* response;
    NHTTPHeader* headers;
    NHTTPHeader* postData;
    char multipartBoundary[20];
    char authorization[0x5C];
    u32 authorizationLength;
    SSLId sslId;
    const char* clientCertData;
    u32 clientCertSize;
    const char* privateKeyData;
    u32 privateKeySize;
    const char* rootCAData;
    u32 rootCASize;
    BOOL clientCertDefault;
    u32 verifyOption;
    u32 _unkD0;
    char proxyServer[0x100];
    u32 proxyPort;
    char proxyAuthorization[0x5C];
    u32 proxyAuthorizationLength;
    u32 recvBufferSize;
    const void* postBuffer;
    u32 postBufferSize;
    void* _unk244;
};

struct NHTTPConnectionInfo
{
    BOOL started;
    s32 state;
    s32 sslError;
    OSCond cond;
    OSMutex mutex;
    NHTTPRequestInfo* request;
    NHTTPResponseInfo* response;
    s32 requestId;
    NHTTPConnectionCallback callback;
    NHTTPConnectionInfo* next;
    u32 _unk40;
    u32 _unk44;
    u32 virtualContentLength;
    NHTTPReqCallback requestCallback;
};

#endif // PRIVATE_NHTTP_H
