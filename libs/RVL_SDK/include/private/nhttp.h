#ifndef PRIVATE_NHTTP_H
#define PRIVATE_NHTTP_H

#include <revolution/nhttp.h>
#include <revolution/os.h>
#include <revolution/ssl.h>

#define STR_POSTBOUND "--t9Sf4yfjf1RtvDu3AA"
#define LEN_POSTBOUND 18
#define LEN_URLBUF 256
#define MIN(AA, BB) (((AA) > (BB)) ? (BB) : (AA))

typedef struct NHTTPBgnEndInfo NHTTPBgnEndInfo;
typedef struct NHTTPRequestInfo NHTTPRequestInfo;
typedef struct NHTTPConnectionInfo NHTTPConnectionInfo;

typedef enum NHTTPConnectionEvent
{
    NHTTP_EVENT_NONE,
    NHTTP_EVENT_POST_SEND,
    NHTTP_EVENT_BODY_RECV_FULL,
    NHTTP_EVENT_BODY_RECV_DONE,
    NHTTP_EVENT_COMPLETE,
    NHTTP_CALLBACK_EVENT_MAX,
} NHTTPConnectionEvent;

typedef s32 (*NHTTPConnectionCallback)(NHTTPConnectionInfo* connection,
    NHTTPConnectionEvent event, void* arg);

typedef struct NHTTPConnectionCallbackParam
{
    void* value;
    u32 _unk4;
    u32 _unk8;
    u32 _unkC;
} NHTTPConnectionCallbackParam;

typedef struct NHTTPPostSendArg
{
    const char* label;
    char* buf;
    u32 size;
    u32 offset;
} NHTTPPostSendArg;

typedef struct NHTTPBodyBufArg
{
    char* buf;
    u32 size;
    u32 offset;
} NHTTPBodyBufArg;

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

typedef struct NHTTPi_HDRBUFLIST
{
    struct NHTTPi_HDRBUFLIST* next_p;
    u8 block[NHTTP_HDRRECVBUF_BLOCKLEN];
} NHTTPi_HDRBUFLIST;

typedef struct NHTTPResponseInfo
{
    volatile s32 headerLen;
    volatile s32 bodyLen;
    volatile s32 totalBodyLen;
    volatile s32 contentLength;
    s32 isSuccess;
    s32 isHeaderParse;
    s32 httpStatus;
    u32 recvBufLen;
    char* allHeader_p;
    char* foundHeader_p;
    char* recvBuf_p;
    NHTTPResponseCallback bufFull;
    NHTTPResponseCleanup freeBuf;
    NHTTPi_HDRBUFLIST* hdrBufBlock_p;
    u8 hdrBufFirst[NHTTP_HDRRECVBUF_INILEN];
    void* param_p;
} NHTTPResponseInfo;

struct NHTTPRequestInfo
{
    BOOL cancel;
    s32 state;
    BOOL secure;
    BOOL proxyEnabled;
    BOOL isRawData;
    s32 hostEnd;
    s32 pathStart;
    s32 method;
    u32 port;
    char* url;
    char* host;
    NHTTPResponseInfo* response;
    NHTTPHeader* headers;
    NHTTPHeader* postData;
    char multipartBoundary[LEN_POSTBOUND + 2];
    char authorization[0x5C];
    s32 authorizationLength;
    SSLId sslId;
    const char* clientCertData;
    u32 clientCertSize;
    const char* privateKeyData;
    u32 privateKeySize;
    const char* rootCAData;
    u32 rootCASize;
    BOOL clientCertDefault;
    u32 verifyOption;
    NHTTPEncodingType encodingType;
    char proxyServer[0x100];
    u32 proxyPort;
    char proxyAuthorization[0x5C];
    s32 proxyAuthorizationLength;
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
