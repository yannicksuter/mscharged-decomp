#ifndef UNCLASSIFIED_TU_800A9B78_H
#define UNCLASSIFIED_TU_800A9B78_H

#include "NL/nlDLListContainer.h"

class DebugWriteCache;
struct UnidentifiedWeatherState;

class TU800A9B78
{
public:
    TU800A9B78();
    ~TU800A9B78();
    void fn_800AA29C();
    UnidentifiedWeatherState* fn_800AA060(int value);
    void fn_800AA0FC(void* context, DebugWriteCache* cache);
    void fn_800AA1D4(float value);
    void fn_800AA3E8(bool value);
    void fn_800AA4B0();
    void fn_800AA568();
    void fn_800AA5F8();

    nlDLListSlotPool<UnidentifiedWeatherState*> field00;
};

#endif // UNCLASSIFIED_TU_800A9B78_H
