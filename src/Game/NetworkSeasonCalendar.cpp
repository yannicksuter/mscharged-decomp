#include <dwc/dwc_nastime.h>
#include "Game/NetworkSeasonCalendar.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakValue.inl"
#include <string.h>

static int DaysInMonth(int month, int year)
{
    if (month == 2)
    {
        if (year % 4 == 0)
        {
            return 29;
        }
        return 28;
    }
    return sMonthDays[month - 1];
}

bool GetAdjustedNetworkDate(DWCDate* date, DWCTime* time)
{
    bool valid = DWC_GetDateTime(date, time);
    if (!valid)
    {
        memset(time, 0, sizeof(*time));
        date->mday = 1;
        date->month = 0;
        date->year = 2000;
        date->wday = 0;
        date->yday = 0;
    }

    ++date->month;
    if (g_nAddHoursTime != 0 || g_nAddMinsTime != 0)
    {
        time->min += g_nAddMinsTime;
        if (time->min > 59)
        {
            time->min -= 60;
            ++time->hour;
        }
        else if (time->min < 0)
        {
            time->min += 60;
            --time->hour;
        }

        time->hour += g_nAddHoursTime;
        if (time->hour > 23)
        {
            time->hour -= 24;
            ++date->mday;
            if (date->mday > DaysInMonth(date->month, date->year))
            {
                date->mday -= DaysInMonth(date->month, date->year);
                if (++date->month > 12)
                {
                    date->month = 1;
                    ++date->year;
                }
            }
        }
        else if (time->hour < 0)
        {
            time->hour += 24;
            if (--date->mday < 1)
            {
                if (--date->month < 1)
                {
                    date->month = 12;
                    --date->year;
                }
                date->mday = DaysInMonth(date->month, date->year);
            }
        }
    }
    return true;
}

int FindNetworkSeasonBoundary(
    const NetworkSeasonDateTable* dates, NetworkSeasonDate date)
{
    int count = dates->mCount;
    int index = 0;
    for (; index < count; ++index)
    {
        if (date.mDay == dates->mDates[index].mDay)
        {
            if (date.mMonth == dates->mDates[index].mMonth)
            {
                return index;
            }
        }
        if (dates->mDates[index].mMonth > date.mMonth)
        {
            break;
        }
        else if (date.mMonth == dates->mDates[index].mMonth)
        {
            if (dates->mDates[index].mDay > date.mDay)
            {
                break;
            }
        }
    }
    --index;
    return index;
}

static int DayOfYear(NetworkSeasonDate date, int year)
{
    int result = 0;
    for (int i = 1; i < date.mMonth; ++i)
    {
        result += DaysInMonth(i, year);
    }
    result += date.mDay - 1;
    return result;
}

int GetDaysUntilNextSeasonBoundary(
    const NetworkSeasonDateTable* dates, int index, int year)
{
    NetworkSeasonDate current = dates->GetDate(index);
    int currentDay;
    int nextDay;
    if (index == dates->mCount - 1)
    {
        const NetworkSeasonDate& next = dates->mDates[0];
        currentDay = DayOfYear(current, year);
        int nextYearDay = DayOfYear(next, year + 1);
        int remaining = (year % 4 == 0) ? 366 : 365;
        nextDay = nextYearDay + remaining;
    }
    else
    {
        currentDay = DayOfYear(current, year);
        const NetworkSeasonDate& next = dates->mDates[index + 1];
        nextDay = DayOfYear(next, year);
    }
    return nextDay - currentDay;
}

int GetDaysSinceSeasonBoundary(const NetworkSeasonDateTable* dates, int index,
    NetworkSeasonDate date, int year)
{
    const NetworkSeasonDate& boundary = dates->mDates[index];
    int boundaryDay = DayOfYear(boundary, year);
    return DayOfYear(date, year) - boundaryDay;
}

NetworkSeasonDate sNetworkSeasonDates[52] = {
    { 1, 1 }, { 1, 8 }, { 1, 15 }, { 1, 22 }, { 1, 29 }, { 2, 5 }, { 2, 12 }, { 2, 19 }, { 2, 26 }, { 3, 5 }, { 3, 12 }, { 3, 19 }, { 3, 26 }, { 4, 2 }, { 4, 9 }, { 4, 16 }, { 4, 23 }, { 4, 30 }, { 5, 7 }, { 5, 14 }, { 5, 21 }, { 5, 28 }, { 6, 4 }, { 6, 11 }, { 6, 18 }, { 6, 25 }, { 7, 2 }, { 7, 9 }, { 7, 16 }, { 7, 23 }, { 7, 30 }, { 8, 6 }, { 8, 13 }, { 8, 20 }, { 8, 27 }, { 9, 3 }, { 9, 10 }, { 9, 17 }, { 9, 24 }, { 10, 1 }, { 10, 8 }, { 10, 15 }, { 10, 22 }, { 10, 29 }, { 11, 5 }, { 11, 12 }, { 11, 19 }, { 11, 26 }, { 12, 3 }, { 12, 10 }, { 12, 17 }, { 12, 24 }
};

int sMonthDays[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

NetworkSeasonDateTable sNetworkSeasonDateTable(
    52, sNetworkSeasonDates);

int g_nAddHoursTime;
int g_nAddMinsTime;

static TweakIntBinding sAddHoursTimeTweak(
    "g_nAddHoursTime", "Network", &g_nAddHoursTime, true);
static TweakIntBinding sAddMinsTimeTweak(
    "g_nAddMinsTime", "Network", &g_nAddMinsTime, true);
