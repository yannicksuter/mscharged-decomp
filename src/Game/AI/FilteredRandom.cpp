#include "Game/AI/FilteredRandom.h"

#include "NL/nlMath.h"

const static u8 InitialHistoryChance[20] = {
    0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0
};

const static u8 InitialHistoryRange[10] = {
    3, 8, 0, 6, 7, 9, 7, 0, 3, 5
};

int FilteredRandomRange::genrand(int range)
{
    return nlRandom(range);
}

FilteredRandomRange::FilteredRandomRange()
{
    m_repeatingRunLength = 1;
    for (int i = 0; i < 10; i++)
    {
        m_hist[i] = InitialHistoryRange[i];
    }
}

bool FilteredRandomChance::genrand(float chance)
{
    float random = nlRandomf(1.0f);
    if (random <= chance)
    {
        return true;
    }
    return false;
}

FilteredRandomChance::FilteredRandomChance()
{
    for (int i = 0; i < 10; i++)
    {
        m_hist[i] = InitialHistoryChance[i];
    }
    for (int i = 0; i < 10; i++)
    {
        m_hist[i + 10] = InitialHistoryChance[i + 10];
    }
}
