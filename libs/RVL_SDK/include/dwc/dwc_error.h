#pragma once

#include <revolution/types.h>

#include <dwc/dwci_error.h>

#define DWC_ECODE_SEQ_LOGIN      (-60000)
#define DWC_ECODE_SEQ_FRIEND     (-70000)
#define DWC_ECODE_SEQ_ETC        (-90000)
#define DWC_ECODE_GS_GP          (-1000)
#define DWC_ECODE_GS_PERS        (-2000)
#define DWC_ECODE_GS_GT2         (-7000)
#define DWC_ECODE_TYPE_ALLOC     (-1)
#define DWC_ECODE_TYPE_PARAM     (-2)
#define DWC_ECODE_TYPE_SO_SOCKET (-3)
#define DWC_ECODE_TYPE_NETWORK   (-10)
#define DWC_ECODE_TYPE_SERVER    (-20)
#define DWC_ECODE_TYPE_DNS       (-30)
#define DWC_ECODE_TYPE_DATA      (-40)
#define DWC_ECODE_TYPE_SOCKET    (-50)
#define DWC_ECODE_TYPE_TIMEOUT   (-70)

typedef DWCErrorType DWCError;

#ifdef __cplusplus
extern "C"
{
#endif

    int DWC_GetLastError(int* errorCode);
    s32 DWC_GetLastErrorEx(s32* errorCode, DWCErrorType* errorType);
    void DWC_ClearError();

#ifdef __cplusplus
}
#endif
