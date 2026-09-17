#include "Game/DB/Simmer.h"

#include "Game/DB/GameProgress.h"
#include "Game/DB/StatsTracker.h"
#include "NL/nlMath.h"

static int RandomRange(int minimum, int maximum)
{
    int value = (int)nlRandom(maximum - minimum, &nlDefaultSeed);
    return minimum + value;
}

static int GetRandomPlayerIndex()
{
    unsigned int player = nlRandom(5, &nlDefaultSeed);
    if (player == 5)
    {
        player = 0;
    }
    return player;
}

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

    int possession = RandomRange(0, 15) + 10;
    possession += RandomRange(0, 100 - possession * 2);
    StatsTracker::Track(STATS_16, 0, 0, possession, 0, 0, 0);
    StatsTracker::Track(STATS_16, 1, 0, 100 - possession, 0, 0, 0);

    for (int team = 0; team < 2; team++)
    {
        fouls[team] = RandomRange(0, 15);
        for (int i = 0; i < fouls[team]; i++)
        {
            int player = GetRandomPlayerIndex();
            StatsTracker::Instance()->TrackStat(
                STATS_FOULS, team, player, 0, 0, 0, 0);
        }

        int passes = RandomRange(0, 15);
        int completedPasses = RandomRange(0, 15);
        completedPasses = nlMin(completedPasses, passes);
        incompletePasses[team] = passes - completedPasses;

        for (int i = 0; i < passes; i++)
        {
            int passer = GetRandomPlayerIndex();
            StatsTracker::Instance()->TrackStat(
                STATS_PASSES_MADE, team, passer, 0, 0, 0, 0);

            if (completedPasses > 0)
            {
                int receiver;
                do
                {
                    receiver = GetRandomPlayerIndex();
                } while (receiver == passer);

                StatsTracker::Instance()->TrackStat(
                    STATS_PASSES_RECEIVED, team, receiver, 0, 0, 0, 0);
                completedPasses--;
            }
        }
    }

    for (int team = 0; team < 2; team++)
    {
        int shotAttempts = RandomRange(0, 15);
        int maximumSpecialGoals = RandomRange(0, 5);
        int shotsOnGoal = RandomRange(0, 15);
        int maximumGoals = RandomRange(0, 5);

        shotAttempts = (shotAttempts > 0 && shotAttempts < 3) ? 3 : shotAttempts;

        int specialGoals = nlMin(maximumSpecialGoals, shotAttempts);
        int goalsRemaining = nlMin(maximumGoals, shotsOnGoal);

        goals[team] += goalsRemaining;
        goals[team] += specialGoals;

        int assistedGoals = RandomRange(0, 5);
        assistedGoals = nlMin(assistedGoals, shotsOnGoal);

        for (int i = 0; i < shotsOnGoal; i++)
        {
            int player = GetRandomPlayerIndex();

            StatsTracker::Instance()->TrackStat(
                STATS_SHOTS_ON_GOAL, team, player, 1, 0, 0, 0);

            if (assistedGoals > 0)
            {
                player = RandomRange(1, 4);
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

        int attackSuccesses = RandomRange(0, 15);
        for (int i = 0; i < attackSuccesses; i++)
        {
            int player = GetRandomPlayerIndex();
            StatsTracker::Instance()->TrackStat(
                STATS_ATTACK_SUCCESSES, team, player, 0, 0, 0, 0);
        }

        int hitsMade = RandomRange(0, 15);
        for (int i = 0; i < hitsMade; i++)
        {
            int player = GetRandomPlayerIndex();
            StatsTracker::Instance()->TrackStat(
                STATS_12, team, player, 0, 0, 0, 0);
        }
    }

    bool overtime = false;
    int winningSide;
    if (goals[0] == goals[1])
    {
        winningSide = RandomRange(0, 2);
        int player = GetRandomPlayerIndex();
        StatsTracker::Instance()->TrackStat(
            STATS_GOALS_FOR, winningSide, player, -1, 0, 1, 0);
        goals[winningSide]++;
        StatsTracker::Instance()->TrackStat(
            STATS_OT_WIN, winningSide, 0, goals[0], goals[1], 0, 0);
        overtime = true;
    }
    else
    {
        winningSide = goals[0] <= goals[1];
        StatsTracker::Instance()->TrackStat(
            STATS_WIN, winningSide, 0, goals[0], goals[1], 0, 0);
    }

    g_pCupManager->fn_8010BCB8(overtime, winningSide);
}
