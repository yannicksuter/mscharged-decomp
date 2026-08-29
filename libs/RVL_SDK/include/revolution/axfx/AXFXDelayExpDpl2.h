#ifndef RVL_SDK_AXFX_DELAY_EXP_DPL2_H
#define RVL_SDK_AXFX_DELAY_EXP_DPL2_H
#include <revolution/types.h>

#include <revolution/axfx/AXFXCommon.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct AXFX_DELAY_EXP_DPL2 {
    s32* line[4];        // at 0x0
    u32 curPos[4];       // at 0x10
    u32 length[4];       // at 0x20
    s32 feedbackGain[4]; // at 0x30
    s32 outGain[4];      // at 0x40
    u32 active;          // at 0x50
    u32 delay[4];        // at 0x54
    u32 feedback[4];     // at 0x64
    u32 output[4];       // at 0x74
} AXFX_DELAY_EXP_DPL2;

u32 AXFXDelayExpGetMemSizeDpl2(const AXFX_DELAY_EXP_DPL2* fx);
BOOL AXFXDelayExpInitDpl2(AXFX_DELAY_EXP_DPL2* fx);
BOOL AXFXDelayExpSettingsDpl2(AXFX_DELAY_EXP_DPL2* fx);
void AXFXDelayExpShutdownDpl2(AXFX_DELAY_EXP_DPL2* fx);
void AXFXDelayExpCallbackDpl2(AXFX_BUFFERUPDATE_DPL2* update, AXFX_DELAY_EXP_DPL2* fx);

#ifdef __cplusplus
}
#endif
#endif
