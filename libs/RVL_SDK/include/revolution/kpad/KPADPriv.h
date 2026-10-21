#ifndef RVL_SDK_KPAD_PRIVATE_H
#define RVL_SDK_KPAD_PRIVATE_H

#include <revolution/kpad/KPAD.h>
#include <revolution/mtx.h>
#include <revolution/wpad.h>

#ifdef __cplusplus
extern "C"
{
#endif // ifdef __cplusplus

#define KPAD_BUTTON_MASK 0x0000ffff
#define KPAD_BUTTON_RPT  0x80000000

#define KPAD_BTN_RPT_TIME_MAX 20000
#define KPAD_BTN_NO_RPT_DELAY 40000

#define KPAD_CMOS_RESO_WX 128
#define KPAD_CMOS_RESO_WY 96
#define KPAD_CMOS_RESO_CX (KPAD_CMOS_RESO_WX / 2)
#define KPAD_CMOS_RESO_CY (KPAD_CMOS_RESO_WY / 2)

#define KPAD_DPD_RESO_WX (WPAD_MAX_DPD_X + 1)
#define KPAD_DPD_RESO_WY (WPAD_MAX_DPD_Y + 1)
#define KPAD_DPD_RESO_CX (KPAD_DPD_RESO_WX / 2)
#define KPAD_DPD_RESO_CY (KPAD_DPD_RESO_WY / 2)

#define KPAD_PLAY_MODE_LOOSE 0
#define KPAD_PLAY_MODE_TIGHT 1
#define KPAD_USE_OBJECTS     2

    typedef struct KPADUnifiedWpadStatus
    {
        union
        {
            WPADStatus core;
            WPADFSStatus fs;
            WPADCLStatus cl;
            WPADTRStatus tr;
            WPADBLStatus bl;
        } u;
        u8 fmt;
        u8 padding;
    } KPADUnifiedWpadStatus;

    typedef struct
    {
        Vec2 center;
        s8 error_fg;
        s8 state_fg;
        s8 _fg_1;
        s8 _fg_2;
    } KPADObject;

    typedef union
    {
        KPADStatus k;
        KPADUnifiedWpadStatus w;
    } KPADTmpStatus;

    typedef WPADChannel KPADChannel;

    typedef struct
    {
        KPADStatus status;

        f32 pos_play_radius;
        f32 pos_sensitivity;

        f32 hori_play_radius;
        f32 hori_sensitivity;

        f32 dist_play_radius;
        f32 dist_sensitivity;

        f32 acc_play_radius;
        f32 acc_sensitivity;
        f32 dist_org;
        Vec2 accXY_nrm_hori;
        Vec2 sec_nrm_hori;
        Vec2 center_org;
        f32 dpd2pos_scale;
        KPADObject kobj_sample[4];
        KPADObject kobj_regular[2];
        s16 valid_objs;
        u8 bufIdx;
        u8 bufCount;
        KPADUnifiedWpadStatus uniRingBuf[16];
        KPADUnifiedWpadStatus* exRingBuf;
        u32 exBufLen;
        f32 sec_length;
        Vec2 sec_nrm;
        f32 sec_dist;
        f32 trust_sec_length;
        Vec hard_acc;
        Vec2 obj_horizon;
        Vec2 acc_horizon;
        Vec2 ah_circle_pos;
        u16 ah_circle_ct;
        u8 dpd_valid2_ct;
        u8 readLocked;
        u16 btn_repeat_time;
        u16 btn_repeat_next;
        u16 btn_repeat_delay;
        u16 btn_repeat_pulse;
        u16 btn_cl_repeat_time;
        u16 btn_cl_repeat_next;
        WPADCallback dpd_ctrl_callback;
        f32 acc_scale_x;
        f32 acc_scale_y;
        f32 acc_scale_z;
        f32 fs_acc_scale_x;
        f32 fs_acc_scale_y;
        f32 fs_acc_scale_z;
        Vec2 kobj_frame_min;
        Vec2 kobj_frame_max;
        f32 err_dist_speed_1;
        f32 err_dist_speedM_1;
        f32 ah_circle_radius2;
        f32 dist_vv1;
        f32 err_dist_min;
        WPADSamplingCallback appSamplingCallback;
        WPADConnectCallback* appConnectCallback;
        u8 resetReq;
        u8 exResetReq;
        u8 dpdEnabled;
        u8 dpdStatus;
        u8 dpdIssued;
        u8 dpdCmd;
        u8 dpdPreCallbackDone;
        u8 dpdPostCallbackDone;
        u8 aimReq;
        u8 aimEnabled;
        u8 fsAccRevise;
        u8 readMode;
        s32 pos_play_mode;
        s32 hori_play_mode;
        s32 dist_play_mode;
        s32 acc_play_mode;
        u32 lastDeviceType;
        u8 accScaleReset;
    } KPADInsideStatus;

    void KPADInitEx(KPADUnifiedWpadStatus* buffer, u32 length);
    s32 KPADiRead(KPADChannel chan, KPADStatus samplingBufs[], u32 length, s32* result, BOOL preserve);
    WPADConnectCallback* KPADSetConnectCallback(KPADChannel chan, WPADConnectCallback* callback);

    void KPADSetSensorHeight(KPADChannel, f32);

    void KPADReset(void);
    void KPADDisableDPD(KPADChannel);
    void KPADEnableDPD(KPADChannel);

    extern KPADInsideStatus inside_kpads[];

#ifdef __cplusplus
};
#endif // ifdef __cplusplus

#endif
