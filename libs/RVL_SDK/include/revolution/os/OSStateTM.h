#ifndef REVOLUTION_OS_STATE_TM_H
#define REVOLUTION_OS_STATE_TM_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*OSStateCallback)(void);

    OSStateCallback OSSetPowerCallback(OSStateCallback callback);
    BOOL OSGetResetButtonState(void);
    bool __OSInitSTM(void);
    void __OSShutdownToSBY(void);
    void __OSHotReset(void);
    s32 __OSSetVIForceDimming(u32 enabled, u32 yShift, u32 xShift);
    s32 __OSUnRegisterStateEvent(void);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_STATE_TM_H
