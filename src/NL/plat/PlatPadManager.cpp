#include "NL/plat/PlatPadManager.h"
#include "Game/PadMonkey.h"
#include "NL/nlMemory.h"

#include <string.h>

PlatPadManager* g_pPlatPadManager;

static void PadExtensionChanged(WPADChannel channel, s32)
{
    g_pPlatPadManager->dpdActive[channel] = false;
    g_pPlatPadManager->dataFormatSet[channel] = false;
}

static void PadConnectChanged(WPADChannel channel, WPADResult result)
{
    PlatPadManager* manager = g_pPlatPadManager;

    switch (result)
    {
    case WPAD_ERR_OK:
        manager->connected[channel] = true;
        WPADSetExtensionCallback(channel, PadExtensionChanged);
        manager->dpdActive[channel] = false;
        manager->dataFormatSet[channel] = false;
        break;
    case WPAD_ERR_NO_CONTROLLER:
    default:
        break;
    }
}

static void* AllocPadMemory(unsigned long size)
{
    return nlMalloc(size);
}

static int FreePadMemory(void* memory)
{
    nlFree(memory);
    return true;
}

void PlatPadManager::Initialize()
{
    memset(status, 0, sizeof(status));
    WPADRegisterAllocator(AllocPadMemory, FreePadMemory);
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

        type[channel] = 0;
        connected[channel] = false;
        dpdEnabled[channel] = false;
        dpdActive[channel] = false;
        dataFormatSet[channel] = false;
        WPADSetConnectCallback(channel, PadConnectChanged);
    }
}

void UpdatePlatPad(PlatPadManager* manager)
{
    for (int channel = 0; channel < WPAD_MAX_CONTROLLERS; ++channel)
    {
        if (manager->connected[channel])
        {
            manager->UpdateChannel(channel);
        }
    }
}

void PlatPadManager::UpdateChannel(int channel)
{
    WPADDeviceType deviceType;
    WPADStatus coreStatus;
    WPADFSStatus freestyleStatus;
    WPADCLStatus classicStatus;
    KPADStatus kpadStatus[KPAD_MAX_SAMPLES];

    int oldType = type[channel];
    int newType = oldType;

    switch (WPADProbe(channel, &deviceType))
    {
    case WPAD_ERR_NO_CONTROLLER:
        connected[channel] = false;
        newType = 0;
        dpdActive[channel] = false;
        dataFormatSet[channel] = false;
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
                && disableFreestyle)
            || (normalizedType == WPAD_DEV_CLASSIC
                && disableClassic))
        {
            normalizedType = WPAD_DEV_CORE;
        }

        if (!dataFormatSet[channel])
        {
            switch (normalizedType)
            {
            case WPAD_DEV_CORE:
                if (WPADSetDataFormat(channel, WPAD_FMT_CORE_BTN_ACC_DPD)
                    == WPAD_ERR_OK)
                {
                    dataFormatSet[channel] = true;
                }
                break;
            case WPAD_DEV_FREESTYLE:
                if (WPADSetDataFormat(channel, WPAD_FMT_FS_BTN_ACC_DPD)
                    == WPAD_ERR_OK)
                {
                    dataFormatSet[channel] = true;
                }
                break;
            case WPAD_DEV_CLASSIC:
                if (WPADSetDataFormat(channel, WPAD_FMT_CLASSIC_BTN_ACC_DPD)
                    == WPAD_ERR_OK)
                {
                    dataFormatSet[channel] = true;
                }
                break;
            }
        }

        if (dataFormatSet[channel] == true)
        {
            UpdateDPD(channel, deviceType);

            switch (normalizedType)
            {
            case WPAD_DEV_CORE:
                WPADRead(channel, &coreStatus);
                if (coreStatus.err == WPAD_ERR_OK)
                {
                    status[channel].core.wpad = coreStatus;
                    newType = 1;
                    if (KPADRead(channel, kpadStatus, 1) > 0)
                    {
                        status[channel].core.kpad = kpadStatus[0];
                    }
                }
                break;
            case WPAD_DEV_FREESTYLE:
                WPADRead(channel, (WPADStatus*)&freestyleStatus);
                if (freestyleStatus.err == WPAD_ERR_OK)
                {
                    status[channel].freestyle.wpad = freestyleStatus;
                    newType = 2;
                    if (KPADRead(channel, kpadStatus, 1) > 0)
                    {
                        status[channel].freestyle.kpad
                            = kpadStatus[0];
                    }
                }
                break;
            case WPAD_DEV_CLASSIC:
                WPADRead(channel, (WPADStatus*)&classicStatus);
                if (classicStatus.err == WPAD_ERR_OK)
                {
                    status[channel].classic.wpad = classicStatus;
                    newType = 3;
                    if (KPADRead(channel, kpadStatus, 1) > 0)
                    {
                        status[channel].classic.kpad = kpadStatus[0];
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

    if (newType != type[channel])
    {
        deviceChanged.UnidentifiedDeliver(
            channel, type[channel], newType);
        type[channel] = newType;
    }
}

void PlatPadManager::SetDPDEnabled(int channel, bool enabled)
{
    dpdEnabled[channel] = enabled;
}

bool PlatPadManager::IsDPDEnabled(int channel) const
{
    return dpdEnabled[channel];
}

void PlatPadManager::UpdateDPD(int channel, unsigned int deviceType)
{
    if (dpdActive[channel] != dpdEnabled[channel])
    {
        if (dpdEnabled[channel])
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
                dpdActive[channel] = true;
            }
        }
        else if (WPADControlDpd(channel, WPAD_DPD_DISABLE, 0) == WPAD_ERR_OK)
        {
            dpdActive[channel] = false;
        }
    }
}

WiiRemotePadStatus* PlatPadManager::GetRemoteStatus(int channel)
{
    return &status[channel].core;
}

WiiFreestylePadStatus* PlatPadManager::GetFreestyleStatus(int channel)
{
    return &status[channel].freestyle;
}

WiiClassicPadStatus* PlatPadManager::GetClassicStatus(int channel)
{
    return &status[channel].classic;
}

WiiPadMonkey::WiiPadMonkey(int padIndex)
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
