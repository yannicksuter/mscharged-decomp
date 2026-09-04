#include "Game/AI/AiUtil.h"
#include "Game/GameTweaks.h"
#include "NL/nlPrint.h"

struct UnclassifiedTerrainName
{
    char mName[0x40];
    int mIndex;
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
    return lbl_8056CF08.mUnidentified04->mUnidentified04;
}

extern "C" float fn_800A928C()
{
    return lbl_8056CF08.mUnidentified04->mUnidentified14;
}

extern "C" float fn_800A92A4(float value)
{
    return Interpolate(0.25f, 0.008f * value, lbl_8056CF08.mUnidentified04->mUnidentified24);
}

extern "C" float fn_800A92C8(float value)
{
    return Interpolate(0.07f, 2.0f * value, lbl_8056CF08.mUnidentified04->mUnidentified34);
}
