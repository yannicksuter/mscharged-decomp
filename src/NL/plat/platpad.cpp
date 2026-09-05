#include "Game/PadActions.h"
#include "Game/PadMonkey.h"
#include "NL/nlMemory.h"

#include <string.h>

#define KPAD_MAX_SAMPLES 16
#define WPAD_MAX_CONTROLLERS 4

enum WPADResult
{
    WPAD_ERR_CORRUPTED = -7,
    WPAD_ERR_INVALID = -4,
    WPAD_ERR_TRANSFER = -3,
    WPAD_ERR_COMMUNICATION_ERROR = -2,
    WPAD_ERR_NO_CONTROLLER = -1,
    WPAD_ERR_OK = 0,
};

enum WPADDeviceType
{
    WPAD_DEV_CORE = 0,
    WPAD_DEV_FREESTYLE = 1,
    WPAD_DEV_CLASSIC = 2,
    WPAD_DEV_FUTURE = 251,
    WPAD_DEV_NOT_SUPPORTED = 252,
    WPAD_DEV_UNKNOWN = 255,
};

enum WPADDataFormat
{
    WPAD_FMT_CORE_BTN_ACC_DPD = 2,
    WPAD_FMT_FS_BTN_ACC_DPD = 5,
    WPAD_FMT_CLASSIC_BTN_ACC_DPD = 8,
};

enum WPADDpdCommand
{
    WPAD_DPD_DISABLE = 0,
    WPAD_DPD_BASIC = 1,
    WPAD_DPD_STANDARD = 3,
};

struct Vec
{
    float x, y, z;
};

struct Vec2
{
    float x, y;
};

union KPADEXStatus
{
    struct
    {
        Vec2 stick;
        Vec acc;
        float accValue;
        float accSpeed;
    } fs;

    struct
    {
        unsigned int hold;
        unsigned int trig;
        unsigned int release;
        Vec2 leftStick;
        Vec2 rightStick;
        float leftTrigger;
        float rightTrigger;
    } classic;
};

struct KPADStatus
{
    unsigned int hold;
    unsigned int trig;
    unsigned int release;
    Vec acc;
    float accValue;
    float accSpeed;
    Vec2 pos;
    Vec2 vec;
    float speed;
    Vec2 horizon;
    Vec2 horizonVec;
    float horizonSpeed;
    float dist;
    float distVec;
    float distSpeed;
    Vec2 accVertical;
    unsigned char deviceType;
    signed char wpadError;
    signed char dpdValid;
    unsigned char dataFormat;
    KPADEXStatus extension;
};

struct DPDObject
{
    short x;
    short y;
    unsigned short size;
    unsigned char traceId;
};

struct WPADStatus
{
    unsigned short button;
    short accX;
    short accY;
    short accZ;
    DPDObject object[4];
    unsigned char device;
    signed char err;
};

struct WPADFSStatus
{
    unsigned short button;
    short accX;
    short accY;
    short accZ;
    DPDObject object[4];
    unsigned char device;
    signed char err;
    short fsAccX;
    short fsAccY;
    short fsAccZ;
    signed char fsStickX;
    signed char fsStickY;
};

struct WPADCLStatus
{
    unsigned short button;
    short accX;
    short accY;
    short accZ;
    DPDObject object[4];
    unsigned char device;
    signed char err;
    unsigned short clButton;
    short clLeftStickX;
    short clLeftStickY;
    short clRightStickX;
    short clRightStickY;
    unsigned char clTriggerL;
    unsigned char clTriggerR;
};

typedef void WPADCallback(int, WPADResult);
typedef void WPADConnectCallback(int, WPADResult);
typedef void WPADExtensionCallback(int, int);

extern "C" {
void KPADSetBtnRepeat(int, float, float);
void KPADSetPosParam(int, float, float);
void KPADSetHoriParam(int, float, float);
void KPADSetDistParam(int, float, float);
void KPADSetAccParam(int, float, float);
int KPADRead(int, KPADStatus*, int);
void KPADInit();

void WPADRegisterAllocator(void* (*)(unsigned long), int (*)(void*));
int WPADGetStatus();
WPADResult WPADProbe(int, WPADDeviceType*);
WPADConnectCallback* WPADSetConnectCallback(int, WPADConnectCallback*);
WPADExtensionCallback*
WPADSetExtensionCallback(int, WPADExtensionCallback*);
int WPADSetDataFormat(int, unsigned int);
void WPADRead(int, WPADStatus*);
int WPADControlDpd(int, unsigned int, WPADCallback*);
}

enum
{
    WPAD_LIB_STATUS_3 = 3,
};

union PlatPadStatus
{
    struct
    {
        WPADStatus wpad;
        KPADStatus kpad;
    } core;

    struct
    {
        WPADFSStatus wpad;
        KPADStatus kpad;
    } freestyle;

    struct
    {
        WPADCLStatus wpad;
        KPADStatus kpad;
    } classic;
};

struct PlatPadManager
{
    PlatPadStatus status[WPAD_MAX_CONTROLLERS];
    bool connected[WPAD_MAX_CONTROLLERS];
    int type[WPAD_MAX_CONTROLLERS];
    bool disableFreestyle;
    bool disableClassic;
    bool dpdEnabled[WPAD_MAX_CONTROLLERS];
    bool dpdActive[WPAD_MAX_CONTROLLERS];
    bool dataFormatSet[WPAD_MAX_CONTROLLERS];
    unsigned char padding[2];
    DeviceChangedEvent_80137B40 deviceChanged;
};

extern "C" PlatPadManager* lbl_806E2478;
PlatPadManager* lbl_806E2478;

extern "C" void fn_803751D4(int channel, int)
{
    lbl_806E2478->dpdActive[channel] = false;
    lbl_806E2478->dataFormatSet[channel] = false;
}

extern "C" void fn_803751F4(int channel, WPADResult result)
{
    PlatPadManager* manager = lbl_806E2478;

    switch (result)
    {
    case WPAD_ERR_OK:
        manager->connected[channel] = true;
        WPADSetExtensionCallback(channel, fn_803751D4);
        manager->dpdActive[channel] = false;
        manager->dataFormatSet[channel] = false;
        break;
    case WPAD_ERR_NO_CONTROLLER:
    default:
        break;
    }
}

extern "C" void* fn_80375260(unsigned long size)
{
    return nlMalloc(size);
}

extern "C" int fn_80375264(void* memory)
{
    nlFree(memory);
    return true;
}

extern "C" void fn_80375288(PlatPadManager* manager)
{
    memset(manager, 0, sizeof(manager->status));
    WPADRegisterAllocator(fn_80375260, fn_80375264);
    KPADInit();

    while (WPADGetStatus() != WPAD_LIB_STATUS_3)
    {
    }

    for (int channel = 0; channel < WPAD_MAX_CONTROLLERS; ++channel)
    {
        KPADSetPosParam(channel, 0.02f, 0.95f);
        KPADSetHoriParam(channel, 0.0f, 1.0f);
        KPADSetDistParam(channel, 0.0f, 1.0f);
        KPADSetAccParam(channel, 0.0f, 1.0f);
        KPADSetBtnRepeat(channel, 0.75f, 0.25f);

        manager->type[channel] = 0;
        manager->connected[channel] = false;
        manager->dpdEnabled[channel] = false;
        manager->dpdActive[channel] = false;
        manager->dataFormatSet[channel] = false;
        WPADSetConnectCallback(channel, fn_803751F4);
    }
}

extern "C" void fn_803753D8(PlatPadManager*, int);

extern "C" void fn_8037537C(PlatPadManager* manager)
{
    for (int channel = 0; channel < WPAD_MAX_CONTROLLERS; ++channel)
    {
        if (manager->connected[channel])
        {
            fn_803753D8(manager, channel);
        }
    }
}

extern "C" void fn_80375E10(
    PlatPadManager* manager, int channel, unsigned int deviceType);

extern "C" void fn_803753D8(PlatPadManager* manager, int channel)
{
    WPADDeviceType deviceType;
    WPADStatus coreStatus;
    WPADFSStatus freestyleStatus;
    WPADCLStatus classicStatus;
    KPADStatus kpadStatus[KPAD_MAX_SAMPLES];

    int oldType = manager->type[channel];
    int newType = oldType;

    switch (WPADProbe(channel, &deviceType))
    {
    case WPAD_ERR_NO_CONTROLLER:
        manager->connected[channel] = false;
        newType = 0;
        manager->dpdActive[channel] = false;
        manager->dataFormatSet[channel] = false;
        break;
    case WPAD_ERR_COMMUNICATION_ERROR:
        break;
    case WPAD_ERR_OK:
    {
        unsigned int normalizedType = deviceType;
        if (normalizedType == WPAD_DEV_FUTURE
            || normalizedType == WPAD_DEV_NOT_SUPPORTED
            || normalizedType == WPAD_DEV_UNKNOWN
            || (normalizedType == WPAD_DEV_FREESTYLE
                && manager->disableFreestyle)
            || (normalizedType == WPAD_DEV_CLASSIC
                && manager->disableClassic))
        {
            normalizedType = WPAD_DEV_CORE;
        }

        if (!manager->dataFormatSet[channel])
        {
            switch (normalizedType)
            {
            case WPAD_DEV_CORE:
                if (WPADSetDataFormat(channel, WPAD_FMT_CORE_BTN_ACC_DPD)
                    == WPAD_ERR_OK)
                {
                    manager->dataFormatSet[channel] = true;
                }
                break;
            case WPAD_DEV_FREESTYLE:
                if (WPADSetDataFormat(channel, WPAD_FMT_FS_BTN_ACC_DPD)
                    == WPAD_ERR_OK)
                {
                    manager->dataFormatSet[channel] = true;
                }
                break;
            case WPAD_DEV_CLASSIC:
                if (WPADSetDataFormat(channel, WPAD_FMT_CLASSIC_BTN_ACC_DPD)
                    == WPAD_ERR_OK)
                {
                    manager->dataFormatSet[channel] = true;
                }
                break;
            }
        }

        if (manager->dataFormatSet[channel] == true)
        {
            fn_80375E10(manager, channel, deviceType);

            switch (normalizedType)
            {
            case WPAD_DEV_CORE:
                WPADRead(channel, &coreStatus);
                if (coreStatus.err == WPAD_ERR_OK)
                {
                    manager->status[channel].core.wpad = coreStatus;
                    newType = 1;
                    if (KPADRead(channel, kpadStatus, 1) > 0)
                    {
                        manager->status[channel].core.kpad = kpadStatus[0];
                    }
                }
                break;
            case WPAD_DEV_FREESTYLE:
                WPADRead(channel, (WPADStatus*)&freestyleStatus);
                if (freestyleStatus.err == WPAD_ERR_OK)
                {
                    manager->status[channel].freestyle.wpad = freestyleStatus;
                    newType = 2;
                    if (KPADRead(channel, kpadStatus, 1) > 0)
                    {
                        manager->status[channel].freestyle.kpad
                            = kpadStatus[0];
                    }
                }
                break;
            case WPAD_DEV_CLASSIC:
                WPADRead(channel, (WPADStatus*)&classicStatus);
                if (classicStatus.err == WPAD_ERR_OK)
                {
                    manager->status[channel].classic.wpad = classicStatus;
                    newType = 3;
                    if (KPADRead(channel, kpadStatus, 1) > 0)
                    {
                        manager->status[channel].classic.kpad = kpadStatus[0];
                    }
                }
                break;
            }
        }
        break;
    }
    default:
        break;
    }

    if (newType != manager->type[channel])
    {
        manager->deviceChanged.UnidentifiedDeliver(
            channel, manager->type[channel], newType);
        manager->type[channel] = newType;
    }
}

extern "C" void fn_80375DF8(
    PlatPadManager* manager, int channel, bool enabled)
{
    manager->dpdEnabled[channel] = enabled;
}

extern "C" bool fn_80375E04(PlatPadManager* manager, int channel)
{
    return manager->dpdEnabled[channel];
}

extern "C" void fn_80375E10(
    PlatPadManager* manager, int channel, unsigned int deviceType)
{
    if (manager->dpdActive[channel] != manager->dpdEnabled[channel])
    {
        if (manager->dpdEnabled[channel])
        {
            int result;
            if (deviceType == WPAD_DEV_CORE
                || deviceType == WPAD_DEV_FUTURE
                || deviceType == WPAD_DEV_NOT_SUPPORTED
                || deviceType == WPAD_DEV_UNKNOWN)
            {
                result = WPADControlDpd(channel, WPAD_DPD_STANDARD, 0);
            }
            else
            {
                result = WPADControlDpd(channel, WPAD_DPD_BASIC, 0);
            }

            if (result == WPAD_ERR_OK)
            {
                manager->dpdActive[channel] = true;
            }
        }
        else if (WPADControlDpd(channel, WPAD_DPD_DISABLE, 0) == WPAD_ERR_OK)
        {
            manager->dpdActive[channel] = false;
        }
    }
}

extern "C" WPADStatus* fn_80375EC8(PlatPadManager* manager, int channel)
{
    return &manager->status[channel].core.wpad;
}

extern "C" WPADFSStatus* fn_80375ED4(
    PlatPadManager* manager, int channel)
{
    return &manager->status[channel].freestyle.wpad;
}

extern "C" WPADCLStatus* fn_80375EE0(
    PlatPadManager* manager, int channel)
{
    return &manager->status[channel].classic.wpad;
}

PadMonkey_80375EEC::PadMonkey_80375EEC(int padIndex)
    : PadMonkey(padIndex)
{
    m_prevPressurePtr = &m_prevPressure[0];
    m_currPressurePtr = &m_currPressure[0];
    m_buttonChance = &m_buttonChances[0];

    memset(m_prevPressurePtr, 0, GetButtonCount() * sizeof(float));
    memset(m_currPressurePtr, 0, GetButtonCount() * sizeof(float));

    for (int i = 0; i < GetButtonCount(); ++i)
    {
        m_buttonChance[i] = 0.0f;
    }
}
