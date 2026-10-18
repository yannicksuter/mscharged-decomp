#pragma once

#include <gamespy/gt2/gt2.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void DWCi_InitReliableQueue(void);
    void DWCi_CleanupReliableQueue(void);
    BOOL DWCi_IsReliableQueueFree(void);
    BOOL DWCi_ProcessReliableQueue(void);
    BOOL DWCi_InsertReliableQueue(GT2Connection connection, const u8* buffer, int size);

#ifdef __cplusplus
}
#endif
