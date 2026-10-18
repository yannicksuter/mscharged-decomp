#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_report.h>

#include <revolution/nwc24/NWC24Config.h>
#include <revolution/os/OS.h>
#include <revolution/os/OSTime.h>
#include <revolution/vf/d_vf.h>

#include <time.h>

enum
{
    UNIX_EPOCH_OFFSET = 946684800
};

static struct tm aTimeStruct;

u64 dwci_np_consoleid;
u32 dwci_np_try_get_consoleid;

void DWCi_Np_CPUCopyFast(void* destination, const void* source, u32 size)
{
    u8* dst8 = destination;
    const u8* src8 = source;

    if (size > 32)
    {
        int relativeAlignment = ((u32)destination ^ (u32)source) & 15;

        if (relativeAlignment & 1)
        {
            goto copyBytes;
        }
        else if (relativeAlignment & 2)
        {
            // This SDK path consumes one byte for either nonzero prefix value.
            int prefix = 2 - ((u32)destination & 1);

            if (prefix != 0)
            {
                *(u8*)destination = *src8++;
                source = src8;
                destination = (u8*)destination + 1;
                --size;
            }
            goto copyHalfwords;
        }
        else if (relativeAlignment & 4)
        {
            int prefix = 4 - ((u32)destination & 3);

            switch (prefix)
            {
            case 3:
                *dst8++ = *src8++;
            case 2:
                *dst8++ = *src8++;
            case 1:
                *dst8 = *src8++;
                source = src8;
                destination = dst8 + 1;
                size -= prefix;
            }
            goto copyWords;
        }
        else
        {
            int prefix = 8 - ((u32)destination & 7);

            switch (prefix)
            {
            case 7:
                *dst8++ = *src8++;
            case 6:
                *dst8++ = *src8++;
            case 5:
                *dst8++ = *src8++;
            case 4:
                *dst8++ = *src8++;
            case 3:
                *dst8++ = *src8++;
            case 2:
                *dst8++ = *src8++;
            case 1:
                *dst8 = *src8++;
                source = src8;
                destination = dst8 + 1;
                size -= prefix;
            }
            goto copyDoublewords;
        }
    }
    else
    {
        if (((u32)destination & 7) == 0 && ((u32)source & 7) == 0)
        {
            goto copyDoublewords;
        }
        if (((u32)destination & 3) == 0 && ((u32)source & 3) == 0)
        {
            goto copyWords;
        }
        if (((u32)destination & 1) == 0 && ((u32)source & 1) == 0)
        {
            goto copyHalfwords;
        }
    }

copyBytes:
    memcpy(destination, source, size);
    return;

copyHalfwords:
    {
        u16* dst = destination;
        const u16* src = source;
        int count = size >> 1;

        if (count != 0)
        {
            int blocks = count >> 4;

            while (blocks > 0)
            {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                dst[4] = src[4];
                dst[5] = src[5];
                dst[6] = src[6];
                dst[7] = src[7];
                dst[8] = src[8];
                dst[9] = src[9];
                dst[10] = src[10];
                dst[11] = src[11];
                dst[12] = src[12];
                dst[13] = src[13];
                dst[14] = src[14];
                dst[15] = src[15];
                dst += 16;
                src += 16;
                --blocks;
            }

            switch (count & 15)
            {
            case 15:
                *dst++ = *src++;
            case 14:
                *dst++ = *src++;
            case 13:
                *dst++ = *src++;
            case 12:
                *dst++ = *src++;
            case 11:
                *dst++ = *src++;
            case 10:
                *dst++ = *src++;
            case 9:
                *dst++ = *src++;
            case 8:
                *dst++ = *src++;
            case 7:
                *dst++ = *src++;
            case 6:
                *dst++ = *src++;
            case 5:
                *dst++ = *src++;
            case 4:
                *dst++ = *src++;
            case 3:
                *dst++ = *src++;
            case 2:
                *dst++ = *src++;
            case 1:
                *dst++ = *src++;
            }
        }

        dst8 = (u8*)dst;
        src8 = (const u8*)src;
        switch ((int)(size & 1))
        {
        case 1:
            *dst8++ = *src8++;
        }
        return;
    }

copyWords:
    {
        u32* dst = destination;
        const u32* src = source;
        int count = size >> 2;

        if (count != 0)
        {
            int blocks = count >> 3;

            while (blocks > 0)
            {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                dst[4] = src[4];
                dst[5] = src[5];
                dst[6] = src[6];
                dst[7] = src[7];
                dst += 8;
                src += 8;
                --blocks;
            }

            switch (count & 7)
            {
            case 7:
                *dst++ = *src++;
            case 6:
                *dst++ = *src++;
            case 5:
                *dst++ = *src++;
            case 4:
                *dst++ = *src++;
            case 3:
                *dst++ = *src++;
            case 2:
                *dst++ = *src++;
            case 1:
                *dst++ = *src++;
            }
        }

        dst8 = (u8*)dst;
        src8 = (const u8*)src;
        switch ((int)(size & 3))
        {
        case 3:
            *dst8++ = *src8++;
        case 2:
            *dst8++ = *src8++;
        case 1:
            *dst8++ = *src8++;
        }
        return;
    }

copyDoublewords:
    {
        u64* dst = destination;
        const u64* src = source;
        int count = size >> 3;

        if (count != 0)
        {
            int blocks = count >> 3;

            while (blocks > 0)
            {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                dst[4] = src[4];
                dst[5] = src[5];
                dst[6] = src[6];
                dst[7] = src[7];
                dst += 8;
                src += 8;
                --blocks;
            }

            switch (count & 7)
            {
            case 7:
                *dst++ = *src++;
            case 6:
                *dst++ = *src++;
            case 5:
                *dst++ = *src++;
            case 4:
                *dst++ = *src++;
            case 3:
                *dst++ = *src++;
            case 2:
                *dst++ = *src++;
            case 1:
                *dst++ = *src++;
            }
        }

        dst8 = (u8*)dst;
        src8 = (const u8*)src;
        switch ((int)(size & 7))
        {
        case 7:
            *dst8++ = *src8++;
        case 6:
            *dst8++ = *src8++;
        case 5:
            *dst8++ = *src8++;
        case 4:
            *dst8++ = *src8++;
        case 3:
            *dst8++ = *src8++;
        case 2:
            *dst8++ = *src8++;
        case 1:
            *dst8++ = *src8++;
        }
        return;
    }
}

s64 DWCi_Np_GetTimeInSeconds(void)
{
    return OS_TICKS_TO_SEC(OSGetTime()) + UNIX_EPOCH_OFFSET;
}

struct tm* DWCi_Np_UtcToTm(s64* theTime)
{
    OSCalendarTime aCalTimeStruct;

    OSTicksToCalendarTime((*theTime - UNIX_EPOCH_OFFSET) * OS_TIME_SPEED, &aCalTimeStruct);

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
        void* work = DWC_AllocEx(DWC_ALLOCTYPE_UTIL, 0x4000, 32);
        void* vfWork = DWC_AllocEx(DWC_ALLOCTYPE_UTIL, 0x8000, 32);

        dwci_np_try_get_consoleid = TRUE;
        if (vfAvailable != TRUE)
        {
            VFInitEx(vfWork, 0x8000);
        }

        if (NWC24GetMyUserId(&dwci_np_consoleid) == NWC24_OK)
        {
            DWC_Printf(0x8000000, " get console friend code = %016lld\n", dwci_np_consoleid);
        }
        else
        {
            DWC_Printf(0x8000000, " failed to get console friend code.\n");
            dwci_np_consoleid = 0;
        }

        if (vfAvailable != TRUE)
        {
            VFFinalize();
        }
        DWC_Free(DWC_ALLOCTYPE_UTIL, work, 0);
        DWC_Free(DWC_ALLOCTYPE_UTIL, vfWork, 0);
    }

    return dwci_np_consoleid;
}

const char* DWCi_Np_GetConsoleName(void)
{
    return "Wii";
}

const char* DWCi_Np_GetAppGameName(void)
{
    return OSGetAppGamename();
}
