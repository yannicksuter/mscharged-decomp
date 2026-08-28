#include "Game/AI/AiUtil.h"
#include "Game/GameTweaks.h"
#include "NL/nlPrint.h"

struct UnclassifiedTerrainName
{
    char mName[0x40];
    int mIndex;
};

struct unk_800A9274
{
    u8 mUnidentified000[0x10];
    float* mValue10;
    u8 mUnidentified014[0x0C];
    float* mValue20;
    u8 mUnidentified024[0x0C];
    float* mValue30;
    u8 mUnidentified034[0x0C];
    float* mValue40;
};

extern "C" const char* lbl_80500BE8[6];
extern "C" const char lbl_80500C00[];

extern "C" int fn_800A9154(int index, char* buffer, unsigned long size)
{
    return nlSNPrintf(buffer, size, lbl_80500C00, lbl_80500BE8[index]);
}

extern "C" UnclassifiedTerrainName* fn_800A917C(UnclassifiedTerrainName* terrain, int index)
{
    terrain->mIndex = index;
    nlSNPrintf(terrain->mName, 0x3F, lbl_80500C00, lbl_80500BE8[index]);
    return terrain;
}

extern "C" float fn_800A9274()
{
    return *lbl_8056CF08.mUnidentified04->mValue10;
}

extern "C" float fn_800A928C()
{
    return *lbl_8056CF08.mUnidentified04->mValue20;
}

extern "C" float fn_800A92A4(float value)
{
    return Interpolate(0.25f, 0.008f * value, *lbl_8056CF08.mUnidentified04->mValue30);
}

extern "C" float fn_800A92C8(float value)
{
    return Interpolate(0.07f, 2.0f * value, *lbl_8056CF08.mUnidentified04->mValue40);
}
