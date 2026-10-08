#ifndef GAME_DB_SIMMER_H
#define GAME_DB_SIMMER_H

struct StatsPair
{
    /* 0x00 */ float mMean;
    /* 0x04 */ float mStandardDeviation;
};

class Simulator
{
public:
    Simulator();
    void SimulateGame();

    /* 0x000 */ StatsPair mStatistics[39];
};

#endif // GAME_DB_SIMMER_H
