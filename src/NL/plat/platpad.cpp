#include "NL/plat/PlatPadManager.h"
#include "Game/PadMonkey.h"
#include "NL/nlMemory.h"

#include <string.h>

PlatPadManager* g_pPlatPadManager;

extern "C" void fn_803751D4(WPADChannel channel, s32)
{
    g_pPlatPadManager->dpdActive[channel] = false;
    g_pPlatPadManager->dataFormatSet[channel] = false;
}

extern "C" void fn_803751F4(WPADChannel channel, WPADResult result)
{
    PlatPadManager* manager = g_pPlatPadManager;

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

extern "C" PlatPadStatus_80375EC8* fn_80375EC8(PlatPadManager* manager, int channel)
{
    return &manager->status[channel].core;
}

extern "C" PlatPadStatus_80375ED4* fn_80375ED4(
    PlatPadManager* manager, int channel)
{
    return &manager->status[channel].freestyle;
}

extern "C" PlatPadStatus_80375EE0* fn_80375EE0(
    PlatPadManager* manager, int channel)
{
    return &manager->status[channel].classic;
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
