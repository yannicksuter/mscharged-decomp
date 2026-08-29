#pragma once

#include <dwc/dwc_init.h>
#include <dwc/dwc_memfunc.h>
#include <revolution/os/OSTime.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void DWCi_Auth_InitInterface(DWC_AuthServer);
    DWC_AuthServer fn_8049EDC0(void);
    int DWCi_Auth_StartAuthentication(const u16*, const char*, u64, DWCAllocFunc, DWCFreeFunc);
    void DWCi_Auth_AbortAuthentication(void);
    void DWCi_Auth_ProcessAuthentication(void);
    BOOL DWCi_Auth_IsFinished(void);
    BOOL DWCi_Auth_IsSucceeded(void);
    int DWCi_Auth_GetErrorCode(void);
    s32 DWCi_Auth_GetIngamesnCheckResult(void);
    u64 DWCi_Auth_GetConsoleUserId(void);
    BOOL fn_8049F7D4(OSTime*);

#ifdef __cplusplus
}
#endif
