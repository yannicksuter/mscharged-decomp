#include "Game/Render/tu_801B532C.h"

static UnidentifiedNPCConfig_801B532C lbl_8051485C[] = {
    { 0, "CowDebris", 1.6f, 0x5F020C40, 0xFD6836E3 },
    { 1, "CatfishDebris", 1.17f, 0x6E3ED339, 0x5B31D25C },
    { 2, "TractorDebris", 2.75f, 0x1E148196, 0x40C16739 },
};

UnidentifiedNPCConfig_801B532C* fn_801B532C(const int& index)
{
    if (index > -1 && index < 3)
    {
        return &lbl_8051485C[index];
    }
    return 0;
}
