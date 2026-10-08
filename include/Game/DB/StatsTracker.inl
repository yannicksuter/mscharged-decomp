#include "Game/DB/StatsTracker.h"

void StatsTracker::Track(ePlayerStats stat, int homeaway, int playerindex,
    int param0, int param1, int param2, int param3)
{
    s_pInstance->TrackStat(
        stat, homeaway, playerindex, param0, param1, param2, param3);
}
