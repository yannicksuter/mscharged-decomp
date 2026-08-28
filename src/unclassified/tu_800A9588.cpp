#include "NL/nlPrint.h"
#include "types.h"

struct UnclassifiedTerrainProperty
{
    u8 mUnidentified000[0x0C];
    int mValue;
};

extern "C" int* fn_800A9664(UnclassifiedTerrainProperty* property)
{
    return &property->mValue;
}

extern "C" int fn_800A9784(UnclassifiedTerrainProperty* property, char* buffer, unsigned long size)
{
    return nlSNPrintf(buffer, size, "%d", property->mValue);
}

extern "C" void fn_800A97A4(UnclassifiedTerrainProperty*, float* first, float* second, float* third)
{
    *first = 0.0f;
    *second = 0.0f;
    *third = 0.0f;
}
