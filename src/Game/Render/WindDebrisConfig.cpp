#include "Game/Render/WindDebrisConfig.h"

static WindDebrisConfig sWindDebrisConfigs[] = {
    { WIND_DEBRIS_COW, "CowDebris", 1.6f, 0x5F020C40, 0xFD6836E3 },
    { WIND_DEBRIS_CATFISH, "CatfishDebris", 1.17f, 0x6E3ED339, 0x5B31D25C },
    { WIND_DEBRIS_TRACTOR, "TractorDebris", 2.75f, 0x1E148196, 0x40C16739 },
};

WindDebrisConfig* GetWindDebrisConfig(const int& index)
{
    if (index > -1 && index < 3)
    {
        return &sWindDebrisConfigs[index];
    }
    return 0;
}
