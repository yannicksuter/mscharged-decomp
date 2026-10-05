#include "NL/plat/PlatPadManager.h"
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

static void* AllocPadMemory(u32 size)
{
    return nlMalloc(size);
}

static BOOL FreePadMemory(void* memory)
{
    nlFree(memory);
    return TRUE;
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

        type[channel] = PLAT_PAD_NONE;
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

    int newType = GetType(channel);

    switch (WPADProbe(channel, &deviceType))
    {
    case WPAD_ERR_NO_CONTROLLER:
        connected[channel] = false;
        newType = PLAT_PAD_NONE;
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
                    newType = PLAT_PAD_REMOTE;
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
                    newType = PLAT_PAD_FREESTYLE;
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
                    newType = PLAT_PAD_CLASSIC;
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

    if (newType != GetType(channel))
    {
        int& currentType = type[channel];
        deviceChanged.Deliver(channel, currentType, newType);
        currentType = newType;
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
