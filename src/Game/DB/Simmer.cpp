#include "Game/DB/Simmer.h"

#include "Game/DB/StatsTracker.h"
#include "NL/nlMath.h"

extern void* lbl_806E0F90;
extern "C" void fn_8010BCB8(void*, bool, int);

/**
 * Offset/Address/Size: 0x0 | 0x80109E30 | size: 0x4
 */
Simulator::Simulator()
{
}

/**
 * Offset/Address/Size: 0x4 | 0x80109E34 | size: 0x5C0
 */
void Simulator::fn_80109E34()
{
    int goals[2] = { 0, 0 };
    int fouls[2] = { 0, 0 };
    int incompletePasses[2] = { 0, 0 };

    int possession = (int)nlRandom(15, &nlDefaultSeed) + 10;
    possession += (int)nlRandom(100 - possession * 2, &nlDefaultSeed);
    StatsTracker::Track(STATS_16, 0, 0, possession, 0, 0, 0);
    StatsTracker::Track(STATS_16, 1, 0, 100 - possession, 0, 0, 0);

    for (int team = 0; team < 2; team++)
    {
        fouls[team] = (int)nlRandom(15, &nlDefaultSeed);
        for (int i = 0; i < fouls[team]; i++)
        {
            int player = (int)nlRandom(5, &nlDefaultSeed);
            if (player == 5)
            {
                player = 0;
            }
            StatsTracker::Instance()->TrackStat(
                STATS_FOULS, team, player, 0, 0, 0, 0);
        }

        int passes = (int)nlRandom(15, &nlDefaultSeed);
        int completedPasses = (int)nlRandom(15, &nlDefaultSeed);
        if (completedPasses > passes)
        {
            completedPasses = passes;
        }
        incompletePasses[team] = passes - completedPasses;

        for (int i = 0; i < passes; i++)
        {
            int passer = (int)nlRandom(5, &nlDefaultSeed);
            if (passer == 5)
            {
                passer = 0;
            }
            StatsTracker::Instance()->TrackStat(
                STATS_PASSES_MADE, team, passer, 0, 0, 0, 0);

            if (completedPasses > 0)
            {
                int receiver;
                do
                {
                    receiver = (int)nlRandom(5, &nlDefaultSeed);
                    if (receiver == 5)
                    {
                        receiver = 0;
                    }
                } while (receiver == passer);

                StatsTracker::Instance()->TrackStat(
                    STATS_PASSES_RECEIVED, team, receiver, 0, 0, 0, 0);
                completedPasses--;
            }
        }
    }

    for (int team = 0; team < 2; team++)
    {
        int shotAttempts = (int)nlRandom(15, &nlDefaultSeed);
        int maximumSpecialGoals = (int)nlRandom(5, &nlDefaultSeed);
        int shotsOnGoal = (int)nlRandom(15, &nlDefaultSeed);
        int maximumGoals = (int)nlRandom(5, &nlDefaultSeed);

        if (shotAttempts > 0 && shotAttempts < 3)
        {
            shotAttempts = 3;
        }

        int specialGoals = maximumSpecialGoals;
        if (specialGoals > shotAttempts)
        {
            specialGoals = shotAttempts;
        }

        int goalsRemaining = maximumGoals;
        if (goalsRemaining > shotsOnGoal)
        {
            goalsRemaining = shotsOnGoal;
        }

        goals[team] += goalsRemaining + specialGoals;

        int assistedGoals = (int)nlRandom(5, &nlDefaultSeed);
        if (assistedGoals > shotsOnGoal)
        {
            assistedGoals = shotsOnGoal;
        }

        for (int i = 0; i < shotsOnGoal; i++)
        {
            int player = (int)nlRandom(5, &nlDefaultSeed);
            if (player == 5)
            {
                player = 0;
            }

            StatsTracker::Instance()->TrackStat(
                STATS_SHOTS_ON_GOAL, team, player, 1, 0, 0, 0);

            if (assistedGoals > 0)
            {
                player = (int)nlRandom(3, &nlDefaultSeed) + 1;
                StatsTracker::Instance()->TrackStat(
                    STATS_04, team, player, 1, 0, 0, 0);
                assistedGoals--;
            }

            if (goalsRemaining > 0)
            {
                StatsTracker::Instance()->TrackStat(
                    STATS_GOALS_FOR, team, player, -1, 0, 1, 0);
                goalsRemaining--;
            }
        }

        StatsTracker::Instance()->TrackStat(
            STATS_09, team, 0, shotAttempts, 0, 0, 0);
        StatsTracker::Instance()->TrackStat(
            STATS_SHOTS_ON_GOAL, team, 0, shotAttempts, 0, 0, 0);

        if (specialGoals > 0)
        {
            StatsTracker::Instance()->TrackStat(
                STATS_0A, team, 0, specialGoals, 0, 0, 0);
            StatsTracker::Instance()->TrackStat(
                STATS_GOALS_FOR, team, 0, -1, 0, specialGoals, 0);
        }

        int attackSuccesses = (int)nlRandom(15, &nlDefaultSeed);
        for (int i = 0; i < attackSuccesses; i++)
        {
            int player = (int)nlRandom(5, &nlDefaultSeed);
            if (player == 5)
            {
                player = 0;
            }
            StatsTracker::Instance()->TrackStat(
                STATS_ATTACK_SUCCESSES, team, player, 0, 0, 0, 0);
        }

        int hitsMade = (int)nlRandom(15, &nlDefaultSeed);
        for (int i = 0; i < hitsMade; i++)
        {
            int player = (int)nlRandom(5, &nlDefaultSeed);
            if (player == 5)
            {
                player = 0;
            }
            StatsTracker::Instance()->TrackStat(
                STATS_12, team, player, 0, 0, 0, 0);
        }
    }

    bool overtime = false;
    int winningSide;
    if (goals[0] == goals[1])
    {
        winningSide = (int)nlRandom(2, &nlDefaultSeed);
        int player = (int)nlRandom(5, &nlDefaultSeed);
        if (player == 5)
        {
            player = 0;
        }
        StatsTracker::Instance()->TrackStat(
            STATS_GOALS_FOR, winningSide, player, -1, 0, 1, 0);
        goals[winningSide]++;
        StatsTracker::Instance()->TrackStat(
            STATS_LOSS, winningSide, 0, goals[0], goals[1], 0, 0);
        overtime = true;
    }
    else
    {
        winningSide = goals[0] < goals[1];
        StatsTracker::Instance()->TrackStat(
            STATS_WIN, winningSide, 0, goals[0], goals[1], 0, 0);
    }

    fn_8010BCB8(lbl_806E0F90, overtime, winningSide);
}
