#include "Game/NetworkRandom.h"

#include "NL/nlTicker.h"

static const char sNetworkNameConsonants[] = "bcdfghjklmnprstvwyz";
static const char sNetworkNameVowels[] = "aeiou";

u32 NetworkRandom()
{
    static u32 sNetworkRandomSeed;

    if (sNetworkRandomSeed == 0)
    {
        sNetworkRandomSeed = nlGetTicker() % 2147483647U;
    }
    sNetworkRandomSeed = 48271U * (sNetworkRandomSeed % 44488U)
                 - 3399U * (sNetworkRandomSeed / 44488U);
    if (sNetworkRandomSeed == 0)
    {
        sNetworkRandomSeed += 2147483647U;
    }
    return sNetworkRandomSeed;
}

void GenerateNetworkName(char* buffer, int size)
{
    int length = NetworkRandom() % size;
    if (length < 2)
    {
        length = 2;
    }

    bool consonant = true;
    int i;
    for (i = 0; i < length; ++i)
    {
        if (consonant)
        {
            buffer[i] = sNetworkNameConsonants[NetworkRandom() % 19];
        }
        else
        {
            buffer[i] = sNetworkNameVowels[NetworkRandom() % 5];
        }
        consonant = !consonant;
    }
    buffer[i] = '\0';
}
