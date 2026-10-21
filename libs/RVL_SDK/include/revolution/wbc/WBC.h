#ifndef RVL_SDK_WBC_H
#define RVL_SDK_WBC_H

#include <revolution/wpad/WPAD.h>

#ifdef __cplusplus
extern "C"
{
#endif

    WPADResult WBCSetupCalibration(void);
    s32 WBCGetCalibrationStatus(void);
    s32 WBCGetBatteryLevel(u8 battery);
    WPADResult WBCRead(WPADBLStatus* status, f64* weight, int count);
    WPADResult WBCSetZEROPoint(f64* weights, int count);
    WPADResult WBCGetTGCWeight(f64 tareWeight, f64* weight, WPADBLStatus* status);

#ifdef __cplusplus
}
#endif

#endif
