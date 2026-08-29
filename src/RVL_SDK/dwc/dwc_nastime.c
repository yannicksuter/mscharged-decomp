#include <dwc/dwc_auth_interface.h>
#include <dwc/dwc_nastime.h>
#include <dwc/dwc_nonport.h>

#include <time.h>

BOOL DWC_GetDateTime(DWCDate* date, DWCTime* time)
{
    s64 diff;
    s64 localsec = DWCi_Np_GetTimeInSeconds();
    BOOL result = fn_8049F7D4(&diff);
    struct tm* tm;

    if (result)
    {
        localsec += diff;
    }

    tm = fn_8049BB54(&localsec);
    date->year = tm->tm_year + 1900;
    date->month = tm->tm_mon;
    date->mday = tm->tm_mday;
    date->yday = tm->tm_yday;
    date->wday = tm->tm_wday;
    time->hour = tm->tm_hour;
    time->min = tm->tm_min;
    time->sec = tm->tm_sec;
    time->msec = 0;
    time->usec = 0;
    return result;
}
