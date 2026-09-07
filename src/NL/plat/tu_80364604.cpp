#include <revolution/kpad/KPAD.h>

#include "NL/plat/tu_80364604.h"

extern "C" void fn_80364604(UnidentifiedPointerData* data)
{
    data->mCount = 0;
    nlVec2Set(data->mUnidentified004, 0.0f, 0.0f);
    data->mAngle = 0;
    nlVec2Set(data->mUnidentified010, 0.0f, 0.0f);
    nlVec2Set(data->mPosition, 0.0f, 0.0f);
}

extern "C" int fn_80364630(UnidentifiedPointerData* data, nlVector2* position, unsigned short* angle)
{
    *position = data->mPosition;
    *angle = data->mAngle;
    return data->mCount;
}

extern "C" void fn_80364650(UnidentifiedPointerData* data, KPADStatus* status)
{
    data->mCount = status->dpd_valid_fg;
    data->mUnidentified004.x = -status->pos.x;
    data->mUnidentified004.y = -status->pos.y;
    float horizonX = status->horizon.x;
    float horizonY = status->horizon.y;
    data->mAngle
        = (unsigned short)(int)(10430.378f * nlATan2f(horizonY, horizonX));
    data->mUnidentified010.x = status->vec.x;
    data->mUnidentified010.y = status->vec.y;
    data->mPosition = data->mUnidentified004;
}
