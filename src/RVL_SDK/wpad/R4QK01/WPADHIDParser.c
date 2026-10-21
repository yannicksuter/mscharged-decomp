#include "revolution/wpad.h"
#include <math.h>
#include <string.h>
#include <revolution/wpad/WPADEncrypt.h>
#include <stdio.h>

extern void DEBUGPrint(const char*, ...);

static u8 _devType[WPAD_MAX_CONTROLLERS];
static u8 _devMode[WPAD_MAX_CONTROLLERS];
static u8 _devCmpt[WPAD_MAX_CONTROLLERS];
static u8 _retryCnt[WPAD_MAX_CONTROLLERS];
static const u8 checkInvalidData[21] = {
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
};
static u8 checkBuffer[21];
static f32 rolag[WPAD_MAX_CONTROLLERS];
static f32 calibX[WPAD_MAX_CONTROLLERS];
static f32 calibY[WPAD_MAX_CONTROLLERS];
static f32 centerX[WPAD_MAX_CONTROLLERS];
static f32 centerY[WPAD_MAX_CONTROLLERS];

void __a1_20_status_report(u8 chan, u8* data, void* rxBuffer);
void __a1_21_user_data(u8 chan, u8* data, void* rxBuffer);
void __a1_22_ack(u8 chan, u8* data, void* rxBuffer);
void __a1_30_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_31_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_32_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_33_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_34_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_35_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_36_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_37_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_3d_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_3e_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_3f_data_type(u8 chan, u8* data, void* rxBuffer);
void __a1_unused_report(u8 chan, u8* data, void* rxBuffer);

#define HID_WPAD_BUTTON_MASK  (u16)(0x9F1F)
#define HID_WPAD_BUTTON_CLEAR (u16)(0x60E0)
#define HID_NEAREMPTY_MASK    (u8)(0x80)
#define HID_DPD_MASK          (u8)(0x8)
#define HID_SPEAKER_MASK      (u8)(0x4)
#define HID_ATTACH_MASK       (u8)(0x2)
#define HID_LOWBAT_MASK       (u8)(0x1)
#define HID_LED_MASK          (u8)(0xF0)
#define HID_FW_MASK           (u8)(0xF0)
#define HID_PRT_MASK          (u8)(0xF)

#define HID_IDX_BYTE7  1
#define HID_IDX_BYTE8  2
#define HID_IDX_BYTE9  3
#define HID_IDX_BYTE10 4
#define HID_IDX_BYTE11 5
#define HID_IDX_BYTE12 6
#define HID_IDX_BYTE13 7
#define HID_IDX_BYTE14 8
#define HID_IDX_BYTE15 9
#define HID_IDX_BYTE16 10
#define HID_IDX_BYTE17 11
#define HID_IDX_BYTE18 12
#define HID_IDX_BYTE19 13
#define HID_IDX_BYTE20 14
#define HID_IDX_BYTE21 15
#define HID_IDX_BYTE22 16
#define HID_IDX_BYTE23 17
#define HID_IDX_BYTE24 18
#define HID_IDX_BYTE25 19
#define HID_IDX_BYTE26 20
#define HID_IDX_BYTE27 21

void (*__a1_input_reports_array[])(u8 chan, u8* data, void* rxBuffer) = {
    __a1_20_status_report, __a1_21_user_data, __a1_22_ack, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_30_data_type, __a1_31_data_type, __a1_32_data_type, __a1_33_data_type, __a1_34_data_type, __a1_35_data_type, __a1_36_data_type, __a1_37_data_type, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_unused_report, __a1_3d_data_type, __a1_3e_data_type, __a1_3f_data_type
};

static inline void initExtension(s32);

void abortInitExtension(WPADChannel chan, WPADResult result)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u32 type;

    if (result != WPAD_ERR_OK)
    {
        WPADiClearQueue(&p_wpd->extCmdQueue);
        p_wpd->extState = 0;
        if (result == WPAD_ERR_NO_CONTROLLER)
        {
            type = WPAD_DEV_NOT_FOUND;
        }

        else if (p_wpd->info.attach)
        {
            if (_retryCnt[chan]++ < 32)
            {
                initExtension(chan);
                return;
            }
            type = WPAD_DEV_NOT_SUPPORTED;
        }

        else
        {
            DEBUGPrint("detaching extension during initialization.\n");
            WPADiSendSetReportType(&p_wpd->extCmdQueue, p_wpd->dataFormat, p_wpd->disableContReport, NULL);
            return;
        }
        p_wpd->devType = (u8)type;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extWasDisconnected = FALSE;
        if (p_wpd->extensionCallback)
        {
            p_wpd->extensionCallback(chan, (s32)type);
        }
    }
}

extern double tan(double);

void getDevConfig(WPADChannel chan, WPADResult result)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8* p_buf = (u8*)p_wpd->wmReadDataPtr;
    u8 checkSum;
    u8 i;
    int j;
    int index;
    DPDObject obj[WPAD_DPD_MAX_OBJECTS];
    const s16 dummyObjX[] = { 127, 896, 896, 127 };
    const s16 dummyObjY[] = { 93, 93, 674, 674 };
    f32 difaveX;
    f32 difaveY;
    f32 deltaX;
    f32 deltaY;
    f32 x[WPAD_DPD_MAX_OBJECTS];
    f32 y[WPAD_DPD_MAX_OBJECTS];
    f32 ag[WPAD_DPD_MAX_OBJECTS];
    f32 ag_org[WPAD_DPD_MAX_OBJECTS];

    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
    {
        p_wpd->devConf.obj[i].x = dummyObjX[i];
        p_wpd->devConf.obj[i].y = dummyObjY[i];
        p_wpd->devConf.obj[i].size = (u16)p_wpd->dpdDummyObjSize;
        p_wpd->devConf.obj[i].traceId = i;
    }

    p_wpd->devConf.acc_0g.x = 530;
    p_wpd->devConf.acc_0g.y = 530;
    p_wpd->devConf.acc_0g.z = 530;
    p_wpd->devConf.acc_1g.x = 636;
    p_wpd->devConf.acc_1g.y = 636;
    p_wpd->devConf.acc_1g.z = 636;
    p_wpd->devConf.volume = 0;
    p_wpd->devConf.motor = 0;

    if (result != WPAD_ERR_OK)
    {
        return;
    }

    if (!p_wpd->oldFw)
    {
        for (i = 0; i < 2; i++)
        {
            checkSum = 0;
            index = i * (11);
            for (j = index; j < index + 10; j++)
            {
                checkSum += p_buf[j];
            }
            checkSum = (u8)(checkSum + 0x55);

            if (checkSum == p_buf[index + 10])
            {
                DEBUGPrint("Dpd Setting is ok.\n");

                obj[0].x = (s16)((s16)((s16)(((u16)(p_buf[index])) & 0xFF) | (u16)(((u16)(p_buf[index + 2] & 0x30)) << 4)));
                obj[0].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(p_buf[index + 1])) & 0xFF) | (u16)(((u16)(p_buf[index + 2] & 0xC0)) << 2)));
                obj[0].size = (u16)p_wpd->dpdDummyObjSize;
                obj[0].traceId = 0;

                obj[1].x = (s16)((s16)((s16)(((u16)(p_buf[index + 3])) & 0xFF) | (u16)(((u16)(p_buf[index + 2] & (u8)(0x30 >> 4))) << 8)));
                obj[1].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(p_buf[index + 4])) & 0xFF) | (u16)(((u16)(p_buf[index + 2] & (u8)(0xC0 >> 4))) << 6)));
                obj[1].size = (u16)p_wpd->dpdDummyObjSize;
                obj[1].traceId = 1;

                obj[2].x = (s16)((s16)((s16)(((u16)(p_buf[index + 5])) & 0xFF) | (u16)(((u16)(p_buf[index + 7] & 0x30)) << 4)));
                obj[2].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(p_buf[index + 6])) & 0xFF) | (u16)(((u16)(p_buf[index + 7] & 0xC0)) << 2)));
                obj[2].size = (u16)p_wpd->dpdDummyObjSize;
                obj[2].traceId = 2;

                obj[3].x = (s16)((s16)((s16)(((u16)(p_buf[index + 8])) & 0xFF) | (u16)(((u16)(p_buf[index + 7] & (u8)(0x30 >> 4))) << 8)));
                obj[3].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)(p_buf[index + 9])) & 0xFF) | (u16)(((u16)(p_buf[index + 7] & (u8)(0xC0 >> 4))) << 6)));
                obj[3].size = (u16)p_wpd->dpdDummyObjSize;
                obj[3].traceId = 3;

                for (j = 0; j < WPAD_DPD_MAX_OBJECTS; j++)
                {
                    if ((obj[j].x < (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y < (s16)(WPAD_DPD_IMG_RESO_WY / 2)))
                    {
                        p_wpd->devConf.obj[0].x = obj[j].x;
                        p_wpd->devConf.obj[0].y = obj[j].y;
                        p_wpd->devConf.obj[0].size = obj[j].size;
                        p_wpd->devConf.obj[0].traceId = obj[j].traceId;
                    }
                    if ((obj[j].x > (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y < (s16)(WPAD_DPD_IMG_RESO_WY / 2)))
                    {
                        p_wpd->devConf.obj[1].x = obj[j].x;
                        p_wpd->devConf.obj[1].y = obj[j].y;
                        p_wpd->devConf.obj[1].size = obj[j].size;
                        p_wpd->devConf.obj[1].traceId = obj[j].traceId;
                    }
                    if ((obj[j].x > (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y > (s16)(WPAD_DPD_IMG_RESO_WY / 2)))
                    {
                        p_wpd->devConf.obj[2].x = obj[j].x;
                        p_wpd->devConf.obj[2].y = obj[j].y;
                        p_wpd->devConf.obj[2].size = obj[j].size;
                        p_wpd->devConf.obj[2].traceId = obj[j].traceId;
                    }
                    if ((obj[j].x < (s16)(WPAD_DPD_IMG_RESO_WX / 2)) && (obj[j].y > (s16)(WPAD_DPD_IMG_RESO_WY / 2)))
                    {
                        p_wpd->devConf.obj[3].x = obj[j].x;
                        p_wpd->devConf.obj[3].y = obj[j].y;
                        p_wpd->devConf.obj[3].size = obj[j].size;
                        p_wpd->devConf.obj[3].traceId = obj[j].traceId;
                    }
                }

                break;
            }
            else
            {
                DEBUGPrint("Dpd Setting is broken.\n");
            }
        }
    }
    else
    {
        DEBUGPrint("Old Firmware uses default values because it has no Dpd Setting.\n");
    }

    difaveX = difaveY = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
    {
        x[i] = (f32)(p_wpd->devConf.obj[i].x);
        y[i] = (f32)(p_wpd->devConf.obj[i].y);
        difaveX += p_wpd->devConf.obj[i].x - dummyObjX[i];
        difaveY += p_wpd->devConf.obj[i].y - dummyObjY[i];

        DEBUGPrint("x = %lf, y = %lf\n", x[i], y[i]);
    }

    difaveX = (f32)((f32)difaveX / (f32)4.0f);
    difaveY = (f32)((f32)difaveY / (f32)4.0f);
    deltaX = (f32)atan((f32)((f32)((f32)126.5f + difaveX) / (f32)(1332.5f))) - (f32)atan((f32)((f32)126.5f / 1332.5f));
    deltaY = (f32)atan((f32)((f32)((f32)93.0f + difaveY) / (f32)(1337.4f))) - (f32)atan((f32)((f32)93.0f / 1337.4f));
    calibX[chan] = (f32)(-1.0f * (f32)tan(deltaX)) * 1332.5f;
    calibY[chan] = (f32)(-1.0f * (f32)tan(deltaY)) * 1337.4f;

    DEBUGPrint("difaveX = %lf, difaveY = %lf\n", difaveX, difaveY);
    DEBUGPrint("deltaX  = %lf, deltaY  = %lf\n", deltaX, deltaY);
    DEBUGPrint("_wpadCalibrationX = %lf, _wpadCalibrationY = %lf\n", calibX[chan], calibY[chan]);

    centerX[chan] = centerY[chan] = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
    {
        x[i] += calibX[chan];
        y[i] += calibY[chan];
        centerX[chan] += x[i];
        centerY[chan] += y[i];
    }

    centerX[chan] = (f32)((f32)centerX[chan] / (f32)4.0f);
    centerY[chan] = (f32)((f32)centerY[chan] / (f32)4.0f);
    DEBUGPrint("centX  = %lf, centY  = %lf\n", centerX[chan], centerY[chan]);

    rolag[chan] = 0.0f;
    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
    {
        ag[i] = (f32)atan((f32)(y[i] - centerY[chan]) / (f32)(x[i] - centerX[chan]));
        ag_org[i] = (f32)atan((f32)((f32)(dummyObjY[i]) - 383.5f) / (f32)((f32)(dummyObjX[i]) - 511.5f));

        rolag[chan] += ag[i] - ag_org[i];
    }

    rolag[chan] = (f32)((f32)rolag[chan] / (f32)4.0f);
    DEBUGPrint("_wpadRolag  = %lf\n", rolag[chan]);

    for (i = 0; i < 2; i++)
    {
        checkSum = 0;

        index = i * (10);
        if (!p_wpd->oldFw)
        {
            index += 22;
        }

        for (j = index; j < index + 9; j++)
        {
            checkSum += p_buf[j];
        }
        checkSum = (u8)(checkSum + 0x55);
        if (checkSum == p_buf[index + 9])
        {
            DEBUGPrint("Acc is ok.\n");
            p_wpd->devConf.acc_0g.x = (s16)((u16)(((u16)(p_buf[index + 0]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 4) & 3));
            p_wpd->devConf.acc_0g.y = (s16)((u16)(((u16)(p_buf[index + 1]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 2) & 3));
            p_wpd->devConf.acc_0g.z = (s16)((u16)(((u16)(p_buf[index + 2]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3])) & 3));
            p_wpd->devConf.acc_1g.x = (s16)((u16)(((u16)(p_buf[index + 4]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 4) & 3));
            p_wpd->devConf.acc_1g.y = (s16)((u16)(((u16)(p_buf[index + 5]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 2) & 3));
            p_wpd->devConf.acc_1g.z = (s16)((u16)(((u16)(p_buf[index + 6]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7])) & 3));
            p_wpd->devConf.volume = (u8)(p_buf[index + 8] & (u8)0x7F);
            p_wpd->devConf.motor = (u8)(p_buf[index + 8] & (u8)0x80);
            break;
        }
        else
        {
            DEBUGPrint("Acc is broken.\n");
        }
    }
    DEBUGPrint("0G:  accX = %d,  accY = %d,  accZ = %d\n", p_wpd->devConf.acc_0g.x, p_wpd->devConf.acc_0g.y, p_wpd->devConf.acc_0g.z);
    DEBUGPrint("1G:  accX = %d,  accY = %d,  accZ = %d\n", p_wpd->devConf.acc_1g.x, p_wpd->devConf.acc_1g.y, p_wpd->devConf.acc_1g.z);
    DEBUGPrint("Volume: %d,   Motor: %d\n", p_wpd->devConf.volume, p_wpd->devConf.motor);
}

void getExtConfig(WPADChannel chan, WPADResult result)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8* p_buf = (u8*)p_wpd->wmReadDataPtr;
    int i, j, k;
    int index = -1;
    int checkSum;

    switch (result)
    {
    case WPAD_ERR_NO_CONTROLLER:
        p_wpd->devType = WPAD_DEV_NOT_FOUND;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extWasDisconnected = FALSE;
        break;

    case WPAD_ERR_OK:
        p_wpd->devType = _devType[chan];
        break;

    default:
        p_wpd->devType = WPAD_DEV_NOT_SUPPORTED;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extWasDisconnected = FALSE;
        break;
    }
    p_wpd->devMode = _devMode[chan];

    if (p_wpd->devType == WPAD_DEV_FREESTYLE || p_wpd->devType == WPAD_DEV_CLASSIC)
    {
        for (i = 0; i < 2; i++)
        {
            checkSum = 0;
            k = i * 16;
            for (j = k; j < k + 14; j++)
            {
                checkSum += p_buf[j];
            }

            if (((u8)(checkSum + 0x55) == p_buf[k + 14]) && ((u8)(checkSum + 0xaa) == p_buf[k + 15]))
            {
                index = k;
            }
        }

        switch (p_wpd->devType)
        {
        case WPAD_DEV_FREESTYLE:
            if (index < 0)
            {
                DEBUGPrint("check sum error.\n");
                p_wpd->extConf.fs.acc_0g.x = 512;
                p_wpd->extConf.fs.acc_0g.y = 512;
                p_wpd->extConf.fs.acc_0g.z = 512;
                p_wpd->extConf.fs.acc_1g.x = 716;
                p_wpd->extConf.fs.acc_1g.y = 716;
                p_wpd->extConf.fs.acc_1g.z = 716;
            }
            else
            {
                p_wpd->extConf.fs.acc_0g.x = (s16)((u16)(((u16)(p_buf[index + 0]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 4) & 3));
                p_wpd->extConf.fs.acc_0g.y = (s16)((u16)(((u16)(p_buf[index + 1]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3]) >> 2) & 3));
                p_wpd->extConf.fs.acc_0g.z = (s16)((u16)(((u16)(p_buf[index + 2]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 3])) & 3));
                p_wpd->extConf.fs.acc_1g.x = (s16)((u16)(((u16)(p_buf[index + 4]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 4) & 3));
                p_wpd->extConf.fs.acc_1g.y = (s16)((u16)(((u16)(p_buf[index + 5]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7]) >> 2) & 3));
                p_wpd->extConf.fs.acc_1g.z = (s16)((u16)(((u16)(p_buf[index + 6]) << 2) & 0xFFFC) | (u16)(((u16)(p_buf[index + 7])) & 3));
                p_wpd->extConf.fs.stick.x_max = (s8)p_buf[index + 8];
                p_wpd->extConf.fs.stick.x_min = (s8)p_buf[index + 9];
                p_wpd->extConf.fs.stick.y_max = (s8)p_buf[index + 11];
                p_wpd->extConf.fs.stick.y_min = (s8)p_buf[index + 12];
                if (!p_wpd->calibrated)
                {
                    p_wpd->extConf.fs.stick.x = (s8)p_buf[index + 10];
                    p_wpd->extConf.fs.stick.y = (s8)p_buf[index + 13];
                }

                DEBUGPrint("0G:  fsaccX = %d,  fsaccY = %d,  fsaccZ = %d\n", p_wpd->extConf.fs.acc_0g.x, p_wpd->extConf.fs.acc_0g.y, p_wpd->extConf.fs.acc_0g.z);
                DEBUGPrint("1G:  fsaccX = %d,  fsaccY = %d,  fsaccZ = %d\n", p_wpd->extConf.fs.acc_1g.x, p_wpd->extConf.fs.acc_1g.y, p_wpd->extConf.fs.acc_1g.z);
                DEBUGPrint("FS:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.fs.stick.x, p_wpd->extConf.fs.stick.x_max, p_wpd->extConf.fs.stick.x_min);
                DEBUGPrint("FS:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.fs.stick.y, p_wpd->extConf.fs.stick.y_max, p_wpd->extConf.fs.stick.y_min);
            }
            break;

        case WPAD_DEV_CLASSIC:
            p_wpd->extConf.cl.lstk.x_max = (s8)p_buf[index + 0];
            p_wpd->extConf.cl.lstk.x_min = (s8)p_buf[index + 1];
            p_wpd->extConf.cl.lstk.y_max = (s8)p_buf[index + 3];
            p_wpd->extConf.cl.lstk.y_min = (s8)p_buf[index + 4];
            p_wpd->extConf.cl.rstk.x_max = (s8)p_buf[index + 6];
            p_wpd->extConf.cl.rstk.x_min = (s8)p_buf[index + 7];
            p_wpd->extConf.cl.rstk.y_max = (s8)p_buf[index + 9];
            p_wpd->extConf.cl.rstk.y_min = (s8)p_buf[index + 10];

            if (!p_wpd->calibrated)
            {
                p_wpd->extConf.cl.lstk.x = (s8)p_buf[index + 2];
                p_wpd->extConf.cl.lstk.y = (s8)p_buf[index + 5];
                p_wpd->extConf.cl.rstk.x = (s8)p_buf[index + 8];
                p_wpd->extConf.cl.rstk.y = (s8)p_buf[index + 11];
                p_wpd->extConf.cl.triggerL = p_buf[index + 12];
                p_wpd->extConf.cl.triggerR = p_buf[index + 13];
            }

            DEBUGPrint("CL:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.cl.lstk.x, p_wpd->extConf.cl.lstk.x_max, p_wpd->extConf.cl.lstk.x_min);
            DEBUGPrint("CL:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.cl.lstk.y, p_wpd->extConf.cl.lstk.y_max, p_wpd->extConf.cl.lstk.y_min);
            DEBUGPrint("CR:  X = %d,  X max = %d,  X min = %d\n", p_wpd->extConf.cl.rstk.x, p_wpd->extConf.cl.rstk.x_max, p_wpd->extConf.cl.rstk.x_min);
            DEBUGPrint("CR:  Y = %d,  Y max = %d,  Y min = %d\n", p_wpd->extConf.cl.rstk.y, p_wpd->extConf.cl.rstk.y_max, p_wpd->extConf.cl.rstk.y_min);
            DEBUGPrint("LR:  L = %d,  R = %d\n", p_wpd->extConf.cl.triggerL, p_wpd->extConf.cl.triggerR);
            break;
        }
    }

    if ((p_wpd->devType != WPAD_DEV_BALANCE_CHECKER || p_wpd->wmReadLength != 1) && p_wpd->cmdBlkCallback && p_wpd->cmdBlkCallback == (WPADCallback)p_wpd->extensionCallback)
    {
        p_wpd->cmdBlkCallback(chan, p_wpd->devType);
        p_wpd->cmdBlkCallback = NULL;
    }
}

static BOOL isExtEncMain(u8 type)
{
    if (type == WPAD_DEV_FREESTYLE || type == WPAD_DEV_CLASSIC || type == WPAD_DEV_BALANCE_CHECKER || type == WPAD_DEV_VSM || type == WPAD_DEV_GUITAR || type == WPAD_DEV_DRUM)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

extern BOOL __OSInIPL;

void getExtType(WPADChannel chan, WPADResult result)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADCmdQueue* cmdq = &p_wpd->extCmdQueue;
    u8* p_buf = (u8*)p_wpd->wmReadDataPtr;

    if (result == WPAD_ERR_OK)
    {
        DEBUGPrint("devId: %d, subId: %d\n", p_buf[5], p_buf[0]);
        _devCmpt[chan] = 0;
        _devMode[chan] = p_buf[4];
        switch (p_buf[5])
        {
        case 0x00:
            _devType[chan] = WPAD_DEV_FREESTYLE;
            break;
        case 0x01:
            _devType[chan] = WPAD_DEV_CLASSIC;
            break;
        case 0x02:
            _devType[chan] = WUDIsLinkedWBC() ? WPAD_DEV_BALANCE_CHECKER : WPAD_DEV_FUTURE;
            break;
        case 0x04:
            _devType[chan] = WPADIsEnabledVSM() ? WPAD_DEV_VSM : WPAD_DEV_FUTURE;
            break;
        case 0x10:
            _devType[chan] = WPADIsEnabledTRN() ? WPAD_DEV_TRAIN : WPAD_DEV_FUTURE;
            break;
        case 0x03:
            if (__OSInIPL)
            {
                _devType[chan] = WPAD_DEV_CLASSIC;
                _devMode[chan] = 1;
                _devCmpt[chan] = 1;
            }
            else
            {
                switch (p_buf[0])
                {
                case 0x00:
                    _devType[chan] = WPADIsEnabledGTR() ? WPAD_DEV_GUITAR : WPAD_DEV_FUTURE;
                    break;
                case 0x01:
                    _devType[chan] = WPADIsEnabledDRM() ? WPAD_DEV_DRUM : WPAD_DEV_FUTURE;
                    break;
                default:
                    _devType[chan] = WPAD_DEV_FUTURE;
                    break;
                }
            }
            break;
        default:
            _devType[chan] = WPAD_DEV_FUTURE;
            break;
        }

        if (_devType[chan] == WPAD_DEV_CLASSIC)
        {
            if (_devMode[chan] < 1 || _devMode[chan] > 3)
            {
                _devType[chan] = WPAD_DEV_NOT_SUPPORTED;
            }
        }
        DEBUGPrint("type : %d\n", _devType[chan]);
        DEBUGPrint("mode : %d\n", _devMode[chan]);
        DEBUGPrint("last : %d\n", p_wpd->savedDevType);
        if (p_wpd->extWasDisconnected && p_wpd->savedDevType == _devType[chan])
        {
            if (p_wpd->calibrated)
            {
                DEBUGPrint("Keep calibration!\n");
                p_wpd->calibrated = TRUE;
            }
        }
        else
        {
            DEBUGPrint("Recalibration!\n");
            p_wpd->calibrated = FALSE;
        }
        p_wpd->savedDevType = _devType[chan];
        p_wpd->extWasDisconnected = FALSE;

        if (_devType[chan] == WPAD_DEV_FUTURE || _devType[chan] == WPAD_DEV_NOT_SUPPORTED)
        {
            p_wpd->devType = _devType[chan];
            p_wpd->devMode = _devMode[chan];
            if (p_wpd->extensionCallback)
            {
                p_wpd->extensionCallback(chan, p_wpd->devType);
            }
        }
        else
        {
            if (isExtEncMain(_devType[chan]))
            {
                p_wpd->extState = 2;
                WPADiCreateKey(chan);
            }
            else
            {
                p_wpd->extState = 3;
                WPADiCreateKeyFor3rd(chan);
            }
            WPADiSendWriteDataCmd(cmdq, 0xAA, 0x4A400F0, abortInitExtension);
            WPADiSendWriteData(cmdq, p_wpd->key, 6, 0x4A40040, abortInitExtension);
            WPADiSendWriteData(cmdq, p_wpd->key + 6, 6, 0x4A40046, abortInitExtension);
            WPADiSendWriteData(cmdq, p_wpd->key + 12, 4, 0x4A4004C, abortInitExtension);
            if (_devType[chan] == WPAD_DEV_BALANCE_CHECKER)
            {
                WPADiSendWriteDataCmd(cmdq, 0xAA, 0x4A400F1, abortInitExtension);
                WPADiSendWriteDataCmd(cmdq, 0xAA, 0x4A400F1, abortInitExtension);
                WPADiSendWriteDataCmd(cmdq, 0xAA, 0x4A400F1, abortInitExtension);
            }
            WPADiSendReadData(cmdq, p_wpd->wmReadDataBuf, 32, 0x4A40020, (WPADCallback)p_wpd->extensionCallback);
        }
    }
    else
    {
        p_wpd->devType = WPAD_DEV_NOT_SUPPORTED;
        p_wpd->devMode = 0;
        p_wpd->savedDevType = p_wpd->devType;
        p_wpd->extWasDisconnected = FALSE;
    }
}

void getGameInfo(s32 chan, s32 result, u8 index)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADMEMGameInfo* p_info = (WPADMEMGameInfo*)p_wpd->wmReadDataPtr;
    u8* p_buf = (u8*)p_wpd->wmReadDataPtr;
    int i;
    u8 sum = 0;

    if (result == WPAD_ERR_OK)
    {
        for (i = 0; i < 47; i++)
        {
            sum += p_buf[i];
        }
        sum += 0x55;

        if (p_info->checkSum == sum)
        {
            memcpy(&p_wpd->gameInfo, p_buf, sizeof(WPADMEMGameInfo));
            p_wpd->gameInfoErr[index] = WPAD_ERR_OK;
        }
        else
        {
            p_wpd->gameInfoErr[index] = WPAD_ERR_INVALID;
        }
    }
    else
    {
        p_wpd->gameInfoErr[index] = WPAD_ERR_INVALID;
    }
}

static inline void initExtension(s32 chan)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 extcmd = 0;
    WPADiClearQueue(&p_wpd->extCmdQueue);
    WPADiSendSetReportType(&p_wpd->extCmdQueue, p_wpd->dataFormat, p_wpd->disableContReport, abortInitExtension);
    p_wpd->extState = 1;
    WPADiSendWriteDataCmd(&p_wpd->extCmdQueue, 0x55, 0x04a400f0, abortInitExtension);
    WPADiSendWriteDataCmd(&p_wpd->extCmdQueue, extcmd, 0x04a400fb, abortInitExtension);
    WPADiSendReadData(&p_wpd->extCmdQueue, p_wpd->wmReadDataBuf, 6, 0x04a400fa, abortInitExtension);
}

static inline void __wpadInvertRxBuffer(u8 chan)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    p_wpd->rxBufIndex = p_wpd->rxBufIndex ? 0 : 1;
}

static inline void* __wpadGetWorkRxBuffer(u8 chan)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    return p_wpd->rxBufs[p_wpd->rxBufIndex];
}

static inline void* __wpadGetLastRxBuffer(u8 chan)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u32 lastIndex = p_wpd->rxBufIndex ? 0 : 1;
    return p_wpd->rxBufs[lastIndex];
}

s32 WPADiHIDParser(u8 chan, u8* data)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enable;
    s32 ret = 0;
    void* rxBuffer;

    if (data[0] >= 0x20 && data[0] <= 0x3F)
    {
        enable = OSDisableInterrupts();
        rxBuffer = __wpadGetWorkRxBuffer(chan);
        if (data[0] != 0x3E && data[0] != 0x3F)
        {
            memset(rxBuffer, 0, WPAD_RX_DATASIZE);
        }
        (*__a1_input_reports_array[data[0] - 0x20])(chan, data, rxBuffer);
        if (!p_wpd->handshakeFinished)
        {
            ((WPADStatus*)rxBuffer)->err = WPAD_ERR_INVALID;
        }
        if (data[0] != 0x3E && data[0] != 0x3F)
        {
            __wpadInvertRxBuffer(chan);
        }
        OSRestoreInterrupts(enable);
        WPADiExcludeButton(chan);
        WPADiCopyOut(chan);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

void __a1_20_status_report(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enable;
    BOOL attach = p_wpd->info.attach;
    WPADStatus* p_status;
    WPADExtensionCallback* extcb;

    DEBUGPrint("Received report 20\n");

    enable = OSDisableInterrupts();

    if (!p_wpd->handshakeFinished)
    {
        OSRestoreInterrupts(enable);
        return;
    }

    p_wpd->info.attach = (BOOL)((data[HID_IDX_BYTE9] & HID_ATTACH_MASK) >> 1);
    p_wpd->info.lowBat = (BOOL)(data[HID_IDX_BYTE9] & HID_LOWBAT_MASK);
    p_wpd->info.led = (u8)((data[HID_IDX_BYTE9] & HID_LED_MASK) >> 4);
    p_wpd->info.protocol = (u8)((data[HID_IDX_BYTE11] & HID_PRT_MASK) >> 4);
    p_wpd->info.firmware = (u8)(data[HID_IDX_BYTE11] & HID_FW_MASK);

    p_wpd->info.nearempty = (u8)((data[HID_IDX_BYTE7] & HID_NEAREMPTY_MASK) >> 7);
    p_wpd->info.dpd = (BOOL)((data[HID_IDX_BYTE9] & HID_DPD_MASK) >> 3);
    p_wpd->info.speaker = (BOOL)((data[HID_IDX_BYTE9] & HID_SPEAKER_MASK) >> 2);

    if (p_wpd->devType == WPAD_DEV_BALANCE_CHECKER)
    {
        p_wpd->info.battery = p_wpd->blcBattery;
    }
    else if ((u8)(data[HID_IDX_BYTE12]) >= 0x55)
    {
        p_wpd->info.battery = WPAD_BATTERY_LEVEL_MAX;
    }
    else if ((u8)(data[HID_IDX_BYTE12]) >= 0x44)
    {
        p_wpd->info.battery = WPAD_BATTERY_LEVEL_HIGH;
    }
    else if ((u8)(data[HID_IDX_BYTE12]) >= 0x33)
    {
        p_wpd->info.battery = WPAD_BATTERY_LEVEL_MEDIUM;
    }
    else if ((u8)(data[HID_IDX_BYTE12]) >= 0x3)
    {
        p_wpd->info.battery = WPAD_BATTERY_LEVEL_LOW;
    }
    else
    {
        p_wpd->info.battery = WPAD_BATTERY_LEVEL_CRITICAL;
    }

    if (p_wpd->info.attach)
    {
        if (!attach)
        {
            DEBUGPrint("initialize attachment\n");

            _retryCnt[chan] = 0;
            extcb = p_wpd->extensionCallback;
            initExtension(chan);
            p_wpd->devType = WPAD_DEV_UNKNOWN;
            p_wpd->devMode = 0;

            if (extcb)
            {
                extcb(chan, WPAD_DEV_UNKNOWN);
            }
        }
    }
    else
    {
        p_wpd->devType = WPAD_DEV_CORE;
        p_wpd->devMode = 0;

        WPADiClearQueue(&p_wpd->extCmdQueue);
        WPADiSendSetReportType(&p_wpd->extCmdQueue, p_wpd->dataFormat, p_wpd->disableContReport, NULL);

        if (attach)
        {
            p_wpd->extWasDisconnected = TRUE;
            p_wpd->reconnectExtMs = 300;
            if (p_wpd->extensionCallback)
            {
                p_wpd->extensionCallback(chan, WPAD_DEV_CORE);
            }
        }
    }

    if (p_wpd->infoOut)
    {
        memcpy(p_wpd->infoOut, &p_wpd->info, sizeof(WPADInfo));
        p_wpd->infoOut = NULL;
    }

    memcpy(rxBuffer, __wpadGetLastRxBuffer(chan), WPAD_RX_DATASIZE);
    p_status = (WPADStatus*)rxBuffer;
    {
        u16 fsButtons = p_status->button & 0x6000;
        p_status->button = (u16)((u16)((u16)((u16)data[HID_IDX_BYTE7] & 0x00ff)) | (u16)((u16)(data[HID_IDX_BYTE8] << 8) & 0xff00)) & HID_WPAD_BUTTON_MASK;
        p_status->button |= fsButtons;
    }
    if (p_status->dev != p_wpd->devType)
    {
        p_status->dev = p_wpd->devType;
        p_status->err = WPAD_ERR_INVALID;
    }

    if (p_wpd->cmdBlkCallback && p_wpd->statusReqBusy)
    {
        p_wpd->cmdBlkCallback(chan, WPAD_ERR_OK);
        p_wpd->cmdBlkCallback = NULL;
    }

    p_wpd->statusReqBusy = 0;

    OSRestoreInterrupts(enable);
}

void __a1_21_user_data(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enable;
    WPADStatus* p_status;
    u16 addr;
    u16 base;
    u16 i2c;
    u8 err;
    u8 len;
    s16 offset;
    s32 status;

    enable = OSDisableInterrupts();
    err = (u8)((u8)(data[HID_IDX_BYTE9]) & 0x0f);

    if (err != 0)
    {
        DEBUGPrint("read error happens!\n");
        p_wpd->wmReadErr = -1;

        if (p_wpd->cmdBlkCallback)
        {
            if (!p_wpd->extensionCallback || (WPADCallback)p_wpd->extensionCallback != p_wpd->cmdBlkCallback)
            {
                p_wpd->cmdBlkCallback(chan, WPAD_ERR_TRANSFER);
            }
            p_wpd->cmdBlkCallback = NULL;
        }
        p_wpd->status = WPAD_ERR_OK;
    }

    len = (u8)(((u8)((u8)(data[HID_IDX_BYTE9]) >> 4) & 0x0f) + 1);
    addr = (u16)((u16)((u16)((u16)(data[HID_IDX_BYTE10]) << 8) & (u16)0xFF00) | (u16)((u16)(data[HID_IDX_BYTE11]) & (u16)0xFF));
    base = (u16)((p_wpd->wmReadAddr & 0xFFFF));
    i2c = (u16)((p_wpd->wmReadAddr & 0xFFFF0000) >> 16);
    offset = (s16)(addr - base);

    if (addr >= base && addr <= base + p_wpd->wmReadLength)
    {
        memcpy((u8*)p_wpd->wmReadDataPtr + offset, data + HID_IDX_BYTE12, len);

        if (base + p_wpd->wmReadLength == addr + len)
        {
            DEBUGPrint("base addr: %08x\n", p_wpd->wmReadAddr);
            DEBUGPrint("length   : %d\n", p_wpd->wmReadLength);
            status = (p_wpd->wmReadErr < 0) ? WPAD_CETRANSFER : WPAD_CESUCCESS;
            DEBUGPrint("i2c = %04x\n", i2c);
            DEBUGPrint("enc = %d\n", p_wpd->extState);

            if (i2c == 0x04a4)
            {
                DEBUGPrint("Access to extension register.\n");
                if (p_wpd->extState == 2 || p_wpd->extState == 3)
                {
                    DEBUGPrint("Decode!!!!\n");
                    DEBUGPrint("    len = %d, addr = %04x\n", len, base);

                    WPADiDecode(chan, p_wpd->wmReadDataPtr, len, base);
                }
            }

            if ((p_wpd->wmReadAddr == 0 && p_wpd->oldFw == FALSE) || (p_wpd->wmReadAddr == 0x176C && p_wpd->oldFw == TRUE))
            {
                getDevConfig(chan, status);
            }
            if (p_wpd->wmReadAddr == 0x04a40020)
            {
                getExtConfig(chan, status);
            }
            if (p_wpd->wmReadAddr == 0x04a400fa)
            {
                getExtType(chan, status);
            }
            if (p_wpd->wmReadAddr == 0x2A)
            {
                getGameInfo(chan, status, 0);
            }
            if (p_wpd->wmReadAddr == 0x2A + sizeof(WPADMEMGameInfo))
            {
                getGameInfo(chan, status, 1);
            }

            if (p_wpd->cmdBlkCallback)
            {
                p_wpd->cmdBlkCallback(chan, status);
                p_wpd->cmdBlkCallback = NULL;
            }
            p_wpd->status = WPAD_ERR_OK;
        }
    }
    else
    {
        DEBUGPrint("received data is out of range!\n");
    }

    memcpy(rxBuffer, __wpadGetLastRxBuffer(chan), WPAD_RX_DATASIZE);
    p_status = (WPADStatus*)rxBuffer;
    {
        u16 fsButtons = p_status->button & 0x6000;
        p_status->button = (u16)((u16)((u16)((u16)data[HID_IDX_BYTE7] & 0x00ff)) | (u16)((u16)(data[HID_IDX_BYTE8] << 8) & 0xff00)) & HID_WPAD_BUTTON_MASK;
        p_status->button |= fsButtons;
    }
    if (p_status->dev != p_wpd->devType)
    {
        p_status->dev = p_wpd->devType;
        p_status->err = WPAD_ERR_INVALID;
    }

    OSRestoreInterrupts(enable);
}

void __a1_22_ack(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 type;
    u8 err;
    BOOL enable;
    WPADStatus* p_status;
    s32 status;

    enable = OSDisableInterrupts();

    DEBUGPrint("Received ack!\n");

    memcpy(rxBuffer, __wpadGetLastRxBuffer(chan), WPAD_RX_DATASIZE);
    p_status = (WPADStatus*)rxBuffer;
    {
        u16 fsButtons = p_status->button & 0x6000;
        p_status->button = (u16)((u16)((u16)((u16)data[HID_IDX_BYTE7] & 0x00ff)) | (u16)((u16)(data[HID_IDX_BYTE8] << 8) & 0xff00)) & HID_WPAD_BUTTON_MASK;
        p_status->button |= fsButtons;
    }
    if (p_status->dev != p_wpd->devType)
    {
        p_status->dev = p_wpd->devType;
        p_status->err = WPAD_ERR_INVALID;
    }

    type = (u8)data[HID_IDX_BYTE9];
    err = (u8)data[HID_IDX_BYTE10];

    status = (err == 0) ? WPAD_CESUCCESS : WPAD_CETRANSFER;
    DEBUGPrint("ack --> report ID = %02x, error code = %d\n", type, err);

    if (p_wpd->lastReportId == type)
    {
        if (p_wpd->cmdBlkCallback)
        {
            p_wpd->cmdBlkCallback(chan, status);
            p_wpd->cmdBlkCallback = NULL;
        }
        p_wpd->status = WPAD_ERR_OK;
    }
    else
    {
        DEBUGPrint("invalid ack!\n");
    }

    OSRestoreInterrupts(enable);
}

static const f32 PI = 3.141592f;

enum
{
    ACC_FORMAT_STANDARD,
    ACC_FORMAT_INTERLEAVED0,
    ACC_FORMAT_INTERLEAVED1
};
#define MAKE_FS_CONFIG_ACC_X                          MAKE_DEV_CONFIG_ACC_X
#define MAKE_FS_CONFIG_ACC_Y                          MAKE_DEV_CONFIG_ACC_Y
#define MAKE_FS_CONFIG_ACC_Z                          MAKE_DEV_CONFIG_ACC_Z
#define MAKE_BUTTON(high_, low_)                      ((u16)((u16)((u16)((u16)(low_) & 0x00ff)) | (u16)((u16)((high_) << 8) & 0xff00)) & HID_WPAD_BUTTON_MASK)
#define MAKE_ACC_STANDARD_X(wpadcb_, high_, low_)     ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)(u16)((low_) >> 5) & 0x03)) - (s16)((wpadcb_)->devConf.acc_0g.x))
#define MAKE_ACC_STANDARD_Y(wpadcb_, high_, low_)     ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)(u16)((low_) >> 4) & 0x02)) - (s16)((wpadcb_)->devConf.acc_0g.y))
#define MAKE_ACC_STANDARD_Z(wpadcb_, high_, low_)     ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)(u16)((low_) >> 5) & 0x02)) - (s16)((wpadcb_)->devConf.acc_0g.z))
#define MAKE_ACC_INTERLEAVED0_X(wpadcb_, high_, low_) ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)(u16)((low_) >> 6) & 0x02)) - (s16)((wpadcb_)->devConf.acc_0g.x))
#define CALC_ACC_INTERLEAVED0_Z(val_, high_, low_)    (val_) |= (s16)((s16)((s16)((s16)(high_) << 3) & ~0xff) | (s16)((s16)(u16)((low_) << 1) & 0xc0))
#define MAKE_ACC_INTERLEAVED1_Y(wpadcb_, high_, low_) ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)(u16)((low_) >> 6) & 0x02)) - (s16)((wpadcb_)->devConf.acc_0g.y))
#define CALC_ACC_INTERLEAVED1_Z(val_, high_, low_)    (val_) |= (s16)((s16)((s16)((s16)(high_) >> 1) & 0x30) | (s16)((s16)(u16)((low_) >> 3) & 0x0c))
#define MAKE_FS_ACC_X(wpadcb_, high_, low_)           ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)((low_) >> 2) & 0x03)) - (s16)((wpadcb_)->extConf.fs.acc_0g.x))
#define MAKE_FS_ACC_Y(wpadcb_, high_, low_)           ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)((low_) >> 4) & 0x03)) - (s16)((wpadcb_)->extConf.fs.acc_0g.y))
#define MAKE_FS_ACC_Z(wpadcb_, high_, low_)           ((s16)((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)((low_) >> 6) & 0x03)) - (s16)((wpadcb_)->extConf.fs.acc_0g.z))
#define CALC_FS_BUTTON(button_, data_)                (button_) |= (u16)((((u16) ~(data_)) << 13) & 0x6000)
#define CLAMP_FS_STICK_X(wpadcb_, x_)                 (x_) = __clamp((u8)(x_) - (u8)(wpadcb_)->extConf.fs.stick.x, -128, 127)
#define CLAMP_FS_STICK_Y(wpadcb_, y_)                 (y_) = __clamp((u8)(y_) - (u8)(wpadcb_)->extConf.fs.stick.y, -128, 127)
#define MAKE_TR_BUTTON                                MAKE_CL_BUTTON
#define MAKE_BL_WEIGHT(high_, low_)                   ((u16)((u16)((high_) << 8) & 0xff00) | (u16)((low_) & 0x00ff))
#define MAKE_BL_WEIGHT_UL                             MAKE_BL_WEIGHT
#define MAKE_BL_WEIGHT_UR                             MAKE_BL_WEIGHT
#define MAKE_BL_WEIGHT_DL                             MAKE_BL_WEIGHT
#define MAKE_BL_WEIGHT_DR                             MAKE_BL_WEIGHT

#define MAKE_DPD_EXTENDED_X(high_, low_)             ((s16)((low_) & 0xff) | (u16)((u16)((high_) & 0x30) << 4))
#define MAKE_DPD_EXTENDED_Y(high_, low_)             (WPAD_MAX_DPD_Y - (s16)((s16)((low_) & 0xff) | (u16)((u16)((high_) & 0xc0) << 2)))
#define MAKE_DPD_EXTENDED_PIXEL(high_, low_)         (u16)((s16)((s16)(((u16)(high_) << 8) & 0xff00) | (u16)((low_) & 0xff)) << 6)
#define MAKE_DPD_EXTENDED_RADIUS(data_)              ((data_) & 0x0f)
#define CALC_DPD_EXTENDED_X(x_)                      (x_) = (x_) << 3
#define CALC_DPD_EXTENDED_Y(y_)                      (y_) = (WPAD_MAX_DPD_Y - (s16)((y_) << 3))
#define MAKE_DPD_EXTENDED_SIZE(radius_)              ((f32)(radius_) * (f32)(radius_) * PI)
#define CLAMP_CL_STICK_LX(wpadcb_, lx_, low_, high_) (lx_) = __clamp((lx_) - (wpadcb_)->extConf.cl.lstk.x, (low_), (high_))
#define CLAMP_CL_STICK_LY(wpadcb_, ly_, low_, high_) (ly_) = __clamp((ly_) - (wpadcb_)->extConf.cl.lstk.y, (low_), (high_))
#define CLAMP_CL_STICK_RX(wpadcb_, rx_, low_, high_) (rx_) = __clamp((rx_) - (wpadcb_)->extConf.cl.rstk.x, (low_), (high_))
#define CLAMP_CL_STICK_RY(wpadcb_, ry_, low_, high_) (ry_) = __clamp((ry_) - (wpadcb_)->extConf.cl.rstk.y, (low_), (high_))
#define MAKE_CL_BUTTON(high_, low_)                  ((u16)((u16)(((u16)((u16)(high_) << 8) & 0xff00) | (u8)(low_)) ^ 0xffff))
#define MAKE_CL_REDUCED_STICK_LX(byte_)              ((s16)((byte_) & 0x3f) << 4)
#define MAKE_CL_REDUCED_STICK_LY(byte_)              ((s16)((byte_) & 0x3f) << 4)
#define MAKE_CL_REDUCED_STICK_RX(high_, mid_, low_)  ((s16)((s16)((s16)((s16)((s16)(high_) >> 3) & 0x18) | (s16)((s16)((s16)(mid_) >> 5) & 0x06) | (s16)((s16)((s16)(low_) >> 7) & 0x01)) << 5))
#define MAKE_CL_REDUCED_STICK_RY(byte_)              ((s16)((byte_) & 0x1f) << 5)
#define MAKE_CL_REDUCED_TRIGGER_L(high_, low_)       ((u8)((u8)((u8)((u8)((high_) >> 2) & 0x18) | (u8)((u8)((low_) >> 5) & 0x07)) << 3))
#define MAKE_CL_REDUCED_TRIGGER_R(byte_)             ((u8)((u8)(byte_) & 0x1f) << 3)
#define MAKE_CL_STANDARD_STICK_AXIS(byte_)           ((s16)((byte_) & 0xff) << 2)
#define MAKE_CL_STANDARD_STICK_LX                    MAKE_CL_STANDARD_STICK_AXIS
#define MAKE_CL_STANDARD_STICK_LY                    MAKE_CL_STANDARD_STICK_AXIS
#define MAKE_CL_STANDARD_STICK_RX                    MAKE_CL_STANDARD_STICK_AXIS
#define MAKE_CL_STANDARD_STICK_RY                    MAKE_CL_STANDARD_STICK_AXIS
#define MAKE_CL_EXTENDED_STICK_LX(high_, low_)       ((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)(low_) & 0x03))
#define MAKE_CL_EXTENDED_STICK_LY(high_, low_)       ((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)((s16)(low_) >> 4) & 0x03))
#define MAKE_CL_EXTENDED_STICK_RX(high_, low_)       ((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)((s16)(low_) >> 2) & 0x03))
#define MAKE_CL_EXTENDED_STICK_RY(high_, low_)       ((s16)((s16)((s16)(high_) << 2) & ~0x03) | (s16)((s16)((s16)(low_) >> 6) & 0x03))
#define MAKE_VS_SET_0(high_, low_)                   ((u16)((u16)((u16)(high_) << 2) & 0xfffc) | (u16)((u16)((low_) >> 6) & 0x0003))
#define MAKE_VS_SET_1(high_, low_)                   ((u16)((u16)((u16)(high_) << 2) & 0xfffc) | (u16)((u16)((low_) >> 4) & 0x0003))
#define MAKE_VS_SET_2(high_, low_)                   ((u16)((u16)((u16)(high_) << 2) & 0xfffc) | (u16)((u16)((low_) >> 2) & 0x0003))
#define MAKE_VS_SET_3(high_, low_)                   ((u16)((u16)((u16)(high_) << 2) & 0xfffc) | (u16)((u16)((low_) >> 0) & 0x0003))
#define MAKE_VS_SET_4(high_, low_)                   ((u16)((u16)((u16)(high_) << 2) & 0xfffc) | (u16)((u16)((low_) >> 6) & 0x0003))
#define MAKE_VS_42(high_, low_)                      ((u16)((u16)((u16)((u16)(high_) << 4) & 0xf3f0)) | (u16)((u16)((low_) >> 2) & 0x000f))
#define MAKE_VS_44(byte_)                            ((u8)((u8)(byte_) & 0x03))

static inline void __wpadSetDpdX(f32 x, DPDObject* object)
{
    object->x = x;
}

void __parse_dpd_data(s32 chan, WPADStatus** p_status, u8 fmt, u8* p_data, u8 len)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 i;
    u8 x;
    u8 y;
    u8 xy;
    f32 a;
    f32 b;

    if (p_wpd->dpdBusy)
    {
        (*p_status)->err = WPAD_ERR_INVALID;
        return;
    }

    if (fmt == 3)
    {
        for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
        {
            if (i * 3 + 2 < len)
            {
                x = *(p_data + i * 3);
                y = *(p_data + i * 3 + 1);
                xy = *(p_data + i * 3 + 2);

                (*p_status)->obj[i].x = (s16)((s16)((s16)(((u16)x) & 0xFF) | (u16)(((u16)xy & 0x30) << 4)));
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)y) & 0xFF) | (u16)(((u16)xy & 0xC0) << 2)));
                (*p_status)->obj[i].size = (u16)(((xy & 0xF) & 0xFF));
                (*p_status)->obj[i].size = (u8)((f32)((*p_status)->obj[i].size) * (f32)((*p_status)->obj[i].size) * PI);
                if ((*p_status)->obj[i].size == 0 || (*p_status)->obj[i].x == (s16)(WPAD_DPD_IMG_RESO_WX - 1) || (*p_status)->obj[i].y == (s16)(WPAD_DPD_IMG_RESO_WY - 1))
                {
                    (*p_status)->obj[i].x = (s16)0;
                    (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);
                    (*p_status)->obj[i].size = 0;
                }
                (*p_status)->obj[i].traceId = i;
            }
            else
            {
                (*p_status)->obj[i].x = (s16)0;
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);
                (*p_status)->obj[i].size = 0;
                (*p_status)->obj[i].traceId = i;
            }
        }
    }
    else if (fmt == 1)
    {
        for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
        {
            x = *(p_data + i * 3 - (i / 2));
            y = *(p_data + i * 3 - (i / 2) + 1);
            xy = *(p_data + (i / 2) * 5 + 2);

            if ((i % 2) == 0)
            {
                (*p_status)->obj[i].x = (s16)((s16)((s16)(((u16)x) & 0xFF) | (u16)(((u16)(xy & 0x30)) << 4)));
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)y) & 0xFF) | (u16)(((u16)(xy & 0xC0)) << 2)));
            }
            else
            {
                (*p_status)->obj[i].x = (s16)((s16)((s16)(((u16)x) & 0xFF) | (u16)(((u16)(xy & (u8)(0x30 >> 4))) << 8)));
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1 - (s16)((s16)(((u16)y) & 0xFF) | (u16)(((u16)(xy & (u8)(0xC0 >> 4))) << 6)));
            }
            if ((*p_status)->obj[i].x == (s16)(WPAD_DPD_IMG_RESO_WX - 1) || (*p_status)->obj[i].y == (s16)(WPAD_DPD_IMG_RESO_WY - 1))
            {
                (*p_status)->obj[i].x = (s16)0;
                (*p_status)->obj[i].y = (s16)(WPAD_DPD_IMG_RESO_WY - 1);
                (*p_status)->obj[i].size = 0;
            }
            else
            {
                (*p_status)->obj[i].size = 12;
            }
            (*p_status)->obj[i].traceId = i;
        }
    }

    for (i = 0; i < WPAD_DPD_MAX_OBJECTS; i++)
    {
        if ((*p_status)->obj[i].x != 0 || (*p_status)->obj[i].y != (WPAD_DPD_IMG_RESO_WY - 1))
        {
            a = (((f32)(*p_status)->obj[i].x) + calibX[chan] - centerX[chan]);
            b = (((f32)(*p_status)->obj[i].y) + calibY[chan] - centerY[chan]);
            __wpadSetDpdX(centerX[chan] + ((a * (f32)cos(rolag[chan] * -1.0f)) - (b * (f32)sin(rolag[chan] * -1.0f))), &(*p_status)->obj[i]);
            (*p_status)->obj[i].y = centerY[chan] + ((a * (f32)sin(rolag[chan] * -1.0f)) + (b * (f32)cos(rolag[chan] * -1.0f)));
        }
    }
}

void __parse_dpdex_data(WPADChannel chan, WPADStatusEx** statusEx,
    u8 objIndex, u8* data,
    u8 length)
{

    statusEx[0]->obj[objIndex].x = MAKE_DPD_EXTENDED_X(data[2], data[0]);
    statusEx[0]->obj[objIndex].y = MAKE_DPD_EXTENDED_Y(data[2], data[1]);
    statusEx[0]->exp[objIndex].pixel = MAKE_DPD_EXTENDED_PIXEL(data[7], data[8]);
    statusEx[0]->exp[objIndex].radius = MAKE_DPD_EXTENDED_RADIUS(data[2]);

    statusEx[0]->exp[objIndex].range_x1 = (s8)data[3] == -1 ? 0 : data[3];
    statusEx[0]->exp[objIndex].range_y1 = (s8)data[4] == -1 ? 0 : data[4];
    statusEx[0]->exp[objIndex].range_x2 = (s8)data[5] == -1 ? 0 : data[5];
    statusEx[0]->exp[objIndex].range_y2 = (s8)data[6] == -1 ? 0 : data[6];

    CALC_DPD_EXTENDED_X(statusEx[0]->exp[objIndex].range_x1);
    CALC_DPD_EXTENDED_Y(statusEx[0]->exp[objIndex].range_y1);
    CALC_DPD_EXTENDED_X(statusEx[0]->exp[objIndex].range_x2);
    CALC_DPD_EXTENDED_Y(statusEx[0]->exp[objIndex].range_y2);
    statusEx[0]->obj[objIndex].size = MAKE_DPD_EXTENDED_SIZE(statusEx[0]->exp[objIndex].radius);

    if (statusEx[0]->obj[objIndex].size == 0
        || statusEx[0]->obj[objIndex].x == WPAD_MAX_DPD_X
        || statusEx[0]->obj[objIndex].y == WPAD_MAX_DPD_Y
        || statusEx[0]->exp[objIndex].radius == 15)
    {
        statusEx[0]->obj[objIndex].x = 0;
        statusEx[0]->obj[objIndex].y = WPAD_MAX_DPD_Y;
        statusEx[0]->obj[objIndex].size = 0;
        statusEx[0]->exp[objIndex].pixel = 0;
        statusEx[0]->exp[objIndex].radius = 0;
    }

    statusEx[0]->obj[objIndex].traceId = objIndex;
}

static inline s16 __clamp(s16 x, s16 low, s16 high)
{
    s16 result = x;

    if (x < low)
        result = low;

    if (x > high)
        result = high;

    return result;
}
void __parse_cl_data(WPADChannel chan, WPADCLStatus** clStatus,
    u8 clFormat, u8* data, u8 length)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];

    switch (clFormat)
    {
    case WPAD_DEV_MODE_CLASSIC_EXTENDED:
        clStatus[0]->clLStickX = MAKE_CL_EXTENDED_STICK_LX(data[0], data[4]);
        clStatus[0]->clRStickX = MAKE_CL_EXTENDED_STICK_RX(data[1], data[4]);
        clStatus[0]->clLStickY = MAKE_CL_EXTENDED_STICK_LY(data[2], data[4]);
        clStatus[0]->clRStickY = MAKE_CL_EXTENDED_STICK_RY(data[3], data[4]);

        clStatus[0]->clTriggerL = (u8)data[5];
        clStatus[0]->clTriggerR = length < 9 ? 0 : (u8)data[6];
        clStatus[0]->clButton = length < 9 ? 0 : MAKE_CL_BUTTON(data[7], data[8]);
        break;

    case WPAD_DEV_MODE_CLASSIC_STANDARD:
        clStatus[0]->clLStickX = MAKE_CL_STANDARD_STICK_LX(data[0]);
        clStatus[0]->clRStickX = MAKE_CL_STANDARD_STICK_RX(data[1]);
        clStatus[0]->clLStickY = MAKE_CL_STANDARD_STICK_LY(data[2]);
        clStatus[0]->clRStickY = MAKE_CL_STANDARD_STICK_RY(data[3]);

        clStatus[0]->clTriggerL = (u8)data[4];
        clStatus[0]->clTriggerR = (u8)data[5];
        clStatus[0]->clButton = length < 8 ? 0 : MAKE_CL_BUTTON(data[6], data[7]);
        break;

    default:
    case WPAD_DEV_MODE_CLASSIC_REDUCED:
        clStatus[0]->clLStickX = MAKE_CL_REDUCED_STICK_LX(data[0]);
        clStatus[0]->clLStickY = MAKE_CL_REDUCED_STICK_LY(data[1]);
        clStatus[0]->clRStickX = MAKE_CL_REDUCED_STICK_RX(data[0], data[1], data[2]);
        clStatus[0]->clRStickY = MAKE_CL_REDUCED_STICK_RY(data[2]);

        clStatus[0]->clTriggerL = MAKE_CL_REDUCED_TRIGGER_L(data[2], data[3]);
        clStatus[0]->clTriggerR = MAKE_CL_REDUCED_TRIGGER_R(data[3]);
        clStatus[0]->clButton = MAKE_CL_BUTTON(data[4], data[5]);
        break;
    }

    if (p_wpd->devType == WPAD_DEV_CLASSIC)
    {
        clStatus[0]->clLStickX -= (s16)512;
        clStatus[0]->clLStickY -= (s16)512;
        clStatus[0]->clRStickX -= (s16)512;
        clStatus[0]->clRStickY -= (s16)512;
    }
    else
    {
        clStatus[0]->clLStickX -= (s16)512;
        clStatus[0]->clLStickY -= (s16)512;
    }

    if (!p_wpd->calibrated)
    {
        p_wpd->calibrated = TRUE;

        p_wpd->extConf.cl.lstk.x = clStatus[0]->clLStickX;
        p_wpd->extConf.cl.lstk.y = clStatus[0]->clLStickY;

        if (p_wpd->devType == WPAD_DEV_CLASSIC)
        {
            p_wpd->extConf.cl.rstk.x = clStatus[0]->clRStickX;
            p_wpd->extConf.cl.rstk.y = clStatus[0]->clRStickY;
            p_wpd->extConf.cl.triggerL = clStatus[0]->clTriggerL;
            p_wpd->extConf.cl.triggerR = clStatus[0]->clTriggerR;
        }
        else
        {
            p_wpd->extConf.cl.rstk.x = 0;
            p_wpd->extConf.cl.rstk.y = 0;
            p_wpd->extConf.cl.triggerL = 0;
            p_wpd->extConf.cl.triggerR = 0;
        }
    }

    CLAMP_CL_STICK_LX(p_wpd, clStatus[0]->clLStickX, -512, 511);
    CLAMP_CL_STICK_LY(p_wpd, clStatus[0]->clLStickY, -512, 511);

    if (p_wpd->devType == WPAD_DEV_CLASSIC)
    {
        CLAMP_CL_STICK_RX(p_wpd, clStatus[0]->clRStickX, -512, 511);
        CLAMP_CL_STICK_RY(p_wpd, clStatus[0]->clRStickY, -512, 511);

        clStatus[0]->clTriggerL = __clamp(clStatus[0]->clTriggerL - p_wpd->extConf.cl.triggerL, 0, 255);
        clStatus[0]->clTriggerR = __clamp(clStatus[0]->clTriggerR - p_wpd->extConf.cl.triggerR, 0, 255);
    }

    if (_devCmpt[chan])
    {
        clStatus[0]->clRStickX = 0;
        clStatus[0]->clRStickY = 0;
        clStatus[0]->clTriggerL = 0;
        clStatus[0]->clTriggerR = 0;
    }
}

void __parse_vs_data(WPADChannel chan, WPADVSStatus** status, u8 fmt, u8* data, u8 length)
{
    (*status)->unknown40 = data[0];
    (*status)->unknown36[0] = MAKE_VS_SET_0(data[1], data[6]);
    (*status)->unknown36[1] = MAKE_VS_SET_1(data[2], data[6]);
    (*status)->unknown36[2] = MAKE_VS_SET_2(data[3], data[6]);
    (*status)->unknown36[3] = MAKE_VS_SET_3(data[4], data[6]);
    (*status)->unknown36[4] = MAKE_VS_SET_4(data[5], data[7]);
    (*status)->unknown34[0] = data[8];
    (*status)->unknown2A[0] = MAKE_VS_SET_0(data[9], data[14]);
    (*status)->unknown2A[1] = MAKE_VS_SET_1(data[10], data[14]);
    (*status)->unknown2A[2] = MAKE_VS_SET_2(data[11], data[14]);
    (*status)->unknown2A[3] = MAKE_VS_SET_3(data[12], data[14]);
    (*status)->unknown2A[4] = MAKE_VS_SET_4(data[13], data[15]);
    (*status)->unknown42 = MAKE_VS_42(data[7], data[15]);
    (*status)->unknown46 = MAKE_VS_44(data[15]);
    if (length < 21)
    {
        (*status)->unknown44 = 0;
        (*status)->unknown47[0] = 0;
        (*status)->unknown47[1] = 0;
        (*status)->unknown47[2] = 0;
    }
    else
    {
        (*status)->unknown44 = MAKE_VS_SET_0(data[16], data[17]);
        (*status)->unknown47[0] = data[18] & 7;
        (*status)->unknown47[1] = data[19];
        (*status)->unknown47[2] = data[20];
    }
}

static inline void __parse_acc_data(WPADChannel chan, WPADStatus** status,
    u8 accFormat, u8* data,
    u8 length)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];

    switch (accFormat)
    {
    case ACC_FORMAT_STANDARD:
        status[0]->accX = MAKE_ACC_STANDARD_X(p_wpd, data[2], data[0]);
        status[0]->accY = MAKE_ACC_STANDARD_Y(p_wpd, data[3], data[1]);
        status[0]->accZ = MAKE_ACC_STANDARD_Z(p_wpd, data[4], data[1]);
        break;

    case ACC_FORMAT_INTERLEAVED0:
        status[0]->accX = MAKE_ACC_INTERLEAVED0_X(p_wpd, data[2], data[0]);
        CALC_ACC_INTERLEAVED0_Z(status[0]->accZ, data[1], data[0]);
        break;

    case ACC_FORMAT_INTERLEAVED1:
        status[0]->accY = MAKE_ACC_INTERLEAVED1_Y(p_wpd, data[2], data[0]);
        CALC_ACC_INTERLEAVED1_Z(status[0]->accZ, data[1], data[0]);
        break;
    }
}

static inline void __parse_fs_data(WPADChannel chan, WPADFSStatus** fsStatus,
    u8 fsFormat, u8* data,
    u8 length)
{
    WPADControlBlock* p_wpd = _wpdcb[chan];

    fsStatus[0]->fsStickX = data[0];
    fsStatus[0]->fsStickY = data[1];

    fsStatus[0]->fsAccX = MAKE_FS_ACC_X(p_wpd, data[2], data[5]);
    fsStatus[0]->fsAccY = MAKE_FS_ACC_Y(p_wpd, data[3], data[5]);
    fsStatus[0]->fsAccZ = MAKE_FS_ACC_Z(p_wpd, data[4], data[5]);

    CALC_FS_BUTTON(fsStatus[0]->button, data[5]);

    if (!p_wpd->calibrated)
    {
        p_wpd->calibrated = TRUE;
        p_wpd->extConf.fs.stick.x = fsStatus[0]->fsStickX;
        p_wpd->extConf.fs.stick.y = fsStatus[0]->fsStickY;
    }

    CLAMP_FS_STICK_X(p_wpd, fsStatus[0]->fsStickX);
    CLAMP_FS_STICK_Y(p_wpd, fsStatus[0]->fsStickY);
}

static inline void __parse_tr_data(WPADChannel chan,
    WPADTRStatus* trStatus,
    u8 trFormat, u8* data,
    u8 length)
{
    trStatus->brake = data[2];
    trStatus->mascon = data[3];
    trStatus->trButton = MAKE_TR_BUTTON(data[6], data[7]);
}

void __a1_30_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    p_status->dev = p_cb->devType;
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
}

void __a1_31_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    p_status->dev = p_cb->devType;
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    __parse_acc_data(chan, &p_status, ACC_FORMAT_STANDARD, data + 1, 5);
}

void __a1_32_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_FS_BTN || p_cb->dataFormat == WPAD_FMT_CLASSIC_BTN || p_cb->dataFormat == WPAD_FMT_TRAIN)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    memcpy(checkBuffer, data + 3, 8);
    WPADiDecode(chan, data + 3, 8, 0);
    if (p_cb->info.attach)
    {
        if (p_cb->devType == WPAD_DEV_FREESTYLE)
        {
            __parse_fs_data(chan, (WPADFSStatus**)&rxBuffer, p_cb->devMode, data + 3, 8);
        }
        else if (p_cb->devType == WPAD_DEV_CLASSIC)
        {
            __parse_cl_data(chan, (WPADCLStatus**)&rxBuffer, p_cb->devMode, data + 3, 8);
        }
        else if (p_cb->devType == WPAD_DEV_TRAIN)
        {
            __parse_tr_data(chan, (WPADTRStatus*)rxBuffer, p_cb->dataFormat, data + 3, 8);
        }
        if (!memcmp(checkBuffer, checkInvalidData, 8))
        {
            if (p_status->err == WPAD_ERR_OK)
            {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }
}

void __a1_33_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    p_status->dev = p_cb->devType;
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC_DPD)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    __parse_acc_data(chan, &p_status, ACC_FORMAT_STANDARD, data + 1, 5);
    __parse_dpd_data(chan, &p_status, p_cb->currentDpdCommand, data + 6, 12);
}

void __a1_34_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_cb->dataFormat == WPAD_FMT_BALANCE_CHECKER)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    memcpy(checkBuffer, data + 3, 19);
    WPADiDecode(chan, data + 3, 19, 0);
    if (p_cb->info.attach)
    {
        if (p_cb->devType == WPAD_DEV_BALANCE_CHECKER)
        {
            WPADBLStatus* p_blStatus = (WPADBLStatus*)rxBuffer;
            u8* extData = data + 3;
            int battery;
            p_blStatus->press[0] = MAKE_BL_WEIGHT_UL(extData[0], extData[1]);
            p_blStatus->press[1] = MAKE_BL_WEIGHT_UR(extData[2], extData[3]);
            p_blStatus->press[2] = MAKE_BL_WEIGHT_DL(extData[4], extData[5]);
            p_blStatus->press[3] = MAKE_BL_WEIGHT_DR(extData[6], extData[7]);

            p_blStatus->temp = extData[8];
            p_blStatus->battery = (u8)extData[10];

            battery = p_blStatus->battery << 1;

            if (battery >= 260)
                p_cb->blcBattery = 4;
            else if (battery < 260 && battery >= 250)
                p_cb->blcBattery = 3;
            else if (battery < 250 && battery >= 240)
                p_cb->blcBattery = 2;
            else if (battery < 240 && battery >= 212)
                p_cb->blcBattery = 1;
            else
                p_cb->blcBattery = 0;

            if (!p_cb->calibrated)
                p_cb->calibrated = TRUE;
        }
        if (!memcmp(checkBuffer, checkInvalidData, 19))
        {
            if (p_status->err == WPAD_ERR_OK)
            {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }
}

void __a1_35_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC || p_cb->dataFormat == WPAD_FMT_FS_BTN || p_cb->dataFormat == WPAD_FMT_FS_BTN_ACC || p_cb->dataFormat == WPAD_FMT_CLASSIC_BTN || p_cb->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC || p_cb->dataFormat == WPAD_FMT_VSM)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    __parse_acc_data(chan, &p_status, ACC_FORMAT_STANDARD, data + 1, 5);
    memcpy(checkBuffer, data + 6, 16);
    WPADiDecode(chan, data + 6, 16, 0);
    if (p_cb->info.attach)
    {
        if (p_cb->devType == WPAD_DEV_FREESTYLE)
        {
            __parse_fs_data(chan, (WPADFSStatus**)&rxBuffer, p_cb->devMode, data + 6, 16);
        }
        else if (p_cb->devType == WPAD_DEV_CLASSIC)
        {
            __parse_cl_data(chan, (WPADCLStatus**)&rxBuffer, p_cb->devMode, data + 6, 16);
        }
        else if (p_cb->devType == WPAD_DEV_VSM)
        {
            __parse_vs_data(chan, (WPADVSStatus**)&rxBuffer, p_cb->dataFormat, data + 6, 16);
        }
        if (!memcmp(checkBuffer, checkInvalidData, 16))
        {
            if (p_status->err == WPAD_ERR_OK)
            {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }
}

void __a1_36_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    p_status->err = WPAD_ERR_INVALID;
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    __parse_dpd_data(chan, &p_status, p_cb->currentDpdCommand, data + 3, 10);
    memcpy(checkBuffer, data + 13, 9);
    WPADiDecode(chan, data + 13, 9, 0);
    if (p_cb->info.attach)
    {
        if (!memcmp(checkBuffer, checkInvalidData, 9))
        {
            if (p_status->err == WPAD_ERR_OK)
            {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }
}

void __a1_37_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    p_status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC_DPD || p_cb->dataFormat == WPAD_FMT_FS_BTN || p_cb->dataFormat == WPAD_FMT_FS_BTN_ACC || p_cb->dataFormat == WPAD_FMT_FS_BTN_ACC_DPD || p_cb->dataFormat == WPAD_FMT_CLASSIC_BTN || p_cb->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC || p_cb->dataFormat == WPAD_FMT_CLASSIC_BTN_ACC_DPD || p_cb->dataFormat == WPAD_FMT_GUITAR || p_cb->dataFormat == WPAD_FMT_DRUM)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = (data[1] & HID_NEAREMPTY_MASK) >> 7;
    __parse_acc_data(chan, &p_status, ACC_FORMAT_STANDARD, data + 1, 5);
    __parse_dpd_data(chan, &p_status, p_cb->currentDpdCommand, data + 6, 10);
    memcpy(checkBuffer, data + 16, 6);
    WPADiDecode(chan, data + 16, 6, 0);
    if (p_cb->info.attach)
    {
        if (p_cb->devType == WPAD_DEV_FREESTYLE)
        {
            __parse_fs_data(chan, (WPADFSStatus**)&rxBuffer, p_cb->devMode, data + 16, 6);
        }
        else if (p_cb->devType == WPAD_DEV_CLASSIC || p_cb->devType == WPAD_DEV_GUITAR || p_cb->devType == WPAD_DEV_DRUM)
        {
            __parse_cl_data(chan, (WPADCLStatus**)&rxBuffer, p_cb->devMode, data + 16, 6);
        }
        if (!memcmp(checkBuffer, checkInvalidData, 6))
        {
            if (p_status->err == WPAD_ERR_OK)
            {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }
}

void __a1_3d_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;

    if (p_cb->dataFormat == WPAD_FMT_14)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    memcpy(checkBuffer, data + 1, 21);
    WPADiDecode(chan, data + 1, 21, 0);
    if (p_cb->info.attach)
    {
        if (p_cb->devType == WPAD_DEV_VSM)
        {
            __parse_vs_data(chan, (WPADVSStatus**)&rxBuffer, p_cb->dataFormat, data + 1, 21);
        }
        if (!memcmp(checkBuffer, checkInvalidData, 21))
        {
            if (p_status->err == WPAD_ERR_OK)
            {
                p_status->err = WPAD_ERR_CORRUPTED;
            }
        }
    }
}

u8 _recv_3e[WPAD_MAX_CONTROLLERS] = { 0, 0, 0, 0 };
u8 _recv_3f[WPAD_MAX_CONTROLLERS] = { 0, 0, 0, 0 };

void __a1_3e_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;
    BOOL enable;

    if (_recv_3e[chan] == 0 && _recv_3f[chan] == 0)
    {
        memset(rxBuffer, 0, WPAD_RX_DATASIZE);
    }
    p_status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC || p_cb->dataFormat == WPAD_FMT_BTN_ACC_DPD_EXTENDED)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = FALSE;
    __parse_acc_data(chan, &p_status, ACC_FORMAT_INTERLEAVED0, data + 1, 3);
    __parse_dpdex_data(chan, (WPADStatusEx**)&rxBuffer, 0, data + 4, 0);
    __parse_dpdex_data(chan, (WPADStatusEx**)&rxBuffer, 1, data + 13, 0);
    enable = OSDisableInterrupts();
    _recv_3e[chan] = 1;
    if (_recv_3e[chan] && _recv_3f[chan])
    {
        p_status->accZ -= p_cb->devConf.acc_0g.z;
        p_cb->rxBufIndex = p_cb->rxBufIndex ? 0 : 1;
        _recv_3e[chan] = _recv_3f[chan] = 0;
    }
    OSRestoreInterrupts(enable);
}

void __a1_3f_data_type(u8 chan, u8* data, void* rxBuffer)
{
    WPADControlBlock* p_cb = _wpdcb[chan];
    WPADStatus* p_status = (WPADStatus*)rxBuffer;
    BOOL enable;

    if (_recv_3e[chan] == 0 && _recv_3f[chan] == 0)
    {
        memset(rxBuffer, 0, WPAD_RX_DATASIZE);
    }
    p_status->button = MAKE_BUTTON(data[2], data[1]);
    if (p_cb->dataFormat == WPAD_FMT_CORE_BTN || p_cb->dataFormat == WPAD_FMT_CORE_BTN_ACC || p_cb->dataFormat == WPAD_FMT_BTN_ACC_DPD_EXTENDED)
    {
        p_status->err = WPAD_ERR_OK;
    }
    else
    {
        p_status->err = WPAD_ERR_INVALID;
    }
    p_status->dev = p_cb->devType;
    p_cb->info.nearempty = FALSE;
    __parse_acc_data(chan, &p_status, ACC_FORMAT_INTERLEAVED1, data + 1, 3);
    __parse_dpdex_data(chan, (WPADStatusEx**)&rxBuffer, 2, data + 4, 0);
    __parse_dpdex_data(chan, (WPADStatusEx**)&rxBuffer, 3, data + 13, 0);
    enable = OSDisableInterrupts();
    _recv_3f[chan] = 1;
    if (_recv_3e[chan] && _recv_3f[chan])
    {
        p_status->accZ -= p_cb->devConf.acc_0g.z;
        p_cb->rxBufIndex = p_cb->rxBufIndex ? 0 : 1;
        _recv_3e[chan] = _recv_3f[chan] = 0;
    }
    OSRestoreInterrupts(enable);
}

void __a1_unused_report(u8 chan, u8* data, void* rxBuffer)
{
}
