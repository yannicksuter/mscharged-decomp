#ifndef HOMEBUTTON_COMMON_H
#define HOMEBUTTON_COMMON_H

#include "revolution/hbm/HBMTypes.h"
#include "revolution/kpad/KPAD.h"
#include <revolution/mem/allocator.h>
#include "revolution/mtx/mtx.h"
#include "revolution/wpad/WPAD.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NW4HBM_VERSION(major, minor) ((major & 0xFF) << 8 | minor & 0xFF)

typedef enum HBMSelectBtnNum {
    /* -1 */ HBM_SELECT_NULL = -1,
    /* 0 */ HBM_SELECT_HOMEBTN,
    /* 1 */ HBM_SELECT_BTN1,
    /* 2 */ HBM_SELECT_BTN2,
    /* 3 */ HBM_SELECT_BTN3,
    /* 4 */ HBM_SELECT_BTN4,
    /* 5 */ HBM_SELECT_MAX
} HBMSelectBtnNum;

typedef enum HBMSoundEvent {
    HBM_SOUND_INIT,
    HBM_SOUND_POST_INIT,
    HBM_SOUND_GOTO_MENU,
    HBM_SOUND_RETURN_APP,
    HBM_SOUND_STOP,
    HBM_SOUND_PLAY,
} HBMSoundEvent;

void HBMCreate(const HBMDataInfo* pHBInfo);
void HBMDelete(void);
void HBMInit(void);
HBMSelectBtnNum HBMCalc(const HBMControllerData* pController);
void HBMDraw(void);
HBMSelectBtnNum HBMGetSelectBtnNum(void);
void HBMSetAdjustFlag(bool flag);
void HBMStartBlackOut(void);
bool HBMIsReassignedControllers(void);

void HBMCreateSound(const char* soundData, void* memBuf, u32 memSize);
void HBMDeleteSound(void);
void HBMUpdateSound(void);

void HBMUpdateSoundArchivePlayer(void);
void HBMPlaySound(int num);
void HBMStopSound(void);

#ifdef __cplusplus
}
#endif

#endif
