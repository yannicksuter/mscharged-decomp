#ifndef METROTRK_DEBUGGERDRIVER_H
#define METROTRK_DEBUGGERDRIVER_H

#include <revolution/os/OSInterrupt.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void DBInitComm(u8** flagOut, OSInterruptHandler handler);
    void DBInitInterrupts(void);
    u32 DBQueryData(void);
    bool DBRead(void* dst, u32 size);
    bool DBWrite(const void* src, u32 size);
    void DBOpen(void);
    void DBClose(void);

#ifdef __cplusplus
}
#endif

#endif // METROTRK_DEBUGGERDRIVER_H
