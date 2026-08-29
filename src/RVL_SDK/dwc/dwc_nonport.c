#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_report.h>

#include <revolution/nwc24/NWC24Config.h>
#include <revolution/nwc24/NWC24Manage.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSTime.h>
#include <revolution/vf/d_vf.h>

#include <time.h>

static struct tm aTimeStruct;

u64 dwci_np_consoleid;
u32 dwci_np_try_get_consoleid;

s64 DWCi_Np_GetTimeInSeconds(void)
{
    return OS_TICKS_TO_SEC(OSGetTime()) + 0x386CC4F0;
}

struct tm* fn_8049BB54(s64* theTime)
{
    OSCalendarTime aCalTimeStruct;

    OSTicksToCalendarTime((*theTime - 0x386CC4F0LL) * OS_TIME_SPEED, &aCalTimeStruct);

    aTimeStruct.tm_sec = aCalTimeStruct.sec;
    aTimeStruct.tm_min = aCalTimeStruct.min;
    aTimeStruct.tm_hour = aCalTimeStruct.hour;
    aTimeStruct.tm_mday = aCalTimeStruct.mday;
    aTimeStruct.tm_mon = aCalTimeStruct.month;
    aTimeStruct.tm_year = aCalTimeStruct.year - 1900;
    aTimeStruct.tm_wday = aCalTimeStruct.wday;
    aTimeStruct.tm_yday = aCalTimeStruct.yday;
    aTimeStruct.tm_isdst = 0;
    return &aTimeStruct;
}

u64 DWCi_Np_GetConsoleId(void)
{
    if (!dwci_np_try_get_consoleid)
    {
        BOOL vfAvailable = VFIsAvailable();
        NWC24Err result;
        NWC24Work* work = DWC_AllocEx(DWC_ALLOCTYPE_UTIL, NWC24_WORK_PUBLIC_SIZE, 32);

        dwci_np_try_get_consoleid = TRUE;
        if (vfAvailable != TRUE)
        {
            VFInit();
        }

        for (;;)
        {
            result = NWC24OpenLib(work);
            if (result != NWC24_ERR_BUSY)
            {
                break;
            }
            OSSleepTicks(OS_MSEC_TO_TICKS(1));
        }

        if (result == NWC24_OK || result == NWC24_ERR_LIB_OPENED)
        {
            if (NWC24GetMyUserId(&dwci_np_consoleid) == NWC24_OK)
            {
                DWC_Printf(0x8000000, " get console friend code = %016lld\n", dwci_np_consoleid);
            }
            else
            {
                DWC_Printf(0x8000000, " failed to get console friend code.\n");
                dwci_np_consoleid = 0;
            }
        }
        else
        {
            DWC_Printf(0x8000000, " failed to open NWC24.[%d]\n", result);
            dwci_np_consoleid = 0;
        }

        if (result == NWC24_OK)
        {
            NWC24CloseLib();
        }
        if (vfAvailable != TRUE)
        {
            VFFinalize();
        }
        DWC_Free(DWC_ALLOCTYPE_UTIL, work, NWC24_WORK_PUBLIC_SIZE);
    }

    return dwci_np_consoleid;
}
