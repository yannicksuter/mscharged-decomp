#pragma once

#include <dwc/dwc_main.h>

typedef struct DWCConnectionInfo
{
    u8 index;
    u8 aid;
    u16 reserve;
    int profileID;
} DWCConnectionInfo;

#ifdef __cplusplus
extern "C"
{
#endif

    DWCConnectionInfo* DWCi_GetConnectionInfoByIdx(int index);

#ifdef __cplusplus
}
#endif
