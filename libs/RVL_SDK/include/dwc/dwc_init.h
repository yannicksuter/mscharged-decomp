#pragma once

#include <dwc/dwc_memfunc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  DWC_SVR_DEV,
  DWC_SVR_RELEASE,
  DWC_SVR_PROHIBITED
} DWC_AuthServer;

int DWC_Init(DWC_AuthServer authSvr, const char* gameName, u32 gameCode,
             DWCAllocEx allocator, DWCFreeEx freeer);
void DWC_Shutdown();
u32 DWCi_GetGamecode();

#ifdef __cplusplus
}
#endif
