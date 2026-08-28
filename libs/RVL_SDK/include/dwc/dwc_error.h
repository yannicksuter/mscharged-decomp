#pragma once

#include <revolution/types.h>

#include <dwc/dwci_error.h>

#define DWC_ECODE_SEQ_LOGIN      (-60000)
#define DWC_ECODE_SEQ_ETC        (-90000)
#define DWC_ECODE_GS_GP          (-1000)
#define DWC_ECODE_GS_GT2         (-7000)
#define DWC_ECODE_TYPE_ALLOC     (-1)
#define DWC_ECODE_TYPE_PARAM     (-2)
#define DWC_ECODE_TYPE_SO_SOCKET (-3)
#define DWC_ECODE_TYPE_NETWORK   (-10)
#define DWC_ECODE_TYPE_SERVER    (-20)

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
