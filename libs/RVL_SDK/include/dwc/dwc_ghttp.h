#pragma once

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DWC_GHTTP_FALSE (-1)
#define DWC_GHTTP_IN_ERROR 0

#define DWC_GHTTP_SOCKET_INIT 0
#define DWC_GHTTP_HOST_LOOKUP 1
#define DWC_GHTTP_LOOKUP_PENDING 2
#define DWC_GHTTP_CONNECTING 3
#define DWC_GHTTP_SECURING_SESSION 4
#define DWC_GHTTP_SENDING_REQUEST 5
#define DWC_GHTTP_POSTING 6
#define DWC_GHTTP_WAITING 7
#define DWC_GHTTP_RECEIVING_STATUS 8
#define DWC_GHTTP_RECEIVING_HEADERS 9
#define DWC_GHTTP_RECEIVING_FILE 10

#define DWC_GHTTP_FAILED_TO_OPEN_FILE (-7)
#define DWC_GHTTP_INVALID_POST (-6)
#define DWC_GHTTP_INSUFFICIENT_MEMORY (-5)
#define DWC_GHTTP_INVALID_FILE_NAME (-4)
#define DWC_GHTTP_INVALID_BUFFER_SIZE (-3)
#define DWC_GHTTP_INVALID_URL (-2)
#define DWC_GHTTP_UNSPECIFIED_ERROR (-1)

#define DWC_GHTTP_SUCCESS 0
#define DWC_GHTTP_OUT_OF_MEMORY 1
#define DWC_GHTTP_BUFFER_OVERFLOW 2
#define DWC_GHTTP_PARSE_URL_FAILED 3
#define DWC_GHTTP_HOST_LOOKUP_FAILED 4
#define DWC_GHTTP_SOCKET_FAILED 5
#define DWC_GHTTP_CONNECT_FAILED 6
#define DWC_GHTTP_BAD_RESPONSE 7
#define DWC_GHTTP_REQUEST_REJECTED 8
#define DWC_GHTTP_UNAUTHORIZED 9
#define DWC_GHTTP_FORBIDDEN 10
#define DWC_GHTTP_FILE_NOT_FOUND 11
#define DWC_GHTTP_SERVER_ERROR 12
#define DWC_GHTTP_FILE_WRITE_FAILED 13
#define DWC_GHTTP_FILE_READ_FAILED 14
#define DWC_GHTTP_FILE_INCOMPLETE 15
#define DWC_GHTTP_FILE_TOO_BIG 16
#define DWC_GHTTP_ENCRYPTION_ERROR 17
#define DWC_GHTTP_REQUEST_CANCELLED 18

typedef int DWCGHTTPState;
typedef int DWCGHTTPResult;

typedef void (*DWCGHTTPProgressCallback)(DWCGHTTPState state, const char* buf,
                                         int len, int bytesReceived,
                                         int totalSize, void* param);
typedef void (*DWCGHTTPCompletedCallback)(const char* buf, int len,
                                          DWCGHTTPResult result, void* param);

BOOL DWC_InitGHTTP(const char* gamename);
BOOL DWC_ShutdownGHTTP(void);
BOOL DWC_ProcessGHTTP(void);
int DWC_GetGHTTPDataEx(const char* url, int bufferlen, BOOL buffer_clear,
                       DWCGHTTPProgressCallback progressCallback,
                       DWCGHTTPCompletedCallback completedCallback,
                       void* param);

#ifdef __cplusplus
}
#endif
