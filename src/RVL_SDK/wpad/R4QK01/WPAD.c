#include <revolution/version.h>
#include <revolution/vi/vi_fwd.h>

#include <revolution/bte.h>
#include <revolution/dvd.h>
#include <revolution/ipc.h>
#include <revolution/os.h>
#include <revolution/sc.h>
#include <revolution/wpad.h>
#include <revolution/wud.h>

#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define DPD_CONFIG1_SIZE 9
#define DPD_CONFIG2_SIZE 2
#define SPK_CONFIG_SIZE 7

RVL_LIB_VERSION(WPAD, "Jun 22 2009", "18:33:21", "0x4302_145");

WPADCB _wpd[WPAD_MAX_CONTROLLERS];
WPADCB* _wpdcb[WPAD_MAX_CONTROLLERS];

u8 __WPADiManageHandlerStack[0x1000] ALIGN(32);

s8 _dev_handle_index[WUD_MAX_DEV_ENTRY];
OSAlarm _managerAlarm;

u8 _gametype;
const char* _gamecode;
u8 _sleepTime;
u8 _dpdSensitivity;
u8 _sensorBarPos;
u32 _rumble;
u8 _speakerVolume;
static u8 _scSetting;
static u8 _shutdown;
static s8 _afhChannel;
u8 _chan_active_state[WPAD_MAX_CONTROLLERS];
static s8 _wpadInfoResult[WPAD_MAX_CONTROLLERS];
static BOOL _initialized;
static BOOL _wpadUsedCallback;

__attribute__((weak)) BOOL _enabledDRM;
__attribute__((weak)) BOOL _enabledGTR;
__attribute__((weak)) BOOL _enabledTRN;
__attribute__((weak)) BOOL _enabledVSM;
__attribute__((weak)) void (*_wpadDRMInit)(void);
__attribute__((weak)) void (*_wpadGTRInit)(void);
__attribute__((weak)) void (*_wpadTRNInit)(void);
__attribute__((weak)) void (*_wpadVSMInit)(void);

static int _recFlag = -1;
static int _recCnt;
static BOOL _startup;
static u8 _rumbleCnt[WPAD_MAX_CONTROLLERS];
static u8 _extCnt[WPAD_MAX_CONTROLLERS];
static u16 _afhCnt;
static u8 _checkCnt;
static u16 _senseCnt;
static u8 _regShutdown;

extern BOOL __OSIsReturnToIdle;
WPADResult WPADWriteExtReg(WPADChannel chan, const void* data, u16 length,
                          u32 address, WPADCallback callback);

static u16 _wpad_diff_count_threshold[WPAD_MAX_NZFILTERS] = {6, 4, 6};
static u16 _wpad_hyst_count_threshold[WPAD_MAX_NZFILTERS] = {30, 30, 30};

static void WPADiConnCallback(WUDDevInfo* devInfo, u8 open);
static void WPADiRecvCallback(UINT8 devHandle, UINT8* pReport, UINT16 len);

static void __wpadSyncCallback(WPADChannel chan, WPADResult result);
static WPADResult __wpadSync(WPADChannel chan);
static DECOMP_INLINE void WPADiDisconnect(s32 chan, BOOL sleep);
static void __SendData(s32 chan, WPADCommand command);
static s32 WPADiGetStatus(s32 chan);

BOOL __CanPushCmdQueue(const WPADCommandQueue* pQueue, s8 num);
s8 __GetCmdNumber(const WPADCommandQueue* pQueue);

BOOL WPADIsUsedCallbackByKPAD(void);
static void __SetScreenSaverFlag(BOOL disable);
static u8 __GetDpdSensitivity(void);
static u8 __GetSensorBarPosition(void);
static u32 __GetMotorMode(void);
static u8 __ClampSpeakerVolume(u8 volume);
static u8 __GetSpeakerVolume(void);

u16 _WPADGetStackBufferStatus(s32 chan);
u16 _WPADGetModuleBufferStatus(s32 chan);

static void* noAlloc(u32 size) {
    return NULL;
}

static BOOL noFree(void* pBlock) {
    return FALSE;
}

BOOL WPADIsEnabledVSM(void) { return _enabledVSM; }
BOOL WPADIsEnabledTRN(void) { return _enabledTRN; }
BOOL WPADIsEnabledGTR(void) { return _enabledGTR; }
BOOL WPADIsEnabledDRM(void) { return _enabledDRM; }

__attribute__((weak)) WPADResult WBCSetupCalibration(void) { return WPAD_ERR_OK; }
__attribute__((weak)) s32 WBCGetCalibrationStatus(void) { return 0; }
__attribute__((weak)) s32 WBCGetBatteryLevel(u8 chan) { return 0; }

__attribute__((weak)) WPADResult WBCRead(WPADBLStatus* status, f64* weight, int count) {
    DEBUGPrint("WBCReadDummy\n");
    return WPAD_ERR_NO_CONTROLLER;
}

__attribute__((weak)) WPADResult WBCSetZEROPoint(f64* weights, int count) {
    DEBUGPrint("WBCSetZEROPointDummy\n");
    return WPAD_ERR_NO_CONTROLLER;
}

__attribute__((weak)) WPADResult WBCGetTGCWeight(f64 tareWeight, f64* weight, WPADBLStatus* status) {
    DEBUGPrint("WBCGetTGCWeightDummy\n");
    return WPAD_ERR_NO_CONTROLLER;
}

static BOOL OnShutdown(BOOL final, u32 event) {
    WUDLibStatus status = WUDGetStatus();
    BOOL cleanup;

    if (final || status == WUD_LIB_STATUS_0) {
        return TRUE;
    }
    if (status == WUD_LIB_STATUS_4 || status == WUD_LIB_STATUS_1 ||
        status == WUD_LIB_STATUS_2) {
        return FALSE;
    }
    if (WUDIsBusy()) {
        WUDCancelSyncDevice();
        return FALSE;
    }
    switch (event) {
    case OS_SD_EVENT_FATAL:
        WPADRegisterAllocator(&noAlloc, &noFree);
        // FALLTHROUGH
    case OS_SD_EVENT_SHUTDOWN:
    case OS_SD_EVENT_3:
        cleanup = TRUE;
        break;
    case OS_SD_EVENT_1:
    case OS_SD_EVENT_RESTART:
    case OS_SD_EVENT_LAUNCH_APP:
        cleanup = FALSE;
        break;
    case OS_SD_EVENT_RETURN_TO_MENU:
        cleanup = __OSIsReturnToIdle ? TRUE : FALSE;
        break;
    }
    if (cleanup) {
        __WPADShutdown();
    } else {
        __WPADReconnect(TRUE);
    }
    return FALSE;
}

static void __wpadSyncCallback(WPADChannel chan, WPADResult result) {
    _wpadInfoResult[chan] = result;

    OSWakeupThread(&_wpdcb[chan]->threadQueue);
}

static WPADResult __wpadSync(WPADChannel chan) {
    OSSleepThread(&_wpdcb[chan]->threadQueue);

    return _wpadInfoResult[chan];
}

static OSShutdownFunctionInfo ShutdownFunctionInfo = {OnShutdown, 127};

static inline WPADCB* __GetControlBlock(s32 chan) {
    return _wpdcb[chan];
}

static void __SendData(s32 chan, WPADCommand command) {
    BOOL enabled;
    BOOL rumble;
    s8 devHandle;
    WPADCB* p;
    UINT8 reportID;
    BT_HDR* pBuffer;
    u8* pRptData;
    s32 status;
    u8* pCmdData;
    UINT16 len;

    pBuffer = NULL;
    p = _wpdcb[chan];

    reportID = command.reportID;
    pCmdData = command.dataBuf;
    len = command.dataLength;

    enabled = OSDisableInterrupts();
    status = p->status;
    devHandle = p->devHandle;

    if (devHandle < 0) {
        OSRestoreInterrupts(enabled);
        return;
    }

    p->status = WPAD_ERR_COMMUNICATION_ERROR;
    rumble = p->motorRunning & _rumble;

    if (reportID == RPTID_SET_RUMBLE) {
        p->status = status;

    } else if (reportID == RPTID_SEND_SPEAKER_DATA) {
        p->status = status;
        p->audioFrames--;

    } else {
        switch (reportID) {
        case RPTID_WRITE_DATA: {
            break;
        }

        case RPTID_READ_DATA: {
            p->wmReadErr = 0;
            p->wmReadAddr = command.readAddress;
            p->wmReadLength = command.readLength;
            p->wmReadDataPtr = command.dstBuf;
            break;
        }

        case RPTID_REQUEST_STATUS: {
            p->status = status;
            p->infoOut = command.statusReportOut;
            p->statusReqBusy = TRUE;
            break;
        }

        default: {
            pCmdData[0] |= (1 << RPT_OUT_FLAG_REQUEST_ACK_RPT);
            break;
        }
        }

        p->cmdBlkCallback = command.cmdCB;
        p->lastReportId = reportID;
        p->lastReportSendTime = __OSGetSystemTime() + OS_SEC_TO_TICKS(2);
        p->UNK_0x910 = 0;

    }

    OSRestoreInterrupts(enabled);

    DEBUGPrint("handle = %d, repid = %02x\n", devHandle, reportID);

    pBuffer = GKI_getbuf((u8)(len + 18));
    pBuffer->len = (u8)(len + 1);
    pBuffer->offset = 10;

    pRptData = (u8*)(pBuffer) + sizeof(BT_HDR) + pBuffer->offset;
    pRptData[0] = reportID;
    memcpy(&pRptData[1], pCmdData, len);

    if (rumble) {
        pRptData[RPT_OUT_FLAGS] |= (1 << RPT_OUT_FLAG_RUMBLE);
    } else {
        pRptData[RPT_OUT_FLAGS] &= ~(1 << RPT_OUT_FLAG_RUMBLE);
    }

    BTA_HhSendData((UINT8)devHandle, pBuffer);
}

static s32 WPADiSendData(s32 chan, WPADCommand command) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p->statusReqBusy ? WPAD_ERR_COMMUNICATION_ERROR : WPADiGetStatus(chan);
    OSRestoreInterrupts(enabled);
    if (status == WPAD_ERR_OK) {
        __SendData(chan, command);
    }
    if (status == WPAD_ERR_COMMUNICATION_ERROR) {
        if ((s32)OS_TICKS_TO_SEC(__OSGetSystemTime() - p->lastReportSendTime) > 1 && _sleepTime != 0) {
            p->lastReportSendTime = __OSGetSystemTime();
            WPADiDisconnect(chan, FALSE);
        }
    }
    return status;
}

static void WPADiRadioSensitivity(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    u32 two = 2;
    u16 a;

    if (_senseCnt != 10) {
        return;
    }

    a = p->radioSensitivity * 9;
    a += (u16)(p->copyOutCount * 100 / two);
    a /= 10;
    a = MIN(100, a);

    p->radioSensitivity = a;
    p->copyOutCount = 0;

    if (p->radioQuality != WPAD_RADIO_QUALITY_GOOD) {
        if (a > 85) {
            p->radioQuality = WPAD_RADIO_QUALITY_GOOD;
            p->radioQualityOkMs = 0;
        } else if (a > 80) {
            p->radioQualityOkMs++;

            if (p->radioQualityOkMs >= 20) {
                p->radioQuality = WPAD_RADIO_QUALITY_GOOD;
                p->radioQualityOkMs = 0;
            }
        }
    } else if (a < 75) {
        p->radioQuality = WPAD_RADIO_QUALITY_BAD;
        p->radioQualityOkMs = 0;
    } else if (a < 80) {
        p->radioQualityOkMs++;

        if (p->radioQualityOkMs >= 1) {
            p->radioQuality = WPAD_RADIO_QUALITY_BAD;
            p->radioQualityOkMs = 0;
        }
    }
}

static u8 IsButtonChanged(u16 lhs, u16 rhs) {
    return lhs != rhs ? TRUE : FALSE;
}

static u8 IsAnalogChanged(s32 lhs, s32 rhs, s32 threshold) {
    s32 diff = lhs - rhs < 0 ? rhs - lhs : lhs - rhs;
    return diff > threshold ? TRUE : FALSE;
}

static BOOL CalcAnalogNoise(WPADCB* p, WPADNZFilter filter, BOOL changed) {
    if (changed) {
        p->filterDiff[filter]++;
        if (p->filterDiff[filter] > _wpad_diff_count_threshold[filter]) {
            p->filterDiff[filter] = 0;
            p->filterSame[filter] = 0;
            return TRUE;
        }
    } else {
        p->filterSame[filter] = (p->filterSame[filter] + 1) % _wpad_hyst_count_threshold[filter];
        if (p->filterSame[filter] == _wpad_hyst_count_threshold[filter] - 1 && p->filterDiff[filter]) {
            p->filterDiff[filter]--;
        }
    }
    return FALSE;
}

static BOOL IsControllerDataChanged(WPADCB* p, void* pLhs, void* pRhs) {
    WPADStatus* lhs = pLhs;
    WPADStatus* rhs = pRhs;
    u8 changed = FALSE;
    if (p->dataFormat != WPAD_FMT_14 &&
        (lhs->err == WPAD_ERR_OK || lhs->err == WPAD_ERR_CORRUPTED) &&
        (rhs->err == WPAD_ERR_OK || rhs->err == WPAD_ERR_CORRUPTED)) {
        changed |= IsButtonChanged(lhs->button, rhs->button);
        changed |= CalcAnalogNoise(p, WPAD_NZFILTER_ACC,
            IsAnalogChanged(lhs->accX, rhs->accX, 12) |
            IsAnalogChanged(lhs->accY, rhs->accY, 12) |
            IsAnalogChanged(lhs->accZ, rhs->accZ, 12));
        changed |= CalcAnalogNoise(p, WPAD_NZFILTER_DPD,
            IsAnalogChanged(lhs->obj[0].x, rhs->obj[0].x, 2) |
            IsAnalogChanged(lhs->obj[0].y, rhs->obj[0].y, 2) |
            IsAnalogChanged(lhs->obj[1].x, rhs->obj[1].x, 2) |
            IsAnalogChanged(lhs->obj[1].y, rhs->obj[1].y, 2) |
            IsAnalogChanged(lhs->obj[2].x, rhs->obj[2].x, 2) |
            IsAnalogChanged(lhs->obj[2].y, rhs->obj[2].y, 2) |
            IsAnalogChanged(lhs->obj[3].x, rhs->obj[3].x, 2) |
            IsAnalogChanged(lhs->obj[3].y, rhs->obj[3].y, 2));
    }
    if (lhs->err == WPAD_ERR_OK && rhs->err == WPAD_ERR_OK) {
        switch (p->dataFormat) {
        case WPAD_FMT_FS_BTN:
        case WPAD_FMT_FS_BTN_ACC:
        case WPAD_FMT_FS_BTN_ACC_DPD: {
            WPADFSStatus* lhsFS = pLhs;
            WPADFSStatus* rhsFS = pRhs;
            changed |= CalcAnalogNoise(p, WPAD_NZFILTER_EXT,
                IsAnalogChanged(lhsFS->fsAccX, rhsFS->fsAccX, 12) |
                IsAnalogChanged(lhsFS->fsAccY, rhsFS->fsAccY, 12) |
                IsAnalogChanged(lhsFS->fsAccZ, rhsFS->fsAccZ, 12));
            changed |= IsAnalogChanged(lhsFS->fsStickX, rhsFS->fsStickX, 1);
            changed |= IsAnalogChanged(lhsFS->fsStickY, rhsFS->fsStickY, 1);
            break;
        }
        case WPAD_FMT_CLASSIC_BTN:
        case WPAD_FMT_CLASSIC_BTN_ACC:
        case WPAD_FMT_CLASSIC_BTN_ACC_DPD:
        case WPAD_FMT_GUITAR:
        case WPAD_FMT_DRUM: {
            WPADCLStatus* lhsCL = pLhs;
            WPADCLStatus* rhsCL = pRhs;
            s32 leftDiv, rightDiv, triggerDiv;
            switch (p->devMode) {
            case WPAD_DEV_MODE_CLASSIC_REDUCED:
                leftDiv = 16;
                rightDiv = 32;
                triggerDiv = 8;
                break;
            case WPAD_DEV_MODE_CLASSIC_STANDARD:
                leftDiv = 4;
                rightDiv = 4;
                triggerDiv = 1;
                break;
            default:
                leftDiv = 1;
                rightDiv = 1;
                triggerDiv = 1;
                break;
            }
            changed |= IsButtonChanged(lhsCL->clButton, rhsCL->clButton);
            changed |= IsAnalogChanged(lhsCL->clLStickX / leftDiv, rhsCL->clLStickX / leftDiv, 1);
            changed |= IsAnalogChanged(lhsCL->clLStickY / leftDiv, rhsCL->clLStickY / leftDiv, 1);
            changed |= IsAnalogChanged(lhsCL->clRStickX / rightDiv, rhsCL->clRStickX / rightDiv, 1);
            changed |= IsAnalogChanged(lhsCL->clRStickY / rightDiv, rhsCL->clRStickY / rightDiv, 1);
            changed |= IsAnalogChanged(lhsCL->clTriggerL / triggerDiv, rhsCL->clTriggerL / triggerDiv, 1);
            changed |= IsAnalogChanged(lhsCL->clTriggerR / triggerDiv, rhsCL->clTriggerR / triggerDiv, 1);
            break;
        }
        case WPAD_FMT_TRAIN: {
            WPADTRStatus* lhsTR = pLhs;
            WPADTRStatus* rhsTR = pRhs;
            changed |= IsButtonChanged(lhsTR->trButton, rhsTR->trButton);
            changed |= IsAnalogChanged(lhsTR->brake, rhsTR->brake, 1);
            changed |= IsAnalogChanged(lhsTR->mascon, rhsTR->mascon, 1);
            break;
        }
        case WPAD_FMT_BALANCE_CHECKER: {
            WPADBLStatus* lhsBL = pLhs;
            WPADBLStatus* rhsBL = pRhs;
            changed |= CalcAnalogNoise(p, WPAD_NZFILTER_EXT,
                IsAnalogChanged(lhsBL->press[0], rhsBL->press[0], 50) |
                IsAnalogChanged(lhsBL->press[1], rhsBL->press[1], 50) |
                IsAnalogChanged(lhsBL->press[2], rhsBL->press[2], 50) |
                IsAnalogChanged(lhsBL->press[3], rhsBL->press[3], 50));
            break;
        }
        case WPAD_FMT_VSM:
        case WPAD_FMT_14: {
            WPADVSStatus* lhsVS = pLhs;
            WPADVSStatus* rhsVS = pRhs;
            int i;
            changed |= IsButtonChanged(lhsVS->unknown46, rhsVS->unknown46);
            changed |= IsAnalogChanged(lhsVS->unknown44, rhsVS->unknown44, 32);
            for (i = 0; i < 5; i++) {
                changed |= IsAnalogChanged(lhsVS->unknown2A[i], lhsVS->unknown36[i], 32);
            }
            break;
        }
        }
    }
    return changed;
}

static void CheckButtonCombination(s32 chan, void* status) {
    WPADCB* p = _wpdcb[chan];
    WPADCLStatus* pStatus = status;
    BOOL pressed = FALSE;
    if ((p->dataFormat == WPAD_FMT_FS_BTN || p->dataFormat == WPAD_FMT_FS_BTN_ACC || p->dataFormat == WPAD_FMT_FS_BTN_ACC_DPD) &&
        pStatus->button == (WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS)) {
        pressed = TRUE;
    }
    if ((p->dataFormat == WPAD_FMT_CLASSIC_BTN || p->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC || p->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC_DPD) &&
        (pStatus->button == (WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS) ||
         (pStatus->clButton == (WPAD_BUTTON_CL_A | WPAD_BUTTON_CL_B | WPAD_BUTTON_CL_PLUS | WPAD_BUTTON_CL_MINUS) && pStatus->err == WPAD_ERR_OK))) {
        pressed = TRUE;
    }
    p->comboHeld += pressed;
    if (p->comboHeld > 600) {
        WPADRecalibrate(chan);
    }
}


static void WPADiCheckContInputs(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL changed;
    BOOL screenSaver = FALSE;
    BOOL enabled;
    u8 rxBufIndex;
    WPADStatus* status;
    WPADStatus* previous;
    if (_checkCnt != 5) {
        return;
    }
    enabled = OSDisableInterrupts();
    rxBufIndex = p->rxBufIndex != 0 ? 0 : 1;
    status = (WPADStatus*)p->rxBufs[rxBufIndex];
    OSRestoreInterrupts(enabled);
    changed = IsControllerDataChanged(p, status, p->rxBufMain);
    CheckButtonCombination(chan, status);
    if (changed) {
        screenSaver = TRUE;
        p->lastControllerDataUpdate = __OSGetSystemTime();
        memcpy(p->rxBufMain, status, RX_BUFFER_SIZE);
    } else if (_sleepTime != 0) {
        s32 time = OS_TICKS_TO_SEC(__OSGetSystemTime() - p->lastControllerDataUpdate);
        if (time > _sleepTime * 60) {
            WPADiDisconnect(chan, FALSE);
        }
    }
    previous = (WPADStatus*)p->rxBufMain;
    if (previous->err != WPAD_ERR_OK && previous->err != WPAD_ERR_CORRUPTED) {
        memcpy(p->rxBufMain, status, RX_BUFFER_SIZE);
    }
    __SetScreenSaverFlag(screenSaver);
}

static BOOL WPADiProcessExtCommand(s32 chan, BOOL prevSuccess) {
    WPADCB* p = _wpdcb[chan];
    BOOL success = FALSE;
    WPADCommand command;
    if (prevSuccess) {
        return TRUE;
    }
    if (_extCnt[chan] != 5) {
        goto end;
    }
    if (!WPADiGetCommand(&p->extCmdQueue, &command)) {
        goto end;
    }
    if (command.reportID != RPTID_SET_DATA_REPORT_MODE && !p->info.attach) {
        goto end;
    }
    if (WPADiSendData(chan, command) != WPAD_ERR_OK) {
        goto end;
    }
    WPADiPopCommand(&p->extCmdQueue);
    success = TRUE;
    _extCnt[chan] = 0;
end:
    _extCnt[chan] = _extCnt[chan] == 5 ? _extCnt[chan] : _extCnt[chan] + 1;
    return success;
}

static BOOL WPADiProcessCommand(s32 chan, BOOL prevSuccess) {
    WPADCB* p = _wpdcb[chan];
    WPADCommand command;
    if (prevSuccess) {
        return TRUE;
    }
    if (WPADiGetCommand(&p->stdCmdQueue, &command) &&
        WPADiSendData(chan, command) == WPAD_ERR_OK) {
        WPADiPopCommand(&p->stdCmdQueue);
        return TRUE;
    }
    return FALSE;
}



static void WPADiGetScSettings(void) {
    if (SCCheckStatus() != SC_STATUS_OK) {
        return;
    }
    WUDUpdateSCSetting();
    if (_scSetting) {
        _dpdSensitivity = __GetDpdSensitivity();
        _sensorBarPos = __GetSensorBarPosition();
        _rumble = __GetMotorMode();
        _speakerVolume = __GetSpeakerVolume();
        _scSetting = FALSE;
    }
}

static void WPADiAfh(void) {
    BOOL enabled;
    u8* pWifiChannel;

    if (_afhCnt != 60000) {
        return;
    }

    pWifiChannel = OSPhysicalToCached(OS_PHYS_WIFI_AFH_CHANNEL);
    DCInvalidateRange(pWifiChannel, sizeof *pWifiChannel);

    DEBUGPrint("Check the update of WiFi using channel\n");

    DEBUGPrint("WiFi uses channel = %d\n", *pWifiChannel);

    if (_afhChannel != *pWifiChannel) {
        enabled = OSDisableInterrupts();
        _afhChannel = *pWifiChannel;
        OSRestoreInterrupts(enabled);

        WUDSetDisableChannel(_afhChannel);
    }
}

static void WPADiRumbleMotor(s32 chan, BOOL prevSuccess) {
    WPADCommand command;

    if (prevSuccess == TRUE || __GetCmdNumber(&_wpdcb[chan]->stdCmdQueue) > 0) {
        _wpdcb[chan]->motorBusy = FALSE;

    } else if (_rumbleCnt[chan] == 5) {
        _wpdcb[chan]->motorBusy = FALSE;

        command.reportID = RPTID_SET_RUMBLE;
        command.dataLength = RPT10_SIZE;
        command.dataBuf[RPT10_RUMBLE] = FALSE;
        command.cmdCB = NULL;

        __SendData(chan, command);
    }

    _rumbleCnt[chan] = _wpdcb[chan]->motorBusy ? _rumbleCnt[chan] + 1 : 0;
}

static void WPADiResetModule(BOOL saveSimpleDevs) {
    DEBUGPrint("Reconnect Start!!\n");
    BTA_DmSendHciReset();
    OSCancelAlarm(&_managerAlarm);
    WUDSetHidRecvCallback(NULL);
    WUDShutdown(saveSimpleDevs);
}

static BOOL WPADiCheckReconnect(void) {
    if (_recFlag >= 0) {
        if (--_recCnt <= 0) {
            WPADiResetModule(_recFlag);
        }
        return TRUE;
    }
    return FALSE;
}

static void WPADiManageHandler(OSAlarm* pAlarm, OSContext* pContext) {
    WPADLibStatus status = WPADGetStatus();
    s32 chan;
    BOOL success;
    if (status != WPAD_LIB_STATUS_3) {
        if (status == WPAD_LIB_STATUS_2 && !_initialized) {
            _initialized = TRUE;
            _recCnt = 50;
            WUDSetHidConnCallback(WPADiConnCallback);
            WUDSetHidRecvCallback(WPADiRecvCallback);
        }
        return;
    }
    if (WPADiCheckReconnect()) {
        return;
    }
    WPADiAfh();
    WPADiGetScSettings();
    for (chan = WPAD_CHAN0; chan < WPAD_MAX_CONTROLLERS; chan++) {
        success = FALSE;
        if (_wpdcb[chan]->used) {
            success |= WPADiProcessExtCommand(chan, success);
            success |= WPADiProcessCommand(chan, success);
            WPADiRumbleMotor(chan, success);
            WPADiCheckContInputs(chan);
            WPADiRadioSensitivity(chan);
        }
        if (_wpdcb[chan]->extWasDisconnected && _wpdcb[chan]->reconnectExtMs-- < 0) {
            _wpdcb[chan]->extWasDisconnected = FALSE;
        }
    }
    _senseCnt = _senseCnt == 10 ? 0 : _senseCnt + 1;
    _checkCnt = _checkCnt == 5 ? 0 : _checkCnt + 1;
    _afhCnt = _afhCnt == 60000 ? 0 : _afhCnt + 1;
    BTA_HhGetAclQueueInfo();
}

static void WPADiManageHandler0(OSAlarm* pAlarm, OSContext* pContext) {
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, WPADiManageHandler,
                    __WPADiManageHandlerStack +
                        sizeof(__WPADiManageHandlerStack));
}

static void __ClearControlBlock(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    WPADStatus* pStatus;
    int filter;

    p->infoOut = NULL;

    p->motorRunning = FALSE;

    p->cmdBlkCallback = NULL;
    p->extensionCallback = NULL;
    p->samplingCB = NULL;

    p->samplingBuf = NULL;
    p->samplingBufIndex = 0;
    p->samplingBufSize = 0;

    p->dataFormat = WPAD_FMT_CORE_BTN;
    p->status = WPAD_ERR_NO_CONTROLLER;
    p->devType = WPAD_DEV_NOT_FOUND;
    p->devMode = WPAD_DEV_MODE_NORMAL;

    p->calibrated = FALSE;
    p->comboHeld = 0;

    p->statusReqBusy = FALSE;

    p->dpdDummyObjSize = 12;
    p->currentDpdCommand = WPAD_DPD_DISABLE;
    p->pendingDpdCommand = 0;

    for (filter = 0; filter < WPAD_MAX_NZFILTERS; filter++) {
        p->filterDiff[filter] = 0;
        p->filterSame[filter] = 0;
    }

    p->lastControllerDataUpdate = __OSGetSystemTime();
    p->lastReportSendTime = __OSGetSystemTime();
    p->UNK_0x910 = 0;
    p->UNK_0x8C9 = 0;
    p->UNK_0x8C4 = 0;
    p->wmReadDataPtr = NULL;
    p->wmReadAddr = 0;
    p->wmReadLength = 0;
    p->wmReadErr = 0;
    p->devHandle = WUD_DEV_HANDLE_INVALID;
    p->used = FALSE;
    p->handshakeFinished = FALSE;
    p->oldFw = 0;
    p->radioQuality = WPAD_RADIO_QUALITY_BAD;
    p->radioQualityOkMs = 0;
    p->audioFrames = 0;
    p->UNK_0x982 = 0;
    p->radioSensitivity = 0;
    p->copyOutCount = 0;
    p->sleeping = TRUE;
    p->getInfoBusy = FALSE;
    p->getInfoCB = NULL;
    p->disableContReport = FALSE;
    p->blcBattery = WPAD_BATTERY_LEVEL_MAX;
    p->savedDevType = WPAD_DEV_NOT_FOUND;
    p->extWasDisconnected = FALSE;
    p->reconnectExtMs = 0;

    memset(&p->info, 0, sizeof(p->info));
    memset(&p->wmReadDataBuf, 0, sizeof(p->wmReadDataBuf));


    memset(&p->devConf, 0, sizeof(WPADDevConfig));
    memset(&p->extConf, 0, sizeof(WPADExtConfig));
    memset(&p->key, 0, sizeof(p->key));
    memset(&p->decryptAddTable, 0, sizeof(p->decryptAddTable));
    memset(&p->decryptXorTable, 0, sizeof(p->decryptXorTable));
    memset(&p->gameInfo, 0, sizeof(WPADGameInfo));

    p->rxBufIndex = 0;
    memset(p->rxBufs, 0, sizeof(p->rxBufs));
    pStatus = (WPADStatus*)p->rxBufs[0];
    pStatus->err = WPAD_ERR_NO_CONTROLLER;
    pStatus = (WPADStatus*)p->rxBufs[1];
    pStatus->err = WPAD_ERR_NO_CONTROLLER;
    memcpy(p->rxBufMain, p->rxBufs[0], sizeof(p->rxBufMain));

    p->gameInfoErr[0] = -1;
    p->gameInfoErr[1] = -1;

    p->stdCmdQueue.buffer = p->stdCmdQueueList;
    p->stdCmdQueue.capacity = ARRAY_SIZE(p->stdCmdQueueList);
    p->extCmdQueue.buffer = p->extCmdQueueList;
    p->extCmdQueue.capacity = ARRAY_SIZE(p->extCmdQueueList);

    WPADiClearQueue(&p->stdCmdQueue);
    WPADiClearQueue(&p->extCmdQueue);


    WPADiClearMemBlock(chan);
    _extCnt[chan] = 0;

    _rumbleCnt[chan] = 0;
}

void WPADiInitSub(void) {
    s32 chan;
    int i;

    WPADSetSensorBarPower(TRUE);

    for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
        _dev_handle_index[i] = WUD_DEV_HANDLE_INVALID;
    }

    DEBUGPrint("WPADInit()\n");

    for (chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++) {
        _wpdcb[chan] = &_wpd[chan];
        _chan_active_state[chan] = FALSE;

        _wpdcb[chan]->connectCB = NULL;
        __ClearControlBlock(chan);
        OSInitThreadQueue(&_wpd[chan].threadQueue);

        _extCnt[chan] = 0;
        _rumbleCnt[chan] = 0;
    }

    _sleepTime = 5;
    _gamecode = OSGetAppGamename();
    _gametype = OSGetAppType();
    _dpdSensitivity = __GetDpdSensitivity();
    _sensorBarPos = __GetSensorBarPosition();
    _rumble = __GetMotorMode();
    _speakerVolume = __GetSpeakerVolume();
    _senseCnt = 0;
    _checkCnt = 0;
    _afhCnt = 0;
    _shutdown = FALSE;
    _scSetting = TRUE;
    _afhChannel = -1;
    _wpadUsedCallback = FALSE;
    OSRegisterVersion(__WPADVersion);
    if (_wpadVSMInit != NULL) { _wpadVSMInit(); }
    if (_wpadTRNInit != NULL) { _wpadTRNInit(); }
    if (_wpadGTRInit != NULL) { _wpadGTRInit(); }
    if (_wpadDRMInit != NULL) { _wpadDRMInit(); }

    OSCreateAlarm(&_managerAlarm);
    OSSetPeriodicAlarm(&_managerAlarm, OSGetTime(), OS_MSEC_TO_TICKS(1),
                       WPADiManageHandler0);

}

void WPADInit(void) {
    _startup = TRUE;
    if (!_regShutdown) {
        OSRegisterShutdownFunction(&ShutdownFunctionInfo);
        _regShutdown = TRUE;
    }
    if (WUDInit()) {
        _initialized = FALSE;
        _recFlag = -1;
        _recCnt = 50;
        WPADiInitSub();
    }
}

DECOMP_INLINE void __WPADShutdown(void) {
    BOOL enabled = OSDisableInterrupts();
    int chan;
    if (_shutdown) {
        OSRestoreInterrupts(enabled);
        return;
    }
    _shutdown = TRUE;
    WUDSetVisibility(FALSE, FALSE);
    for (chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++) {
        WUDSetDeviceHistory(chan, NULL);
    }
    OSCancelAlarm(&_managerAlarm);
    WUDSetHidRecvCallback(NULL);
    WUDShutdown(FALSE);
    OSRestoreInterrupts(enabled);
}

void __WPADReconnect(BOOL saveSimpleDevs) {
    BOOL enabled = OSDisableInterrupts();
    if (!_startup) {
        OSRestoreInterrupts(enabled);
        return;
    }
    if (_shutdown) {
        OSRestoreInterrupts(enabled);
        return;
    }
    _shutdown = TRUE;
    _recFlag = saveSimpleDevs ? TRUE : FALSE;
    DEBUGPrint("Wait for %d ms until start reconnect!\n", _recCnt);
    OSRestoreInterrupts(enabled);
}

BOOL WPADStartSimpleSync(void) {
    return WUDStartSyncSimple();
}

BOOL WPADStartFastSimpleSync(void) {
    return WUDStartFastSyncSimple();
}

BOOL WPADStopSimpleSync(void) {
    return WUDStopSyncSimple();
}

BOOL WPADStartClearDevice(void) {
    return WUDStartClearDevice();
}

WPADSyncDeviceCallback
WPADSetSimpleSyncCallback(WPADSyncDeviceCallback pCallback) {
    return WUDSetSyncSimpleCallback(pCallback);
}

WPADClearDeviceCallback
WPADSetClearDeviceCallback(WPADClearDeviceCallback pCallback) {
    return WUDSetClearDeviceCallback(pCallback);
}

void WPADRegisterAllocator(WPADAllocFunc pAllocFunc, WPADFreeFunc pFreeFunc) {
    WUDRegisterAllocator(pAllocFunc, pFreeFunc);
}

u32 WPADGetWorkMemorySize(void) {
    return WUDGetAllocatedMemSize();
}

WPADLibStatus WPADGetStatus(void) {
    return (WPADLibStatus)WUDGetStatus();
}

u8 WPADGetRadioSensitivity(s32 chan) {
    WPADCB* p_wpd = _wpdcb[chan];
    BOOL enabled;
    u8 sensitivity;

    enabled = OSDisableInterrupts();
    sensitivity = p_wpd->radioSensitivity;
    OSRestoreInterrupts(enabled);

    return sensitivity;
}

void WPADGetAddress(s32 chan, BD_ADDR_PTR pAddr) {
    WPADCB* p = _wpdcb[chan];
    BD_ADDR_PTR pDevAddr;
    BOOL enabled;
    s8 devHandle;

    enabled = OSDisableInterrupts();
    devHandle = p->devHandle;
    OSRestoreInterrupts(enabled);

    pDevAddr = _WUDGetDevAddr(devHandle);

    if (pDevAddr != NULL)
        WUD_BDCPY(pAddr, pDevAddr);
    else {
        memset(pAddr, 0, BD_ADDR_LEN);
    }
}

u8 WPADGetSensorBarPosition(void) {
    BOOL enabled = OSDisableInterrupts();
    u8 pos = _sensorBarPos;
    OSRestoreInterrupts(enabled);
    return pos;
}

static void setupCallback(WPADChannel chan, WPADResult status) {
    WPADCB* p = _wpdcb[chan];

    if (status == WPAD_ERR_OK) {
        p->handshakeFinished = TRUE;

        if (p->connectCB != NULL) {
            p->connectCB(chan, status);
        }
    } else {
        WPADiDisconnect(chan, FALSE);
    }
}

static void abortConnCallback(WPADChannel chan, WPADResult status) {
    WPADCB* p = _wpdcb[chan];
    if (status != WPAD_ERR_OK) {
        WPADiClearQueue(&p->stdCmdQueue);
        WPADiDisconnect(chan, FALSE);
    }
}


static void firmwareCheckCallback(WPADChannel chan, WPADResult status) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    u32 address;
    u16 size;
    u8 port;

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return;
    }

    enabled = OSDisableInterrupts();

    p->oldFw = status == WPAD_ERR_OK ? 1 : 0;
    p->status = WPAD_ERR_OK;

    OSRestoreInterrupts(enabled);

    size = status == WPAD_ERR_OK ? 20 : 42;
    address =
        status == WPAD_ERR_OK ? WM_ADDR_MEM_176C : WM_ADDR_MEM_DEV_CONFIG_0;

    DEBUGPrint(" ==>this error means that the firmware is for NDEV %s\n",
               p->oldFw != 0 ? "2.0" : "2.1 or later");

    port = p->devType == WPAD_DEV_BALANCE_CHECKER ? 1 : 1 << chan;

    WPADiSendSetReportType(&p->stdCmdQueue, WPAD_FMT_CORE_BTN, p->disableContReport,
                           &abortConnCallback);

    WPADiSendDPDCSB(&p->stdCmdQueue, FALSE, &abortConnCallback);
    WPADiSendSetPort(&p->stdCmdQueue, port, &abortConnCallback);

    WPADiSendReadData(&p->stdCmdQueue, p->wmReadDataBuf, sizeof(WPADGameInfo),
                      WM_ADDR_MEM_GAME_INFO_0, &abortConnCallback);
    WPADiSendReadData(&p->stdCmdQueue, p->wmReadDataBuf, sizeof(WPADGameInfo),
                      WM_ADDR_MEM_GAME_INFO_1, &abortConnCallback);

    WPADiSendReadData(&p->stdCmdQueue, p->wmReadDataBuf, size, address,
                      &setupCallback);

    WPADiSendGetContStat(&p->stdCmdQueue, NULL, NULL);
}

static s32 WPADiRetrieveChannel(WUDDevInfo* devInfo) {
    BD_ADDR_PTR pAddr;
    s32 i;
    s32 chan = WPAD_CHAN_INVALID;
    u8 devHandle = devInfo->devHandle;
    pAddr = _WUDGetDevAddr(devHandle);
    if (memcmp(devInfo->conf.devName, "Nintendo RVL-WBC", 16) == 0) {
        chan = WPAD_CHAN3;
        if (_wpdcb[chan]->used) {
            btm_remove_acl(pAddr);
            return WPAD_CHAN_INVALID;
        }
    } else {
        for (i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
            if (WUDIsLatestDevice(i, pAddr) && !_chan_active_state[i]) {
                chan = i;
                break;
            }
            if (!_chan_active_state[i] && chan < WPAD_CHAN0) {
                chan = i;
            }
        }
    }
    if (!WUDIsLatestDevice(chan, pAddr)) {
        WUDSetDeviceHistory(chan, pAddr);
    }
    _chan_active_state[chan] = TRUE;
    return chan;
}

static void WPADiConnCallback(WUDDevInfo* devInfo, u8 open) {
    UINT8 devHandle = devInfo->devHandle;
    s32 chan;
    WPADCB* p;
    WPADCommand command;

    DEBUGPrint("connection is %s\n", open ? "opened" : "closed");
    if (open) {
        chan = WPADiRetrieveChannel(devInfo);
        if (chan < 0) {
            return;
        }
        _dev_handle_index[devHandle] = chan;
        __ClearControlBlock(chan);
        p = _wpdcb[chan];
        if (memcmp(devInfo->conf.devName, "Nintendo RVL-CNT", 16) == 0) {
            p->devType = WPAD_DEV_CORE;
            p->dataFormat = WPAD_FMT_CORE_BTN;
        } else if (memcmp(devInfo->conf.devName, "Nintendo RVL-WBC", 16) == 0 && WUDIsLinkedWBC()) {
            p->devType = WPAD_DEV_BALANCE_CHECKER;
            p->dataFormat = WPAD_FMT_BALANCE_CHECKER;
        } else {
            p->devType = WPAD_DEV_FUTURE;
            p->dataFormat = WPAD_FMT_CORE_BTN;
        }
        p->devHandle = devHandle;
        p->used = TRUE;
        p->status = WPAD_ERR_OK;
        p->radioSensitivity = 100;
        p->sleeping = FALSE;
        p->extState = 0;
        p->savedDevType = p->devType;
        WPADiSendReadData(&p->stdCmdQueue, p->wmReadDataBuf, 1,
                         WM_ADDR_MEM_1770, firmwareCheckCallback);
        __SetScreenSaverFlag(TRUE);
    } else {
        chan = _dev_handle_index[devHandle];
        _dev_handle_index[devHandle] = WUD_DEV_HANDLE_INVALID;
        if (chan != WUD_DEV_HANDLE_INVALID) {
            p = _wpdcb[chan];
            p->status = WPAD_ERR_NO_CONTROLLER;
            if (p->cmdBlkCallback != NULL) {
                p->cmdBlkCallback(chan, WPAD_ERR_NO_CONTROLLER);
            } else if (p->memBlock.callback != NULL) {
                p->memBlock.callback(chan, WPAD_ERR_NO_CONTROLLER);
            }
            while (WPADiGetCommand(&p->stdCmdQueue, &command)) {
                if (command.cmdCB != NULL) {
                    command.cmdCB(chan, WPAD_ERR_NO_CONTROLLER);
                }
                WPADiPopCommand(&p->stdCmdQueue);
            }
            DEBUGPrint("clean up command queue\n");
            if (p->samplingBuf != NULL) {
                WPADSetAutoSamplingBuf(chan, p->samplingBuf, p->samplingBufSize);
            }
            __ClearControlBlock(chan);
            _chan_active_state[chan] = FALSE;
            if (p->connectCB != NULL) {
                p->connectCB(chan, WPAD_ERR_NO_CONTROLLER);
            }
        } else {
            DEBUGPrint("WARNING: disconnection for device handle not assigned to channel.\n");
        }
    }
}

static void WPADiRecvCallback(UINT8 devHandle, UINT8* pReport, UINT16 len) {
    s32 result;
    u8 port = _dev_handle_index[devHandle];
    if (port < WPAD_MAX_CONTROLLERS) {
        result = WPADiHIDParser(port, pReport);
        if (result != 0) {
            DEBUGPrint("HID Parser reports: %d\n", result);
        }
    }
}

static s32 WPADiGetStatus(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    s32 status = p->status;

    OSRestoreInterrupts(enabled);
    return status;
}

void WPADGetAccGravityUnit(s32 chan, u32 type, WPADAccGravityUnit* pAcc) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    if (pAcc != NULL) {
        switch (type) {
        case WPAD_ACC_GRAVITY_UNIT_CORE: {
            pAcc->x = p->devConf.acc_1g.x - p->devConf.acc_0g.x;
            pAcc->y = p->devConf.acc_1g.y - p->devConf.acc_0g.y;
            pAcc->z = p->devConf.acc_1g.z - p->devConf.acc_0g.z;
            break;
        }

        case WPAD_ACC_GRAVITY_UNIT_FS: {
            pAcc->x = p->extConf.fs.acc_1g.x - p->extConf.fs.acc_0g.x;
            pAcc->y = p->extConf.fs.acc_1g.y - p->extConf.fs.acc_0g.y;
            pAcc->z = p->extConf.fs.acc_1g.z - p->extConf.fs.acc_0g.z;
            break;
        }
        }
    }

    OSRestoreInterrupts(enabled);
}

static void DisconnectCallback(WPADChannel chan, WPADResult status) {
    WPADCB* p = _wpdcb[chan];

    if (status != WPAD_ERR_NO_CONTROLLER) {
        BTA_HhClose(p->devHandle);
    }
}

static void WPADiDisconnect(s32 chan, BOOL sleep) {
    WPADCB* p = _wpdcb[chan];
    s32 status;
    BOOL enabled;

    status = WPADiGetStatus(chan);
    if (status == WPAD_ERR_NO_CONTROLLER) {
        return;
    }

    if (sleep) {
        enabled = OSDisableInterrupts();

        if (p->sleeping) {
            OSRestoreInterrupts(enabled);
            return;
        }

        p->sleeping = TRUE;

        OSRestoreInterrupts(enabled);
        WPADControlLed(chan, 0, DisconnectCallback);
    }
    else {
        BD_ADDR addr;
        WPADGetAddress(chan, addr);

        btm_remove_acl(addr);
    }
}

void WPADDisconnect(s32 chan) {
    s32 status = WPADiGetStatus(chan);
    if (status == WPAD_ERR_NO_CONTROLLER) {
        return;
    }
    WUDSetDeviceHistory(chan, NULL);
    WPADiDisconnect(chan, TRUE);
}

WPADResult WPADProbe(WPADChannel chan, WPADDeviceType* pDevType) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    s32 status;

    enabled = OSDisableInterrupts();

    if (pDevType != NULL) {
        *pDevType = p->devType;
    }

    status = p->status;

    if (status != WPAD_ERR_NO_CONTROLLER) {
        if (p->devType == WPAD_DEV_NOT_FOUND) {
            status = WPAD_ERR_NO_CONTROLLER;
        } else if (!p->handshakeFinished) {
            status = WPAD_ERR_COMMUNICATION_ERROR;
        }
    }

    OSRestoreInterrupts(enabled);
    return status;
}

WPADSamplingCallback WPADSetSamplingCallback(s32 chan, WPADSamplingCallback pCallback) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    WPADSamplingCallback pOldCallback;

    if (WPADIsUsedCallbackByKPAD()) {
        OSReport("WARNING: Overwritten the callback needed by KPAD.\n");
        OSReport("         Please call KPADSetSamplingCallback instead of WPADSetSamplingCallback.\n");
    }

    enabled = OSDisableInterrupts();
    pOldCallback = p->samplingCB;
    p->samplingCB = pCallback;
    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

WPADConnectCallback* WPADSetConnectCallback(s32 chan, WPADConnectCallback* pCallback) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    WPADConnectCallback* pOldCallback;

    if (WPADIsUsedCallbackByKPAD()) {
        OSReport("WARNING: Overwritten the callback needed by KPAD.\n");
        OSReport("         Please call KPADSetConnectCallback instead of WPADSetConnectCallback.\n");
    }

    enabled = OSDisableInterrupts();
    pOldCallback = p->connectCB;
    p->connectCB = pCallback;
    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

WPADExtensionCallback* WPADSetExtensionCallback(s32 chan, WPADExtensionCallback* pCallback) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    WPADExtensionCallback* pOldCallback;

    enabled = OSDisableInterrupts();
    pOldCallback = p->extensionCallback;
    p->extensionCallback = pCallback;
    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

u32 WPADGetDataFormat(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u32 format = p->dataFormat;
    OSRestoreInterrupts(enabled);
    return format;
}

s32 WPADSetDataFormat(s32 chan, u32 format) {
    WPADCB* p = _wpdcb[chan];
    s32 status;
    BOOL enabled = OSDisableInterrupts();
    BOOL handshake = p->handshakeFinished;
    u32 currFormat;
    status = p->status;
    currFormat = p->dataFormat;
    OSRestoreInterrupts(enabled);
    if (status == WPAD_ERR_NO_CONTROLLER) {
        return WPAD_ERR_NO_CONTROLLER;
    }
    if (!handshake) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }
    if (currFormat == format) {
        return WPAD_ERR_OK;
    }
    if (WPADiSendSetReportType(&p->stdCmdQueue, format, p->disableContReport, NULL)) {
        enabled = OSDisableInterrupts();
        p->dataFormat = format;
        OSRestoreInterrupts(enabled);
        return WPAD_ERR_OK;
    }
    return WPAD_ERR_COMMUNICATION_ERROR;
}

WPADResult WPADGetInfo(WPADChannel chan, WPADInfo* pInfo) {
    WPADResult result = WPADGetInfoAsync(chan, pInfo, __wpadSyncCallback);

    if (result == WPAD_ERR_BUSY) {
        return result;
    }

    return __wpadSync(chan);
}

static void __infoCallback(WPADChannel chan, WPADResult result) {
    WPADCB* p = _wpdcb[chan];

    if (p->getInfoCB != NULL) {
        p->getInfoCB(chan, result);
    }

    p->getInfoCB = NULL;
    p->getInfoBusy = FALSE;
}

s32 WPADGetInfoAsync(s32 chan, WPADInfo* pInfo, WPADCallback pCallback) {
    BOOL handshake;
    s32 status;
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    u8 getInfoBusy;

    enabled = OSDisableInterrupts();

    handshake = p->handshakeFinished;
    status = p->status;
    getInfoBusy = p->getInfoBusy;

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto _end;
    }

    if (!handshake || getInfoBusy) {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto _end;
    }

    enabled = OSDisableInterrupts();

    p->getInfoBusy = TRUE;
    p->getInfoCB = pCallback;

    OSRestoreInterrupts(enabled);

    if (WPADiSendGetContStat(&p->stdCmdQueue, pInfo, __infoCallback)) {
        status = WPAD_ERR_OK;
        goto _end;
    }

    status = WPAD_ERR_COMMUNICATION_ERROR;

    enabled = OSDisableInterrupts();

    p->getInfoBusy = FALSE;
    p->getInfoCB = NULL;

    OSRestoreInterrupts(enabled);

_end:
    if (status != WPAD_ERR_OK && pCallback != NULL) {
        pCallback(chan, status);
    }

    return status;
}

void WPADControlMotor(s32 chan, u32 command) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p->status;

    if (status == WPAD_ERR_NO_CONTROLLER) {
        OSRestoreInterrupts(enabled);
        return;
    }

    if (!_rumble && (command != WPAD_MOTOR_STOP || p->motorRunning != TRUE)) {
        OSRestoreInterrupts(enabled);
        return;
    }

    if (command == WPAD_MOTOR_STOP && p->motorRunning == FALSE ||
        command == WPAD_MOTOR_RUMBLE && p->motorRunning == TRUE) {
        OSRestoreInterrupts(enabled);
        return;
    }

    p->motorRunning = command == WPAD_MOTOR_STOP ? FALSE : TRUE;
    p->motorBusy = TRUE;

    OSRestoreInterrupts(enabled);
}

void WPADEnableMotor(BOOL enable) {
    BOOL enabled = OSDisableInterrupts();
    _rumble = enable;
    OSRestoreInterrupts(enabled);
}

BOOL WPADIsMotorEnabled(void) {
    BOOL enabled = OSDisableInterrupts();
    BOOL motorEnabled = _rumble;
    OSRestoreInterrupts(enabled);

    return motorEnabled;
}

s32 WPADControlLed(s32 chan, u8 flags, WPADCallback pCallback) {
    WPADCB* p = _wpdcb[chan];
    BOOL success;
    BOOL enabled;
    BOOL handshake;
    s32 status;

    status = WPAD_ERR_OK;

    enabled = OSDisableInterrupts();
    status = p->status;
    handshake = p->handshakeFinished;
    OSRestoreInterrupts(enabled);

    if (status != WPAD_ERR_NO_CONTROLLER) {
        if (!handshake) {
            status = WPAD_ERR_COMMUNICATION_ERROR;
        } else {
            success = WPADiSendSetPort(&p->stdCmdQueue, flags, pCallback);
            status = success ? 0 : WPAD_ERR_COMMUNICATION_ERROR;
        }
    }

    if (status != WPAD_ERR_OK && pCallback != NULL) {
        pCallback(chan, status);
    }

    return status;
}

BOOL WPADSaveConfig(WPADSaveCallback pCallback) {
    BOOL success = TRUE;
    BOOL enabled;
    u8 volume;
    u8 mode;

    if (SCCheckStatus() != SC_STATUS_OK) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    volume = _speakerVolume;
    mode = _rumble ? SC_MOTOR_ON : SC_MOTOR_OFF;
    OSRestoreInterrupts(enabled);

    success &= SCSetWpadSpeakerVolume(volume);
    success &= SCSetWpadMotorMode(mode);

    if (success) {
        SCFlushAsync(pCallback);
    } else if (pCallback != NULL) {
        pCallback(SC_STATUS_FATAL);
    }

    return success;
}

static u32 __wpadFmt2Size(u32 format) {
    u32 size;
    switch (format) {
    case WPAD_FMT_FS_BTN:
    case WPAD_FMT_FS_BTN_ACC:
    case WPAD_FMT_FS_BTN_ACC_DPD:
        size = sizeof(WPADFSStatus);
        break;
    case WPAD_FMT_CLASSIC_BTN:
    case WPAD_FMT_CLASSIC_BTN_ACC:
    case WPAD_FMT_CLASSIC_BTN_ACC_DPD:
    case WPAD_FMT_GUITAR:
    case WPAD_FMT_DRUM:
        size = sizeof(WPADCLStatus);
        break;
    case WPAD_FMT_TRAIN:
        size = sizeof(WPADTRStatus);
        break;
    case WPAD_FMT_BALANCE_CHECKER:
        size = sizeof(WPADBLStatus);
        break;
    case WPAD_FMT_VSM:
    case WPAD_FMT_14:
        size = sizeof(WPADVSStatus);
        break;
    case WPAD_FMT_BTN_ACC_DPD_EXTENDED:
        size = sizeof(WPADStatusEx);
        break;
    default:
        size = sizeof(WPADStatus);
        break;
    }
    return size;
}

void WPADRead(s32 chan, WPADStatus* pStatus) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 rxBufIndex = p->rxBufIndex != 0 ? 0 : 1;
    WPADStatus* pRxStatus = (WPADStatus*)p->rxBufs[rxBufIndex];
    u32 size = __wpadFmt2Size(p->dataFormat);
    if (pRxStatus->err != WPAD_ERR_OK) {
        size = sizeof(WPADStatus);
    }
    memcpy(pStatus, pRxStatus, size);
    OSRestoreInterrupts(enabled);
}

void WPADSetAutoSamplingBuf(s32 chan, void* pBuffer, u32 len) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s8 defaultErr = p->status == WPAD_ERR_NO_CONTROLLER ? WPAD_ERR_NO_CONTROLLER : WPAD_ERR_INVALID;
    u32 size = __wpadFmt2Size(p->dataFormat);
    int i;
    if (pBuffer != NULL) {
        memset(pBuffer, 0, size * len);
        for (i = 0; (u32)i < len; i++) {
            ((WPADStatus*)((u8*)pBuffer + i * size))->err = defaultErr;
        }
        p->samplingBufIndex = -1;
        p->samplingBufSize = len;
    }
    p->samplingBuf = pBuffer;
    OSRestoreInterrupts(enabled);
}

void WPADiExcludeButton(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    u8 rxBufIndex = p->rxBufIndex != 0 ? 0 : 1;
    void* pRxBuffer = p->rxBufs[rxBufIndex];

    WPADStatus* pStatus;
    WPADCLStatus* pStatusCL;

    pStatus = (WPADStatus*)pRxBuffer;

    if ((pStatus->button & (WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT)) ==
        (WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT)) {

        pStatus->button &= ~WPAD_BUTTON_RIGHT;
    }

    if ((pStatus->button & (WPAD_BUTTON_UP | WPAD_BUTTON_DOWN)) ==
        (WPAD_BUTTON_UP | WPAD_BUTTON_DOWN)) {

        pStatus->button &= ~WPAD_BUTTON_DOWN;
    }

    if (p->dataFormat == WPAD_FMT_CLASSIC_BTN ||
        p->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC ||
        p->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC_DPD ||
        p->dataFormat == WPAD_FMT_GUITAR || p->dataFormat == WPAD_FMT_DRUM) {
        pStatusCL = (WPADCLStatus*)pRxBuffer;

        if ((pStatusCL->clButton &
             (WPAD_BUTTON_CL_LEFT | WPAD_BUTTON_CL_RIGHT)) ==
            (WPAD_BUTTON_CL_LEFT | WPAD_BUTTON_CL_RIGHT)) {

            pStatusCL->clButton &= ~WPAD_BUTTON_CL_RIGHT;
        }

        if ((pStatusCL->clButton & (WPAD_BUTTON_CL_UP | WPAD_BUTTON_CL_DOWN)) ==
            (WPAD_BUTTON_CL_UP | WPAD_BUTTON_CL_DOWN)) {

            pStatusCL->clButton &= ~WPAD_BUTTON_CL_DOWN;
        }
    }

    if (p->dataFormat == WPAD_FMT_TRAIN) {
        WPADTRStatus* pStatusTR = (WPADTRStatus*)pStatus;
        if ((pStatusTR->trButton & (WPAD_BUTTON_CL_LEFT | WPAD_BUTTON_CL_RIGHT)) ==
            (WPAD_BUTTON_CL_LEFT | WPAD_BUTTON_CL_RIGHT)) {
            pStatusTR->trButton &= ~WPAD_BUTTON_CL_RIGHT;
        }
        if ((pStatusTR->trButton & (WPAD_BUTTON_CL_UP | WPAD_BUTTON_CL_DOWN)) ==
            (WPAD_BUTTON_CL_UP | WPAD_BUTTON_CL_DOWN)) {
            pStatusTR->trButton &= ~WPAD_BUTTON_CL_DOWN;
        }
    }
    OSRestoreInterrupts(enabled);
}

void WPADiCopyOut(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 rxBufIndex = p->rxBufIndex != 0 ? 0 : 1;
    WPADStatus* pStatus = (WPADStatus*)p->rxBufs[rxBufIndex];
    u32 size = __wpadFmt2Size(p->dataFormat);
    if (p->samplingBuf != NULL) {
        void* dest;
        if (++p->samplingBufIndex >= p->samplingBufSize) {
            p->samplingBufIndex = 0;
        }
        dest = (u8*)p->samplingBuf + p->samplingBufIndex * size;
        if (pStatus->err != WPAD_ERR_OK) {
            size = sizeof(WPADStatus);
        }
        memcpy(dest, pStatus, size);
    }
    if (p->samplingCB != NULL) {
        p->samplingCB(chan);
    }
    p->copyOutCount++;
    OSRestoreInterrupts(enabled);
}

BOOL WPADIsSpeakerEnabled(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    BOOL spkEnabled = p->info.speaker;

    OSRestoreInterrupts(enabled);
    return spkEnabled;
}

WPADResult WPADControlSpeaker(WPADChannel chan, u32 command, WPADCallback pCallback) {
    WPADCB* p;
    BOOL enabled;
    BOOL spkEnable;
    BOOL handshake;
    s32 status;

    u8 config[SPK_CONFIG_SIZE] = {0x00,
                                  0x00,                            // 4-bit ADPCM
                                  0xD0, 0x07,                      // 3000Hz (little-endian)
                                  WPAD_MAX_SPEAKER_VOLUME / 2 + 1, // Default volume
                                  0x0C, 0x0E                       // ???
    };

    p = _wpdcb[chan];

    enabled = OSDisableInterrupts();

    spkEnable = p->info.speaker;
    status = p->status;
    handshake = p->handshakeFinished;

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto _end;
    }

    if (!handshake) {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto _end;
    }

    if (command == WPAD_SPEAKER_OFF) {
        if (!spkEnable) {
            status = WPAD_ERR_OK;
        } else {
            enabled = OSDisableInterrupts();

            if (__CanPushCmdQueue(&p->stdCmdQueue, 5)) {
                WPADiSendMuteSpeaker(&p->stdCmdQueue, TRUE, NULL);

                WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x01, WM_REG_SPEAKER_01,
                                      NULL);
                WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x00, WM_REG_SPEAKER_09,
                                      NULL);

                WPADiSendEnableSpeaker(&p->stdCmdQueue, FALSE, NULL);

                WPADiSendGetContStat(&p->stdCmdQueue, NULL, pCallback);

                OSRestoreInterrupts(enabled);
                return WPAD_ERR_OK;
            }

            status = WPAD_ERR_COMMUNICATION_ERROR;
            OSRestoreInterrupts(enabled);
        }
    } else {
        switch (command) {
        case WPAD_SPEAKER_ON:
        case WPAD_SPEAKER_5: {
            enabled = OSDisableInterrupts();

            if (__CanPushCmdQueue(&p->stdCmdQueue, 7)) {
                WPADiSendEnableSpeaker(&p->stdCmdQueue, TRUE, NULL);
                WPADiSendMuteSpeaker(&p->stdCmdQueue, TRUE, NULL);

                WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x01, WM_REG_SPEAKER_09,
                                      NULL);
                WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x80, WM_REG_SPEAKER_01,
                                      NULL);

                config[4] = _speakerVolume;

                WPADiSendWriteData(&p->stdCmdQueue, &config, SPK_CONFIG_SIZE,
                                   WM_REG_SPEAKER_01, NULL);

                WPADiSendMuteSpeaker(&p->stdCmdQueue, FALSE, NULL);

                WPADiSendGetContStat(&p->stdCmdQueue, NULL, pCallback);

                OSRestoreInterrupts(enabled);
                return WPAD_ERR_OK;
            }

            status = WPAD_ERR_COMMUNICATION_ERROR;

            OSRestoreInterrupts(enabled);
            break;
        }

        case WPAD_SPEAKER_MUTE: {
            if (!WPADiSendMuteSpeaker(&p->stdCmdQueue, TRUE, pCallback)) {
                status = WPAD_ERR_COMMUNICATION_ERROR;
                break;
            }

            return WPAD_ERR_OK;
        }

        case WPAD_SPEAKER_UNMUTE: {
            if (!WPADiSendMuteSpeaker(&p->stdCmdQueue, FALSE, pCallback)) {
                status = WPAD_ERR_COMMUNICATION_ERROR;
                break;
            }

            return WPAD_ERR_OK;
        }

        case WPAD_SPEAKER_PLAY: {
            if (!WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x01, WM_REG_SPEAKER_08,
                                       pCallback)) {

                status = WPAD_ERR_COMMUNICATION_ERROR;
                break;
            }

            return WPAD_ERR_OK;
        }
        }
    }

_end:
    if (pCallback != NULL) {
        pCallback(chan, status);
    }

    return status;
}

u8 WPADGetSpeakerVolume(void) {
    BOOL enabled = OSDisableInterrupts();
    u8 volume = _speakerVolume;
    OSRestoreInterrupts(enabled);
    return volume;
}

void WPADSetSpeakerVolume(u8 volume) {
    BOOL enabled = OSDisableInterrupts();
    _speakerVolume = __ClampSpeakerVolume(volume);
    OSRestoreInterrupts(enabled);
}

static BOOL IsBusyStream(s32 chan) {
    BOOL enabled;
    WPADCB* p = _wpdcb[chan];
    u8 radioQuality;
    u32 devType;

    u8 bufferStatus;
    u16 bteBufferStatus;
    u16 btmBufferStatus;

    u8 audioFrames;
    s8 queueSize;
    u8 linkNumber;

    enabled = OSDisableInterrupts();

    radioQuality = p->radioQuality;
    devType = p->devType;
    bufferStatus = WUDGetBufferStatus();

    queueSize = __GetCmdNumber(&p->stdCmdQueue);

    bteBufferStatus = _WPADGetStackBufferStatus(chan);
    btmBufferStatus = _WPADGetModuleBufferStatus(chan);
    audioFrames = p->audioFrames;

    linkNumber = _WUDGetLinkNumber();

    OSRestoreInterrupts(enabled);

    if (radioQuality != WPAD_RADIO_QUALITY_GOOD || btmBufferStatus > 3 ||
        bufferStatus == 10 || bufferStatus >= linkNumber * 2 + 2 ||
        devType == WPAD_DEV_UNKNOWN || queueSize >= 21 || audioFrames >= 1) {

        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL WPADCanSendStreamData(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    BOOL handshake;
    s32 status;

    enabled = OSDisableInterrupts();
    status = p->status;
    handshake = p->handshakeFinished;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER || !handshake || IsBusyStream(chan)) {

        return FALSE;
    } else {
        return TRUE;
    }
}

s32 WPADSendStreamData(s32 chan, void* pData, u16 len) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    BOOL handshake;
    s32 status;

    enabled = OSDisableInterrupts();
    status = p->status;
    handshake = p->handshakeFinished;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return WPAD_ERR_NO_CONTROLLER;
    }

    if (!handshake) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    if (IsBusyStream(chan)) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    if (!WPADiSendStreamData(&p->stdCmdQueue, pData, len)) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    enabled = OSDisableInterrupts();
    p->audioFrames++;
    OSRestoreInterrupts(enabled);

    return WPAD_ERR_OK;
}

u8 WPADGetDpdSensitivity(void) {
    BOOL enabled = OSDisableInterrupts();
    u8 sensitivity = _dpdSensitivity;
    OSRestoreInterrupts(enabled);
    return sensitivity;
}

BOOL WPADSetSensorBarPower(BOOL enable) {
    u32 oldReg;
    u32 newReg;
    BOOL enabled;
    BOOL old;

    enabled = OSDisableInterrupts();
    oldReg = ACRReadReg(ACR_GPIO1BOUT);

    if (enable) {
        newReg = oldReg | GPIO_SENSORBAR;
    } else {
        newReg = oldReg & ~GPIO_SENSORBAR;
    }

    ACRWriteReg(ACR_GPIO1BOUT, newReg);
    old = oldReg & GPIO_SENSORBAR ? TRUE : FALSE;

    OSRestoreInterrupts(enabled);
    return old;
}

BOOL WPADIsDpdEnabled(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    BOOL dpdEnabled = p->info.dpd;

    OSRestoreInterrupts(enabled);
    return dpdEnabled;
}

u8 WPADGetDpdFormat(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 format = p->info.dpd ? p->currentDpdCommand : WPAD_DPD_DISABLE;
    OSRestoreInterrupts(enabled);
    return format;
}

static void __dpdCb(WPADChannel chan, WPADResult result) {
    WPADCB* p = _wpdcb[chan];

    p->currentDpdCommand = p->pendingDpdCommand;
    p->dpdBusy = FALSE;
    p->info.dpd = p->pendingDpdCommand == WPAD_DPD_DISABLE ? 0 : 1;
}

s32 WPADControlDpd(s32 chan, u32 command, WPADCallback pCallback) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    BOOL dpdEnabled;
    BOOL handshake;
    u8 currCmd;
    u8 pendingCmd;
    s32 status;

    static const u8 cfg1[WPAD_MAX_DPD_SENS][DPD_CONFIG1_SIZE] = {
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x64, 0x00, 254},
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96, 0x00, 180},
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xAA, 0x00, 100},
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xC8, 0x00,  54},
        {0x07, 0x00, 0x00, 0x71, 0x01, 0x00, 0x72, 0x00,  32}
    };

    static const u8 cfg2[WPAD_MAX_DPD_SENS][DPD_CONFIG2_SIZE] = {
        {0xFD, 0x05},
        {0xB3, 0x04},
        {0x63, 0x03},
        {0x35, 0x03},
        {0x1F, 0x03}
    };

    enabled = OSDisableInterrupts();

    dpdEnabled = p->info.dpd;
    currCmd = p->currentDpdCommand;
    pendingCmd = p->pendingDpdCommand;
    status = p->status;
    handshake = p->handshakeFinished;

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto _end;
    }

    if (!handshake) {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto _end;
    }

    if (command == WPAD_DPD_DISABLE) {
        if (!dpdEnabled) {
            status = WPAD_ERR_OK;
        } else {
            enabled = OSDisableInterrupts();

            if (__CanPushCmdQueue(&p->stdCmdQueue, 3)) {
                p->pendingDpdCommand = command;

                WPADiSendEnableDPD(&p->stdCmdQueue, FALSE, NULL);
                WPADiSendDPDCSB(&p->stdCmdQueue, FALSE, &__dpdCb);

                WPADiSendGetContStat(&p->stdCmdQueue, NULL, pCallback);

                OSRestoreInterrupts(enabled);
                return WPAD_ERR_OK;
            }

            status = WPAD_ERR_COMMUNICATION_ERROR;

            OSRestoreInterrupts(enabled);
        }
    } else if (command != pendingCmd) {
        enabled = OSDisableInterrupts();

        if (__CanPushCmdQueue(&p->stdCmdQueue, 8)) {
            p->pendingDpdCommand = command;
            p->dpdBusy = TRUE;

            WPADiSendEnableDPD(&p->stdCmdQueue, TRUE, NULL);
            WPADiSendDPDCSB(&p->stdCmdQueue, TRUE, NULL);

            WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x01, WM_REG_DPD_30, NULL);

            WPADiSendWriteData(&p->stdCmdQueue, cfg1[_dpdSensitivity - 1],
                               DPD_CONFIG1_SIZE, WM_REG_DPD_CONFIG_BLOCK_1,
                               NULL);

            WPADiSendWriteData(&p->stdCmdQueue, cfg2[_dpdSensitivity - 1],
                               DPD_CONFIG2_SIZE, WM_REG_DPD_CONFIG_BLOCK_2,
                               NULL);

            WPADiSendWriteDataCmd(&p->stdCmdQueue, command,
                                  WM_REG_DPD_DATA_FORMAT, NULL);
            WPADiSendWriteDataCmd(&p->stdCmdQueue, 0x08, WM_REG_DPD_30,
                                  &__dpdCb);

            WPADiSendGetContStat(&p->stdCmdQueue, NULL, pCallback);

            OSRestoreInterrupts(enabled);
            return WPAD_ERR_OK;
        }

        status = WPAD_ERR_COMMUNICATION_ERROR;

        OSRestoreInterrupts(enabled);
    }

_end:
    if (pCallback != NULL) {
        pCallback(chan, status);
    }

    return status;
}



WPADResult WPADControlBLC(WPADChannel chan, s32 command, WPADCallback callback) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p->status;
    BOOL handshakeFinished = p->handshakeFinished;
    OSRestoreInterrupts(enabled);
    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }
    if (!handshakeFinished || !WUDIsLinkedWBC()) {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto end;
    } else {
        u8 data[7];
        data[0] = data[1] = data[2] = 0xAA;
        data[3] = 0x55;
        data[4] = data[5] = data[6] = command;
        enabled = OSDisableInterrupts();
        switch (command) {
        case 0xAA:
            if (__CanPushCmdQueue(&p->stdCmdQueue, 4)) {
                WPADWriteExtReg(chan, data, sizeof(data), 0xF1, NULL);
                WPADWriteExtReg(chan, data, 1, 0xF1, NULL);
                WPADWriteExtReg(chan, data, 1, 0xF1, NULL);
                WPADWriteExtReg(chan, data, 1, 0xF1, callback);
                OSRestoreInterrupts(enabled);
                return WPAD_ERR_OK;
            }
            status = WPAD_ERR_COMMUNICATION_ERROR;
            break;
        case 0x55:
            status = WPADWriteExtReg(chan, data, sizeof(data), 0xF1, callback);
            if (status == WPAD_ERR_OK) {
                OSRestoreInterrupts(enabled);
                return WPAD_ERR_OK;
            }
            break;
        case 0:
            status = WPADWriteExtReg(chan, data, 1, 0xF1, callback);
            if (status == WPAD_ERR_OK) {
                OSRestoreInterrupts(enabled);
                return WPAD_ERR_OK;
            }
            break;
        default:
            status = WPAD_ERR_COMMUNICATION_ERROR;
            break;
        }
    }
    OSRestoreInterrupts(enabled);
end:
    if (callback != NULL) { callback(chan, status); }
    return status;
}

BOOL WPADiSendSetPort(WPADCommandQueue* pQueue, u8 port,
                      WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_SET_PORT;
    command.dataLength = RPT11_SIZE;
    command.dataBuf[RPT11_LED] = port << 4;
    command.cmdCB = pCallback;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendSetReportType(WPADCommandQueue* pQueue, s32 format, BOOL disableContReport,
                            WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_SET_DATA_REPORT_MODE;
    command.dataLength = RPT12_SIZE;
    command.dataBuf[RPT12_CONT_REPORT] = disableContReport ? 0 : 4;
    command.cmdCB = pCallback;

    switch (format) {
    case WPAD_FMT_CORE_BTN: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN;
        break;
    }
    case WPAD_FMT_CORE_BTN_ACC: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC;
        break;
    }

    case WPAD_FMT_CORE_BTN_ACC_DPD: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_DPD12;
        break;
    }

    case WPAD_FMT_FS_BTN: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_EXT8;
        break;
    }

    case WPAD_FMT_FS_BTN_ACC: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_EXT16;
        break;
    }

    case WPAD_FMT_FS_BTN_ACC_DPD: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_DPD10_EXT9;
        break;
    }

    case WPAD_FMT_CLASSIC_BTN: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_EXT8;
        break;
    }

    case WPAD_FMT_CLASSIC_BTN_ACC: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_EXT16;
        break;
    }

    case WPAD_FMT_CLASSIC_BTN_ACC_DPD: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_DPD10_EXT9;
        break;
    }

    case WPAD_FMT_BTN_ACC_DPD_EXTENDED: {
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_DPD18_1;
        break;
    }
    case WPAD_FMT_TRAIN:
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_EXT8;
        break;
    case WPAD_FMT_GUITAR:
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_DPD10_EXT9;
        break;
    case WPAD_FMT_DRUM:
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_DPD10_EXT9;
        break;
    case WPAD_FMT_BALANCE_CHECKER:
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_EXT19;
        break;
    case WPAD_FMT_VSM:
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_BTN_ACC_EXT16;
        break;
    case WPAD_FMT_14:
        command.dataBuf[RPT12_DATA_REPORT_MODE] = RPTID_DATA_EXT21;
        break;
    }

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendEnableDPD(WPADCommandQueue* pQueue, BOOL enable,
                        WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_ENABLE_DPD;
    command.dataLength = RPT13_SIZE;
    command.dataBuf[RPT13_DPD_ENABLE] = enable ? 4 : 0;
    command.cmdCB = pCallback;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendEnableSpeaker(WPADCommandQueue* pQueue, BOOL enable,
                            WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_ENABLE_SPEAKER;
    command.dataLength = RPT14_SIZE;
    command.dataBuf[RPT14_SPEAKER_ENABLE] = enable ? 4 : 0;
    command.cmdCB = pCallback;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendGetContStat(WPADCommandQueue* pQueue, WPADInfo* pInfo,
                          WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_REQUEST_STATUS;
    command.dataLength = RPT15_SIZE;
    command.dataBuf[0] = 0;
    command.cmdCB = pCallback;
    command.statusReportOut = pInfo;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendWriteDataCmd(WPADCommandQueue* pQueue, u8 cmd, u32 addr,
                           WPADCallback pCallback) {

    return WPADiSendWriteData(pQueue, &cmd, sizeof(u8), addr, pCallback);
}

BOOL WPADiSendWriteData(WPADCommandQueue* pQueue, const void* pSrc, u16 len,
                        u32 addr, WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;
    u8 packedLen = len & 31;

    command.reportID = RPTID_WRITE_DATA;
    command.dataLength = RPT16_SIZE;
    command.cmdCB = pCallback;
    memcpy(&command.dataBuf[RPT16_DATA_DST_ADDRESS], &addr, sizeof(u32));
    memcpy(&command.dataBuf[RPT16_DATA_LENGTH], &packedLen, sizeof(u8));
    memcpy(&command.dataBuf[RPT16_DATA], pSrc, len);

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendReadData(WPADCommandQueue* pQueue, void* pDst, u16 len, u32 addr,
                       WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_READ_DATA;
    command.dataLength = RPT17_SIZE;
    command.cmdCB = pCallback;
    memcpy(&command.dataBuf[RPT17_DATA_SRC_ADDRESS], &addr, sizeof(u32));
    memcpy(&command.dataBuf[RPT17_DATA_LENGTH], &len, sizeof(u16));
    command.dstBuf = pDst;
    command.readLength = len;
    command.readAddress = addr;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendStreamData(WPADCommandQueue* pQueue, const void* pData, u16 len) {
    BOOL success;
    WPADCommand command;
    u8 packedLen = len << 3;

    command.reportID = RPTID_SEND_SPEAKER_DATA;
    command.dataLength = sizeof(command.dataBuf);
    command.dataBuf[RPT18_DATA_LENGTH] = packedLen;
    command.cmdCB = NULL;
    memcpy(&command.dataBuf[RPT18_DATA], pData, len);

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendMuteSpeaker(WPADCommandQueue* pQueue, BOOL mute,
                          WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_MUTE_SPEAKER;
    command.dataLength = RPT19_SIZE;
    command.dataBuf[RPT19_SPEAKER_MUTE] = mute ? 4 : 0;
    command.cmdCB = pCallback;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL WPADiSendDPDCSB(WPADCommandQueue* pQueue, BOOL enable,
                     WPADCallback pCallback) {
    BOOL success;
    WPADCommand command;

    command.reportID = RPTID_SEND_DPD_CSB;
    command.dataLength = RPT1A_SIZE;
    command.dataBuf[RPT1A_DPD_CSB] = enable ? 4 : 0;
    command.cmdCB = pCallback;

    success = WPADiPushCommand(pQueue, command);
    return success;
}

BOOL __CanPushCmdQueue(const WPADCommandQueue* pQueue, s8 num) {
    s8 queueSize = __GetCmdNumber(pQueue);

    if ((u32)(queueSize + num) <= pQueue->capacity - 1) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s8 __GetCmdNumber(const WPADCommandQueue* pQueue) {
    BOOL enabled;
    s8 remain;

    enabled = OSDisableInterrupts();

    remain = pQueue->back - pQueue->front;
    if (remain < 0) {
        remain += pQueue->capacity;
    }

    OSRestoreInterrupts(enabled);

    return remain;
}

void WPADiClearQueue(WPADCommandQueue* pQueue) {
    BOOL enabled = OSDisableInterrupts();

    pQueue->front = 0;
    pQueue->back = 0;
    memset(pQueue->buffer, 0, pQueue->capacity * sizeof(WPADCommand));

    OSRestoreInterrupts(enabled);
}

BOOL WPADiPushCommand(WPADCommandQueue* pQueue, WPADCommand command) {
    BOOL enabled = OSDisableInterrupts();

    if (pQueue->capacity - 1 == __GetCmdNumber(pQueue)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    // @bug Useless memset
    memset(&pQueue->buffer[pQueue->back], 0, sizeof(WPADCommand));
    memcpy(&pQueue->buffer[pQueue->back], &command, sizeof(WPADCommand));

    pQueue->back = pQueue->back == pQueue->capacity - 1 ? 0 : pQueue->back + 1;

    OSRestoreInterrupts(enabled);

    return TRUE;
}

BOOL WPADiGetCommand(const WPADCommandQueue* pQueue, WPADCommand* pCommand) {
    BOOL enabled;
    if (__GetCmdNumber(pQueue) == 0) {
        return FALSE;
    }
    enabled = OSDisableInterrupts();
    memcpy(pCommand, &pQueue->buffer[pQueue->front], sizeof(WPADCommand));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL WPADiPopCommand(WPADCommandQueue* pQueue) {
    BOOL enabled = OSDisableInterrupts();

    if (__GetCmdNumber(pQueue) == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    memset(&pQueue->buffer[pQueue->front], 0, sizeof(WPADCommand));

    pQueue->front =
        pQueue->front == pQueue->capacity - 1 ? 0 : pQueue->front + 1;

    OSRestoreInterrupts(enabled);

    return TRUE;
}

static void __SetScreenSaverFlag(BOOL disable) {
    if (disable) {
        __VIResetRFIdle();
    }
}

static u8 __GetDpdSensitivity(void) {
    u8 result = SCGetBtDpdSensibility();

    if (result < WPAD_MIN_DPD_SENS) {
        result = WPAD_MIN_DPD_SENS;
    }

    if (result > WPAD_MAX_DPD_SENS) {
        result = WPAD_MAX_DPD_SENS;
    }

    return result;
}

static u8 __GetSensorBarPosition(void) {
    // SC_SENSOR_BAR_TOP ? WPAD_SENSOR_BAR_TOP : WPAD_SENSOR_BAR_BOTTOM
    return !!(SCGetWpadSensorBarPosition() == 1);
}

static u32 __GetMotorMode(void) {
    // SC_MOTOR_ON ? WPAD_MOTOR_ON : WPAD_MOTOR_OFF
    return !!(SCGetWpadMotorMode() == 1);
}

static u8 __ClampSpeakerVolume(u8 volume) {
    u8 result = volume;

    if (volume <= 0) {
        result = 0;
    }

    if (volume >= WPAD_MAX_SPEAKER_VOLUME) {
        result = WPAD_MAX_SPEAKER_VOLUME;
    }

    return result;
}

static u8 __GetSpeakerVolume(void) {
    u8 volume = SCGetWpadSpeakerVolume();
    return __ClampSpeakerVolume(volume);
}

u16 _WPADGetStackBufferStatus(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    s32 status;
    s8 handle;

    enabled = OSDisableInterrupts();
    status = p->status;
    handle = p->devHandle;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return 0;
    }

    return _WUDGetQueuedSize(handle);
}

u16 _WPADGetModuleBufferStatus(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enabled;
    s32 status;
    s8 handle;

    enabled = OSDisableInterrupts();
    status = p->status;
    handle = p->devHandle;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return 0;
    }

    return _WUDGetNotAckedSize(handle);
}

void WPADRecalibrate(s32 chan) {
    WPADCB* p = _wpdcb[chan];
    BOOL enable = OSDisableInterrupts();

    p->calibrated = FALSE;
    p->comboHeld = 0;

    OSRestoreInterrupts(enable);
}

BOOL WPADIsUsedCallbackByKPAD(void) {
    return _wpadUsedCallback;
}

void WPADSetCallbackByKPAD(BOOL isKPAD) {
    _wpadUsedCallback = isKPAD;
}
