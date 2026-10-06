#ifndef GAME_NETWORK_SEASON_CALENDAR_H
#define GAME_NETWORK_SEASON_CALENDAR_H

struct DWCDate;
struct DWCTime;

struct NetworkSeasonDate
{
    int mMonth;
    int mDay;
}; // size: 0x8

struct NetworkSeasonDateTable
{
    NetworkSeasonDateTable(int count, NetworkSeasonDate* dates)
        : mCount(count)
        , mDates(dates)
    {
    }

    NetworkSeasonDate GetDate(int index) const
    {
        return mDates[index];
    }

    mutable int mCount;
    NetworkSeasonDate* mDates;
}; // size: 0x8

bool GetAdjustedNetworkDate(DWCDate* date, DWCTime* time);
int FindNetworkSeasonBoundary(
    const NetworkSeasonDateTable* dates, NetworkSeasonDate date);
int GetDaysUntilNextSeasonBoundary(
    const NetworkSeasonDateTable* dates, int index, int year);
int GetDaysSinceSeasonBoundary(const NetworkSeasonDateTable* dates, int index,
    NetworkSeasonDate date, int year);

extern NetworkSeasonDateTable sNetworkSeasonDateTable;

extern int g_nAddHoursTime;
extern int g_nAddMinsTime;

extern NetworkSeasonDate sNetworkSeasonDates[52];
extern int sMonthDays[12];

#endif // GAME_NETWORK_SEASON_CALENDAR_H
