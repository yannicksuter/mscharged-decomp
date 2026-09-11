#include "NL/nlPrint.h"
#include "types.h"

class TweakValueBase;

struct UnclassifiedTerrainProperty
{
    u8 mUnidentified000[0x0C];
    int mValue;
};

extern "C" int fn_800A964C(UnclassifiedTerrainProperty*)
{
    return 4;
}

extern "C" int fn_800A9654(UnclassifiedTerrainProperty*)
{
    return 1;
}

extern "C" void* fn_800A965C(UnclassifiedTerrainProperty*)
{
    return 0;
}

extern "C" int* fn_800A9664(UnclassifiedTerrainProperty* property)
{
    return &property->mValue;
}

extern "C" int fn_800A9784(UnclassifiedTerrainProperty* property, char* buffer, unsigned long size)
{
    return nlSNPrintf(buffer, size, "%d", property->mValue);
}

extern "C" void fn_800A97A0(UnclassifiedTerrainProperty*, const char*)
{
}

extern "C" void fn_800A97A4(UnclassifiedTerrainProperty*, float* first, float* second, float* third)
{
    *first = 0.0f;
    *second = 0.0f;
    *third = 0.0f;
}

extern "C" void fn_800A97B8(UnclassifiedTerrainProperty*)
{
}

extern "C" void fn_800A97BC(UnclassifiedTerrainProperty*, TweakValueBase*)
{
}

extern "C" int fn_800A97C0(UnclassifiedTerrainProperty*)
{
    return 0;
}

extern "C" int fn_800A97C8(UnclassifiedTerrainProperty*)
{
    return 0;
}

extern "C" void* fn_800A97D0(UnclassifiedTerrainProperty*)
{
    return 0;
}

extern "C" void fn_800A97D8(UnclassifiedTerrainProperty*, char* buffer, unsigned long)
{
    buffer[0] = '\0';
}

extern "C" void fn_800A97E4(UnclassifiedTerrainProperty*, const char*)
{
}
